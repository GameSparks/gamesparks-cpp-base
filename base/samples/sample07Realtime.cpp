#include <iostream>

#include <GameSparks/GS.h>
#include <GameSparks/IGSPlatform.h>
#include <GameSparks/generated/GSRequests.h>
#include <GameSparks/generated/GSMessages.h>

#include <GameSparksRT/IRTSession.hpp>
#include <GameSparksRT/IRTSessionListener.hpp>
#include <GameSparksRT/RTData.hpp>

#include "sample_configuration.h"
//#include <memory> // for std::unique_ptr
//#include <ctime> // for initializing rand()
//#include <sstream>

/*
	This sample illustrates some of the features of GameSparks RT.
	For this to work, you need to create a match with a short code of "TestMatch".
*/



void MatchmakingRequest_Response(GameSparks::Core::GS&, const GameSparks::Api::Responses::MatchmakingResponse& response)
{
	if (response.GetHasErrors())
	{
		gsstl::cerr << "!!! Error in Response:" << response.GetErrors().GetValue().GetJSON() << gsstl::endl;
	}
}

void SendMatchmakingRequest(GameSparks::Core::GS& gs)
{
	GameSparks::Api::Requests::MatchmakingRequest makeMatch(gs);
	makeMatch.SetSkill(1);
	makeMatch.SetMatchShortCode("TestMatch");
	makeMatch.Send(MatchmakingRequest_Response);
}

void AuthenticationRequest_Response(GameSparks::Core::GS& gsInstance, const GameSparks::Api::Responses::AuthenticationResponse& response)
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

		// OK, we're authenticated, send out the initial MatchmakingRequest ...
		SendMatchmakingRequest(gsInstance);
	}
}

void GameSparksAvailable(GameSparks::Core::GS& gsInstance, bool available)
{
	gsstl::cout << "GameSparks is " << (available ? "available" : "not available") << gsstl::endl;

	if (available)
	{
		// send a device authentication request with a random device id.
		// this is done so that you can start two instances on the same machine.
		// In a production title, you'd use IGSPlatform::GetDeviceId() instead.
		GameSparks::Api::Requests::DeviceAuthenticationRequest authRequest(gsInstance);	
		gsstl::srand((unsigned int)gsstl::time(nullptr));
		auto rrr = gsstl::rand();

		gsstl::stringstream ss;
		ss << rrr;
		authRequest.SetDeviceId(ss.str()); // generate a random device id (for easy testing)
		authRequest.SetDeviceOS("W8");
		authRequest.Send(AuthenticationRequest_Response);
	}
}


class GameSession : GameSparks::RT::IRTSessionListener
{
	public:
		gsstl::unique_ptr<GameSparks::RT::IRTSession> Session;

		GameSession(const gsstl::string& connectToken, const gsstl::string& host, const gsstl::string& port)
		{
			Session.reset(GameSparks::RT::GameSparksRT
				::SessionBuilder()
				.SetConnectToken(connectToken)
				.SetHost(host)
				.SetPort(port)
				.SetListener(this)
				.Build());

			Session->Start();
		}

		void OnPlayerConnect(int peerId) override
		{
			gsstl::clog << " OnPlayerConnect:" << peerId << gsstl::endl;
		}

		void OnPlayerDisconnect(int peerId) override
		{
			gsstl::clog << " OnPlayerDisconnect:" << peerId << gsstl::endl;
		}

		void OnReady(bool ready) override
		{
			gsstl::clog << " OnReady:" << gsstl::boolalpha << ready << gsstl::endl;
		}

		void OnPacket(const GameSparks::RT::RTPacket& packet) override
		{
			gsstl::clog << "OnPacket: " << packet.Data << gsstl::endl;
		}
};

#include <thread>

// reimplementation of usleep, so that this file can be used standalone.
namespace
{
	static void usleep(long long usec)
	{
		gsstl::this_thread::sleep_for(gsstl::chrono::microseconds(usec));
	}
}

#if _DURANGO || defined(NN_NINTENDO_SDK)
int sample_main(int argc, const char* argv[])
#else
int main(int argc, const char* argv[])
#endif
{
    (void)(argc); // unused
    (void)(argv); // unused

    using namespace GameSparks::Core;
	using namespace GameSparks::Api::Messages;

    GameSparks::Core::GS gs;
    
    SampleConfiguration::NativePlatformDescription platform;
	gs.Initialise(&platform);

	gs.GameSparksAvailable = GameSparksAvailable;

	// this will hold out GameSession once we have foud a match
	gsstl::unique_ptr<GameSession> gameSession;

	// these message handlers are called after a MatchmakingRequest has been sent (see SendMatchmakingRequest)

	// MatchFoundMessage
	gs.SetMessageListener<MatchFoundMessage>([&](GS&, const MatchFoundMessage& message) {
		gsstl::clog << "MatchFoundMessage: " << message.GetJSONString() << gsstl::endl;

		gsstl::stringstream ss;
		ss << message.GetPort().GetValue();

		gameSession.reset(new GameSession(
			message.GetAccessToken().GetValue(),
			message.GetHost().GetValue(),
			ss.str()
		));
	});

	// MatchNotFoundMessage
	gs.SetMessageListener<MatchNotFoundMessage>([&](GS&, const MatchNotFoundMessage& message) {
		gsstl::clog << "MatchNotFoundMessage: " << message.GetJSONString() << gsstl::endl;
		SendMatchmakingRequest(gs); // try again
	});

	// MatchUpdatedMessage
	gs.SetMessageListener<MatchUpdatedMessage>([&](GS&, const MatchUpdatedMessage& message) {
		gsstl::clog << "MatchUpdatedMessage: " << message.GetJSONString() << gsstl::endl;
	});

	int cyclesLeft = 200000;
	while (cyclesLeft-- > 0)
	{
		// deltaTime has to be provided in seconds
		gs.Update(0.1f);

		if (gameSession)
		{
			gameSession->Session->Update();
			GameSparks::RT::RTData data;
			data.SetInt(1, cyclesLeft); // do something useful here
			gameSession->Session->SendRTData(1, GameSparks::RT::GameSparksRT::DeliveryIntent::RELIABLE, data, {});
		}

		usleep(100000);
	}

	gs.ShutDown();

	return 0;
}
