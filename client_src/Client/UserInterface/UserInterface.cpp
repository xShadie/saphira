#include "StdAfx.h"
#include "PythonApplication.h"
#include "ProcessScanner.h"
#include "PythonExceptionSender.h"
#include "resource.h"
#include "Version.h"

#ifdef _DEBUG
#include <crtdbg.h>
#endif

#include "../eterPack/EterPackManager.h"
#include "../eterLib/Util.h"
#ifdef CEF_BROWSER
#include "CefWebBrowser.h"
#else
#include "../CWebBrowser/CWebBrowser.h"
#endif
#include "../eterBase/CPostIt.h"

#include "CheckLatestFiles.h"

#include "PythonConfig.h"
#ifdef ENABLE_AUTO_TRANSLATE_WHISPER
#include "CGoogleTranslate.h"
#endif
#include "ProcessCRC.h"
#ifdef ENABLE_HWID
#include "CHwidManager.h"
#endif

extern "C" {
extern int _fltused;
volatile int _AVOID_FLOATING_POINT_LIBRARY_BUG = _fltused;
};

extern "C" { FILE __iob_func[3] = { *stdin,*stdout,*stderr }; }

#pragma comment(linker, "/NODEFAULTLIB:libci.lib")

#pragma comment( lib, "version.lib" )

#ifdef _DEBUG
#pragma comment( lib, "python27_d.lib" )
#else
#pragma comment( lib, "python27.lib" )
#endif
#pragma comment( lib, "imagehlp.lib" )
#pragma comment( lib, "devil.lib" )
#pragma comment( lib, "granny2.lib" )
#pragma comment( lib, "mss32.lib" )
#pragma comment( lib, "winmm.lib" )
#pragma comment( lib, "imm32.lib" )
#pragma comment( lib, "oldnames.lib" )
#pragma comment( lib, "SpeedTreeRT.lib" )
#pragma comment( lib, "dinput8.lib" )
#pragma comment( lib, "dxguid.lib" )
#pragma comment( lib, "ws2_32.lib" )
#pragma comment( lib, "strmiids.lib" )
#pragma comment( lib, "ddraw.lib" )
#pragma comment( lib, "dmoguids.lib" )
#if defined(ENABLE_AUTO_TRANSLATE_WHISPER)
#pragma comment( lib, "libcurl_a.lib" )
#endif
#ifdef ENABLE_FOX_FS
#include <iphlpapi.h>

#pragma comment( lib, "iphlpapi.lib" )
#pragma comment( lib, "lz4.lib" )
#pragma comment( lib, "xxhash.lib" )
#pragma comment( lib, "FoxFS.lib" )
#endif

//#pragma comment( lib, "wsock32.lib" )
#include <stdlib.h>
#include <cryptopp/cryptoppLibLink.h>
#define HACKTRAP_LICENSE "dsaaaaa2aawaacaacsab2aanaaiaaaaaa6aaaaaaaafaagaaaaaaaaaaaeasaaiadsaaaaenanaaansaaaaaaaaaacacmyvbaaaaaaaaaaaaa5idaaagq25cnftyu2vvnj5yi2mxpp5yq4vdnt3hc3djnjsyk2mvppsyc3dvpfszgpdbnj3zg4mbqp3yc75dnfuyc65cgpvzi5tyn7vzn2tvpf7hc63uq75vg4mqqb4deqmjn65hi35tqa4ygp5in7xye7j3pb4gcn52pw6y4252qx4zc73wnw3gipvtqtwvs7mcg3vzk8mfpbzgi7vhpj6yn7vmpw4ygqj2gtxdu2t2nptve25dnp7gs45snf4zk7vgnp7gn6dpnpuy6254gi5hg3jyqpxvi6m4pxwza2m3nxugs3dprf2ge7bzgxxvg352qbtyqpduqx4di2jvpx6gq65fgxugn23un7wys8mvhfvyc4mun7uvn3vbq7svn6tygttgq4bxqx7he6bxnt4dspjvp23di2vqg73gq5vvqjxhg3mxrj6gqn52pw5zu7t2nfvve8b2hf3yw25jqs3dupbyqp4yg7mjn7xgcnvhrjwdep3wpp2gg35kpi6de7vhnf6gn2myhf4yypvtqx2hu33vnpvzn3v3pjyvk8mdq33zg43ypx6g6ntynf6ys2meqe5dk7jynpvdk8j2nn5hw3dsnx5yq45zqx4zc8dgq25dnnvkrbyyq6jxppszu6djrbzgs8bwgjxvg5vipa5zu3duqjwge6dxn32gy6txp3uvkpvihf2ge7jyqjyvg4mbqftg445wpftdun53g7wg44vzg35yuqjzpw5y66vjnpsyk4mwgn3ge65qq7vdn5vwgpxy44mqne3yq252qpsyk8vuqp3g46tvgtude3b3hfuvn5twqtyzi6v2nw5vgn5knpxzg2jzhb2g62tvnxwds33zgx7hk8dynttgq5vxrbwdn3tzpfszw4vqnfyvs35pqw5ve7bygtwvgnvinw4dg5vxqs5yy7vinfvyqn5tgs4yw7mgg3tgu7vig26g66vdq26y435xpb3gq8d2gi5yk3b3n23gypmfpjxvk5jzpa4y67vvpxuhe7vira5gu75iq7thcpdtqi4dg4dwhb3yg45wg3zhi7mbpfvhu6vepxxzg7dxqf3zu2mfgjwyspb3nxtvuqdqqp4hu4mhg73gi35kqbwhi3vepbxhi7v2qt2vknvwntwy66dzrb6gc75mp36yqqdvhfvzw45hpi6ze65hp36yi8vtqn4vg7vsnxxguqd2gp3g66jvqt3hw5v2gxsza35hnj5hcpdypb2gcnvzqtszqnvjppwdupbzqt4gwp5qpx4dq8jvre6vipmdq3uhu3j3qtsyy8mzhf2zu35epfthw6bvpptyentuntugi8mhpbtyq8dqn73gu8bxgt2hg45mg64gcp5qrf5vkpdvq32hw3mbpe6geqdqgw5dg5jzgtvy45typjuza72aaaaaaaaaaaaaaaaaueppcjxhvykrs48nu76wkccc2myesepmzg8qqwjfh66iyeuesnfsfm4g7qdrcy4naxpuwk4x8iiinfdwzg6ade3juehyecjtiab6zucbq3972c58yd8mby7ud5dai89smrwefk6qizts8r7t9hhwqbm8kvzawis98skrn7mn4zvxtn5m9p57z8znxqgdcvtnpaqbewqjigav49qbar3uyan6duj5uk7ndmsmykrymkcdkwmy2rreyczddhzjp39bt35ajwg8davrmdnp8ehh57rpcsumghyfqb7ku65hrnycn2avdcyfvif9zd5taeybjc38zeriybbcudvvnuis3f4etuucv8kthti2xd8rhyksngz2xgr97fjg936utvazks2ka89w49w35f7xjx2r56i6j2"
// #include "HTDefaultImplementation.h"

