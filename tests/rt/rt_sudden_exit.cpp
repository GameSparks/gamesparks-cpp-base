#include <GameSparksRT/IRTSessionListener.hpp>
#include <GameSparksRT/IRTSession.hpp>
#include <GameSparksRT/RTData.hpp>


#include <gtest/gtest.h>
#include "rt_TestUtils.hpp"
#include <memory>
#include <chrono>

using namespace GameSparks::RT;

namespace { // anonymous namespace
    class Player : public GameSparks::RT::IRTSessionListener
    {
        public:
            std::unique_ptr<IRTSession> Session;
            volatile bool read;

            Player(const std::string &connectToken, const std::string &host, const std::string &port,
                   const std::string &name) {
                Session.reset(GameSparksRT
                              ::SessionBuilder()
                                      .SetConnectToken(connectToken)
                                      .SetHost(host)
                                      .SetPort(port)
                                      .SetListener(this)
                                      .Build());

                Session->Start();
            }
    };
} /* anonymous namespace */

static void run_for_seconds(const std::function<void ()>& f, int seconds)
{
    for(auto start = std::chrono::steady_clock::now(); std::chrono::steady_clock::now() < start + std::chrono::seconds(seconds);)
    {
        f();
    }
}


TEST(Basic, SuddenExit)
{
    auto creds = TestUtils::getSessions();
    Player p1(creds[0].token, creds[1].host, creds[1].port, creds[0].name);
}


TEST(Basic, WrongHost)
{
    auto creds = TestUtils::getSessions();
    Player p1(creds[0].token, "www.i-do-not-exist-123-321.com", "123", creds[0].name);
}

TEST(Basic, WrongPort)
{
    auto creds = TestUtils::getSessions();
    Player p1(creds[0].token, creds[0].host, "abc", creds[0].name);
    run_for_seconds([&](){ p1.Session->Update(); }, 2);
}

TEST(Basic, WrongPort2)
{
    auto creds = TestUtils::getSessions();
    Player p1(creds[0].token, creds[0].host, "12345", creds[0].name);
    run_for_seconds([&](){ p1.Session->Update(); }, 2);
}

TEST(Basic, WrongService)
{
    auto creds = TestUtils::getSessions();
    Player p1(creds[0].token, "google.com", "80", "player1");
    run_for_seconds([&](){ p1.Session->Update(); }, 2);
}

TEST(Basic, WrongService2)
{
    auto creds = TestUtils::getSessions();
    Player p1(creds[0].token, "8.8.8.8", "53", "player1");
    run_for_seconds([&](){ p1.Session->Update(); }, 2);
}

TEST(Basic, WrongServiceTwoPlayers)
{
    auto creds = TestUtils::getSessions();
    Player p1(creds[0].token, "google.com", "80", creds[0].name);
    Player p2(creds[1].token, "google.com", "80", creds[1].name);
    run_for_seconds([&](){ p1.Session->Update(); p2.Session->Update(); }, 2);
}

TEST(Basic, DestroyListenerBeforeSession)
{
    struct Listener : public IRTSessionListener {

        virtual void OnPlayerConnect(int peerId) override
        {
            ASSERT_TRUE(false);
            IRTSessionListener::OnPlayerConnect(peerId);
        }

        virtual void OnPlayerDisconnect(int peerId) override
        {
            ASSERT_TRUE(false);
            IRTSessionListener::OnPlayerDisconnect(peerId);
        }

        virtual void OnReady(bool ready) override
        {
            ASSERT_TRUE(false);
            IRTSessionListener::OnReady(ready);
        }

        virtual void OnPacket(const RTPacket &packet) override
        {
            ASSERT_TRUE(false);
            IRTSessionListener::OnPacket(packet);
        }
    };

    auto listener = new Listener();

    auto creds = TestUtils::getSessions();

    auto session = GameSparksRT::SessionBuilder ()
        .SetConnectToken (creds[0].token)
        .SetHost (creds[0].host)
        .SetPort (creds[0].port)
        .SetListener(listener)
        .Build ();
    session->Start();
    delete listener; // the listener should unregister itself and the following run just fine.
    Player p2(creds[1].token, creds[1].host, creds[1].port, creds[1].name);
    run_for_seconds([&](){ session->Update(); p2.Session->Update(); }, 5);
    delete session;
}
