#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>
#include "../src/TarkovAPIManager.hpp"
#include "../src/LogHelper.hpp"

using namespace TarkovAPI;
static TarkovAPIManager* gs_apiMgr = nullptr;

TEST_CASE("Init test", "[multi-file:1]")
{
    gs_apiMgr = new TarkovAPIManager();
    REQUIRE(gs_apiMgr);

    try
    {
        auto ret = gs_apiMgr->InitializeTarkovAPIManager();
        REQUIRE(ret);
    }
    catch (const TarkovAPIException & ex)
    {
        gs_apiMgr->Log(__FUNCTION__, LL_SYS, fmt::format("TarkovAPIException - An exception handled! Data: {}", ex.details().c_str()));
    }
}

TEST_CASE("Destroy test", "[multi-file:1]")
{
    REQUIRE(gs_apiMgr);

    try
    {
        auto ret = gs_apiMgr->FinalizeTarkovAPIManager();
        REQUIRE(ret);
    }
    catch (const TarkovAPIException & ex)
    {
        gs_apiMgr->Log(__FUNCTION__, LL_SYS, fmt::format("TarkovAPIException - An exception handled! Data: {}", ex.details().c_str()));
    }

    delete gs_apiMgr;
    gs_apiMgr = nullptr;
}