bool __IS_TEST_SERVER_MODE__=false;

// #define __USE_CYTHON__
#ifdef __USE_CYTHON__
// don't include these two files .h .cpp if you're implementing cython via .pyd
#include "PythonrootlibManager.h"
// it would be better including such file in the project, but this is easier at this moment:
//#include "PythonrootlibManager.cpp"
#endif

// #define __USE_EXTRA_CYTHON__
#ifdef __USE_EXTRA_CYTHON__
// don't include these two files .h .cpp if you're implementing cython via .pyd
#include "PythonuiscriptlibManager.h"
// it would be better including such file in the project, but this is easier at this moment:
//#include "PythonuiscriptlibManager.cpp"
#endif

// #define ENABLE_DAEMONPROTECTION

#ifdef ENABLE_DAEMONPROTECTION
#define DPDLL_FILENAME	"dpdll.dll"
#define DPDLL_CRC32		0x48104810
#define DPDLL_FILESIZE	1234567+4
#endif

extern bool SetDefaultCodePage(DWORD codePage);

#ifdef USE_OPENID
extern int openid_test;
#endif

static std::string JUST_INC_SIZE = "ADAKSJCNSMNMDSAADAKSJCNSMNMDSAADAKSJCNSMNMDSAADAKSJCNSMNMDSAADAKSJCNSMNMDSAADAKSJCNSMNMDSAADAKSJCNSMNMDSAADAKSJCNSMNMDSAADAKSJCNSMNMDSA";

static const char * sc_apszPythonLibraryFilenames[] =
{
	"UserDict.pyc",
	"__future__.pyc",
	"copy_reg.pyc",
	"linecache.pyc",
	"ntpath.pyc",
	"os.pyc",
	"site.pyc",
	"stat.pyc",
	"string.pyc",
	"traceback.pyc",
	"types.pyc",
	"\n",
};

#ifdef ENABLE_PYLIB_CHECK
#define PRINT_LEVEL 0
#define PRINTME(level, ...) if(PRINT_LEVEL>=level) TraceError(__VA_ARGS__);
#define PYFOLD "./lib"
// #define PYFORCE

typedef struct PyLibFiles_s
{
	std::string sFileName;
	size_t stSize;
	DWORD dwCRC32;
} PyLibFiles_t;

PyLibFiles_t PyLibFilesTable[] =
{
#if PY_VERSION_HEX==0x020706f0
	{ PYFOLD"/abc.pyc", 6187, 3834771731},
	{ PYFOLD"/bisect.pyc", 3236, 3116899751},
	{ PYFOLD"/codecs.pyc", 36978, 2928014693},
	{ PYFOLD"/collections.pyc", 26172, 385366131},
	{ PYFOLD"/copy.pyc", 13208, 1091298715},
	{ PYFOLD"/copy_reg.pyc", 5157, 536292604},
	{ PYFOLD"/encodings/aliases.pyc", 8803, 3888310600},
	{ PYFOLD"/encodings/cp949.pyc", 2009, 1824094431},
	{ PYFOLD"/encodings/__init__.pyc", 4510, 2926961588},
	{ PYFOLD"/fnmatch.pyc", 3732, 4270526278},
	{ PYFOLD"/functools.pyc", 6193, 3257285433},
	{ PYFOLD"/genericpath.pyc", 3303, 1652596334},
	{ PYFOLD"/heapq.pyc", 13896, 2948659214},
	{ PYFOLD"/keyword.pyc", 2169, 2178546341},
	{ PYFOLD"/linecache.pyc", 3235, 4048207604},
	{ PYFOLD"/locale.pyc", 49841, 4114662314},
	{ PYFOLD"/ntpath.pyc", 11961, 2765879465},
	{ PYFOLD"/os.pyc", 25769, 911432770},
	{ PYFOLD"/pyexpat.pyd", 127488, 2778492911},
	{ PYFOLD"/pyexpat_d.pyd", 194560, 2589182738},
	{ PYFOLD"/re.pyc", 13178, 1671609387},
	{ PYFOLD"/shutil.pyc", 19273, 1873281015},
	{ PYFOLD"/site.pyc", 20019, 3897044925},
	{ PYFOLD"/sre_compile.pyc", 11107, 1620746411},
	{ PYFOLD"/sre_constants.pyc", 6108, 3900811275},
	{ PYFOLD"/sre_parse.pyc", 19244, 1459430047},
	{ PYFOLD"/stat.pyc", 2791, 1375966108},
	{ PYFOLD"/string.pyc", 19656, 1066063587},
	{ PYFOLD"/sysconfig.pyc", 17571, 1529083148},
	{ PYFOLD"/traceback.pyc", 11703, 3768933732},
	{ PYFOLD"/types.pyc", 2530, 920695307},
	{ PYFOLD"/UserDict.pyc", 9000, 1431875928},
	{ PYFOLD"/warnings.pyc", 13232, 3752454002},
	{ PYFOLD"/weakref.pyc", 16037, 2124701469},
	{ PYFOLD"/xml/dom/domreg.pyc", 3506, 2127674645},
	{ PYFOLD"/xml/dom/expatbuilder.pyc", 36698, 316034696},
	{ PYFOLD"/xml/dom/minicompat.pyc", 4144, 747596376},
	{ PYFOLD"/xml/dom/minidom.pyc", 74704, 1543233763},
	{ PYFOLD"/xml/dom/nodefilter.pyc", 1243, 3805409468},
	{ PYFOLD"/xml/dom/xmlbuilder.pyc", 18659, 4118801318},
	{ PYFOLD"/xml/dom/__init__.pyc", 7337, 343751384},
	{ PYFOLD"/xml/parsers/expat.pyc", 326, 2425747752},
	{ PYFOLD"/xml/parsers/__init__.pyc", 353, 1691127318},
	{ PYFOLD"/xml/__init__.pyc", 1117, 3531597556},
	{ PYFOLD"/_abcoll.pyc", 22339, 2365844594},
	{ PYFOLD"/_locale.pyc", 49841, 4114662314},
	{ PYFOLD"/_weakrefset.pyc", 10490, 1576811346},
	{ PYFOLD"/__future__.pyc", 4431, 2857792867},
#elif PY_VERSION_HEX==0x020203f0
#else
#error "unknown python version"
#endif
};

