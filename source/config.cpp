/*
 * MADJ project
 * Copyright (C) 2015 Siim Meerits <siim@yutani.ee>
 */

// Internal
#include "config.hpp"

// Boost
#include <boost/algorithm/string.hpp>

// Exception
void c_exception::print()
{
    m_print = m_error;
    if (!m_param.empty())
        m_print += " (" + boost::algorithm::join(m_param, ", ") + ")";
}

// Exception
void msg_exception(c_exception& ex, std::string error)
{
    msg_excompose(ex, error);
    std::cout << "[e] " << error << std::endl;
}

void msg_excompose(c_exception& ex, std::string& msg)
{
    // Message
    std::string& print = ex.message();
    if (!msg.empty() && !print.empty())
        msg += " ";
    msg += print;

    // Nested error
    try {
        std::rethrow_if_nested(ex);
    } catch (c_exception& ex_new) {
        msg_excompose(ex_new, msg);
    } catch (std::exception& ex_new) {
        std::string print_new = ex_new.what();
        if (print_new.empty())
            print_new = "Unknown exception!";
        if (!msg.empty())
            msg += " ";
        msg += print_new;
    }
}
