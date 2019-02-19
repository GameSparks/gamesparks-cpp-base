#include <gtest/gtest.h>
#include <easywsclient/easywsclient.hpp>
#include <thread>
#include <GameSparks/../../src/System/Net/Sockets/TcpClient.hpp>

#if WIN32
// note: only works for WINAPI_FAMILY_PARTITION(WINAPI_PARTITION_DESKTOP)
TEST(TLS, CertificateVerificationFails)
{
	auto ws = easywsclient::WebSocket::from_url("wss://174.129.224.73/");
	ASSERT_NE(nullptr, ws);

	bool verifying_invalid_cert_failed = true;

	for (int i = 0; i != 30; ++i)
	{
		ws->poll(0, [](const easywsclient::WSError& error, void* verifying_invalid_cert_failed_) {
			//Note: the TLS Handshake happens in the DNS Lookup thread.
			if (error.code != easywsclient::WSError::CONNECTION_CLOSED)
			{
				ASSERT_EQ(error.code, easywsclient::WSError::CONNECT_FAILED) << error.message;
				*reinterpret_cast<bool*>(verifying_invalid_cert_failed_) = true;
				std::cerr << "ERROR" << error.message << std::endl;
			}
		}, &verifying_invalid_cert_failed);
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
	}

	ASSERT_TRUE(verifying_invalid_cert_failed);

	delete ws;
}

TEST(TLS, CertificateVerificationFailsForTLSSocket)
{
	System::Net::Sockets::TcpClient client(System::Net::Sockets::AddressFamily::InterNetwork);
	System::Net::IPEndPoint endpoint;
	endpoint.Host = "174.129.224.73";
	endpoint.Port = "https";

	client.BeginConnect(endpoint, [&](const System::IAsyncResult& ar) {

	});

	for (int i = 0; i != 30; ++i)
	{
		client.Poll();
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
	}

	ASSERT_FALSE(client.Connected());
}
#endif

// --gtest_filter=TLS.CertificateVerification
TEST(TLS, CertificateVerification)
{
	auto ws = easywsclient::WebSocket::from_url("wss://echo.websocket.org/");

	ASSERT_NE(nullptr, ws);

	for (int i = 0; i != 30; ++i)
	{
		ws->poll(0, [](const easywsclient::WSError& error, void*) {
			ASSERT_TRUE(false);
			std::cerr << "ERROR" << error.message << std::endl;
		}, nullptr);
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
	}

	delete ws;
}


TEST(TLS, CertificateVerificationSucceedsForTLSSocket)
{
	System::Net::Sockets::TcpClient client(System::Net::Sockets::AddressFamily::InterNetwork);
	System::Net::IPEndPoint endpoint;
	endpoint.Host = "echo.websocket.org";
	endpoint.Port = "https";

	client.BeginConnect(endpoint, [&](const System::IAsyncResult& ar) {

	});

	for (int i = 0; i != 30; ++i)
	{
		client.Poll();
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
	}

	ASSERT_TRUE(client.Connected());
}
