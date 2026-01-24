include(Catch2Config)

#/usr/local/Cellar/openssl@3/3.6.0/lib/libcrypto.dylib
find_package(OpenSSL REQUIRED)

add_executable(
        sock
        sender_receiver_tests.cpp
)

target_link_libraries(sock PRIVATE socket Catch2::Catch2WithMain)
target_link_libraries(sock PRIVATE OpenSSL::SSL OpenSSL::Crypto)

include(CTest)
include(Catch)
catch_discover_tests(sock)
