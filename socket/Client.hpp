//
//  Client.hpp
//  
//
//  Created by Maryam Karampour on 2025-11-29.
//

#ifndef Client_hpp
#define Client_hpp

#include <stdio.h>
#include "SenderReceiver.hpp"

class Client {
    
    SenderReceiver m_receiver;
    
public:
    Client();
    ~Client();
};

#endif /* Client_hpp */
