//
//  SenderReceiver.hpp
//  
//
//  Created by Maryam Karampour on 2025-11-29.
//

#ifndef SenderReceiver_hpp
#define SenderReceiver_hpp

#include <stdio.h>
#include <string>
#include <unordered_set>
#include "Sender.hpp"
#include "ServiceInfo.hpp"
#include "RequestProcessor.hpp"

class SenderReceiver {
    
    std::string m_listener_address;
    int m_listener_port;
    Sender m_sender;
    
    void StartSender(int listener_port, bool use_public_ip);
    
public:
    
    int request_receive_interval = 1;
    int response_receive_interval = 1;
    int response_send_interval = 2;
    
    int request_receive_buffer = 1024;
    int response_receive_buffer = 1024;
    
    bool using_ssl = true;
    
    /** @brief Does not creates the Sender. */
    SenderReceiver();
    /** @brief Creates the Sender. */
    SenderReceiver(int listener_port);
    /** @brief Creates the Sender. */
    SenderReceiver(std::string listener_address, int listener_port);
    ~SenderReceiver();
    
    /** @brief Called is responsible for starting the Sender. */
    void SetSender(Sender sender);
    /** @brief Starts the Sender. */
    int Start(bool use_public_ip);
    /** @brief Stops the Sender. */
    void Stop();
    
    /** @brief Makes a TCP/IP based request.
     @return Returns success or status if the request failed at any stage. */
    std::pair<SYS_UTIL_REQUEST_STATUS, std::string> SendRequest(RequestObject obj);
    /** @brief Makes a TCP/IP based request.
     @return Returns success or status if the request failed at any stage. */
    std::pair<SYS_UTIL_REQUEST_STATUS, std::string> SendRequest(RequestObject obj, int listener_socket);
    /** @brief Respondes to a TCP/IP based request.
     @param creator Subclass ResponseCreator and implement virtual methods that will be used for creating a responce.
     @return Returns success or status if the request failed at any stage. */
    std::pair<SYS_UTIL_REQUEST_STATUS, std::string> AcceptRequests(const std::unordered_set<ServiceInfo>& services, ResponseCreator& creator);
};

#endif /* SenderReceiver_hpp */
