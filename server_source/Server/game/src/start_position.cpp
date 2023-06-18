#include "stdafx.h"
#include "start_position.h"


#include "../../common/CommonDefines.h"


char g_nation_name[4][32] =
{
	"",
	"Shinsoo",
	"Chunjo",
	"Jinno",
};

#ifdef __ENABLE_CAPITALE_MAP__
long g_start_map[4] =
{
	0,	// reserved
	1,	// �ż���
	21,	// õ����
	41	// ���뱹
};

DWORD g_start_position[4][2] =
{
	{      0,      0 },	// reserved
	{ 469300, 964200 },	
	{  55700, 157900 },	
	{ 969600, 278400 }	
};

DWORD arena_return_position[4][2] =
{
	{      0,      0 },	// reserved
	{   347600, 882700  }, 
	{   138600, 236600  }, 
	{   857200, 251800  }  
};


DWORD g_create_position[4][2] =
{
	{      0,      0 },	// reserved
	{ 459800, 953900 },
	{ 52070, 166600 },
	{ 957300, 255200 },
};

DWORD g_create_position_canada[4][2] =
{
	{      0,      0 },	// reserved
	{ 457100, 946900 },
	{ 45700, 166500 },
	{ 966300, 288300 },
};
#else
long g_start_map[4] =
{
	0,	// reserved
	1,	// �ż���
	21,	// õ����
	41	// ���뱹
};

DWORD g_start_position[4][2] =
{
	{      0,      0 },	// reserved
	{ 311200, 336800 },	// �ż��� Rossi
	{ 516400, 336600 },	// õ���� gialli
	{ 413700, 337000 }	// ���뱹 Blu
};

DWORD arena_return_position[4][2] =
{
	{      0,      0 },	// reserved
	{ 311200, 336800 },	// �ż��� Rossi
	{ 516400, 336600 },	// õ���� gialli
	{ 413700, 337000 }	// ���뱹 Blu
};


DWORD g_create_position[4][2] =
{
	{      0,      0 },	// reserved
	{ 311200, 336800 },	// �ż��� Rossi
	{ 516400, 336600 },	// õ���� gialli
	{ 413700, 337000 }	// ���뱹 Blu
};

DWORD g_create_position_canada[4][2] =
{
	{      0,      0 },	// reserved
	{ 311200, 336800 },	// �ż��� Rossi
	{ 516400, 336600 },	// õ���� gialli
	{ 413700, 337000 }	// ���뱹 Blu
};
#endif

