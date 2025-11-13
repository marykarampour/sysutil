include(Catch2Config)

add_executable(sock sock_tests.cpp)

target_link_libraries(sock PRIVATE socket Catch2::Catch2WithMain)

include(CTest)
include(Catch)
catch_discover_tests(sock)