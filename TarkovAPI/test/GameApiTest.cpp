#include <catch2/catch.hpp>
#include <json.hpp>

#include "Common.hpp"
#include "../src/TarkovAPIManager.hpp"
#include "../src/LogHelper.hpp"

using namespace TarkovAPI;
using json = nlohmann::json;

TEST_CASE("Keep alive test", "[multi-file:4]")
{
    auto apiMgr = new TarkovAPIManager();
    REQUIRE(apiMgr);

    try
    {
        auto ret = apiMgr->InitializeTarkovAPIManager();
        REQUIRE(ret);

        apiMgr->Login(ACC_EMAIL, ACC_PWD, ACC_HWID);

        auto me = apiMgr->GetMyProfile();
        REQUIRE(me.contains("_id"));

        apiMgr->SelectProfile(me["_id"].get<std::string>());
        REQUIRE(me.contains("Info"));
        REQUIRE(me["Info"].contains("Nickname"));

        apiMgr->KeepAlive();

        ret = apiMgr->FinalizeTarkovAPIManager();
        REQUIRE(ret);
    }
    catch (const json::exception & ex)
    {
        apiMgr->Log(__FUNCTION__, LL_SYS, fmt::format("json::exception - An exception handled! Data: {}", ex.what()));
    }
    catch (const TarkovAPIException & ex)
    {
        apiMgr->Log(__FUNCTION__, LL_SYS, fmt::format("TarkovAPIException - An exception handled! Data: {}", ex.details().c_str()));
    }

    delete apiMgr;
    apiMgr = nullptr;
}

TEST_CASE("Constants stuff tests", "[multi-file:4]")
{
    auto apiMgr = new TarkovAPIManager();
    REQUIRE(apiMgr);

    try
    {
        auto ret = apiMgr->InitializeTarkovAPIManager();
        REQUIRE(ret);

        apiMgr->Login(ACC_EMAIL, ACC_PWD, ACC_HWID);

        auto data = apiMgr->GetItems();
        REQUIRE(!data.empty());

        data = apiMgr->GetItemPrices();
        REQUIRE(!data.empty());

        data = apiMgr->GetLocations();
        REQUIRE(!data.empty());

        data = apiMgr->GetWeather();
        REQUIRE(!data.empty());

        data = apiMgr->GetI18n("en");
        REQUIRE(!data.empty());

        ret = apiMgr->FinalizeTarkovAPIManager();
        REQUIRE(ret);
    }
    catch (const json::exception & ex)
    {
        apiMgr->Log(__FUNCTION__, LL_SYS, fmt::format("json::exception - An exception handled! Data: {}", ex.what()));
    }
    catch (const TarkovAPIException & ex)
    {
        apiMgr->Log(__FUNCTION__, LL_SYS, fmt::format("TarkovAPIException - An exception handled! Data: {}", ex.details().c_str()));
    }

    delete apiMgr;
    apiMgr = nullptr;
}

TEST_CASE("Profile tests", "[multi-file:4]")
{
    auto apiMgr = new TarkovAPIManager();
    REQUIRE(apiMgr);

    try
    {
        auto ret = apiMgr->InitializeTarkovAPIManager();
        REQUIRE(ret);

        apiMgr->Login(ACC_EMAIL, ACC_PWD, ACC_HWID);

        auto me = apiMgr->GetMyProfile();
        REQUIRE(me.contains("_id"));

        apiMgr->SelectProfile(me["_id"].get<std::string>());
        REQUIRE(me.contains("Info"));
        REQUIRE(me["Info"].contains("Nickname"));

        apiMgr->Log(__FUNCTION__, LL_SYS, fmt::format("> Hello, {}!", me["Info"]["Nickname"].get<std::string>()));

        auto friends = apiMgr->GetFriends();
        REQUIRE(!friends.empty());

        ret = apiMgr->FinalizeTarkovAPIManager();
        REQUIRE(ret);
    }
    catch (const json::exception & ex)
    {
        apiMgr->Log(__FUNCTION__, LL_SYS, fmt::format("json::exception - An exception handled! Data: {}", ex.what()));
    }
    catch (const TarkovAPIException & ex)
    {
        apiMgr->Log(__FUNCTION__, LL_SYS, fmt::format("TarkovAPIException - An exception handled! Data: {}", ex.details().c_str()));
    }

    delete apiMgr;
    apiMgr = nullptr;
}

