#include "StdAfx.h"
#include "PythonChat.h"

#include "AbstractApplication.h"
#include "PythonCharacterManager.h"
#include "../eterbase/Timer.h"
#include "PythonApplication.h"
#ifdef ENABLE_MULTI_LANGUAGE
#include "../gamelib/ItemManager.h"
#endif
#ifdef ENABLE_AUTO_TRANSLATE_WHISPER
#include "CGoogleTranslate.h"
#include "PythonSystem.h"
#endif

int CPythonChat::TChatSet::ms_iChatModeSize = CHAT_TYPE_MAX_NUM;

const float c_fStartDisappearingTime = 5.0f;
const int c_iMaxLineCount = 5;

///////////////////////////////////////////////////////////////////////////////////////////////////

CDynamicPool<CPythonChat::SChatLine> CPythonChat::SChatLine::ms_kPool;

void CPythonChat::SetChatColor(UINT eType, UINT r, UINT g, UINT b)
{
	if (eType>=CHAT_TYPE_MAX_NUM)
		return;

	DWORD dwColor=(0xff000000)|(r<<16)|(g<<8)|(b);
	m_akD3DXClrChat[eType]=D3DXCOLOR(dwColor);
}

///////////////////////////////////////////////////////////////////////////////////////////////////

CPythonChat::SChatLine* CPythonChat::SChatLine::New()
{
	return ms_kPool.Alloc();
}

void CPythonChat::SChatLine::Delete(CPythonChat::SChatLine* pkChatLine)
{
	pkChatLine->Instance.Destroy();
#ifdef ENABLE_MULTI_LANGUAGE
	pkChatLine->pLanguage.Destroy();
#endif	
	ms_kPool.Free(pkChatLine);
}

void CPythonChat::SChatLine::DestroySystem()
{
	ms_kPool.Destroy();
}

void CPythonChat::SChatLine::SetColor(DWORD dwID, DWORD dwColor)
{
	assert(dwID < CHAT_LINE_COLOR_ARRAY_MAX_NUM);

	if (dwID >= CHAT_LINE_COLOR_ARRAY_MAX_NUM)
		return;

	aColor[dwID] = dwColor;
}

void CPythonChat::SChatLine::SetColorAll(DWORD dwColor)
{
	for (int i = 0; i < CHAT_LINE_COLOR_ARRAY_MAX_NUM; ++i)
		aColor[i] = dwColor;
}

D3DXCOLOR & CPythonChat::SChatLine::GetColorRef(DWORD dwID)
{
	assert(dwID < CHAT_LINE_COLOR_ARRAY_MAX_NUM);

	if (dwID >= CHAT_LINE_COLOR_ARRAY_MAX_NUM)
	{
		static D3DXCOLOR color(1.0f, 0.0f, 0.0f, 1.0f);
		return color;
	}

	return aColor[dwID];
}

CPythonChat::SChatLine::SChatLine()
{
	for (int i = 0; i < CHAT_LINE_COLOR_ARRAY_MAX_NUM; ++i)
		aColor[i] = 0xff0000ff;
}
CPythonChat::SChatLine::~SChatLine()
{
	Instance.Destroy();
#ifdef ENABLE_MULTI_LANGUAGE
	pLanguage.Destroy();
#endif	
}

///////////////////////////////////////////////////////////////////////////////////////////////////

int CPythonChat::CreateChatSet(DWORD dwID)
{
	m_ChatSetMap.insert(std::make_pair(dwID, TChatSet()));
	return dwID;
}

void CPythonChat::UpdateViewMode(DWORD dwID)
{
	IAbstractApplication& rApp=IAbstractApplication::GetSingleton();

	float fcurTime = rApp.GetGlobalTime();

	TChatSet * pChatSet = GetChatSetPtr(dwID);
	if (!pChatSet)
		return;

	TChatLineList * pLineList = &(pChatSet->m_ShowingChatLineList);

#ifdef ENABLE_NEW_CHAT
	TChatLineList::iterator itor = pLineList->begin();
	for (; itor != pLineList->end();) {
		TChatLine * pChatLine = (*itor);
		if (!pChatSet->CheckMode(pChatLine->iType)) {
			itor = pLineList->erase(itor);
		} else {
			++itor;
		}
	}
#endif

	int iLineIndex = pLineList->size();
	int iHeight = -(int(pLineList->size()+1) * pChatSet->m_iStep);

#ifndef ENABLE_NEW_CHAT
	TChatLineList::iterator itor;
#endif
	for (itor = pLineList->begin(); itor != pLineList->end();)
	{
		TChatLine * pChatLine = (*itor);
		D3DXCOLOR & rColor = pChatLine->GetColorRef(dwID);

		float fElapsedTime = (fcurTime - pChatLine->fAppendedTime);
		if (fElapsedTime >= c_fStartDisappearingTime || iLineIndex >= c_iMaxLineCount)
		{
			rColor.a -= rColor.a / 10.0f;

			if (rColor.a <= 0.1f)
			{
				itor = pLineList->erase(itor);
			}
			else
			{
				++itor;
			}
		}
		else
		{
			++itor;
		}

		/////

		iHeight += pChatSet->m_iStep;
		--iLineIndex;
		int iWidth = pChatSet->m_ix;
#ifdef ENABLE_MULTI_LANGUAGE
		if (pChatLine->pLanguage.GetWidth() > 0)
		{
			pChatLine->pLanguage.SetPosition(iWidth, pChatSet->m_iy + iHeight + 2);
			iWidth += pChatLine->pLanguage.GetWidth() + 3;
		}
#endif
		
		pChatLine->Instance.SetPosition(iWidth, pChatSet->m_iy + iHeight);
		pChatLine->Instance.SetColor(rColor);
		pChatLine->Instance.Update();
	}
}

