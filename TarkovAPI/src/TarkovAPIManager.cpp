#include "Hwid.hpp"
#include "TarkovAPIManager.hpp"
#include "Auth.hpp"
#include "LogHelper.hpp"
#include "Constants.hpp"
#include "Exception.hpp"
#include <cassert>
#include <json.hpp>

namespace TarkovAPI
{
    static TarkovAPIManager* gs_pAPIInstance = nullptr;


    TarkovAPIManager* TarkovAPIManager::InstancePtr()
    {
        return gs_pAPIInstance;
    }
    TarkovAPIManager& TarkovAPIManager::Instance()
    {
        assert(gs_pAPIInstance);
        return *gs_pAPIInstance;
    }

    TarkovAPIManager::TarkovAPIManager()
    {
        assert(!gs_pAPIInstance);

        gs_pAPIInstance = this;
        m_pkClient = nullptr;
    }
    TarkovAPIManager::~TarkovAPIManager()
    {
        assert(gs_pAPIInstance == this);

        gs_pAPIInstance = nullptr;
    }


    bool TarkovAPIManager::InitializeTarkovAPIManager()
    {
        assert(!gs_pAPILogInstance);

        DeleteFileA(CUSTOM_LOG_FILENAME);

        gs_pAPILogInstance = new CLog("EFTAPILogger", CUSTOM_LOG_FILENAME);
        if (!gs_pAPILogInstance)
        {
            Logf(CUSTOM_LOG_ERROR_FILENAME, "TarkovAPIManager::InitializeTarkovAPIManager: Log manager inilization fail!");
            return false;
        }

        m_pkClient = new cpr::Session();
        if (!m_pkClient)
        {
            gs_pAPILogInstance->Log(__FUNCTION__, LL_ERR, "CURL Session could not created!");
            return false;
        }

        auth::CheckLauncherVersion(m_pkClient);
        auth::CheckGameVersion(m_pkClient);

        gs_pAPILogInstance->Log(__FUNCTION__, LL_SYS, fmt::format("API Manager Initialized! Build: {}", __TIMESTAMP__));
        return true;
    }
    bool TarkovAPIManager::FinalizeTarkovAPIManager()
    {
        if (m_pkClient)
        {
            delete m_pkClient;
            m_pkClient = nullptr;
        }
        if (gs_pAPILogInstance)
        {
            gs_pAPILogInstance->Log(__FUNCTION__, LL_SYS, "API Manager finalized!");

            delete gs_pAPILogInstance;
            gs_pAPILogInstance = nullptr;
        }
        return true;
    }

    void TarkovAPIManager::Log(const std::string& func, int32_t level, const std::string& data)
    {
        if (gs_pAPILogInstance)
        {
            gs_pAPILogInstance->Log(func, level, data);
        }
    }


    std::string TarkovAPIManager::Generate_Random_Hwid()
    {
        auto stHwid = hwid::generate_hwid();
        assert(stHwid.size() == 258 && "Hwid bad format");

        return stHwid;
    }

    quicktype::ResponseBody TarkovAPIManager::Post_Json(const std::string& url, const std::string& body)
    {
        assert(m_pkClient && "Null m_pkClient");

        Log(__FUNCTION__, LL_DEV, fmt::format("Request: {} To: {}", body, url));

        m_pkClient->SetUrl(url);
        m_pkClient->SetBody(cpr::Body{ body });

        auto headers = cpr::Header{
            {"Content-Type", "application/json"},
            {"User-Agent", fmt::format("UnityPlayer/{} (UnityWebRequest/1.0, libcurl/7.52.0-DEV)", UNITY_VERSION)},
            {"App-Version", fmt::format("EFT Client {}", GAME_VERSION)},
            {"X-Unity-Version", UNITY_VERSION},
            {"Cookie", fmt::format("PHPSESSID={}", m_stSessionID)},
            {"GClient-RequestId", fmt::format("{}", m_nReqCounter++)}
        };
        m_pkClient->SetHeader(headers);

        auto res = m_pkClient->Post();

        switch (res.error.code) // CPR error code
        {
        case cpr::ErrorCode::OK:
            break;
        default:
            throw TarkovAPIException(Error::CprApiFailed, res.error.message);
        }

        switch (res.status_code) // HTTP status code
        {
        case 200: // OK
            break;
        default:
            throw TarkovAPIException(Error::CprPostFailed, res.status_code);
        }

        std::vector <uint8_t> decompresseddata(res.text.size() * 100);

        auto decompressedsize = static_cast<uLongf>(decompresseddata.size());
        auto ret = uncompress(
            reinterpret_cast<Bytef*>(&decompresseddata[0]), &decompressedsize,
            reinterpret_cast<const Bytef*>(res.text.data()), static_cast<uLong>(res.text.size())
        );
        if (Z_OK != ret)
        {
            throw TarkovAPIException(Error::ZlibDecompressFailed, ret);
        }

        json deserialized{};
        try
        {
            deserialized = json::parse(decompresseddata.data());
        }
        catch (const json::exception & ex)
        {
            std::stringstream ss;
            ss << "Message: " << ex.what() << '\n' << "exception id: " << ex.id << std::endl;

            throw TarkovAPIException(Error::JsonParseFailed, ss.str());
        }

        Log(__FUNCTION__, LL_DEV, fmt::format("Response: {}", deserialized.dump()));

        if (!deserialized.contains("err") || !deserialized.contains("errmsg"))
        {
            throw TarkovAPIException(Error::JsonBadFormat, "'err' or 'errmsg' key is not available");
        }
        else if (!deserialized.contains("data"))
        {
            throw TarkovAPIException(Error::JsonBadFormat, "'data' key is not available");
        }

        return parse_response(deserialized);
    }


