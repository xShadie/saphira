
#define _cube_cpp_

#include "stdafx.h"
#include "constants.h"
#include "utils.h"
#include "log.h"
#include "char.h"
#include "dev_log.h"
#include "locale_service.h"
#include "item.h"
#include "item_manager.h"
#include "questmanager.h"
#include <sstream>
#include "packet.h"
#include "desc_client.h"
#include "battle_pass.h"
#include "config.h"

#ifdef __ENABLE_NEW_OFFLINESHOP__
#include "new_offlineshop.h"
#include "new_offlineshop_manager.h"
#endif
#ifdef ENABLE_STOLE_COSTUME
#include "../../common/stole_length.h"
#endif

static std::vector<CUBE_RENEWAL_DATA*>	s_cube_proto;

typedef std::vector<CUBE_RENEWAL_VALUE>	TCubeValueVector;

struct SCubeMaterialInfo
{
	SCubeMaterialInfo()
	{
		bHaveComplicateMaterial = false;
	};

	CUBE_RENEWAL_VALUE			reward;							// º¸»ó?? ¹¹³?
	TCubeValueVector	material;						// ?ç·áµé?º ¹¹³?
	DWORD				gold;							// µ·?º ¾ó¸¶µå³?
	int 				percent;
	std::string		category;
	TCubeValueVector	complicateMaterial;				// º¹?â??-_- ?ç·áµé
#ifdef ENABLE_GAYA_SYSTEM
	DWORD 				gaya;
#endif

#ifdef ENABLE_CUBE_RENEWAL_COPY_WORLDARD
	DWORD 				allowCopy;
#endif

	std::string			infoText;		
	bool				bHaveComplicateMaterial;		//
};

struct SItemNameAndLevel
{
	SItemNameAndLevel() { level = 0; }

	std::string		name;
	int				level;
};


typedef std::vector<SCubeMaterialInfo>								TCubeResultList;
typedef boost::unordered_map<DWORD, TCubeResultList>				TCubeMapByNPC;				// °¢°¢?? NPCº°·? ¾î¶² °? ¸¸µé ¼ö ??°í ?ç·á°¡ ¹º?ö...

TCubeMapByNPC cube_info_map;


static bool FN_check_valid_npc( WORD vnum )
{
	for ( std::vector<CUBE_RENEWAL_DATA*>::iterator iter = s_cube_proto.begin(); iter != s_cube_proto.end(); iter++ )
	{
		if ( std::find((*iter)->npc_vnum.begin(), (*iter)->npc_vnum.end(), vnum) != (*iter)->npc_vnum.end() )
			return true;
	}

	return false;
}


static bool FN_check_cube_data (CUBE_RENEWAL_DATA *cube_data)
{
	DWORD	i = 0;
	DWORD	end_index = 0;

	end_index = cube_data->npc_vnum.size();
	for (i=0; i<end_index; ++i)
	{
		if ( cube_data->npc_vnum[i] == 0 )	return false;
	}

	end_index = cube_data->item.size();
	for (i=0; i<end_index; ++i)
	{
		if ( cube_data->item[i].vnum == 0 )		return false;
		if ( cube_data->item[i].count == 0 )	return false;
	}

	end_index = cube_data->reward.size();
	for (i=0; i<end_index; ++i)
	{
		if ( cube_data->reward[i].vnum == 0 )	return false;
		if ( cube_data->reward[i].count == 0 )	return false;
	}
	return true;
}

static int FN_check_cube_item_vnum_material(const SCubeMaterialInfo& materialInfo, int index)
{
	if ((unsigned int)index <= materialInfo.material.size()){
		return materialInfo.material[index-1].vnum;
	}
	return 0;
}

static int FN_check_cube_item_count_material(const SCubeMaterialInfo& materialInfo,int index)
{
	if ((unsigned int)index <= materialInfo.material.size()){
		return materialInfo.material[index-1].count;
	}

	return 0;
}

