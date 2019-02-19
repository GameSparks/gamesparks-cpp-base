#include <gtest/gtest.h>
#include <GameSparks/GS.h>
#include <GameSparks/generated/GSRequests.h>

#include "TestPlatform.h"
#include <iostream>

gsstl::map<gsstl::string, gsstl::string> gs_get_device_stats();

static bool gs_is_digits(const std::string &str)
{
	return str.find_first_not_of("0123456789") == std::string::npos;
}

// split s by '.' and return at most the first two parts. if non digits are found in the first two places, an empty vector is returned.
// if only one part is found, an empty vector is returned
static gsstl::vector<gsstl::string> gs_major_minor_version_split(const gsstl::string& s)
{
	const char delim = '.';
	gsstl::vector<gsstl::string> tokens;
	size_t start = 0U;
	size_t end = s.find(delim);
	for (;end != std::string::npos;start = end + 1, end = s.find(delim, start))
	{
		gsstl::string part = s.substr(start, end - start);
		if (!gs_is_digits(part))
		{
			return gsstl::vector<gsstl::string>();
		}
		if(!part.empty())
		{
			tokens.push_back(part);
		}
		if(tokens.size() == 2)
		{
			return tokens;
		}
	}

	gsstl::string part = s.substr(start, end);
	if(!part.empty())
	{
		tokens.push_back(part);
	}
    if(tokens.size() != 2)
    {
        return gsstl::vector<gsstl::string>();
    }
	return tokens;
}

TEST(DeviceStats, VersionSplit)
{
	auto test_strings = {
		"3.16.0-4-amd64",
		"9.3.5",
		"10.1",
		"10.11.6",
		"23",
		"10.0.14393.0",
		"",
		".",
		"..",
		"12.A.13",
		"A.22.a",
		"23.a.b",
		"a.b.c",
		"12-3.44",
        "123456789.123456789.12345798"

	};

	for(const auto& version_ : test_strings)
	{
		const gsstl::string version(version_);
		std::clog << "version: " << version << std::endl;
		auto tokens = gs_major_minor_version_split(version);
        ASSERT_LE(tokens.size(),size_t(2));
        for(const auto& s : tokens)
        {
            ASSERT_TRUE( gs_is_digits(s) );
        }
        ASSERT_TRUE(tokens.size() == 0 || tokens.size() == 2);

        for (auto iter = tokens.begin(); iter != tokens.end(); iter++) {
            if (iter != tokens.begin()) std::clog << ".";
            std::clog << *iter;
        }

		std::clog << std::endl;
		std::clog << tokens.size() << std::endl;
	}
}


TEST(DeviceStats, Test)
{
    auto device_stats = gs_get_device_stats();

    std::clog << "Device Stats:" << std::endl;
    for(const auto& p : device_stats)
    {
        std::clog << "  " << p.first << " : " << p.second << std::endl;
    }

    // if there's major, there also needs to be minor
    if(device_stats.count("os.version.major"))
        ASSERT_TRUE(device_stats.count("os.version.major.minor") > 0);

    if(device_stats.count("os.version.major.minor") > 0)
    {
        auto major_minor = device_stats["os.version.major.minor"];
        // there needs to be a dot
        ASSERT_TRUE(std::count(major_minor.begin(), major_minor.end(), '.') == 1);
    }


	using namespace GameSparks::Core;

	GS gs;

	bool isAvailable = false;
	bool authenticationSucceeded = false;

	gs.GameSparksAvailable = [&](GameSparks::Core::GS& gsInstance, bool avail)
	{
		isAvailable = avail;

		if (avail)
		{
			ASSERT_TRUE(gsInstance.GetAvailable());
			GameSparks::Api::Requests::AuthenticationRequest requestRight(gsInstance);
			requestRight.SetUserName("abcdefgh");
			requestRight.SetPassword("abcdefgh");
			requestRight.Send([&](GameSparks::Core::GS& gsInstance, const GameSparks::Api::Responses::AuthenticationResponse& response)
			{
				authenticationSucceeded = !response.GetHasErrors();

				ASSERT_TRUE(authenticationSucceeded) << response.GetErrors().GetValue().GetJSON();
			});
		}
	};

	bool condition_passed = TestPlatform::runTestLoop(
		gs,
		[&]() {
		return isAvailable && authenticationSucceeded;
	});

	ASSERT_TRUE(condition_passed) << "the test timed out";
	ASSERT_TRUE(isAvailable) << "gamesparks is not available";
	ASSERT_TRUE(authenticationSucceeded) << "the authentication request failed";
}
