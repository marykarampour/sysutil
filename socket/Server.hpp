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
#include "Sender.hpp"

class Server {
    std::vector<std::thread> m_thread_pool;
    std::atomic<bool> m_paused;
    int m_port;
    Sender m_sender;
    
public:
    Server(int port, bool use_public_ip, size_t pool_size = std::thread::hardware_concurrency());
    ~Server();
    
    void Restart();
    void Pause();
    void Stop();
};

#endif /* Server_hpp */