    bool TarkovAPIManager::OnResponseHandle(const std::string& func, int64_t error_code, const std::string& data)
    {
        switch (error_code)
        {
        case ErrorCodes::OK:
        {
            Log(func, LL_DEV, "Request completed!");
        } return true;

        case ErrorCodes::NotAuthorized:
        {
            Log(func, LL_ERR, fmt::format("Func: {} - Error: {}", func, "Not authorized or game profile not selected"));
        } break;
        case ErrorCodes::InvalidUserID:
        {
            Log(func, LL_ERR, fmt::format("Func: {} - Error: {}", func, "Invalid user ID selected."));
        } break;
        case ErrorCodes::InvalidParameters:
        {
            Log(func, LL_ERR, fmt::format("Func: {} - Error: {}", func, "Invalid or missing parameters."));
        } break;
        case ErrorCodes::InvalidBarterItems:
        {
            Log(func, LL_ERR, fmt::format("Func: {} - Error: {}", func, "Provided `BarterItem` is invalid, not enough quantities available or not found."));
        } break;
        case ErrorCodes::Maintenance:
        {
            Log(func, LL_ERR, fmt::format("Func: {} - Error: {}", func, "EFT API is down for maintenance."));
        } break;
        case ErrorCodes::BackendError:
        {
            Log(func, LL_ERR, fmt::format("Func: {} - Error: {}", func, "Backend error. No other information is given."));
        } break;
        case ErrorCodes::MaxOfferCount:
        {
            Log(func, LL_ERR, fmt::format("Func: {} - Error: {}", func, "Maximum outstanding offer count of 3 was reached."));
        } break;
        case ErrorCodes::InsufficientTaxFunds:
        {
            Log(func, LL_ERR, fmt::format("Func: {} - Error: {}", func, "Insufficient funds to pay the flea market fee."));
        } break;
        case ErrorCodes::OfferNotFound:
        {
            Log(func, LL_ERR, fmt::format("Func: {} - Error: {}", func, "Offer not found, sold out or out of stock."));
        } break;
        case ErrorCodes::BadLoyaltyLevel:
        {
            Log(func, LL_ERR, fmt::format("Func: {} - Error: {}", func, "Loyalty level is not high enough to purchase this item."));
        } break;
        case ErrorCodes::OfferNotAvailableYet:
        {
            Log(func, LL_ERR, fmt::format("Func: {} - Error: {}", func, "Offer is not available yet."));
        } break;
        case ErrorCodes::TransactionError:
        {
            Log(func, LL_ERR, fmt::format("Func: {} - Error: {}", func, "Transaction error"));
        } break;

        default:
            Log(func, LL_ERR, fmt::format("Func: {} - Unknown error code: {} Data: {}", func, error_code, data));
            break;
        }
        return false;
    }


    void TarkovAPIManager::Login_Session(const std::string& session, const std::string& hwid)
    {
        if (session.empty() || hwid.empty())
        {
            throw TarkovAPIException(Error::InvalidParameter);
        }

        if (!m_pkClient)
        {
            throw TarkovAPIException(Error::CprSessionFailed);
        }

        m_stHwid = hwid;
        m_stSessionID = session;

        Log(__FUNCTION__, LL_SYS, fmt::format("Login succesfully completed! Hardware ID: {} Session ID: {}", m_stHwid, m_stSessionID));
    }

    void TarkovAPIManager::Login_Token(const std::string& token, const std::string& hwid)
    {
        if (token.empty() || hwid.empty())
        {
            throw TarkovAPIException(Error::InvalidParameter);
        }

        if (!m_pkClient)
        {
            throw TarkovAPIException(Error::CprSessionFailed);
        }

        auto session = auth::ExchangeAccessToken(m_pkClient, token, hwid);
        if (session.empty())
        {
            throw TarkovAPIException(Error::JsonBadFormat, "Null json data(session)");
        }

        json session_ctx{};
        try
        {
            session_ctx = json::parse(session.data());
        }
        catch (const json::parse_error & ex)
        {
            std::stringstream ss;
            ss << "JSON data(session) could not parsed: Error:" << "Message: " << ex.what() << '\n' << "exception id: " << ex.id << '\n' << "byte position of error: " << ex.byte << std::endl;

            throw TarkovAPIException(Error::JsonParseFailed, ss.str());
        }
        if (!session_ctx.contains("session"))
        {
            throw TarkovAPIException(Error::JsonParseFailed, "'session' key is not available");
        }

        Login_Session(session_ctx["session"].get<std::string>(), hwid);
    }

    void TarkovAPIManager::Login(const std::string& email, const std::string& password, const std::string& hwid, const std::string& captcha)
    {
        if (email.empty() || password.empty() || hwid.empty())
        {
            throw TarkovAPIException(Error::InvalidParameter);
        }

        if (!m_pkClient)
        {
            throw TarkovAPIException(Error::CprSessionFailed);
        }

        auto user = auth::LoginImpl(m_pkClient, email, password, captcha, hwid);
        if (user.empty())
        {
            throw TarkovAPIException(Error::JsonBadFormat, "Null json data(user)");
        }

        json user_ctx{};
        try
        {
            user_ctx = json::parse(user.data());
        }
        catch (const json::parse_error & ex)
        {
            std::stringstream ss;
            ss << "JSON data(user) could not parsed: Error:" << "Message: " << ex.what() << '\n' << "exception id: " << ex.id << '\n' << "byte position of error: " << ex.byte << std::endl;

            throw TarkovAPIException(Error::JsonParseFailed, ss.str());
        }
        if (!user_ctx.contains("access_token"))
        {
            throw TarkovAPIException(Error::JsonParseFailed, "'access_token' key is not available");
        }

        Login_Token(user_ctx["access_token"].get<std::string>(), hwid);
    }

