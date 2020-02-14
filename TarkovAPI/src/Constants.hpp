#pragma once
#include <json.hpp>
#include <cpr/cpr.h>
#include <fmt/format.h>

#include <cstring>
#include <cstdint>
#include <vector>
#include <memory>
#include <variant>
#include <optional>

#ifndef NLOHMANN_OPT_HELPER
#define NLOHMANN_OPT_HELPER
namespace nlohmann {
    template <typename T>
    struct adl_serializer<std::shared_ptr<T>> {
        static void to_json(json& j, const std::shared_ptr<T>& opt) {
            if (!opt) j = nullptr; else j = *opt;
        }

        static std::shared_ptr<T> from_json(const json& j) {
            if (j.is_null()) return std::unique_ptr<T>(); else return std::unique_ptr<T>(new T(j.get<T>()));
        }
    };
}
#endif

namespace quicktype // https://app.quicktype.io
{
    inline nlohmann::json get_untyped(const nlohmann::json& j, const char* property)
    {
        if (!j.empty() && j.find(property) != j.end())
        {
            return j.at(property).get<nlohmann::json>();
        }
        return nlohmann::json();
    }

    inline nlohmann::json get_untyped(const nlohmann::json& j, const std::string& property)
    {
        return get_untyped(j, property.data());
    }

    template <typename T>
    inline std::shared_ptr<T> get_optional(const nlohmann::json& j, const char* property) {
        if (j.find(property) != j.end()) {
            return j.at(property).get<std::shared_ptr<T>>();
        }
        return std::shared_ptr<T>();
    }

    template <typename T>
    inline std::shared_ptr<T> get_optional(const nlohmann::json& j, std::string property) {
        return get_optional<T>(j, property.data());
    }

    // Common decompressed API response
    struct ResponseBody
    {
        nlohmann::json data;
        int64_t err;
        std::string errmsg;
    };

    // Profile
    struct Settings
    {
        std::shared_ptr<std::string> bot_difficulty;
        std::shared_ptr<int64_t> experience;
        std::shared_ptr<std::string> role;
    };

    struct Info
    {
        int64_t account_type;
        std::vector<nlohmann::json> bans;
        int64_t experience;
        std::string game_version;
        bool global_wipe;
        int64_t last_time_played_as_savage;
        int64_t level;
        std::string lower_nickname;
        std::string member_category;
        bool need_wipe;
        std::string nickname;
        int64_t nickname_change_date;
        int64_t registration_date;
        int64_t savage_lock_time;
        quicktype::Settings settings;
        std::string side;
        std::string voice;
        bool locked_move_commands;
    };

    // Common
    struct TraderBarterItem
    {
        std::string _tpl;
        double count;
    };

    // Buy
    struct BuyOfferContext
    {
        std::string id;
        int64_t count;
        std::vector <quicktype::TraderBarterItem> items;
    };
    struct BuyDatumContext
    {
        std::string action;
        std::vector <quicktype::BuyOfferContext> offers;
    };
    struct MarketBuyReqBody
    {
        std::vector <quicktype::BuyDatumContext> data;
        int64_t tm;
    };

    // Sell
    struct SellItemContext
    {
        std::string id;
        int64_t count;
        std::string scheme_id;
    };
    struct SellDatumContext
    {
        std::string action;
        std::string type;
        std::string tid;
        std::vector <quicktype::SellItemContext> items;
    };
    struct MarketSellReqBody
    {
        std::vector <quicktype::SellDatumContext> data;
        int64_t tm;
    };

    // Offer
    struct OfferRequirementContext
    {
        std::string _tpl;
        int64_t price;
    };
    struct OfferRequirementsContext
    {
        std::string _tpl;
        int64_t count;
        int64_t level;
        int64_t side;
        bool onlyFunctional;
    };
    struct OfferDatumContext
    {
        std::string action;
        bool sell_in_one_piece;
        std::vector <std::string> items;
        std::vector <quicktype::OfferRequirementsContext> requirements;
        int64_t tm;
    };
    struct MarketOfferReqBody
    {
        std::vector <quicktype::OfferDatumContext> data;
    };

