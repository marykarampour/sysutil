//
//  CPPUtility.hpp
//  
//
//  Created by Maryam Karampour on 2025-12-06.
//

#ifndef CPPUtility_hpp
#define CPPUtility_hpp

#include <stdio.h>
#include <unordered_map>
#include <concepts>
#include <utility>
#include <string>

#pragma mark - concepts

template <typename T>
concept SUObject =
requires(T o) { { o.Description() } -> std::same_as<std::string>;} ||
requires(T o) { { std::string(o)  } -> std::convertible_to<std::string>;};

#pragma mark - map

template <typename T, typename U>
std::pair<T, U> PairForValue(std::unordered_map<T, U> m, U value) {
    for (const std::pair<const T, U>& obj : m) {
        if (obj.second == value) return obj;
    }
    return std::pair<T, U>();
}

template <typename T, typename U>
T KeyForValue(std::unordered_map<T, U> m, U value) {
    return PairForValue(m, value).first;
}

inline std::string MapToString(std::unordered_map<std::string, std::string> m, std::string delimiter, std::string endLine) {
    std::string str;
    for (const std::pair<std::string, std::string> obj : m) {
        str += obj.first + delimiter + obj.second + endLine;
    }
    return str;
}

inline std::string MapToJSONString(std::unordered_map<std::string, std::string> m) {
    return "\\{" + MapToString(m, ": ", "\r\n") + "\\}";
}

template <typename T, typename U>
requires SUObject<T> && SUObject<U>
inline std::string MapToString(std::unordered_map<T, U> m, std::string delimiter, std::string endLine) {
    std::string str;
    for (const std::pair<T, U> obj : m) {
        str += obj.first.Description() + delimiter + obj.second.Description() + endLine;
    }
    return str;
}

template <typename T>
requires SUObject<T>
inline std::string MapToJSONString(std::unordered_map<T, std::string> m) {
    return "\\{" + MapToString(m, ": ", "\r\n") + "\\}";
}

template <typename T>
requires SUObject<T>
inline std::string MapToString(std::unordered_map<T, std::string> m, std::string delimiter, std::string endLine) {
    std::string str;
    for (const std::pair<T, std::string> obj : m) {
        str += obj.first.Description() + delimiter + obj.second + endLine;
    }
    return str;
}

template <typename U>
requires SUObject<U>
inline std::string MapToJSONString(std::unordered_map<std::string, U> m) {
    return "\\{" + MapToString(m, ": ", "\r\n") + "\\}";
}

template <typename U>
requires SUObject<U>
inline std::string MapToString(std::unordered_map<std::string, U> m, std::string delimiter, std::string endLine) {
    std::string str;
    for (const std::pair<std::string, U> obj : m) {
        str += obj.first + delimiter + obj.second.Description() + endLine;
    }
    return str;
}

template <typename T, typename U>
requires SUObject<T> && SUObject<U>
inline std::string MapToJSONString(std::unordered_map<T, U> m) {
    return "\\{" + MapToString(m, ": ", "\r\n") + "\\}";
}



#endif /* CPPUtility_hpp */
