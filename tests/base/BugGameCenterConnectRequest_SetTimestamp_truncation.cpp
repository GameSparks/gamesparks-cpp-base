#include <gtest/gtest.h>
#include <GameSparks/generated/GSRequests.h>
#include <cmath>
#include <GameSparks/GS.h>

static_assert(sizeof(int) == 4, "int must be 32 bit");
static_assert(sizeof(long long) == 8, "long long must be 64 bit");

// Note: numbers are stored as doubles internally, so only up to 53 bits of integer precision can be represented
TEST(Bugs, GameCenterConnectRequest_SetTimestamp_truncation)
{
    GameSparks::Core::GS gs;

    GameSparks::Api::Requests::GameCenterConnectRequest request(gs);

	// Note: the largest consecutive integer that can be stored in a 64bit double precission IEEE floating point is 2^53
	// 0x20000000000000L is 2^53
	// if std::numeric_limits<long long>::max() can be stored (and restored!) is platform dependend

	{
		auto int_53_bits = 0x20000000000000L;
		request.SetTimestamp(int_53_bits);
		auto json = request.m_Request.GetJSON();
		auto parsed = GameSparks::Core::GSObject::FromJSON(json);
		auto timestamp = parsed.GetLongLong("timestamp");
		ASSERT_TRUE(timestamp.HasValue());
		ASSERT_EQ(timestamp.GetValue(), int_53_bits);
	}

	{
		auto int_53_bits = -0x20000000000000L;
		request.SetTimestamp(int_53_bits);
		auto json = request.m_Request.GetJSON();
		auto parsed = GameSparks::Core::GSObject::FromJSON(json);
		auto timestamp = parsed.GetLongLong("timestamp");
		ASSERT_TRUE(timestamp.HasValue());
		ASSERT_EQ(timestamp.GetValue(), int_53_bits);
	}
}