    // Merge
    struct ItemStackDatum
    {
        std::string action;
        std::string item;
        std::string with;
    };
    struct ItemStackBody
    {
        std::vector <quicktype::ItemStackDatum> data;
        int64_t tm;
    };
    // Transfer
    struct ItemTransferDatum
    {
        std::string action;
        std::string item;
        std::string with;
        int64_t count;
    };
    struct ItemTransferBody
    {
        std::vector <quicktype::ItemTransferDatum> data;
        int64_t tm;
    };

    // Move
    struct ItemMoveLocation
    {
        int64_t x;
        int64_t y;
        int64_t r;
    };
    struct ItemMoveTo
    {
        std::string id;
        std::string container{ "main" };
        quicktype::ItemMoveLocation location{ 0, 0, 0 };
    };
    struct ItemMoveDatum
    {
        std::string action;
        std::string item;
        quicktype::ItemMoveTo to;
    };
    struct ItemMoveBody
    {
        std::vector <quicktype::ItemMoveDatum> data;
        int64_t tm;
    };

    // Trader
    struct TraderItemUpd
    {
        int64_t stack_objects_count;
        std::shared_ptr<bool> unlimited_count;
        std::shared_ptr<int64_t> buy_restriction_current;
        std::shared_ptr<int64_t> buy_restriction_max;
    };

    struct TraderItem
    {
        std::string _id;
        std::string _tpl;
        std::shared_ptr <TraderItemUpd> upd;
        std::vector <quicktype::TraderBarterItem> costs;
        int64_t loyalty_level;
    };

    // Trade
    struct TradeItemDatum
    {
        std::string action;
        std::string type;
        std::string tid;
        std::string item_id;
        int64_t count;
        int64_t scheme_id;
        std::vector <quicktype::TraderBarterItem> scheme_items;
    };

    struct TradeItemBody
    {
        std::vector <quicktype::TradeItemDatum> data;
        int64_t tm;
    };

    // Custom
    struct InventoryItemContext
    {
        std::string id;
        uint64_t count;
    };

    // Enums
    enum class MarketSortBy : int64_t
    {
        /// Sort by ID
        ID = 0,
        /// Sort by bartering offers
        BarteringOffers = 2,
        /// Sort by merchant rating
        MerchantRating = 3,
        /// Sort by price (default)
        Price = 5,
        /// Sort by expiry
        Expiry = 6
    };

    enum class MarketSortDirection : int64_t
    {
        /// Sort ascending (default)
        Ascending = 0,
        /// Sort descending
        Descending = 1,
    };

    enum class MarketCurrency : int64_t
    {
        /// Any currency (default)
        Any = 0,
        /// Rouble
        Rouble = 1,
        /// US dollar
        Dollar = 2,
        /// Euro
        Euro = 3,
    };

    enum class MarketOwner : int64_t
    {
        /// Any owner (default)
        Any = 0,
        /// Item listed by traders
        Traders = 1,
        /// Item listed by players
        Player = 2,
    };

    // Market filter context
    struct MarketFilterBody
    {
        int64_t page{ 0 };
        int64_t limit{ 15 };
        MarketSortBy sort_type{ MarketSortBy::Price };
        MarketSortDirection sort_direction{ MarketSortDirection::Ascending };
        MarketCurrency currency{ MarketCurrency::Any };
        int64_t price_from{ 0 };
        int64_t price_to{ 0 };
        int64_t quantity_from{ 0 };
        int64_t quantity_to{ 0 };
        int64_t condition_from{ 0 };
        int64_t condition_to{ 100 };
        bool one_hour_expiration{ false };
        bool remove_bartering{ true };
        MarketOwner offer_owner_type{ MarketOwner::Any };
        bool only_functional{ true };
        bool update_offer_count{ true };
        std::string handbook_id;
        std::string linked_search_id;
        std::string needed_search_id;
        int64_t tm{ 1 };

