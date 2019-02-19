//
// Created by Benjamin Schulz on 22/04/15.
//


#include <gtest/gtest.h>
#include <GameSparks/GS.h>
#include <GameSparks/generated/GSRequests.h>

#include "TestPlatform.h"

void OnAuthenticated(GameSparks::Core::GS& gs, const gsstl::string& userId) {}

TEST(Bugs, UninitializedGameSparksAuthenticated)
{
    using namespace GameSparks::Core;

    GS gs;

    #if !defined(STL_FUNCTIONAL_SUPPORTED) || !STL_FUNCTIONAL_SUPPORTED
    ASSERT_EQ(gs.GameSparksAuthenticated, GameSparks::Core::GS_::t_AuthenticatedCallback());
    gs.GameSparksAuthenticated = OnAuthenticated;
    ASSERT_EQ(gs.GameSparksAuthenticated, OnAuthenticated);
    #endif /* STL_FUNCTIONAL_SUPPORTED */
}
