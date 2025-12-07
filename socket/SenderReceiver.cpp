//
//  SenderReceiver.cpp
//  
//
//  Created by Maryam Karampour on 2025-11-29.
//

#include "SenderReceiver.hpp"
#include <thread>
#include <chrono>

SenderReceiver::SenderReceiver() {
}

SenderReceiver::SenderReceiver(int listener_port) {
    m_listener_port = listener_port;
    StartSender(listener_port, false);
}

SenderReceiver::SenderReceiver(std::string listener_address, int listener_port) {
    m_listener_address = listener_address;
    m_listener_port = listener_port;
    StartSender(listener_port, true);
}

SenderReceiver::~SenderReceiver() {
}

void SenderReceiver::SetSender(Sender sender) {
    m_sender = sender;
    m_listener_address = m_sender.Address();
    m_listener_port = m_sender.Port();
}

void SenderReceiver::StartSender(int listener_port, bool use_public_ip) {
    m_sender = Sender(listener_port);
    Start(use_public_ip);
}

int SenderReceiver::Start(bool use_public_ip) {
    return m_sender.Start(use_public_ip);
}

void SenderReceiver::Stop() {
   m_sender.Stop();
}

std::pair<SYS_UTIL_REQUEST_STATUS, std::string> SenderReceiver::Request(RequestObject obj) {
    int sock = get_listener_socket(m_listener_address.c_str(), m_listener_port);
    return Request(obj, sock);
}

std::pair<SYS_UTIL_REQUEST_STATUS, std::string> SenderReceiver::Request(RequestObject obj, int listener_socket) {
    if (listener_socket < 0)
        return std::pair(SYS_UTIL_REQUEST_STATUS::SEND_FAILED, "client");
    
    std::string str = obj.Description();
    m_sender.SendData(listener_socket, str);
    
    std::string data;
    while (0 == data.length()) {
        data = std::string(receive_data(listener_socket, request_receive_buffer));
        std::this_thread::sleep_for(std::chrono::seconds(request_receive_interval));
    }
    
    close(listener_socket);
    
    return std::pair(SYS_UTIL_REQUEST_STATUS::SUCCESS, data);
}

std::pair<SYS_UTIL_REQUEST_STATUS, std::string> SenderReceiver::Response(std::unordered_set<ServiceInfo> services) {
    
    int accept_sock = m_sender.AcceptConnection();
    if (accept_sock < 0)
        return std::pair(SYS_UTIL_REQUEST_STATUS::ACCEPT_FAILED, "server");
    std::string data;
    while (0 == data.length()) {
        data = std::string(receive_data(accept_sock, response_receive_buffer));
        std::this_thread::sleep_for(std::chrono::seconds(response_receive_interval));
    }
    
    //TODO: parsing request
    RequestObject obj = RequestObject(data);

    //TODO: handling response buffer
    
    ssize_t size = 0;
    while (size == 0) {
        size = m_sender.SendData(accept_sock, data);
        std::this_thread::sleep_for(std::chrono::seconds(response_send_interval));
    }
    
    close(accept_sock);
    
    return std::pair(SYS_UTIL_REQUEST_STATUS::SUCCESS, data);
}