        std::string dump()
        {
            auto out = fmt::format(
                "\nMarketFilterBody\n\t"
                "page: {}\n\t"
                "limit: {}\n\t"
                "sort_type: {}\n\t"
                "sort_direction: {}\n\t"
                "currency: {}\n\t"
                "price_from: {}\n\t"
                "price_to: {}\n\t"
                "quantity_from: {}\n\t"
                "quantity_to: {}\n\t"
                "condition_from: {}\n\t"
                "condition_to: {}\n\t"
                "one_hour_expiration: {}\n\t"
                "remove_bartering: {}\n\t"
                "offer_owner_type: {}\n\t"
                "only_functional: {}\n\t"
                "update_offer_count: {}\n\t"
                "handbook_id: {}\n\t"
                "linked_search_id: {}\n\t"
                "needed_search_id: {}\n\t"
                ,
                page, limit, sort_type, sort_direction, currency,
                price_from, price_to, quantity_from, quantity_to,
                condition_from, condition_to, one_hour_expiration,
                remove_bartering, offer_owner_type, only_functional,
                update_offer_count, handbook_id, linked_search_id,
                needed_search_id
            );
            return out;
        }

        auto set_page(int64_t val) { page = val; return *this; }
        auto set_limit(int64_t val) { limit = val; return *this; }
        auto set_sort_type(MarketSortBy val) { sort_type = val; return *this; }
        auto set_sort_direction(MarketSortDirection val) { sort_direction = val; return *this; }
        auto set_currency(MarketCurrency val) { currency = val; return *this; }
        auto set_price_from(int64_t val) { price_from = val; return *this; }
        auto set_price_to(int64_t val) { price_to = val; return *this; }
        auto set_quantity_from(int64_t val) { quantity_from = val; return *this; }
        auto set_quantity_to(int64_t val) { quantity_to = val; return *this; }
        auto set_condition_from(int64_t val) { condition_from = val; return *this; }
        auto set_condition_to(int64_t val) { condition_to = val; return *this; }
        auto set_one_hour_expiration(bool val) { one_hour_expiration = val; return *this; }
        auto set_remove_bartering(bool val) { remove_bartering = val; return *this; }
        auto set_offer_owner_type(MarketOwner val) { offer_owner_type = val; return *this; }
        auto set_only_functional(bool val) { only_functional = val; return *this; }
        auto set_update_offer_count(bool val) { update_offer_count = val; return *this; }
        auto set_handbook_id(std::string val) { handbook_id = val; return *this; }
        auto set_linked_search_id(std::string val) { linked_search_id = val; return *this; }
        auto set_needed_search_id(std::string val) { needed_search_id = val; return *this; }
        auto set_tm(int64_t val) { tm = val; return *this; }
    };
}


namespace nlohmann {
    namespace detail {
        void from_json(const json& j, quicktype::BuyOfferContext& x);
        void to_json(json& j, const quicktype::BuyOfferContext& x);

        void from_json(const json& j, quicktype::BuyDatumContext& x);
        void to_json(json& j, const quicktype::BuyDatumContext& x);

        void from_json(const json& j, quicktype::MarketBuyReqBody& x);
        void to_json(json& j, const quicktype::MarketBuyReqBody& x);

        void from_json(const json& j, quicktype::SellItemContext& x);
        void to_json(json& j, const quicktype::SellItemContext& x);

        void from_json(const json& j, quicktype::SellDatumContext& x);
        void to_json(json& j, const quicktype::SellDatumContext& x);

        void from_json(const json& j, quicktype::MarketSellReqBody& x);
        void to_json(json& j, const quicktype::MarketSellReqBody& x);

        void from_json(const json& j, quicktype::OfferRequirementContext& x);
        void to_json(json& j, const quicktype::OfferRequirementContext& x);

        void from_json(const json& j, quicktype::OfferDatumContext& x);
        void to_json(json& j, const quicktype::OfferDatumContext& x);

        void from_json(const json& j, quicktype::MarketOfferReqBody& x);
        void to_json(json& j, const quicktype::MarketOfferReqBody& x);

        void from_json(const json& j, quicktype::ItemStackDatum& x);
        void to_json(json& j, const quicktype::ItemStackDatum& x);

        void from_json(const json& j, quicktype::ItemStackBody& x);
        void to_json(json& j, const quicktype::ItemStackBody& x);

        void from_json(const json& j, quicktype::ItemTransferDatum& x);
        void to_json(json& j, const quicktype::ItemTransferDatum& x);

        void from_json(const json& j, quicktype::ItemTransferBody& x);
        void to_json(json& j, const quicktype::ItemTransferBody& x);