    void TarkovAPIManager::Login_2FA(const std::string& email, const std::string& password, const std::string& code, const std::string& hwid)
    {
        if (email.empty() || password.empty() || code.empty() || hwid.empty())
        {
            throw TarkovAPIException(Error::InvalidParameter);
        }

        if (!m_pkClient)
        {
            throw TarkovAPIException(Error::CprSessionFailed);
        }

        auth::ActivateHardware(m_pkClient, email, code, hwid);

        Login(email, password, hwid);
    }

    void TarkovAPIManager::Login_Captcha(const std::string& email, const std::string& password, const std::string& captcha, const std::string& hwid)
    {
        if (email.empty() || password.empty() || captcha.empty() || hwid.empty())
        {
            throw TarkovAPIException(Error::InvalidParameter);
        }

        if (!m_pkClient)
        {
            throw TarkovAPIException(Error::CprSessionFailed);
        }

        Login(email, password, hwid, captcha);
    }


    void TarkovAPIManager::KeepAlive()
    {
        auto url = fmt::format(
            "{}/client/game/keepalive",
            PROD_ENDPOINT
        );

        auto res = Post_Json(url);

        switch (res.err)
        {
        case ErrorCodes::OK:
            break;
        default:
            throw TarkovAPIException(Error::KeepAliveFailed, res.err);
            break;
        }
    }

    json TarkovAPIManager::GetProfiles()
    {
        auto url = fmt::format(
            "{}/client/game/profile/list",
            PROD_ENDPOINT
        );

        auto res = Post_Json(url);

        if (!OnResponseHandle(__FUNCTION__, res.err, res.data.dump()))
        {
            throw TarkovAPIException(Error::ResponseHandleFailed, res.errmsg);
        }

        return res.data;
    }

    json TarkovAPIManager::GetMyProfile()
    {
        json out{};

        auto profiles = GetProfiles();
        if (profiles.empty())
        {
            throw TarkovAPIException(Error::ResponseHandleFailed, "GetProfiles null data");
        }

        try
        {
            auto it = std::find_if(profiles.begin(), profiles.end(), [](const nlohmann::json& x)
                {
                    return (
                        x.is_object() &&
                        x.contains("Info") && x["Info"].is_object() &&
                        x["Info"].contains("Side") && x["Info"]["Side"].is_string() &&
                        x["Info"]["Side"].get<std::string>() != "Savage"
                        );
                });

            out = nlohmann::json::parse(it->dump());
        }
        catch (const json::exception & ex)
        {
            Log(__FUNCTION__, LL_ERR, fmt::format("An exception handled! Data: {}", ex.what()));
        }

        return out;
    }

    void TarkovAPIManager::SelectProfile(const std::string& user_id)
    {
        if (user_id.empty())
        {
            throw TarkovAPIException(Error::InvalidParameter);
        }

        auto url = fmt::format(
            "{}/client/game/profile/select",
            PROD_ENDPOINT
        );

        json body{};
        body["uid"] = user_id;

        auto res = Post_Json(url, body.dump());

        if (!OnResponseHandle(__FUNCTION__, res.err, res.data.dump()))
        {
            throw TarkovAPIException(Error::SelectProfileFail);
        }
    }

    json TarkovAPIManager::GetFriends()
    {
        auto url = fmt::format(
            "{}/client/friend/list",
            PROD_ENDPOINT
        );

        auto res = Post_Json(url);

        if (!OnResponseHandle(__FUNCTION__, res.err, res.data.dump()))
        {
            throw TarkovAPIException(Error::ResponseHandleFailed, res.errmsg);
        }
        return res.data;
    }

    json TarkovAPIManager::GetI18n(const std::string& language)
    {
        if (language.empty())
        {
            throw TarkovAPIException(Error::InvalidParameter);
        }

        auto it = m_pkJsonLocale.find(language);
        if (it != m_pkJsonLocale.end())
        {
            return it->second;
        }

        auto url = fmt::format(
            "{}/client/locale/{}",
            PROD_ENDPOINT, language
        );

        auto res = Post_Json(url);

        if (!OnResponseHandle(__FUNCTION__, res.err, res.data.dump()))
        {
            throw TarkovAPIException(Error::ResponseHandleFailed, res.errmsg);
        }

        m_pkJsonLocale[language] = res.data;
        return m_pkJsonLocale[language];
    }

    json TarkovAPIManager::GetTraders()
    {
        auto url = fmt::format(
            "{}/client/trading/api/getTradersList",
            TRADING_ENDPOINT
        );

        auto res = Post_Json(url);

        if (!OnResponseHandle(__FUNCTION__, res.err, res.data.dump()))
        {
            throw TarkovAPIException(Error::ResponseHandleFailed, res.errmsg);
        }
        return res.data;
    }