CUBE_RENEWAL_DATA::CUBE_RENEWAL_DATA()
{
	this->gold = 0;
	this->category = "WORLDARD";
#ifdef ENABLE_CUBE_RENEWAL_COPY_WORLDARD
	this->allowCopy = 0;
#endif
#ifdef ENABLE_GAYA_SYSTEM
	this->gaya = 0;
#endif
}


void Cube_init()
{
	CUBE_RENEWAL_DATA * p_cube = NULL;
	std::vector<CUBE_RENEWAL_DATA*>::iterator iter;

	char file_name[256+1];
	snprintf(file_name, sizeof(file_name), "%s/cube.txt", LocaleService_GetBasePath().c_str());

	sys_log(0, "Cube_Init %s", file_name);

	for (iter = s_cube_proto.begin(); iter!=s_cube_proto.end(); iter++)
	{
		p_cube = *iter;
		M2_DELETE(p_cube);
	}

	s_cube_proto.clear();

	if (false == Cube_load(file_name))
		sys_err("Cube_Init failed");
}

bool Cube_load (const char *file)
{
	FILE	*fp;


	const char	*value_string;

	char	one_line[256];
	int		value1, value2;
	const char	*delim = " \t\r\n";
	char	*v, *token_string;
	//char *v1;
	CUBE_RENEWAL_DATA	*cube_data = NULL;
	CUBE_RENEWAL_VALUE	cube_value = {0,0};

	if (0 == file || 0 == file[0])
		return false;

	if ((fp = fopen(file, "r")) == 0)
		return false;

	while (fgets(one_line, 256, fp))
	{
		value1 = value2 = 0;

		if (one_line[0] == '#')
			continue;

		token_string = strtok(one_line, delim);

		if (NULL == token_string)
			continue;

		// set value1, value2
		if ((v = strtok(NULL, delim)))
			str_to_number(value1, v);
		    value_string = v;

		if ((v = strtok(NULL, delim)))
			str_to_number(value2, v);

		TOKEN("section")
		{
			cube_data = M2_NEW CUBE_RENEWAL_DATA;
		}
		else TOKEN("npc")
		{
			cube_data->npc_vnum.push_back((WORD)value1);
		}
		else TOKEN("item")
		{
			cube_value.vnum		= value1;
			cube_value.count	= value2;

			cube_data->item.push_back(cube_value);
		}
		else TOKEN("reward")
		{
			cube_value.vnum		= value1;
			cube_value.count	= value2;

			cube_data->reward.push_back(cube_value);
		}
		else TOKEN("percent")
		{

			cube_data->percent = value1;
		}

		else TOKEN("category")
		{
			cube_data->category = value_string;
		}
#ifdef ENABLE_CUBE_RENEWAL_COPY_WORLDARD

		else TOKEN("allow_copy")
		{
			cube_data->allowCopy = value1;
		}

#endif

#ifdef ENABLE_GAYA_SYSTEM
		else TOKEN("gaya")
		{
			cube_data->gaya = value1;
		}
#endif
		else TOKEN("gold")
		{
			// ?¦?¶¿¡ ??¿ä?? ±?¾?
			cube_data->gold = value1;
		}
		else TOKEN("end")
		{

			// TODO : check cube data
			if (false == FN_check_cube_data(cube_data))
			{
				dev_log(LOG_DEB0, "something wrong");
				M2_DELETE(cube_data);
				continue;
			}
			s_cube_proto.push_back(cube_data);
		}
	}

	fclose(fp);
	return true;
}


SItemNameAndLevel SplitItemNameAndLevelFromName(const std::string& name)
{
	int level = 0;
	SItemNameAndLevel info;
	info.name = name;

	size_t pos = name.find("+");
	
	if (std::string::npos != pos)
	{
		const std::string levelStr = name.substr(pos + 1, name.size() - pos - 1);
		str_to_number(level, levelStr.c_str());

		info.name = name.substr(0, pos);
	}

	info.level = level;

	return info;
};