        void from_json(const json& j, quicktype::ItemMoveLocation& x);
        void to_json(json& j, const quicktype::ItemMoveLocation& x);

        void from_json(const json& j, quicktype::ItemMoveTo& x);
        void to_json(json& j, const quicktype::ItemMoveTo& x);

        void from_json(const json& j, quicktype::ItemMoveDatum& x);
        void to_json(json& j, const quicktype::ItemMoveDatum& x);

        void from_json(const json& j, quicktype::ItemMoveBody& x);
        void to_json(json& j, const quicktype::ItemMoveBody& x);

        void from_json(const json& j, quicktype::TraderBarterItem& x);
        void to_json(json& j, const quicktype::TraderBarterItem& x);

        void from_json(const json& j, quicktype::TradeItemDatum& x);
        void to_json(json& j, const quicktype::TradeItemDatum& x);

        void from_json(const json& j, quicktype::TradeItemBody& x);
        void to_json(json& j, const quicktype::TradeItemBody& x);

        inline void from_json(const json& j, quicktype::BuyOfferContext& x) {
            x.id = j.at("id").get<std::string>();
            x.count = j.at("count").get<int64_t>();
            x.items = j.at("items").get<std::vector<quicktype::TraderBarterItem>>();
        }

        inline void to_json(json& j, const quicktype::BuyOfferContext& x) {
            j = json::object();
            j["id"] = x.id;
            j["count"] = x.count;
            j["items"] = x.items;
        }

        inline void from_json(const json& j, quicktype::BuyDatumContext& x) {
            x.action = j.at("Action").get<std::string>();
            x.offers = j.at("offers").get<std::vector<quicktype::BuyOfferContext>>();
        }

        inline void to_json(json& j, const quicktype::BuyDatumContext& x) {
            j = json::object();
            j["Action"] = x.action;
            j["offers"] = x.offers;
        }

        inline void from_json(const json& j, quicktype::MarketBuyReqBody& x) {
            x.data = j.at("data").get<std::vector<quicktype::BuyDatumContext>>();
            x.tm = j.at("tm").get<int64_t>();
        }

        inline void to_json(json& j, const quicktype::MarketBuyReqBody& x) {
            j = json::object();
            j["data"] = x.data;
            j["tm"] = x.tm;
        }


        inline void from_json(const json& j, quicktype::SellItemContext& x) {
            x.id = j.at("id").get<std::string>();
            x.count = j.at("count").get<int64_t>();
            x.scheme_id = j.at("scheme_id").get<std::string>();
        }

        inline void to_json(json& j, const quicktype::SellItemContext& x) {
            j = json::object();
            j["id"] = x.id;
            j["count"] = x.count;
            j["scheme_id"] = x.scheme_id;
        }

        inline void from_json(const json& j, quicktype::SellDatumContext& x) {
            x.action = j.at("Action").get<std::string>();
            x.type = j.at("type").get<std::string>();
            x.tid = j.at("tid").get<std::string>();
            x.items = j.at("items").get<std::vector<quicktype::SellItemContext>>();
        }

        inline void to_json(json& j, const quicktype::SellDatumContext& x) {
            j = json::object();
            j["Action"] = x.action;
            j["type"] = x.type;
            j["tid"] = x.tid;
            j["items"] = x.items;
        }

        inline void from_json(const json& j, quicktype::MarketSellReqBody& x) {
            x.data = j.at("data").get<std::vector<quicktype::SellDatumContext>>();
            x.tm = j.at("tm").get<int64_t>();
        }

        inline void to_json(json& j, const quicktype::MarketSellReqBody& x) {
            j = json::object();
            j["data"] = x.data;
            j["tm"] = x.tm;
        }

        inline void from_json(const json& j, quicktype::OfferRequirementsContext& x) {
            x._tpl = j.at("_tpl").get<std::string>();
            x.count = j.at("count").get<int64_t>();
            x.level = j.at("level").get<int64_t>();
            x.side = j.at("side").get<int64_t>();
            x.onlyFunctional = j.at("onlyFunctional").get<bool>();
        }

        inline void to_json(json& j, const quicktype::OfferRequirementsContext& x) {
            j = json::object();
            j["_tpl"] = x._tpl;
            j["count"] = x.count;
            j["level"] = x.level;
            j["side"] = x.side;
            j["onlyFunctional"] = x.onlyFunctional;
        }

