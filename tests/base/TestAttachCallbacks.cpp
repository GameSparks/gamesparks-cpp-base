//
// Created by Benjamin Schulz on 22/04/15.
//


#include <gtest/gtest.h>
#include <GameSparks/GS.h>
#include <GameSparks/generated/GSRequests.h>
#include <GameSparks/generated/GSMessages.h>

#include "TestPlatform.h"

using namespace GameSparks::Core;
using namespace GameSparks::Api::Messages;
using namespace GameSparks::Api::Responses;
using namespace GameSparks::Api::Requests;

TEST(DurableQueue, TestAttachCallbacks)
{
    using namespace GameSparks::Core;

    /*{ // put item into queue
		GS_ gs;
		TestPlatform platform;
		gs.Initialise(&platform);

		GameSparks::Api::Requests::AuthenticationRequest req(gs);
		req.SetUserName("abcdefgh");
		req.SetPassword("abcdefgh");
		req.SetDurable(true);
		req.Send();
    }*/

    GS gs;

    bool isAvailable = false;
	bool callbackDispatched = false;
	bool isAuthenticated = false;

    gs.OnPersistentQueueLoadedCallback = [&](GS& gs){
		std::clog << "******************** !!!!!!!!!!!!!!!!!!!! ******************" << std::endl;

		isAuthenticated = true;

		GameSparks::Api::Requests::AccountDetailsRequest req(gs);
		req.SetDurable(true);
		req.Send();

		for(auto& request : gs.GetDurableQueueEntries())
    	{
    		std::clog << "<<<<<>>>>>><<<<<<>>>>>><<<>>>>>><<<<<<<>>>>>>><<<<<<<>>>>>>><<>><>>>>>>" << std::endl;
    		std::clog << "******* attaching callback " << std::endl;

    		ASSERT_FALSE(request.HasCallbacks()) << "the requests in the queue are supposed to have no callbacks on startup.";
    		request.SetCallback([&](GS& gs, const GSData& data){
    			callbackDispatched = true;
    		});
     	}
        
        for(auto& request : gs.GetDurableQueueEntries())
        {
            ASSERT_TRUE(request.HasCallbacks());
        }
	};

    gs.GameSparksAvailable = [&](GameSparks::Core::GS& gs, bool avail)
    {
        isAvailable = avail;

        if(avail)
        {
			GameSparks::Api::Requests::AuthenticationRequest req(gs);
			req.SetUserName("abcdefgh");
			req.SetPassword("abcdefgh");
			req.Send();   	
        }
    };

    bool condition_passed = TestPlatform::runTestLoop(
	    gs,
	    [&](){
		    return isAvailable && isAuthenticated && callbackDispatched;
		});

    ASSERT_TRUE(isAvailable) << "gamesparks is not available";
    ASSERT_TRUE(isAuthenticated) << "auth failed";
    ASSERT_TRUE(callbackDispatched) << "The callback re-attached to the durable request has not been called.";
    ASSERT_TRUE(condition_passed) << "the test timed out";
}
