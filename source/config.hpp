/*
 * MADJ project
 * Copyright (C) 2016 Siim Meerits <siim@yutani.ee>
 */

#ifndef H_CONFIG
#define H_CONFIG

// C general
#include <cassert>
#include <cstdint>

// C++ general
#include <string>
#include <memory>
#include <algorithm>
#include <exception>

// C++ IO
#include <iostream>

// C++ STL containers
#include <vector>

// Boost
#include <boost/assert.hpp>
#include <boost/format.hpp>
#include <boost/noncopyable.hpp>

// Config
#include "cmake.hpp"

// Exceptions
class c_exception : public std::exception
{
    public:
        // Constructor
        c_exception()
            { print(); }
        c_exception(std::string error) :
            m_error(error) { print(); }
        c_exception(std::string error, std::vector<std::string> param) :
            m_error(error), m_param(param) { print(); }

        // Operators
        c_exception& operator<<(std::string param) {
            m_param.push_back(param);
            print();
            return *this;
        }

        // Message
        std::string& message() noexcept {
            return m_print;
        }
        virtual const char* what() const noexcept {
            return m_print.c_str();
        }
    private:
        // Error and parameters
        std::string m_error;
        std::vector<std::string> m_param;

        // Message
        std::string m_print;
        void print();
};

// Macros
#define msg_cout std::cout
#define dbg_cout (std::cout << boost::format("[%s:%d] ") % __FILE__ % __LINE__)

// Exception
void msg_exception(c_exception& ex, std::string error = "");
void msg_excompose(c_exception& ex, std::string& msg);

// Exception macros
inline void throw_nested() {
    std::throw_with_nested(c_exception());
}
inline void throw_nested(std::string error) {
    std::throw_with_nested(c_exception(std::move(error)));
}
inline void throw_nested(std::string error, std::vector<std::string> param) {
    std::throw_with_nested(c_exception(std::move(error), std::move(param)));
}
#define throw_debug() throw c_exception("Unexpected error!", { boost::str(boost::format("source=\"%1%:%2%\"") %  __FILE__ % __LINE__ }))
#define throw_debug_ex(ex) throw (ex << boost::str(boost::format("source=\"%1%:%2%\"") %  __FILE__ % __LINE__))

// Error parameter formatting
template <typename t_type>
inline std::string throw_format(std::string key, t_type value) {
    return boost::str(boost::format("%1%=%2%") % key % value);
}
template <>
inline std::string throw_format <std::string> (std::string key, std::string value) {
    return key + "=\"" + value + "\"";
}
template <>
inline std::string throw_format <bool> (std::string key, bool value) {
    return key + "=" + (value ? "true" : "false");
}

#endif