        inline void from_json(const json& j, quicktype::OfferDatumContext& x) {
            x.action = j.at("Action").get<std::string>();
            x.sell_in_one_piece = j.at("sellInOnePiece").get<bool>();
            x.items = j.at("items").get<std::vector<std::string>>();
            x.requirements = j.at("requirements").get<std::vector<quicktype::OfferRequirementsContext>>();
            x.tm = j.at("tm").get<int64_t>();
        }

        inline void to_json(json& j, const quicktype::OfferDatumContext& x) {
            j = json::object();
            j["Action"] = x.action;
            j["sellInOnePiece"] = x.sell_in_one_piece;
            j["items"] = x.items;
            j["requirements"] = x.requirements;
            j["tm"] = x.tm;
        }

        inline void from_json(const json& j, quicktype::MarketOfferReqBody& x) {
            x.data = j.at("data").get<std::vector<quicktype::OfferDatumContext>>();
        }

        inline void to_json(json& j, const quicktype::MarketOfferReqBody& x) {
            j = json::object();
            j["data"] = x.data;
        }

        inline void from_json(const json& j, quicktype::ItemStackDatum& x) {
            x.action = j.at("Action").get<std::string>();
            x.item = j.at("item").get<std::string>();
            x.with = j.at("with").get<std::string>();
        }

        inline void to_json(json& j, const quicktype::ItemStackDatum& x) {
            j = json::object();
            j["Action"] = x.action;
            j["item"] = x.item;
            j["with"] = x.with;
        }

        inline void from_json(const json& j, quicktype::ItemStackBody& x) {
            x.data = j.at("data").get<std::vector<quicktype::ItemStackDatum>>();
            x.tm = j.at("tm").get<int64_t>();
        }

        inline void to_json(json& j, const quicktype::ItemStackBody& x) {
            j = json::object();
            j["data"] = x.data;
            j["tm"] = x.tm;
        }

        inline void from_json(const json& j, quicktype::ItemTransferDatum& x) {
            x.action = j.at("Action").get<std::string>();
            x.item = j.at("item").get<std::string>();
            x.with = j.at("with").get<std::string>();
            x.count = j.at("count").get<int64_t>();
        }

        inline void to_json(json& j, const quicktype::ItemTransferDatum& x) {
            j = json::object();
            j["Action"] = x.action;
            j["item"] = x.item;
            j["with"] = x.with;
            j["count"] = x.count;
        }

        inline void from_json(const json& j, quicktype::ItemTransferBody& x) {
            x.data = j.at("data").get<std::vector<quicktype::ItemTransferDatum>>();
            x.tm = j.at("tm").get<int64_t>();
        }

        inline void to_json(json& j, const quicktype::ItemTransferBody& x) {
            j = json::object();
            j["data"] = x.data;
            j["tm"] = x.tm;
        }

        inline void from_json(const json& j, quicktype::ItemMoveLocation& x) {
            x.x = j.at("x").get<int64_t>();
            x.y = j.at("y").get<int64_t>();
            x.r = j.at("r").get<int64_t>();
        }

        inline void to_json(json& j, const quicktype::ItemMoveLocation& x) {
            j = json::object();
            j["x"] = x.x;
            j["y"] = x.y;
            j["r"] = x.r;
        }

        inline void from_json(const json& j, quicktype::ItemMoveTo& x) {
            x.id = j.at("id").get<std::string>();
            x.container = j.at("container").get<std::string>();
            x.location = j.at("location").get<quicktype::ItemMoveLocation>();
        }

        inline void to_json(json& j, const quicktype::ItemMoveTo& x) {
            j = json::object();
            j["id"] = x.id;
            j["container"] = x.container;
            j["location"] = x.location;
        }

        inline void from_json(const json& j, quicktype::ItemMoveDatum& x) {
            x.action = j.at("Action").get<std::string>();
            x.item = j.at("item").get<std::string>();
            x.to = j.at("to").get<quicktype::ItemMoveTo>();
        }

        inline void to_json(json& j, const quicktype::ItemMoveDatum& x) {
            j = json::object();
            j["Action"] = x.action;
            j["item"] = x.item;
            j["to"] = x.to;
        }

