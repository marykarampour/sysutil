//
//  Sender.cpp
//  
//
//  Created by Maryam Karampour on 2025-11-09.
//

#include "Sender.hpp"
#include <iostream>
#include <stdlib.h>
#include <string.h>
#include <poll.h>

int Sender::Start(bool usePublicIP) {
    
    connect_addr_info *listener = create_listener_info(m_listener_port, usePublicIP);
    
    if (listener == NULL || listener->socket == -1) {
        return -1;
    }
    
    listen(listener->socket, 10);
    
    listener_file_descriptor = {listener->socket, POLLIN};
    m_socket = listener->socket;
    if (listener->public_ip)
        ip_address.assign(listener->public_ip);
    
    std::cout << "Sender " << ip_address << " is ready to receive requests ..." << std::endl;
    return 0;
}

int Sender::AcceptConnection() {
    return create_client_socket(m_socket);
}

std::string Sender::Address() {
    return std::string(ip_address);
}

int Sender::Port() {
    return m_listener_port;
}

void Sender::AddNewConnection(int sock) {
    pollfd po{sock, POLLIN, 0};
    poll_file_descriptors.push_back(po);
}

ssize_t Sender::SendData(int client, std::string data) {
    return send_data(client, data.c_str(), data.length());
}
