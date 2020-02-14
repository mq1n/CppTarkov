#pragma once
#include <random.hpp>
#include <fmt/format.h>
#include <windows.h>
#include <Wincrypt.h>
#include <stdint.h>
#include <iterator>
#include <vector>
#include <cstring>
#include <sstream>
#include "LogHelper.hpp"

namespace TarkovAPI
{
    namespace md5
    {
        // https://stackoverflow.com/a/13259720
        static std::string CreateHash(const void* data, const size_t data_size)
        {
            std::string hash = "";
            HCRYPTPROV hProv = NULL;

            if (!CryptAcquireContext(&hProv, NULL, NULL, PROV_RSA_FULL, CRYPT_VERIFYCONTEXT))
            {
                if (gs_pAPILogInstance) gs_pAPILogInstance->Log(__FUNCTION__, LL_ERR, fmt::format("CryptAcquireContext failed. Error code: {}", GetLastError()));
                return hash;
            }

            HCRYPTPROV hHash = NULL;
            if (!CryptCreateHash(hProv, CALG_MD5, 0, 0, &hHash))
            {
                if (gs_pAPILogInstance) gs_pAPILogInstance->Log(__FUNCTION__, LL_ERR, fmt::format("CryptCreateHash failed. Error code: {}", GetLastError()));
                CryptReleaseContext(hProv, 0);
                return hash;
            }

            if (!CryptHashData(hHash, static_cast<const BYTE*>(data), static_cast<DWORD>(data_size), 0))
            {
                if (gs_pAPILogInstance) gs_pAPILogInstance->Log(__FUNCTION__, LL_ERR, fmt::format("CryptHashData failed. Error code: {}", GetLastError()));
                CryptDestroyHash(hHash);
                CryptReleaseContext(hProv, 0);
                return hash;
            }

            DWORD cbHashSize = 0, dwCount = sizeof(DWORD);
            if(!CryptGetHashParam( hHash, HP_HASHSIZE, (BYTE*)&cbHashSize, &dwCount, 0))
            {
                if (gs_pAPILogInstance) gs_pAPILogInstance->Log(__FUNCTION__, LL_ERR, fmt::format("CryptGetHashParam(1) failed. Error code: {}", GetLastError()));
                CryptDestroyHash(hHash);
                CryptReleaseContext(hProv, 0);
                return hash;
            }

            std::vector <BYTE> buffer( cbHashSize );
            if (!CryptGetHashParam( hHash, HP_HASHVAL, reinterpret_cast<BYTE*>(&buffer[0]), &cbHashSize, 0))
            {
                if (gs_pAPILogInstance) gs_pAPILogInstance->Log(__FUNCTION__, LL_ERR, fmt::format("CryptGetHashParam(2) failed. Error code: {}", GetLastError()));
                CryptDestroyHash(hHash);
                CryptReleaseContext(hProv, 0);
                return hash;
            }

            std::ostringstream oss;
            
            for (auto iter = buffer.begin(); iter != buffer.end(); ++iter)
            {
                oss.fill('0');
                oss.width(2);
                oss << std::hex << static_cast<const int>(*iter);
            }

            hash = oss.str();
            CryptDestroyHash(hHash);
            CryptReleaseContext(hProv, 0);

            return hash;
        }
    };

    namespace hwid
    {
        using Random = effolkronium::random_static;

        static auto random_md5()
        {
            const auto data = Random::get<uint64_t>();
            return fmt::format("{}", md5::CreateHash(reinterpret_cast<const void*>(data), sizeof(uint64_t)));
        }

        static auto short_md5()
        {
            auto hash = random_md5();
            hash.erase(hash.size() - 8, std::string::npos);
            return hash;
        };

        static auto generate_hwid()
        {
            return fmt::format(
                "#1-{}:{}:{}-{}-{}-{}-{}-{}",
                random_md5(),
                random_md5(),
                random_md5(),
                random_md5(),
                random_md5(),
                random_md5(),
                random_md5(),
                short_md5()
            );
        }
    }
}