        inline void from_json(const json& j, quicktype::ItemMoveBody& x) {
            x.data = j.at("data").get<std::vector<quicktype::ItemMoveDatum>>();
            x.tm = j.at("tm").get<int64_t>();
        }

        inline void to_json(json& j, const quicktype::ItemMoveBody& x) {
            j = json::object();
            j["data"] = x.data;
            j["tm"] = x.tm;
        }

        inline void from_json(const json& j, quicktype::TraderBarterItem& x) {
            x._tpl = j.at("id").get<std::string>();
            x.count = j.at("count").get<double>();
        }

        inline void to_json(json& j, const quicktype::TraderBarterItem& x) {
            j = json::object();
            j["id"] = x._tpl;
            j["count"] = x.count;
        }

        inline void from_json(const json& j, quicktype::TradeItemDatum& x) {
            x.action = j.at("Action").get<std::string>();
            x.type = j.at("type").get<std::string>();
            x.tid = j.at("tid").get<std::string>();
            x.item_id = j.at("item_id").get<std::string>();
            x.count = j.at("count").get<int64_t>();
            x.scheme_id = j.at("scheme_id").get<int64_t>();
            x.scheme_items = j.at("scheme_items").get<std::vector<quicktype::TraderBarterItem>>();
        }

        inline void to_json(json& j, const quicktype::TradeItemDatum& x) {
            j = json::object();
            j["Action"] = x.action;
            j["type"] = x.type;
            j["tid"] = x.tid;
            j["item_id"] = x.item_id;
            j["count"] = x.count;
            j["scheme_id"] = x.scheme_id;
            j["scheme_items"] = x.scheme_items;
        }

        inline void from_json(const json& j, quicktype::TradeItemBody& x) {
            x.data = j.at("data").get<std::vector<quicktype::TradeItemDatum>>();
            x.tm = j.at("tm").get<int64_t>();
        }

        inline void to_json(json& j, const quicktype::TradeItemBody& x) {
            j = json::object();
            j["data"] = x.data;
            j["tm"] = x.tm;
        }

        inline void from_json(const json& j, quicktype::TraderItemUpd& x) {
            x.stack_objects_count = j.at("StackObjectsCount").get<int64_t>();
            x.unlimited_count = quicktype::get_optional<bool>(j, "UnlimitedCount");
            x.buy_restriction_current = quicktype::get_optional<int64_t>(j, "BuyRestrictionCurrent");
            x.buy_restriction_max = quicktype::get_optional<int64_t>(j, "BuyRestrictionMax");
        }

        inline void to_json(json& j, const quicktype::TraderItemUpd& x) {
            j = json::object();
            j["StackObjectsCount"] = x.stack_objects_count;
            j["UnlimitedCount"] = x.unlimited_count;
            j["BuyRestrictionCurrent"] = x.buy_restriction_current;
            j["BuyRestrictionMax"] = x.buy_restriction_max;
        }
    }
}

namespace TarkovAPI
{
    static std::string GAME_VERSION = "0.12.3.5834";
    static std::string LAUNCHER_VERSION = "0.9.3.1023";
    static std::string UNITY_VERSION = "2018.4.13f1";

    static constexpr auto LAUNCHER_ENDPOINT = "https://launcher.escapefromtarkov.com";
    static constexpr auto PROD_ENDPOINT = "https://prod.escapefromtarkov.com";
    static constexpr auto TRADING_ENDPOINT = "https://trading.escapefromtarkov.com";
    static constexpr auto RAGFAIR_ENDPOINT = "https://ragfair.escapefromtarkov.com";

    static constexpr auto ROUBLE_ITEM_ID = "5449016a4bdc2d6f028b456f";
    static constexpr auto USD_ITEM_ID = "5696686a4bdc2da3298b456a";
    static constexpr auto EURO_ITEM_ID = "569668774bdc2da2298b4568";

    enum ErrorCodes
    {
        OK = 0,
        NotAuthorized = 201,
        InvalidUserID = 205,
        BadLogin = 206,
        InvalidParameters = 207,
        TwoFactorRequired = 209,
        BadTwoFactorCode = 211,
        CaptchaRequired = 214,
        InvalidBarterItems = 228,
        RateLimited = 230,
        Maintenance = 263,
        BackendError = 1000,
        MaxOfferCount = 1501,
        InsufficientTaxFunds = 1502,
        OfferNotFound = 1507,
        BadLoyaltyLevel = 1510,
        OfferNotAvailableYet = 1512,
        TransactionError = 1514
    };

