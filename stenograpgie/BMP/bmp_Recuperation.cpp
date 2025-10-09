#include "bmp_Recuperation.h"
#include "../utils/utils_bin.h"

#include <iostream>
#include <fstream>
#include <vector>
#include <bitset>
#include <cctype>

using namespace std;

string bmpRecup(const string& inputPath) {

    string bitsLus;

    // où sera enregistrer le message en binaire
    string messageBinaire;

    // Taille de l'entète format BMP
    size_t headerSize = 54;

    // Ouverture du fichier en mode binaire
    ifstream file(inputPath, std::ios::binary);
    if (!file)
    {
        cerr << "Erreur : impossible d'ouvrir " << inputPath << std::endl;
        return "1";
    }

    // Ce code C++ permet de lire l'intégralité d'un fichier en mémoire en une seule instruction.
    vector<unsigned char> data((std::istreambuf_iterator<char>(file)),
                                    std::istreambuf_iterator<char>());

    // Fermer le fichier
    file.close();

    // Nombre de bits de la signature
    size_t nbBits = getSignatureBinarySize();

    // Lire uniquement le LSB de chaque octet après le header
    for (size_t i = 0; i < nbBits; ++i) {
        unsigned char octet = data[headerSize + i];
        char lsb = (octet & 0x01) + '0'; // prendre le bit faible et le convertir en '0' ou '1'
        bitsLus += lsb;
    }

    // Comparer avec la chaîne attendue
    string chaineAttendue = getSignatureBinary();

    if (bitsLus != chaineAttendue){
        return "2";
    }

    // Lire tous les LSB (1 bit par octet)
    size_t bitsALire = 256; // 🔹 Limite à 60 bits
    size_t maxIndex = headerSize + bitsALire;

    // Lire les bits faibles
    for (size_t i = headerSize; i < headerSize + bitsALire; ++i) {
        char bitLSB = (data[i] & 1) ? '1' : '0'; // plus sûr que bitset
        messageBinaire += bitLSB;
    }

    // Conversion et retour
    return binaireVersTexte(messageBinaire);

}
