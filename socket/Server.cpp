//
//  Server.cpp
//  P2P
//
//  Created by Maryam Karampour on 2025-11-17.
//

#include "Server.hpp"
#include <iostream>

Server::Server(int port, bool use_public_ip, size_t pool_size) {
    
    m_sender = Sender(port);
    m_sender.Start(use_public_ip);
    
    for (size_t i=0; i<pool_size; i++) {
        m_thread_pool.emplace_back([&] {
            while (true) {
                if (m_paused) return;
                
                int sock = m_sender.AcceptConnection();
                if (0 <= sock) {
                    std::string str("Hello Client %d", sock);
                    std::cout << "Sending data to client -> " << str << std::endl;
                    m_sender.SendData(sock, str);
                }
            }
        });
    }
}

Server::~Server() {
    Stop();
}

void Server::Restart() {
    m_paused = false;
}

void Server::Pause() {
    m_paused = true;
}

void Server::Stop() {
    for (std::thread &t : m_thread_pool) {
        t.join();
    }
}

