//
//  ServiceInfo.cpp
//  
//
//  Created by Maryam Karampour on 2025-11-27.
//

#include "ServiceInfo.hpp"
#include <sstream>

RequestObject::RequestObject(const std::string& request) {
    Parse(request);
}

RequestObject::RequestObject(SYS_UTIL_REQUEST_TYPE type, std::string endpoint, std::unordered_map<std::string, std::string> headers, std::string body) : ServiceBase(type, endpoint) {
    m_body = body;
    m_headers = headers;
}

void RequestObject::Parse(const std::string &request) {
    std::istringstream stream(request);
    std::string line;
    
    std::getline(stream, line);
    std::istringstream reqStream(line);
    std::string type;
    std::string version;
    
    reqStream >> type >> m_endpoint >> version;
    m_request_type = KeyForValue(http_request_type_map, type);
    
    while (std::getline(stream, line) && line != "\r") {
        size_t colonPos = line.find(':');
        if (colonPos != std::string::npos) {
            std::string name = line.substr(0, colonPos);
            std::string value = line.substr(colonPos + 2);
            m_headers[name] = value;
        }
    }
    
    if (m_headers.count("Content-Length")) {
        int contentLength = std::stoi(m_headers["Content-Length"]);
        
        while (std::getline(stream, line) && line != "\r" && m_body.length() <= contentLength) {
            m_body += line;
        }
    }
}

std::string RequestObject::Description() {
    std::string str;
    std::string type = http_request_type_map.at(m_request_type);
    std::string sp = " ";
    std::string cs = ": ";
    std::string nl = "\r\n";
    
    str += type + sp + m_endpoint + sp + "HTTP/1.1" + nl;
    str += MapToString(m_headers, cs, nl);
    str += m_body;
    str += nl;
    
    return str;
}

ServiceBase::ServiceBase() {
}

ServiceBase::~ServiceBase() {
}

RequestObject::~RequestObject() {
}

ServiceInfo::~ServiceInfo() {
}