TEST_CASE("Search traders", "[multi-file:4]")
{
    auto apiMgr = new TarkovAPIManager();
    REQUIRE(apiMgr);

    try
    {
        auto ret = apiMgr->InitializeTarkovAPIManager();
        REQUIRE(ret);

        apiMgr->Login(ACC_EMAIL, ACC_PWD, ACC_HWID);

        auto me = apiMgr->GetMyProfile();
        REQUIRE(me.contains("_id"));

        apiMgr->SelectProfile(me["_id"].get<std::string>());
        REQUIRE(me.contains("Info"));
        REQUIRE(me["Info"].contains("Nickname"));

        apiMgr->Log(__FUNCTION__, LL_SYS, fmt::format("> Hello, {}!", me["Info"]["Nickname"].get<std::string>()));

        auto traders = apiMgr->GetTraders();
        REQUIRE(!traders.empty());

        auto mechanic_id = apiMgr->GetTraderIdByName("Mechanic");
        REQUIRE(!mechanic_id.empty());

        auto mechanic = apiMgr->GetTrader(mechanic_id);
        REQUIRE(!mechanic.empty());

        ret = apiMgr->FinalizeTarkovAPIManager();
        REQUIRE(ret);
    }
    catch (const json::exception & ex)
    {
        apiMgr->Log(__FUNCTION__, LL_SYS, fmt::format("json::exception - An exception handled! Data: {}", ex.what()));
    }
    catch (const TarkovAPIException & ex)
    {
        apiMgr->Log(__FUNCTION__, LL_SYS, fmt::format("TarkovAPIException - An exception handled! Data: {}", ex.details().c_str()));
    }

    delete apiMgr;
    apiMgr = nullptr;
}

TEST_CASE("Search trader items", "[multi-file:4]")
{
    auto apiMgr = new TarkovAPIManager();
    REQUIRE(apiMgr);

    try
    {
        auto ret = apiMgr->InitializeTarkovAPIManager();
        REQUIRE(ret);

        apiMgr->Login(ACC_EMAIL, ACC_PWD, ACC_HWID);

        auto me = apiMgr->GetMyProfile();
        REQUIRE(me.contains("_id"));

        apiMgr->SelectProfile(me["_id"].get<std::string>());
        REQUIRE(me.contains("Info"));
        REQUIRE(me["Info"].contains("Nickname"));

        apiMgr->Log(__FUNCTION__, LL_SYS, fmt::format("> Hello, {}!", me["Info"]["Nickname"].get<std::string>()));

        auto mechanic_id = apiMgr->GetTraderIdByName("Mechanic");
        REQUIRE(!mechanic_id.empty());

        auto traders = apiMgr->GetTraders();
        REQUIRE(!traders.empty());

        auto mechanic = apiMgr->GetTrader(mechanic_id);
        REQUIRE(!mechanic.empty());

        auto mechanic_items = apiMgr->GetTraderItems(mechanic_id);
        REQUIRE(!mechanic_items.empty());

        /*
        "56d59d3ad2720bdb418b4577":{
            "Description":"9x19 mm Pst gzh round. Steel core bullet. Developed by TSNIITOCHMASH in the early 90s. Bullet weight — 5,4 g, muzzle velocity — 445—470 m/s. It outperforms the commercially available 9×19 mm Parabellum ordnance and corresponds to the more powerful 9×19 mm NATO army rounds (9×19 +P). The bullet hits through 4 mm plate of St.3 steel at distance of 55 m.",
            "Name":"9x19 mm Pst gzh",
            "ShortName":"Pst gzh",
            "casingName":"9x19 mm Pst gzh"
        },
        */
        auto buy_item_tpl = std::string("56d59d3ad2720bdb418b4577");
        REQUIRE(!buy_item_tpl.empty());

        auto it = std::find_if(mechanic_items.begin(), mechanic_items.end(), [&buy_item_tpl](const quicktype::TraderItem& x)
            {
                return x._tpl == buy_item_tpl;
            });
        REQUIRE(it != mechanic_items.end());

        ret = apiMgr->FinalizeTarkovAPIManager();
        REQUIRE(ret);
    }
    catch (const json::exception & ex)
    {
        apiMgr->Log(__FUNCTION__, LL_SYS, fmt::format("json::exception - An exception handled! Data: {}", ex.what()));
    }
    catch (const TarkovAPIException & ex)
    {
        apiMgr->Log(__FUNCTION__, LL_SYS, fmt::format("TarkovAPIException - An exception handled! Data: {}", ex.details().c_str()));
    }

    delete apiMgr;
    apiMgr = nullptr;
}

