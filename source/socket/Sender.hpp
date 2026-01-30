//
//  Sender.hpp
//  
//
//  Created by Maryam Karampour on 2025-11-09.
//

#ifndef Sender_hpp
#define Sender_hpp

#include <stdio.h>
#include <string>
#include "socketutil.h"

class Sender {
    
    int m_listener_port;
    int m_socket = -1;
    std::string ip_address;

public:

    bool using_ssl = true;

    Sender();
    Sender(int listener_port)
    : m_listener_port(listener_port)
    {}
    
    int Start(bool use_public_ip);
    int AcceptConnection();
    ssize_t SendData(int client, std::string data);
    void Stop();

    std::string Address();
    int Port();
};

#endif /* Sender_hpp */
