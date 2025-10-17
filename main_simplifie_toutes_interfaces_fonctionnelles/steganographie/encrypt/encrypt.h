#pragma once
#include <string>

std::string xor_encrypt(const std::string &data, const std::string &key);
std::string generate_key(size_t length);
std::string to_hex(const std::string &s);
std::string hex_to_key(const std::string &hex);
