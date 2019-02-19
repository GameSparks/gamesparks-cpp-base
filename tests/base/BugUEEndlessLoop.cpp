//
// Created by Benjamin Schulz on 22/04/15.
//


#include <gtest/gtest.h>
#include <GameSparks/GS.h>
#include <GameSparks/generated/GSRequests.h>
#include <GameSparks/generated/GSMessages.h>

#include "TestPlatform.h"

using namespace GameSparks::Core;
using namespace GameSparks::Api::Messages;
using namespace GameSparks::Api::Responses;
using namespace GameSparks::Api::Requests;

// --gtest_filter=Bugs.UEEndlessLoop
TEST(Bugs, UEEndlessLoop)
{
    using namespace GameSparks::Core;

    GS gs;

    bool isAvailable = false;
    bool isAuthenticated = false;

    gs.GameSparksAvailable = [&](GameSparks::Core::GS& gs, bool avail)
    {
        isAvailable = avail;

        if(avail)
        {
            GameSparks::Api::Requests::AuthenticationRequest req(gs);
            req.SetUserName("abcdefgh");
            req.SetPassword("abcdefgh");
            req.SetUserData((void*)0x1234);
            req.Send();
            gs.ChangeUserDataForRequests((void*)0x1234, (void*)0x4321);
            isAuthenticated = true;
        }
    };

    bool condition_passed = TestPlatform::runTestLoop(
            gs,
            [&](){
                return isAvailable && isAuthenticated;
            });

    ASSERT_TRUE(isAvailable) << "gamesparks is not available";
    ASSERT_TRUE(isAuthenticated) << "auth failed";
    ASSERT_TRUE(condition_passed) << "the test timed out";
}


TEST(Bugs, UEEditorCrash)
{
    using namespace GameSparks::Core;

    GS gs;

    bool isAvailable = false;
    bool isAuthenticated = false;

    gs.GameSparksAvailable = [&](GameSparks::Core::GS& gs, bool avail)
    {
        isAvailable = avail;

        if(avail)
        {
            GameSparks::Api::Requests::AuthenticationRequest req(gs);
            req.SetUserName("abcdefgh");
            req.SetPassword("abcdefgh");
            req.SetUserData((void*)0x1234);
            req.Send([&](GameSparks::Core::GS&, const GameSparks::Api::Responses::AuthenticationResponse& response){
                ASSERT_EQ(response.GetUserData(), (void*)0x4321);
                isAuthenticated = true;
            });
            gs.ChangeUserDataForRequests((void*)0x1234, (void*)0x4321);
        }
    };

    bool condition_passed = TestPlatform::runTestLoop(
    gs,
    [&](){
        return isAvailable && isAuthenticated;
    });

    ASSERT_TRUE(isAvailable) << "gamesparks is not available";
    ASSERT_TRUE(isAuthenticated) << "auth failed";
    ASSERT_TRUE(condition_passed) << "the test timed out";
}

