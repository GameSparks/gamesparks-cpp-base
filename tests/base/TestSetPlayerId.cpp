#include <gtest/gtest.h>
#include <GameSparks/GS.h>
#include <GameSparks/generated/GSRequests.h>

#include "TestPlatform.h"

TEST(SetPlayerId, Test)
{
    using namespace GameSparks::Core;

    const gsstl::string TEST_PLAYER_ID("FOO_BERT_BAZ");

    GS gs;
    GameSparks::Api::Requests::AuthenticationRequest request(gs);
    request.SetPlayerId(TEST_PLAYER_ID);

    const auto json = request.m_Request.GetJSON();
    const auto parsed = GSObject::FromJSON(json);
    ASSERT_TRUE(parsed.GetString("playerId").HasValue());
    ASSERT_EQ(parsed.GetString("playerId").GetValue(), TEST_PLAYER_ID);
}
