#pragma once
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <thread>
#include "DllMain.h"
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")
//#include "App.h"

static void HP_ServerThread() {
    while (true)
    {
        ServerListen(&g_ser);
        ServerAccept(&g_ser);
        Sleep(1000);
    }
}

static void HP_ClientThread(Client* cl) {
    while (true)
    {
        if (!cl->mIsLoaded)
            break;
        auto update_err = cl->UpdateMessage();
        cl->CheckConnect(update_err);
        Sleep(100);
    }
}

void Entry() {
#ifdef _MAINDLL_QUICK
    SocketInit();
    std::string key = "127.0.0.1$10000";

    g_info = GetServerInfoByKeyStr(key);
    g_is_client = false;
    g_ser = Server();
    g_ser.Init(g_info.mPort);

    volatile std::thread* th = new std::thread(HP_ServerThread);

    _hp_Show_Tip("PvzHelper注入成功");

    DoMain();
    return;
#endif

    volatile HANDLE h_map = nullptr;
    SocketInit();

    while (true) {
        h_map = OpenFileMapping(
            FILE_MAP_ALL_ACCESS,  // read/write access
            FALSE,                // do not inherit the name
            L"__PVZ_HP_MAP__");
        if (h_map)
            break;
    }
    if (h_map)//防止编译器警告
    {
        g_lp_map = MapViewOfFile(h_map,  // handle to map object
            FILE_MAP_ALL_ACCESS, // read/write permission
            0,
            0,
            0);
        
        if (g_lp_map) 
        {
            std::string key;
            volatile _HP_S_Code code = 0;
            while (true)
            {
                code = *(int*)g_lp_map;
                if (code == 514|| code == 114)
                    break;
            }
            
            *(int*)g_lp_map = 0;
            key = (char*)(int(g_lp_map) + 8);
            if (code == CODE_SERVER) {
                g_info = GetServerInfoByKeyStr(key);
                g_is_client = false;
                g_ser = Server();
                g_ser.Init(g_info.mPort);
                new std::thread(HP_ServerThread);
            }
            else {
                g_info = GetServerInfoByKeyStr(key);
                g_is_client = true;
                g_cl = Client();
                g_cl.Init(g_info);
                if (g_cl.mToken <= 0)
                    return;
                new std::thread(HP_ClientThread,&g_cl);
            }

            _hp_Show_Tip("PvzHelper注入成功");

            DoMain();

            if (g_lp_map)
                UnmapViewOfFile((LPCVOID)g_lp_map);
        }
    }
}


BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    std::thread* tptr;
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        tptr = new std::thread(Entry);
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}



