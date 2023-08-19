#include "StdAfx.h"
#include <intrin.h>
#include <Iphlpapi.h>
#include <windows.h>
#include "HAntiMultipleFarm.h"
#include "PythonPlayer.h"
#include "PythonNetworkStream.h"

#pragma comment(lib, "iphlpapi.lib")

CAntiMultipleFarm::CAntiMultipleFarm() {}
CAntiMultipleFarm::~CAntiMultipleFarm() {};

auto CAntiMultipleFarm::GetClientMacAdress() -> const char *
{
	PIP_ADAPTER_INFO AdapterInfo;
	DWORD dwBufLen = sizeof(AdapterInfo);
	if (!(AdapterInfo = (IP_ADAPTER_INFO *)malloc(sizeof(IP_ADAPTER_INFO)))) return "";
	
	if (GetAdaptersInfo(AdapterInfo, &dwBufLen) == ERROR_BUFFER_OVERFLOW)
		if (!(AdapterInfo = (IP_ADAPTER_INFO *)malloc(dwBufLen))) return "";
	
	char* mac_addr = (char*)malloc(17);
	CHAR _MACFORMAT[] = {
		'%', '0', '2', 'X', ':', '%', '0', '2', 'X', ':',
		'%', '0', '2', 'X', ':', '%', '0', '2', 'X', ':',
		'%', '0', '2', 'X', ':', '%', '0', '2', 'X', 0x0
	};
	
	if (GetAdaptersInfo(AdapterInfo, &dwBufLen) == NO_ERROR) {
		PIP_ADAPTER_INFO pAdapterInfo = AdapterInfo;
		do {
			sprintf(mac_addr, _MACFORMAT,
				pAdapterInfo->Address[0], pAdapterInfo->Address[1],
				pAdapterInfo->Address[2], pAdapterInfo->Address[3],
				pAdapterInfo->Address[4], pAdapterInfo->Address[5]);
			return mac_addr;

			pAdapterInfo = pAdapterInfo->Next;
		} while (pAdapterInfo);
	}
	
	free(AdapterInfo);
	return NULL;
}

auto CAntiMultipleFarm::AddNewPlayerData(TAntiFarmPlayerInfo data) -> void
{
	if (!data.dwPID || !strlen(data.szName)) return;
	
	auto it = std::find_if(v_AntiFarmData.begin(), v_AntiFarmData.end(),
		[&](const TAntiFarmPlayerInfo& sR) { return sR.dwPID == data.dwPID; });
	
	if (it == v_AntiFarmData.end()) v_AntiFarmData.emplace_back(data);
	else it->bDropStatus = data.bDropStatus;
}

auto CAntiMultipleFarm::GetAntiFarmPlayerData(int idx, TAntiFarmPlayerInfo * sPInfo) -> bool
{
	if (GetAntiFarmPlayerCount() <= idx) return false;
	
	*sPInfo = v_AntiFarmData.at(idx);
	return true;
}

auto CAntiMultipleFarm::GetMainCharacterDropState() -> bool
{
	auto &rkPlayerInstance = CPythonPlayer::Instance();
	const char * cMainPlayerName = rkPlayerInstance.GetName();
	
	auto it = std::find_if(v_AntiFarmData.begin(), v_AntiFarmData.end(),
		[&](const TAntiFarmPlayerInfo & s1) { return (strcmp(s1.szName, cMainPlayerName) == 0); });
	
	if (it != v_AntiFarmData.end()) return it->bDropStatus;
	return false;
}

PyObject * methGetAntiFarmPlayerData(PyObject* poSelf, PyObject* poArgs)
{
	int iPlayerIdx;
	TAntiFarmPlayerInfo sPInfo;
	auto &rkAntiMFarm = CAntiMultipleFarm::instance();
	
	if (!PyTuple_GetInteger(poArgs, 0, &iPlayerIdx)) return Py_BuildException();
	if (!rkAntiMFarm.GetAntiFarmPlayerData(iPlayerIdx, &sPInfo)) return Py_BuildException();
	
	return Py_BuildValue("sii", sPInfo.szName, sPInfo.dwPID, (int)sPInfo.bDropStatus);
}

PyObject * methGetAntiFarmPlayerCount(PyObject* poSelf, PyObject* poArgs)
{
	auto &rkAntiMFarm = CAntiMultipleFarm::instance();
	return Py_BuildValue("i", rkAntiMFarm.GetAntiFarmPlayerCount());
}

PyObject * methGetAntiFarmMainPlayerStatus(PyObject* poSelf, PyObject* poArgs)
{
	auto &rkAntiMFarm = CAntiMultipleFarm::instance();
	return Py_BuildValue("i", (int)rkAntiMFarm.GetMainCharacterDropState());
}

PyObject * methSendAntiFarmStatus(PyObject* poSelf, PyObject* poArgs)
{
	auto & rkNetStream = CPythonNetworkStream::Instance();
	std::vector<DWORD> dwPIDs;
	
	for (uint8_t i = 0; i < PyTuple_Size(poArgs); ++i) {
		int dwPID;
		if (!PyTuple_GetInteger(poArgs, i, &dwPID)) return Py_BuildException();
		dwPIDs.emplace_back((DWORD)dwPID);
	}
	
	if (dwPIDs.size() != MULTIPLE_FARM_MAX_ACCOUNT) return Py_BuildException();
	
	rkNetStream.SendAntiFarmStatus(dwPIDs);
	return Py_BuildNone();
}

void initAntiMultipleFarmMethods()
{
	static PyMethodDef s_methods[] =
	{
		{ "GetAntiFarmPlayerCount",			methGetAntiFarmPlayerCount,					METH_VARARGS },
		{ "GetAntiFarmPlayerData",			methGetAntiFarmPlayerData,					METH_VARARGS },
		{ "GetAntiFarmMainPlayerStatus",	methGetAntiFarmMainPlayerStatus,			METH_VARARGS },
		
		{ "SendAntiFarmStatus",				methSendAntiFarmStatus,						METH_VARARGS },

		{ NULL,								NULL,										NULL },
	};
	
	PyObject * poModule = Py_InitModule("anti_multiple_farm", s_methods);
	PyModule_AddIntConstant(poModule, "MULTIPLE_FARM_MAX_ACCOUNT",	MULTIPLE_FARM_MAX_ACCOUNT);
}
