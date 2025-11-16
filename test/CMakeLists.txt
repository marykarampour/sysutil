include(Catch2Config)

add_executable(
        sock
        sender_tests.cpp
        receiver_tests.cpp
)

target_link_libraries(sock PRIVATE socket Catch2::Catch2WithMain -static)

include(CTest)
include(Catch)
catch_discover_tests(sock)