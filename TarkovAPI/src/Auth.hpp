#pragma once
#include "Constants.hpp"
#include "LogHelper.hpp"
#include "Exception.hpp"
#include "TarkovAPIManager.hpp"
#include "Hwid.hpp"

#include <stdint.h>
#include <string>
#include <regex>
#include <vector>

#include <fmt/format.h>
#include <zlib.h>
#include <cpr/cpr.h>
#include <cpr/session.h>
#include <json.hpp>

namespace TarkovAPI
{
    namespace auth
    {
        using json = nlohmann::json;

        static void OnLauncherResponseHandle(const std::string& func, const std::string& err, const int errCode)
        {
            switch (errCode)
            {
                case ErrorCodes::OK:
                    break;

                case ErrorCodes::NotAuthorized:
                {
                    throw TarkovAPIException(Error::ApiServerError, fmt::format("Func: {} - Error: {}", func, "Not authorized or game profile not selected"));
                } break;

                case ErrorCodes::BadLogin:
                {
                    throw TarkovAPIException(Error::ApiServerError, fmt::format("Func: {} - Error: {}", func, "Bad login, invalid email or password."));
                } break;

                case ErrorCodes::InvalidParameters:
                {
                    throw TarkovAPIException(Error::ApiServerError, fmt::format("Func: {} - Error: {}", func, "Invalid or missing login parameters"));
                } break;

                case ErrorCodes::TwoFactorRequired:
                {
                    throw TarkovAPIException(Error::ApiServerError, fmt::format("Func: {} - Error: {}", func, "2FA code is required to continue authentication."));
                } break;

                case ErrorCodes::BadTwoFactorCode:
                {
                    throw TarkovAPIException(Error::ApiServerError, fmt::format("Func: {} - Error: {}", func, "Incorrect 2FA code."));
                } break;

                case ErrorCodes::CaptchaRequired:
                {
                    throw TarkovAPIException(Error::ApiServerError, fmt::format("Func: {} - Error: {}", func, "Captcha response is required to continue authentication.."));
                } break;

                case ErrorCodes::RateLimited:
                {
                    throw TarkovAPIException(Error::ApiServerError, fmt::format("Func: {} - Error: {}", func, "Too many login attempts"));
                } break;

            default:
                throw TarkovAPIException(Error::ApiServerError, fmt::format("Func: {} - Unknown error code: {}({})", func, err, errCode));
                break;
            }
        }

        static json LauncherPostJson(cpr::Session* client, const std::string& url, const std::string& body = "", const std::string& token = "")
        {
            gs_pAPILogInstance->Log(__FUNCTION__, LL_DEV, fmt::format("Sending request to {} ({})", url, body));

            auto headers = cpr::Header{
                {"Content-Type", "application/json"},
                {"User-Agent", fmt::format("BSG Launcher {}", LAUNCHER_VERSION)}
            };
            if (!token.empty())
                headers.emplace("Authorization", token);

            client->SetUrl(cpr::Url{url});
            client->SetBody(cpr::Body{body});
            client->SetHeader(headers);
            
            auto res = client->Post();

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

            std::vector <uint8_t> decompresseddata(res.text.size() * 10);

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
            catch (const json::parse_error& ex)
            {
                std::stringstream ss;
                ss << "Message: " << ex.what() << '\n' << "exception id: " << ex.id << '\n' << "byte position of error: " << ex.byte << std::endl;

                throw TarkovAPIException(Error::JsonParseFailed, ss.str());
            }

            gs_pAPILogInstance->Log(__FUNCTION__, LL_DEV, fmt::format("Response: {}", deserialized.dump()));

            if (!deserialized.contains("err") || !deserialized.contains("errmsg"))
            {
                throw TarkovAPIException(Error::JsonBadFormat, "'err' or 'errmsg' key is not available");
            }
            else if (!deserialized.contains("data"))
            {
                throw TarkovAPIException(Error::JsonBadFormat, "'data' key is not available");
            }

            return deserialized;
        }
        
        static void CheckLauncherVersion(cpr::Session* client)
        {
            auto url = fmt::format(
                "{}/launcher/GetLauncherDistrib",
                LAUNCHER_ENDPOINT
            );

            auto res = LauncherPostJson(client, url, "");  
            OnLauncherResponseHandle(__FUNCTION__, res["errmsg"].dump(), res["err"].get<int32_t>());

            if (!res["data"].contains("Version"))
            {
                throw TarkovAPIException(Error::VersionCheckError, "Issues while fetching launcherVersion...");
            }

            auto version = res["data"]["Version"].get<std::string>();
            if (version != LAUNCHER_VERSION)
            {
                LAUNCHER_VERSION = version;
                gs_pAPILogInstance->Log(__FUNCTION__, LL_SYS, fmt::format("Launcher updated to version: {}", LAUNCHER_VERSION));
            }
            else
            {
                gs_pAPILogInstance->Log(__FUNCTION__, LL_SYS, fmt::format("Launcher is up-to-date on version: {}", LAUNCHER_VERSION));
            }
        }

