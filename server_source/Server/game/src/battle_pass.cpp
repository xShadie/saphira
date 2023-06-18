#include "stdafx.h"
#include "battle_pass.h"

#include "p2p.h"
#include "locale_service.h"
#include "char.h"
#include "desc_client.h"
#include "desc_manager.h"
#include "buffer_manager.h"
#include "packet.h"
#include "questmanager.h"
#include "questlua.h"
#include "start_position.h"
#include "char_manager.h"
#include "item_manager.h"
#include "sectree_manager.h"
#include "regen.h"
#include "log.h"
#include "db.h"
#include "target.h"
#include "party.h"

#include <string>
#include <algorithm>

const std::string g_astMissionType[MISSION_TYPE_MAX] = {
	"",
	"MONSTER_KILL",
	"PLAYER_KILL",
	"MONSTER_DAMAGE",
	"PLAYER_DAMAGE",
	"USE_ITEM",
	"SELL_ITEM",
	"CRAFT_ITEM",
	"REFINE_ITEM",
	"DESTROY_ITEM",
	"COLLECT_ITEM",
	"FRY_FISH",
	"CATCH_FISH",
	"SPENT_YANG",
	"FARM_YANG",
	"COMPLETE_DUNGEON",
	"COMPLETE_MINIGAME",
#ifdef ENABLE_BATTLE_PASS_STAY_ONLINE
	"STAY_ONLINE_MINUTES",
#endif
#ifdef ENABLE_BATTLE_PASS_CHAT_CNT
	"COUNTER_CHAT",
#endif
};

CBattlePass::CBattlePass()
{
	m_pLoader = NULL;
}

CBattlePass::~CBattlePass()
{
	if (m_pLoader)
		delete m_pLoader;
}

bool CBattlePass::ReadBattlePassFile()
{
	char szBattlePassFileName[256];
	snprintf(szBattlePassFileName, sizeof(szBattlePassFileName),"%s/battle_pass.txt", LocaleService_GetBasePath().c_str());
			
	m_pLoader = new CGroupTextParseTreeLoader;
	CGroupTextParseTreeLoader& loader = *m_pLoader;

	if (false == loader.Load(szBattlePassFileName))
	{
		sys_err("battle_pass.txt load error");
		return false;
	}

	if (!ReadBattlePassGroup())
		return false;
	
	if (!ReadBattlePassMissions())
		return false;
	
	return true;
}

bool CBattlePass::ReadBattlePassGroup()
{
	std::string stName;

	CGroupNode* pGroupNode = m_pLoader->GetGroup("battlepass");

	if (NULL == pGroupNode)
	{
		sys_err("battle_pass.txt need BattlePass group.");
		return false;
	}

	int n = pGroupNode->GetRowCount();
	if (0 == n)
	{
		sys_err("Group BattlePass is Empty.");
		return false;
	}

	std::set<BYTE> setIDs;

	for (int i = 0; i < n; i++)
	{
		const CGroupNode::CGroupNodeRow* pRow;
		pGroupNode->GetRow(i, &pRow);

		std::string stBattlePassName;
		BYTE battlePassId;
		
		if (!pRow->GetValue("battlepassname", stBattlePassName))
		{
			sys_err ("In Group BattlePass, No BattlePassName column.");
			return false;
		}
		
		if (!pRow->GetValue("battlepassid", battlePassId))
		{
			sys_err ("In Group BattlePass, %s's ID is invalid", stBattlePassName.c_str());
			return false;
		}

		if (setIDs.end() != setIDs.find(battlePassId))
		{
			sys_err ("In Group BattlePass, duplicated id exist.");
			return false;
		}
		
		setIDs.insert(battlePassId);

		m_map_battle_pass_name.insert(TMapBattlePassName::value_type(battlePassId, stBattlePassName));
	}
	
	return true;
}

