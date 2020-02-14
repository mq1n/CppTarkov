#pragma once
#include "Constants.hpp"
#include <exception>
#include <cstring>
#include <cstdint>
#include <fmt/format.h>

class TarkovAPIException : public std::exception
{
public:
    explicit TarkovAPIException(uint8_t err_id, const std::string& desc = "") :
        error_id(err_id), error_desc(desc)
    { }
	explicit TarkovAPIException(uint8_t err_id, int64_t err_number) :
        error_id(err_id), error_desc(std::to_string(err_number))
    { }
    virtual ~TarkovAPIException() throw ()
	{ }

	virtual std::string details() const throw()
	{
		switch (error_id)
		{
			case TarkovAPI::Error::InvalidParameter:
			{
				return "Invalid or missing login parameters";
			} break;

			case TarkovAPI::Error::CprApiFailed:
			{
				return fmt::format("CPR API failed: {}", error_desc);
			} break;

			case TarkovAPI::Error::CprSessionFailed:
			{
				return "CURL session could not created";
			} break;

			case TarkovAPI::Error::CprPostFailed:
			{
				return fmt::format("Post request could not succesfully sent, API server error: {}", error_desc);
			} break;

			case TarkovAPI::Error::ZlibDecompressFailed:
			{
				return fmt::format("Compressed data could not extract: Error: {}", error_desc);
			} break;

			case TarkovAPI::Error::JsonParseFailed:
			{
				return fmt::format("JSON data could not parsed: Error: {}", error_desc);
			} break;

			case TarkovAPI::Error::JsonBadFormat:
			{
				return fmt::format("JSON data could not processed: Details: {}", error_desc);
			} break;

			case TarkovAPI::Error::ApiServerError:
			{
				return fmt::format("Game server returned with error code: Details: {}", error_desc);
			} break;

			case TarkovAPI::Error::KeepAliveFailed:
			{
				return fmt::format("Heartbeat request has been failed: Error code: {}", error_desc);
			} break;

			case TarkovAPI::Error::ResponseHandleFailed:
			{
				return fmt::format("Response could not handled: Error: {}", error_desc);
			} break;

			case TarkovAPI::Error::MarketBadRequest:
			{
				return fmt::format("Market request invalid format: Error: {}", error_desc);
			} break;

			case TarkovAPI::Error::NullDataForParse:
			{
				return fmt::format("Data: {} is null, could not processed!", error_desc);
			} break;

			case TarkovAPI::Error::TraderNotFound:
			{
				return fmt::format("Trader: '{}' data is not found on database!", error_desc);
			} break;

			case TarkovAPI::Error::ItemNotFound:
			{
				return fmt::format("Item: '{}' data is not found on database!", error_desc);
			} break;

			default:
				return fmt::format("Unknown error ID: {}", error_id);
		}
	}

    virtual int64_t getErrorID() const throw()
	{
        return error_id;
    }
	virtual std::string getErrorDesc() const throw()
	{
        return error_desc;
    }
    
private:
    int64_t error_id;
    std::string error_desc;
};
