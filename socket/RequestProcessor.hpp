//
//  RequestProcessor.hpp
//  
//
//  Created by Maryam Karampour on 2025-12-07.
//

#ifndef RequestProcessor_hpp
#define RequestProcessor_hpp

#include <stdio.h>
#include "ServiceInfo.hpp"

class ResponseCreator {
    
public:
    virtual std::string CreateGETResponse(const std::string endpoint) = 0;
    virtual std::string CreatePUTResponse(const std::string endpoint, std::string body = "") = 0;
    virtual std::string CreatePOSTResponse(const std::string endpoint, std::string body = "") = 0;
    virtual std::string CreateDELETEResponse(const std::string endpoint) = 0;

public:
    ResponseCreator();
    ~ResponseCreator();
};

class RequestProcessor {
    
public:
    RequestProcessor();
    ~RequestProcessor();
    
    std::string Response(const ServiceInfo& info, ResponseCreator& creator, const std::string body = "");
};

#endif /* RequestProcessor_hpp */
