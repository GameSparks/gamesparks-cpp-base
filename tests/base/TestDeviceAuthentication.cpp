//
// Created by Benjamin Schulz on 22/04/15.
//


#include <gtest/gtest.h>
#include <GameSparks/GS.h>
#include <GameSparks/generated/GSRequests.h>

#include "TestPlatform.h"


TEST(DeviceAuthenticationTest, Succeed)
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
            GameSparks::Api::Requests::DeviceAuthenticationRequest request(gsInstance);

            // for now, this has to be done manually
            request.SetDeviceId("I-AM-A-TEST-DEVICE-ID");
            request.SetDeviceOS("IOS"); //IOS, ANDROID, WP8 or W8

            request.Send([&](GameSparks::Core::GS& gsInstance, const GameSparks::Api::Responses::AuthenticationResponse& response)
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

