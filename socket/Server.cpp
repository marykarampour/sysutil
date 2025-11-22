//
//  Server.cpp
//  P2P
//
//  Created by Maryam Karampour on 2025-11-17.
//

#include "Server.hpp"
#include <iostream>
#include <format>
#include <stop_token>

ServerThread::ServerThread() {
    m_paused = false;
    m_busy = false;
}

ServerThread::ServerThread(const ServerThread& server) {
    //TODO: Start to re-run the thread? Terminate then restart?
    m_paused.store(server.m_paused.load());
    m_busy.store(server.m_busy.load());
}

ServerThread::~ServerThread() {
}

void ServerThread::Start(Sender& sender) {
    m_thread = std::jthread([&](std::stop_token token) {
        while (!token.stop_requested()) {
            if (m_paused || m_busy) return;

            m_busy = true;
            int sock = sender.AcceptConnection();
            std::stop_callback callback(token, [&](){
                close(sock);
            });
            
            if (0 <= sock) {
                std::string str = std::format("Hello {}", sock);
                sender.SendData(sock, str);
                close(sock);
                m_busy = false;
            }
        }
        Stop();
    });
}

void ServerThread::Stop() {
    m_thread.request_stop();
}

void ServerThread::Pause() {
    m_paused = true;
}

void ServerThread::Restart() {
    m_paused = false;
}

Server::Server(int port, bool use_public_ip) {
    m_paused = false;
    m_sender = Sender(port);
    m_sender.Start(use_public_ip);
}

Server::~Server() {
    Stop();
}

void Server::Start(size_t pool_size) {
    m_thread_pool.clear();
    m_thread_pool.reserve(pool_size);

    for (size_t i=0; i<pool_size; i++) {
        m_thread_pool.emplace_back();
        m_thread_pool.back().Start(m_sender);
    }
}

void Server::Restart() {
    for (auto &t : m_thread_pool) {
        t.Restart();
    }
}

void Server::Pause() {
    for (auto &t : m_thread_pool) {
        t.Pause();
    }
}

void Server::Stop() {
    for (auto &t : m_thread_pool) {
        t.Stop();
    }
    m_sender.Stop();
    m_thread_pool.clear();
}