    json TarkovAPIManager::GetTrader(const std::string& trader_id)
    {
        if (trader_id.empty())
        {
            throw TarkovAPIException(Error::InvalidParameter);
        }

        auto url = fmt::format(
            "{}/client/trading/api/getTrader/{}",
            TRADING_ENDPOINT, trader_id
        );

        auto res = Post_Json(url);

        if (!OnResponseHandle(__FUNCTION__, res.err, res.data.dump()))
        {
            throw TarkovAPIException(Error::ResponseHandleFailed, res.errmsg);
        }
        return res.data;
    }

    std::string TarkovAPIManager::GetTraderIdByName(const std::string& name)
    {
        auto locale = GetI18n("en");
        if (locale.empty())
        {
            throw TarkovAPIException(Error::NullDataForParse, "locale");
        }

        auto target = std::string();
        auto iter = std::find_if(locale.begin(), locale.end(), [&target, &name](const nlohmann::json& root)
            {
                if (root.is_object())
                {
                    for (const auto& it : root.items())
                    {
                        if (it.value().is_object())
                        {
                            for (const auto& it2 : it.value().items())
                            {
                                if (it2.key() == "Nickname" && it2.value() == name)
                                {
                                    target = it.key();
                                    return true;
                                }
                            }
                        }
                    }
                }
                return false;
            });

        if (iter == locale.end())
        {
            throw TarkovAPIException(Error::TraderNotFound, name);
        }

        return target;
    }

    json TarkovAPIManager::GetTraderItemsRaw(const std::string& trader_id)
    {
        if (trader_id.empty())
        {
            throw TarkovAPIException(Error::InvalidParameter);
        }

        auto url = fmt::format(
            "{}/client/trading/api/getTraderAssort/{}",
            TRADING_ENDPOINT, trader_id
        );

        auto res = Post_Json(url);

        if (!OnResponseHandle(__FUNCTION__, res.err, res.data.dump()))
        {
            throw TarkovAPIException(Error::ResponseHandleFailed, res.errmsg);
        }
        return res.data;
    }

    json TarkovAPIManager::GetTraderPricesRaw(const std::string& trader_id)
    {
        if (trader_id.empty())
        {
            throw TarkovAPIException(Error::InvalidParameter);
        }

        auto url = fmt::format(
            "{}/client/trading/api/getUserAssortPrice/trader/{}",
            TRADING_ENDPOINT, trader_id
        );

        auto res = Post_Json(url);

        if (!OnResponseHandle(__FUNCTION__, res.err, res.data.dump()))
        {
            throw TarkovAPIException(Error::ResponseHandleFailed, res.errmsg);
        }
        return res.data;
    }

    json TarkovAPIManager::GetWeather()
    {
        auto url = fmt::format(
            "{}/client/weather",
            PROD_ENDPOINT
        );

        auto res = Post_Json(url);

        if (!OnResponseHandle(__FUNCTION__, res.err, res.data.dump()))
        {
            throw TarkovAPIException(Error::ResponseHandleFailed, res.errmsg);
        }
        return res.data;
    }

    json TarkovAPIManager::GetItems()
    {
        if (!m_pkJsonItems.empty())
        {
            return m_pkJsonItems;
        }

        auto url = fmt::format(
            "{}/client/items",
            PROD_ENDPOINT
        );

        json body{};
        body["crc"] = 0;

        auto res = Post_Json(url, body.dump());

        if (!OnResponseHandle(__FUNCTION__, res.err, res.data.dump()))
        {
            throw TarkovAPIException(Error::ResponseHandleFailed, res.errmsg);
        }
        m_pkJsonItems = res.data;
        return m_pkJsonItems;
    }

    json TarkovAPIManager::GetItemPrices()
    {
        if (!m_pkJsonItemPrices.empty())
        {
            return m_pkJsonItemPrices;
        }

        auto url = fmt::format(
            "{}/client/items/prices",
            PROD_ENDPOINT
        );

        json body{};
        body["crc"] = 0;

        auto res = Post_Json(url, body.dump());

        if (!OnResponseHandle(__FUNCTION__, res.err, res.data.dump()))
        {
            throw TarkovAPIException(Error::ResponseHandleFailed, res.errmsg);
        }
        m_pkJsonItemPrices = res.data;
        return m_pkJsonItemPrices;
    }

    json TarkovAPIManager::GetMailList()
    {
        auto url = fmt::format(
            "{}/client/mail/dialog/list",
            PROD_ENDPOINT
        );

        json body{};
        body["crc"] = 0;

        auto res = Post_Json(url, body.dump());

        if (!OnResponseHandle(__FUNCTION__, res.err, res.data.dump()))
        {
            throw TarkovAPIException(Error::ResponseHandleFailed, res.errmsg);
        }
        return res.data;
    }

    json TarkovAPIManager::GetMail(const std::string& mail_id, int64_t type)
    {
        auto url = fmt::format(
            "{}/client/mail/dialog/view",
            PROD_ENDPOINT
        );

        json body{};
        body["dialogId"] = mail_id;
        body["type"] = type;

        auto res = Post_Json(url, body.dump());

        if (!OnResponseHandle(__FUNCTION__, res.err, res.data.dump()))
        {
            throw TarkovAPIException(Error::ResponseHandleFailed, res.errmsg);
        }
        return res.data;
    }

    json TarkovAPIManager::GetMailAttachments(const std::string& mail_id)
    {
        auto url = fmt::format(
            "{}/client/mail/dialog/getAllAttachments",
            PROD_ENDPOINT
        );

        json body{};
        body["dialogId"] = mail_id;

        auto res = Post_Json(url, body.dump());

        if (!OnResponseHandle(__FUNCTION__, res.err, res.data.dump()))
        {
            throw TarkovAPIException(Error::ResponseHandleFailed, res.errmsg);
        }
        return res.data;
    }