bool checkPyLibDir(const std::string szDirName)
{
	bool HasHack = false;

	char szDirNamePath[MAX_PATH];
	sprintf(szDirNamePath, "%s\\*", szDirName.c_str());

	WIN32_FIND_DATA f;
	HANDLE h = FindFirstFile(szDirNamePath, &f);

	if (h == INVALID_HANDLE_VALUE) { return HasHack; }

	do
	{
		if (HasHack)
			break;
		const char * name = f.cFileName;

		if (strcmp(name, ".") == 0 || strcmp(name, "..") == 0) { continue; }

		if (f.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY)
		{
			char filePath[MAX_PATH];
			sprintf(filePath, "%s%s%s", szDirName.c_str(), "\\", name);
			PRINTME(1, "sub %s", filePath);
            checkPyLibDir(filePath);
		}
		else
		{
			// start processing file
			PRINTME(1, "starting %s", name);
			std::string sName(name);
			std::string sPathName(szDirName+"/"+name);
			// change \\ to /
			std::replace(sPathName.begin(), sPathName.end(), '\\', '/');
			PRINTME(1, "path %s", sPathName.c_str());
			// lower file name
			std::transform(sName.begin(), sName.end(), sName.begin(), ::tolower);
			{
				PRINTME(1, "verify %s", sName.c_str());
				bool isPyLibFound = false;
				for (PyLibFiles_t *i1=std::begin(PyLibFilesTable), *e1=std::end(PyLibFilesTable); i1<e1; i1++)
				{
					if (!i1->sFileName.compare(sPathName))
					{
						PRINTME(1, "found %s==%s", i1->sFileName.c_str(), sName.c_str());
						DWORD dwCrc32 = GetFileCRC32(sPathName.c_str());
						// assert(dwCrc32);
						DWORD dwFileSize = f.nFileSizeLow;
						if (i1->stSize!=dwFileSize)
						{
							PRINTME(1, "wrong size %u==%u", i1->stSize, dwFileSize);
							HasHack = true;
							PRINTME(0, "wrong size %u for %s", dwFileSize, sPathName.c_str());
							return HasHack;
						}
						else if (i1->dwCRC32 != dwCrc32)
						{
							PRINTME(1, "wrong crc32 %u==%u", i1->dwCRC32, dwCrc32);
							HasHack = true;
							PRINTME(0, "wrong crc32 %u for %s", dwCrc32, sPathName.c_str());
							return HasHack;
						}
						PRINTME(1, "right size %u==%u", i1->stSize, dwFileSize);
						PRINTME(1, "right crc32 %u==%u", i1->dwCRC32, dwCrc32);
						PRINTME(2, "{ \"%s\", %u, %u},", sPathName.c_str(), dwFileSize, dwCrc32);
						isPyLibFound = true;
						break;
					}
				}
				// block ambiguous pyc/d files
				if (!isPyLibFound)
				{
					PRINTME(1, "not found %s", sName.c_str());
#ifdef PYFORCE
					HasHack = true;
					PRINTME(0, "ambiguous file for %s", sPathName.c_str());
					return HasHack;
#endif
				}
				PRINTME(1, "skipping file(%s) hack(%u) found(%u)", sName.c_str(), HasHack, isPyLibFound);
			}
		}

	} while (FindNextFile(h, &f));
	FindClose(h);
	return HasHack;
}

bool __CheckPyLibFiles()
{
	PRINTME(1, "__CheckPyLibFiles processing "PYFOLD);
	if (checkPyLibDir(PYFOLD))
		return false;
	return true;
}
#endif

#ifdef ENABLE_MILES_CHECK
#include <algorithm>
#include "../EterBase/Filename.h"
#define PRINT_LEVEL 0
#define PRINTME(level, ...) if(PRINT_LEVEL>=level) TraceError(__VA_ARGS__);

typedef struct MilesFiles_s
{
	std::string sFileName;
	size_t stSize;
	DWORD dwCRC32;
} MilesFiles_t;

typedef struct MilesExten_s
{
	std::string ExtName;
	bool IsUni;
} MilesExten_t;

MilesExten_t MilesExtenTable[] = {
	{"dll", false},
	{"asi", true},
	{"flt", true},
	{"m3d", true},
	{"mix", true},
};

MilesFiles_t MilesFilesTable[] =
{
	{"mss32.dll", 349952, 4215862425},
	{"mssa3d.m3d", 83712, 3119483101},
	{"mssds3d.m3d", 70912, 3281351518},
	{"mssdsp.flt", 93952, 1708231078},
	{"mssdx7.m3d", 81152, 1952777504},
	{"msseax.m3d", 103680, 2275540199},
	{"mssmp3.asi", 126208, 2026369144},
	{"mssrsx.m3d", 355072, 2962657887},
	{"msssoft.m3d", 67328, 2604026672},
	{"mssvoice.asi", 197376, 1992264499},
};

#define MSGBOX(x) \
{ \
   std::ostringstream oss; \
   oss << x; \
   MessageBox(NULL, oss.str().c_str(), "Andra Global", MB_ICONSTOP); \
}

bool checkMilesDir(const std::string szDirName)
{
	bool HasHack = false;

	char szDirNamePath[MAX_PATH];
	sprintf(szDirNamePath, "%s\\*", szDirName.c_str());

	WIN32_FIND_DATA f;
	HANDLE h = FindFirstFile(szDirNamePath, &f);

	if (h == INVALID_HANDLE_VALUE) {
		return HasHack;
	}

	do
	{
		if (HasHack)
			break;

		const char * name = f.cFileName;
		if (strcmp(name, ".") == 0 || strcmp(name, "..") == 0) {
			continue;
		}

		if (f.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
		} else {
			std::string sName(name);
			std::string sPathName(szDirName+"/"+name);
			std::transform(sName.begin(), sName.end(), sName.begin(), ::tolower);
			std::string sNameExt = CFileNameHelper::GetExtension(sName);
			bool isMilesFile = false, isMilesUnique = false;
			for (MilesExten_t *i1=std::begin(MilesExtenTable), *e1=std::end(MilesExtenTable); i1<e1; i1++) {
				if (!sNameExt.compare(0, i1->ExtName.length(), i1->ExtName)) {
					isMilesFile = true;
					isMilesUnique = i1->IsUni;
				}
			}

			if (isMilesFile) {
				bool isMilesFound = false;
				for (MilesFiles_t *i1=std::begin(MilesFilesTable), *e1=std::end(MilesFilesTable); i1<e1; i1++) {
					if (!i1->sFileName.compare(sName)) {
						DWORD dwCrc32 = GetFileCRC32(sPathName.c_str());
						DWORD dwFileSize = f.nFileSizeLow;
						if (i1->stSize!=dwFileSize) {
							HasHack = true;
							MSGBOX("The file " << name << " was edited, please run the autopatcher!");
							return HasHack;
						} else if (i1->dwCRC32 != dwCrc32) {
							HasHack = true;
							MSGBOX("The file " << name << " was edited, please run the autopatcher!");
							return HasHack;
						}

						isMilesFound = true;
						break;
					}
				}

				if (!isMilesFound && isMilesUnique) {
					HasHack = true;
					MSGBOX("The file " << sPathName.c_str() << " is not allowed, please remove it!");
					return HasHack;
				}
			}
		}
	} while (FindNextFile(h, &f));

	FindClose(h);
	return HasHack;
}

