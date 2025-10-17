#pragma once
#include "../external/stb_image.h"
#include "../external/stb_image_write.h"
#include "../utils_bin.h"
#include <iostream>
#include <vector>
#include <string>
#include <cstdint>
#include <algorithm>
#include <bitset>
#include <cmath>

using namespace std;

// ============ FONCTIONS IMAGE ============

// Charger une image (retourne pointeur et remplit largeur, hauteur, canaux)
inline unsigned char* loadImage(const string& path, int& w, int& h, int& c) {
    unsigned char* img = stbi_load(path.c_str(), &w, &h, &c, 0);
    if (!img) cerr << "Erreur : impossible de charger " << path << "\n";
    return img;
}

// Sauvegarder une image PNG
inline bool saveImage(const string& path, const unsigned char* data, int w, int h, int c) {
    return stbi_write_png(path.c_str(), w, h, c, data, w * c);
}

// Redimensionner une image (interpolation bilinéaire pour meilleure qualité)
inline vector<unsigned char> resizeImage(unsigned char* src, int sw, int sh, int sc, int newW, int newH) {
    vector<unsigned char> resized(newW * newH * sc);
    
    float xRatio = (float)(sw - 1) / newW;
    float yRatio = (float)(sh - 1) / newH;
    
    for (int y = 0; y < newH; ++y) {
        for (int x = 0; x < newW; ++x) {
            float srcX = x * xRatio;
            float srcY = y * yRatio;
            
            int x1 = (int)srcX;
            int y1 = (int)srcY;
            int x2 = min(x1 + 1, sw - 1);
            int y2 = min(y1 + 1, sh - 1);
            
            float xWeight = srcX - x1;
            float yWeight = srcY - y1;
            
            for (int c = 0; c < sc; ++c) {
                // Interpolation bilinéaire
                float p1 = src[(y1 * sw + x1) * sc + c] * (1 - xWeight) + src[(y1 * sw + x2) * sc + c] * xWeight;
                float p2 = src[(y2 * sw + x1) * sc + c] * (1 - xWeight) + src[(y2 * sw + x2) * sc + c] * xWeight;
                float value = p1 * (1 - yWeight) + p2 * yWeight;
                
                resized[(y * newW + x) * sc + c] = (unsigned char)(value + 0.5f);
            }
        }
    }
    
    return resized;
}

// Calculer les dimensions optimales pour faire rentrer l'image secrète
inline bool calculateOptimalSize(int cw, int ch, int cc, int& sw, int& sh, int sc, 
                                  unsigned char*& secret, vector<unsigned char>& resizedBuffer,
                                  int maxBitsPerChannel = 2) {
    // Calculer la taille du message avec signature et balises
    size_t signatureBits = getSignatureBinarySize();
    size_t balisesBits = getBaliseBinarySize() * 2;
    size_t headerBits = 96; // 3 x 32 bits
    
    // Taille actuelle de l'image secrète en bits
    size_t currentSizeBits = (size_t)sw * sh * sc * 8;
    size_t totalNeededBits = currentSizeBits + signatureBits + balisesBits + headerBits;
    
    // Capacité maximale AVEC LIMITE pour préserver la qualité visuelle
    // Par défaut, on limite à 2 bits par canal pour que l'image reste visuellement identique
    size_t maxCapacityBits = (size_t)cw * ch * cc * maxBitsPerChannel;
    
    if (totalNeededBits <= maxCapacityBits) {
        return true; // Pas besoin de redimensionner
    }
    
    // Si ça ne rentre pas avec la limite, on DOIT redimensionner
    cout << "⚠️  Image secrète trop grande pour préserver la qualité de l'image porteuse.\n";
    cout << "   Image secrète : " << sw << "x" << sh << " (" << (currentSizeBits / 8 / 1024) << " Ko)\n";
    cout << "   Capacité max : " << (maxCapacityBits / 8 / 1024) << " Ko (avec " << maxBitsPerChannel << " bits/canal pour qualité optimale)\n";
    
    // Calculer la taille maximale disponible pour les données d'image
    size_t maxDataBits = maxCapacityBits - signatureBits - balisesBits - headerBits;
    size_t maxDataBytes = maxDataBits / 8;
    
    // Calculer le facteur de réduction (avec marge de sécurité de 1%)
    float scale = sqrt((float)maxDataBytes / (sw * sh * sc)) * 0.99f;
    
    // Nouvelles dimensions
    int newW = max(1, (int)(sw * scale));
    int newH = max(1, (int)(sh * scale));
    
    cout << "📐 Redimensionnement à " << newW << "x" << newH << " (interpolation bilinéaire)...\n";
    
    // Redimensionner avec interpolation bilinéaire
    resizedBuffer = resizeImage(secret, sw, sh, sc, newW, newH);
    secret = resizedBuffer.data();
    sw = newW;
    sh = newH;
    
    return true;
}

