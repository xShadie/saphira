#include "stdafx.h"
#include "char.h"
#include "desc.h"
#include "buffer_manager.h"
#ifdef ENABLE_ANTICHEAT
#include "db.h"
#include "log.h"
#include "desc_manager.h"
#endif
#ifdef ENABLE_HWID
#include "hwidmanager.h"
#endif

#ifdef TEXTS_IMPROVEMENT
void CHARACTER::ChatPacketNew(BYTE type, DWORD idx, const char * format, ...) {
	if (type != CHAT_TYPE_INFO && 
		type != CHAT_TYPE_NOTICE && 
		type != CHAT_TYPE_BIG_NOTICE
#ifdef ENABLE_DICE_SYSTEM
		 && type != CHAT_TYPE_DICE_INFO
#endif
#ifdef ENABLE_NEW_CHAT
		 && type != CHAT_TYPE_INFO_EXP
		 && type != CHAT_TYPE_INFO_ITEM
		 && type != CHAT_TYPE_INFO_VALUE
#endif
		&& type != CHAT_TYPE_DIALOG
	)
		return;

	LPDESC d = GetDesc();
	if (!d)
		return;

	char chatbuf[256];
	va_list args;
	va_start(args, format);
	int len = vsnprintf(chatbuf, sizeof(chatbuf), format, args);
	va_end(args);

	TPacketGCChatNew p;
	p.header = HEADER_GC_CHAT_NEW;
	p.type = type;
	p.idx = idx;
	p.size = sizeof(p) + len;

	TEMP_BUFFER buf;
	buf.write(&p, sizeof(p));
	if (len > 0) {
		buf.write(chatbuf, len);
	}
	d->Packet(buf.read_peek(), buf.size());
}
#endif

#ifdef __DUNGEON_INFO_SYSTEM__
void CHARACTER::SetQuestDamage(int race, int dmg) {
	if (race != 693 && 
		race != 768 && 
		race != 1093 && 
		race != 2092 && 
		race != 2493 && 
		race != 2598 && 
		race != 3962 && 
		race != 4011 && 
		race != 4158 && 
		race != 6091 && 
		race != 6191 && 
		race != 6192 && 
		race != 6118 && 
		race != 6393
#ifdef ENABLE_CHRISTMAS_2021
		 && race != 4081
		&& race != 4479
#endif
	) {
		return;
	}

	auto it = dungeonDamage.find(race);
	if (it == dungeonDamage.end()) {
		dungeonDamage.insert(dungeonDamage.begin(), std::pair<int, int>(race, dmg));
	} else {
		if (dmg > it->second)
		{
			it->second = dmg;
		}
	}
}

int CHARACTER::GetQuestDamage(int race) {
	auto it = dungeonDamage.find(race);
	return it == dungeonDamage.end() ? 0 : it->second;
}
#endif

#ifdef ENABLE_ANTICHEAT
void CHARACTER::ProcessCheatCheck(int32_t time) {
	if (GetGMLevel() == GM_PLAYER) {
		if (m_rewardCount == 0) {
			m_firstReward = time;
		}

		m_rewardCount++;

		if (m_rewardCount >= 7) {
			int32_t n = time - m_firstReward;
			if (n <= 7) {
				CHwidManager::Instance().SendBlockHwid("ANTICHEAT", GetName());

				LPDESC desc = GetDesc();
				if (desc) {
					desc->DelayedDisconnect(5);
				}
			} else {
				m_rewardCount = 0;
			}
		}
	}
//	if (GetGMLevel() == GM_PLAYER) {
//		if (m_rewardCount == 0) {
//			m_firstReward = time;
//		}
//
//		m_rewardCount++;
//
//		if (m_rewardCount >= 3) {
//			m_rewardCount = 0;
//
//			int32_t n = time - m_firstReward;
//			if (n < 5) {
//				m_checkRepeated++;
//			} else {
//				m_rewardCount = 0;
//				if (n >= 120) {
//					m_checkRepeated = 0;
//				}
//			}
//		}
//
//#ifdef ENABLE_HWID
//		if (m_checkRepeated == 5) {
//			CHwidManager::Instance().SendBlockHwid("ANTICHEAT", GetName());
//			LPDESC desc = GetDesc();
//			if (desc) {
//				desc->DelayedDisconnect(5);
//			}
//		}
//#endif
//	}
}

void CHARACTER::ClearCheatChecks() {
	m_firstReward = 0;
	m_rewardCount = 0;
	m_checkRepeated = 0;
}
#endif