    enum Error : uint8_t
    {
        InvalidParameter,
        CprApiFailed,
        CprSessionFailed,
        CprPostFailed,
        ZlibDecompressFailed,
        JsonParseFailed,
        JsonBadFormat,
        ApiServerError,
        KeepAliveFailed,
        SelectProfileFail,
        VersionCheckError,
        ResponseHandleFailed,
        MarketBadRequest,
        NullDataForParse,
        TraderNotFound,
        ItemNotFound
    };

    inline quicktype::ResponseBody parse_response(const nlohmann::json& j)
    {
        quicktype::ResponseBody x{};
        x.data = quicktype::get_untyped(j, "data");
        x.err = j.at("err").get<int64_t>();
        x.errmsg = j.at("errmsg").is_string() ? j.at("errmsg").get<std::string>() : "";
        return x;
    }

    inline nlohmann::json serialize_market_finder(const quicktype::MarketFilterBody& x)
    {
        auto j = nlohmann::json::object();
        j["page"] = x.page;
        j["limit"] = x.limit;
        j["sortType"] = x.sort_type;
        j["sortDirection"] = x.sort_direction;
        j["currency"] = x.currency;
        j["priceFrom"] = x.price_from;
        j["priceTo"] = x.price_to;
        j["quantityFrom"] = x.quantity_from;
        j["quantityTo"] = x.quantity_to;
        j["conditionFrom"] = x.condition_from;
        j["conditionTo"] = x.condition_to;
        j["oneHourExpiration"] = x.one_hour_expiration;
        j["removeBartering"] = x.remove_bartering;
        j["offerOwnerType"] = x.offer_owner_type;
        j["onlyFunctional"] = x.only_functional;
        j["updateOfferCount"] = x.update_offer_count;
        j["handbookId"] = x.handbook_id;
        j["linkedSearchId"] = x.linked_search_id;
        j["neededSearchId"] = x.needed_search_id;
        j["tm"] = x.tm;
        return j;
    }

    inline nlohmann::json serialize_market_buy_request(const quicktype::MarketBuyReqBody& x)
    {
        auto j = nlohmann::json::object();
        j["data"] = x.data;
        j["tm"] = x.tm;
        return j;
    }

    inline nlohmann::json serialize_market_sell_request(const quicktype::MarketSellReqBody& x)
    {
        auto j = nlohmann::json::object();
        j["data"] = x.data;
        j["tm"] = x.tm;
        return j;
    }

    inline nlohmann::json serialize_market_offer_request(const quicktype::MarketOfferReqBody& x)
    {
        auto j = nlohmann::json::object();
        j["data"] = x.data;
        return j;
    }

    inline nlohmann::json serialize_item_merge_request(const quicktype::ItemStackBody& x)
    {
        auto j = nlohmann::json::object();
        j["data"] = x.data;
        j["tm"] = x.tm;
        return j;
    }
    inline nlohmann::json serialize_item_transfer_request(const quicktype::ItemTransferBody& x)
    {
        auto j = nlohmann::json::object();
        j["data"] = x.data;
        j["tm"] = x.tm;
        return j;
    }

    inline nlohmann::json serialize_item_move_request(const quicktype::ItemMoveBody& x)
    {
        auto j = nlohmann::json::object();
        j["data"] = x.data;
        j["tm"] = x.tm;
        return j;
    }

    inline nlohmann::json serialize_trade_item_request(const quicktype::TradeItemBody& x)
    {
        auto j = nlohmann::json::object();
        j["data"] = x.data;
        j["tm"] = x.tm;
        return j;
    }

    inline quicktype::TraderItem parse_trader_item(const nlohmann::json& j)
    {
        quicktype::TraderItem x{};
        x._id = j.at("_id").get<std::string>();
        x._tpl = j.at("_tpl").get<std::string>();
        x.upd = quicktype::get_optional<quicktype::TraderItemUpd>(j, "upd");
        return x;
    }
};