void CPythonChat::UpdateEditMode(DWORD dwID)
{
	TChatSet * pChatSet = GetChatSetPtr(dwID);
	if (!pChatSet)
		return;

	const int c_iAlphaLine = max(0, GetVisibleLineCount(dwID) - GetEditableLineCount(dwID) + 2);

	int iLineIndex = 0;
	float fAlpha = 0.0f;
	float fAlphaStep = 0.0f;

	if (c_iAlphaLine > 0)
		fAlphaStep = 1.0f / float(c_iAlphaLine);

	TChatLineList * pLineList = &(pChatSet->m_ShowingChatLineList);
	int iHeight = -(int(pLineList->size()+1) * pChatSet->m_iStep);

	for (TChatLineList::iterator itor = pLineList->begin(); itor != pLineList->end(); ++itor)
	{
		TChatLine * pChatLine = (*itor);

		D3DXCOLOR & rColor = pChatLine->GetColorRef(dwID);

		if (iLineIndex < c_iAlphaLine)
		{
			rColor.a += (fAlpha - rColor.a) / 10.0f;
			fAlpha = fMIN(fAlpha+fAlphaStep, 1.0f);
		}
		else
		{
			rColor.a = fMIN(rColor.a+0.05f, 1.0f);
		}

		int iWidth = pChatSet->m_ix;
		iHeight += pChatSet->m_iStep;
#ifdef ENABLE_MULTI_LANGUAGE
		if (pChatLine->pLanguage.GetWidth() > 0)
		{
			pChatLine->pLanguage.SetPosition(iWidth, pChatSet->m_iy + iHeight + 2);
			iWidth += pChatLine->pLanguage.GetWidth() + 3;
		}
#endif
		
		pChatLine->Instance.SetPosition(iWidth, pChatSet->m_iy + iHeight);
		pChatLine->Instance.SetColor(rColor);
		pChatLine->Instance.Update();
	}
}

void CPythonChat::UpdateLogMode(DWORD dwID)
{
	TChatSet * pChatSet = GetChatSetPtr(dwID);
	if (!pChatSet)
		return;

	TChatLineList * pLineList = &(pChatSet->m_ShowingChatLineList);
	int iHeight = 0;

	for (TChatLineList::reverse_iterator itor = pLineList->rbegin(); itor != pLineList->rend(); ++itor)
	{
		TChatLine * pChatLine = (*itor);

		iHeight -= pChatSet->m_iStep;
		
		int iWidth = pChatSet->m_ix;
#ifdef ENABLE_MULTI_LANGUAGE
		if (pChatLine->pLanguage.GetWidth() > 0)
		{
			pChatLine->pLanguage.SetPosition(iWidth, pChatSet->m_iy + iHeight + 2);
			iWidth += pChatLine->pLanguage.GetWidth() + 3;
		}
#endif
		pChatLine->Instance.SetPosition(iWidth, pChatSet->m_iy + iHeight);
		pChatLine->Instance.SetColor(pChatLine->GetColorRef(dwID));
		pChatLine->Instance.Update();
	}
}

void CPythonChat::Update(DWORD dwID)
{
	TChatSet * pChatSet = GetChatSetPtr(dwID);
	if (!pChatSet)
		return;

	switch (pChatSet->m_iBoardState)
	{
		case BOARD_STATE_VIEW:
			UpdateViewMode(dwID);
			break;
		case BOARD_STATE_EDIT:
			UpdateEditMode(dwID);
			break;
		case BOARD_STATE_LOG:
//#ifdef ENABLE_MULTI_LANGUAGE
//		{
//			UpdateLogMode(dwID);
//			UpdateViewMode(dwID);
//			UpdateEditMode(dwID);
//		}
//#else
			UpdateLogMode(dwID);
//#endif
			break;
	}

	DWORD dwcurTime = CTimer::Instance().GetCurrentMillisecond();
	for (TWaitChatList::iterator itor = m_WaitChatList.begin(); itor != m_WaitChatList.end();)
	{
		TWaitChat & rWaitChat = *itor;

		if (rWaitChat.dwAppendingTime < dwcurTime)
		{
			AppendChat(rWaitChat.iType, rWaitChat.strChat.c_str());

			itor = m_WaitChatList.erase(itor);
		}
		else
		{
			++itor;
		}
	}
}

