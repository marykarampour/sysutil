//
//  RequestProcessor.cpp
//  
//
//  Created by Maryam Karampour on 2025-12-07.
//

#include "RequestProcessor.hpp"

ResponseCreator::ResponseCreator() {
}

ResponseCreator::~ResponseCreator() {
}

RequestProcessor::RequestProcessor() {
}

RequestProcessor::~RequestProcessor() {
}

std::string RequestProcessor::Response(const ServiceInfo& info, ResponseCreator& creator, const std::string body) {
    switch (info.m_request_type) {
        case SYS_UTIL_REQUEST_TYPE::GET:
            return creator.CreateGETResponse(info.m_endpoint);

        case SYS_UTIL_REQUEST_TYPE::PUT:
            return creator.CreatePUTResponse(info.m_endpoint, body);

        case SYS_UTIL_REQUEST_TYPE::POST:
            return creator.CreatePOSTResponse(info.m_endpoint, body);

        case SYS_UTIL_REQUEST_TYPE::DELETE:
            return creator.CreateDELETEResponse(info.m_endpoint);

        default:
            return "";
    }
}
