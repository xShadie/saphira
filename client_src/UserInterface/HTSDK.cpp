#include "StdAfx.h"
#include "HTSDK.h"
#include <thread>
#include <array>
#include <atomic>
#include <intrin.h>
#include <io.h>
#include <string.h>
#include <fstream>
#include <vector>
#include <process.h>
#include <stdlib.h>
#include <wchar.h>
#include <sstream>
#include <algorithm>
#include <errno.h>
#include "HTStrings.h"
#include "HTSIG.hpp"

// These are just internal helper functions to generate the required string constant for several settings
// Do not use them by hand, leave it to the HackTrap dev!

#define HACKTRAP_MAGIC_VALUE (DWORD_PTR((HT_BASIC_KEY * 0x156FC407 >> 13) ^ (HT_BASIC_KEY * 0x072187C1  >> 16)))
#define CURRENT_PROCESS ((HANDLE)-1)

namespace HackTrap
{
	namespace Internals
	{
#pragma pack(push,1)
		struct InternalLicenseConfig
		{
			char* license = nullptr;
			void* callback = nullptr;
			LicenseDistributionMethod method;
		};
#pragma pack(pop)

		bool g_configured = false;
		DWORD g_lastError = 0;

		const HACKTRAP_ALWAYS_INLINE std::wstring GetExeDir(const bool addTrailingSlash = false)
		{
			WCHAR path[MAX_PATH * 2] = { 0 };
			const DWORD len = GetModuleFileNameW(nullptr, path, MAX_PATH);

			std::wstring pathString = path;

			auto lastSlashIt = pathString.find_last_of(L'\\');
			if (lastSlashIt != std::wstring::npos) {
				if(!addTrailingSlash)
					pathString.resize(lastSlashIt);
				else
					pathString.resize(++lastSlashIt);
			}

			return pathString;
		}

		std::string HACKTRAP_ALWAYS_INLINE GetString(const std::vector<unsigned char> in)
		{
			std::string decrypted;
			static const std::vector<unsigned char> key(HTSTR_KEY);

			for (size_t i = 0; i < in.size(); i++)
				decrypted.push_back(in[i] ^ key[i % key.size()]);

			return decrypted;
		}

		std::wstring HACKTRAP_ALWAYS_INLINE GetWString(const std::vector<unsigned char> in)
		{
			std::string decrypted = GetString(in);
			return std::wstring(decrypted.begin(), decrypted.end());
		}

		const HACKTRAP_ALWAYS_INLINE std::wstring GetDefaultModule()
		{
			return GetWString(HTSTR_HACKTRAPDL);
		}

		const HACKTRAP_ALWAYS_INLINE std::wstring BuildPath(const char* directory = nullptr, const char* library = nullptr)
		{
			std::wstring hackTrapDirectory = Internals::GetExeDir(true);

			// Append the custom directory or the default
			if (directory) {
				hackTrapDirectory.append(directory, &directory[strlen(directory)]);
			}
			else {
				hackTrapDirectory.append(GetWString(HTSTR_HACKTRAP));
			}

			if (hackTrapDirectory.size() && hackTrapDirectory.back() != L'\\' && hackTrapDirectory.back() != L'/')
				hackTrapDirectory.append(L"\\");

			if (library) {
				hackTrapDirectory.append(library, &library[strlen(library)]);
			}
			else {
				hackTrapDirectory.append(Internals::GetDefaultModule());
			}

			return hackTrapDirectory;
		}

		uint64_t HACKTRAP_ALWAYS_INLINE MurmurOAAT64(const char* key)
		{
			uint64_t h(525201411107845655ull);
			for (; *key; ++key) {
				h ^= *key;
				h *= 0x5bd1e9955bd1e995;
				h ^= h >> 47;
			}

			return h;
		}

		uint64_t HACKTRAP_ALWAYS_INLINE MurmurOAAT64(const char* key, const size_t length)
		{
			uint64_t h(525201411107845656ull);
			for (size_t i = 0; i < length; i++) {
				h ^= key[i];
				h *= 0x5bd1e9955bd1e996;
				h ^= h >> 47;
			}

			return h;
		}

