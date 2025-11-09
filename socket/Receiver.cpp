//
//  Receiver.cpp
//  
//
//  Created by Maryam Karampour on 2025-11-09.
//

#include "Receiver.hpp"
#include <iostream>
#include <stdlib.h>
#include <string.h>

std::string Receiver::GetData() {
    
    int sock = get_listener_socket(m_listener_address.c_str(), m_listener_port);
    if (sock == -1) return std::string();
    pollfd po{sock, POLLIN, 0};
    return std::string(receive_data(po, m_buffer_size));
}