bool Cube_InformationInitialize()
{
	for (unsigned int i = 0; i < s_cube_proto.size(); ++i)
	{
		CUBE_RENEWAL_DATA* cubeData = s_cube_proto[i];

		const std::vector<CUBE_RENEWAL_VALUE>& rewards = cubeData->reward;

		if (1 != rewards.size())
		{
			sys_err("[CubeInfo] WARNING! Does not support multiple rewards (count: %d)", rewards.size());			
			continue;
		}

		const CUBE_RENEWAL_VALUE& reward = rewards.at(0);
		const WORD& npcVNUM = cubeData->npc_vnum.at(0);
		// bool bComplicate = false;
		
		TCubeMapByNPC& cubeMap = cube_info_map;
		TCubeResultList& resultList = cubeMap[npcVNUM];
		SCubeMaterialInfo materialInfo;

		materialInfo.reward = reward;
		materialInfo.gold = cubeData->gold;
		materialInfo.percent = cubeData->percent;
#ifdef ENABLE_GAYA_SYSTEM
		materialInfo.gaya = cubeData->gaya;
#endif
#ifdef ENABLE_CUBE_RENEWAL_COPY_WORLDARD
		materialInfo.allowCopy = cubeData->allowCopy;
#endif
		materialInfo.material = cubeData->item;
		materialInfo.category = cubeData->category;

		resultList.push_back(materialInfo);
	}

	return true;
}


void Cube_open (LPCHARACTER ch)
{
	LPCHARACTER	npc;
	npc = ch->GetQuestNPC();

	

	if (NULL==npc)
	{
		if (test_server)
			dev_log(LOG_DEB0, "cube_npc is NULL");
		return;
	}

	DWORD npcVNUM = npc->GetRaceNum();

	if ( FN_check_valid_npc(npcVNUM) == false )
	{
		if ( test_server == true )
		{
			dev_log(LOG_DEB0, "cube not valid NPC");
		}
		return;
	}


	if (ch->GetExchange() || ch->GetMyShop() || ch->GetShopOwner() || ch->IsOpenSafebox() || ch->IsCubeOpen()
#ifdef ENABLE_ACCE_SYSTEM
		 || ch->IsAcceOpen()
#endif
#ifdef __ENABLE_NEW_OFFLINESHOP__
		 || ch->GetOfflineShopGuest() || ch->GetAuctionGuest()
#endif
#ifdef __ATTR_TRANSFER_SYSTEM__
		 || ch->IsAttrTransferOpen()
#endif
	)
	{
#ifdef TEXTS_IMPROVEMENT
		ch->ChatPacketNew(CHAT_TYPE_INFO, 815, "");
#endif
		return;
	}

	long distance = DISTANCE_APPROX(ch->GetX() - npc->GetX(), ch->GetY() - npc->GetY());

	if (distance >= CUBE_MAX_DISTANCE)
	{
		sys_log(1, "CUBE: TOO_FAR: %s distance %d", ch->GetName(), distance);
		return;
	}


	SendDateCubeRenewalPackets(ch,CUBE_RENEWAL_SUB_HEADER_CLEAR_DATES_RECEIVE);
	SendDateCubeRenewalPackets(ch,CUBE_RENEWAL_SUB_HEADER_DATES_RECEIVE,npcVNUM);
	SendDateCubeRenewalPackets(ch,CUBE_RENEWAL_SUB_HEADER_DATES_LOADING);
	SendDateCubeRenewalPackets(ch,CUBE_RENEWAL_SUB_HEADER_OPEN_RECEIVE);

	ch->SetCubeNpc(npc);
}

void Cube_close(LPCHARACTER ch)
{
	ch->SetCubeNpc(NULL);
}