    json TarkovAPIManager::GetMailReward(const std::string& from_item_id, const std::string& to_item_id, const std::string& previous_owner_id, const quicktype::MailRewardToLocation& to_stash_location)
    {
        if (from_item_id.empty() || to_item_id.empty() || previous_owner_id.empty())
        {
            throw TarkovAPIException(Error::InvalidParameter);
        }

        auto url = fmt::format(
            "{}/client/game/profile/items/moving",
            PROD_ENDPOINT
        );

        auto body = quicktype::MailRewardBody
        {
            std::vector <quicktype::MailRewardDatum>
            {
                quicktype::MailRewardDatum
                {
                    "Move",
                    from_item_id,
                    quicktype::MailRewardTo
                    {
                        to_item_id,
                        "hideout",
                        to_stash_location
                    },
                    quicktype::MailFromOwner
                    {
                        previous_owner_id,
                        "Mail"
                    }
                }
            },
            2
        };
        
        auto req = serialize_get_mail_reward(body).dump();
        auto res = Post_Json(url, req);

        if (!OnResponseHandle(__FUNCTION__, res.err, res.data.dump()))
        {
            throw TarkovAPIException(Error::ResponseHandleFailed, res.errmsg);
        }
        if (res.data.contains("badRequest") && !res.data["badRequest"].empty())
        {
            throw TarkovAPIException(Error::MarketBadRequest, res.data["badRequest"].dump());
        }
        return res.data;
    }

    json TarkovAPIManager::GetLocations()
    {
        if (!m_pkJsonLocations.empty())
        {
            return m_pkJsonLocations;
        }

        auto url = fmt::format(
            "{}/client/locations",
            PROD_ENDPOINT
        );

        json body{};
        body["crc"] = 0;

        auto res = Post_Json(url, body.dump());

        if (!OnResponseHandle(__FUNCTION__, res.err, res.data.dump()))
        {
            throw TarkovAPIException(Error::ResponseHandleFailed, res.errmsg);
        }
        m_pkJsonLocations = res.data;
        return m_pkJsonLocations;
    }

    json TarkovAPIManager::SearchMarket(const quicktype::MarketFilterBody& filter)
    {
        if (!filter.limit)
        {
            throw TarkovAPIException(Error::InvalidParameter);
        }

        auto url = fmt::format(
            "{}/client/ragfair/find",
            RAGFAIR_ENDPOINT
        );

        auto res = Post_Json(url, serialize_market_finder(filter).dump());

        if (!OnResponseHandle(__FUNCTION__, res.err, res.data.dump()))
        {
            throw TarkovAPIException(Error::ResponseHandleFailed, res.errmsg);
        }
        return res.data;
    }

    json TarkovAPIManager::BuyItem(const std::string& offer_id, int64_t quantity, const std::vector <quicktype::TraderBarterItem>& barter_items)
    {
        if (offer_id.empty() || !quantity || barter_items.empty())
        {
            throw TarkovAPIException(Error::InvalidParameter);
        }

        auto url = fmt::format(
            "{}/client/game/profile/items/moving",
            PROD_ENDPOINT
        );

        auto body = quicktype::MarketBuyReqBody
        {
            std::vector <quicktype::BuyDatumContext>
            {
                quicktype::BuyDatumContext{ "RagFairBuyOffer",
                    std::vector <quicktype::BuyOfferContext>
                    {
                        quicktype::BuyOfferContext{ offer_id, quantity, barter_items }
                    }
                }
             },
            2
        };

        auto req = serialize_market_buy_request(body).dump();
        auto res = Post_Json(url, req);

        if (!OnResponseHandle(__FUNCTION__, res.err, res.data.dump()))
        {
            switch (res.err)
            {
            case 1503:
            case 1506:
            case 1507:
            {
                Log(__FUNCTION__, LL_ERR, "Offer not found!");
            } break;

            default:
                throw TarkovAPIException(Error::ResponseHandleFailed, res.errmsg);
                break;
            }
        }
        if (res.data.contains("badRequest") && !res.data["badRequest"].empty())
        {
            throw TarkovAPIException(Error::MarketBadRequest, res.data["badRequest"].dump());
        }
        if (res.data.contains("items"))
        {
            return res.data["items"];
        }
        return res.data;
    }

    json TarkovAPIManager::GetItemPrice(const std::string& schema_id)
    {
        if (schema_id.empty())
        {
            throw TarkovAPIException(Error::InvalidParameter);
        }

        auto url = fmt::format(
            "{}/client/ragfair/itemMarketPrice",
            RAGFAIR_ENDPOINT
        );

        json body{};
        body["templateId"] = schema_id;

        auto res = Post_Json(url, body.dump());

        if (!OnResponseHandle(__FUNCTION__, res.err, res.data.dump()))
        {
            throw TarkovAPIException(Error::ResponseHandleFailed, res.errmsg);
        }
        return res.data;
    }