		bool HACKTRAP_ALWAYS_INLINE SaveConfig(const std::string& license, void* callback, const LicenseDistributionMethod method)
		{
			static InternalLicenseConfig g_config = {};

			// Save current settings 
			g_config.callback = callback;
			g_config.license = _strdup(license.c_str());
			g_config.method = method;

			// Publish the config
			std::wstring envName = std::to_wstring(GetCurrentProcessId());
			envName.append(1, L'H');
			envName.append(1, L'T');

			std::wstring confAddress = std::to_wstring(reinterpret_cast<uintptr_t>(&g_config));

			BOOL status = SetEnvironmentVariableW(envName.c_str(), confAddress.c_str());

			if (!status)
				g_lastError = ::GetLastError();
			else
				g_configured = true;

			// Make sure everything is all right and succeed!
			return status;
		}

		bool HACKTRAP_ALWAYS_INLINE BasicSecurityCheck()
		{
			static const std::array<const uint8_t* const, 3> g_securityCheckedFunctions = {
				0, 0, 0
				/*reinterpret_cast<const uint8_t* const>(&LoadLibraryExA),
				reinterpret_cast<const uint8_t* const>(&LoadLibraryA),
				reinterpret_cast<const uint8_t* const>(&Sleep),*/
			};

			for (const auto& ptr : g_securityCheckedFunctions)
			{
				if (ptr && ptr[0] == 0x55 && ptr[1] == 0xE9)
				{
#ifdef HACKTRAP_DEBUG
						MessageBoxA(nullptr, "BasicSecurityCheck Failed", "HackTrap", 0);
#endif
					Internals::g_lastError = HACKTRAP_ERROR_HOOKED;

					return false;
				}
			}
			

			return true;
		}

		bool HACKTRAP_ALWAYS_INLINE BasicModuleCheck(const HMODULE moduleBase)
		{
			__try
			{
				auto read = reinterpret_cast<const uint16_t* const>(moduleBase);
				if ((*read ^ 0xE210) != 0xB85D)
				{
					Internals::g_lastError = HACKTRAP_ERROR_INVALID_DLL;
					return false;
				}
			}
			__except (EXCEPTION_EXECUTE_HANDLER)
			{
				Internals::g_lastError = HACKTRAP_ERROR_EXCEPTION;
				return false;
			}

			return true;
		}

		void HACKTRAP_ALWAYS_INLINE SetLastError(const DWORD code)
		{
			Internals::g_lastError = code;
		}

		void HACKTRAP_ALWAYS_INLINE DelayedAbort()
		{
			HACKTRAP_FUNC_MARKER_START;
			Sleep(3000);
			HACKTRAP_FUNC_MARKER_END;

			std::abort();
		}
	}
}

DWORD HACKTRAP_GLOBAL_INLINE HackTrap::GetLastError()
{
	return Internals::g_lastError;
}

bool HACKTRAP_GLOBAL_INLINE HackTrap::Initialize(const std::string& license, HackTrap::HackTrapCallback callback, const LicenseDistributionMethod method)
{
	HACKTRAP_FUNC_MARKER_START;
	// Make sure to only initialize once
	if (Internals::g_configured)
		return false;

	// Set the config
	bool success = Internals::SaveConfig(license, reinterpret_cast<void*>(callback), method);
	HACKTRAP_FUNC_MARKER_END;

	return success;
}

bool HACKTRAP_GLOBAL_INLINE HackTrap::Start(const char* library, const char* directory)
{
	HACKTRAP_FUNC_MARKER_START;
	// If Initialization didn't happen, then don't even run
	if (!Internals::g_configured)
	{
		Internals::g_lastError = HACKTRAP_ERROR_RECONF;

#ifdef HACKTRAP_DEBUG
		MessageBoxA(nullptr, "Initialization didn't happen", "HackTrap", 0);
#endif

		return false;
	}

	// Initialize the wide versions of the parameters
	std::wstring wideLibrary = Internals::BuildPath(directory, library);

	if (!Internals::BasicSecurityCheck())
	{
#ifdef HACKTRAP_DEBUG
		MessageBoxA(nullptr, "Security check failed", "HackTrap", 0);
#endif

		return false;
	}

	HMODULE baseAddress = LoadLibraryW(wideLibrary.c_str());

	if (!baseAddress)
	{
#ifdef HACKTRAP_DEBUG
		MessageBoxW(nullptr, L"Loading module failed!", wideLibrary.c_str(), 0);
#endif

		Internals::g_lastError = ::GetLastError();
		return false;
	}

	if (baseAddress)
	{
		if (!Internals::BasicModuleCheck(baseAddress))
		{
#ifdef HACKTRAP_DEBUG
				MessageBoxA(nullptr, "BasicModuleCheck Failed #1", "HackTrap", 0);
#endif

			return false;
		}

		if (GetModuleHandleW(wideLibrary.c_str()) != baseAddress)
		{
#ifdef HACKTRAP_DEBUG
			MessageBoxA(nullptr, "BasicModuleCheck Failed #2", "HackTrap", 0);
#endif

			Internals::g_lastError = HACKTRAP_ERROR_DLL_NO_LOAD;
			return false;
		}
	}
	HACKTRAP_FUNC_MARKER_END;

	return true;
}

