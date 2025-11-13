//
// Created by aviar on 11/13/25.
//

#include <catch2/catch_test_macros.hpp>
#include "Sender.hpp"

TEST_CASE("Sender tests", "[sender]") {
    Sender sender(8080);
    REQUIRE(0 <= sender.Start());
}