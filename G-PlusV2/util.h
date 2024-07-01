#pragma once
#include <iostream>
#include "Windows.h"

namespace util {
    void clear_console();
    void CoutCentered(std::string text);
    int setenv(const char* name, const char* value, int overwrite);
}