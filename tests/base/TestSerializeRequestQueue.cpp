#include <gtest/gtest.h>
#include <GameSparks/GS.h>
#include <GameSparks/generated/GSRequests.h>

#include "TestPlatform.h"

TEST(TestSerializeRequestQueue, Test)
{
    using namespace GameSparks::Core;

    GS gs;

    GS::t_PersistentQueue q;
    q.push_back(GSRequest(gs, "fooRequestType"));
    q.push_back(GSRequest(gs, "barRequestType"));
    q.push_back(GSRequest(gs, "bazRequestType"));
    q.push_back(GSRequest(gs, "quxRequestType"));
    auto serialized = gs.SerializeRequestQueue(q);
    auto deserialized = gs.DeserializeRequestQueue(serialized);

    ASSERT_EQ(q, deserialized);
}