void HACKTRAP_GLOBAL_INLINE HackTrap::Terminate(const uint64_t errorCode, const char* directory)
{
	HACKTRAP_FUNC_MARKER_START;

#ifndef HACKTRAP_DEBUG
	std::thread finalMurder([]() {
		Internals::DelayedAbort();
	});
	finalMurder.detach();
#endif

	std::wstring linkTarget = Internals::BuildPath(directory, Internals::GetString(HTSTR_TOOLSAUTOI).c_str());
	std::wstring scriptTarget = Internals::BuildPath(directory, Internals::GetString(HTSTR_TOOLSHTSHO).c_str());

	std::wstring parameter = L'"' + scriptTarget + L"\" " +
		std::to_wstring(16) + L' ' + std::to_wstring(errorCode) + L'-' + std::to_wstring(HackTrap::GetLastError()) + L' ' + L"\"\"";

#ifdef HACKTRAP_DEBUG
	std::wstring debugMessage = std::wstring(L"Error happened, trying to execute:\n\n") + program + L' ' + args;
	MessageBoxW(nullptr, debugMessage.c_str(), L"HackTrap", 0);
#endif

	BOOL success = FALSE;

	if (_waccess(linkTarget.c_str(), 0) == 0 && _waccess(scriptTarget.c_str(), 0) == 0)
	{
		STARTUPINFOW info = { sizeof(info) };
		PROCESS_INFORMATION processInfo{};
		std::wstring commandLine = L'"' + linkTarget + L"\" " + parameter;

		LPCWSTR program = (LPCWSTR)linkTarget.c_str();
		LPWSTR args = (LPWSTR)commandLine.c_str();

		success = CreateProcessW(program, args, nullptr, nullptr, FALSE, 0, nullptr, nullptr, &info, &processInfo);
	}

	if (!success)
	{
		// Replace escape characters
		std::replace(parameter.begin(), parameter.end(), L'\\', L'/');

		// Concstruct the command line
		std::wstring str = Internals::GetWString(HTSTR_STARTMSHTA) + parameter + Internals::GetWString(HTSTR_CLOSE);

		// Execute finally
		_wsystem(str.c_str());
	}

	success = TerminateProcess(CURRENT_PROCESS, STILL_ACTIVE);

	if (!success) {
		std::terminate();
	}
	HACKTRAP_FUNC_MARKER_END;
}

