#include <iostream>
#include <GameSparksRT/IRTSessionListener.hpp>
#include <GameSparksRT/IRTSession.hpp>
#include <GameSparksRT/RTData.hpp>
#include <list>
#include <thread>
#include <memory>

#include "../../src/System/Threading/Thread.hpp"
#include "rt_TestUtils.hpp"

namespace GameSparks { namespace RT { namespace Test
{
    class Tester : IRTSessionListener
    {
        public: std::unique_ptr<IRTSession> Session;
        private: const std::string name;
        //private: std::list<RTData> received;
        public: Tester(const std::string& connectToken, const std::string& host, const std::string& port, const std::string& name)
                    :name(name)
            {
                Session.reset(GameSparksRT
                ::SessionBuilder ()
                        .SetConnectToken (connectToken)
                        .SetHost (host)
                        .SetPort (port)
                        .SetListener (this)
                        .Build ());

                Session->Start ();
            }

        public: void OnPlayerConnect (int peerId) override
        {
            std::clog << name << " OnPlayerConnect:" << peerId << std::endl;
        }

        public: void OnPlayerDisconnect (int peerId) override
        {
            std::clog << name << " OnPlayerDisconnect:" << peerId << std::endl;
        }

        public: void OnReady (bool ready) override
        {
            std::clog << name << " OnReady:" << std::boolalpha << ready << std::endl;
        }

        public: void OnPacket (const RTPacket& packet) override {
			std::list<RTData> received;
			received.push_back (packet.Data);
            for (const RTData& data : received) {
                std::clog << "R:" << data << std::endl;
            }
            //std::clog << name + " OnPacket " + System::String::ToString(packet.OpCode) + " " + System::String::ToString(packet.Sender) << packet.Data << std::endl;
            //Console.WriteLine (name + " OnPacket " + packet.OpCode + " " + packet.Sender + " " + (packet.Data != null ? packet.Data.ToString() : ""));
        }
    };

    class Program
    {
        static bool isLittleEndian()
        {
            short int number = 0x1;
            char *numPtr = (char*)&number;
            return (numPtr[0] == 1);
        }

        public: static void Main() {

            bool running = true;

            auto creds = TestUtils::getSessions();

            Tester tester1(creds[0].token, creds[0].host, creds[0].port, creds[0].name);
            Tester tester2(creds[1].token, creds[1].host, creds[1].port, creds[1].name);

            //This thread will call Update on the session
            //Will generally be done by a MonoBehaviour
            auto bgThread = std::thread([&running, &tester1, &tester2](){
                    System::Threading::Thread::SetName("Test Update Thread");

                    //Start it !
                    while(running){
                        tester1.Session->Update();
                        tester2.Session->Update();
                        std::this_thread::sleep_for(std::chrono::milliseconds(100));
                        tester1.Session->SendData(120, GameSparksRT::DeliveryIntent::RELIABLE, { 0, 1 }, GetRTData(1), {});
                    }
            });

            //Wait for some console input before stopping
            /*while (true) {
                std::string line;
                std::getline(std::cin, line);

                if (line == "exit") {
                    running = false;
                    tester1.Session->Stop ();
                    tester2.Session->Stop ();
                    return;
                } else if (!line.empty()){
                    int opCode = std::stoi(line);
                    //using(
                    RTData data = GetRTData(opCode);
                    //){
                    Tester& toSendTo = tester1;//(opCode % 2 == 0) ? tester2 : tester1;
                    toSendTo.Session->SendData(opCode, GameSparksRT::DeliveryIntent::RELIABLE, { 0, 1 }, data, {});
                    //}
                }
            }*/

            for(auto start = std::chrono::steady_clock::now(), now=start; now < start + std::chrono::seconds(5); now = std::chrono::steady_clock::now())
            {
                int opCode = rand() % 32+1;
                RTData data = GetRTData(opCode);
                Tester& toSendTo = tester1;//(opCode % 2 == 0) ? tester2 : tester1;
                toSendTo.Session->SendData(opCode, GameSparksRT::DeliveryIntent::RELIABLE, { 0, 1 }, data, {});
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
            }

            running = false;
            tester1.Session->Stop ();
            tester2.Session->Stop ();

            if(bgThread.joinable())
                bgThread.join();
        }

        private: static RTData GetRTData(int index){
            if (index == 1) {
                return RTData()
                        .SetLong (1, 1)
                        .SetString (2, "Gabs")
                        .SetData (3, RTData()
                                .SetLong (1, 1)
                                .SetString (2, "Gabs")
                        )
                        .SetRTVector (4, RTVector (1.f, 2.f))
                        .SetRTVector (5, RTVector (1.f, 2.f, 3.f))
                        .SetRTVector (6, RTVector (1.f, 2.f, 3.f, 4.f));
            } else if (index == 2) {
                return RTData()
                    .SetLong (1, 1);
            }
            return {};
        }
    };

}}}

#include <gtest/gtest.h>
TEST(GameSparksRT, Simple)
{
    System::Threading::Thread::SetName("Main Thread");
    GameSparks::RT::Test::Program::Main();
}
