#pragma once
#include "BasicLog.hpp"

#include <spdlog/spdlog.h>
#include <spdlog/sinks/msvc_sink.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/stdout_sinks.h>
#include <spdlog/sinks/stdout_color_sinks.h>

namespace TarkovAPI
{
    static constexpr auto CUSTOM_LOG_FILENAME = "TarkovAPI.log";
    static constexpr auto CUSTOM_LOG_ERROR_FILENAME = "TarkovAPIError.log";

    enum ELogLevels
    {
        LL_SYS,
        LL_ERR,
        LL_CRI,
        LL_WARN,
        LL_DEV,
        LL_TRACE
    };

    class CLog
    {
    public:
        CLog() = default;
        CLog(const std::string& stLoggerName, const std::string& stFileName);
        ~CLog() = default;

        void Log(const std::string& stFunction, int32_t nLevel, const std::string& stBuffer);

    private:
        mutable std::recursive_mutex		m_pkMtMutex;

        std::shared_ptr <spdlog::logger>	m_pkLoggerImpl;
        std::string							m_stLoggerName;
        std::string							m_stFileName;
    };

    extern CLog* gs_pAPILogInstance;
}