bool __CheckMilesFiles()
{
	if (checkMilesDir(".") || checkMilesDir(".\\miles")) {
		return false;
	} else {
		return true;
	}
}
#endif

char gs_szErrorString[512] = "";

void ApplicationSetErrorString(const char* szErrorString)
{
	strcpy(gs_szErrorString, szErrorString);
}

bool CheckPythonLibraryFilenames()
{
	for (int i = 0; *sc_apszPythonLibraryFilenames[i] != '\n'; ++i)
	{
		std::string stFilename = "lib\\";
		stFilename += sc_apszPythonLibraryFilenames[i];

		if (_access(stFilename.c_str(), 0) != 0)
		{
			return false;
		}

		MoveFile(stFilename.c_str(), stFilename.c_str());
	}

	return true;
}

struct ApplicationStringTable
{
	HINSTANCE m_hInstance;
	std::map<DWORD, std::string> m_kMap_dwID_stLocale;
} gs_kAppStrTable;

void ApplicationStringTable_Initialize(HINSTANCE hInstance)
{
	gs_kAppStrTable.m_hInstance=hInstance;
}

const std::string& ApplicationStringTable_GetString(DWORD dwID, LPCSTR szKey)
{
	char szBuffer[512];
	char szIniFileName[256];
	char szLocale[256];

	::GetCurrentDirectory(sizeof(szIniFileName), szIniFileName);
	if(szIniFileName[lstrlen(szIniFileName)-1] != '\\')
		strcat(szIniFileName, "\\");
	strcat(szIniFileName, "metin2client.dat");

	strcpy(szLocale, LocaleService_GetLocalePath());
	if(strnicmp(szLocale, "locale/", strlen("locale/")) == 0)
		strcpy(szLocale, LocaleService_GetLocalePath() + strlen("locale/"));
	::GetPrivateProfileString(szLocale, szKey, NULL, szBuffer, sizeof(szBuffer)-1, szIniFileName);
	if(szBuffer[0] == '\0')
		LoadString(gs_kAppStrTable.m_hInstance, dwID, szBuffer, sizeof(szBuffer)-1);
	if(szBuffer[0] == '\0')
		::GetPrivateProfileString("en", szKey, NULL, szBuffer, sizeof(szBuffer)-1, szIniFileName);
	if(szBuffer[0] == '\0')
		strcpy(szBuffer, szKey);

	std::string& rstLocale=gs_kAppStrTable.m_kMap_dwID_stLocale[dwID];
	rstLocale=szBuffer;

	return rstLocale;
}

const std::string& ApplicationStringTable_GetString(DWORD dwID)
{
	char szBuffer[512];

	LoadString(gs_kAppStrTable.m_hInstance, dwID, szBuffer, sizeof(szBuffer)-1);
	std::string& rstLocale=gs_kAppStrTable.m_kMap_dwID_stLocale[dwID];
	rstLocale=szBuffer;

	return rstLocale;
}

const char* ApplicationStringTable_GetStringz(DWORD dwID, LPCSTR szKey)
{
	return ApplicationStringTable_GetString(dwID, szKey).c_str();
}

const char* ApplicationStringTable_GetStringz(DWORD dwID)
{
	return ApplicationStringTable_GetString(dwID).c_str();
}

////////////////////////////////////////////

int Setup(LPSTR lpCmdLine); // Internal function forward

