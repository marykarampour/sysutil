set(
        SOCK_SOURCE
        Receiver.cpp
        Sender.cpp
        senderreceiver.c
        stun.c
        utility.c
)
add_library(socket STATIC ${SOCK_SOURCE})
target_include_directories(socket PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})