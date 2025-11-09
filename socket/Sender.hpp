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
#include <vector>
#include "senderreceiver.h"

class Sender {
    
    int m_listener_port;
    pollfd listener_file_descriptor;
    int m_socket = -1;
    std::string ip_address;
    std::vector<pollfd> poll_file_descriptors;
    
public:
    Sender(int listener_port)
    : m_listener_port(listener_port)
    {}
    
    int Start();
    int AcceptConnection();
    void AddNewConnection(int sock);
    ssize_t SendData(int client, std::string data);
    void HandleRequest();
    
    std::string Address();
    int Port();
};

#endif /* Sender_hpp */
