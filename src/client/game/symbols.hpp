#pragma once

#define WEAK __declspec(selectany)

namespace game
{
	WEAK symbol<int(int)> Live_SyncOnlineDataFlags{ 0x43F760, 0x0 };
	WEAK symbol<bool()> Sys_IsDatabaseReady2{ 0x55EB90, 0x0 };
	WEAK symbol<void()> Com_Frame_Try_Block_Function{ 0x4E46A0, 0x0 };
	WEAK symbol<void()> G_Glass_Update{ 0x55C550, 0x0 };
	WEAK symbol<DWORD()> Sys_Milliseconds{ 0x4BB3E0, 0x0 };
	WEAK symbol<void(errorParm_t, const char*, ...)> Com_Error{ 0x4A6660, 0x0 };
	WEAK symbol<void()> Sys_ShowConsole{ 0x51B770, 0x0 };
	WEAK symbol<void(const char*)> Conbuf_AppendText{ 0x4F7300, 0x0 };
	WEAK symbol<void(const char*)> Cmd_TokenizeString{ 0x414770, 0x0 };
	WEAK symbol<void()> Cmd_EndTokenizeString{ 0x4D9A30, 0x0 };
	WEAK symbol<void(const char*, const char*)> Dvar_SetCommand{ 0x440FA0, 0x0 };
	WEAK symbol<void(const char*, void(), cmd_function_s*)> Cmd_AddCommandInternal{ 0x537E70, 0x0 };
	WEAK symbol<void(int, svscmd_type, const char*)> SV_GameSendServerCommand{ 0x4EF450, 0x0 };
	WEAK symbol<void(int, int, const char*)> Cmd_ExecuteSingleCommand{ 0x4EB8F0, 0x0 };
	WEAK symbol<void(int, const char*)> Cbuf_AddText{ 0x4C1030, 0x0 };
	WEAK symbol<void()> Com_Quit_f{ 0x556060, 0x0 };
	WEAK symbol<dvar_t* (const char*)> Dvar_FindVar{ 0x4EBB50, 0 };

	WEAK symbol<dvar_t* (const char*, bool, unsigned int, const char*)>Dvar_RegisterBool{ 0x4A3300, 0 };
	WEAK symbol<dvar_t* (const char*, float, float, float, float, unsigned int, const char*)>Dvar_RegisterColor{ 0x44EB50, 0 };
	WEAK symbol<dvar_t* (const char*, const char* const*, int, unsigned int, const char*)>Dvar_RegisterEnum{ 0x4D9310, 0 };
	WEAK symbol<dvar_t* (const char*, float, float, float, unsigned int, const char*)>Dvar_RegisterFloat{ 0x4A5CF0, 0 };
	WEAK symbol<dvar_t* (const char*, int, unsigned int, unsigned int, unsigned int, const char*)>Dvar_RegisterInt{ 0x50C760, 0 };
	WEAK symbol<dvar_t* (const char*, const char*, unsigned int, const char*)>Dvar_RegisterString{ 0x4157E0, 0 };
	WEAK symbol<dvar_t* (const char*, float, float, float, float, unsigned int, const char*)> Dvar_RegisterVec2{ 0x48AFE0, 0 };
	WEAK symbol<dvar_t* (const char*, float, float, float, float, float, unsigned int, const char*)>Dvar_RegisterVec3{ 0x531DC0, 0 };
	WEAK symbol<dvar_t* (const char*, float, float, float, float, unsigned int, const char*)>Dvar_RegisterVec3Color{ 0x534CF0, 0 };
	WEAK symbol<dvar_t* (const char*, float, float, float, float, float, float, unsigned int, const char*)>Dvar_RegisterVec4{ 0x5125B0, 0 };


	WEAK symbol<gentity_s> g_entities{ 0x1A65EA8,0x0 };
	WEAK symbol<int> com_num_console_lines{ 0x1CE7728,0x0 };
	WEAK symbol<char*> com_console_lines{ 0x1CE61B8,0x0 };
	WEAK symbol<CmdArgs> cmd_args{ 0x1C96850,0x0 };
	WEAK symbol<CmdArgs> sv_cmd_args{ 0x1CA9918,0x0 };
}
