#include <iostream>

#include <GameSparks/GS.h>
#include <GameSparks/IGSPlatform.h>
#include <GameSparks/generated/GSRequests.h>

#include "./extra/usleep.h"
#include "sample_configuration.h"

void AuthenticationRequest_Response(GameSparks::Core::GS&, const GameSparks::Api::Responses::AuthenticationResponse& response)
{
	if (response.GetHasErrors())
	{
		gsstl::cout << "something went wrong during the authentication" << gsstl::endl;
		gsstl::cout << response.GetErrors().GetValue().GetJSON().c_str() << gsstl::endl;
	}
	else
	{
		gsstl::cout << "you successfully authenticated to GameSparks with your credentials" << gsstl::endl;
		gsstl::cout << "your displayname is " << response.GetBaseData().GetString("displayName").GetValue().c_str() << gsstl::endl;
	}
}

void GameSparksAvailable(GameSparks::Core::GS& gsInstance, bool available)
{
	gsstl::cout << "GameSparks is " << (available ? "available" : "not available") << gsstl::endl;

	if (available)
	{
		// try with wrong credentials
		GameSparks::Api::Requests::AuthenticationRequest requestWrong(gsInstance);
		requestWrong.SetUserName("TOTALLYWRONGUSER");
		requestWrong.SetPassword("TOTALLYWRONGPASSWORD");
		requestWrong.Send(AuthenticationRequest_Response);

		// try with right credentials
		GameSparks::Api::Requests::AuthenticationRequest requestRight(gsInstance);
		requestRight.SetUserName("abcdefgh");
		requestRight.SetPassword("abcdefgh");
		requestRight.Send(AuthenticationRequest_Response);
	}
}

int main(int argc, const char* argv[])
{
    (void)(argc); // unused
    (void)(argv); // unused

    using namespace GameSparks::Core;

    GameSparks::Core::GS gs;
    
    SampleConfiguration::NativePlatformDescription platform;
	gs.Initialise(&platform);

	gs.GameSparksAvailable = GameSparksAvailable;

	int cyclesLeft = 200000;
	while (cyclesLeft-- > 0)
	{
		// deltaTime has to be provided in seconds
		gs.Update(GameSparks::Seconds(rand()%10)); // note: time frame here varies randomly from 0..9 seconds to stress test timeouts
		//usleep(100000);
		usleep(1000);

		if (rand() % 1000 == 0)
		{
			gsstl::clog << "restart" << gsstl::endl;
			gs.ShutDown();
			gs = GameSparks::Core::GS();
			gs.GameSparksAvailable = GameSparksAvailable;
			gs.Initialise(&platform);
		}
	}

	gs.ShutDown();
}
