#include <iostream>
#include <string>
#include <cstdlib>
#include "utils.h" 
#include <string>
#include <windows.h>
#include <vector>
std::string normalizeText(const std::string& text);
void speak_async(const Config& config, const std::string& text);
void speak(const Config& config, const std::string& text);