#ifdef ENABLE_BATTLE_PASS_RELOAD
bool CBattlePass::ReadBattlePassMissions(bool isReloading)
#else
bool CBattlePass::ReadBattlePassMissions()
#endif
{
#ifdef ENABLE_BATTLE_PASS_RELOAD
	if (isReloading)
	{
		m_map_battle_pass_reward.clear();
		m_map_battle_pass_mission_info.clear();
	}
#endif

	TMapBattlePassName::iterator it = m_map_battle_pass_name.begin();
	while (it != m_map_battle_pass_name.end())
	{
		std::string battlePassName = (it++)->second;
		
		CGroupNode* pGroupNode = m_pLoader->GetGroup(battlePassName.c_str());
	
		if (NULL == pGroupNode)
		{
			sys_err ("battle_pass.txt need group %s.", battlePassName.c_str());
			return false;
		}
		
		int n = pGroupNode->GetChildNodeCount();
		if (n < 2)
		{
			sys_err("Group %s need to have at least one grup for Reward and one Mission. Row: %d", battlePassName.c_str(), n);
			return false;
		}
		
		{
			CGroupNode* pChild;
			if (NULL == (pChild = pGroupNode->GetChildNode("reward")))
			{
				sys_err ("In Group %s, Reward group is not defined.", battlePassName.c_str());
				return false;
			}
			
			int m = pChild->GetRowCount();
			std::vector<TBattlePassRewardItem> rewardVector;

			for (int j = 1; j <= m; j++)
			{
				std::stringstream ss;
				ss << j;
				const CGroupNode::CGroupNodeRow* pRow = NULL;
	
				pChild->GetRow(ss.str(), &pRow);
				if (NULL == pRow)
				{
					sys_err("In Group %s, subgroup Reward, No %d row.", battlePassName.c_str(), j);
					return false;
				}
				
				TBattlePassRewardItem itemReward;

				if (!pRow->GetValue("itemvnum", itemReward.dwVnum))
				{
					sys_err("In Group %s, subgroup Reward, ItemVnum is empty.", battlePassName.c_str());
					return false;
				}
				
				if (!pRow->GetValue("itemcount", itemReward.bCount))
				{
					sys_err("In Group %s, subgroup Reward, ItemCount is empty.", battlePassName.c_str());
					return false;
				}
				
				rewardVector.push_back(itemReward);
			}
			
			m_map_battle_pass_reward.insert(TMapBattlePassReward::value_type(battlePassName.c_str(), rewardVector));
		}
		
		std::vector<TBattlePassMissionInfo> missionInfoVector;
		
		for (int i = 1; i < n; i++)
		{
			std::stringstream ss;
			ss << "mission_" << i;

			CGroupNode* pChild;
			if (NULL == (pChild = pGroupNode->GetChildNode(ss.str())))
			{
				sys_err("In Group %s, %s subgroup is not defined.", battlePassName.c_str(), ss.str().c_str());
				return false;
			}
			
			int m = pChild->GetRowCount();
			
			std::string stMissionSearch[] = {"", ""};
			bool bAlreadySearched = false;
			BYTE bRewardContor = 0;
			TBattlePassMissionInfo missionInfo;
			memset(&missionInfo, 0, sizeof(TBattlePassMissionInfo));
			
			for (int j = 0; j < m; j++)
			{
				const CGroupNode::CGroupNodeRow* pRow = NULL;
	
				pChild->GetRow(j, &pRow);
				if (NULL == pRow)
				{
					sys_err("In Group %s and subgroup %s null row.", battlePassName.c_str(), ss.str().c_str());
					return false;
				}
				
				// InfoDesc = ItemVnum from reward
				// InfoName = ItemCount from reward

				std::string stInfoDesc;
				if (!pRow->GetValue("infodesc", stInfoDesc))
				{
					sys_err("In Group %s and subgroup %s InfoDesc does not exist.", battlePassName.c_str(), ss.str().c_str());
					return false;
				}
				
				if(stInfoDesc == "type")
				{
					std::string stInfoName;
					if (!pRow->GetValue("infoname", stInfoName))
					{
						sys_err("In Group %s and subgroup %s InfoName does not exist.", battlePassName.c_str(), ss.str().c_str());
						return false;
					}
					
					missionInfo.bMissionType = GetMissionTypeByName(stInfoName);
				}
				
				if(missionInfo.bMissionType <= MISSION_TYPE_NONE || missionInfo.bMissionType >= MISSION_TYPE_MAX)
				{
					sys_err("In Group %s and subgroup %s Wrong mission type: %d.", battlePassName.c_str(), ss.str().c_str(), missionInfo.bMissionType);
					return false;
				}
				
				if(!bAlreadySearched)
				{
					GetMissionSearchName(missionInfo.bMissionType, &stMissionSearch[0], &stMissionSearch[1]);
					bAlreadySearched = true;
				}
				
				for(int k = 0; k < 2; k++)
				{
					if(stMissionSearch[k] != "")
					{
						if(stInfoDesc == stMissionSearch[k])
						{
							if (!pRow->GetValue("infoname", missionInfo.dwMissionInfo[k]))
							{
								sys_err("In Group %s and subgroup %s InfoDesc %s InfoName does not exist.", battlePassName.c_str(), ss.str().c_str(), stMissionSearch[k].c_str());
								return false;
							}
							
							sys_log(0, "BattlePassInfo: Group %s // Subgroup %s // InfoName %s // InfoValue %d", 
								battlePassName.c_str(), ss.str().c_str(), stMissionSearch[k].c_str(), missionInfo.dwMissionInfo[k]);
							
							stMissionSearch[k] = "";
						}
					}
				}
				
				if(bRewardContor >= MISSION_REWARD_COUNT)
				{
					sys_err("In Group %s and subgroup %s More than 3 rewards.", battlePassName.c_str(), ss.str().c_str());
					return false;
				}
				
				if(isdigit(*stInfoDesc.c_str()))
				{
					DWORD dwVnum = atoi(stInfoDesc.c_str());
					BYTE bCount = 1;
		
					if (!pRow->GetValue("infoname", bCount))
					{
						sys_err("In Group %s and subgroup %s Wrong ItemCount.", battlePassName.c_str(), ss.str().c_str());
						return false;
					}
							
					missionInfo.aRewardList[bRewardContor].dwVnum = dwVnum;
					missionInfo.aRewardList[bRewardContor].bCount = bCount;
					bRewardContor++;
				}
			}
			
			missionInfoVector.push_back(missionInfo);
		}
		
		m_map_battle_pass_mission_info.insert(TMapBattleMissionInfo::value_type(battlePassName.c_str(), missionInfoVector));
	}

	return true;
}

