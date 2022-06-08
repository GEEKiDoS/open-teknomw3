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

	WEAK symbol<dvar_t* (const char*, bool, unsigned int, const char*)> Dvar_RegisterBool{ 0x4A3300, 0 };
	WEAK symbol<dvar_t* (const char*, float, float, float, float, unsigned int, const char*)> Dvar_RegisterColor{ 0x44EB50, 0 };
	WEAK symbol<dvar_t* (const char*, const char* const*, int, unsigned int, const char*)> Dvar_RegisterEnum{ 0x4D9310, 0 };
	WEAK symbol<dvar_t* (const char*, float, float, float, unsigned int, const char*)> Dvar_RegisterFloat{ 0x4A5CF0, 0 };
	WEAK symbol<dvar_t* (const char*, int, unsigned int, unsigned int, unsigned int, const char*)> Dvar_RegisterInt{ 0x50C760, 0 };
	WEAK symbol<dvar_t* (const char*, const char*, unsigned int, const char*)> Dvar_RegisterString{ 0x4157E0, 0 };
	WEAK symbol<dvar_t* (const char*, float, float, float, float, unsigned int, const char*)> Dvar_RegisterVec2{ 0x48AFE0, 0 };
	WEAK symbol<dvar_t* (const char*, float, float, float, float, float, unsigned int, const char*)> Dvar_RegisterVec3{ 0x531DC0, 0 };
	WEAK symbol<dvar_t* (const char*, float, float, float, float, unsigned int, const char*)> Dvar_RegisterVec3Color{ 0x534CF0, 0 };
	WEAK symbol<dvar_t* (const char*, float, float, float, float, float, float, unsigned int, const char*)> Dvar_RegisterVec4{ 0x5125B0, 0 };

	WEAK symbol<void(void*, int, size_t)> Scr_RegisterFunction{ 0x5320D0, 0 };

	WEAK symbol<ScreenPlacement* ()> ScrPlace_GetViewPlacement{ 0x5346F0, 0 };
	WEAK symbol<Font_s* (const char* font)> R_RegisterFont{ 0x522510, 0x0 };
	WEAK symbol<void(float x, float y, float width, float height, float s0, float t0, float s1, float t1, float* color, Material* material)> 
		R_AddCmdDrawStretchPic{ 0x4EE060, 0 };
	WEAK symbol<Material* (const char* material)> Material_RegisterHandle{ 0x41B6B0, 0 };
	WEAK symbol<void(const char*, int, Font_s*, float, float, float, float, float, float*, int)> R_AddCmdDrawText{0x42C970, 0};
	WEAK symbol<int(const char* text, int maxChars, Font_s* font)> R_TextWidth{ 0x431750, 0 };
	WEAK symbol<void(const char*, int, Font_s*, float, float, float, float, float, const float*, int, int, char)> 
		R_AddCmdDrawTextWithCursor{ 0x50D110, 0 };
	WEAK symbol<const char* (dvar_t* dvar, DvarValue value)> Dvar_ValueToString{ 0x4327A0, 0 };

	WEAK symbol<XAssetHeader (XAssetType type, const char *name, bool allowCreateDefault)> DB_FindXAssetHeader{ 0x4B25C0, 0 };
	WEAK symbol<bool (XAssetType type, const char *name)> DB_IsXAssetDefault{ 0x4471F0, 0 };
	WEAK symbol<void (RawFile* rawfile, char* buf, int size)> DB_GetRawBuffer{ 0x49C920, 0 };

	WEAK symbol<void ()> GScr_LoadGameTypeScript{ 0x4CF510, 0 };
	WEAK symbol<void ()> Scr_LoadGameType{ 0x4DA3B0, 0 };
	WEAK symbol<bool (const char*)> Scr_LoadScript{ 0x482DD0, 0 };
	WEAK symbol<int (const char* filename, unsigned int str)> Scr_GetFunctionHandle{ 0x433150, 0 };
	WEAK symbol<int16_t(int, uint32_t)> Scr_ExecThread{ 0x40CAD0, 0 };
	WEAK symbol<void (int16_t)> Scr_FreeThread{ 0x538EF0, 0 };

	WEAK symbol<void (const char* gameName)> FS_Startup{ 0x4AF680, 0 };
	WEAK symbol<int (const char* qpath, char** buffer)> FS_ReadFile{ 0x533EB0, 0 };
	WEAK symbol<void(void* buffer)> FS_FreeFile{ 0x530EA0, 0 };

	WEAK symbol<XAssetEntry*(XAssetType type, XAssetHeader *header)> DB_LinkXAssetEntry1{ 0x5C8C60, 0 };
	WEAK symbol<char*(size_t len)> TempMalloc{ 0x51C0B0, 0 };
	WEAK symbol<char *(int)> DB_AllocStreamPos{ 0x46C620, 0 };
	WEAK symbol<void(size_t len)> DB_IncStreamPos{ 0x55DF70, 0 };
	WEAK symbol<void(bool atStreamStart, void* ptr, size_t size)> Load_Strem{ 0x4C53A0, 0 };

	WEAK symbol<gentity_s> g_entities{ 0x1A65EA8,0x0 };
	WEAK symbol<int> com_num_console_lines{ 0x1CE7728,0x0 };
	WEAK symbol<char*> com_console_lines{ 0x1CE61B8,0x0 };
	WEAK symbol<CmdArgs> cmd_args{ 0x1C96850,0x0 };
	WEAK symbol<CmdArgs> sv_cmd_args{ 0x1CA9918,0x0 };

	WEAK symbol<void*> DB_GetXAssetSizeHandlers{ 0x8AFB80,0x0 };
	WEAK symbol<void*> DB_XAssetPool{ 0x8B0080,0x0 };
	WEAK symbol<size_t> g_poolSize{ 0x8AFDA0,0x0 };

	WEAK symbol<void*> g_scr_func_table{ 0x1D6A364, 0x0 };

	WEAK symbol<int> keyCatchers{ 0x106020C, 0 };
	WEAK symbol<PlayerKeyState> playerKeys{0xB3A268, 0};

	WEAK symbol<cmd_function_s*> cmd_functions{ 0x1C968D8, 0 };

	WEAK symbol<int> dvarCount{ 0x59B3CA0, 0 };
	WEAK symbol<dvar_t*> sortedDvars{ 0x59B3CC8, 0 };

	WEAK symbol<char> programBuffer{ 0x20899DC, 0 };
}
