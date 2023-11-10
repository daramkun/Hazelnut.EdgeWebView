#if WINDOWS

#include <Windows.h>

BOOL WINAPI DllMain(HINSTANCE hInstDll, DWORD fdwReason, LPVOID lpvReserved) {
    switch (fdwReason) {
        case DLL_PROCESS_ATTACH:
            break;

        case DLL_THREAD_ATTACH:
            break;

        case DLL_THREAD_DETACH:
            break;

        case DLL_PROCESS_DETACH:
            if (lpvReserved != nullptr) {
                break;
            }

            break;

        default:
            return FALSE;
    }

    return TRUE;
}

#endif