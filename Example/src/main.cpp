#include <random.hpp>
#include <Windows.h>
#include <iostream>

#include "../../TarkovAPI/test/Common.hpp"
#include "../../TarkovAPI/src/TarkovAPIManager.hpp"
#include "../../TarkovAPI/src/LogHelper.hpp"
using namespace TarkovAPI;

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    AllocConsole();
    freopen("CONOUT$", "a", stdout);
    freopen("CONIN$", "r", stdin);

    auto apiMgr = std::make_unique<TarkovAPIManager>();
    if (!apiMgr || !apiMgr.get() || !apiMgr->InitializeTarkovAPIManager())
    {
        std::cout << "Tarkov API initilization fail!" << std::endl;
        return EXIT_FAILURE;
    }

    try
    {
        apiMgr->Login(ACC_EMAIL, ACC_PWD, ACC_HWID);
    }
    catch (const json::exception & ex)
    {
        apiMgr->Log(__FUNCTION__, LL_SYS, fmt::format("json::exception - An exception handled! Data: {}", ex.what()));
    }
    catch (const TarkovAPIException & ex)
    {
        apiMgr->Log(__FUNCTION__, LL_SYS, fmt::format("An exception handled! Data: {}", ex.details().c_str()));
    }

    std::cin.get();

    apiMgr->FinalizeTarkovAPIManager();

    FreeConsole();
    return EXIT_SUCCESS;
}