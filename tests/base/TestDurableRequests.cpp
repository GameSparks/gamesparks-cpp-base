#include <gtest/gtest.h>
#include <GameSparks/GS.h>
#include <GameSparks/generated/GSRequests.h>
#include <GameSparks/generated/GSMessages.h>

#include "TestPlatform.h"

#include <iostream>

TEST(DurableRequests, delivered)
{
    enum { NUM_DURABLE_REQUESTS=4 };
    
	using namespace GameSparks::Core;

    
    { // authenticate, so that we can be sure which queue we're using
        GS gs;
        
        bool isAvailable = false;
        bool authenticationSucceeded = false;
        
        gs.GameSparksAvailable = [&isAvailable, &authenticationSucceeded](GS& gs, bool available){
            isAvailable = available;
            
            GameSparks::Api::Requests::DeviceAuthenticationRequest authRequest(gs);
            
            TestPlatform platform;
            
            authRequest.SetDeviceId(platform.GetDeviceId());
            authRequest.SetDeviceOS(platform.GetDeviceOS());
            
            authRequest.Send([&authenticationSucceeded](GS& instance, const GameSparks::Api::Responses::AuthenticationResponse& response){
                authenticationSucceeded = !response.GetHasErrors();
            });
        };
        
        bool condition_passed = TestPlatform::runTestLoop(
                                                          gs,
                                                          [&](){
                                                              return isAvailable && authenticationSucceeded;
                                                          });
        
        ASSERT_TRUE(condition_passed) << "the test timed out";
        ASSERT_TRUE(isAvailable) << "gamesparks is not available";
        ASSERT_TRUE(authenticationSucceeded) << "the authentication request failed";
    }

    
    
	{	// first, clear the durable queue - in case there's still something in there
		GS gs;
		TestPlatform platform;
		gs.Initialise(&platform);
		GS::t_PersistentQueue requests = gs.GetDurableQueueEntries();

		for(const auto& req : requests)
		{
			gs.RemoveDurableQueueEntry(req);
		}
	}
    
    
    {
        GS gs;
        TestPlatform platform;
        gs.Initialise(&platform);
        ASSERT_EQ(gs.GetDurableQueueEntries().size(), 0);
    }
    

	{ // now add some durable requests
		GS gs;

		bool isAvailable = false;
		bool authenticationSucceeded = false;

		gs.GameSparksAvailable = [&isAvailable, &authenticationSucceeded](GS& gs, bool available){
			isAvailable = available;

			GameSparks::Api::Requests::DeviceAuthenticationRequest authRequest(gs);

			TestPlatform platform;

			authRequest.SetDeviceId(platform.GetDeviceId());
			authRequest.SetDeviceOS(platform.GetDeviceOS());

			authRequest.Send([&authenticationSucceeded](GS& instance, const GameSparks::Api::Responses::AuthenticationResponse& response){
				authenticationSucceeded = !response.GetHasErrors();

				for(int i=0; i != NUM_DURABLE_REQUESTS; ++i)
				{
					GameSparks::Api::Requests::LogEventRequest logEvent(instance);
					logEvent.SetEventKey("testMessage");
					logEvent.SetEventAttribute("foo", "bar");
					logEvent.SetDurable(true);
                    logEvent.Send();
				}
			});
		};

        bool condition_passed = TestPlatform::runTestLoop(
            gs,
            [&](){
                return isAvailable && authenticationSucceeded;
            });
        
        ASSERT_TRUE(condition_passed) << "the test timed out";
        ASSERT_TRUE(isAvailable) << "gamesparks is not available";
        ASSERT_TRUE(authenticationSucceeded) << "the authentication request failed";
    }
    
    
    { // check that they where all persisted
        GS gs;
        TestPlatform platform;
        gs.Initialise(&platform);
        ASSERT_EQ(gs.GetDurableQueueEntries().size(), NUM_DURABLE_REQUESTS);
    }
    
    
    { // finally, test if all the requests from the queue get delivered
        GS gs;
        
        bool isAvailable = false;
        bool authenticationSucceeded = false;
        int number_of_messages_received = 0;
        
        gs.SetMessageListener<GameSparks::Api::Messages::ScriptMessage>([&number_of_messages_received](GS& gs, const GameSparks::Api::Messages::ScriptMessage& message){
            if (!message.GetHasErrors())
            {
                number_of_messages_received++;
            }
        });
        
        gs.GameSparksAvailable = [&isAvailable, &authenticationSucceeded](GS& gs, bool available){
            isAvailable = available;
            
            GameSparks::Api::Requests::DeviceAuthenticationRequest authRequest(gs);
            
            TestPlatform platform;
            
            authRequest.SetDeviceId(platform.GetDeviceId());
            authRequest.SetDeviceOS(platform.GetDeviceOS());
            
            authRequest.Send([&authenticationSucceeded](GS& instance, const GameSparks::Api::Responses::AuthenticationResponse& response){
                authenticationSucceeded = !response.GetHasErrors();
            });
        };
        
        bool condition_passed = TestPlatform::runTestLoop(
            gs,
            [&](){
                return isAvailable && authenticationSucceeded && number_of_messages_received == NUM_DURABLE_REQUESTS;
            });
        
        ASSERT_TRUE(isAvailable) << "gamesparks is not available";
        ASSERT_TRUE(authenticationSucceeded) << "the authentication request failed";
        ASSERT_EQ(number_of_messages_received, NUM_DURABLE_REQUESTS) << " the number of messages we received did not match the number of durable requests";
        ASSERT_TRUE(condition_passed) << "the test timed out";
    }
}