bool PackInitialize(const char * c_pszFolder)
{
	NANOBEGIN

	// struct stat st;
	// if (stat( "D:\\ymir work", &st) == 0 ) {
		// MessageBox(NULL, "Please remove the folder D:\Ymir Work!", "Andra Global", MB_ICONSTOP);
		// return false;
	// }

	if (_access(c_pszFolder, 0) != 0)
		return false;

	std::string stFolder(c_pszFolder);
	stFolder += "/";

	CTextFileLoader::SetCacheMode();

#ifdef ENABLE_FOX_FS
	CEterPackManager::Instance().SetCacheMode();
	CEterPackManager::Instance().SetSearchMode(CEterPackManager::SEARCH_PACK);

	CSoundData::SetPackMode();
	CEterPackManager::Instance().RegisterPack("pack/bgm", "");
	CEterPackManager::Instance().RegisterPack("pack/effect", "");
	CEterPackManager::Instance().RegisterPack("pack/etc", "");
	CEterPackManager::Instance().RegisterPack("pack/guild", "");
	CEterPackManager::Instance().RegisterPack("pack/icon", "");
	CEterPackManager::Instance().RegisterPack("pack/locale", "");
	CEterPackManager::Instance().RegisterPack("pack/maps", "");
	CEterPackManager::Instance().RegisterPack("pack/patch_icons", "");
	CEterPackManager::Instance().RegisterPack("pack/patch_maps", "");
	CEterPackManager::Instance().RegisterPack("pack/patch_models", "");
	CEterPackManager::Instance().RegisterPack("pack/patch_models2", "");
	CEterPackManager::Instance().RegisterPack("pack/patch_systems", "");
	CEterPackManager::Instance().RegisterPack("pack/property", "");
	CEterPackManager::Instance().RegisterPack("pack/seasons", "");
	CEterPackManager::Instance().RegisterPack("pack/sound", "");
	CEterPackManager::Instance().RegisterPack("pack/terrain", "");
	CEterPackManager::Instance().RegisterPack("pack/textureset", "");
	CEterPackManager::Instance().RegisterPack("pack/tree", "");
	CEterPackManager::Instance().RegisterPack("pack/yw_models", "");
	CEterPackManager::Instance().RegisterPack("pack/zone", "");
	CEterPackManager::Instance().RegisterRootPack((stFolder + std::string("root")).c_str());
#else
#ifdef ENABLE_LITTLE_PACK
#if defined(USE_RELATIVE_PATH)
	CEterPackManager::Instance().SetRelativePathMode();
#endif
	CEterPackManager::Instance().SetCacheMode();
	CEterPackManager::Instance().SetSearchMode(true);

	CSoundData::SetPackMode();
	// CEterPackManager::Instance().RegisterPack("pack/effect", "d:/ymir work/effect/");
	// CEterPackManager::Instance().RegisterPack("pack/etc", "pack/");
	// CEterPackManager::Instance().RegisterPack("pack/guild", "d:/ymir work/guild/");
	// CEterPackManager::Instance().RegisterPack("pack/icon", "pack/");
	// CEterPackManager::Instance().RegisterPack("pack/locale", "pack/");
	// CEterPackManager::Instance().RegisterPack("pack/maps", "pack/");
	// CEterPackManager::Instance().RegisterPack("pack/patch_icons", "pack/");
	// CEterPackManager::Instance().RegisterPack("pack/patch_maps", "pack/");
	// CEterPackManager::Instance().RegisterPack("pack/patch_models", "pack/");
	// CEterPackManager::Instance().RegisterPack("pack/patch_systems", "pack/");
	// CEterPackManager::Instance().RegisterPack("pack/property", "pack/");
	// CEterPackManager::Instance().RegisterPack("pack/seasons", "pack/");
	// CEterPackManager::Instance().RegisterPack("pack/sound", "sound/");
	// CEterPackManager::Instance().RegisterPack("pack/terrain", "d:/ymir work/terrainmaps/");
	// CEterPackManager::Instance().RegisterPack("pack/textureset", "textureset");
	// CEterPackManager::Instance().RegisterPack("pack/tree", "d:/ymir work/tree/");
	// CEterPackManager::Instance().RegisterPack("pack/yw_models", "d:/ymir work/item/");
	// CEterPackManager::Instance().RegisterPack("pack/yw_models", "d:/ymir work/monster/");
	// CEterPackManager::Instance().RegisterPack("pack/yw_models", "d:/ymir work/monster2/");
	// CEterPackManager::Instance().RegisterPack("pack/yw_models", "d:/ymir work/npc/");
	// CEterPackManager::Instance().RegisterPack("pack/yw_models", "pack/");
	// CEterPackManager::Instance().RegisterPack("pack/yw_models", "d:/ymir work/pc/");
	// CEterPackManager::Instance().RegisterPack("pack/yw_models", "d:/ymir work/pc2/");
	// CEterPackManager::Instance().RegisterPack("pack/zone", "pack/");
	CEterPackManager::Instance().RegisterPack("pack/bgm", "");
	CEterPackManager::Instance().RegisterPack("pack/effect", "");
	CEterPackManager::Instance().RegisterPack("pack/etc", "");
	CEterPackManager::Instance().RegisterPack("pack/guild", "");
	CEterPackManager::Instance().RegisterPack("pack/icon", "");
	CEterPackManager::Instance().RegisterPack("pack/locale", "");
	CEterPackManager::Instance().RegisterPack("pack/maps", "");
	CEterPackManager::Instance().RegisterPack("pack/patch_icons", "");
	CEterPackManager::Instance().RegisterPack("pack/patch_maps", "");
	CEterPackManager::Instance().RegisterPack("pack/patch_models", "");
	CEterPackManager::Instance().RegisterPack("pack/patch_models2", "");
	CEterPackManager::Instance().RegisterPack("pack/patch_systems", "");
	CEterPackManager::Instance().RegisterPack("pack/property", "");
	CEterPackManager::Instance().RegisterPack("pack/seasons", "");
	CEterPackManager::Instance().RegisterPack("pack/sound", "");
	CEterPackManager::Instance().RegisterPack("pack/terrain", "");
	CEterPackManager::Instance().RegisterPack("pack/textureset", "");
	CEterPackManager::Instance().RegisterPack("pack/tree", "");
	CEterPackManager::Instance().RegisterPack("pack/yw_models", "");
	CEterPackManager::Instance().RegisterPack("pack/zone", "");
#else
	std::string stFileName(stFolder);
	stFileName += "Index";

	CMappedFile file;
	LPCVOID pvData;

	if (!file.Create(stFileName.c_str(), &pvData, 0, 0))
	{
		LogBoxf("FATAL ERROR! File not exist: %s", stFileName.c_str());
		TraceError("FATAL ERROR! File not exist: %s", stFileName.c_str());
		return true;
	}

	CMemoryTextFileLoader TextLoader;
	TextLoader.Bind(file.Size(), pvData);

	bool bPackFirst = TRUE;

	const std::string& strPackType = TextLoader.GetLineString(0);

	if (strPackType.compare("FILE") && strPackType.compare("PACK"))
	{
		TraceError("Pack/Index has invalid syntax. First line must be 'PACK' or 'FILE'");
		return false;
	}

#ifdef NDEBUG // @warme601 _DISTRIBUTE -> NDEBUG
	Tracef("Note: PackFirst mode enabled. [pack]\n");

	//if (0 == strPackType.compare("FILE"))
	//{
	//	bPackFirst = FALSE;
	//	Tracef("알림: 파일 모드입니다.\n");
	//}
	//else
	//{
	//	Tracef("알림: 팩 모드입니다.\n");
	//}
#else
	bPackFirst = FALSE;
	Tracef("Note: PackFirst mode not enabled. [file]\n");
#endif

#if defined(USE_RELATIVE_PATH)
	CEterPackManager::Instance().SetRelativePathMode();
#endif
	CEterPackManager::Instance().SetCacheMode();
	CEterPackManager::Instance().SetSearchMode(bPackFirst);

	CSoundData::SetPackMode(); // Miles 파일 콜백을 셋팅

	std::string strPackName, strTexCachePackName;
	for (DWORD i = 1; i < TextLoader.GetLineCount() - 1; i += 2)
	{
		const std::string & c_rstFolder = TextLoader.GetLineString(i);
		const std::string & c_rstName = TextLoader.GetLineString(i + 1);

		strPackName = stFolder + c_rstName;
		strTexCachePackName = strPackName + "_texcache";

		CEterPackManager::Instance().RegisterPack(strPackName.c_str(), c_rstFolder.c_str());
		CEterPackManager::Instance().RegisterPack(strTexCachePackName.c_str(), c_rstFolder.c_str());
	}
#endif
	CEterPackManager::Instance().RegisterRootPack((stFolder + std::string("root")).c_str());
#endif
	NANOEND
	return true;
}

