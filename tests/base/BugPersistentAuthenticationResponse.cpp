#include <gtest/gtest.h>
#include <GameSparks/GS.h>
#include <GameSparks/generated/GSRequests.h>
#include <GameSparks/generated/GSMessages.h>

#include "TestPlatform.h"

#include <iostream>

TEST(Bugs, AuthenticationResponse)
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
            GameSparks::Api::Requests::DeviceAuthenticationRequest authRequest(gs);

            TestPlatform platform;

            authRequest.SetDeviceId(platform.GetDeviceId());
            authRequest.SetDeviceOS(platform.GetDeviceOS());
            authRequest.SetDurable(true);

            authRequest.Send([&authenticationSucceeded](GS& instance, const GameSparks::Api::Responses::AuthenticationResponse& response){
                authenticationSucceeded = !response.GetHasErrors();
            });
        }
    };

    { // switch to persistent queue of unauthenticated user
        TestPlatform platform;
        platform.SetUserId("");
        platform.SetAuthToken("");
        std::clog << "persistent queue is: " << platform.LoadValue(platform.GetUserId() + "_persistentQueue") << std::endl;
        platform.StoreValue(platform.GetUserId() + "_persistentQueue", "[]");
        ASSERT_EQ(platform.LoadValue(platform.GetUserId() + "_persistentQueue"), "[]");
    }

    bool condition_passed = TestPlatform::runTestLoop(
        gs,
        [&](){
            return isAvailable && authenticationSucceeded;
    });

    ASSERT_TRUE(isAvailable) << "gamesparks is not available";
    ASSERT_TRUE(authenticationSucceeded) << "the authentication request failed";
    ASSERT_TRUE(condition_passed) << "the test timed out";

    {
        TestPlatform platform;
        platform.SetUserId("");
        ASSERT_EQ(platform.LoadValue(platform.GetUserId() + "_persistentQueue"), "[]");
    }
}
