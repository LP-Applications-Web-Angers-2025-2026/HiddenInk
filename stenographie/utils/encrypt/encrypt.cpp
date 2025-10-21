#include "encrypt.h"
#include <random>
#include <sstream>
#include <iomanip>
using namespace std;

string generate_key(size_t n)
{
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<int> d(0, 255);
    string k;
    k.resize(n);
    for (size_t i = 0; i < n; ++i) k[i] = static_cast<char>(d(gen));
    return k;
}

string xor_encrypt(const string& data, const string& key)
{
    if (key.empty()) return data;
    string out;
    out.resize(data.size());

    for (size_t i = 0; i < data.size(); ++i)
    {
        unsigned char d = static_cast<unsigned char>(data[i]);
        unsigned char k = static_cast<unsigned char>(key[i % key.size()]);
        out[i] = static_cast<char>(d ^ k);
    }

    return out;
}

string to_hex(const string& s)
{
    ostringstream os;
    os << hex << setfill('0');
    for (unsigned char c : s) os << setw(2) << (int)c;
    return os.str();
}

string hex_to_key(const string& hex)
{
    string key;
    for (size_t i = 0; i < hex.length(); i += 2)
    {
        string byteString = hex.substr(i, 2);
        unsigned char byte = static_cast<unsigned char>(stoi(byteString, nullptr, 16));
        key += byte;
    }
    return key;
}
