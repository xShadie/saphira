#pragma once
#define HACKTRAP_BASICHEARTBEAT_ENABLED
#include "HTSDK.h"
#include <array>
#include <thread>
#include <objbase.h>

#ifndef HACKTRAP_LIBRARY
	#define HACKTRAP_LIBRARY nullptr
#endif

#ifndef HACKTRAP_PRECALL
#ifdef CEF_BROWSER
#define HACKTRAP_PRECALL [](){ CefWebBrowser_Startup(GetModuleHandleW(nullptr)); }()
#else
#define HACKTRAP_PRECALL
#endif
#endif

#ifndef HACKTRAP_DIRECTORY
	#define HACKTRAP_DIRECTORY nullptr
#endif

#define HACKTRAP_MAGIC_VALUE ((HT_BASIC_KEY * 0x6ACD97A7 >> 13) ^ (HT_BASIC_KEY * 0x95B898D5 >> 16))

namespace HackTrapDefaultImplementation
{
#define HACKTRAP_ERROR_CBRECALL		40001
#define HACKTRAP_ERROR_CBINVALID	40002
#define HACKTRAP_ERROR_INIT			40003
#define HACKTRAP_ERROR_VERIFY		40004
#define HACKTRAP_ERROR_START		40005
#define HACKTRAP_ERROR_NOINIT		40006
#define HACKTRAP_ERROR_TIMEOUT		40007

	uint32_t CALLBACK DefaultCallback(const HackTrap::Command command, const uint8_t* const message, const uint32_t messageSize)
	{
		switch (command)
		{
		// HackTrap is initialized!
		case HackTrap::Command::INITIALIZE_CALLBACK:
			HACKTRAP_FUNC_MARKER_START;
			if (HackTrap::BasicHeartbeat::g_initialized == HackTrap::BasicHeartbeat::HB_STATE::GOOD)
			{
#ifdef HACKTRAP_DEBUG
				MessageBoxA(nullptr, "HackTrap::INITIALIZE_CALLBACK thrown error", "HackTrap", 0);
#endif
				HackTrap::Terminate(HACKTRAP_ERROR_CBRECALL, HACKTRAP_DIRECTORY);
			}
				

			if (!HackTrap::IsValidInitNumber(message, messageSize))
			{
#ifdef HACKTRAP_DEBUG
				MessageBoxA(nullptr, "HackTrap::IsValidInitNumber thrown error", "HackTrap", 0);
#endif

				HackTrap::Terminate(HACKTRAP_ERROR_CBINVALID, HACKTRAP_DIRECTORY);
			}

			HackTrap::BasicHeartbeat::HeartbeatRecieved();
			HACKTRAP_FUNC_MARKER_END;
			return TRUE;

		// Used to call Heartbeat v2 API
		case HackTrap::Command::BROADCAST_HEARTBEAT2_FUNCTION:
			HACKTRAP_FUNC_MARKER_START;
			HackTrap::BasicHeartbeat::Heartbeat2SetUID = HackTrap::GetFunction<HackTrap::Heartbeat2SetUID>(message, messageSize);
			HACKTRAP_FUNC_MARKER_END;
			return TRUE;
		
		// Used to call HeartbeatGenerator (soon to be deprecated)
		case HackTrap::Command::BROADCAST_HEARTBEAT_FUNCTION:
			HACKTRAP_FUNC_MARKER_START;
			HackTrap::BasicHeartbeat::StaticSequenceGenerator = HackTrap::GetFunction<HackTrap::StaticSequenceGenerator>(message, messageSize);

			{
				uint64_t generatedElement = 0;
				HackTrap::BasicHeartbeat::StaticSequenceGenerator(0, &generatedElement, true, HackTrap::BasicHeartbeat::g_pid);
				HackTrap::BasicHeartbeat::g_confirmed = generatedElement;
			}
			HACKTRAP_FUNC_MARKER_END;

			return TRUE;

		// Heartbeat v1 related APIs
		case HackTrap::Command::SEND_HEARTBEAT_MESSAGE:
			HACKTRAP_FUNC_MARKER_START;
			HackTrap::BasicHeartbeat::HeartbeatRecieved();
			HACKTRAP_FUNC_MARKER_END;
		case HackTrap::Command::CHECK_NEW_SEED:
			return TRUE;

		// HackTrap DLL version check
		case HackTrap::Command::VERSION_CHECK:
			HACKTRAP_FUNC_MARKER_START;
			HackTrap::BasicHeartbeat::g_versionVerified = HackTrap::GetVersionNumber(message, messageSize);
			HACKTRAP_FUNC_MARKER_END;
			if (HackTrap::BasicHeartbeat::g_versionVerified >= HACKTRAP_VERSION)
				return TRUE;
			else
				return FALSE;

		// HackTrap Database version check
		case HackTrap::Command::VERSION_CHECK_DATABASE:
			if (HackTrap::GetVersionNumber(message, messageSize) >= HACKTRAP_DATABASE_VERSION)
				return TRUE;
			else
				return FALSE;

		case HackTrap::Command::HARDWARE_IDENTIFIER:
			// HWID is stored currently in the message array

		// Hack/Suspicious detection found
		// Not that you don't HAVE TO do anything here
		// It's only used if you'd like to do EXTRA things apart from just simply showing error message and exit
		case HackTrap::Command::HACK_FOUND:
		case HackTrap::Command::SUSPICIOUS_FOUND:

		// Unknown
		default:
			return FALSE;
		}
		HACKTRAP_FUNC_MARKER_END;
	}

