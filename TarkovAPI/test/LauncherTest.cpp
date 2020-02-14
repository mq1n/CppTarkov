#include <random.hpp>
#include <catch2/catch.hpp>
#include <json.hpp>

#include "Common.hpp"
#include "../src/Auth.hpp"
#include "../src/Exception.hpp"
#include "../src/Hwid.hpp"

using namespace TarkovAPI;
using json = nlohmann::json;

TEST_CASE( "Basic auth test with random hwid", "[multi-file:3][!hide]" ) // Hide than test list due than require input for 2FA
{
    auto apiMgr = new TarkovAPIManager();
    REQUIRE( apiMgr );

    try
    {
        auto ret = apiMgr->InitializeTarkovAPIManager();
        REQUIRE( ret );

        auto client = new cpr::Session();
        REQUIRE( client );

        auto hwid = hwid::generate_hwid();
        REQUIRE( hwid.size() == 258 );

        auto user = auth::LoginImpl(client, ACC_EMAIL, ACC_PWD, "", hwid);
        REQUIRE( user.size() );

        json user_ctx{};
        try
        {
            user_ctx = json::parse(user.data());
        }
        catch (const json::parse_error& ex)
        {
            std::stringstream ss;
            ss << "JSON data(user) could not parsed: Error:" << "Message: " << ex.what() << '\n' << "exception id: " << ex.id << '\n' << "byte position of error: " << ex.byte << std::endl;

            FAIL(ss.str());
        }

        if (!user_ctx.contains("access_token"))
        {
            FAIL("'access_token' key is not available");
        }

        auto session = auth::ExchangeAccessToken(client, user_ctx["access_token"].get<std::string>(), hwid);
        if (session.empty())
        {
            FAIL("Null json data(session)");
        }

        json session_ctx{};
        try
        {
            session_ctx = json::parse(session.data());
        }
        catch (const json::parse_error& ex)
        {
            std::stringstream ss;
            ss << "JSON data(session) could not parsed: Error:" << "Message: " << ex.what() << '\n' << "exception id: " << ex.id << '\n' << "byte position of error: " << ex.byte << std::endl;

            FAIL(ss.str());
        }
        if (!session_ctx.contains("session"))
        {
            FAIL("'session' key is not available");
        }

        ret = apiMgr->FinalizeTarkovAPIManager();
        REQUIRE( ret );
    }
    catch (const TarkovAPIException& ex)
    {
        apiMgr->Log(__FUNCTION__, LL_SYS, fmt::format("TarkovAPIException - An exception handled! Data: {}", ex.details().c_str()));
    }
    
    delete apiMgr;	
	apiMgr = nullptr;
}

TEST_CASE( "Basic auth test with known hwid", "[multi-file:3]" )
{
    auto apiMgr = new TarkovAPIManager();
    REQUIRE( apiMgr );

    try
    {
        auto ret = apiMgr->InitializeTarkovAPIManager();
        REQUIRE( ret );

        auto client = new cpr::Session();
        REQUIRE( client );

        auto hwid = ACC_HWID;
        REQUIRE( strlen(hwid) == 258 );

        auto user = auth::LoginImpl(client, ACC_EMAIL, ACC_PWD, "", hwid);
        REQUIRE( user.size() );

        json user_ctx{};
        try
        {
            user_ctx = json::parse(user.data());
        }
        catch (const json::parse_error& ex)
        {
            std::stringstream ss;
            ss << "JSON data(user) could not parsed: Error:" << "Message: " << ex.what() << '\n' << "exception id: " << ex.id << '\n' << "byte position of error: " << ex.byte << std::endl;

            FAIL(ss.str());
        }

        if (!user_ctx.contains("access_token"))
        {
            FAIL("'access_token' key is not available");
        }

        auto session = auth::ExchangeAccessToken(client, user_ctx["access_token"].get<std::string>(), hwid);
        if (session.empty())
        {
            FAIL("Null json data(session)");
        }

        json session_ctx{};
        try
        {
            session_ctx = json::parse(session.data());
        }
        catch (const json::parse_error& ex)
        {
            std::stringstream ss;
            ss << "JSON data(session) could not parsed: Error:" << "Message: " << ex.what() << '\n' << "exception id: " << ex.id << '\n' << "byte position of error: " << ex.byte << std::endl;

            FAIL(ss.str());
        }
        if (!session_ctx.contains("session"))
        {
            FAIL("'session' key is not available");
        }

        ret = apiMgr->FinalizeTarkovAPIManager();
        REQUIRE( ret );
    }
    catch (const json::exception& ex)
    {
        apiMgr->Log(__FUNCTION__, LL_SYS, fmt::format("json::exception - An exception handled! Data: {}", ex.what()));
	}
    catch (const TarkovAPIException& ex)
    {
        apiMgr->Log(__FUNCTION__, LL_SYS, fmt::format("TarkovAPIException - An exception handled! Data: {}", ex.details().c_str()));
    }

    delete apiMgr;	
	apiMgr = nullptr;
}
