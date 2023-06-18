#pragma once

#include "EffectInstance.h"

class CEffectManager : public CScreen, public CSingleton<CEffectManager>
{
	public:
		enum EEffectType
		{
			EFFECT_TYPE_NONE				= 0,
			EFFECT_TYPE_PARTICLE			= 1,
			EFFECT_TYPE_ANIMATION_TEXTURE	= 2,
			EFFECT_TYPE_MESH				= 3,
			EFFECT_TYPE_SIMPLE_LIGHT		= 4,

			EFFECT_TYPE_MAX_NUM				= 4,
		};

		typedef std::map<DWORD, CEffectData*> TEffectDataMap;
		typedef std::map<DWORD, CEffectInstance*> TEffectInstanceMap;
	public:
		CEffectManager();
		virtual ~CEffectManager();

		void Destroy();

		void UpdateSound();
		void Update();
		void Render();
		void RenderOne(DWORD id);
#ifdef INGAME_WIKI
		void WikiModuleRenderOneEffect(DWORD id);
#endif
		void GetInfo(std::string* pstInfo);

		bool IsAliveEffect(DWORD dwInstanceIndex);

		// Register
#ifdef ENABLE_SKILL_COLOR_SYSTEM
		BOOL RegisterEffect(const char * c_szFileName,bool isExistDelete=false,bool isNeedCache=false, const char * name=NULL);
		BOOL RegisterEffect2(const char * c_szFileName, DWORD* pdwRetCRC, bool isNeedCache=false, const char * name=NULL);
#else
		BOOL RegisterEffect(const char * c_szFileName,bool isExistDelete=false,bool isNeedCache=false);
		BOOL RegisterEffect2(const char * c_szFileName, DWORD* pdwRetCRC, bool isNeedCache=false);
#endif

		void DeleteAllInstances();

		// Usage
#ifdef ENABLE_SKILL_COLOR_SYSTEM
		int CreateEffect(DWORD dwID, const D3DXVECTOR3 & c_rv3Position, const D3DXVECTOR3 & c_rv3Rotation, DWORD * dwSkillColor = NULL);
#else
		int CreateEffect(DWORD dwID, const D3DXVECTOR3 & c_rv3Position, const D3DXVECTOR3 & c_rv3Rotation);
#endif
		int CreateEffect(const char * c_szFileName, const D3DXVECTOR3 & c_rv3Position, const D3DXVECTOR3 & c_rv3Rotation);

#ifdef ENABLE_SKILL_COLOR_SYSTEM
		void CreateEffectInstance(DWORD dwInstanceIndex, DWORD dwID, DWORD * dwSkillColor = NULL);
#else
		void CreateEffectInstance(DWORD dwInstanceIndex, DWORD dwID);
#endif
		BOOL SelectEffectInstance(DWORD dwInstanceIndex);
		bool DestroyEffectInstance(DWORD dwInstanceIndex);
		void DeactiveEffectInstance(DWORD dwInstanceIndex);
		void SetEffectOption(BYTE Type, bool bVisible) { BLDisableEffects[Type] = bVisible; };
		bool GetEffectOption(BYTE Type) const { return BLDisableEffects[Type]; };
		bool CanRenderFunction(CEffectInstance* pkEftInst) const;
		enum {
			EFFECT_OPTION_BUFF,
			EFFECT_OPTION_SKILL,
			EFFECT_OPTION_ARMORS_SHININGS,
			EFFECT_OPTION_WEAPONS_SHININGS,
			EFFECT_OPTION_ALL,
			EFFECT_OPTION_MAX
		};

		void SetEffectTextures(DWORD dwID, std::vector<std::string> textures);
		void SetEffectInstancePosition(const D3DXVECTOR3 & c_rv3Position);
		void SetEffectInstanceRotation(const D3DXVECTOR3 & c_rv3Rotation);
		void SetEffectInstanceGlobalMatrix(const D3DXMATRIX & c_rmatGlobal);

		void ShowEffect();
		void HideEffect();

		// Temporary function
		DWORD GetRandomEffect();
		int GetEmptyIndex();
		bool GetEffectData(DWORD dwID, CEffectData ** ppEffect);
		bool GetEffectData(DWORD dwID, const CEffectData ** c_ppEffect);

		// Area�� ���� ��� Effect�� �Լ�... EffectInstance�� Pointer�� ��ȯ�Ѵ�.
		// EffectManager ���� EffectInstanceMap�� �̿����� �ʴ´�.
		void CreateUnsafeEffectInstance(DWORD dwEffectDataID, CEffectInstance ** ppEffectInstance);
		bool DestroyUnsafeEffectInstance(CEffectInstance * pEffectInstance);

		int GetRenderingEffectCount();
	protected:
		void __Initialize();

		void __DestroyEffectInstanceMap();
		void __DestroyEffectCacheMap();
		void __DestroyEffectDataMap();

	protected:
		bool m_isDisableSortRendering;
		TEffectDataMap					m_kEftDataMap;
		TEffectInstanceMap				m_kEftInstMap;
		TEffectInstanceMap				m_kEftCacheMap;
		CEffectInstance *				m_pSelectedEffectInstance;
		bool							BLDisableEffects[EFFECT_OPTION_MAX];
};