TEST_CASE("Sell item to trader", "[multi-file:4]")
{
    auto apiMgr = new TarkovAPIManager();
    REQUIRE(apiMgr);

    try
    {
        auto ret = apiMgr->InitializeTarkovAPIManager();
        REQUIRE(ret);

        apiMgr->Login(ACC_EMAIL, ACC_PWD, ACC_HWID);

        auto me = apiMgr->GetMyProfile();
        REQUIRE(me.contains("_id"));

        apiMgr->SelectProfile(me["_id"].get<std::string>());
        REQUIRE(me.contains("Info"));
        REQUIRE(me["Info"].contains("Nickname"));

        apiMgr->Log(__FUNCTION__, LL_SYS, fmt::format("> Hello, {}!", me["Info"]["Nickname"].get<std::string>()));

        auto mechanic_id = apiMgr->GetTraderIdByName("Mechanic");
        REQUIRE(!mechanic_id.empty());

        auto traders = apiMgr->GetTraders();
        REQUIRE(!traders.empty());

        auto mechanic = apiMgr->GetTrader(mechanic_id);
        REQUIRE(!mechanic.empty());

        uint64_t roubleCount = apiMgr->GetRoubleCount();
        REQUIRE(roubleCount > 3000);

        const auto number_fmt = [](uint64_t n)
        {
            std::stringstream ss;
            ss << n;

            std::string s = ss.str();
            s.reserve(s.length() + s.length() / 3);

            for (int32_t i = 0, j = 3 - s.length() % 3; i < s.length(); ++i, ++j)
            {
                if (i != 0 && j % 3 == 0)
                {
                    s.insert(i++, 1, '.');
                }
            }

            return s;
        };
        apiMgr->Log(__FUNCTION__, LL_SYS, fmt::format("> Rouble count: {}", number_fmt(roubleCount)));

        apiMgr->SellItem(mechanic_id, "5e41e545d36a7e4a927490ca", 10); // 10x 7N39

        roubleCount = apiMgr->GetRoubleCount();
        apiMgr->Log(__FUNCTION__, LL_SYS, fmt::format("> Rouble count: {}", number_fmt(roubleCount)));

        ret = apiMgr->FinalizeTarkovAPIManager();
        REQUIRE(ret);
    }
    catch (const json::exception & ex)
    {
        apiMgr->Log(__FUNCTION__, LL_SYS, fmt::format("json::exception - An exception handled! Data: {}", ex.what()));
    }
    catch (const TarkovAPIException & ex)
    {
        apiMgr->Log(__FUNCTION__, LL_SYS, fmt::format("TarkovAPIException - An exception handled! Data: {}", ex.details().c_str()));
    }

    delete apiMgr;
    apiMgr = nullptr;
}

TEST_CASE("Buy item than trader with cash", "[multi-file:4]")
{
    auto apiMgr = new TarkovAPIManager();
    REQUIRE(apiMgr);

    try
    {
        auto ret = apiMgr->InitializeTarkovAPIManager();
        REQUIRE(ret);

        apiMgr->Login(ACC_EMAIL, ACC_PWD, ACC_HWID);

        auto me = apiMgr->GetMyProfile();
        REQUIRE(me.contains("_id"));

        apiMgr->SelectProfile(me["_id"].get<std::string>());
        REQUIRE(me.contains("Info"));
        REQUIRE(me["Info"].contains("Nickname"));

        apiMgr->Log(__FUNCTION__, LL_SYS, fmt::format("> Hello, {}!", me["Info"]["Nickname"].get<std::string>()));

        auto mechanic_id = apiMgr->GetTraderIdByName("Mechanic");
        REQUIRE(!mechanic_id.empty());

        auto traders = apiMgr->GetTraders();
        REQUIRE(!traders.empty());

        auto mechanic = apiMgr->GetTrader(mechanic_id);
        REQUIRE(!mechanic.empty());

        auto rouble_count = apiMgr->GetRoubleCount();
        REQUIRE(rouble_count);

        const auto number_fmt = [](uint64_t n)
        {
            std::stringstream ss;
            ss << n;

            std::string s = ss.str();
            s.reserve(s.length() + s.length() / 3);

            for (int32_t i = 0, j = 3 - s.length() % 3; i < s.length(); ++i, ++j)
            {
                if (i != 0 && j % 3 == 0)
                {
                    s.insert(i++, 1, '.');
                }
            }

            return s;
        };
        apiMgr->Log(__FUNCTION__, LL_SYS, fmt::format("> Rouble count: {}", number_fmt(rouble_count)));

        auto mechanic_items = apiMgr->GetTraderItems(mechanic_id);
        REQUIRE(!mechanic_items.empty());

        /*
        "56d59d3ad2720bdb418b4577":{
            "Description":"9x19 mm Pst gzh round. Steel core bullet. Developed by TSNIITOCHMASH in the early 90s. Bullet weight — 5,4 g, muzzle velocity — 445—470 m/s. It outperforms the commercially available 9×19 mm Parabellum ordnance and corresponds to the more powerful 9×19 mm NATO army rounds (9×19 +P). The bullet hits through 4 mm plate of St.3 steel at distance of 55 m.",
            "Name":"9x19 mm Pst gzh",
            "ShortName":"Pst gzh",
            "casingName":"9x19 mm Pst gzh"
        },
        */
        auto buy_item_tpl = std::string("56d59d3ad2720bdb418b4577");
        REQUIRE(!buy_item_tpl.empty());

        auto buy_item_count = 3;
        REQUIRE(buy_item_count > 0);

        auto it = std::find_if(mechanic_items.begin(), mechanic_items.end(), [&buy_item_tpl](const quicktype::TraderItem& x)
            {
                return x._tpl == buy_item_tpl;
            });
        REQUIRE(it != mechanic_items.end());

        REQUIRE(it->costs.size() == 1);
        REQUIRE(it->costs.at(0)._tpl == ROUBLE_ITEM_ID);
        REQUIRE(buy_item_count <= it->upd->stack_objects_count);

        auto item_price = apiMgr->GetItemPrice(it->costs.at(0).count, buy_item_count);
        REQUIRE(item_price > 0);

        auto given_item_ids = apiMgr->FindItemStack(ROUBLE_ITEM_ID, static_cast<uint64_t>(item_price));
        REQUIRE(!given_item_ids.empty());

        apiMgr->TradeItem(mechanic_id, it->_id, buy_item_count, given_item_ids);

        auto new_rouble = apiMgr->GetRoubleCount();
        REQUIRE(rouble_count != new_rouble);

        apiMgr->Log(__FUNCTION__, LL_SYS, fmt::format("> Rouble count: {}", number_fmt(new_rouble)));

        ret = apiMgr->FinalizeTarkovAPIManager();
        REQUIRE(ret);
    }
    catch (const json::exception & ex)
    {
        apiMgr->Log(__FUNCTION__, LL_SYS, fmt::format("json::exception - An exception handled! Data: {}", ex.what()));
    }
    catch (const TarkovAPIException & ex)
    {
        apiMgr->Log(__FUNCTION__, LL_SYS, fmt::format("TarkovAPIException - An exception handled! Data: {}", ex.details().c_str()));
    }

    delete apiMgr;
    apiMgr = nullptr;
}

