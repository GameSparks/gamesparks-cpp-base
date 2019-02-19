#include <GameSparksRT/IRTSessionListener.hpp>
#include <GameSparksRT/IRTSession.hpp>
#include <GameSparksRT/RTData.hpp>

#include <gtest/gtest.h>
#include <thread>
#include <memory>
#include <chrono>
#include "rt_TestUtils.hpp"

using namespace GameSparks::RT;

namespace // anonymous namespace
{
    class Player : public GameSparks::RT::IRTSessionListener
    {
            std::unique_ptr<IRTSession> Session;
        public:
            Player(const std::string &connectToken, const std::string &host, const std::string &port)
            {
                Session.reset(GameSparksRT::SessionBuilder()
                  .SetConnectToken(connectToken)
                  .SetHost(host)
                  .SetPort(port)
                  .SetListener(this)
                  .Build());

                Session->Start();
            }

            int got_packets = 0;
            int got_packets_after_close = 0;
            bool session_was_closed = false;
            bool is_ready = false;

            virtual void OnReady(bool b) override {
                is_ready = true;
            }

            virtual void OnPacket(const RTPacket &packet) override {
                if(session_was_closed)
                    got_packets_after_close++;
                else
                    got_packets++;
            }

            void Send() {
                static const System::Bytes test_payload = {1, 2, 3};
                Session->SendBytes(1, GameSparksRT::DeliveryIntent::RELIABLE, test_payload, {});
            }

            void Update() {
                this->Session->Update();
            }

            void Stop()  {
                this->Session->Stop();
                session_was_closed = true;
            }
    };

} /* anonymous namespace */

typedef decltype(std::chrono::steady_clock::now()-std::chrono::steady_clock::now()) duration;

static void run_for_seconds(const std::function<bool (const duration&)>& f, int seconds)
{
    for(auto start = std::chrono::steady_clock::now(); std::chrono::steady_clock::now() < start + std::chrono::seconds(seconds);)
    {
        if (f(std::chrono::steady_clock::now()-start))
            return;
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
    throw std::runtime_error("Test timed out!");
}

/*!
 * Scenario: as soo as the session is reasy player1 sends packets to player2
 * After player2 got 5 packets the session is clossed.
 *
 * OnPacket() should no longer be called after the session was closed.
 *
 * */
TEST(Delivery, NoPacketsAfterClose)
{
    auto creds = TestUtils::getSessions();

    Player p1(creds[0].token, creds[0].host, creds[0].port);
    Player p2(creds[1].token, creds[1].host, creds[1].port);

    ASSERT_NO_THROW(run_for_seconds([&](const duration& duration)
    {
        p1.Update();
        p2.Update();

        if(p2.is_ready)
            p2.Send();

        if(p1.got_packets >= 5 && !p1.session_was_closed)
            p1.Stop();

        std::this_thread::sleep_for(std::chrono::milliseconds(100));

        return duration >= std::chrono::seconds(8);
    }, 10));

    ASSERT_EQ(p1.got_packets_after_close, 0);
    ASSERT_TRUE(p1.session_was_closed);
    ASSERT_FALSE(p2.session_was_closed);
    ASSERT_EQ(p2.got_packets, 0);
    ASSERT_GE(p1.got_packets, 5);

    p1.Stop();
    p2.Stop();
}
