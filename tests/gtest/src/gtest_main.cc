#if defined(IW_SDK)
#   include <s3e.h>
#endif

#include <stdio.h>

#include "gtest/gtest.h"

GTEST_API_ int main(int argc, char **argv) {
  testing::InitGoogleTest(&argc, argv);
  //::testing::TestEventListeners& listeners =
  //    ::testing::UnitTest::GetInstance()->listeners();
  // Adds a listener to the end.  Google Test takes the ownership.
  //listeners.Append(new WebReportTestEventListener());
  auto result = RUN_ALL_TESTS();

  #if defined(IW_SDK)
  // this leaves the Marmalade Test-UI open until the user closes it.
  while(!s3eDeviceCheckQuitRequest())
  {
    //Update the sub systems
    s3eKeyboardUpdate();
    s3ePointerUpdate();
    s3eSurfaceShow();
    s3eDeviceYield(0);
  }
  #endif

  return result;
}
