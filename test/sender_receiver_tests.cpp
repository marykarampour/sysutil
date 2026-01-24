//
// Created by aviar on 11/13/25.
//

#include <thread>
#include <iostream>
#include <atomic>
#include <catch2/catch_test_macros.hpp>
#include <unistd.h>
#include "Sender.hpp"
#include "Server.hpp"
#include "SenderReceiver.hpp"
#include "RequestProcessor.hpp"

class ResponseCreator_Test : public ResponseCreator {
    
    std::string CreateGETResponse(const std::string endpoint) override;
    std::string CreatePUTResponse(const std::string endpoint, std::string body = "") override;
    std::string CreatePOSTResponse(const std::string endpoint, std::string body = "") override;
    std::string CreateDELETEResponse(const std::string endpoint) override;
    
public:
    using ResponseCreator::ResponseCreator;
};

std::string ResponseCreator_Test::CreateGETResponse(const std::string endpoint) {
    return "GET response";
}

std::string ResponseCreator_Test::CreatePUTResponse(const std::string endpoint, std::string body) {
    return "PUT response";
}

std::string ResponseCreator_Test::CreatePOSTResponse(const std::string endpoint, std::string body) {
    return "POST response";
}

std::string ResponseCreator_Test::CreateDELETEResponse(const std::string endpoint) {
    return "DELETE response";
}

TEST_CASE("Serialization GET", "[Serialization]") {
    
    std::string endpoint = "endpoint";
    std::unordered_map<std::string, std::string> headers = {
        {"Host","0.0.0.0"},
        {"Content-Type","application/json"},
    };
    
    RequestObject obj1 = RequestObject(SYS_UTIL_REQUEST_TYPE::GET, endpoint, headers);
    std::string str = obj1.Description();
    RequestObject obj2 = RequestObject(str);
    REQUIRE(obj1 == obj2);
}

TEST_CASE("Serialization POST", "[Serialization]") {
    
    std::unordered_map<std::string, std::string> body = {
        {"username","johndoe"},
        {"password","securepassword"}
    };
    
    std::string content = MapToJSONString(body);
    std::string endpoint = "endpoint";
    std::unordered_map<std::string, std::string> headers = {
        {"Host","0.0.0.0"},
        {"Content-Type","application/json"},
        {"Content-Length",std::to_string(content.length())}
    };
    
    RequestObject obj1 = RequestObject(SYS_UTIL_REQUEST_TYPE::POST, endpoint, headers, content);
    std::string str = obj1.Description();
    RequestObject obj2 = RequestObject(str);
    REQUIRE(obj1 == obj2);
}

TEST_CASE("Process Response", "[Serialization]") {
    
    std::unordered_map<std::string, std::string> body = {
        {"username","johndoe"},
        {"password","securepassword"}
    };
    std::string content = MapToJSONString(body);
    const ServiceInfo info = ServiceInfo(SYS_UTIL_REQUEST_TYPE::GET, "api/v1/post");
    ResponseCreator_Test creator = ResponseCreator_Test();
    RequestProcessor processor = RequestProcessor();
    std::string response = processor.Response(info, creator, content);
    
    REQUIRE(response.find(http_request_type_map.at(info.m_request_type)) != std::string::npos);
}

TEST_CASE("Sender Start", "[start]") {

    Sender sender(10101);
    int sock = sender.Start(false);
    REQUIRE(0 <= sock);
}

TEST_CASE("Sender Send Data", "[send]") {

    std::thread sender_thread([&] {
        Sender sender(10101);
        sender.Start(false);
        int sock = sender.AcceptConnection();
        if (0 <= sock) {
            std::string str("Hello Client!");
            std::cout << "Sending data to client -> " << str << std::endl;
            sender.SendData(sock, str);
            close(sock);
        }
    });

    std::thread receiver_thread([&] {
        unsigned char *bytes = receive_data_from("::1", 10101, 1024, false);
        std::string data = std::string(reinterpret_cast<char *>(bytes));
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
        std::cout << "Data from sender -> " << data << std::endl;
        REQUIRE(data.empty() == false);
    });

    sender_thread.join();
    receiver_thread.join();
}

TEST_CASE("Sender Receiver", "[Send]") {
    
    std::atomic<int> port = 11234;
    std::unordered_map<std::string, std::string> body = {
        {"username","johndoe"},
        {"password","securepassword"}
    };
    std::string content = MapToJSONString(body);
    const ServiceInfo info = ServiceInfo(SYS_UTIL_REQUEST_TYPE::POST, "api/v1/post");
    std::unordered_set<ServiceInfo> services = {info};
    std::unordered_map<std::string, std::string> headers = {
        {"Host","::1"},
        {"Content-Type","application/json"},
        {"Content-Length",std::to_string(content.length())}
    };
    
    std::jthread sender_thread([&] {
        SenderReceiver sender(port.load());
        ResponseCreator_Test creator = ResponseCreator_Test();
        
        std::pair<SYS_UTIL_REQUEST_STATUS, std::string> res = sender.AcceptRequests(services, creator);
        if (res.first == SYS_UTIL_REQUEST_STATUS::SUCCESS) {
            std::cout << "Data sent to client -> " << res.second << std::endl;
        }
    });
    
    std::jthread receiver_thread([&] {
        
        SenderReceiver client("::1", port.load());
        RequestObject obj = RequestObject(info.m_request_type, info.m_endpoint, headers, MapToJSONString(body));
        std::pair<SYS_UTIL_REQUEST_STATUS, std::string> res = client.SendRequest(obj);
        
        if (res.first == SYS_UTIL_REQUEST_STATUS::SUCCESS) {
            std::cout << http_request_type_map.at(info.m_request_type) << " data from sender -> " << res.second << std::endl;
            REQUIRE(res.second.empty() == false);
        }
    });
}

TEST_CASE("Server", "[pool]") {
    
    std::unordered_map<std::string, std::string> body = {
        {"username","johndoe"},
        {"password","securepassword"}
    };
    std::string content = MapToJSONString(body);
    std::vector<ServiceInfo> vect = {
        ServiceInfo(SYS_UTIL_REQUEST_TYPE::GET, "api/v1/get"),
        ServiceInfo(SYS_UTIL_REQUEST_TYPE::POST, "api/v1/post")};
    std::unordered_set<ServiceInfo> services = {vect.begin(), vect.end()};
    
    std::unordered_map<std::string, std::string> headers = {
        {"Host","0.0.0.0"},
        {"Content-Type","application/json"},
        {"Content-Length",std::to_string(content.length())}
    };
    
    std::atomic<int> port = 11234;
    std::atomic<int> success_count = 0;
    int client_count = 10;
    
    ResponseCreator_Test creator = ResponseCreator_Test();
    Server server(port.load(), false, services, creator);
    server.Start(4);
    
    //TODO: need to create a Client that can send request data
    for (size_t i=0; i<client_count; i++) {
        
        SenderReceiver client("::1", port.load());
        int index = i%2;
        const ServiceInfo info = vect[index];
        RequestObject obj = RequestObject(info.m_request_type, info.m_endpoint, headers, content);
        std::pair<SYS_UTIL_REQUEST_STATUS, std::string> res = client.SendRequest(obj);
        
        if (res.first == SYS_UTIL_REQUEST_STATUS::SUCCESS) {
            std::cout << "Data from sender -> " << res.second << " to client index " << i << std::endl;
            if (res.second.empty() == false) success_count.store(success_count.load()+1);
        }
    }
    
    std::cout << "Server done!" << std::endl;
    server.Stop();
    //This is not called. REQUIRE is not thread safe.
    REQUIRE(client_count == success_count);
}