BYTE CBattlePass::GetMissionTypeByName(std::string stMissionName)
{
	for(int i = 0; i < MISSION_TYPE_MAX; i++)
	{
		if(g_astMissionType[i] == stMissionName)
			return i;
	}
	return 0;
}

std::string CBattlePass::GetMissionNameByType(BYTE bType)
{
	for(int i = 0; i < MISSION_TYPE_MAX; i++)
	{
		if(i == bType)
			return g_astMissionType[i];
	}
	
	return "";
}

std::string CBattlePass::GetBattlePassNameByID(BYTE bID)
{
	TMapBattlePassName::iterator it = m_map_battle_pass_name.find(bID);
	
	if(it == m_map_battle_pass_name.end())
	{
		return "";
	}
	
	return it->second;
}

void CBattlePass::GetMissionSearchName(BYTE bMissionType, std::string * st_name_1, std::string * st_name_2)
{
	switch(bMissionType)
	{
		case MONSTER_KILL:
		case USE_ITEM:
		case SELL_ITEM:
		case CRAFT_ITEM:
		case REFINE_ITEM:
		case DESTROY_ITEM:
		case COLLECT_ITEM:
			*st_name_1 = "vnum";
			*st_name_2 = "count";
			break;
			
		case PLAYER_KILL:
			*st_name_1 = "min_level";
			*st_name_2 = "count";
			break;
			
		case MONSTER_DAMAGE:
			*st_name_1 = "vnum";
			*st_name_2 = "value";
			break;
			
		case PLAYER_DAMAGE:
			*st_name_1 = "min_level";
			*st_name_2 = "value";
			break;
			
		case FRY_FISH:
		case CATCH_FISH:
#ifdef ENABLE_BATTLE_PASS_STAY_ONLINE
		case STAY_ONLINE_MINUTES:
#endif
#ifdef ENABLE_BATTLE_PASS_CHAT_CNT
		case COUNTER_CHAT:
#endif
			*st_name_1 = "";
			*st_name_2 = "count";
			break;
			
		case SPENT_YANG:
		case FARM_YANG:
			*st_name_1 = "";
			*st_name_2 = "value";
			break;	
			
		case COMPLETE_DUNGEON:
		case COMPLETE_MINIGAME:
			*st_name_1 = "id";
			*st_name_2 = "count";
			break;	
		
		default:
			*st_name_1 = "";
			*st_name_2 = "";
			break;
	}
}

