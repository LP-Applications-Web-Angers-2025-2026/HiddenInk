#pragma once
#include "stb_image.h"
#include "stb_image_write.h"
#include <iostream>
#include <vector>
#include <string>
#include <cstdint>
#include <algorithm>

using namespace std;

// Charger une image (retourne pointeur et remplit largeur, hauteur, canaux)
inline unsigned char* loadImage(const string& path, int& w, int& h, int& c) {
    unsigned char* img = stbi_load(path.c_str(), &w, &h, &c, 0);
    if (!img) cerr << "Erreur : impossible de charger " << path << "\n";
    return img;
}

// Sauvegarder une image PNG
inline bool saveImage(const string& path, unsigned char* data, int w, int h, int c) {
    return stbi_write_png(path.c_str(), w, h, c, data, w * c);
}

// Cacher une image dans une autre (choix automatique des bits)
inline vector<unsigned char> hideImageInImage(unsigned char* carrier, int cw, int ch, int cc,
                                              unsigned char* secret, int sw, int sh, int sc,
                                              int bitsPerChannel = 0) {
    // Taille totale (en bits) du message à cacher
    size_t secretBits = (sw * sh * sc * 8) + 96; // 96 bits pour le header

    // Si bitsPerChannel == 0, on choisit automatiquement le plus petit possible
    if (bitsPerChannel == 0) {
        for (int b = 1; b <= 8; ++b) {
            size_t capacity = cw * ch * cc * b;
            if (secretBits <= capacity) {
                bitsPerChannel = b;
                break;
            }
        }
        if (bitsPerChannel == 0) bitsPerChannel = 8; // par sécurité
    }

    cout << "ℹ️  Utilisation de " << bitsPerChannel
         << " bit(s) par canal (choisi automatiquement).\n";

    size_t carrierBits = cw * ch * cc * bitsPerChannel;
    if (secretBits > carrierBits) {
        cerr << "❌ Erreur : l'image porteuse est trop petite même avec 8 bits.\n";
        return {};
    }

    // Créer une copie pour ne pas altérer l'image d'origine
    vector<unsigned char> encoded(carrier, carrier + (cw * ch * cc));

    // Construire le header (3x32 bits : largeur, hauteur, canaux)
    vector<uint32_t> header = { (uint32_t)sw, (uint32_t)sh, (uint32_t)sc };
    vector<int> bits;
    for (auto v : header)
        for (int i = 31; i >= 0; --i)
            bits.push_back((v >> i) & 1);

    // Ajouter les bits de l’image secrète
    for (int i = 0; i < sw * sh * sc; ++i)
        for (int b = 7; b >= 0; --b)
            bits.push_back((secret[i] >> b) & 1);

    // Injection des bits dans l'image porteuse
    size_t bitIndex = 0;
    for (int i = 0; i < cw * ch * cc && bitIndex < bits.size(); ++i) {
        for (int b = 0; b < bitsPerChannel && bitIndex < bits.size(); ++b) {
            encoded[i] &= ~(1 << b);
            encoded[i] |= bits[bitIndex++] << b;
        }
    }

    return encoded;
}

// Extraire une image cachée
inline vector<unsigned char> extractImageFromImage(unsigned char* carrier, int cw, int ch, int cc,
                                                   int bitsPerChannel, int& outW, int& outH, int& outC) {
    bitsPerChannel = max(1, min(bitsPerChannel, 8));
    vector<int> bits;

    for (int i = 0; i < cw * ch * cc; ++i)
        for (int b = 0; b < bitsPerChannel; ++b)
            bits.push_back((carrier[i] >> b) & 1);

    if (bits.size() < 96) {
        cerr << "Erreur : données insuffisantes !\n";
        return {};
    }

    // Lire largeur, hauteur, canaux
    outW = 0; outH = 0; outC = 0;
    for (int i = 0; i < 32; ++i) outW = (outW << 1) | bits[i];
    for (int i = 32; i < 64; ++i) outH = (outH << 1) | bits[i];
    for (int i = 64; i < 96; ++i) outC = (outC << 1) | bits[i];

    size_t totalBits = (size_t)outW * outH * outC * 8;
    if (96 + totalBits > bits.size()) {
        cerr << "Erreur : données incomplètes !\n";
        return {};
    }

    vector<unsigned char> secret(outW * outH * outC);
    size_t bitIndex = 96;
    for (size_t i = 0; i < secret.size(); ++i) {
        unsigned char val = 0;
        for (int b = 0; b < 8; ++b)
            val = (val << 1) | bits[bitIndex++];
        secret[i] = val;
    }

    return secret;
}