TEST_CASE("Buy item than trader with barter", "[multi-file:4]")
{
    auto apiMgr = new TarkovAPIManager();
    REQUIRE(apiMgr);

    try
    {
        auto ret = apiMgr->InitializeTarkovAPIManager();
        REQUIRE(ret);

        apiMgr->Login(ACC_EMAIL, ACC_PWD, ACC_HWID);

        auto me = apiMgr->GetMyProfile();
        REQUIRE(me.contains("_id"));

        apiMgr->SelectProfile(me["_id"].get<std::string>());
        REQUIRE(me.contains("Info"));
        REQUIRE(me["Info"].contains("Nickname"));

        apiMgr->Log(__FUNCTION__, LL_SYS, fmt::format("> Hello, {}!", me["Info"]["Nickname"].get<std::string>()));

        auto therapist_id = apiMgr->GetTraderIdByName("Therapist");
        REQUIRE(!therapist_id.empty());

        auto traders = apiMgr->GetTraders();
        REQUIRE(!traders.empty());

        auto therapist = apiMgr->GetTrader(therapist_id);
        REQUIRE(!therapist.empty());

        auto therapist_items = apiMgr->GetTraderItems(therapist_id);
        REQUIRE(!therapist_items.empty());

        /*
        "544fb37f4bdc2dee738b4567":{
            "Description":"The most cheap and widely spread painkillers.",
            "Name":"Analgin painkillers",
            "ShortName":"Painkillers"
        },
        */
        auto buy_item_tpl = std::string("544fb37f4bdc2dee738b4567");
        REQUIRE(!buy_item_tpl.empty());

        auto buy_item_count = 3;
        REQUIRE(buy_item_count > 0);

        /*
        "57347b8b24597737dd42e192":{
            "Description":"A matchbox full of safety matches. Lighters are more reliable and easier to use, but that's exactly why everybody tries to keep them hidden when asked for a light.",
            "Name":"Matches",
            "ShortName":"Matches"
        },
        */

        auto matches_tpl = std::string("57347b8b24597737dd42e192");
        REQUIRE(!matches_tpl.empty());

        auto it = std::find_if(therapist_items.begin(), therapist_items.end(), [&buy_item_tpl, &matches_tpl](const quicktype::TraderItem& x)
            {
                return x._tpl == buy_item_tpl && x.costs.at(0)._tpl == matches_tpl;
            });
        REQUIRE(it != therapist_items.end());

        REQUIRE(it->costs.size() == 1);
        REQUIRE(buy_item_count <= it->upd->stack_objects_count);

        auto required_item_count = it->costs.at(0).count * buy_item_count;
        REQUIRE(required_item_count);

        auto given_item_ids = apiMgr->FindItemStack(matches_tpl, static_cast<int64_t>(it->costs.at(0).count)* buy_item_count);
        REQUIRE(!given_item_ids.empty());

        auto trade_ret = apiMgr->TradeItem(therapist_id, it->_id, buy_item_count, given_item_ids);
        REQUIRE(!trade_ret.empty());

        ret = apiMgr->FinalizeTarkovAPIManager();
        REQUIRE(ret);
    }
    catch (const json::exception & ex)
    {
        apiMgr->Log(__FUNCTION__, LL_SYS, fmt::format("json::exception - An exception handled! Data: {}", ex.what()));
    }
    catch (const TarkovAPIException & ex)
    {
        apiMgr->Log(__FUNCTION__, LL_SYS, fmt::format("TarkovAPIException - An exception handled! Data: {}", ex.details().c_str()));
    }

    delete apiMgr;
    apiMgr = nullptr;
}

