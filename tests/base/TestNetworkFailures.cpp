#include <gtest/gtest.h>
#include <GameSparks/GS.h>
#include <GameSparks/generated/GSRequests.h>

#include "TestPlatform.h"

TEST(DISABLED_NetworkFailures, TransportError)
{
  using namespace GameSparks::Core;

  GS gs;

  bool isAvailable = false;
  bool errorReceived = false;
  gs.GameSparksAvailable = [&](GameSparks::Core::GS& gs, bool avail)
  {
      isAvailable = avail;
      
      std::cout << "********* Available" << std::endl;

      GameSparks::Api::Requests::AuthenticationRequest requestRight(gs);
      requestRight.SetUserName("abcdefgh");
      requestRight.SetPassword("abcdefgh");
      requestRight.Send([&](GameSparks::Core::GS& gs, const GameSparks::Api::Responses::AuthenticationResponse& response)
      {
          errorReceived = response.GetHasErrors();
          ASSERT_TRUE(errorReceived) << response.GetJSONString();
      }, 0.3f);
  };

  bool condition_passed = TestPlatform::runTestLoop<>(
    gs,
    [&](){ return isAvailable && errorReceived; });

  ASSERT_TRUE(condition_passed) << "the test timed out";
}