	static DWORD64 timer;

	void CALLBACK SecurityCheckCallback(HWINEVENTHOOK hook, DWORD event, HWND hwnd, LONG idObject, LONG idChild, DWORD dwEventThread, DWORD dwmsEventTime)
	{
		// Temporarily not checked
		// while (HackTrap::BasicHeartbeat::g_initialized != HackTrap::BasicHeartbeat::HB_STATE::GOOD)
		//	Sleep(delayer++);

		const DWORD64 now = GetTickCount64();

		if (!HackTrap::BasicHeartbeat::Heartbeat2SetUID || !HackTrap::BasicHeartbeat::StaticSequenceGenerator)
		{
			if (now - timer > 45000)
			{
#ifdef HACKTRAP_DEBUG
				MessageBoxA(nullptr, "HACKTRAP_INITIALIZED Failed #1", "HackTrap", 0);
#endif
				HACKTRAP_FUNC_MARKER_START;
				HackTrap::BasicHeartbeat::g_initialized = HackTrap::BasicHeartbeat::HB_STATE::BAD;
				HackTrap::Terminate(HACKTRAP_ERROR_TIMEOUT, HACKTRAP_DIRECTORY);
				HACKTRAP_FUNC_MARKER_END;
			}

			return;
		}

		if (now - timer > 30000)
		{
			timer = now;

			std::thread t1([]() {
				// Do an integrity check
				DWORD local = 0;
				HACKTRAP_CHECK_INTEGRITY_EX(HACKTRAP_INTEGRITY_SLOW, &local, HACKTRAP_MAGIC_VALUE);

				if (!HACKTRAP_INITIALIZED || local != HACKTRAP_MAGIC_VALUE) {
#ifdef HACKTRAP_DEBUG
					MessageBoxA(nullptr, "HACKTRAP_INITIALIZED Failed #2", "HackTrap", 0);
#endif

					HackTrap::BasicHeartbeat::g_initialized = HackTrap::BasicHeartbeat::HB_STATE::BAD;
					HackTrap::Terminate(HACKTRAP_ERROR_NOINIT, HACKTRAP_DIRECTORY);
				}
			});

			t1.detach();
		}
	}

	int HACKTRAP_ALWAYS_INLINE StartHackTrap()
	{
		HACKTRAP_FUNC_MARKER_START;
		HackTrap::SetCwdToRealDir();
		CoInitialize(nullptr);

#ifdef HACKTRAP_DEBUG
		MessageBoxA(nullptr, "This is a DEBUG enabled HackTrap SDK. Never release this to users.", "!!! WARNING !!!", 0);
#endif

		HACKTRAP_PRECALL;

		bool validated = HackTrap::ValidateSignature(HACKTRAP_LIBRARY, HACKTRAP_DIRECTORY);
		if (!validated)
		{
			// Some error occured. You can message the user or log into syserr or anything you want
			DWORD errorCode = HackTrap::GetLastError();

#ifdef HACKTRAP_DEBUG
			MessageBoxA(nullptr, "HackTrap::ValidateSignature thrown error", "HackTrap", 0);
#endif

			HackTrap::Terminate(HACKTRAP_ERROR_VERIFY, HACKTRAP_DIRECTORY);
			return 0;
		}

		bool initialized = HackTrap::Initialize(HACKTRAP_LICENSE, DefaultCallback);
		if (!initialized)
		{
			// Some error occured. You can message the user or log into syserr or anything you want
			DWORD errorCode = HackTrap::GetLastError();

#ifdef HACKTRAP_DEBUG
			MessageBoxA(nullptr, "HackTrap::Initialize thrown error", "HackTrap", 0);
#endif

			HackTrap::Terminate(HACKTRAP_ERROR_INIT, HACKTRAP_DIRECTORY);
			return 0;
		}

		bool started = HackTrap::Start(HACKTRAP_LIBRARY, HACKTRAP_DIRECTORY);
		if (!started)
		{
			// Some error occured. You can message the user or log into syserr or anything you want
			DWORD errorCode = HackTrap::GetLastError();

#ifdef HACKTRAP_DEBUG
			MessageBoxA(nullptr, "HackTrap::Start thrown error", "HackTrap", 0);
#endif

			HackTrap::Terminate(HACKTRAP_ERROR_START, HACKTRAP_DIRECTORY);
			return 0;
		}

		// Initialize client-only heartbeat if server-sided is not available
		HackTrap::BasicHeartbeat::BeginProtection();

		
		timer = GetTickCount64();
		HWINEVENTHOOK hid = SetWinEventHook(EVENT_OBJECT_CREATE, EVENT_MAX, nullptr, &SecurityCheckCallback, 0, 0, WINEVENT_OUTOFCONTEXT | WINEVENT_SKIPOWNPROCESS);

#ifdef HACKTRAP_DEBUG
		MessageBoxA(nullptr, "HackTrap Started successfully! Press ok to continue.", "HackTrap", 0);
#endif
		HACKTRAP_FUNC_MARKER_END;

		return hid != 0;
	}

	static volatile int skeletonInitializer = StartHackTrap();
}