TEST_CASE("Flea market getters", "[multi-file:4]")
{
    auto apiMgr = new TarkovAPIManager();
    REQUIRE(apiMgr);

    try
    {
        auto ret = apiMgr->InitializeTarkovAPIManager();
        REQUIRE(ret);

        apiMgr->Login(ACC_EMAIL, ACC_PWD, ACC_HWID);

        auto me = apiMgr->GetMyProfile();
        REQUIRE(me.contains("_id"));

        apiMgr->SelectProfile(me["_id"].get<std::string>());
        REQUIRE(me.contains("Info"));
        REQUIRE(me["Info"].contains("Nickname"));

        apiMgr->Log(__FUNCTION__, LL_SYS, fmt::format("> Hello, {}!", me["Info"]["Nickname"].get<std::string>()));

        auto search_ret = apiMgr->SearchMarket(quicktype::MarketFilterBody{});
        REQUIRE(!search_ret.empty());

        auto price_ret = apiMgr->GetItemPrice("5696686a4bdc2da3298b456a");
        REQUIRE(!price_ret.empty());

        ret = apiMgr->FinalizeTarkovAPIManager();
        REQUIRE(ret);
    }
    catch (const json::exception & ex)
    {
        apiMgr->Log(__FUNCTION__, LL_SYS, fmt::format("json::exception - An exception handled! Data: {}", ex.what()));
    }
    catch (const TarkovAPIException & ex)
    {
        apiMgr->Log(__FUNCTION__, LL_SYS, fmt::format("TarkovAPIException - An exception handled! Data: {}", ex.details().c_str()));
    }

    delete apiMgr;
    apiMgr = nullptr;
}

TEST_CASE("Sell item to regfair", "[multi-file:4]")
{
    auto apiMgr = new TarkovAPIManager();
    REQUIRE(apiMgr);

    try
    {
        auto ret = apiMgr->InitializeTarkovAPIManager();
        REQUIRE(ret);

        apiMgr->Login(ACC_EMAIL, ACC_PWD, ACC_HWID);

        auto me = apiMgr->GetMyProfile();
        REQUIRE(me.contains("_id"));

        apiMgr->SelectProfile(me["_id"].get<std::string>());
        REQUIRE(me.contains("Info"));
        REQUIRE(me["Info"].contains("Nickname"));

        apiMgr->Log(__FUNCTION__, LL_SYS, fmt::format("> Hello, {}!", me["Info"]["Nickname"].get<std::string>()));

        auto painkiller_tpl = "544fb37f4bdc2dee738b4567";

        auto painkiller = apiMgr->FindItemStack(painkiller_tpl);
        REQUIRE(!painkiller.empty());

        // '{"avg":21345.837037037,"max":155000,"min":483,"templateId":"544fb37f4bdc2dee738b4567"}'
        auto price = apiMgr->GetItemPrice(painkiller_tpl);
        REQUIRE(!price.empty());
        REQUIRE(price.contains("avg"));

        auto avg_price = static_cast<int64_t>(std::round(price["avg"].get<double>()));
        REQUIRE(avg_price > 0);

        auto offer_ret = apiMgr->OfferItem(
            { painkiller.at(0)._tpl },
            { ROUBLE_ITEM_ID, avg_price },
            false
        );
        REQUIRE(!offer_ret.empty());

        ret = apiMgr->FinalizeTarkovAPIManager();
        REQUIRE(ret);
    }
    catch (const json::exception & ex)
    {
        apiMgr->Log(__FUNCTION__, LL_SYS, fmt::format("json::exception - An exception handled! Data: {}", ex.what()));
    }
    catch (const TarkovAPIException & ex)
    {
        apiMgr->Log(__FUNCTION__, LL_SYS, fmt::format("TarkovAPIException - An exception handled! Data: {}", ex.details().c_str()));
    }

    delete apiMgr;
    apiMgr = nullptr;
}

