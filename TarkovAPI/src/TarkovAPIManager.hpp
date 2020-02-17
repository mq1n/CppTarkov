#pragma once
#include <cstdint>
#include <memory>
#include <string>
#include <vector>
#include <map>
#include <sstream>
#include <functional>
#include <ctime>
#include <limits>

#include "Constants.hpp"
#include "Exception.hpp"
#include "StashHelper.hpp"

#include <cpr/cpr.h>
#include <json.hpp>

namespace TarkovAPI
{
	using json = nlohmann::json;

	class TarkovAPIManager
	{
	public:
		virtual ~TarkovAPIManager();

		TarkovAPIManager(const TarkovAPIManager&) = delete;
		TarkovAPIManager(TarkovAPIManager&&) noexcept = delete;
		TarkovAPIManager& operator=(const TarkovAPIManager&) = delete;
		TarkovAPIManager& operator=(TarkovAPIManager&&) noexcept = delete;

	public:
		TarkovAPIManager();

		bool InitializeTarkovAPIManager();
		bool FinalizeTarkovAPIManager();

		void Log(const std::string& func, int32_t level, const std::string& data);

		static TarkovAPIManager* InstancePtr();
		static TarkovAPIManager& Instance();

		std::string Generate_Random_Hwid();
		quicktype::ResponseBody Post_Json(const std::string& url, const std::string& body = "");

		bool OnResponseHandle(const std::string& func, int64_t error_code, const std::string& data);

		void Login(const std::string& email, const std::string& password, const std::string& hwid, const std::string& captcha = "");
		void Login_2FA(const std::string& email, const std::string& password, const std::string& code, const std::string& hwid);
		void Login_Captcha(const std::string& email, const std::string& password, const std::string& captcha, const std::string& hwid);
		void Login_Token(const std::string& token, const std::string& hwid);
		void Login_Session(const std::string& session, const std::string& hwid);

		void KeepAlive();
		json GetI18n(const std::string& language);
		json GetWeather();

		json GetProfiles();
		json GetMyProfile();
		void SelectProfile(const std::string& user_id);
		json GetFriends();

		json GetTraders();
		json GetTrader(const std::string& trader_id);
		std::string GetTraderIdByName(const std::string& name);
		json GetTraderItemsRaw(const std::string& trader_id);
		json GetTraderPricesRaw(const std::string& trader_id);
		json SellItem(const std::string& trader_id, const std::string& item_id, int64_t quantity);
		json TradeItem(const std::string& trader_id, const std::string& item_id, int64_t quantity, const std::vector <quicktype::TraderBarterItem>& barter_items);
		std::vector <quicktype::TraderItem> GetTraderItems(const std::string& trader_id);

		json SearchMarket(const quicktype::MarketFilterBody& filter);
		json BuyItem(const std::string& offer_id, int64_t quantity, const std::vector <quicktype::TraderBarterItem>& barter_items);
		json OfferItem(const std::vector <std::string>& items, const quicktype::OfferRequirementContext& requirement, bool sell_all = false);
		json GetItemPrice(const std::string& schema_id);

		json StackItem(const std::string& from_item_id, const std::string& to_item_id, int64_t count = 0);
		json MoveItem(const std::string& item_id, const quicktype::ItemMoveTo& destination);

		json GetMailList();
		json GetMail(const std::string& mail_id, int64_t type);
		json GetMailAttachments(const std::string& mail_id);
		json GetMailReward(const std::string& from_item_id, const std::string& to_item_id, const std::string& previous_owner_id, const quicktype::MailRewardToLocation& to_stash_location);

		double GetItemPrice(double listed_price, int64_t amount);
		json GetItems();
		json GetItemPrices();
		json GetLocations();
		json GetMyItems();
		uint64_t GetRoubleCount();
		std::vector <quicktype::TraderBarterItem> FindItemStack(const std::string& schema_id, uint64_t required = 1);
		std::string GetMainStashID();
		quicktype::ItemMoveLocation FindBlankStashPos();
		std::string GetItemName(const std::string& schema_id);

	private:
		cpr::Session* m_pkClient;
		std::string m_stHwid;
		std::string m_stSessionID;

		std::map <std::string /* locale_id */, json /* dump */> m_pkJsonLocale;
		json m_pkJsonItems;
		json m_pkJsonItemPrices;
		json m_pkJsonLocations;

		int64_t m_nReqCounter{ 1 };
	};
};