void CPythonChat::Render(DWORD dwID)
{
	TChatLineList * pLineList = GetChatLineListPtr(dwID);
	if (!pLineList)
		return;

	for (TChatLineList::iterator itor = pLineList->begin(); itor != pLineList->end(); ++itor)
	{
		CGraphicTextInstance & rInstance = (*itor)->Instance;
		rInstance.Render();
#ifdef ENABLE_MULTI_LANGUAGE
		CGraphicImageInstance & lInstance = (*itor)->pLanguage;
		lInstance.Render();
#endif	
	}
}


void CPythonChat::SetBoardState(DWORD dwID, int iState)
{
	TChatSet * pChatSet = GetChatSetPtr(dwID);
	if (!pChatSet)
		return;

	pChatSet->m_iBoardState = iState;
	ArrangeShowingChat(dwID);
}
void CPythonChat::SetPosition(DWORD dwID, int ix, int iy)
{
	TChatSet * pChatSet = GetChatSetPtr(dwID);
	if (!pChatSet)
		return;

	pChatSet->m_ix = ix;
	pChatSet->m_iy = iy;
}
void CPythonChat::SetHeight(DWORD dwID, int iHeight)
{
	TChatSet * pChatSet = GetChatSetPtr(dwID);
	if (!pChatSet)
		return;

	pChatSet->m_iHeight = iHeight;
}
void CPythonChat::SetStep(DWORD dwID, int iStep)
{
	TChatSet * pChatSet = GetChatSetPtr(dwID);
	if (!pChatSet)
		return;

	pChatSet->m_iStep = iStep;
}
void CPythonChat::ToggleChatMode(DWORD dwID, int iMode)
{
	TChatSet * pChatSet = GetChatSetPtr(dwID);
	if (!pChatSet)
		return;

	pChatSet->m_iMode[iMode] = 1 - pChatSet->m_iMode[iMode];
// 	Tracef("ToggleChatMode : %d\n", iMode);
	ArrangeShowingChat(dwID);
}
void CPythonChat::EnableChatMode(DWORD dwID, int iMode)
{
	TChatSet * pChatSet = GetChatSetPtr(dwID);
	if (!pChatSet)
		return;

	pChatSet->m_iMode[iMode] = TRUE;
// 	Tracef("EnableChatMode : %d\n", iMode);
	ArrangeShowingChat(dwID);
}
void CPythonChat::DisableChatMode(DWORD dwID, int iMode)
{
	TChatSet * pChatSet = GetChatSetPtr(dwID);
	if (!pChatSet)
		return;

	pChatSet->m_iMode[iMode] = FALSE;
// 	Tracef("DisableChatMode : %d\n", iMode);
	ArrangeShowingChat(dwID);
}
void CPythonChat::SetEndPos(DWORD dwID, float fPos)
{
	TChatSet * pChatSet = GetChatSetPtr(dwID);
	if (!pChatSet)
		return;

	fPos = max(0.0f, fPos);
	fPos = min(1.0f, fPos);
	if (pChatSet->m_fEndPos != fPos)
	{
		pChatSet->m_fEndPos = fPos;
		ArrangeShowingChat(dwID);
	}
}


int CPythonChat::GetVisibleLineCount(DWORD dwID)
{
	TChatLineList * pLineList = GetChatLineListPtr(dwID);
	if (!pLineList)
		return 0;

	return pLineList->size();
}

int CPythonChat::GetEditableLineCount(DWORD dwID)
{
	TChatSet * pChatSet = GetChatSetPtr(dwID);
	if (!pChatSet)
		return 0;

	return pChatSet->m_iHeight / pChatSet->m_iStep + 1;
}

int CPythonChat::GetLineCount(DWORD dwID)
{
	TChatSet * pChatSet = GetChatSetPtr(dwID);
	if (!pChatSet)
		return 0;

	int iCount = 0;
	for (DWORD i = 0; i < m_ChatLineDeque.size(); ++i)
	{
		if (!pChatSet->CheckMode(m_ChatLineDeque[i]->iType))
			continue;

		++iCount;
	}

	return iCount;
}

int CPythonChat::GetLineStep(DWORD dwID)
{
	TChatSet * pChatSet = GetChatSetPtr(dwID);
	if (!pChatSet)
		return 0;

	return pChatSet->m_iStep;
}

CPythonChat::TChatLineList * CPythonChat::GetChatLineListPtr(DWORD dwID)
{
	TChatSetMap::iterator itor = m_ChatSetMap.find(dwID);
	if (m_ChatSetMap.end() == itor)
		return NULL;

	TChatSet & rChatSet = itor->second;
	return &(rChatSet.m_ShowingChatLineList);
}

CPythonChat::TChatSet * CPythonChat::GetChatSetPtr(DWORD dwID)
{
	TChatSetMap::iterator itor = m_ChatSetMap.find(dwID);
	if (m_ChatSetMap.end() == itor)
		return NULL;

	TChatSet & rChatSet = itor->second;
	return &rChatSet;
}