TEST_CASE("Buy item than regfair", "[multi-file:4]")
{
    auto apiMgr = new TarkovAPIManager();
    REQUIRE(apiMgr);

    try
    {
        auto ret = apiMgr->InitializeTarkovAPIManager();
        REQUIRE(ret);

        apiMgr->Login(ACC_EMAIL, ACC_PWD, ACC_HWID);

        auto me = apiMgr->GetMyProfile();
        REQUIRE(me.contains("_id"));

        apiMgr->SelectProfile(me["_id"].get<std::string>());
        REQUIRE(me.contains("Info"));
        REQUIRE(me["Info"].contains("Nickname"));

        apiMgr->Log(__FUNCTION__, LL_SYS, fmt::format("> Hello, {}!", me["Info"]["Nickname"].get<std::string>()));

        auto locale = apiMgr->GetI18n("en");
        REQUIRE(!locale.empty());
        REQUIRE(locale.contains("handbook"));

        auto barter_catergory_id = std::string();
        for (const auto& item : locale["handbook"].items())
        {
            if (item.value().get<std::string>() == "Barter items")
            {
                barter_catergory_id = item.key();
                break;
            }
        }
        REQUIRE(!barter_catergory_id.empty());

        auto filter = quicktype::MarketFilterBody{}
            .set_price_to(2000)
            .set_handbook_id(barter_catergory_id)
            .set_offer_owner_type(quicktype::MarketOwner::Player)
            .set_remove_bartering(true)
            .set_currency(quicktype::MarketCurrency::Rouble)
            ;

        auto offers = apiMgr->SearchMarket(filter);
        REQUIRE(!offers.empty());
        REQUIRE(offers.contains("offers"));
        /*
        {
            "categories":{
                ...
            },
            "offers":[
                ...
            ],
            "offersCount":65,
            "selectedCategory":"5b47574386f77428ca22b33e"
        }
        */

        auto st = std::time(nullptr);
        auto epoch_time = static_cast<std::chrono::seconds>(st).count();

        /*
        {
            "_id":"...",
            "endTime":1581724471,
            "intId":"18198680185",
            "items":[
            ],
            "itemsCost":1927,
            "loyaltyLevel":1,
            "requirements":[
                {
                    "_tpl":"5449016a4bdc2d6f028b456f",
                    "count":1400
                }
            ],
            "requirementsCost":1400,
            "root":"...",
            "sellInOnePiece":false,
            "startTime":1581681271,
            "summaryCost":1400,
            "user":{
                "avatar":null,
                "id":"...",
                "isRatingGrowing":true,
                "memberType":0,
                "nickname":"...",
                "rating":15.68
            }
        },
        */

        for (const auto& offers_ctx : offers["offers"].items())
        {
            const auto offer_index = offers_ctx.key();
            const auto curr_offer = offers_ctx.value();

            REQUIRE(curr_offer.contains("startTime"));
            REQUIRE(curr_offer.contains("endTime"));
            REQUIRE(curr_offer.contains("requirementsCost"));
            REQUIRE(curr_offer.contains("_id"));

            if (curr_offer["startTime"].get<int64_t>() + 60 <= epoch_time &&
                curr_offer["endTime"].get<int64_t>() >= epoch_time)
            {
                auto offer_id = curr_offer["_id"].get<std::string>();
                auto required_rouble = curr_offer["requirementsCost"].get<uint64_t>();
                auto rouble_count = apiMgr->GetRoubleCount();

                REQUIRE(curr_offer["items"].size() == 1);
                REQUIRE(curr_offer["items"][0].contains("_tpl"));

                auto item_schema_id = curr_offer["items"][0]["_tpl"].get<std::string>();
                auto item_name = apiMgr->GetItemName(item_schema_id);

                apiMgr->Log(__FUNCTION__, LL_SYS, fmt::format("Item: {} -> Price: {}", item_name, required_rouble));

                if (rouble_count >= required_rouble)
                {
                    auto rouble_ids = apiMgr->FindItemStack(ROUBLE_ITEM_ID, required_rouble);

                    auto buy_ret = apiMgr->BuyItem(offer_id, 1, rouble_ids);
                    REQUIRE(!buy_ret.empty());

                    apiMgr->Log(__FUNCTION__, LL_SYS, fmt::format("The {}(Offer: '{}'): has been bought for {} roubles",
                        item_name, offer_id, required_rouble));
                }
            }
        }

        ret = apiMgr->FinalizeTarkovAPIManager();
        REQUIRE(ret);
    }
    catch (const json::exception & ex)
    {
        apiMgr->Log(__FUNCTION__, LL_SYS, fmt::format("json::exception - An exception handled! Data: {}", ex.what()));
    }
    catch (const TarkovAPIException & ex)
    {
        apiMgr->Log(__FUNCTION__, LL_SYS, fmt::format("TarkovAPIException - An exception handled! Data: {}", ex.details().c_str()));
    }

    delete apiMgr;
    apiMgr = nullptr;
}

