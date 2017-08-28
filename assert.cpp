#include "assert.hpp"
#include <iostream>

bool Assert::operator()(const bool condition,
                        const char *const condition_text,
                        const char *const file_name,
                        const int line_number) {
    if (!condition) {
        m_failed_assertions++;
        std::cerr << "'" << condition_text << "' is not true in file "
        << "'" << file_name << "' at line " << line_number << "."
        << std::endl;
    }
    
    m_total_assertions++;
    return condition;
}

size_t Assert::get_number_of_failed_assertions() const {
    return m_failed_assertions;
}

size_t Assert::get_total_number_of_assertions() const {
    return m_total_assertions;
}

void Assert::fail(const char *msg) {
    std::cerr << "FAILURE: " << msg << '\n';
    m_failed_assertions++;
}

void Assert::report() const {
    std::cout << "[TOTAL ASSERTIONS: "
    << m_total_assertions
    << ", FAILED ASSERTIONS: "
    << m_failed_assertions
    << ", PASS RATIO: ";
    
    if (m_total_assertions == 0)
    {
        std::cout << "N/A";
    }
    else
    {
        std::cout << ((float)
                      (m_total_assertions - m_failed_assertions)) / m_total_assertions;
    }
    
    std::cout << "]";
    
    if (m_failed_assertions == 0) {
        std::cout << " Test success!\n";
    } else {
        std::cout << " Some tests failed.\n";
    }
}

Assert assert;