void CPythonChat::ArrangeShowingChat(DWORD dwID)
{
	TChatSet * pChatSet = GetChatSetPtr(dwID);
	if (!pChatSet)
		return;

	pChatSet->m_ShowingChatLineList.clear();

	TChatLineDeque TempChatLineDeque;
	for (TChatLineDeque::iterator itor = m_ChatLineDeque.begin(); itor != m_ChatLineDeque.end(); ++itor)
	{
		TChatLine * pChatLine = *itor;
		if (pChatSet->CheckMode(pChatLine->iType))
			TempChatLineDeque.push_back(pChatLine);
	}

	int icurLineCount = TempChatLineDeque.size();
	int iVisibleLineCount = min(icurLineCount, (pChatSet->m_iHeight + pChatSet->m_iStep) / pChatSet->m_iStep);
	int iEndLine = iVisibleLineCount + int(float(icurLineCount - iVisibleLineCount - 1) * pChatSet->m_fEndPos);

	/////

	int iHeight = 12;
	for (int i = min(icurLineCount-1, iEndLine); i >= 0; --i)
	{
		if (!pChatSet->CheckMode(TempChatLineDeque[i]->iType))
			continue;

		if (pChatSet->m_iHeight + pChatSet->m_iStep <= iHeight)
		{
			break;
		}

		pChatSet->m_ShowingChatLineList.push_front(TempChatLineDeque[i]);

		iHeight += pChatSet->m_iStep;
	}
}

#ifdef ENABLE_MULTI_LANGUAGE
bool CPythonChat::IsFilteredLanguage(std::string szLanguage)
{
	std::string filteredList = CPythonSystem::Instance().GetChatFilterValue();
	std::size_t langPos = filteredList.find(szLanguage);

	if (langPos != std::string::npos)
		return true;

	return false;
}
#endif

void CPythonChat::AppendChat(int iType, const char * c_szChat)
{
	// DEFAULT_FONT
	//static CResource * s_pResource = CResourceManager::Instance().GetResourcePointer(g_strDefaultFontName.c_str());
	CGraphicText* pkDefaultFont = static_cast<CGraphicText*>(DefaultFont_GetResource());
	if (!pkDefaultFont)
	{
		TraceError("CPythonChat::AppendChat - CANNOT_FIND_DEFAULT_FONT");
		return;
	}

	IAbstractApplication& rApp=IAbstractApplication::GetSingleton();
	SChatLine * pChatLine = SChatLine::New();
#ifdef ENABLE_MULTI_LANGUAGE
	std::string chatStr = c_szChat;
	std::string replace = "";
	int firstDelPos = chatStr.find("[{");
	int secondDelPos = chatStr.find("}]");
	int vnum = 0;
	while (firstDelPos > 0 && secondDelPos > 0) {
		replace = chatStr.substr(firstDelPos + 1, secondDelPos-firstDelPos);
		vnum = stoi(chatStr.substr(firstDelPos+2, secondDelPos-firstDelPos-2));
		CItemManager::Instance().SelectItemData(vnum);
		CItemData * pItemData = CItemManager::Instance().GetSelectedItemDataPointer();

		std::string vnumStr = std::to_string(vnum);
		chatStr.replace(firstDelPos + 1, secondDelPos-firstDelPos, vnumStr.c_str());
		if (pItemData) {
			vnumStr = pItemData->GetName();
		}

		firstDelPos = chatStr.find("[-");
		secondDelPos = chatStr.find("-]");
		if (firstDelPos > 0 && secondDelPos > 0) {
			replace = chatStr.substr(firstDelPos + 1, secondDelPos-firstDelPos);
			chatStr.replace(firstDelPos + 1, secondDelPos-firstDelPos, vnumStr.c_str());
		}

		firstDelPos = chatStr.find("[{");
		secondDelPos = chatStr.find("}]");
	}

	std::size_t startLang = chatStr.find("|L");
	std::size_t endLang = chatStr.find("|l");
	if (startLang != std::string::npos && endLang != std::string::npos)
	{
		std::string strLanguage = chatStr.substr(startLang + 2, endLang - 2);
		if (IsFilteredLanguage(strLanguage))
		{
			SChatLine::Delete(pChatLine);
			return;
		}
	
		char szLangName[256];
		sprintf(szLangName, "d:/ymir work/ui/game/flag/%s.tga", strLanguage.c_str());
		if (CResourceManager::Instance().IsFileExist(szLangName))
		{
			CGraphicImage * pLanguageImage = (CGraphicImage *)CResourceManager::Instance().GetResourcePointer(szLangName);
	
			if (pLanguageImage)
				pChatLine->pLanguage.SetImagePointer(pLanguageImage);
		}
	
		chatStr = chatStr.substr(endLang + 2, chatStr.length());
	}
#endif
	
	pChatLine->iType = iType;
#ifdef ENABLE_MULTI_LANGUAGE
	pChatLine->Instance.SetValue(chatStr.c_str());
#else
	pChatLine->Instance.SetValue(c_szChat);
#endif
	pChatLine->Instance.SetTextPointer(pkDefaultFont);
	// END_OF_DEFAULT_FONT

	pChatLine->fAppendedTime = rApp.GetGlobalTime();
	pChatLine->SetColorAll(GetChatColor(iType));

	m_ChatLineDeque.push_back(pChatLine);
	if (m_ChatLineDeque.size() > CHAT_LINE_MAX_NUM)
	{
		SChatLine * pChatLine = m_ChatLineDeque.front();
		SChatLine::Delete(pChatLine);
		m_ChatLineDeque.pop_front();
	}

	for (TChatSetMap::iterator itor = m_ChatSetMap.begin(); itor != m_ChatSetMap.end(); ++itor)
	{
		TChatSet * pChatSet = &(itor->second);
		//pChatLine->SetColor(itor->first, GetChatColor(iType));

		// Edit Mode ???| ?uiAo??I ??????o ???AA?????a A??C?? A????????
		if (BOARD_STATE_EDIT == pChatSet->m_iBoardState)
		{
			ArrangeShowingChat(itor->first);
		}
		else// if (BOARD_STATE_VIEW == pChatSet->m_iBoardState)
		{
			pChatSet->m_ShowingChatLineList.push_back(pChatLine);
			if (pChatSet->m_ShowingChatLineList.size() > CHAT_LINE_MAX_NUM)
			{
				pChatSet->m_ShowingChatLineList.pop_front();
			}
		}
	}
}

