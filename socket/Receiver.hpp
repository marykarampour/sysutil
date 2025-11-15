//
//  Receiver.hpp
//  
//
//  Created by Maryam Karampour on 2025-11-09.
//

#ifndef Receiver_hpp
#define Receiver_hpp

#include <stdio.h>
#include <string>
#include <vector>
#include "senderreceiver.h"

class Receiver {
    
    int m_listener_port;
    std::string m_listener_address;
    
public:
    Receiver(std::string listener_address, int listener_port)
    :  m_listener_address(listener_address), m_listener_port(listener_port)
    {}
    
    std::string GetData(int buffer_size);
};

#endif /* Receiver_hpp */
