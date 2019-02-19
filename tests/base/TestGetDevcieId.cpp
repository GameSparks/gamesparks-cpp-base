#include <gtest/gtest.h>
#include <GameSparks/GS.h>

#include "TestPlatform.h"


TEST(GSPlatform, GetDeviceId)
{
  using namespace GameSparks::Core;

  TestPlatform platform;

  ASSERT_NE(platform.GetDeviceId(), "-TEST-RUNNER");
  ASSERT_NE(platform.GetDeviceId(), "NO-UUID-SUPPORT-FOR-THIS-PLATFORM-TEST-RUNNER");
}