    json TarkovAPIManager::TradeItem(const std::string& trader_id, const std::string& item_id, int64_t quantity, const std::vector <quicktype::TraderBarterItem>& barter_items)
    {
        if (trader_id.empty() || item_id.empty() || !quantity || barter_items.empty())
        {
            throw TarkovAPIException(Error::InvalidParameter);
        }

        auto url = fmt::format(
            "{}/client/game/profile/items/moving",
            PROD_ENDPOINT
        );

        auto body = quicktype::TradeItemBody
        {
            std::vector <quicktype::TradeItemDatum>
            {
                quicktype::TradeItemDatum{
                    "TradingConfirm",
                    "buy_from_trader",
                    trader_id,
                    item_id,
                    quantity,
                    0,
                    barter_items
                }
             },
            0
        };

        auto req = serialize_trade_item_request(body).dump();
        auto res = Post_Json(url, req);

        if (!OnResponseHandle(__FUNCTION__, res.err, res.data.dump()))
        {
            throw TarkovAPIException(Error::ResponseHandleFailed, res.errmsg);
        }
        if (res.data.contains("badRequest") && !res.data["badRequest"].empty())
        {
            throw TarkovAPIException(Error::MarketBadRequest, res.data["badRequest"].dump());
        }
        if (res.data.contains("items"))
        {
            return res.data["items"];
        }
        return res.data;
    }

    std::vector <quicktype::TraderItem> TarkovAPIManager::GetTraderItems(const std::string& trader_id)
    {
        auto result = std::vector <quicktype::TraderItem>();

        if (trader_id.empty())
        {
            throw TarkovAPIException(Error::InvalidParameter);
        }

        auto items = GetTraderItemsRaw(trader_id);
        auto prices = GetTraderPricesRaw(trader_id);

        if (!items.contains("barter_scheme"))
        {
            throw TarkovAPIException(Error::JsonParseFailed, "items::barter_scheme");
        }
        else if (!items.contains("items"))
        {
            throw TarkovAPIException(Error::JsonParseFailed, "items::items");
        }
        else if (!items.contains("loyal_level_items"))
        {
            throw TarkovAPIException(Error::JsonParseFailed, "items::loyal_level_items");
        }

        auto barter_scheme = items["barter_scheme"];
        if (barter_scheme.empty())
        {
            throw TarkovAPIException(Error::NullDataForParse, "barter_scheme");
        }

        auto loyal_level_items = items["loyal_level_items"];
        if (loyal_level_items.empty())
        {
            throw TarkovAPIException(Error::NullDataForParse, "loyal_level_items");
        }

        for (const auto& item : items["items"].items())
        {
            auto ctx = item.value();

            if (ctx.empty() || (ctx.contains("parentId") && ctx["parentId"] != "hideout") || !ctx.contains("_id"))
            {
                continue;
            }

            auto item_data = parse_trader_item(ctx);

            if (!loyal_level_items.contains(item_data._id))
            {
                Log(__FUNCTION__, LL_ERR, "Loyalty level could not be mapped.");
                continue;
            }
            auto loyalty_level = loyal_level_items[item_data._id];

            auto item_costs_container = std::vector <quicktype::TraderBarterItem>();

            if (!barter_scheme.contains(item_data._id))
            {
                if (!prices.contains(item_data._id))
                {
                    Log(__FUNCTION__, LL_CRI, fmt::format("Any price or barter data could not found! Trader: {} Item: '{}' - '{}", trader_id, item_data._id, item_data._tpl));
                    continue;
                }
                else
                {
                    auto item_prices = prices[item_data._id][0];

                    for (const auto& price_item : item_prices.items())
                    {
                        if (price_item.value().type() != nlohmann::json::object())
                        {
                            throw TarkovAPIException(Error::JsonBadFormat, "price_item.value().type() != nlohmann::json::object()");
                        }

                        auto price_item_ctx = price_item.value();
                        if (!price_item_ctx.contains("_tpl") || !price_item_ctx.contains("count"))
                        {
                            throw TarkovAPIException(Error::JsonBadFormat, "!price_item_ctx.contains('_tpl') ||!ctx.contains('count')");
                        }

                        quicktype::TraderBarterItem price_node{};
                        price_node._tpl = price_item_ctx["_tpl"].get<std::string>();
                        price_node.count = price_item_ctx["count"].get<double>();
                        item_costs_container.emplace_back(price_node);
                    }
                }
            }

            auto barter_items = barter_scheme[item_data._id][0];
            for (const auto& barter_item : barter_items.items())
            {
                if (barter_item.value().type() != nlohmann::json::object())
                {
                    throw TarkovAPIException(Error::JsonBadFormat, "barter_item.value().type() != nlohmann::json::object()");
                }

                auto barter_item_ctx = barter_item.value();
                if (!barter_item_ctx.contains("_tpl") || !barter_item_ctx.contains("count"))
                {
                    throw TarkovAPIException(Error::JsonBadFormat, "!barter_item_ctx.contains('_tpl') ||!ctx.contains('count')");
                }

                quicktype::TraderBarterItem barter_node{};
                barter_node._tpl = barter_item_ctx["_tpl"].get<std::string>();
                barter_node.count = barter_item_ctx["count"].get<double>();
                item_costs_container.emplace_back(barter_node);
            }

            result.emplace_back(
                quicktype::TraderItem{
                    item_data._id,
                    item_data._tpl,
                    item_data.upd,
                    item_costs_container,
                    loyalty_level
                }
            );
        }

        return result;
    }