void CBattlePass::BattlePassRequestOpen(LPCHARACTER pkChar)
{
	if(!pkChar)
		return;
	
	if(!pkChar->GetDesc())
		return;
	
	if(!pkChar->IsLoadedBattlePass())
	{
#ifdef TEXTS_IMPROVEMENT
		pkChar->ChatPacketNew(CHAT_TYPE_INFO, 776, "");
#endif
		return;
	}

	BYTE bBattlePassId = pkChar->GetBattlePassId();
	BYTE fakeBattlePassID = 1; // (can be return actual month)
	
	// So if there is no active battlepass, we can't send data info,
	// but we do a fake id to send the infos else we continue as how until now.
	if (!bBattlePassId)
		bBattlePassId = fakeBattlePassID;

	TMapBattlePassName::iterator it = m_map_battle_pass_name.find(bBattlePassId);
	
	if(it == m_map_battle_pass_name.end())
	{
#ifdef TEXTS_IMPROVEMENT
		pkChar->ChatPacketNew(CHAT_TYPE_INFO, 777, "%d", bBattlePassId);
#endif
		return;
	}
	
	std::string battlePassName = it->second;
	TMapBattleMissionInfo::iterator itInfo = m_map_battle_pass_mission_info.find(battlePassName);
	
	if(itInfo == m_map_battle_pass_mission_info.end())
	{
#ifdef TEXTS_IMPROVEMENT
		pkChar->ChatPacketNew(CHAT_TYPE_INFO, 778, "%s", battlePassName.c_str());
#endif
		return;
	}
	
	TMapBattlePassReward::iterator itReward = m_map_battle_pass_reward.find(battlePassName);
	if(itReward == m_map_battle_pass_reward.end())
	{
#ifdef TEXTS_IMPROVEMENT
		pkChar->ChatPacketNew(CHAT_TYPE_INFO, 779, "%s", battlePassName.c_str());
#endif
		return;
	}
	
	std::vector<TBattlePassRewardItem> rewardInfo = itReward->second;
	std::vector<TBattlePassMissionInfo> missionInfo = itInfo->second;
	
	for (unsigned int i = 0; i < missionInfo.size(); i++)
	{
		missionInfo[i].dwMissionInfo[2] = pkChar->GetMissionProgress(missionInfo[i].bMissionType, bBattlePassId);
	}
	
	if(!missionInfo.empty())
	{
		TPacketGCBattlePass packet;
		packet.bHeader = HEADER_GC_BATTLE_PASS_OPEN;
		packet.wSize = sizeof(packet) + sizeof(TBattlePassMissionInfo) * missionInfo.size();
		packet.wRewardSize = sizeof(TBattlePassRewardItem) * rewardInfo.size();

		pkChar->GetDesc()->BufferedPacket(&packet, sizeof(packet));
		pkChar->GetDesc()->BufferedPacket(&missionInfo[0], sizeof(TBattlePassMissionInfo) * missionInfo.size());
		pkChar->GetDesc()->Packet(&rewardInfo[0], sizeof(TBattlePassRewardItem) * rewardInfo.size());
	}
}

