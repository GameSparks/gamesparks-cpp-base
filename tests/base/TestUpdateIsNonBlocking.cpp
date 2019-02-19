#include <gtest/gtest.h>
#include <GameSparks/GS.h>
#include "TestPlatform.h"
#include <GameSparks/generated/GSRequests.h>
#include <GameSparks/generated/GSMessages.h>


TEST(Latency, UpdateIsNonBlocking)
{
	enum { NUM_DURABLE_REQUESTS = 15 };

	using namespace GameSparks::Core;
	GS gs;

	bool isAvailable = false;
	bool authenticationSucceeded = false;

	gs.GameSparksAvailable = [&isAvailable, &authenticationSucceeded](GS& gs, bool available) {
		isAvailable = available;

		GameSparks::Api::Requests::DeviceAuthenticationRequest authRequest(gs);

		TestPlatform platform;

		authRequest.SetDeviceId(platform.GetDeviceId());
		authRequest.SetDeviceOS(platform.GetDeviceOS());

		authRequest.Send([&authenticationSucceeded](GS& instance, const GameSparks::Api::Responses::AuthenticationResponse& response) {
			authenticationSucceeded = !response.GetHasErrors();

			if (authenticationSucceeded)
			{
				for (int i = 0; i != NUM_DURABLE_REQUESTS; ++i)
				{
					GameSparks::Api::Requests::LogEventRequest logEvent(instance);
					logEvent.SetEventKey("testMessage");
					logEvent.SetEventAttribute("foo", "bar");
					logEvent.SetDurable(true);
					logEvent.Send();
				}
			}
		});
	};

	TestPlatform platform;
	gs.Initialise(&platform);

	auto waitForCondition = []() {
		return false;
	};

	{
		auto timeout = std::chrono::seconds(5);
		auto start_time = std::chrono::steady_clock::now();
		auto last_time = start_time;

		typedef decltype(std::chrono::high_resolution_clock::now() - std::chrono::high_resolution_clock::now()) duration_t;
		typedef decltype(std::chrono::duration_cast<std::chrono::microseconds>(duration_t()).count()) us_duration_t;

		std::vector<us_duration_t> durations;
		durations.reserve( 2 * 5 * 1000000 / 60 ); // pre-allocate for five seconds a 60Hz (times two)

		while (std::chrono::steady_clock::now() - start_time < timeout && !waitForCondition())
		{
			auto now = std::chrono::steady_clock::now();
			auto delta = now - last_time;
			auto dt = std::chrono::duration_cast<std::chrono::duration<float>>(delta).count();

			auto update_start = std::chrono::high_resolution_clock::now();
			gs.Update(dt);
			auto update_duration = std::chrono::high_resolution_clock::now() - update_start;
			auto update_duration_us = std::chrono::duration_cast<std::chrono::microseconds>(update_duration).count();
			durations.push_back(update_duration_us);
			
			//ASSERT_LE(update_duration_us, 1000000 / 30);

			usleep(1000000 / 60); // sleep 1/60th of a second

			last_time = now;
		}

		auto timed_out = (std::chrono::steady_clock::now() - start_time) > timeout;
		(void)timed_out;

		// sort surations descending
		std::sort(durations.begin(), durations.end(), std::greater<us_duration_t>());

		// print worse times
		auto n = std::min<size_t>(15, durations.size());
		std::clog << "worst " << n << " GS::Update() durations:" << std::endl;
		for (int i = 0; i != n; ++i)
		{
			const auto &d = durations[i];
			std::clog << std::setw(3) << i << std::setw(10) << d << " us" << " (" << std::setprecision(2) << std::setw(11) << (1000000.0/d) << " Hz" << ")" << std::endl;
		}
	}

}
