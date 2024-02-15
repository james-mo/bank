#include <fstream>
#include "util.hpp"

bool fileExists(const std::string& filePath) {
    std::ifstream file(filePath);
    return file.good();
}