bool RunMainScript(CPythonLauncher& pyLauncher, const char* lpCmdLine)
{
	initpack();
	initdbg();
	initime();
	initgrp();
	initgrpImage();
	initgrpText();
	initwndMgr();
	/////////////////////////////////////////////
	initudp();
	initapp();
	initsystemSetting();
	initchr();
	initchrmgr();
	initPlayer();
	initItem();
	initNonPlayer();
	initTrade();
	initChat();
	initTextTail();
	initnet();
	initMiniMap();
	initProfiler();
	initEvent();
	initeffect();
	initfly();
	initsnd();
	initeventmgr();
	initshop();
	initskill();
#ifdef NEW_PET_SYSTEM
	initskillpet();
#endif
	initquest();
	initBackground();
	initMessenger();
#ifdef ENABLE_ACCE_SYSTEM
	initAcce();
#endif

#ifdef ENABLE_CONFIG_MODULE
	initcfg();
#endif

	initsafebox();
	initguild();
	initServerStateChecker();
	initRenderTarget();
#ifdef ENABLE_SWITCHBOT
	initSwitchbot();
#endif
#ifdef ENABLE_CUBE_RENEWAL_WORLDARD
	intcuberenewal();
#endif
#ifdef INGAME_WIKI
	initWiki();
#endif
#ifdef __USE_CYTHON__
	// don't add this line if you're implementing cython via .pyd:
	initrootlibManager();
#endif
#ifdef __USE_EXTRA_CYTHON__
	// don't add this line if you're implementing cython via .pyd:
	inituiscriptlibManager();
#endif

#ifdef __ENABLE_NEW_OFFLINESHOP__
	initofflineshop();
#endif

	NANOBEGIN

    PyObject * builtins = PyImport_ImportModule("__builtin__");
#ifdef NDEBUG // @warme601 _DISTRIBUTE -> NDEBUG
	PyModule_AddIntConstant(builtins, "__DEBUG__", 1);
#else
	PyModule_AddIntConstant(builtins, "__DEBUG__", 0);
#endif
#ifdef __USE_CYTHON__
	PyModule_AddIntConstant(builtins, "__USE_CYTHON__", 1);
#else
	PyModule_AddIntConstant(builtins, "__USE_CYTHON__", 0);
#endif
#ifdef __USE_EXTRA_CYTHON__
	PyModule_AddIntConstant(builtins, "__USE_EXTRA_CYTHON__", 1);
#else
	PyModule_AddIntConstant(builtins, "__USE_EXTRA_CYTHON__", 0);
#endif

	// RegisterCommandLine
	{
		std::string stRegisterCmdLine;

		const char * loginMark = "-cs";
		const char * loginMark_NonEncode = "-ncs";
		const char * seperator = " ";

		std::string stCmdLine;
		const int CmdSize = 3;
		std::vector<std::string> stVec;
		SplitLine(lpCmdLine,seperator,&stVec);
		if (CmdSize == stVec.size() && stVec[0]==loginMark)
		{
			char buf[MAX_PATH];	//TODO 아래 함수 std::string 형태로 수정
			base64_decode(stVec[2].c_str(),buf);
			stVec[2] = buf;
			string_join(seperator,stVec,&stCmdLine);
		}
		else if (CmdSize <= stVec.size() && stVec[0]==loginMark_NonEncode)
		{
			stVec[0] = loginMark;
			string_join(" ",stVec,&stCmdLine);
		}
		else
			stCmdLine = lpCmdLine;

		PyModule_AddStringConstant(builtins, "__COMMAND_LINE__", stCmdLine.c_str());
	}
	{
		std::vector<std::string> stVec;
		SplitLine(lpCmdLine," " ,&stVec);

		if (stVec.size() != 0 && "--pause-before-create-window" == stVec[0])
		{
			system("pause");
		}
#ifdef ENABLE_DAEMONPROTECTION
		DWORD dwCrc32, dwSize;
		if (!((dwCrc32 = GetFileCRC32(DPDLL_FILENAME))==DPDLL_CRC32))
		{
			TraceError("dpdll wrong crc32 %d", dwCrc32);
			return false;
		}
		if (!((dwSize = GetFileSize(DPDLL_FILENAME))==DPDLL_FILESIZE))
		{
			TraceError("dpdll wrong size %d", dwSize);
			return false;
		}
		if (!LoadLibraryA(DPDLL_FILENAME))
		{
			TraceError("dpdll not loaded");
			return false;
		}
#endif

#ifdef __USE_CYTHON__
		if (!pyLauncher.RunLine("import rootlib\nrootlib.moduleImport('system')"))
#else
		if (!pyLauncher.RunFile("system.py"))
#endif
		{
			TraceError("RunMain Error");
			return false;
		}
	}

	NANOEND
	return true;
}

bool Main(HINSTANCE hInstance, LPSTR lpCmdLine)
{
#ifdef LOCALE_SERVICE_YMIR
	extern bool g_isScreenShotKey;
	g_isScreenShotKey = true;
#endif

	DWORD dwRandSeed=time(NULL)+DWORD(GetCurrentProcess());
	srandom(dwRandSeed);
	srand(random());

	SetLogLevel(1);

#ifdef LOCALE_SERVICE_VIETNAM_MILD
	extern BOOL USE_VIETNAM_CONVERT_WEAPON_VNUM;
	USE_VIETNAM_CONVERT_WEAPON_VNUM = true;
#endif

	if (_access("perf_game_update.txt", 0)==0)
	{
		DeleteFile("perf_game_update.txt");
	}

	if (_access("newpatch.exe", 0)==0)
	{
		system("patchupdater.exe");
		return false;
	}
#ifndef __VTUNE__
	ilInit();
#endif
	if (!Setup(lpCmdLine))
		return false;

#ifdef _DEBUG
	OpenConsoleWindow();
	OpenLogFile(true); // true == uses syserr.txt and log.txt
#else
	OpenLogFile(false); // false == uses syserr.txt only
#endif

	BuildProcessCRC();

	static CLZO							lzo;
	static CEterPackManager				EterPackManager;
#ifdef ENABLE_AUTO_TRANSLATE_WHISPER
	static CGoogleTranslate				google_translate;
#endif
#ifdef ENABLE_HWID
	static CHwidManager HwidManager;
#endif

	if (!PackInitialize("pack"))
	{
#ifdef ENABLE_LITTLE_PACK
		TraceError("Pack initialization failed.");
#else
		LogBox("Pack Initialization failed. Check log.txt file..");
#endif
		return false;
	}

#ifdef ENABLE_CONFIG_MODULE
	static CPythonConfig m_pyConfig;
	m_pyConfig.Initialize("config.cfg");
#endif

	if(LocaleService_LoadGlobal(hInstance))
		SetDefaultCodePage(LocaleService_GetCodePage());

#ifdef ENABLE_PYLIB_CHECK
	if (!__CheckPyLibFiles())
		return false;
#endif
#ifdef ENABLE_MILES_CHECK
	if (!__CheckMilesFiles())
		return false;
#endif

	CPythonApplication * app = new CPythonApplication;

	app->Initialize(hInstance);

	bool ret=false;
	{
		CPythonLauncher pyLauncher;
		CPythonExceptionSender pyExceptionSender;
		SetExceptionSender(&pyExceptionSender);

		if (pyLauncher.Create()) {
			ret=RunMainScript(pyLauncher, lpCmdLine);	//게임 실행중엔 함수가 끝나지 않는다.
		}

		//ProcessScanner_ReleaseQuitEvent();

		//게임 종료시.
		app->Clear();

		timeEndPeriod(1);
		pyLauncher.Clear();
	}

	app->Destroy();
	delete app;

	return ret;
}

