//
// Created by aviar on 11/13/25.
//

#include <thread>
#include <iostream>
#include <atomic>
#include <catch2/catch_test_macros.hpp>
#include "Receiver.hpp"
#include "Server.hpp"

TEST_CASE("Sender Start", "[start]") {

    Sender sender(8080);
    int sock = sender.Start(false);
    REQUIRE(0 <= sock);
}

TEST_CASE("Sender Send Data", "[send]") {

    std::thread sender_thread([&] {
        Sender sender(8080);
        sender.Start(false);
        int sock = sender.AcceptConnection();
        if (0 <= sock) {
            std::string str("Hello Client!");
            std::cout << "Sending data to client -> " << str << std::endl;
            sender.SendData(sock, str);
        }
    });

    std::thread receiver_thread([&] {
        Receiver receiver("0.0.0.0", 8080);
        std::string data = receiver.GetData(1024);
        std::cout << "Data from sender -> " << data << std::endl;
        REQUIRE(data.empty() == false);
    });

    sender_thread.join();
    receiver_thread.join();
}

TEST_CASE("Server", "[pool]") {

    Server server(8080, false);
    server.Start(4);
	std::atomic<int> success_count = 0;
	int client_count = 10;

    for (size_t i=0; i<client_count; i++) {
        std::jthread receiver_thread([&] {
            Receiver receiver("0.0.0.0", 8080);
            std::string data = receiver.GetData(1024);
            std::cout << "Data from sender -> " << data << std::endl;
            if (data.empty() == false) success_count.store(success_count.load()+1);
        });
    }

	server.Stop();
	REQUIRE(client_count == success_count);
}
