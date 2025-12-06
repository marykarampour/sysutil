//
//  ServiceInfo.cpp
//  
//
//  Created by Maryam Karampour on 2025-11-27.
//

#include "ServiceInfo.hpp"

RequestObject::RequestObject(SYS_UTIL_REQUEST_TYPE type, std::string endpoint, std::string data) : ServiceBase(type, endpoint) {
    m_data = data;
}

ServiceBase::~ServiceBase() {
}

RequestObject::~RequestObject() {
}

ServiceInfo::~ServiceInfo() {
}



