#pragma once

#include "RaceData.h"

#ifdef ENABLE_RACE_HEIGHT
typedef struct SRaceHeight {
	float	fHeight;
	float	fRiding;
} TRaceHeight;
#endif

class CRaceManager : public CSingleton<CRaceManager>
{
	public:
		typedef std::map<DWORD, CRaceData *> TRaceDataMap;
		typedef TRaceDataMap::iterator TRaceDataIterator;

	public:
		CRaceManager();
		virtual ~CRaceManager();

		void Create();
		void Destroy();

		void RegisterRaceName(DWORD dwRaceIndex, const char * c_szName);
#ifdef ENABLE_CLIENT_OPTIMIZATION
	void RegisterRaceSrcName(const std::string& c_szName, const std::string& c_szSrcName);
#else
		void RegisterRaceSrcName(const char * c_szName, const char * c_szSrcName);
#endif
		void SetPathName(const char * c_szPathName);
		const char * GetFullPathFileName(const char* c_szFileName);

		// Handling
		void CreateRace(DWORD dwRaceIndex);
		void SelectRace(DWORD dwRaceIndex);
		CRaceData * GetSelectedRaceDataPointer();
		// Handling

#ifdef INGAME_WIKI
		BOOL GetRaceDataPointer(DWORD dwRaceIndex, CRaceData ** ppRaceData, bool printTrace = true);
#else
		BOOL GetRaceDataPointer(DWORD dwRaceIndex, CRaceData ** ppRaceData);
#endif

#ifdef ENABLE_RACE_HEIGHT
	public:
		void	SetRaceHeight(int iVnum, float fHeight, float fRiding);
		float	GetRaceHeight(int iVnum);
		float	GetRidingRaceHeight(int iVnum);

	protected:
		std::map<int, TRaceHeight>	m_kMap_iRaceKey_fRaceAdditionalHeight;
#endif

	protected:
		CRaceData* __LoadRaceData(DWORD dwRaceIndex);
		bool __LoadRaceMotionList(CRaceData& rkRaceData, const char* pathName, const char* motionListFileName);

		void __Initialize();
		void __DestroyRaceDataMap();

	protected:
		TRaceDataMap					m_RaceDataMap;

		std::map<std::string, std::string> m_kMap_stRaceName_stSrcName;
		std::map<DWORD, std::string>	m_kMap_dwRaceKey_stRaceName;

	private:
		std::string						m_strPathName;
		CRaceData *						m_pSelectedRaceData;
};