HANDLE CreateMetin2GameMutex()
{
	SECURITY_ATTRIBUTES sa;
	ZeroMemory(&sa, sizeof(SECURITY_ATTRIBUTES));
	sa.nLength				= sizeof(sa);
	sa.lpSecurityDescriptor	= NULL;
	sa.bInheritHandle		= FALSE;

	return CreateMutex(&sa, FALSE, "Metin2GameMutex");
}

void DestroyMetin2GameMutex(HANDLE hMutex)
{
	if (hMutex)
	{
		ReleaseMutex(hMutex);
		hMutex = NULL;
	}
}

void __ErrorPythonLibraryIsNotExist()
{
	LogBoxf("FATAL ERROR!! Python Library file not exist!");
}

bool __IsTimeStampOption(LPSTR lpCmdLine)
{
	const char* TIMESTAMP = "/timestamp";
	return (strncmp(lpCmdLine, TIMESTAMP, strlen(TIMESTAMP))==0);
}

void __PrintTimeStamp()
{
#ifdef	_DEBUG
	if (__IS_TEST_SERVER_MODE__)
		LogBoxf("METIN2 BINARY TEST DEBUG VERSION %s  ( MS C++ %d Compiled )", __TIMESTAMP__, _MSC_VER);
	else
		LogBoxf("METIN2 BINARY DEBUG VERSION %s ( MS C++ %d Compiled )", __TIMESTAMP__, _MSC_VER);

#else
	if (__IS_TEST_SERVER_MODE__)
		LogBoxf("METIN2 BINARY TEST VERSION %s  ( MS C++ %d Compiled )", __TIMESTAMP__, _MSC_VER);
	else
		LogBoxf("METIN2 BINARY DISTRIBUTE VERSION %s ( MS C++ %d Compiled )", __TIMESTAMP__, _MSC_VER);
#endif
}

bool __IsLocaleOption(LPSTR lpCmdLine)
{
	return (strcmp(lpCmdLine, "--locale") == 0);
}

bool __IsLocaleVersion(LPSTR lpCmdLine)
{
	return (strcmp(lpCmdLine, "--perforce-revision") == 0);
}

#ifdef USE_OPENID
//2012.07.16 김용욱
//일본 OpenID 지원. 인증키 인자 추가
bool __IsOpenIDAuthKeyOption(LPSTR lpCmdLine)
{
	return (strcmp(lpCmdLine, "--openid-authkey") == 0);
}

bool __IsOpenIDTestOption(LPSTR lpCmdLine) //클라이언트에서 로그인이 가능하다.
{
	return (strcmp(lpCmdLine, "--openid-test") == 0);
}
#endif /* USE_OPENID */

#ifdef ENABLE_BLOCK_MULTIFARM
#include <stdio.h>
#ifdef _WIN32
#include <intrin.h>
#else
#include <cpuid.h>
#endif

int isHypervisor(void)
{
#ifdef _WIN32
	int cpuinfo[4];
	__cpuid(cpuinfo, 1);
	if (cpuinfo[2] >> 31 & 1)
		return 1;
#else
	unsigned int eax, ebx, ecx, edx;
	__get_cpuid (1, &eax, &ebx, &ecx, &edx);
	if (ecx >> 31 & 1)
		return 1;
#endif
	return 0;
}
#endif
//#ifdef ENABLE_BLOCK_MULTIFARM
//bool genMutex(int id)
//{
//    std::string mutex_name = "Andra Global";
//    mutex_name.push_back(id);
//    HANDLE Mutex = OpenMutexA(MUTEX_ALL_ACCESS, 1, mutex_name.c_str());
//    
//    if (!Mutex || WaitForSingleObject(Mutex,500) == WAIT_ABANDONED)
//    {
//        CreateMutexA(0, 1, mutex_name.c_str());
//        Sleep(INFINITY);//locks mutex
//        return true;
//    }
//    
//    return false;
//}
//#endif

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
//	if (strstr(lpCmdLine, "--hackshield") != 0)
//		return 0;
#ifdef ENTER_BY_AUTOPATCHER
	if (strstr(lpCmdLine, "-autopatch") == 0)
	{
		MessageBox(0, "You must enter through the updater", "Please initial by Andra_AutoPatch", MB_OK | MB_ICONERROR);
		return 0;
	}
#endif
//#ifdef ENABLE_BLOCK_MULTIFARM
//    bool ret = false;
//    for (int i = 0;i<3;i++)
//    {
//        ret = genMutex(i);
//        if (ret)
//            break;
//    }
//    if (!ret)
//    {
//        MessageBoxA(NULL, "You cannot farm with multiple accounts at the same time!", "Andra Global", MB_OK);
//        ExitProcess(0);
//    }
//#endif

#ifdef _DEBUG
	_CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_CHECK_CRT_DF | _CRTDBG_LEAK_CHECK_DF );
	//_CrtSetBreakAlloc( 110247 );
#endif

	ApplicationStringTable_Initialize(hInstance);
	LocaleService_LoadConfig("locale.cfg");
	SetDefaultCodePage(LocaleService_GetCodePage());

#if defined(CHECK_LATEST_DATA_FILES)
	if (!CheckLatestFiles())
		return 0;