// ============ STÉGANOGRAPHIE IMAGE DANS IMAGE (avec signature et balises) ============

// Cacher une image dans une autre (avec signature et balises - logique BMP adaptée)
inline vector<unsigned char> hideImageInImage(unsigned char* carrier, int cw, int ch, int cc,
                                              unsigned char* secret, int sw, int sh, int sc,
                                              int bitsPerChannel = 1) {
    // Construire le message binaire complet
    string messageBinaire;
    
    // 1. Ajouter la signature
    messageBinaire += getSignatureBinary();
    
    // 2. Ajouter la balise ouvrante
    messageBinaire += getBaliseBinary(true);
    
    // 3. Ajouter le header (dimensions de l image secrète: largeur, hauteur, canaux)
    for (uint32_t val : {(uint32_t)sw, (uint32_t)sh, (uint32_t)sc}) {
        bitset<32> bits(val);
        messageBinaire += bits.to_string();
    }
    
    // 4. Ajouter les données de l image secrète
    for (int i = 0; i < sw * sh * sc; ++i) {
        bitset<8> bits(secret[i]);
        messageBinaire += bits.to_string();
    }
    
    // 5. Ajouter la balise fermante
    messageBinaire += getBaliseBinary(false);
    
    // Mode automatique : si bitsPerChannel == 0, calculer le minimum nécessaire
    // MAIS limiter à 2 bits maximum pour préserver la qualité visuelle
    if (bitsPerChannel == 0) {
        // Essayer d'abord avec 1 bit (le plus discret), puis 2 bits
        for (int b = 1; b <= 2; ++b) {
            size_t capacite = cw * ch * cc * b;
            if (messageBinaire.size() <= capacite) {
                bitsPerChannel = b;
                break;
            }
        }
        
        // Si ça ne rentre pas même avec 2 bits, c'est une erreur
        // (calculateOptimalSize aurait dû redimensionner l'image secrète)
        if (bitsPerChannel == 0) {
            cerr << "❌ ERREUR CRITIQUE : L'image secrète est trop grande.\n";
            cerr << "   Cela ne devrait pas arriver. Redimensionnez manuellement l'image secrète.\n";
            return {};
        }
    }
    
    // Limiter à 2 bits maximum pour qualité optimale
    if (bitsPerChannel > 2) {
        cerr << "❌ ERREUR : " << bitsPerChannel << " bits par canal demandé.\n";
        cerr << "   Maximum autorisé : 2 bits pour préserver la qualité visuelle.\n";
        cerr << "   Utilisez une image porteuse plus grande ou réduisez l'image secrète.\n";
        return {};
    }
    
    // Vérifier la capacité de l image porteuse
    size_t capacite = cw * ch * cc * bitsPerChannel;
    if (messageBinaire.size() > capacite) {
        cerr << "Erreur : l image porteuse est trop petite.\n";
        cerr << "   Besoin de " << messageBinaire.size() << " bits, capacité: " << capacite << " bits.\n";
        cerr << "   Solution : utilisez une image porteuse plus grande ou une image secrète plus petite.\n";
        return {};
    }
    
    cout << "Utilisation de " << bitsPerChannel << " bit(s) par canal (LSB).\n";
    cout << "Message: " << messageBinaire.size() << " bits, Capacité: " << capacite << " bits.\n";
    
    // Créer une copie de l image porteuse
    vector<unsigned char> encoded(carrier, carrier + (cw * ch * cc));
    
    // Injection des bits dans les LSB de l image porteuse (comme dans bmpConvert)
    size_t bitIndex = 0;
    for (int i = 0; i < cw * ch * cc && bitIndex < messageBinaire.size(); ++i) {
        for (int b = 0; b < bitsPerChannel && bitIndex < messageBinaire.size(); ++b) {
            // Modifier le bit de poids faible (LSB)
            encoded[i] &= ~(1 << b);  // Effacer le bit (équivalent à &= 0xFE pour LSB)
            encoded[i] |= ((messageBinaire[bitIndex] - '0') << b);  // Insérer le nouveau bit
            bitIndex++;
        }
    }
    
    return encoded;
}

