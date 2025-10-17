#include "encrypt.h"
#include <random>
#include <sstream>
#include <iomanip>
using namespace std;

std::string generate_key(size_t n) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> d(0, 255);
    std::string k; k.resize(n);
    for (size_t i = 0; i < n; ++i) k[i] = static_cast<char>(d(gen));
    return k;
}

std::string xor_encrypt(const std::string &data, const std::string &key) {
    if (key.empty()) return data;
    std::string out; out.resize(data.size());
    for (size_t i = 0; i < data.size(); ++i)
        out[i] = data[i] ^ key[i % key.size()];
    return out;
}

std::string to_hex(const std::string &s) {
    std::ostringstream os;
    os << std::hex << std::setfill('0');
    for (unsigned char c : s) os << std::setw(2) << (int)c;
    return os.str();
}

std::string hex_to_key(const std::string &hex) {
    std::string key;
    for (size_t i = 0; i < hex.length(); i += 2) {
        std::string byteString = hex.substr(i, 2);
        unsigned char byte = static_cast<unsigned char>(std::stoi(byteString, nullptr, 16));
        key += byte;
    }
    return key;
}