TEST_CASE("Stack/Transfer roubles in inventory", "[multi-file:4]")
{
    auto apiMgr = new TarkovAPIManager();
    REQUIRE(apiMgr);

    try
    {
        auto ret = apiMgr->InitializeTarkovAPIManager();
        REQUIRE(ret);

        apiMgr->Login(ACC_EMAIL, ACC_PWD, ACC_HWID);

        auto me = apiMgr->GetMyProfile();
        REQUIRE(me.contains("_id"));

        apiMgr->SelectProfile(me["_id"].get<std::string>());
        REQUIRE(me.contains("Info"));
        REQUIRE(me["Info"].contains("Nickname"));

        apiMgr->Log(__FUNCTION__, LL_SYS, fmt::format("> Hello, {}!", me["Info"]["Nickname"].get<std::string>()));

        auto rouble_ids = apiMgr->FindItemStack(ROUBLE_ITEM_ID, std::numeric_limits<uint64_t>::max());

        auto from_id = std::string();
        auto to_id = std::string();
        int64_t amount = 0;
        auto it = std::find_if(rouble_ids.begin(), rouble_ids.end(), [&from_id, &to_id, &amount](const quicktype::TraderBarterItem& x)
            {
                if (to_id.empty() && x.count < 500000)
                {
                    to_id = x._tpl;
                    amount = 500000 - static_cast<int64_t>(x.count);
                }
                if (from_id.empty() && to_id != x._tpl)
                {
                    from_id = x._tpl;
                }
                return !from_id.empty() && !to_id.empty();
            });

        auto stack_ret = apiMgr->StackItem(from_id, to_id, amount);
        REQUIRE(!stack_ret.empty());

        ret = apiMgr->FinalizeTarkovAPIManager();
        REQUIRE(ret);
    }
    catch (const json::exception & ex)
    {
        apiMgr->Log(__FUNCTION__, LL_SYS, fmt::format("json::exception - An exception handled! Data: {}", ex.what()));
    }
    catch (const TarkovAPIException & ex)
    {
        apiMgr->Log(__FUNCTION__, LL_SYS, fmt::format("TarkovAPIException - An exception handled! Data: {}", ex.details().c_str()));
    }

    delete apiMgr;
    apiMgr = nullptr;
}

TEST_CASE("Merge bullets in inventory", "[multi-file:4]")
{
    auto apiMgr = new TarkovAPIManager();
    REQUIRE(apiMgr);

    try
    {
        auto ret = apiMgr->InitializeTarkovAPIManager();
        REQUIRE(ret);

        apiMgr->Login(ACC_EMAIL, ACC_PWD, ACC_HWID);

        auto me = apiMgr->GetMyProfile();
        REQUIRE(me.contains("_id"));

        apiMgr->SelectProfile(me["_id"].get<std::string>());
        REQUIRE(me.contains("Info"));
        REQUIRE(me["Info"].contains("Nickname"));

        apiMgr->Log(__FUNCTION__, LL_SYS, fmt::format("> Hello, {}!", me["Info"]["Nickname"].get<std::string>()));

        /*
        "56d59d3ad2720bdb418b4577":{
            "Description":"9x19 mm Pst gzh round. Steel core bullet. Developed by TSNIITOCHMASH in the early 90s. Bullet weight — 5,4 g, muzzle velocity — 445—470 m/s. It outperforms the commercially available 9×19 mm Parabellum ordnance and corresponds to the more powerful 9×19 mm NATO army rounds (9×19 +P). The bullet hits through 4 mm plate of St.3 steel at distance of 55 m.",
            "Name":"9x19 mm Pst gzh",
            "ShortName":"Pst gzh",
            "casingName":"9x19 mm Pst gzh"
        },
        */
        auto bullet_ids = apiMgr->FindItemStack("56d59d3ad2720bdb418b4577", std::numeric_limits<uint64_t>::max());

        auto from_id = std::string();
        auto to_id = std::string();
        auto it = std::find_if(bullet_ids.begin(), bullet_ids.end(), [&from_id, &to_id](const quicktype::TraderBarterItem& x)
            {
                if (to_id.empty())
                {
                    to_id = x._tpl;
                }
                if (from_id.empty() && to_id != x._tpl)
                {
                    from_id = x._tpl;
                }
                return !from_id.empty() && !to_id.empty();
            });

        auto stack_ret = apiMgr->StackItem(from_id, to_id);
        REQUIRE(!stack_ret.empty());

        ret = apiMgr->FinalizeTarkovAPIManager();
        REQUIRE(ret);
    }
    catch (const json::exception & ex)
    {
        apiMgr->Log(__FUNCTION__, LL_SYS, fmt::format("json::exception - An exception handled! Data: {}", ex.what()));
    }
    catch (const TarkovAPIException & ex)
    {
        apiMgr->Log(__FUNCTION__, LL_SYS, fmt::format("TarkovAPIException - An exception handled! Data: {}", ex.details().c_str()));
    }

    delete apiMgr;
    apiMgr = nullptr;
}