void CPythonChat::AppendChatWithDelay(int iType, const char * c_szChat, int iDelay)
{
	TWaitChat WaitChat;
	WaitChat.iType = iType;
	WaitChat.strChat = c_szChat;
	WaitChat.dwAppendingTime = CTimer::Instance().GetCurrentMillisecond() + iDelay;
	m_WaitChatList.push_back(WaitChat);
}

DWORD CPythonChat::GetChatColor(int iType)
{
	if (iType<CHAT_TYPE_MAX_NUM)
	{
		return m_akD3DXClrChat[iType];
	}

	return D3DXCOLOR(0.0f, 0.0f, 1.0f, 1.0f);
}

void CPythonChat::IgnoreCharacter(const char * c_szName)
{
	TIgnoreCharacterSet::iterator itor = m_IgnoreCharacterSet.find(c_szName);
	// NOTE : ??? ??? ??????..
	if (m_IgnoreCharacterSet.end() != itor)
	{
		m_IgnoreCharacterSet.erase(itor);
	}
	// NOTE : ???????? ??? ????????.
	else
	{
		m_IgnoreCharacterSet.insert(c_szName);
	}
}

BOOL CPythonChat::IsIgnoreCharacter(const char * c_szName)
{
	TIgnoreCharacterSet::iterator itor = m_IgnoreCharacterSet.find(c_szName);

	if (m_IgnoreCharacterSet.end() == itor)
		return FALSE;

	return TRUE;
}

CWhisper * CPythonChat::CreateWhisper(const char * c_szName)
{
	CWhisper * pWhisper = CWhisper::New();
	m_WhisperMap.insert(TWhisperMap::value_type(c_szName, pWhisper));
	return pWhisper;
}

void CPythonChat::AppendWhisper(int iType, const char * c_szName, const char * c_szChat
#ifdef ENABLE_AUTO_TRANSLATE_WHISPER
, bool translated, bool bnew, int unicode
#endif
)
{
#ifndef ENABLE_AUTO_TRANSLATE_WHISPER
	TWhisperMap::iterator itor = m_WhisperMap.find(c_szName);

	CWhisper * pWhisper;
	if (itor == m_WhisperMap.end())
	{
		pWhisper = CreateWhisper(c_szName);
	}
	else
	{
		pWhisper = itor->second;
	}

	pWhisper->AppendChat(iType, c_szChat);
#else
	if (translated) {
		TWhisperMap::iterator itor = m_WhisperMap.find(c_szName);
		CWhisper * pWhisper;
		if (itor == m_WhisperMap.end()) {
			pWhisper = CreateWhisper(c_szName);
		} else {
			pWhisper = itor->second;
		}

		pWhisper->AppendChat(iType, c_szChat, bnew, unicode);
	} else {
		if (CGoogleTranslate::Instance().run(iType, c_szName, c_szChat) == 1) {
		}
	}
#endif
}

void CPythonChat::ClearWhisper(const char * c_szName)
{
	TWhisperMap::iterator itor = m_WhisperMap.find(c_szName);

	if (itor != m_WhisperMap.end())
	{
		CWhisper * pWhisper = itor->second;
		CWhisper::Delete(pWhisper);

		m_WhisperMap.erase(itor);
	}
}

BOOL CPythonChat::GetWhisper(const char * c_szName, CWhisper ** ppWhisper)
{
	TWhisperMap::iterator itor = m_WhisperMap.find(c_szName);

	if (itor == m_WhisperMap.end())
		return FALSE;

	*ppWhisper = itor->second;

	return TRUE;
}