// Détecter automatiquement le nombre de bits par canal utilisé
inline int detectBitsPerChannel(unsigned char* carrier, int cw, int ch, int cc) {
    // Essayer de 1 à 8 bits par canal
    for (int bitsToTry = 1; bitsToTry <= 8; ++bitsToTry) {
        string bitsLus;
        
        // Extraire suffisamment de bits pour vérifier la signature
        size_t signatureSize = getSignatureBinary().size();
        size_t bitsNeeded = signatureSize;
        
        for (int i = 0; i < cw * ch * cc && bitsLus.size() < bitsNeeded; ++i) {
            for (int b = 0; b < bitsToTry && bitsLus.size() < bitsNeeded; ++b) {
                char bit = ((carrier[i] >> b) & 0x01) + '0';
                bitsLus += bit;
            }
        }
        
        // Vérifier si la signature correspond
        string signatureAttendue = getSignatureBinary();
        if (bitsLus.substr(0, signatureSize) == signatureAttendue) {
            cout << "Détection automatique : " << bitsToTry << " bit(s) par canal détecté.\n";
            return bitsToTry;
        }
    }
    
    cerr << "Avertissement : impossible de détecter automatiquement le nombre de bits. Utilisation de 1 bit par défaut.\n";
    return 1;
}

// Extraire une image cachée (avec vérification de signature et balises - logique BMP adaptée)
inline vector<unsigned char> extractImageFromImage(unsigned char* carrier, int cw, int ch, int cc,
                                                   int bitsPerChannel, int& outW, int& outH, int& outC) {
    // Si bitsPerChannel == 0, détecter automatiquement
    if (bitsPerChannel == 0) {
        bitsPerChannel = detectBitsPerChannel(carrier, cw, ch, cc);
    }
    
    bitsPerChannel = max(1, min(bitsPerChannel, 8));
    
    // Extraire tous les LSB de l image (comme dans bmpRecup)
    string bitsLus;
    for (int i = 0; i < cw * ch * cc; ++i) {
        for (int b = 0; b < bitsPerChannel; ++b) {
            char bit = ((carrier[i] >> b) & 0x01) + '0';
            bitsLus += bit;
        }
    }
    
    // Vérifier la signature
    string signatureAttendue = getSignatureBinary();
    size_t signatureSize = signatureAttendue.size();
    
    if (bitsLus.size() < signatureSize) {
        cerr << "Erreur : données insuffisantes !\n";
        return {};
    }
    
    string signatureTrouvee = bitsLus.substr(0, signatureSize);
    if (signatureTrouvee != signatureAttendue) {
        cerr << "Erreur : signature invalide ! Aucune image cachée détectée.\n";
        return {};
    }
    
    cout << "Signature détectée.\n";
    
    // Rechercher les balises
    string baliseOuvrante = getBaliseBinary(true);
    string baliseFermante = getBaliseBinary(false);
    
    size_t posOuv = bitsLus.find(baliseOuvrante);
    size_t posFerm = bitsLus.find(baliseFermante, posOuv);
    
    if (posOuv == string::npos || posFerm == string::npos) {
        cerr << "Erreur : balises non trouvées !\n";
        return {};
    }
    
    cout << "Balises détectées.\n";
    
    // Extraire le contenu entre les balises
    size_t debutContenu = posOuv + baliseOuvrante.size();
    string contenu = bitsLus.substr(debutContenu, posFerm - debutContenu);
    
    // Lire le header (96 bits = 3 x 32 bits pour largeur, hauteur, canaux)
    if (contenu.size() < 96) {
        cerr << "Erreur : header incomplet !\n";
        return {};
    }
    
    outW = bitset<32>(contenu.substr(0, 32)).to_ulong();
    outH = bitset<32>(contenu.substr(32, 32)).to_ulong();
    outC = bitset<32>(contenu.substr(64, 32)).to_ulong();
    
    cout << "Dimensions: " << outW << "x" << outH << " (" << outC << " canaux)\n";

    // Extraire les données de l'image
    size_t totalBits = (size_t)outW * outH * outC * 8;
    if (96 + totalBits > contenu.size()) {
        cerr << "Erreur : données d image incomplètes !\n";
        return {};
    }
    
    vector<unsigned char> secret(outW * outH * outC);
    for (size_t i = 0; i < secret.size(); ++i) {
        size_t bitIndex = 96 + (i * 8);
        string octetBinaire = contenu.substr(bitIndex, 8);
        secret[i] = static_cast<unsigned char>(bitset<8>(octetBinaire).to_ulong());
    }
    
    return secret;
}
