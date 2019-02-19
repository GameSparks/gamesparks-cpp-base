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
    static std::vector<RTVector> MakeTestVectors()
    {
        RTVector vx;
        vx.x = 9.75f;

        RTVector vxy;
        vxy.x = 5.77f;
        vxy.y = 0.53f;

        RTVector vxyz;
        vxyz.x = 3.73f;
        vxyz.y = 3.16f;
        vxyz.z = 5.87f;

        RTVector vxyzw;
        vxyzw.x = 9.15f;
        vxyzw.y = 0.22f;
        vxyzw.z = 8.88f;
        vxyzw.w = 7.21f;

        return {vx, vxy, vxyz, vxyzw};
    }

    static RTData MakeTestData(int peerId)
    {
        RTData data;
        data.SetFloat(1, 43.0f);
        data.SetInt(2, 44);
        data.SetLong(3, std::numeric_limits<int64_t>::max());
		data.SetLong(4, std::numeric_limits<int64_t>::lowest());
		data.SetString(5, "Hello, World!");
        data.SetInt(6, peerId);
        data.SetDouble(7, 42.0);

        int i = 8;
        for (const auto &v : MakeTestVectors()) {
            data.SetRTVector(i, v);
            ++i;
        }
        return data;
    }

    static const System::Bytes test_payload = {1, 0, 2, 0, 3, 0, 255, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 95, 96, 97, 98, 99, 100, 101, 102, 103, 104, 105, 106, 107, 108, 109, 110, 111, 112, 113, 114, 115, 116, 117, 118, 119, 120, 121, 122, 123, 124, 125, 126, 127, 128, 129, 130, 131, 132, 133, 134, 135, 136, 137, 138, 139, 140, 141, 142, 143, 144, 145, 146, 147, 148, 149, 150, 151, 152, 153, 154, 155, 156, 157, 158, 159, 160, 161, 162, 163, 164, 165, 166, 167, 168, 169, 170, 171, 172, 173, 174, 175, 176, 177, 178, 179, 180, 181, 182, 183, 184, 185, 186, 187, 188, 189, 190, 191, 192, 193, 194, 195, 196, 197, 198, 199, 200, 201, 202, 203, 204, 205, 206, 207, 208, 209, 210, 211, 212, 213, 214, 215, 216, 217, 218, 219, 220, 221, 222, 223, 224, 225, 226, 227, 228, 229, 230, 231, 232, 233, 234, 235, 236, 237, 238, 239, 240, 241, 242, 243, 244, 245, 246, 247, 248, 249, 250, 251, 252, 253, 254, 255};

    static void ValidateTestData(const RTPacket& packet, int peerId)
    {
        ASSERT_EQ(packet.Data.GetFloat(1).Value(), 43.0f);
        ASSERT_EQ(packet.Data.GetInt(2).Value(), 44);
        ASSERT_EQ(packet.Data.GetLong(3).Value(), std::numeric_limits<int64_t>::max());
		ASSERT_EQ(packet.Data.GetLong(4).Value(), std::numeric_limits<int64_t>::lowest());
		ASSERT_EQ(packet.Data.GetString(5).Value(), "Hello, World!");
        ASSERT_NE(packet.Data.GetInt(6).Value(), peerId);
        ASSERT_EQ(packet.Data.GetDouble(7).Value(), 42.0);

        int i = 8;
        for (const auto &v : MakeTestVectors()) {
            auto from_network = packet.Data.GetRTVector(i);
            ASSERT_EQ(v, from_network.Value());
            ++i;
        }

        ASSERT_EQ(packet.Payload, test_payload);
    }

    class Player : public GameSparks::RT::IRTSessionListener
    {
        private:
            volatile int number_of_received_packets;

        protected:
            std::unique_ptr<IRTSession> Session;

        public:
            Player(const std::string &connectToken, const std::string &host, const std::string &port, GameSparksRT::DeliveryIntent deliveryIntent)
            : number_of_received_packets(0)
			, got_all_packets(false)
            , deliveryIntent(deliveryIntent)
            {
                Session.reset(GameSparksRT::SessionBuilder()
                    .SetConnectToken(connectToken)
                    .SetHost(host)
                    .SetPort(port)
                    .SetListener(this)
                    .Build());

                Session->Start();
            }

			volatile bool got_all_packets;

            virtual void OnPacket(const RTPacket &packet) override
            {
                ValidateTestData(packet, Session->PeerId.Value());
                number_of_received_packets++;
                got_all_packets = number_of_received_packets >= 10;
            }

            void Update()
            {
                if (Session->ActivePeers.size() > 1) {
                    auto data = MakeTestData(Session->PeerId.Value());
                    Session->SendData(120, deliveryIntent, test_payload, data, {});
                }
                this->Session->Update();
            }

            void Stop()
            {
                this->Session->Stop();
            }

        protected:
            GameSparksRT::DeliveryIntent deliveryIntent;
    };

    class PingPongPlayer : public Player
    {
        public:
            PingPongPlayer (const std::string &connectToken, const std::string &host, const std::string &port, GameSparksRT::DeliveryIntent deliveryIntent, int counter)
            :Player(connectToken, host, port, deliveryIntent), counter(counter) {}

            int counter;

            virtual void OnPacket(const RTPacket &packet) override
            {
                std::clog << "Player " << Session->PeerId << " got packet" << std::endl;

                //ASSERT_FALSE(got_all_packets);

                ValidateTestData(packet, Session->PeerId.Value());
                auto data = MakeTestData(Session->PeerId.Value());
                Session->SendData(120, deliveryIntent, test_payload, data, {});
                counter--;

                if(counter==0)
                {
                    got_all_packets = true;
                }
            }

            virtual void OnReady(bool ready) override
            {
                if (counter % 2 == 0 && Session->ActivePeers.size() > 1) {
                    auto data = MakeTestData(Session->PeerId.Value());
                    Session->SendData(120, deliveryIntent, test_payload, data, {});
                    counter--;
                }
            }

            virtual void OnPlayerConnect (int peerId) override
            {
                OnReady(true);
            }

            void Update()
            {
                this->Session->Update();
            }
    };
} /* anonymous namespace */

                
static void run_for_seconds(const std::function<bool ()>& f, int seconds)
{
    for(auto start = std::chrono::steady_clock::now(); std::chrono::steady_clock::now() < start + std::chrono::seconds(seconds);)
    {
        if (f())
            return;
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
    throw std::runtime_error("Test timed out!");
}


static void TestRTPacketDeliveryPingPong(GameSparksRT::DeliveryIntent deliveryIntent)
{
    auto creds = TestUtils::getSessions();
    PingPongPlayer p1(creds[0].token, creds[0].host, creds[0].port, deliveryIntent, 6);
    PingPongPlayer p2(creds[1].token, creds[1].host, creds[1].port, deliveryIntent, 5);

    ASSERT_NO_THROW(run_for_seconds([&]()
    {
        p1.Update();
        p2.Update();
        return p1.got_all_packets && p2.got_all_packets;
    }, 120));

    p1.Stop();
    p2.Stop();
}


TEST(Delivery, RTPacketDeliveryPingPongReliable)
{
    TestRTPacketDeliveryPingPong(GameSparksRT::DeliveryIntent::RELIABLE);
}

TEST(Delivery, RTPacketDeliveryPingPongUnreliable)
{
    TestRTPacketDeliveryPingPong(GameSparksRT::DeliveryIntent::UNRELIABLE);
}

TEST(Delivery, RTPacketDeliveryPingPongUnreliableSequenced)
{
    TestRTPacketDeliveryPingPong(GameSparksRT::DeliveryIntent::UNRELIABLE_SEQUENCED);
}




static void TestRTPacketDelivery(GameSparksRT::DeliveryIntent deliveryIntent)
{
    auto creds = TestUtils::getSessions();
    Player p1(creds[0].token, creds[0].host, creds[0].port, deliveryIntent);
    Player p2(creds[1].token, creds[1].host, creds[1].port, deliveryIntent);

    ASSERT_NO_THROW(run_for_seconds([&]()
    {
        p1.Update();
        p2.Update();
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        return p1.got_all_packets && p2.got_all_packets;
    }, 120));

    p1.Stop();
    p2.Stop();
}

TEST(Delivery, RTPacketDeliveryReliable)
{
    TestRTPacketDelivery(GameSparksRT::DeliveryIntent::RELIABLE);
}

TEST(Delivery, RTPacketDeliveryUnreliable)
{
    TestRTPacketDelivery(GameSparksRT::DeliveryIntent::UNRELIABLE);
}

TEST(Delivery, RTPacketDeliveryUnreliableSequenced)
{
    TestRTPacketDelivery(GameSparksRT::DeliveryIntent::UNRELIABLE_SEQUENCED);
}