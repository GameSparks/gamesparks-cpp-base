#include <gtest/gtest.h>
#include <GameSparks/GS.h>
#include <thread>

#include "TestPlatform.h"

TEST(ConnectionTest, TestAvailableCalled)
{
  using namespace GameSparks::Core;

  GS gs;
  TestPlatform platform;
  gs.Initialise(&platform);

  auto start_time = std::chrono::steady_clock::now();
  auto last_time = start_time;

  bool isAvailable = false;
  gs.GameSparksAvailable = [&isAvailable](const GameSparks::Core::GS& gs, bool avail)
  {
      isAvailable = avail;
  };

  while (std::chrono::steady_clock::now() - start_time < std::chrono::seconds(10) && !isAvailable)
  {
    auto now = std::chrono::steady_clock::now();
    auto delta = now - last_time;
    auto dt = std::chrono::duration_cast<std::chrono::duration<float>>(delta).count();

    gs.Update(dt);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    last_time = now;
  }

  // timed out
  GTEST_ASSERT_LT(
      std::chrono::duration_cast<std::chrono::duration<double>>(
          std::chrono::steady_clock::now() - start_time
      ).count(),
      10.0
  ) << "connection timed out";
  ASSERT_TRUE(isAvailable) << "available callback was not called";

  gs.ShutDown();
}


TEST(ConnectionTest, Compact)
{
  using namespace GameSparks::Core;

  GS gs;

  bool isAvailable = false;
  gs.GameSparksAvailable = [&](const GameSparks::Core::GS& gs, bool avail)
  {
      isAvailable = avail;
  };

  bool condition_passed = TestPlatform::runTestLoop(
    gs,
    [&](){ return isAvailable; });

  ASSERT_TRUE(condition_passed) << "the test timed out";
}
