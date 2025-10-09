#include "utils_bin.h"

#include <iostream>
#include <fstream>
#include <bitset>

using namespace std;

string BinForLetter(char letter) {return bitset<8>(static_cast<unsigned char>(letter)).to_string();}

string binaireVersTexte(const string& binaire) {

    string texte; // sortie texte

    // transforme les binaires (bits) des morceaux de différents octet ( recrée en paramtère) en code ascii
    for (size_t i = 0; i < binaire.size(); i += 8) {
        string octet = binaire.substr(i, 8);
        bitset<8> b(octet);
        char caractere = static_cast<char>(b.to_ulong());
        texte += caractere;
    }
    return texte;
}


string getSignature() {
    return "!#@!";
}

size_t getSignatureSize() {
    return getSignature().size();
}

string getSignatureBinary() {
    string signature = getSignature();
    string signatureBinaire;

    // Pour chaque caractère de la signature, on ajoute sa représentation binaire sur 8 bits
    for (char c : signature) {
        bitset<8> b(static_cast<unsigned char>(c));
        signatureBinaire += b.to_string();
    }

    return signatureBinaire; // renvoie la signature en binaire
}

size_t getSignatureBinarySize() {
    // Taille en bits = nombre de caractères * 8
    return  getSignatureBinary().size();
}

