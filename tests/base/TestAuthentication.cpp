//
// Created by Benjamin Schulz on 13/03/15.
//


#include <gtest/gtest.h>
#include <GameSparks/GS.h>
#include <GameSparks/generated/GSRequests.h>

#include "TestPlatform.h"


TEST(AuthenticationTest, Succeed)
{
    using namespace GameSparks::Core;

    GS gs;

    bool isAvailable = false;
    bool authenticationSucceeded = false;

    gs.GameSparksAvailable = [&](GameSparks::Core::GS& gsInstance, bool avail)
    {
        isAvailable = avail;

        if(avail)
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
            [&](){
                return isAvailable && authenticationSucceeded;
            });

    ASSERT_TRUE(condition_passed) << "the test timed out";
    ASSERT_TRUE(isAvailable) << "gamesparks is not available";
    ASSERT_TRUE(authenticationSucceeded) << "the authentication request failed";
}


TEST(AuthenticationTest, Fail)
{
    using namespace GameSparks::Core;

    GS gs;

    bool isAvailable = false;
    bool authenticationFailed = false;

    gs.GameSparksAvailable = [&](GameSparks::Core::GS& gsInstance, bool avail)
    {
        isAvailable = avail;

        if(avail)
        {
            ASSERT_TRUE(gsInstance.GetAvailable());

            GameSparks::Api::Requests::AuthenticationRequest requestRight(gsInstance);
            requestRight.SetUserName("i-am-not-a-valid-login");
            requestRight.SetPassword("and-i-am-not-a-valid-password");
            requestRight.Send([&](GameSparks::Core::GS& gsInstance, const GameSparks::Api::Responses::AuthenticationResponse& response)
            {
                authenticationFailed = response.GetHasErrors();

                ASSERT_TRUE(authenticationFailed) << response.GetErrors().GetValue().GetJSON();
            });
        }
    };

    bool condition_passed = TestPlatform::runTestLoop(
            gs,
            [&](){
                return isAvailable && authenticationFailed;
            });

    ASSERT_TRUE(condition_passed) << "the test timed out";
    ASSERT_TRUE(isAvailable) << "gamesparks is not available";
    ASSERT_TRUE(authenticationFailed) << "the authentication should have failed";
}
