#pragma once
#include <iostream>
#include <fstream>
#include <vector>
#include <bitset>
#include <string>

using namespace std;

// Lecture d'un fichier en bytes
vector<unsigned char> lireFichier(const string& nom) {
    ifstream fichier(nom, ios::binary);
    if (!fichier) return {};
    return vector<unsigned char>((istreambuf_iterator<char>(fichier)), {});
}

// Écriture d'un fichier texte
void ecrireFichier(const string& nom, const string& contenu) {
    ofstream fichier(nom, ios::binary);
    fichier << contenu;
}

// Écriture en bytes (binaire)
void writeFileBytes(const string& nom, const vector<unsigned char>& data) {
    ofstream fichier(nom, ios::binary);
    fichier.write(reinterpret_cast<const char*>(data.data()), data.size());
}


void embedLSB(vector<unsigned char>& target, const string& message, int bitsPerByte = 8) {
    vector<bool> bits;
    for (unsigned char c : message) {
        bitset<8> b(c);
        for (int i = 7; i >= 0; --i)
            bits.push_back(b[i]);
    }

    // Taille nécessaire en bytes
    size_t neededBytes = (bits.size() + bitsPerByte - 1) / bitsPerByte;
    if (target.size() < neededBytes)
        target.resize(neededBytes, 0);

    size_t bitIndex = 0;
    for (auto& byte : target) {
        for (int i = 0; i < bitsPerByte && bitIndex < bits.size(); ++i) {
            byte &= ~(1 << i);
            byte |= bits[bitIndex++] << i;
        }
        if (bitIndex >= bits.size()) break;
    }
}


string extractLSB(const vector<unsigned char>& source, int bitsPerByte = 8) {
    vector<bool> bits;
    for (unsigned char byte : source) {
        for (int i = 0; i < bitsPerByte; ++i)
            bits.push_back((byte >> i) & 1);
    }

    string message;
    for (size_t i = 0; i + 7 < bits.size(); i += 8) {
        unsigned char c = 0;
        for (int j = 0; j < 8; ++j)
            c = (c << 1) | bits[i + j];
        message.push_back(c);
    }


    size_t end = message.find_last_not_of('\0');
    if (end != string::npos)
        message = message.substr(0, end + 1);

    return message;
}