    json TarkovAPIManager::SellItem(const std::string& trader_id, const std::string& item_id, int64_t quantity)
    {
        if (trader_id.empty() || item_id.empty() || !quantity)
        {
            throw TarkovAPIException(Error::InvalidParameter);
        }

        auto url = fmt::format(
            "{}/client/game/profile/items/moving",
            PROD_ENDPOINT
        );

        auto body = quicktype::MarketSellReqBody
        {
            std::vector <quicktype::SellDatumContext>
            {
                quicktype::SellDatumContext
                {
                    "TradingConfirm",
                    "sell_to_trader",
                    trader_id,
                    std::vector <quicktype::SellItemContext>
                    {
                        quicktype::SellItemContext{ item_id, quantity, "0" }
                    }
                }
            },
            0
        };

        auto req = serialize_market_sell_request(body).dump();
        auto res = Post_Json(url, req);

        if (!OnResponseHandle(__FUNCTION__, res.err, res.data.dump()) ||
            !res.data.contains("items"))
        {
            throw TarkovAPIException(Error::ResponseHandleFailed, res.errmsg);
        }

        return res.data["items"];
    }

    json TarkovAPIManager::OfferItem(const std::vector <std::string>& items, const quicktype::OfferRequirementContext& requirement, bool sell_all)
    {
        if (items.empty() || (requirement._tpl.empty() && !requirement.price))
        {
            throw TarkovAPIException(Error::InvalidParameter);
        }

        auto url = fmt::format(
            "{}/client/game/profile/items/moving",
            PROD_ENDPOINT
        );

        auto body = quicktype::MarketOfferReqBody
        {
            std::vector <quicktype::OfferDatumContext>
            {
                quicktype::OfferDatumContext
                {
                    "RagFairAddOffer",
                    sell_all,
                    items,
                    std::vector <quicktype::OfferRequirementsContext>
                    {
                        quicktype::OfferRequirementsContext{ requirement._tpl, requirement.price, 0, 0, false }
                    },
                    2
                }
            }
        };

        auto req = serialize_market_offer_request(body).dump();
        auto res = Post_Json(url, req);

        if (!OnResponseHandle(__FUNCTION__, res.err, res.data.dump()))
        {
            throw TarkovAPIException(Error::ResponseHandleFailed, res.errmsg);
        }
        if (res.data.contains("badRequest") && !res.data["badRequest"].empty())
        {
            throw TarkovAPIException(Error::MarketBadRequest, res.data["badRequest"].dump());
        }
        if (res.data.contains("items"))
        {
            return res.data["items"];
        }

        return res.data;
    }

    json TarkovAPIManager::StackItem(const std::string& from_item_id, const std::string& to_item_id, int64_t count)
    {
        if (from_item_id.empty() || from_item_id.empty())
        {
            throw TarkovAPIException(Error::InvalidParameter);
        }

        auto url = fmt::format(
            "{}/client/game/profile/items/moving",
            PROD_ENDPOINT
        );
        auto req = std::string();

        if (!count)
        {
            auto body = quicktype::ItemStackBody
            {
                std::vector <quicktype::ItemStackDatum>
                {
                    quicktype::ItemStackDatum
                    {
                        "Merge",
                        from_item_id,
                        to_item_id
                    }
                },
                2
            };

            req = serialize_item_merge_request(body).dump();
        }
        else
        {
            auto body = quicktype::ItemTransferBody
            {
                std::vector <quicktype::ItemTransferDatum>
                {
                    quicktype::ItemTransferDatum
                    {
                        "Transfer",
                        from_item_id,
                        to_item_id,
                        count
                    }
                },
                2
            };

            req = serialize_item_transfer_request(body).dump();
        }

        auto res = Post_Json(url, req);

        if (!OnResponseHandle(__FUNCTION__, res.err, res.data.dump()))
        {
            throw TarkovAPIException(Error::ResponseHandleFailed, res.errmsg);
        }

        return res.data;
    }

    json TarkovAPIManager::MoveItem(const std::string& item_id, const quicktype::ItemMoveTo& destination)
    {
        if (item_id.empty() || destination.id.empty())
        {
            throw TarkovAPIException(Error::InvalidParameter);
        }

        auto url = fmt::format(
            "{}/client/game/profile/items/moving",
            PROD_ENDPOINT
        );

        auto body = quicktype::ItemMoveBody
        {
            std::vector <quicktype::ItemMoveDatum>
            {
                quicktype::ItemMoveDatum
                {
                    "Move",
                    item_id,
                    quicktype::ItemMoveTo
                    {
                        destination.id,
                        destination.container,
                        destination.location
                    }
                }
            },
            2
        };

        auto req = serialize_item_move_request(body).dump();
        auto res = Post_Json(url, req);

        if (!OnResponseHandle(__FUNCTION__, res.err, res.data.dump()))
        {
            throw TarkovAPIException(Error::ResponseHandleFailed, res.errmsg);
        }

        return res.data;
    }

    json TarkovAPIManager::GetMyItems()
    {
        auto me = GetMyProfile();
        if (!me.contains("_id"))
        {
            throw TarkovAPIException(Error::JsonParseFailed, "!me.contains('_id')");
        }

        SelectProfile(me["_id"].get<std::string>());
        if (!me.contains("Info") || !me["Info"].contains("Nickname"))
        {
            throw TarkovAPIException(Error::JsonParseFailed, "!me.contains('Info') || !me['Info'].contains('Nickname')");
        }

        auto inventory = me["Inventory"];
        if (inventory.empty())
        {
            throw TarkovAPIException(Error::JsonParseFailed, "inventory.empty()");
        }

        auto items = inventory["items"];
        if (items.empty())
        {
            throw TarkovAPIException(Error::JsonParseFailed, "items.empty()");
        }

        return items;
    }

