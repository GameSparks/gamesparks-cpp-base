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

class GetAchievementResponse : public GameSparks::Core::GSTypedResponse
{
public:
	GetAchievementResponse(const  GameSparks::Core::GSData& data)
		: GSTypedResponse(data)
	{

	}

	GetAchievementResponse(const GetAchievementResponse& other)
		: GSTypedResponse(other)
	{
	}
};

class GetAchievementRequest : public GameSparks::Core::GSTypedRequest < GetAchievementRequest, GetAchievementResponse >
{
public:
	GetAchievementRequest(GameSparks::Core::GS& gsInstance)
		: GSTypedRequest(gsInstance)
	{
		m_Request.AddString("eventKey", "GETACHIEVEMENT");
	}

	GetAchievementRequest& SetAchievementName(const gsstl::string& achievementName)
	{
		m_Request.AddString("ACHIEVEMENTNAME", achievementName);
		return *this;
	}
};

TEST(MessageDispatchingTest, Test01)
{
    using namespace GameSparks::Core;

    GS gs;

    bool isAvailable = false;
	bool registrationSucceeded = false;
	bool authenticationSucceeded = false;
	bool messageDispatched = false;

	gs.SetMessageListener<AchievementEarnedMessage>([&](GS& gs, const AchievementEarnedMessage& message) {
		std::clog << "message received" << std::endl;
		messageDispatched = true;
	});

    gs.GameSparksAvailable = [&](GameSparks::Core::GS& gsInstance, bool avail)
    {
        isAvailable = avail;

        if(avail)
        {
			RegistrationRequest reg_request(gs);
			reg_request.SetUserName("test_user");
			reg_request.SetPassword("test_password");
			reg_request.SetDisplayName("TestBert");
			reg_request.Send([&](GS& gs, const RegistrationResponse& reg_response) {
				registrationSucceeded = true;

				AuthenticationRequest auth_request(gs);
				auth_request.SetUserName("test_user");
				auth_request.SetPassword("test_password");
				auth_request.Send([&](GS& gs, const AuthenticationResponse& auth_response) {
					ASSERT_FALSE(auth_response.GetHasErrors()) << auth_response.GetErrors().GetValue().GetJSON();
					if (auth_response.GetHasErrors())
					{
						std::clog << "auth failed!" << std::endl;
					}
					else
					{
						authenticationSucceeded = true;

						GetAchievementRequest request(gsInstance);
						request.SetAchievementName("MYACHIEVEMENT");
						request.Send();
					}
				});
			});
        }
    };

    bool condition_passed = TestPlatform::runTestLoop(
            gs,
            [&](){
                return isAvailable && registrationSucceeded && authenticationSucceeded && messageDispatched;
            });

    ASSERT_TRUE(condition_passed) << "the test timed out";
    ASSERT_TRUE(isAvailable) << "gamesparks is not available";
    ASSERT_TRUE(authenticationSucceeded) << "the authentication request failed";
}

TEST(MessageDispatchingTest, Clear)
{
	GS gs;

	gs.SetMessageListener<AchievementEarnedMessage>([&](GS& gs, const AchievementEarnedMessage& message) {
	});
	gs.SetMessageListener<AchievementEarnedMessage>({});

	gs.SetMessageListener<AchievementEarnedMessage>([&](GS& gs, const AchievementEarnedMessage& message) {
	});
	gs.ClearMessageListener<AchievementEarnedMessage>(); // does the same

	gs.SetMessageListener<AchievementEarnedMessage>([&](GS& gs, const AchievementEarnedMessage& message) {
	});
	gs.ClearAllMessageListeners();
}
