//
// Created by aviar on 11/13/25.
//

#include <future>
#include <iostream>
#include <catch2/catch_test_macros.hpp>
#include "Receiver.hpp"

TEST_CASE("Receiver Get", "[get]") {

    std::promise<std::string> res;
    std::future<std::string> f = res.get_future();
    std::thread t([&res]{
        Receiver receiver("0.0.0.0", 8080);
        std::string data = receiver.GetData(1024);
        res.set_value(data);
    });

    f.wait();
    std::string str = f.get();
    std::cout << "Data from sender -> " << str << std::endl;
    REQUIRE(str.empty() == false);
    t.join();
}
