//
// Created by aviar on 11/13/25.
//

#include <future>
#include <iostream>
#include <catch2/catch_test_macros.hpp>
#include "Sender.hpp"

TEST_CASE("Sender Start", "[start]") {

    std::promise<int> res;
    std::future<int> f = res.get_future();
    std::thread t([&res]{
        Sender sender(8080);
        int sock = sender.Start();
        res.set_value(sock);
    });

    f.wait();
    REQUIRE(0 <= f.get());
    t.join();
}

TEST_CASE("Sender Accept", "[accept]") {

    Sender sender(8080);
    sender.Start();
    std::promise<int> res;
    std::future<int> f = res.get_future();
    std::thread t([&res, &sender]{
        int sock = sender.AcceptConnection();
        res.set_value(sock);
    });

    f.wait();
    REQUIRE(0 <= f.get());
    t.join();
}

TEST_CASE("Sender Send Data", "[send]") {

    Sender sender(8080);
    sender.Start();
    std::promise<int> res;
    std::future<int> f = res.get_future();
    std::thread t([&res, &sender]{
        int sock = sender.AcceptConnection();
        if (0 <= sock) {
            std::string str("Hello Client!");
            std::cout << "Sending data to client -> " << str << std::endl;
            sender.SendData(sock, str);
        }
        res.set_value(sock);
    });

    f.wait();
    REQUIRE(0 <= f.get());
    t.join();
}