void CPythonChat::InitWhisper(PyObject * ppyObject)
{
	TWhisperMap::iterator itor = m_WhisperMap.begin();
	for (; itor != m_WhisperMap.end(); ++itor)
	{
		std::string strName = itor->first;
		PyCallClassMemberFunc(ppyObject, "MakeWhisperButton", Py_BuildValue("(s)", strName.c_str()));
	}
}

void CPythonChat::__DestroyWhisperMap()
{
	TWhisperMap::iterator itor = m_WhisperMap.begin();
	for (; itor != m_WhisperMap.end(); ++itor)
	{
		CWhisper::Delete(itor->second);
	}
	m_WhisperMap.clear();
}

void CPythonChat::Close()
{
	TChatSetMap::iterator itor = m_ChatSetMap.begin();
	for (; itor != m_ChatSetMap.end(); ++itor)
	{
		TChatSet & rChatSet = itor->second;
		TChatLineList * pLineList = &(rChatSet.m_ShowingChatLineList);
		for (TChatLineList::iterator itor = pLineList->begin(); itor != pLineList->end(); ++itor)
		{
			TChatLine * pChatLine = (*itor);
			pChatLine->fAppendedTime = 0.0f;
		}
	}
}

void CPythonChat::Destroy()
{
	__DestroyWhisperMap();

	m_ShowingChatLineList.clear();
	m_ChatSetMap.clear();
	m_ChatLineDeque.clear();

	SChatLine::DestroySystem();
	CWhisper::DestroySystem();

	__Initialize();
}

void CPythonChat::__Initialize()
{
	m_akD3DXClrChat[CHAT_TYPE_TALKING]		= D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	m_akD3DXClrChat[CHAT_TYPE_INFO]			= D3DXCOLOR(1.0f, 0.785f, 0.785f, 1.0f);
	m_akD3DXClrChat[CHAT_TYPE_NOTICE]		= D3DXCOLOR(1.0f, 0.902f, 0.730f, 1.0f);
	m_akD3DXClrChat[CHAT_TYPE_PARTY]		= D3DXCOLOR(0.542f, 1.0f, 0.949f, 1.0f);
	m_akD3DXClrChat[CHAT_TYPE_GUILD]		= D3DXCOLOR(0.906f, 0.847f, 1.0f, 1.0f);
	m_akD3DXClrChat[CHAT_TYPE_COMMAND]		= D3DXCOLOR(0.658f, 1.0f, 0.835f, 1.0f);
	m_akD3DXClrChat[CHAT_TYPE_SHOUT]		= D3DXCOLOR(0.658f, 1.0f, 0.835f, 1.0f);
	m_akD3DXClrChat[CHAT_TYPE_WHISPER]		= D3DXCOLOR(0xff4AE14A);
	m_akD3DXClrChat[CHAT_TYPE_BIG_NOTICE]	= D3DXCOLOR(1.0f, 0.902f, 0.730f, 1.0f);
#ifdef ENABLE_DICE_SYSTEM
	m_akD3DXClrChat[CHAT_TYPE_DICE_INFO]	= D3DXCOLOR(0xFF4AE14A);
#endif
#ifdef ENABLE_NEW_CHAT
	m_akD3DXClrChat[CHAT_TYPE_INFO_EXP] = D3DXCOLOR(1.0f, 0.785f, 0.785f, 1.0f);
	m_akD3DXClrChat[CHAT_TYPE_INFO_ITEM] = D3DXCOLOR(1.0f, 0.785f, 0.785f, 1.0f);
	m_akD3DXClrChat[CHAT_TYPE_INFO_VALUE] = D3DXCOLOR(1.0f, 0.785f, 0.785f, 1.0f);
#endif
}

CPythonChat::CPythonChat()
{
	__Initialize();
}

CPythonChat::~CPythonChat()
{
	assert(m_ChatLineDeque.empty());
	assert(m_ShowingChatLineList.empty());
	assert(m_ChatSetMap.empty());
	assert(m_WhisperMap.empty());
}

///////////////////////////////////////////////////////////////////////////////////////////////////
CDynamicPool<CWhisper> CWhisper::ms_kPool;

CWhisper* CWhisper::New()
{
	return ms_kPool.Alloc();
}

void CWhisper::Delete(CWhisper* pkWhisper)
{
	pkWhisper->Destroy();
	ms_kPool.Free(pkWhisper);
}

void CWhisper::DestroySystem()
{
	ms_kPool.Destroy();

	SChatLine::DestroySystem();
}

void CWhisper::SetPosition(float fPosition)
{
	m_fcurPosition = fPosition;
#ifdef ENABLE_AUTO_TRANSLATE_WHISPER
	bool bAutoTranslate = CPythonSystem::Instance().IsAutoTranslateWhisper();
	if (bAutoTranslate)
		__ArrangeChat2();
	else
		__ArrangeChat();
#else
	__ArrangeChat();
#endif
}

