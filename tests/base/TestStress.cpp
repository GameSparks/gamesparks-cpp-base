//
// Created by Benjamin Schulz on 13/03/15.
//

#include <gtest/gtest.h>
#include <GameSparks/GS.h>
#include <GameSparks/generated/GSRequests.h>
#include <chrono>

#include "TestPlatform.h"

/*!
* This test just randomly times out and creates/destoys gamesparks instances.
* This test will fail if any exceptions or runtime error occur.
* */
TEST(StressTest, Stress)
{
    using namespace GameSparks::Core;

    GameSparks::Core::GS GS;

    TestPlatform platform;
    GS.Initialise(&platform);

    auto start = std::chrono::high_resolution_clock::now();
    while (std::chrono::high_resolution_clock::now() - start < std::chrono::seconds(10))
    {
        // deltaTime has to be provided in seconds
        GS.Update(GameSparks::Seconds(rand()%10)); // note: time frame here varies randomly from 0..9 seconds to stress test timeouts
        //usleep(1000);

        if (rand() % 1000 == 0)
        {
            std::clog << "restart" << std::endl;
            GS.ShutDown();
            GS = GameSparks::Core::GS();
            GS.Initialise(&platform);
        }
    }
}
