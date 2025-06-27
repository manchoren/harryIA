#include "autoexec.h"
#include <iostream>
#include <thread>
#include <chrono>

void delayPrint(const std::string &text, int delay_ms) {
    for (char c : text) {
        std::cout << c;
        std::flush(std::cout);
        std::this_thread::sleep_for(std::chrono::milliseconds(delay_ms));
    }
    std::cout << std::endl;
}