void Cube_Make(LPCHARACTER ch, int index, int count_item, int index_item_improve)
{
	LPCHARACTER	npc;

	npc = ch->GetQuestNPC();

	if (!ch->IsCubeOpen()) {
		return;
	}

	if (NULL == npc)
	{
		return;
	}

	int index_value = 0;
	bool material_check = true;
	bool item_frozen = false;
	LPITEM pItem;
	int iEmptyPos;
#ifdef ENABLE_CUBE_RENEWAL_COPY_WORLDARD
	DWORD copyAttr[ITEM_ATTRIBUTE_MAX_NUM][2];
	DWORD copySocket[ITEM_SOCKET_MAX_NUM];
	bool item_copy_bonus = false;
#endif
	const TCubeResultList& resultList = cube_info_map[npc->GetRaceNum()];
	for (TCubeResultList::const_iterator iter = resultList.begin(); resultList.end() != iter; ++iter)
	{
		if (index_value == index)
		{
			const SCubeMaterialInfo& materialInfo = *iter;

			for (unsigned int i = 0; i < materialInfo.material.size(); ++i)
			{
				if (ch->CountSpecifyItemRenewal(materialInfo.material[i].vnum) < (materialInfo.material[i].count*count_item))
				{
					item_frozen = true;
				}

				if (ch->CountSpecifyItem(materialInfo.material[i].vnum) < (materialInfo.material[i].count*count_item))
				{
					material_check = false;
				}

			}

			if (materialInfo.gold != 0){
				if (ch->GetGold() < (materialInfo.gold*count_item)) {
#ifdef TEXTS_IMPROVEMENT
					ch->ChatPacketNew(CHAT_TYPE_INFO, 232, "");
#endif
					return;
				}
			}
#ifdef ENABLE_GAYA_SYSTEM
			if (materialInfo.gaya != 0){
				if ((int32_t)ch->GetGaya() < (int32_t)(materialInfo.gaya*count_item))
				{
#ifdef TEXTS_IMPROVEMENT
					ch->ChatPacketNew(CHAT_TYPE_INFO, 524, "");
#endif
					return;
				}
			}
#endif

			if (item_frozen && material_check)
			{
#ifdef TEXTS_IMPROVEMENT
				ch->ChatPacketNew(CHAT_TYPE_INFO, 816, "");
#endif
				return;
			}

			if (material_check){
				
				int percent_number;
				int total_items_give = 0;

				
				int porcent_item_improve = 0;

				if (index_item_improve != -1)
				{

					LPITEM item = ch->GetInventoryItem(index_item_improve);
					if(item != NULL)
					{

						if(item->GetCount() <= 40){
							if (materialInfo.percent+item->GetCount() <= 100){
								porcent_item_improve = item->GetCount();
							}

							if(materialInfo.percent < 100)
							{
								if (materialInfo.percent+item->GetCount() > 100){
									porcent_item_improve = 100 - materialInfo.percent;
								}
							}
						}
					}

					if(porcent_item_improve != 0)
					{
						item->SetCount(item->GetCount()-porcent_item_improve);
					}
				}

				for (int i = 0; i < count_item; ++i)
				{
					percent_number = number(1,100);
					if ( percent_number<=materialInfo.percent+porcent_item_improve)
					{
						total_items_give++;
					}
				}
#ifdef ENABLE_CUBE_RENEWAL_COPY_WORLDARD

				LPITEM item = ITEM_MANAGER::instance().CreateItem(materialInfo.reward.vnum);

				pItem = ch->FindSpecifyItem(materialInfo.allowCopy);

				if(pItem != NULL){
					if ((pItem->GetType() == ITEM_WEAPON  || pItem->GetType() == ITEM_ARMOR) && item->GetSubType() == pItem->GetSubType() && item_copy_bonus == false)
					{

						for (int a = 0; a < ITEM_ATTRIBUTE_MAX_NUM; a++)
						{
							copyAttr[a][0] = pItem->GetAttributeType(a);
							copyAttr[a][1] = pItem->GetAttributeValue(a);
						}
							
						for (int a = 0; a < ITEM_SOCKET_MAX_NUM; a++)
						{
							copySocket[a] = pItem->GetSocket(a);
						}

						item_copy_bonus = true;
					}
				}
#endif
				pItem = ITEM_MANAGER::instance().CreateItem(materialInfo.reward.vnum,(materialInfo.reward.count*count_item));
				if (pItem->IsDragonSoul())
				{
					iEmptyPos = ch->GetEmptyDragonSoulInventory(pItem);
				}

#ifdef ENABLE_EXTRA_INVENTORY
				else if (pItem->IsExtraItem())
				{
					iEmptyPos = ch->GetEmptyExtraInventory(pItem);
				}
#endif
				else{
					iEmptyPos = ch->GetEmptyInventory(pItem->GetSize());
				}

				if (iEmptyPos < 0)
				{
#ifdef TEXTS_IMPROVEMENT
					ch->ChatPacketNew(CHAT_TYPE_INFO, 366, "");
#endif
					return;
				}

				for (unsigned int i = 0; i < materialInfo.material.size(); ++i)
				{
					ch->RemoveSpecifyItem(materialInfo.material[i].vnum, (materialInfo.material[i].count*count_item), true);
				}

				if (materialInfo.gold != 0){
					ch->PointChange(POINT_GOLD, -static_cast<long long>(materialInfo.gold*count_item), false);
						
				}
#ifdef ENABLE_GAYA_SYSTEM
				if (materialInfo.gaya != 0) {
#ifdef ENABLE_LONG_LONG
					ch->PointChange(POINT_GAYA, -static_cast<long long>(materialInfo.gaya*count_item), false);
#else
					ch->PointChange(POINT_GAYA, -(materialInfo.gaya*count_item), false);
#endif
				}
#endif
				if(total_items_give <= 0)
				{
#ifdef TEXTS_IMPROVEMENT
					ch->ChatPacketNew(CHAT_TYPE_INFO, 817, "");
#endif
					return;
				}
				
				int totalCount = materialInfo.reward.count * total_items_give;
				pItem = ITEM_MANAGER::instance().CreateItem(materialInfo.reward.vnum, totalCount);
#ifdef ENABLE_BATTLE_PASS
				BYTE bBattlePassId = ch->GetBattlePassId();
				if(bBattlePassId)
				{
					DWORD dwItemVnum, dwCount;
					if(CBattlePass::instance().BattlePassMissionGetInfo(bBattlePassId, CRAFT_ITEM, &dwItemVnum, &dwCount))
					{
						if(dwItemVnum == materialInfo.reward.vnum && ch->GetMissionProgress(CRAFT_ITEM, bBattlePassId) < dwCount)
							ch->UpdateMissionProgress(CRAFT_ITEM, bBattlePassId, totalCount, dwCount);
					}
				}
#endif				
				//toshow ChatPacket success item
#ifdef TEXTS_IMPROVEMENT
				ch->ChatPacketNew(CHAT_TYPE_INFO, 818, "");
#endif

#ifdef ENABLE_CUBE_RENEWAL_COPY_WORLDARD
				if (materialInfo.allowCopy != 0 && item_copy_bonus == true) {
					pItem->ClearAttribute();
					for (int a = 0; a < ITEM_ATTRIBUTE_MAX_NUM; a++) {
						pItem->SetForceAttribute(a, copyAttr[a][0], copyAttr[a][1]);
					}

					for (int b = 0; b < ITEM_SOCKET_MAX_NUM; b++) {
						pItem->SetSocket(b, copySocket[b]);
					}
				}
#endif

#ifdef ENABLE_BUG_FIXES
				if (!item_copy_bonus && pItem->GetType() == ITEM_COSTUME)
				{
					pItem->ClearAttribute();
#ifdef ENABLE_STOLE_COSTUME
					if (pItem->GetSubType() == COSTUME_STOLE)
					{
						uint8_t grade = pItem->GetValue(0);
						if (grade > 0)
						{
							grade = grade > 4 ? 4 : grade;
							uint8_t random = (grade * 4);

							for (int i = 0; i < MAX_ATTR; i++)
							{
								pItem->SetForceAttribute(i, stoleInfoTable[i][0], stoleInfoTable[i][number(random - 3, random)]);
							}
						}
					}
					else
					{
						pItem->AlterToMagicItem();
					}
#else
					pItem->AlterToMagicItem();
#endif
				}
#endif

				if (pItem->IsDragonSoul())
				{
					iEmptyPos = ch->GetEmptyDragonSoulInventory(pItem);
					pItem->AddToCharacter(ch, TItemPos(DRAGON_SOUL_INVENTORY, iEmptyPos));
				}

#ifdef ENABLE_EXTRA_INVENTORY
				else if (pItem->IsExtraItem())
				{
					iEmptyPos = ch->GetEmptyExtraInventory(pItem);
					if (pItem->IsStackable() && !IS_SET(pItem->GetAntiFlag(), ITEM_ANTIFLAG_STACK)) {
#ifdef ENABLE_NEW_STACK_LIMIT
						int 
#else
						BYTE 
#endif
						bCount = pItem->GetCount();
						for (int i = 0; i < EXTRA_INVENTORY_MAX_NUM; ++i) {
							LPITEM item2 = ch->GetExtraInventoryItem(i);
							if (!item2)
								continue;

							if (item2->GetVnum() == pItem->GetVnum()) {
								int j = 0;
								for (j = 0; j < ITEM_SOCKET_MAX_NUM; ++j)
									if (item2->GetSocket(j) != pItem->GetSocket(j))
										break;

								if (j != ITEM_SOCKET_MAX_NUM)
									continue;

#ifdef ENABLE_NEW_STACK_LIMIT
								int 
#else
								BYTE 
#endif
								bCount2 = MIN(g_bItemCountLimit - item2->GetCount(), bCount);
								bCount -= bCount2;

								item2->SetCount(item2->GetCount() + bCount2);
								if (bCount == 0) {
									M2_DESTROY_ITEM(pItem);
									pItem = NULL;
									break;
								}
							}
						}

						if (pItem != NULL) {
							pItem->SetCount(bCount);
							pItem->AddToCharacter(ch, TItemPos(EXTRA_INVENTORY, iEmptyPos));
						}
					} else {
						pItem->AddToCharacter(ch, TItemPos(EXTRA_INVENTORY, iEmptyPos));
					}
				}
#endif
				else {
					iEmptyPos = ch->GetEmptyInventory(pItem->GetSize());
					if (pItem->IsStackable() && !IS_SET(pItem->GetAntiFlag(), ITEM_ANTIFLAG_STACK)) {
#ifdef ENABLE_NEW_STACK_LIMIT
						int 
#else
						BYTE 
#endif
						bCount = pItem->GetCount();
						for (int i = 0; i < INVENTORY_MAX_NUM; ++i) {
							LPITEM item2 = ch->GetInventoryItem(i);
							if (!item2)
								continue;

							if (item2->GetVnum() == pItem->GetVnum()) {
								int j = 0;
								for (j = 0; j < ITEM_SOCKET_MAX_NUM; ++j)
									if (item2->GetSocket(j) != pItem->GetSocket(j))
										break;

								if (j != ITEM_SOCKET_MAX_NUM)
									continue;

#ifdef ENABLE_NEW_STACK_LIMIT
								int 
#else
								BYTE 
#endif
								bCount2 = MIN(g_bItemCountLimit - item2->GetCount(), bCount);
								bCount -= bCount2;

								item2->SetCount(item2->GetCount() + bCount2);
								if (bCount == 0) {
									M2_DESTROY_ITEM(pItem);
									pItem = NULL;
									break;
								}
							}
						}

						if (pItem != NULL) {
							pItem->SetCount(bCount);
							pItem->AddToCharacter(ch, TItemPos(INVENTORY, iEmptyPos));
						}
					} else {
						pItem->AddToCharacter(ch, TItemPos(INVENTORY, iEmptyPos));
					}
				}
			}
#ifdef TEXTS_IMPROVEMENT
			else {
				ch->ChatPacketNew(CHAT_TYPE_INFO, 819, "");
			}
#endif
		}

		index_value++;
	}
}


