#pragma once
#include "external/stb_image.h"
#include "external/stb_image_write.h"
#include <iostream>
#include <fstream>
#include <iomanip>
#include <vector>
#include <string>
#include <bitset>
#include <cmath>

using namespace std;

// ============ FONCTIONS UTILITAIRES ============

// Signature pour identifier un message caché
inline string getTextSignature() {
    return "!#TEXT#!";
}

inline string getTextSignatureBinary() {
    string signatureBinaire;
    for (char c : getTextSignature()) {
        bitset<8> b(static_cast<unsigned char>(c));
        signatureBinaire += b.to_string();
    }
    return signatureBinaire;
}

// Balises pour délimiter le contenu texte
inline string getTextBalise(bool ouverture) {
    return ouverture ? "~<TXT>" : "</TXT>~";
}

inline string getTextBaliseBinary(bool ouverture) {
    string baliseBinaire;
    for (char c : getTextBalise(ouverture)) {
        bitset<8> b(static_cast<unsigned char>(c));
        baliseBinaire += b.to_string();
    }
    return baliseBinaire;
}

// Conversion binaire vers texte
inline string binToText(const string& binaire) {
    string texte;
    for (size_t i = 0; i < binaire.size(); i += 8) {
        if (i + 8 <= binaire.size()) {
            string octet = binaire.substr(i, 8);
            bitset<8> b(octet);
            texte += static_cast<char>(b.to_ulong());
        }
    }
    return texte;
}

// ============ STÉGANOGRAPHIE TEXTE ============

// Déclaration anticipée
inline int detectTextBitsPerChannel(unsigned char* carrier, int cw, int ch, int cc);

// Cacher un message texte dans une image
inline vector<unsigned char> hideTextInImage(unsigned char* carrier, int cw, int ch, int cc,
                                             const string& message, int bitsPerChannel = 0) {
    // Si bitsPerChannel = 0, calculer automatiquement le minimum nécessaire
    if (bitsPerChannel == 0) {
        bitsPerChannel = 1; // Par défaut 1 bit (meilleure qualité)
    }
    
    // Limiter à 2 bits maximum pour préserver la qualité
    if (bitsPerChannel > 2) {
        bitsPerChannel = 2;
    }
    
    // Construire le message binaire
    string messageBinaire;
    
    // 1. Signature
    messageBinaire += getTextSignatureBinary();
    
    // 2. Balise ouvrante
    messageBinaire += getTextBaliseBinary(true);
    
    // 3. Le message en binaire
    for (char c : message) {
        bitset<8> bits(static_cast<unsigned char>(c));
        messageBinaire += bits.to_string();
    }
    
    // 4. Balise fermante
    messageBinaire += getTextBaliseBinary(false);
    
    // Vérifier la capacité
    size_t capacite = (size_t)cw * ch * cc * bitsPerChannel;
    if (messageBinaire.size() > capacite) {
        cerr << "❌ Erreur : message trop long pour l'image.\n";
        cerr << "   Besoin de " << messageBinaire.size() << " bits, capacité: " << capacite << " bits.\n";
        cerr << "   Capacité max: " << (capacite / 8) << " caractères.\n";
        return {};
    }
    
    cout << "\n📊 INFORMATIONS :\n";
    cout << "   Image porteuse : " << cw << "x" << ch << " (" << cc << " canaux)\n";
    cout << "   Message : " << message.length() << " caractères\n";
    cout << "   Capacité disponible : " << capacite << " bits\n";
    cout << "   Capacité nécessaire : " << messageBinaire.size() << " bits\n";
    cout << "   Utilisation : " << fixed << setprecision(2) << (messageBinaire.size() * 100.0 / capacite) << "%\n";
    cout << "   Bits par canal : " << bitsPerChannel << "\n";
    
    // Créer une copie de l'image
    vector<unsigned char> encoded(carrier, carrier + (cw * ch * cc));
    
    // Injection dans les LSB
    size_t bitIndex = 0;
    for (int i = 0; i < cw * ch * cc && bitIndex < messageBinaire.size(); ++i) {
        for (int b = 0; b < bitsPerChannel && bitIndex < messageBinaire.size(); ++b) {
            encoded[i] &= ~(1 << b);
            encoded[i] |= ((messageBinaire[bitIndex] - '0') << b);
            bitIndex++;
        }
    }
    
    return encoded;
}

