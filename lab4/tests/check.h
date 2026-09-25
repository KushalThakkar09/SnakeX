#pragma once

#include <iostream>
#include <stdexcept>
#include <string>

inline void check(bool condition, const char* expression, const char* file, int line) {
    if (!condition) {
        throw std::runtime_error(std::string(file) + ":" + std::to_string(line) +
                                 ": " + expression);
    }
}
#define CHECK(expression) check((expression), #expression, __FILE__, __LINE__)

inline void runTest(const char* name, void (*test)(), int& failures) {
    try {
        test();
        std::cout << "PASS " << name << '\n';
    } catch (const std::exception& error) {
        ++failures;
        std::cerr << "FAIL " << name << ": " << error.what() << '\n';
    }
}
