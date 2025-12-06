//
//  ServiceInfo.hpp
//  
//
//  Created by Maryam Karampour on 2025-11-27.
//

#ifndef ServiceInfo_h
#define ServiceInfo_h

#include <stdio.h>
#include <string>

enum class SYS_UTIL_REQUEST_TYPE {
    GET,
    POST
};

enum class SYS_UTIL_REQUEST_STATUS {
    UNKNOWN,
    SEND_FAILED,
    ACCEPT_FAILED,
    SUCCESS
};

class ServiceBase {
    
public:
    SYS_UTIL_REQUEST_TYPE m_request_type;
    std::string m_endpoint;
    
    ServiceBase(SYS_UTIL_REQUEST_TYPE type, std::string endpoint) : m_request_type(type), m_endpoint(endpoint)
    {}
    ~ServiceBase();
    
    bool operator==(const ServiceBase& x) const {
        return m_request_type == x.m_request_type && m_endpoint == x.m_endpoint;
    }
};

class RequestObject : public ServiceBase {
    
public:
    std::string m_data;
    
    using ServiceBase::ServiceBase;
    RequestObject(SYS_UTIL_REQUEST_TYPE type, std::string endpoint, std::string data);
    ~RequestObject();
};

class ServiceInfo : public ServiceBase {
    //TODO: params
    
public:
    
    using ServiceBase::ServiceBase;
    ~ServiceInfo();
    
    bool operator==(const ServiceInfo& x) const {
        return m_request_type == x.m_request_type && m_endpoint == x.m_endpoint;
    }
};

template<>
struct std::hash<ServiceInfo> {
    size_t operator()(const ServiceInfo& obj) const {
        size_t type = std::hash<size_t>()(static_cast<std::size_t>(obj.m_request_type));
        size_t len = std::hash<std::string>()(obj.m_endpoint);
        return type ^ (len << 1);
    }
};

#endif /* ServiceInfo_h */
