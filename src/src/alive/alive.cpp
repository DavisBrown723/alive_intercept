#include <Windows.h>
#include <stdio.h>
#include <cstdint>
#include <sstream>

#include "intercept.hpp"
#include "core\core.hpp"

#include <random>
#include <thread>
#include <chrono>
#include <sstream>



// required exported function to return API version

int __cdecl intercept::api_version() {
    return 1;
}



void __cdecl intercept::pre_start() {
    alive::Core::get().onPreStart();
}

void __cdecl intercept::pre_init() {
    alive::Core::get().onPreInit();
}

void __cdecl intercept::post_init() {
    alive::Core::get().onPostInit();
}

void __cdecl intercept::on_frame() {
    alive::Core::get().onSimulationStep();
}







// Normal Windows DLL junk...
BOOL APIENTRY DllMain(HMODULE hModule,
                      DWORD  ul_reason_for_call,
                      LPVOID lpReserved
)
{
    switch (ul_reason_for_call)
    {
        case DLL_PROCESS_ATTACH:
            break;
        case DLL_THREAD_ATTACH:
        case DLL_THREAD_DETACH:
        case DLL_PROCESS_DETACH:
            break;
    }
    return TRUE;
}
