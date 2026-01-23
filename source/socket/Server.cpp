//
//  Server.cpp
//  
//
//  Created by Maryam Karampour on 2025-11-17.
//

#include "Server.hpp"
#include <iostream>
#include <format>
#include <stop_token>
#include <iterator>
#include <chrono>
#include "SenderReceiver.hpp"

ServerThread::ServerThread() {
    m_paused = false;
}

ServerThread::ServerThread(const ServerThread& server) {
    m_paused.store(server.m_paused.load());
}

ServerThread::~ServerThread() {
}

void ServerThread::Start(Sender& sender, const std::unordered_set<ServiceInfo>& services, ResponseCreator& creator) {
    m_thread = std::jthread([&](std::stop_token token) {
        while (!token.stop_requested()) {
            if (m_paused.load()) return;

            SenderReceiver request = SenderReceiver();
            request.SetSender(sender);
            
            std::stop_callback callback(token, [&]() {
                request.Stop();
            });
            
            std::pair<SYS_UTIL_REQUEST_STATUS, std::string> status = request.AcceptRequests(services, creator);
            std::this_thread::sleep_for(std::chrono::seconds(request_accept_interval));
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

Server::Server(int port, bool use_public_ip, const std::unordered_set<ServiceInfo>& services, ResponseCreator& creator) : m_services(services), m_response_creator(creator) {
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
        m_thread_pool.back().Start(m_sender, m_services, m_response_creator);
    }
}

void Server::Restart() {
    for (auto& t : m_thread_pool) {
        t.Restart();
    }
}

void Server::Pause() {
    for (auto& t : m_thread_pool) {
        t.Pause();
    }
}

void Server::Stop() {
    m_sender.Stop();
    for (auto& t : m_thread_pool) {
        t.Stop();
    }
    m_thread_pool.clear();
}

