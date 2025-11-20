//
//  Server.hpp
//  P2P
//
//  Created by Maryam Karampour on 2025-11-17.
//

#ifndef Server_hpp
#define Server_hpp

#include <stdio.h>
#include <vector>
#include <thread>
#include <atomic>
#include <functional>
#include "Sender.hpp"

class ServerThread {
    std::jthread m_thread;
    std::atomic<bool> m_running;
    std::atomic<bool> m_paused;
    std::atomic<bool> m_busy;

public:
    ServerThread();
    ServerThread(const ServerThread& server);
    ~ServerThread();

    void Start(Sender sender, const std::function<int()>&& stop_handler);
    void Restart();
    void Pause();
    void Stop();
};

class Server {
    std::vector<ServerThread> m_thread_pool;
    std::atomic<bool> m_paused = false;
    std::atomic<bool> m_running_counter = 0;
    int m_port;
    Sender m_sender;
    
public:
    Server(int port, bool use_public_ip);
    ~Server();

    void Start(size_t pool_size = std::thread::hardware_concurrency());
    void Restart();
    void Pause();
    void Stop();
};

#endif /* Server_hpp */
