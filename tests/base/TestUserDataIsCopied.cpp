//
// Created by Benjamin Schulz on 1/07/15.
//


#include <gtest/gtest.h>
#include <GameSparks/GS.h>
#include <GameSparks/generated/GSRequests.h>

#include "TestPlatform.h"


TEST(UserDataTest, Succeed)
{
    using namespace GameSparks::Core;
    
    GS gs;
    
    bool isAvailable = false;
    bool responseReceived;
    void* test_ptr = (void*)0xCAFED00D;
    
    
    gs.GameSparksAvailable = [&](GameSparks::Core::GS& gsInstance, bool avail)
    {
        isAvailable = avail;
        
        if(avail)
        {
            GameSparks::Api::Requests::AuthenticationRequest requestRight(gsInstance);
            requestRight.SetUserName("abcdefgh");
            requestRight.SetPassword("abcdefgh");
            requestRight.SetUserData(test_ptr);
            requestRight.Send([&](GameSparks::Core::GS& gsInstance, const GameSparks::Api::Responses::AuthenticationResponse& response)
            {
                ASSERT_EQ(test_ptr, response.GetUserData());
                responseReceived = true;
            });
        }
    };
    
    bool condition_passed = TestPlatform::runTestLoop(
        gs,
        [&](){ return isAvailable && responseReceived; }
    );
    
    ASSERT_TRUE(condition_passed) << "the test timed out";
    ASSERT_TRUE(isAvailable) << "gamesparks is not available";
    ASSERT_TRUE(responseReceived) << "No response received";
}