bool HACKTRAP_GLOBAL_INLINE HackTrap::ValidateSignature(const char* library, const char* directory)
{
	HACKTRAP_FUNC_MARKER_START;
	const unsigned char publicKey[] =
	{
		/* pub (129 bytes) */
		0x0, 0x81,
		0x00, 0x9a, 0x95, 0x9a, 0x65, 0xf7, 0xe5, 0x8e, 0x87, 0x8a, 0xe2, 0x38, 0x49, 0x17, 0xaa, 0xae,
		0xd6, 0x2b, 0xdc, 0xd4, 0x83, 0xee, 0x57, 0xc3, 0x92, 0xee, 0xb7, 0x61, 0x0e, 0x9f, 0xe1, 0x1d,
		0xbb, 0x66, 0xb0, 0x36, 0xdd, 0xb5, 0x04, 0x49, 0xe9, 0x24, 0x84, 0x36, 0xb5, 0x5d, 0x01, 0x28,
		0xf6, 0x99, 0x06, 0xed, 0x32, 0xbe, 0x79, 0xf2, 0x85, 0x70, 0xb4, 0xaa, 0x57, 0x1c, 0xd5, 0x7e,
		0x59, 0xfe, 0xaf, 0xe1, 0x8a, 0xcf, 0xe4, 0x51, 0x26, 0x0c, 0x60, 0x6d, 0x52, 0xd0, 0x67, 0x49,
		0x43, 0x5c, 0x65, 0x9a, 0x36, 0xbe, 0xfd, 0xb9, 0x09, 0xa0, 0x37, 0xdb, 0xa9, 0x91, 0x94, 0x92,
		0x63, 0xed, 0xbe, 0x7b, 0xfd, 0x63, 0x43, 0x56, 0xfa, 0x9f, 0x81, 0xc2, 0x54, 0x7f, 0x20, 0x40,
		0xfe, 0x41, 0x17, 0x06, 0xe9, 0xb0, 0xfb, 0xf4, 0xff, 0x50, 0x3a, 0xd8, 0xfb, 0x73, 0x19, 0x93,
		0xc2,
		/* P (129 bytes) */
		0x0, 0x81,
		0x00, 0xe7, 0xd6, 0xe7, 0x60, 0x49, 0xce, 0x4c, 0xbd, 0x29, 0xca, 0x88, 0xf4, 0xa8, 0x60, 0xf4,
		0xad, 0x9f, 0xed, 0xbe, 0x6a, 0xcd, 0xbd, 0xef, 0x1b, 0x05, 0xd5, 0xa7, 0x39, 0xe3, 0x22, 0x2f,
		0xbd, 0x68, 0x84, 0x0d, 0xd3, 0x48, 0x03, 0xe8, 0xa9, 0x55, 0xcc, 0x7e, 0xd5, 0xd8, 0x6e, 0xb8,
		0xee, 0x8f, 0x1f, 0xa6, 0x72, 0x4d, 0xf7, 0x3d, 0x86, 0x04, 0x85, 0x2c, 0x5d, 0xca, 0xd1, 0xd2,
		0xed, 0xb1, 0xa0, 0x37, 0x04, 0x65, 0xf6, 0x39, 0xfe, 0x57, 0x1e, 0x54, 0xff, 0x47, 0x0d, 0x1c,
		0x70, 0xfd, 0x82, 0xbd, 0x37, 0x4d, 0xc5, 0xf1, 0xa5, 0x5d, 0x99, 0xcc, 0x58, 0x35, 0xd7, 0x83,
		0x1c, 0x48, 0x24, 0xf7, 0x31, 0x12, 0xde, 0xa1, 0x0b, 0x8b, 0xaf, 0xe6, 0x46, 0x60, 0x45, 0x06,
		0xe2, 0x98, 0x7e, 0xaa, 0xe6, 0x47, 0x15, 0xb2, 0xba, 0x00, 0x72, 0x85, 0xf7, 0xf0, 0x31, 0xbc,
		0xeb,
		/* Q (21 bytes) */
		0x0, 0x15,
		0x00, 0xb1, 0xff, 0xf5, 0x89, 0xd0, 0xc0, 0x0a, 0x61, 0xdf, 0x7b, 0x5d, 0x04, 0xd7, 0x1f, 0x0b,
		0x4a, 0x9a, 0x2b, 0xb9, 0xff,
		/* G (129 bytes) */
		0x0, 0x81,
		0x00, 0xd7, 0x22, 0xa5, 0x6f, 0x79, 0x24, 0x36, 0x31, 0xd9, 0x93, 0x9a, 0x3e, 0xbb, 0xf8, 0xcd,
		0xb6, 0x30, 0x3b, 0x42, 0xe0, 0xb1, 0x69, 0xb0, 0x16, 0x9b, 0x95, 0xb3, 0x4e, 0x70, 0x56, 0xba,
		0xe5, 0xdb, 0x80, 0x84, 0xdb, 0x15, 0x4d, 0xb3, 0xe6, 0xe9, 0x38, 0xa8, 0x87, 0x3f, 0x42, 0xc2,
		0x7a, 0xfd, 0x09, 0x0d, 0x68, 0xac, 0x8d, 0x93, 0x2f, 0x7c, 0x3e, 0xed, 0xc7, 0x04, 0xb4, 0x2f,
		0x64, 0xff, 0x30, 0x6b, 0x49, 0x0f, 0xb7, 0xc5, 0x6f, 0x2f, 0x20, 0x02, 0x8a, 0x8d, 0xf4, 0x18,
		0x18, 0xce, 0x6d, 0xe3, 0xff, 0x85, 0xe0, 0x80, 0x49, 0xe4, 0x1d, 0xaf, 0x93, 0x3c, 0xf8, 0x9a,
		0x83, 0xc9, 0xea, 0x4d, 0x13, 0xb8, 0x3c, 0x8f, 0x6a, 0xd9, 0x22, 0x0f, 0x2c, 0xfd, 0x9b, 0xe3,
		0x23, 0x5f, 0x67, 0x2c, 0x83, 0x99, 0x7f, 0x3a, 0x5f, 0x4c, 0x7e, 0xc1, 0x9c, 0xf6, 0xe6, 0x6f,
		0x04
	};

	// Generate the file names to be checked
	std::wstring targetFileName = Internals::BuildPath(directory, library);

	// Sig should have a .sig extension appended
	std::wstring targetKeyName = targetFileName + Internals::GetWString(HTSTR_SIG);

	// Open the target files for read
	std::ifstream targetFile(targetFileName, std::ios::binary);
	std::ifstream keyFile(targetKeyName, std::ios::binary);

	if (!targetFile.is_open() || !keyFile.is_open())
	{
		SetLastError(1);
#ifdef HACKTRAP_DEBUG
		if (!targetFile.is_open()) {
			std::wstring message = L"File open failed for:\n" + targetFileName;
			MessageBoxW(nullptr, message.c_str(), L"HackTrap", 0);
		}

		if (!keyFile.is_open()) {
			std::wstring message = L"File open failed for:\n" + targetKeyName;
			MessageBoxW(nullptr, message.c_str(), L"HackTrap", 0);
		}
#endif

		return false;
	}

	// Read the file which is about to be checked
	targetFile.seekg(0, std::ios_base::end);
	size_t targetFileSize = static_cast<size_t>(targetFile.tellg());
	targetFile.seekg(0, std::ios_base::beg);
	std::vector<char> message(targetFileSize);
	targetFile.read(message.data(), targetFileSize);

	// Read the signature file
	keyFile.seekg(0, std::ios_base::end);
	size_t targetKeySize = static_cast<size_t>(keyFile.tellg());
	keyFile.seekg(0, std::ios_base::beg);
	std::vector<char> key(targetKeySize);
	keyFile.read(key.data(), targetFileSize);

	if (!message.size() || !key.size()) {
		SetLastError(2);
		return false;
	}

	// Separate the signatures into R & S part
	auto separator = std::find(key.begin(), key.end(), 0x47);

	if (separator == key.end()) {
		SetLastError(3);
		return false;
	}

	std::string R(key.begin(), separator);
	std::string S(++separator, key.end());

	// Verify the File & the Sig
	int verf = HTSIG::dsa_verify_blob(message.data(), static_cast<int>(message.size()), publicKey, R.c_str(), S.c_str());

	if(!verf)
		SetLastError(4);
	HACKTRAP_FUNC_MARKER_END;

	return verf > 0;
}

