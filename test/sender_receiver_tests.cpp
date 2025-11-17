//
// Created by aviar on 11/13/25.
//

#include <thread>
#include <iostream>
#include <catch2/catch_test_macros.hpp>
#include "Receiver.hpp"
#include "Sender.hpp"

TEST_CASE("Sender Start", "[start]") {

    Sender sender(8080);
    int sock = sender.Start(false);
    REQUIRE(0 <= sock);
}

TEST_CASE("Sender Send Data", "[send]") {

    Sender sender(8080);
    sender.Start(false);
    std::thread sender_thread([&sender]{
        int sock = sender.AcceptConnection();
        if (0 <= sock) {
            std::string str("Hello Client!");
            std::cout << "Sending data to client -> " << str << std::endl;
            sender.SendData(sock, str);
        }
    });

    std::thread receiver_thread([&]{
        Receiver receiver("0.0.0.0", 8080);
        std::string data = receiver.GetData(1024);
        std::cout << "Data from sender -> " << data << std::endl;
        REQUIRE(data.empty() == false);
    });

    sender_thread.join();
    receiver_thread.join();
}