void CWhisper::SetBoxSize(float fWidth, float fHeight)
{
	m_fWidth = fWidth;
	m_fHeight = fHeight;
	
	for (TChatLineDeque::iterator itor = m_ChatLineDeque.begin(); itor != m_ChatLineDeque.end(); ++itor) {
		TChatLine * pChatLine = *itor;
		pChatLine->Instance.SetLimitWidth(fWidth);
	}
	
#ifdef ENABLE_AUTO_TRANSLATE_WHISPER
	for (TChatLineDeque2::iterator itor = m_ChatLineDeque2.begin(); itor != m_ChatLineDeque2.end(); ++itor) {
		TChatLine * pChatLine = *itor;
		pChatLine->Instance.SetLimitWidth(fWidth);
	}
#endif
}

void CWhisper::AppendChat(int iType, const char * c_szChat
#ifdef ENABLE_AUTO_TRANSLATE_WHISPER
, bool bTranslated, int codepage
#endif
)
{
	// DEFAULT_FONT
	//static CResource * s_pResource = CResourceManager::Instance().GetResourcePointer(g_strDefaultFontName.c_str());

	CGraphicText* pkDefaultFont = static_cast<CGraphicText*>(DefaultFont_GetResource());
	if (!pkDefaultFont)
	{
		TraceError("CWhisper::AppendChat - CANNOT_FIND_DEFAULT_FONT");
		return;
	}
	// END_OF_DEFAULT_FONT

	SChatLine * pChatLine = SChatLine::New();
	pChatLine->Instance.SetValue(c_szChat
#ifdef ENABLE_AUTO_TRANSLATE_WHISPER
	, -1, codepage
#endif
	);

	// DEFAULT_FONT
	pChatLine->Instance.SetTextPointer(pkDefaultFont);
	// END_OF_DEFAULT_FONT

	pChatLine->Instance.SetLimitWidth(m_fWidth);
	pChatLine->Instance.SetMultiLine(TRUE);

	switch(iType)
	{
		case CPythonChat::WHISPER_TYPE_SYSTEM:
			pChatLine->Instance.SetColor(D3DXCOLOR(1.0f, 0.785f, 0.785f, 1.0f));
			break;
		case CPythonChat::WHISPER_TYPE_GM:
			pChatLine->Instance.SetColor(D3DXCOLOR(1.0f, 0.632f, 0.0f, 1.0f));
			break;
#if defined(BL_OFFLINE_MESSAGE)
		case CPythonChat::WHISPER_TYPE_OFFLINE:
			pChatLine->Instance.SetColor(0xffb9f2ff);
			break;
#endif
		case CPythonChat::WHISPER_TYPE_CHAT:
		default:
			pChatLine->Instance.SetColor(0xffffffff);
			break;
	}
	
#ifdef ENABLE_AUTO_TRANSLATE_WHISPER
	if (bTranslated) {
		m_ChatLineDeque2.push_back(pChatLine);
	} else {
		m_ChatLineDeque.push_back(pChatLine);
	}
	
	__ArrangeChat(bTranslated);
#else
	m_ChatLineDeque.push_back(pChatLine);
	__ArrangeChat();
#endif
}

void CWhisper::__ArrangeChat(
#ifdef ENABLE_AUTO_TRANSLATE_WHISPER
bool bTranslated
#endif
)
{
#ifdef ENABLE_AUTO_TRANSLATE_WHISPER
	if (bTranslated) {
		for (TChatLineDeque2::iterator itor = m_ChatLineDeque2.begin(); itor != m_ChatLineDeque2.end(); ++itor)
		{
			TChatLine * pChatLine = *itor;
			pChatLine->Instance.Update();
		}
	}
	else {
		for (TChatLineDeque::iterator itor = m_ChatLineDeque.begin(); itor != m_ChatLineDeque.end(); ++itor)
		{
			TChatLine * pChatLine = *itor;
			pChatLine->Instance.Update();
		}
	}
#else
	for (TChatLineDeque::iterator itor = m_ChatLineDeque.begin(); itor != m_ChatLineDeque.end(); ++itor)
	{
		TChatLine * pChatLine = *itor;
		pChatLine->Instance.Update();
	}
#endif
}

#ifdef ENABLE_AUTO_TRANSLATE_WHISPER
void CWhisper::__ArrangeChat2()
{
	bool bAutoTranslate = CPythonSystem::Instance().IsAutoTranslateWhisper();
	if (bAutoTranslate) {
		for (TChatLineDeque2::iterator itor = m_ChatLineDeque2.begin(); itor != m_ChatLineDeque2.end(); ++itor)
		{
			TChatLine * pChatLine = *itor;
			pChatLine->Instance.Update();
		}
	}
	else {
		for (TChatLineDeque::iterator itor = m_ChatLineDeque.begin(); itor != m_ChatLineDeque.end(); ++itor)
		{
			TChatLine * pChatLine = *itor;
			pChatLine->Instance.Update();
		}
	}
}
#endif