TEST_CASE("Mail stuffs", "[multi-file:4]")
{
    auto apiMgr = new TarkovAPIManager();
    REQUIRE(apiMgr);

    try
    {
        auto ret = apiMgr->InitializeTarkovAPIManager();
        REQUIRE(ret);

        apiMgr->Login(ACC_EMAIL, ACC_PWD, ACC_HWID);

        auto me = apiMgr->GetMyProfile();
        REQUIRE(me.contains("_id"));

        apiMgr->SelectProfile(me["_id"].get<std::string>());
        REQUIRE(me.contains("Info"));
        REQUIRE(me["Info"].contains("Nickname"));

        apiMgr->Log(__FUNCTION__, LL_SYS, fmt::format("> Hello, {}!", me["Info"]["Nickname"].get<std::string>()));

        auto items = apiMgr->GetItems();
        //apiMgr->FindBlankStashPos();
        //__debugbreak();

        auto mail_list = apiMgr->GetMailList();
        REQUIRE(!mail_list.empty());
        REQUIRE(mail_list.is_array());

        for (const auto& mails : mail_list.items())
        {
            const auto mail_index = mails.key();
            const auto mail_data = mails.value();
            
            REQUIRE(mail_data.is_object());
            REQUIRE(mail_data.contains("_id"));
            REQUIRE(mail_data.contains("type"));

            if (mail_data["type"].get<int64_t>() == MailTypes::MAIL_TYPE_MARKET)
            {
                auto mail = apiMgr->GetMail(mail_data["_id"], mail_data["type"].get<int64_t>());
                REQUIRE(!mail.empty());

                auto mail_attachments = apiMgr->GetMailAttachments(mail_data["_id"]);
                REQUIRE(!mail_attachments.empty());
                REQUIRE(mail_attachments.contains("messages"));

                for (const auto& message : mail_attachments["messages"].items())
                {
                    /*
                    {
                        "_id":"...",
                        "dt":1234567890.3169,
                        "hasRewards":true,
                        "items":{
                            "data":[
                                {
                                    "_id":"...",
                                    "_tpl":"...",
                                    "parentId":"...",
                                    "slotId":"main",
                                    "upd":{
                                        "StackObjectsCount":100
                                    }
                                }
                            ],
                            "stash":"..."
                        },
                        "maxStorageTime":0,
                        "systemData":{
                            "buyerNickname":"...",
                            "itemCount":1,
                            "soldItem":"..."
                        },
                        "templateId":"...",
                        "type":4,
                        "uid":"..."
                    }
                    */

                    auto context = message.value();
                    REQUIRE(context.contains("_id"));
                    REQUIRE(context.contains("items"));
                    REQUIRE(context["items"].contains("data"));
                    REQUIRE(!context["items"]["data"].empty());

                    // TODO: For rouble schema IDs, At the first search < 500000 roubles for merge in stash
                    /* // Merge payload:
                    {
                        "Action": "Merge",
                        "item": "...",
                        "with": "...",
                        "fromOwner": {
                            "id": "...",
                            "type": "Mail"
                        }
                    }
                    */

                    for (const auto& item : context["items"]["data"].items())
                    {
                        auto reward = item.value();
                        REQUIRE(reward.contains("_id"));
                       
                        auto stash_id = apiMgr->GetMainStashID();
                        REQUIRE(!stash_id.empty());

                        // TODO: Enumerate stash items and find blank position automatically
                        auto pos = quicktype::MailRewardToLocation {
                            2,
                            0,
                            "Horizontal",
                            true
                        };

                        auto get_reward_ret = apiMgr->GetMailReward(reward["_id"].get<std::string>(), stash_id, context["_id"].get<std::string>(), pos);
                        REQUIRE(!get_reward_ret.empty());
                        return; // Just get once for pre-defined postion
                    }
                }
            }
        }

        ret = apiMgr->FinalizeTarkovAPIManager();
        REQUIRE(ret);
    }
    catch (const json::exception & ex)
    {
        apiMgr->Log(__FUNCTION__, LL_SYS, fmt::format("json::exception - An exception handled! Data: {}", ex.what()));
    }
    catch (const TarkovAPIException & ex)
    {
        apiMgr->Log(__FUNCTION__, LL_SYS, fmt::format("TarkovAPIException - An exception handled! Data: {}", ex.details().c_str()));
    }

    delete apiMgr;
    apiMgr = nullptr;
}