        static void CheckGameVersion(cpr::Session* client)
        {
            auto url = fmt::format(
                "{}/launcher/GetPatchList?launcherVersion={}&branch=live",
                LAUNCHER_ENDPOINT, LAUNCHER_VERSION
            );

            auto res = LauncherPostJson(client, url, "");  
            OnLauncherResponseHandle(__FUNCTION__, res["errmsg"].dump(), res["err"].get<int32_t>());

            if (!res["data"][0].contains("Version"))
            {
                throw TarkovAPIException(Error::VersionCheckError, "Issues while fetching gameVersion...");
            }

            auto version = res["data"][0]["Version"].get<std::string>();
            if (version != GAME_VERSION)
            {
                GAME_VERSION = version;
                gs_pAPILogInstance->Log(__FUNCTION__, LL_SYS, fmt::format("Game updated to version: {}", GAME_VERSION));
            }
            else
            {
                gs_pAPILogInstance->Log(__FUNCTION__, LL_SYS, fmt::format("Game is up-to-date on version: {}", GAME_VERSION));
            }
        }

        static void ActivateHardware(cpr::Session* client, const std::string& email, const std::string& code, const std::string& hwid)
        {
            auto url = fmt::format(
                "{}/launcher/hardwareCode/activate?launcherVersion={}",
                LAUNCHER_ENDPOINT, LAUNCHER_VERSION
            );

            json body{};
            body["activateCode"] = code;
            body["hwCode"] = hwid;
            body["email"] = email;

            auto res = LauncherPostJson(client, url, body.dump());  
            OnLauncherResponseHandle(__FUNCTION__, res["errmsg"].dump(), res["err"].get<int32_t>());
        }

        static std::string ExchangeAccessToken(cpr::Session* client, const std::string& access_token, const std::string& hwid)
        {
            auto url = fmt::format(
                "{}/launcher/game/start?launcherVersion={}&branch=live",
                PROD_ENDPOINT, LAUNCHER_VERSION
            );

            json version_ctx{};
            version_ctx["backend"] = "6";
            version_ctx["game"] = "live";
            version_ctx["major"] = GAME_VERSION;

            json body{};
            body["hwCode"] = hwid;
            body["version"] = version_ctx;

            auto res = LauncherPostJson(client, url, body.dump(), access_token);  
            OnLauncherResponseHandle(__FUNCTION__, res["errmsg"].dump(), res["err"].get<int32_t>());

            return res["data"].dump();
        }

        static std::string LoginImpl(cpr::Session* client, const std::string& email, const std::string& password, const std::string& captcha, const std::string& hwid)
        {
            auto url = fmt::format(
                "{}/launcher/login?launcherVersion={}&branch=live",
                LAUNCHER_ENDPOINT, LAUNCHER_VERSION
            );
            auto password_hash = md5::CreateHash(password.data(), password.size());

            json body{};
            body["email"] = email;
            body["pass"] = password_hash;
            body["hwCode"] = hwid;
            body["captcha"] = captcha;

            auto res = LauncherPostJson(client, url, body.dump());
            switch (res["err"].get<int32_t>())
            {
                case ErrorCodes::TwoFactorRequired:
                {
                    gs_pAPILogInstance->Log(__FUNCTION__, LL_SYS, "2FA code is required to continue authentication.");
                    std::cout << "Code: ";
                    
                    std::string code;
                    std::cin >> code;

                    auth::ActivateHardware(client, email, code, hwid);
                    return LoginImpl(client, email, password, captcha, hwid);
                } break;

                case ErrorCodes::CaptchaRequired:
                {
                    gs_pAPILogInstance->Log(__FUNCTION__, LL_CRI, "Captcha required to continue authentication, Change your IP address and try again.");
                    abort();

                    // #TODO: Launch lightweight Web UI and solve manually
                    // URL: https://launcher.escapefromtarkov.com/launcher/login
                    // Site key: 6LexEI4UAAAAAIFtNZALcloZfEgHhB8rEUqC1LwV
                } break;

                default:
                    OnLauncherResponseHandle(__FUNCTION__, res["errmsg"].dump(), res["err"].get<int32_t>());
                    break;
            }

            return res["data"].dump();
        }
    }
};
