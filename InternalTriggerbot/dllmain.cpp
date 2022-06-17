#include <iostream>
#include "mem.h"
#include "ent.h"

typedef ent* (__cdecl* tGetCrossHairEnt)();

tGetCrossHairEnt GetCrossHairEnt = nullptr;

DWORD WINAPI HackThread(HMODULE hModule)
{
    //Create Console
    AllocConsole();
    FILE* f;
    freopen_s(&f, "CONOUT$", "w", stdout);
    SetConsoleTitleA("Wake me up, turn me up, run it up till sun is up");
    std::cout << "Doing this shit I'm going insane\n\n";

    std::cout << "[F1] - Triggerbot\n";
    std::cout << "[F2] - Ammo Hack\n";
    std::cout << "[F3] - Health Hack\n";
    std::cout << "[F4] - No Recoil\n\n";

    std::cout << "[+] Build: 17 June\n";
    std::cout << "[+] Made by hj.#0010\n";

    uintptr_t moduleBase = (uintptr_t)GetModuleHandle(L"ac_client.exe");

    GetCrossHairEnt = (tGetCrossHairEnt)(moduleBase + 0x607c0);

    bool bTriggerbot = false, bRecoil = false, bAmmo = false, bHealth = false;

    ent* localPlayerPtr{ nullptr };
    localPlayerPtr = *(ent**)(moduleBase + 0x10F4F4);

    while (true)
    {
        if (GetAsyncKeyState(VK_END) & 1) //unloads cheat from process without crashing the game xD
        {
            break;
        }
        if (GetAsyncKeyState(VK_F1) & 1)
        {
            bTriggerbot = !bTriggerbot;
        }
        if (GetAsyncKeyState(VK_F2) & 1)
        {
            bAmmo = !bAmmo;
        }
        if (GetAsyncKeyState(VK_F3) & 1)
        {
            bHealth = !bHealth;
        }
        if (GetAsyncKeyState(VK_F4) & 1)
        {
            bRecoil = !bRecoil;

            if (bRecoil)
            {
                //nop
                mem::Nop((BYTE*)(moduleBase + 0x63786), 10);
            }
            else
            {
                //50 8D 4C 24 1C 51 8B CE FF D2
                mem::Patch((BYTE*)(moduleBase + 0x63786), (BYTE*)"\x50\x8D\x4C\x24\x1C\x51\x8B\xCE\xFF\xD2", 10);
            }
        }
            //Ammo code
            if (bAmmo)
            {
                uintptr_t ammoAddr = mem::FindDMAAddy(moduleBase + 0x10F4F4, { 0x374, 0x14, 0x0 });
                int* ammo = (int*)ammoAddr;
                *ammo = 1337;

                //Or This
                *(int*)mem::FindDMAAddy(moduleBase + 0x10F4F4, { 0x374, 0x14, 0x0 }) = 1337;
            }
            //Health code
            if (bHealth)
            {
                localPlayerPtr->health = 1337;
            }
            //Triggerbot Code
            if (bTriggerbot)
            {
                ent* crossHairEnt = GetCrossHairEnt();

                if (crossHairEnt)
                {
                    if (localPlayerPtr->team != crossHairEnt->team)
                    {
                        localPlayerPtr->bAttack = 1;
                    }
                }
                else
                {
                    localPlayerPtr->bAttack = 0;
                }
            }
        Sleep(5);
    }

    fclose(f);
    FreeConsole();
    FreeLibraryAndExitThread(hModule, 0);
    return 0;
}

BOOL APIENTRY DllMain(HMODULE hModule,
    DWORD  ul_reason_for_call,
    LPVOID lpReserved
)
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        CloseHandle(CreateThread(nullptr, 0, (LPTHREAD_START_ROUTINE)HackThread, hModule, 0, nullptr));
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}