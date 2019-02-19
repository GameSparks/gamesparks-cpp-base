#include <gtest/gtest.h>
#include <GameSparks/GS.h>
#include <GameSparks/GSUtil.h>
#include "TestPlatform.h"


TEST(OnNonce, CustomizedSuccess)
{
    using namespace GameSparks::Core;

    GS gs;

    gs.OnNonce = [](const gsstl::string& nonce)
    {
        return GameSparks::Util::getHMAC(nonce, "exampleHmac123456789012345678901");
    };

    bool isAvailable = false;

    gs.GameSparksAvailable = [&](GameSparks::Core::GS& gsInstance, bool avail)
    {
        isAvailable = avail;
    };

    bool condition_passed = TestPlatform::runTestLoop(
        gs,
        [&](){
            return isAvailable;
        });

    ASSERT_TRUE(condition_passed) << "the test timed out";
    ASSERT_TRUE(isAvailable) << "gamesparks is not available";
}


TEST(OnNonce, CustomizedFail)
{
    using namespace GameSparks::Core;

    GS gs;

    gs.OnNonce = [](const gsstl::string& nonce)
    {
        return nonce;
    };

    bool onAvailableCalled = false;

    gs.GameSparksAvailable = [&](GameSparks::Core::GS& gsInstance, bool avail)
    {
        onAvailableCalled = true;
    };

    bool condition_passed = TestPlatform::runTestLoop(
        gs,
        [&](){
            return onAvailableCalled;
        });

    ASSERT_TRUE(!condition_passed) << "the test did not time out";
    ASSERT_TRUE(!onAvailableCalled) << "gamesparks is not available";
}