void CBattlePass::BattlePassRewardMission(LPCHARACTER pkChar, DWORD bMissionType, DWORD bBattlePassId)
{
	if(!pkChar)
		return;
	
	if(!pkChar->GetDesc())
		return;

	if (!bBattlePassId)
	{
#ifdef TEXTS_IMPROVEMENT
		pkChar->ChatPacketNew(CHAT_TYPE_INFO, 780, "");
#endif
		return;
	}
	
	TMapBattlePassName::iterator it = m_map_battle_pass_name.find(bBattlePassId);
	
	if(it == m_map_battle_pass_name.end())
	{
#ifdef TEXTS_IMPROVEMENT
		pkChar->ChatPacketNew(CHAT_TYPE_INFO, 777, "%d", bBattlePassId);
#endif
		return;
	}
	
	std::string battlePassName = it->second;
	TMapBattleMissionInfo::iterator itInfo = m_map_battle_pass_mission_info.find(battlePassName);
	
	if(itInfo == m_map_battle_pass_mission_info.end())
	{
#ifdef TEXTS_IMPROVEMENT
		pkChar->ChatPacketNew(CHAT_TYPE_INFO, 778, "%d", battlePassName.c_str());
#endif
		return;
	}
	
	std::vector<TBattlePassMissionInfo> missionInfo = itInfo->second;
	
	for (unsigned int i = 0; i < missionInfo.size(); i++)
	{
		if(missionInfo[i].bMissionType == bMissionType)
		{
			for(int j = 0; j < MISSION_REWARD_COUNT; j++)
			{
				if(missionInfo[i].aRewardList[j].dwVnum && missionInfo[i].aRewardList[j].bCount > 0)
					pkChar->AutoGiveItem(missionInfo[i].aRewardList[j].dwVnum, missionInfo[i].aRewardList[j].bCount);
			}
			
			break;
		}
	}
}

void CBattlePass::BattlePassRequestReward(LPCHARACTER pkChar)
{
	if(!pkChar)
		return;
	
	if(!pkChar->GetDesc())
		return;
	
	BYTE bBattlePassId = pkChar->GetBattlePassId();
	if (!bBattlePassId)
		return;
	
	TMapBattlePassName::iterator it = m_map_battle_pass_name.find(bBattlePassId);
	if(it == m_map_battle_pass_name.end())
		return;
	
	std::string battlePassName = it->second;
	TMapBattleMissionInfo::iterator itInfo = m_map_battle_pass_mission_info.find(battlePassName);
	
	if(itInfo == m_map_battle_pass_mission_info.end())
		return;
	
	std::vector<TBattlePassMissionInfo> missionInfo = itInfo->second;
	
	bool bIsCompleted = true;
	for (unsigned int i = 0; i < missionInfo.size(); i++)
	{
		if(!pkChar->IsCompletedMission(missionInfo[i].bMissionType))
		{
			bIsCompleted = false;
			break;
		}
	}
	
	if(bIsCompleted)
	{
#ifdef TEXTS_IMPROVEMENT
		BroadcastNoticeNew(CHAT_TYPE_NOTICE, 0, 0, 548, "%s", pkChar->GetName(), battlePassName.c_str());
#endif
		BattlePassReward(pkChar);
	}
}

