#define __IS_SERVER_WIKI_INCLUDE__

#ifndef __IS_SERVER_WIKI_INCLUDE__
#pragma once
#endif

#ifndef __INC_IN_GAME_WIKI_H__
#define __INC_IN_GAME_WIKI_H__

#pragma pack(1)

namespace CommonWikiData
{
	static const int MAX_REFINE_COUNT = 9;
	static const int REFINE_MATERIAL_MAX_NUM = 5;
	
	typedef struct SRefineMaterialNew
	{
		~SRefineMaterialNew() = default;
		SRefineMaterialNew() {
			vnum = 0;
			count = 0;
		}
		
		DWORD vnum;
		int count;
	} TRefineMaterialNew;
	
	typedef struct SWikiChestInfo
	{
		~SWikiChestInfo() = default;
		SWikiChestInfo(DWORD r_vnum = 0, DWORD r_count = 0) {
			vnum = r_vnum;
			count = r_count;
		}
		
		DWORD vnum;
		int count;
	} TWikiChestInfo;
	
	typedef struct SWikiMobDropInfo
	{
		~SWikiMobDropInfo() = default;
		SWikiMobDropInfo(DWORD r_vnum = 0, DWORD r_count = 0) {
			vnum = r_vnum;
			count = r_count;
		}
		
		DWORD vnum;
		int count;
	} TWikiMobDropInfo;
	
	typedef struct SWikiRefineInfo
	{
		~SWikiRefineInfo() = default;
		SWikiRefineInfo() {
			index = 0;
			memset(&materials, 0, sizeof(materials));
			mat_count = 0;
			price = 0;
		}
		
		int index;
		TRefineMaterialNew materials[REFINE_MATERIAL_MAX_NUM];
		DWORD mat_count;
		int price;
	} TWikiRefineInfo;
	
	typedef struct SWikiItemOriginInfo
	{
		~SWikiItemOriginInfo() = default;
		SWikiItemOriginInfo(DWORD r_vnum = 0, bool r_is_mob = false) {
			vnum = r_vnum;
			is_mob = r_is_mob;
		}
		
		void set_vnum(DWORD r_vnum) { vnum = r_vnum; }
		void set_is_mob(bool flag) { is_mob = flag; }
		
		DWORD vnum;
		bool is_mob;
	} TWikiItemOriginInfo;
	
	typedef struct SWikiInfoTable
	{
		~SWikiInfoTable() = default;
		SWikiInfoTable() {
			is_common = false;
			refine_infos_count = 0;
			chest_info_count = 0;
			origin_vnum = 0;
		}
		
		bool is_common;
		int refine_infos_count;
		int chest_info_count;
		DWORD origin_vnum;
	} TWikiInfoTable;
}

namespace InGameWiki
{
	enum HCommunicationsPacketsCG
	{
		HEADER_CG_WIKI = 200,
	};

	enum HCommunicationsPacketsGC
	{
		HEADER_GC_WIKI = 211,
	};
	
	enum SHCommunicationsPacketsGC
	{
		LOAD_WIKI_ITEM,
		LOAD_WIKI_MOB,
	};
	
	using namespace CommonWikiData;
	
	typedef struct SCGWikiPacket
	{
		~SCGWikiPacket() = default;
		SCGWikiPacket() {
			bHeader = HEADER_CG_WIKI;
			is_mob = false;
			vnum = 0;
			ret_id = 0;
		}

		BYTE bHeader;
		bool is_mob;
		DWORD vnum;
		unsigned long long ret_id;
	} TCGWikiPacket;
	
	typedef struct SGCWikiPacket
	{
		~SGCWikiPacket() = default;
		SGCWikiPacket() {
			bHeader = HEADER_GC_WIKI;
			size = 0;
			bSubHeader = LOAD_WIKI_ITEM;
		}
		
		void increment_data_size(WORD r_data_size) { size += r_data_size; }
		void set_data_type(BYTE r_dataType) { bSubHeader = r_dataType; }
		bool is_data_type(BYTE r_dataType) { return bSubHeader == r_dataType; }
		
		BYTE bHeader;
		WORD size;
		BYTE bSubHeader;
	} TGCWikiPacket;
	
	typedef struct SGCItemWikiPacket
	{
		~SGCItemWikiPacket() = default;
		SGCItemWikiPacket() = default;
		
		void mutable_wiki_info(TWikiInfoTable r_wif) { wiki_info = r_wif; }
		void set_origin_infos_count(int elem_count) { origin_infos_count = elem_count; }
		void set_vnum(DWORD r_vnum) { vnum = r_vnum; }
		void set_ret_id(unsigned long long r_ret_id) { ret_id = r_ret_id; }
		
		TWikiInfoTable wiki_info;
		int origin_infos_count;
		DWORD vnum;
		unsigned long long ret_id;
	} TGCItemWikiPacket;
	
	typedef struct SGCMobWikiPacket
	{
		~SGCMobWikiPacket() = default;
		SGCMobWikiPacket() {
			vnum = 0;
			ret_id = 0;
		}
		
		void set_drop_info_count(int r_drop_info_count) { drop_info_count = r_drop_info_count; }
		void set_vnum(DWORD r_vnum) { vnum = r_vnum; }
		void set_ret_id(unsigned long long r_ret_id) { ret_id = r_ret_id; }
		
		int drop_info_count;
		DWORD vnum;
		unsigned long long ret_id;
	} TGCMobWikiPacket;
}

#pragma pack()
#endif // __INC_IN_GAME_WIKI_H__