void SendDateCubeRenewalPackets(LPCHARACTER ch, BYTE subheader, DWORD npcVNUM)
{
	
	TPacketGCCubeRenewalReceive pack;
	pack.subheader = subheader;

	if(subheader == CUBE_RENEWAL_SUB_HEADER_DATES_RECEIVE)
	{
		const TCubeResultList& resultList = cube_info_map[npcVNUM];
		for (TCubeResultList::const_iterator iter = resultList.begin(); resultList.end() != iter; ++iter)
		{

			const SCubeMaterialInfo& materialInfo = *iter;

			pack.date_cube_renewal.vnum_reward = materialInfo.reward.vnum;
			pack.date_cube_renewal.count_reward = materialInfo.reward.count;

			LPITEM item = ITEM_MANAGER::instance().CreateItem(materialInfo.reward.vnum, materialInfo.reward.count);
			//tofix item not stackable
			if (item->IsStackable() && !IS_SET(item->GetAntiFlag(), ITEM_ANTIFLAG_STACK)){
				pack.date_cube_renewal.item_reward_stackable = true;
			}else{
				pack.date_cube_renewal.item_reward_stackable = false;
			}

			pack.date_cube_renewal.vnum_material_1 = FN_check_cube_item_vnum_material(materialInfo,1);
			pack.date_cube_renewal.count_material_1 = FN_check_cube_item_count_material(materialInfo,1);

			pack.date_cube_renewal.vnum_material_2 = FN_check_cube_item_vnum_material(materialInfo,2);
			pack.date_cube_renewal.count_material_2 = FN_check_cube_item_count_material(materialInfo,2);

			pack.date_cube_renewal.vnum_material_3 = FN_check_cube_item_vnum_material(materialInfo,3);
			pack.date_cube_renewal.count_material_3 = FN_check_cube_item_count_material(materialInfo,3);

			pack.date_cube_renewal.vnum_material_4 = FN_check_cube_item_vnum_material(materialInfo,4);
			pack.date_cube_renewal.count_material_4 = FN_check_cube_item_count_material(materialInfo,4);

			pack.date_cube_renewal.vnum_material_5 = FN_check_cube_item_vnum_material(materialInfo,5);
			pack.date_cube_renewal.count_material_5 = FN_check_cube_item_count_material(materialInfo,5);

			pack.date_cube_renewal.gold = materialInfo.gold;
			pack.date_cube_renewal.percent = materialInfo.percent;

#ifdef ENABLE_GAYA_SYSTEM
			pack.date_cube_renewal.gaya = materialInfo.gaya;
#endif

#ifdef ENABLE_CUBE_RENEWAL_COPY_WORLDARD
			pack.date_cube_renewal.allowCopy = materialInfo.allowCopy;
#endif
			
			memcpy (pack.date_cube_renewal.category, 	materialInfo.category.c_str(), 		sizeof(pack.date_cube_renewal.category));

			LPDESC d = ch->GetDesc();

			if (NULL == d)
			{
				sys_err ("User SendDateCubeRenewalPackets (%s)'s DESC is NULL POINT.", ch->GetName());
				return ;
			}

			d->Packet(&pack, sizeof(pack));
		}
	}
	else{

		LPDESC d = ch->GetDesc();

		if (NULL == d)
		{
			sys_err ("User SendDateCubeRenewalPackets (%s)'s DESC is NULL POINT.", ch->GetName());
			return ;
		}

		d->Packet(&pack, sizeof(pack));
	}
}