void CBattlePass::BattlePassReward(LPCHARACTER pkChar)
{
	if(!pkChar)
		return;
	
	if(!pkChar->GetDesc())
		return;

	BYTE bBattlePassId = pkChar->GetBattlePassId();
	if (!bBattlePassId)
	{
#ifdef TEXTS_IMPROVEMENT
		pkChar->ChatPacketNew(CHAT_TYPE_INFO, 780, "");
#endif
		return;
	}
	
	TMapBattlePassName::iterator it = m_map_battle_pass_name.find(bBattlePassId);
	
	if(it == m_map_battle_pass_name.end())
	{
#ifdef TEXTS_IMPROVEMENT
		pkChar->ChatPacketNew(CHAT_TYPE_INFO, 777, "%d", bBattlePassId);
#endif
		return;
	}
	
	std::string battlePassName = it->second;
	TMapBattlePassReward::iterator itReward = m_map_battle_pass_reward.find(battlePassName);
	if(itReward == m_map_battle_pass_reward.end())
	{
#ifdef TEXTS_IMPROVEMENT
		pkChar->ChatPacketNew(CHAT_TYPE_INFO, 778, "%d", battlePassName.c_str());
#endif
		return;
	}
	
	pkChar->RemoveAffect(AFFECT_BATTLE_PASS);
	
	std::vector<TBattlePassRewardItem> rewardInfo = itReward->second;	
	
	for (unsigned int i = 0; i < rewardInfo.size(); i++)
	{
		pkChar->AutoGiveItem(rewardInfo[i].dwVnum, rewardInfo[i].bCount);
	}
	
	TBattlePassRegisterRanking ranking;
	ranking.bBattlePassId = bBattlePassId;
	strlcpy(ranking.playerName, pkChar->GetName(), sizeof(ranking.playerName));
	db_clientdesc->DBPacket(HEADER_GD_REGISTER_BP_RANKING, 0, &ranking, sizeof(TBattlePassRegisterRanking));
}

bool CBattlePass::BattlePassMissionGetInfo(BYTE bBattlePassId, BYTE bMissionType, DWORD* dwFirstInfo, DWORD* dwSecondInfo)
{
	TMapBattlePassName::iterator it = m_map_battle_pass_name.find(bBattlePassId);
	if(it == m_map_battle_pass_name.end())
		return false;

	std::string battlePassName = it->second;
	TMapBattleMissionInfo::iterator itInfo = m_map_battle_pass_mission_info.find(battlePassName);
	
	if(itInfo == m_map_battle_pass_mission_info.end())
		return false;

	std::vector<TBattlePassMissionInfo> missionInfo = itInfo->second;
	
	for (unsigned int i = 0; i < missionInfo.size(); i++)
	{
		if(missionInfo[i].bMissionType == bMissionType)
		{
			*dwFirstInfo = missionInfo[i].dwMissionInfo[0];
			*dwSecondInfo = missionInfo[i].dwMissionInfo[1];
			return true;
		}
	}

	return false;
}

#ifdef ENABLE_BATTLE_PASS_SECURITY_KILL
bool CBattlePass::IsEligibleForPlayerKill(DWORD dwKillerID, DWORD dwPlayerID)
{
	TKillMap::iterator it = m_playersKills.find(dwKillerID);
	if(it == m_playersKills.end()) // no instance for pid so can register
		return true;
	
	std::vector<TBattlePassKillVictim *> &victimVector = it->second;
	
	itertype(victimVector) itV = victimVector.begin();
	while (itV != victimVector.end())
	{
		TBattlePassKillVictim * tempVictim = *itV;
	
		if(tempVictim->dwVictimPid == dwPlayerID)
		{
			if (get_dword_time() < tempVictim->dwLastKillTime + 900 * 1000) // 900 seconds = 15 minutes
			{
				++itV;
				return false;
			}
			else
			{
				itV = victimVector.erase(itV);
				return true;
			}
		}
		
		++itV;
	}
	
	return true;
}

void CBattlePass::RegisterPlayerKill(DWORD dwKillerID, DWORD dwPlayerID)
{
	TKillMap::iterator it = m_playersKills.find(dwKillerID);
	if(it == m_playersKills.end())
	{
		std::vector<TBattlePassKillVictim *> victimVector;
		
		TBattlePassKillVictim * tempVictim = new TBattlePassKillVictim;
		tempVictim->dwVictimPid = dwPlayerID;
		tempVictim->dwLastKillTime = get_dword_time();
		
		victimVector.push_back(tempVictim);
		
		m_playersKills.insert(std::make_pair(dwKillerID, victimVector));
	}
	else
	{
		std::vector<TBattlePassKillVictim *> &victimVector = it->second;
		
		TBattlePassKillVictim * tempVictim = new TBattlePassKillVictim;
		tempVictim->dwVictimPid = dwPlayerID;
		tempVictim->dwLastKillTime = get_dword_time();
		
		victimVector.push_back(tempVictim);
	}
}
#endif