    uint64_t TarkovAPIManager::GetRoubleCount()
    {
        auto items = GetMyItems();

        uint64_t roubleCount = 0;
        auto iter = std::find_if(items.begin(), items.end(), [&roubleCount](const nlohmann::json& root)
            {
                if (root.is_object() && root.contains("_tpl") && root["_tpl"] == ROUBLE_ITEM_ID && root.contains("upd") && root["upd"].contains("StackObjectsCount"))
                {
                    roubleCount += root["upd"]["StackObjectsCount"].get<uint64_t>();
                }
                return false;
            });
        return roubleCount;
    }

    std::string TarkovAPIManager::GetMainStashID()
    {
        auto my_items = GetMyItems();

        auto main_stash_id = std::string();
        for (auto i = 0; i < MAXIMUM_STASH_SIZE; ++i)
        {
            auto item = my_items[i];
            if (!item.empty())
            {
                if (GetItemName(item["_tpl"].get<std::string>()) == "Stash") // Find first stash ID, which is parent of all other single items
                {
                    main_stash_id = item["_id"];
                    break;
                }
            }
        }
        return main_stash_id;
    }

    quicktype::ItemMoveLocation TarkovAPIManager::FindBlankStashPos() // Not works ATM
    {   
        quicktype::ItemMoveLocation a; 
        return a;
        
        auto stash_helper = StashHelper{10, 26};
        auto items = GetItems();

        auto main_stash_id = GetMainStashID();

        auto my_items = GetMyItems();
        for (const auto& root : my_items)
        {
                if (root.is_object() && root.contains("_tpl") &&
                    root.contains("parentId") && root["parentId"].get<std::string>() == main_stash_id &&
                    root.contains("location") && root["location"].is_object())
                {
                    auto x = root["location"]["x"].get<int32_t>() + 1;
                    auto y = root["location"]["y"].get<int32_t>() + 1;

                    if (items.contains(root["_tpl"]))
                    {
                        auto item_data = items[root["_tpl"].get<std::string>()];
                        if (item_data.is_object() && item_data.contains("_props") &&
                            item_data["_props"].contains("Width") && item_data["_props"].contains("Height"))
                        {
                            auto width = item_data["_props"]["Width"].get<int32_t>();
                            auto height = item_data["_props"]["Height"].get<int32_t>();
                            // FIXME: Customized weapon sizes are not correct, it's just throw base weapon size

                            if (root["location"]["r"].get<int64_t>())
                                std::swap(width, height);

                            auto stash_base_pos = ((y * stash_helper.GetWidth()) + x) - stash_helper.GetWidth();

                            Log(__FUNCTION__, LL_SYS, fmt::format("{} / {} - {} ({})  | {} - {}",
                                GetItemName(root["_tpl"].get<std::string>()), x, y, stash_base_pos, width, height));

                            stash_helper.Put(
                                stash_base_pos,
                                width,
                                height
                            );
                        }
                    }
                }
        };
        
        stash_helper.Dump();
    }

    std::vector <quicktype::TraderBarterItem> TarkovAPIManager::FindItemStack(const std::string& schema_id, uint64_t required)
    {
        auto container = std::vector <quicktype::TraderBarterItem>();

        if (schema_id.empty())
        {
            throw TarkovAPIException(Error::InvalidParameter);
        }

        auto items = GetMyItems();

        uint64_t roubleCount = 0;
        auto iter = std::find_if(items.begin(), items.end(), [&schema_id, &required, &container](const nlohmann::json& root)
            {
                if (root.is_object() && root.contains("_tpl") && root["_tpl"] == schema_id)
                {
                    auto count = (root.contains("upd") && root["upd"].contains("StackObjectsCount")) ? root["upd"]["StackObjectsCount"].get<uint64_t>() : 1;
                    if (count >= required)
                    {
                        container.emplace_back(quicktype::TraderBarterItem{ root["_id"].get<std::string>(), static_cast<double>(required) });
                        return true;
                    }
                    else
                    {
                        container.emplace_back(quicktype::TraderBarterItem{ root["_id"].get<std::string>(), static_cast<double>(count) });
                        required -= count;
                    }

                    if (!required)
                    {
                        return true;
                    }
                }
                return false;
            });
        /*
        if (iter == items.end())
        {
            container.clear();
        }
        */
        return container;
    }

    double TarkovAPIManager::GetItemPrice(double listed_price, int64_t amount)
    {
        auto old_price = listed_price * amount;

        double whole = 0;
        auto fractional = std::modf(old_price, &whole);
        if (fractional)
            whole += 1;

        return whole;
    }

    std::string TarkovAPIManager::GetItemName(const std::string& schema_id)
    {
        auto locale = GetI18n("en");
        if (locale.empty())
        {
            throw TarkovAPIException(Error::NullDataForParse, "locale");
        }

        auto target = std::string();
        auto iter = std::find_if(locale.begin(), locale.end(), [&target, &schema_id](const nlohmann::json& root)
            {
                if (root.is_object())
                {
                    for (const auto& it : root.items())
                    {
                        if (it.value().is_object())
                        {
                            if (it.key() == schema_id && it.value().type() == nlohmann::json::object() && it.value().contains("Name"))
                            {
                                target = it.value()["Name"].get<std::string>();
                                return true;
                            }
                        }
                    }
                }
                return false;
            });

        if (iter == locale.end())
        {
            throw TarkovAPIException(Error::ItemNotFound, schema_id);
        }

        return target;
    }
};