void CWhisper::Render(float fx, float fy)
{
#ifdef ENABLE_AUTO_TRANSLATE_WHISPER
	bool bAutoTranslate = CPythonSystem::Instance().IsAutoTranslateWhisper();
	if (bAutoTranslate) {
		float fHeight = fy + m_fHeight;

		int iViewCount = int(m_fHeight / m_fLineStep) - 1;
		int iLineCount = int(m_ChatLineDeque2.size());
		int iStartLine = -1;
		if (iLineCount > iViewCount)
		{
			iStartLine = int(float(iLineCount-iViewCount) * m_fcurPosition) + iViewCount - 1;
		}
		else if (!m_ChatLineDeque2.empty())
		{
			iStartLine = iLineCount - 1;
		}

		RECT Rect = { LONG(fx), LONG(fy), LONG(fx+m_fWidth), LONG(fy+m_fHeight) };

		for (int i = iStartLine; i >= 0; --i)
		{
			assert(i >= 0 && i < int(m_ChatLineDeque2.size()));
			TChatLine * pChatLine = m_ChatLineDeque2[i];

			WORD wLineCount = pChatLine->Instance.GetTextLineCount();
			fHeight -= wLineCount * m_fLineStep;

			pChatLine->Instance.SetPosition(fx, fHeight);
			pChatLine->Instance.Render(&Rect);

			if (fHeight < fy)
				break;
		}
	} else {
		float fHeight = fy + m_fHeight;

		int iViewCount = int(m_fHeight / m_fLineStep) - 1;
		int iLineCount = int(m_ChatLineDeque.size());
		int iStartLine = -1;
		if (iLineCount > iViewCount)
		{
			iStartLine = int(float(iLineCount-iViewCount) * m_fcurPosition) + iViewCount - 1;
		}
		else if (!m_ChatLineDeque.empty())
		{
			iStartLine = iLineCount - 1;
		}

		RECT Rect = { LONG(fx), LONG(fy), LONG(fx+m_fWidth), LONG(fy+m_fHeight) };

		for (int i = iStartLine; i >= 0; --i)
		{
			assert(i >= 0 && i < int(m_ChatLineDeque.size()));
			TChatLine * pChatLine = m_ChatLineDeque[i];

			WORD wLineCount = pChatLine->Instance.GetTextLineCount();
			fHeight -= wLineCount * m_fLineStep;

			pChatLine->Instance.SetPosition(fx, fHeight);
			pChatLine->Instance.Render(&Rect);

			if (fHeight < fy)
				break;
		}
	}
#else
	float fHeight = fy + m_fHeight;

	int iViewCount = int(m_fHeight / m_fLineStep) - 1;
	int iLineCount = int(m_ChatLineDeque.size());
	int iStartLine = -1;
	if (iLineCount > iViewCount)
	{
		iStartLine = int(float(iLineCount-iViewCount) * m_fcurPosition) + iViewCount - 1;
	}
	else if (!m_ChatLineDeque.empty())
	{
		iStartLine = iLineCount - 1;
	}

	RECT Rect = { LONG(fx), LONG(fy), LONG(fx+m_fWidth), LONG(fy+m_fHeight) };

	for (int i = iStartLine; i >= 0; --i)
	{
		assert(i >= 0 && i < int(m_ChatLineDeque.size()));
		TChatLine * pChatLine = m_ChatLineDeque[i];

		WORD wLineCount = pChatLine->Instance.GetTextLineCount();
		fHeight -= wLineCount * m_fLineStep;

		pChatLine->Instance.SetPosition(fx, fHeight);
		pChatLine->Instance.Render(&Rect);

		if (fHeight < fy)
			break;
	}
#endif
}

void CWhisper::__Initialize()
{
	m_fLineStep = 15.0f;
	m_fWidth = 300.0f;
	m_fHeight = 120.0f;
	m_fcurPosition = 1.0f;
}

void CWhisper::Destroy()
{
	std::for_each(m_ChatLineDeque.begin(), m_ChatLineDeque.end(), SChatLine::Delete);
	m_ChatLineDeque.clear();
#ifdef ENABLE_AUTO_TRANSLATE_WHISPER
	std::for_each(m_ChatLineDeque2.begin(), m_ChatLineDeque2.end(), SChatLine::Delete);
	m_ChatLineDeque2.clear();
#endif
	m_ShowingChatLineList.clear();
}

CWhisper::CWhisper()
{
	__Initialize();
}

CWhisper::~CWhisper()
{
	Destroy();
}

///////////////////////////////////////////////////////////////////////////////////////////////////

CDynamicPool<CWhisper::SChatLine> CWhisper::SChatLine::ms_kPool;

CWhisper::SChatLine* CWhisper::SChatLine::New()
{
	return ms_kPool.Alloc();
}

void CWhisper::SChatLine::Delete(CWhisper::SChatLine* pkChatLine)
{
	pkChatLine->Instance.Destroy();
	ms_kPool.Free(pkChatLine);
}

void CWhisper::SChatLine::DestroySystem()
{
	ms_kPool.Destroy();
}
