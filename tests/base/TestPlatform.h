#include <GameSparks/IGSPlatform.h>
#include <GameSparks/GS.h>
#include <iostream>
#include <cassert>
#include "usleep.h"
#include <chrono>
#include <gtest/gtest.h>

#if defined(_MSC_VER)
#	pragma warning(disable: 4996)
#endif

class TestPlatform : public GameSparks::Core::IGSPlatform
{
        static gsstl::string get_env_or_fail(const gsstl::string& name)
        {
            const char* name_ = name.c_str();

            const char* val_ = std::getenv(name_);
            
            gsstl::string str_val(val_?val_:"");
            
            EXPECT_NE(str_val, "") << "required environment variable named " << name << " is not set";
            
            if ( !val_ )
            {
                std::clog << "Failed to get environmant varaible named " << name << std::endl;
				assert(false);
                exit(1);
            }
            return val_;
        }

    public:
		static gsstl::string get_api_key()
		{
			return get_env_or_fail("GAMESPARKS_API_KEY");
		}

		static gsstl::string get_api_secret()
		{
			return get_env_or_fail("GAMESPARKS_API_SECRET");
		}

		static gsstl::string get_rt_seesion_0()
		{
			return get_env_or_fail("RT_SESSION_0");
		}

		static gsstl::string get_rt_seesion_1()
		{
			return get_env_or_fail("RT_SESSION_1");
		}

		TestPlatform(gsstl::string api_key = TestPlatform::get_api_key(), gsstl::string api_secret=TestPlatform::get_api_secret())
        :GameSparks::Core::IGSPlatform(api_key, api_secret, true, true)
        {}

        virtual gsstl::string GetDeviceId() const
        {
            return IGSPlatform::GetDeviceId() + "-TEST-RUNNER";
        }

        virtual gsstl::string GetDeviceType() const
        {
            return "TEST-RUNNER";
        }

        virtual gsstl::string GetSDK() const
        {
            return "C++";
        }

        virtual void DebugMsg(const gsstl::string& message) const
        {
            std::cout << "GameSparks: " << message << std::endl;
        }

        /*! helper to run the main-loop
        * returns false, if loop timeout has reached, true otherwise
        */
        template <typename Platform=TestPlatform>
        static bool runTestLoop
        (
            GameSparks::Core::GS& gs, ///< the gs instance
            std::function<bool ()> waitForCondition,// = [](){ return true; }, ///< run loop, while this condition is NOT true
            std::chrono::steady_clock::duration timeout = std::chrono::seconds(10)
        )
        {
            Platform platform;
            gs.Initialise(&platform);

            auto start_time = std::chrono::steady_clock::now();
            auto last_time = start_time;

            while (std::chrono::steady_clock::now() - start_time < timeout && !waitForCondition())
            {
                auto now = std::chrono::steady_clock::now();
                auto delta = now - last_time;
                auto dt = std::chrono::duration_cast<std::chrono::duration<float>>(delta).count();

                gs.Update(dt);
                usleep(100000);

                last_time = now;
            }

            return (std::chrono::steady_clock::now() - start_time) < timeout;
        }
};
