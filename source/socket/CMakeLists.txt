set(
        SOCK_SOURCE
        SenderReceiver.cpp
        Sender.cpp
        Server.cpp
        ServiceInfo.cpp
        RequestProcessor.cpp
        socketutil.c
        stun.c
        ../utility/cutility.c
        ../utility/CPPUtility.cpp
)

set(OPENSSL_ROOT_DIR /opt/homebrew/opt/openssl@3)
find_package(OpenSSL REQUIRED)

add_library(socket ${SOCK_SOURCE})
target_include_directories(socket PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_include_directories(socket PUBLIC ${CMAKE_SOURCE_DIR}/source/utility)
target_link_libraries(socket PUBLIC OpenSSL::SSL OpenSSL::Crypto)
