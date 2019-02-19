//
// Created by Benjamin Schulz on 22/04/15.
//


#include <gtest/gtest.h>
#include <GameSparks/GS.h>
#include <GameSparks/generated/GSMessages.h>

#include "TestPlatform.h"

static void Listener(GameSparks::Core::GS&, const GameSparks::Api::Messages::AchievementEarnedMessage&) { }

TEST(Bugs, UnregisterMessageListenerBeforeDestruct)
{
    using namespace GameSparks::Core;

    GS gs;
    gs.SetMessageListener<GameSparks::Api::Messages::AchievementEarnedMessage>(Listener);
    gs.SetMessageListener<GameSparks::Api::Messages::AchievementEarnedMessage>({});
}