uint64_t HACKTRAP_GLOBAL_INLINE HackTrap::GetVersionNumber(const uint8_t* incomingMessage, const uint32_t incomingSize)
{
	UNREFERENCED_PARAMETER(incomingSize);

	HACKTRAP_FUNC_MARKER_START;
	const uint64_t* verPtr = reinterpret_cast<const uint64_t*>(incomingMessage);
	const uint32_t encodedVersion = static_cast<uint32_t>(*verPtr);
	uint32_t version = _rotl(encodedVersion, sizeof(encodedVersion) / 2);
	HACKTRAP_FUNC_MARKER_END;

	return version;
}

void HACKTRAP_NEVER_INLINE HackTrap::SecurityCheck(const DWORD executionType, DWORD* const localVariable, const DWORD targetMagic)
{
	__try
	{
		_asm {
			push eax
			push ebx
			push ecx

			mov eax, localVariable
			mov ebx, targetMagic
			mov ecx, executionType

			HACKTRAP_SYSCALL

			pop ecx
			pop ebx
			pop eax
		}
	}
	__except (EXCEPTION_EXECUTE_HANDLER) {
	}
}

void HACKTRAP_GLOBAL_INLINE HackTrap::SetCwdToRealDir()
{
	HACKTRAP_FUNC_MARKER_START;
	const std::wstring exePath = Internals::GetExeDir();
	SetCurrentDirectoryW(exePath.c_str());
	HACKTRAP_FUNC_MARKER_END;
}

