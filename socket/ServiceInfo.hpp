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
#include <unordered_map>
#include "CPPUtility.hpp"

enum class SYS_UTIL_REQUEST_TYPE : int {
    GET,
    PUT,
    POST,
    DELETE,
    COUNT
};

inline SYS_UTIL_REQUEST_TYPE& operator++(SYS_UTIL_REQUEST_TYPE& t) {
    int n = static_cast<int>(t); ++n;
    t = static_cast<SYS_UTIL_REQUEST_TYPE>(n);
    if (t == SYS_UTIL_REQUEST_TYPE::COUNT) t = SYS_UTIL_REQUEST_TYPE::GET;
    return t;
}

inline SYS_UTIL_REQUEST_TYPE operator++(SYS_UTIL_REQUEST_TYPE& t, int) {
    SYS_UTIL_REQUEST_TYPE type = t; ++t;
    return type;
}

enum class SYS_UTIL_REQUEST_STATUS {
    UNKNOWN,
    SEND_FAILED,
    ACCEPT_FAILED,
    SUCCESS
};

typedef std::string SYS_UTIL_REQUEST_TYPE_NAME;

static SYS_UTIL_REQUEST_TYPE_NAME const SYS_UTIL_REQUEST_TYPE_NAME_GET    = "GET";
static SYS_UTIL_REQUEST_TYPE_NAME const SYS_UTIL_REQUEST_TYPE_NAME_PUT    = "PUT";
static SYS_UTIL_REQUEST_TYPE_NAME const SYS_UTIL_REQUEST_TYPE_NAME_POST   = "POST";
static SYS_UTIL_REQUEST_TYPE_NAME const SYS_UTIL_REQUEST_TYPE_NAME_DELETE = "DELETE";

static const std::unordered_map<SYS_UTIL_REQUEST_TYPE, SYS_UTIL_REQUEST_TYPE_NAME> http_request_type_map = {
    {SYS_UTIL_REQUEST_TYPE::GET    , SYS_UTIL_REQUEST_TYPE_NAME_GET   },
    {SYS_UTIL_REQUEST_TYPE::PUT    , SYS_UTIL_REQUEST_TYPE_NAME_PUT   },
    {SYS_UTIL_REQUEST_TYPE::POST   , SYS_UTIL_REQUEST_TYPE_NAME_POST  },
    {SYS_UTIL_REQUEST_TYPE::DELETE , SYS_UTIL_REQUEST_TYPE_NAME_DELETE}
};

class ServiceBase {
    
public:
    SYS_UTIL_REQUEST_TYPE m_request_type;
    std::string m_endpoint;
    
    ServiceBase();
    ServiceBase(SYS_UTIL_REQUEST_TYPE type, std::string endpoint) : m_request_type(type), m_endpoint(endpoint)
    {}
    ~ServiceBase();
    
    bool operator==(const ServiceBase& x) const {
        return m_request_type == x.m_request_type && m_endpoint == x.m_endpoint;
    }
};

//TODO: Add support for custom protocol - user implements parse and toString
class RequestObject : public ServiceBase {
    
    void Parse(const std::string& request);
    
public:
    std::string m_body;
    std::unordered_map<std::string, std::string> m_headers;
    
    using ServiceBase::ServiceBase;
    RequestObject(const std::string& request);
    RequestObject(SYS_UTIL_REQUEST_TYPE type, std::string endpoint, std::unordered_map<std::string, std::string> headers, std::string body = "");
    ~RequestObject();

    std::string Description();
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

static_assert(SUObject<RequestObject>);

#endif /* ServiceInfo_h */
