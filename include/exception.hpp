#pragma once

#include <exception>
#include <string>

// Your custom compiler error exception
class CompilerErr : public std::exception {
private:
    std::string message;

public:
    // Constructor allows you to pass a dynamic error message
    explicit CompilerErr(const std::string& msg) : message("CompilerErr: " + msg) {}

    // Overriding the virtual what() method. 
    // 'noexcept' promises the compiler this function won't throw an exception itself.
    const char* what() const noexcept override {
        return message.c_str();
    }
};