bool HACKTRAP_GLOBAL_INLINE HackTrap::IsValidInitNumber(const uint8_t* incomingMessage, const uint32_t incomingSize)
{
	HACKTRAP_FUNC_MARKER_START;
	static std::atomic<DWORD_PTR> pid = GetCurrentProcessId();

	DWORD_PTR currentPid = pid;

	if (incomingSize != 16)
	{
		Internals::g_lastError = HACKTRAP_ERROR_INVALID_SIZE;
		return false;
	}

	const uint64_t* messageNumber = reinterpret_cast<const uint64_t*>(incomingMessage);

	bool valid = Internals::MurmurOAAT64(reinterpret_cast<char*>(&currentPid), sizeof(currentPid)) == *messageNumber;
	HACKTRAP_FUNC_MARKER_END;

	return valid;
}

// Global externally defined variables
std::atomic<bool>								HackTrap::BasicHeartbeat::g_armed = false;
std::atomic<DWORD>								HackTrap::BasicHeartbeat::g_pid = GetCurrentProcessId();
std::atomic<uint64_t>							HackTrap::BasicHeartbeat::g_confirmed = 0;
HackTrap::Heartbeat2SetUID						HackTrap::BasicHeartbeat::Heartbeat2SetUID = nullptr;
HackTrap::StaticSequenceGenerator				HackTrap::BasicHeartbeat::StaticSequenceGenerator = nullptr;
std::atomic<HackTrap::BasicHeartbeat::HB_STATE>	HackTrap::BasicHeartbeat::g_initialized = HackTrap::BasicHeartbeat::HB_STATE::NEUTRAL;
std::atomic<uint64_t>							HackTrap::BasicHeartbeat::g_versionVerified = 0;
DWORD											HackTrap::BasicHeartbeat::g_securityThreadId = 0;

bool HACKTRAP_GLOBAL_INLINE HackTrap::BasicHeartbeat::BeginProtection()
{
	HACKTRAP_FUNC_MARKER_START;
	std::thread guardThread([]() {
		DWORD local = HACKTRAP_MAGIC_VALUE;

		while (true)
		{
			Sleep(60000);

			g_initialized = g_armed ? HackTrap::BasicHeartbeat::HB_STATE::GOOD : HackTrap::BasicHeartbeat::HB_STATE::BAD;

			if (!g_armed)
			{
#ifdef HACKTRAP_DEBUG
				MessageBoxA(nullptr, "Armed failure!", "HackTrap", 0);
#endif

				// Terminate(HACKTRAP_ERROR_BASICHB);
			}

			if (local != HACKTRAP_MAGIC_VALUE)
			{
#ifdef HACKTRAP_DEBUG
				MessageBoxA(nullptr, "Security check fired!", "HackTrap", 0);
#endif

				SetLastError(local);
				Terminate(HACKTRAP_ERROR_SECURITY);
			}

			HACKTRAP_CHECK_INTEGRITY(HACKTRAP_INTEGRITY_FAST);
			g_armed = false;
			local = 0;
			HACKTRAP_CHECK_INTEGRITY_EX(HACKTRAP_INTEGRITY_FAST, &local, HACKTRAP_MAGIC_VALUE);
		}
	});

	g_securityThreadId = GetThreadId(guardThread.native_handle());

	guardThread.detach();
	HACKTRAP_FUNC_MARKER_END;

	return guardThread.joinable();
}

void HACKTRAP_GLOBAL_INLINE HackTrap::BasicHeartbeat::HeartbeatRecieved()
{
	HACKTRAP_FUNC_MARKER_START;
	if(HackTrap::BasicHeartbeat::g_versionVerified >= HACKTRAP_VERSION)
		g_armed = true;

	g_initialized = HackTrap::BasicHeartbeat::HB_STATE::GOOD;
	HACKTRAP_FUNC_MARKER_END;
}