// Extraire un message texte d'une image
inline string extractTextFromImage(unsigned char* carrier, int cw, int ch, int cc, int bitsPerChannel = 0) {
    // Si bitsPerChannel = 0, détecter automatiquement
    if (bitsPerChannel == 0) {
        cout << "🔍 Détection automatique...\n";
        bitsPerChannel = detectTextBitsPerChannel(carrier, cw, ch, cc);
        if (bitsPerChannel == 0) {
            cerr << "❌ Aucun message texte détecté.\n";
            return "";
        }
    }
    
    bitsPerChannel = max(1, min(bitsPerChannel, 8));
    
    // Extraire tous les bits
    string bitsLus;
    for (int i = 0; i < cw * ch * cc; ++i) {
        for (int b = 0; b < bitsPerChannel; ++b) {
            char bit = ((carrier[i] >> b) & 0x01) + '0';
            bitsLus += bit;
        }
    }
    
    // Vérifier la signature
    string signatureAttendue = getTextSignatureBinary();
    size_t signatureSize = signatureAttendue.size();
    
    if (bitsLus.size() < signatureSize) {
        return ""; // Pas de message
    }
    
    string signatureTrouvee = bitsLus.substr(0, signatureSize);
    if (signatureTrouvee != signatureAttendue) {
        return ""; // Signature invalide
    }
    
    cout << "✓ Signature texte détectée.\n";
    
    // Rechercher les balises
    string baliseOuvrante = getTextBaliseBinary(true);
    string baliseFermante = getTextBaliseBinary(false);
    
    size_t posOuv = bitsLus.find(baliseOuvrante);
    size_t posFerm = bitsLus.find(baliseFermante, posOuv);
    
    if (posOuv == string::npos || posFerm == string::npos) {
        return ""; // Balises non trouvées
    }
    
    cout << "✓ Balises détectées.\n";
    
    // Extraire le message
    size_t debutMessage = posOuv + baliseOuvrante.size();
    string messageBinaire = bitsLus.substr(debutMessage, posFerm - debutMessage);
    
    return binToText(messageBinaire);
}

// Détecter automatiquement le nombre de bits pour un message texte
inline int detectTextBitsPerChannel(unsigned char* carrier, int cw, int ch, int cc) {
    for (int bitsToTry = 1; bitsToTry <= 8; ++bitsToTry) {
        string bitsLus;
        size_t signatureSize = getTextSignatureBinary().size();
        
        for (int i = 0; i < cw * ch * cc && bitsLus.size() < signatureSize; ++i) {
            for (int b = 0; b < bitsToTry && bitsLus.size() < signatureSize; ++b) {
                char bit = ((carrier[i] >> b) & 0x01) + '0';
                bitsLus += bit;
            }
        }
        
        if (bitsLus.substr(0, signatureSize) == getTextSignatureBinary()) {
            cout << "✓ Détection : " << bitsToTry << " bit(s) par canal.\n";
            return bitsToTry;
        }
    }
    return 0; // Pas de message trouvé
}

// Sauvegarder le message extrait dans un fichier .txt
inline bool saveMessageToFile(const string& message, const string& filepath) {
    ofstream file(filepath);
    if (!file.is_open()) {
        cerr << "❌ Erreur : impossible de créer le fichier " << filepath << "\n";
        return false;
    }
    
    file << message;
    file.close();
    
    cout << "💾 Message sauvegardé dans : " << filepath << "\n";
    return true;
}
