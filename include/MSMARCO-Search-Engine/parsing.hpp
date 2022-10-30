#pragma once

#include <iostream>
#include <cmath>
#include <fstream>
#include <random>
#include <string>
#include <unordered_set>
#include <unordered_map>
#include <sstream>
#include <filesystem>
#include <boost/filesystem.hpp>

std::unordered_map<std::string, int> getTokens(const std::string content);