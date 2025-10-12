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
    string signatureAttendue = getSignatureBinary();

    if (bitsLus != signatureAttendue){
        return "2";
    }

    // ---- On recherche les balises ouvrantes et fermantes ----

    // Compater avec les balises ouvrantes et fermantes
    string baliseOuvrante = getBaliseBinary(true);
    string baliseFermante = getBaliseBinary(false);

    bitsLus.clear();
    for (size_t i = headerSize; i < data.size(); ++i) {
        char bit = (data[i] & 0x01) + '0';
        bitsLus += bit;
    }

    size_t posOuv = bitsLus.find(getBaliseBinary(true));
    size_t posFerm = bitsLus.find(getBaliseBinary(false), posOuv);

    // Vérifie si les deux balises existent
    if (posOuv == string::npos || posFerm == string::npos) {
        return "3";
    }

    // On extrait le message entre les balises
    messageBinaire = bitsLus.substr(
    posOuv + getBaliseBinary(true).size(),
    posFerm - (posOuv + getBaliseBinary(true).size())
);

    // Conversion et retour
    return binaireVersTexte(messageBinaire);
};