#endif

	bool bQuit = false;
	bool bAuthKeyChecked = false;	//OpenID 버전에서 인증키가 들어왔는지 알기 위한 인자.
	int nArgc = 0;
	PCHAR* szArgv = CommandLineToArgv( lpCmdLine, &nArgc );

	for( int i=0; i < nArgc; i++ ) {
		if(szArgv[i] == 0)
			continue;
		if (__IsLocaleVersion(szArgv[i])) // #0000829: [M2EU] 버전 파일이 항상 생기지 않도록 수정
		{
			char szModuleName[MAX_PATH];
			char szVersionPath[MAX_PATH];
			GetModuleFileName(NULL, szModuleName, sizeof(szModuleName));
			sprintf(szVersionPath, "%s.version", szModuleName);
			FILE* fp = fopen(szVersionPath, "wt");
			if (fp)
			{
				extern int METIN2_GET_VERSION();
				fprintf(fp, "r%d\n", METIN2_GET_VERSION());
				fclose(fp);
			}
			bQuit = true;
		} else if (__IsLocaleOption(szArgv[i]))
		{
			FILE* fp=fopen("locale.txt", "wt");
			fprintf(fp, "service[%s] code_page[%d]",
				LocaleService_GetName(), LocaleService_GetCodePage());
			fclose(fp);
			bQuit = true;
		} else if (__IsTimeStampOption(szArgv[i]))
		{
			__PrintTimeStamp();
			bQuit = true;
		} else if ((strcmp(szArgv[i], "--force-set-locale") == 0))
		{
			// locale 설정엔 인자가 두 개 더 필요함 (로케일 명칭, 데이터 경로)
			if (nArgc <= i + 2)
			{
				MessageBox(NULL, "Invalid arguments", ApplicationStringTable_GetStringz(IDS_APP_NAME, "APP_NAME"), MB_ICONSTOP);
				goto Clean;
			}

			const char* localeName = szArgv[++i];
			const char* localePath = szArgv[++i];

			LocaleService_ForceSetLocale(localeName, localePath);
		}
#ifdef USE_OPENID
		else if (__IsOpenIDAuthKeyOption(szArgv[i]))	//2012.07.16 OpenID : 김용욱
		{
			// 인증키 설정엔 인자가 한 개 더 필요함 (인증키)
			if (nArgc <= i + 1)
			{
				MessageBox(NULL, "Invalid arguments", ApplicationStringTable_GetStringz(IDS_APP_NAME, "APP_NAME"), MB_ICONSTOP);
				goto Clean;
			}

			const char* authKey = szArgv[++i];

			//ongoing (2012.07.16)
			//인증키 저장하는 부분
			LocaleService_SetOpenIDAuthKey(authKey);

			bAuthKeyChecked = true;
		}
		else if (__IsOpenIDTestOption(szArgv[i]))
		{
			openid_test = 1;

		}
#endif /* USE_OPENID */
	}

#ifdef USE_OPENID
	//OpenID
	//OpenID 클라이언트의 경우인증키를 받아오지 않을 경우 (웹을 제외하고 실행 시) 클라이언트 종료.

	if (false == bAuthKeyChecked && !openid_test)
	{
		MessageBox(NULL, "Invalid execution", ApplicationStringTable_GetStringz(IDS_APP_NAME, "APP_NAME"), MB_ICONSTOP);
		goto Clean;
	}
#endif /* USE_OPENID */


	if(bQuit)
		goto Clean;

#if defined(NEEDED_COMMAND_ARGUMENT)
	// 옵션이 없으면 비정상 실행으로 간주, 프로그램 종료
	if (strstr(lpCmdLine, NEEDED_COMMAND_ARGUMENT) == 0) {
		MessageBox(NULL, ApplicationStringTable_GetStringz(IDS_ERR_MUST_LAUNCH_FROM_PATCHER, "ERR_MUST_LAUNCH_FROM_PATCHER"), ApplicationStringTable_GetStringz(IDS_APP_NAME, "APP_NAME"), MB_ICONSTOP);
			goto Clean;
	}
#endif

#if defined(NEEDED_COMMAND_CLIPBOARD)
	{
		CHAR szSecKey[256];
		CPostIt cPostIt( "VOLUME1" );

		if( cPostIt.Get( "SEC_KEY", szSecKey, sizeof(szSecKey) ) == FALSE ) {
			MessageBox(NULL, ApplicationStringTable_GetStringz(IDS_ERR_MUST_LAUNCH_FROM_PATCHER, "ERR_MUST_LAUNCH_FROM_PATCHER"), ApplicationStringTable_GetStringz(IDS_APP_NAME, "APP_NAME"), MB_ICONSTOP);
			goto Clean;
		}
		if( strstr(szSecKey, NEEDED_COMMAND_CLIPBOARD) == 0 ) {
			MessageBox(NULL, ApplicationStringTable_GetStringz(IDS_ERR_MUST_LAUNCH_FROM_PATCHER, "ERR_MUST_LAUNCH_FROM_PATCHER"), ApplicationStringTable_GetStringz(IDS_APP_NAME, "APP_NAME"), MB_ICONSTOP);
			goto Clean;
		}
		cPostIt.Empty();
	}
#endif

#ifdef CEF_BROWSER
	CefWebBrowser_Startup(hInstance);
#else
	WebBrowser_Startup(hInstance);
#endif

#ifndef ENABLE_PYLIB_CHECK
	if (!CheckPythonLibraryFilenames())
	{
		__ErrorPythonLibraryIsNotExist();
		goto Clean;
	}
#endif

	Main(hInstance, lpCmdLine);

#ifdef CEF_BROWSER
	CefWebBrowser_Cleanup();
#else
	WebBrowser_Cleanup();
#endif


	::CoUninitialize();

	if(gs_szErrorString[0])
		MessageBox(NULL, gs_szErrorString, ApplicationStringTable_GetStringz(IDS_APP_NAME, "APP_NAME"), MB_ICONSTOP);

Clean:
	SAFE_FREE_GLOBAL(szArgv);

	return 0;
}

static void GrannyError(granny_log_message_type Type,
						granny_log_message_origin Origin,
						char const* File,
						granny_int32x Line,
						char const *Error,
						void *UserData)
{
    /*TraceError("GRANNY: %s", Error);*/
}

int Setup(LPSTR lpCmdLine)
{
	/*
	 *	타이머 정밀도를 올린다.
	 */
	TIMECAPS tc;
	UINT wTimerRes;

	if (timeGetDevCaps(&tc, sizeof(TIMECAPS)) != TIMERR_NOERROR)
		return 0;

	wTimerRes = MINMAX(tc.wPeriodMin, 1, tc.wPeriodMax);
	timeBeginPeriod(wTimerRes);

	/*
	 *	그래니 에러 핸들링
	 */

/*	granny_log_callback Callback;
    Callback.Function = GrannyError;
    Callback.UserData = 0;
    GrannySetLogCallback(&Callback);
*/
	return 1;
}
