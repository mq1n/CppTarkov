#include <catch2/catch.hpp>
#include "../src/Hwid.hpp"

TEST_CASE("MD5 Hash generator", "[multi-file:2]")
{
	const auto data = std::string("69");
	const auto hash = TarkovAPI::md5::CreateHash(data.data(), data.length());

	REQUIRE(hash == "14bfa6bb14875e45bba028a21ed38046");
}

TEST_CASE("Hwid generator", "[multi-file:2]")
{
	const auto hwid = TarkovAPI::hwid::generate_hwid();

	REQUIRE(hwid.size() == 258);
}
