#include "bmp_steg.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <bitset>
#include <algorithm>
#include <cctype>

using namespace std;

// ========================================
// MASQUES POUR MANIPULATION DE BITS
// ========================================
// --- Masques pour mettre un bit à 1 (OR avec |=) ---
// Bit 0 (LSB) : 0x01 = 0000 0001 = 1   --> data[i] |= 0x01;
// Bit 1       : 0x02 = 0000 0010 = 2   --> data[i] |= 0x02;
// Bit 2       : 0x04 = 0000 0100 = 4   --> data[i] |= 0x04;
// Bit 3       : 0x08 = 0000 1000 = 8   --> data[i] |= 0x08;
// Bit 4       : 0x10 = 0001 0000 = 16  --> data[i] |= 0x10;
// Bit 5       : 0x20 = 0010 0000 = 32  --> data[i] |= 0x20;
// Bit 6       : 0x40 = 0100 0000 = 64  --> data[i] |= 0x40;
// Bit 7 (MSB) : 0x80 = 1000 0000 = 128 --> data[i] |= 0x80;
// --- Masques pour mettre un bit à 0 (AND avec &=) ---
// Bit 0 (LSB) : 0xFE = 1111 1110 = 254 --> data[i] &= 0xFE;
// Bit 1       : 0xFD = 1111 1101 = 253 --> data[i] &= 0xFD;
// Bit 2       : 0xFB = 1111 1011 = 251 --> data[i] &= 0xFB;
// Bit 3       : 0xF7 = 1111 0111 = 247 --> data[i] &= 0xF7;
// Bit 4       : 0xEF = 1110 1111 = 239 --> data[i] &= 0xEF;
// Bit 5       : 0xDF = 1101 1111 = 223 --> data[i] &= 0xDF;
// Bit 6       : 0xBF = 1011 1111 = 191 --> data[i] &= 0xBF;
// Bit 7 (MSB) : 0x7F = 0111 1111 = 127 --> data[i] &= 0x7F;


string BinForLetter(char letter) {return bitset<8>(static_cast<unsigned char>(letter)).to_string();}

string binaireVersTexte(const string& binaire) {
    cout << "Char : " << binaire << endl;
    string texte;
    for (size_t i = 0; i < binaire.size(); i += 8) {
        string octet = binaire.substr(i, 8);
        bitset<8> b(octet);
        char caractere = static_cast<char>(b.to_ulong());
        texte += caractere;
    }
    return texte;
}

int bmpConvert(string message) {
    int taillechaine = message.size();

    string TabBinascii[taillechaine + 1];

    for (int i=0; i<taillechaine; i++) {
        TabBinascii[i] = BinForLetter(message[i]);
    }

    // Taille de l'entète format BMP
    size_t headerSize = 54;

    // Image que tu veux modifier
    string inputPath = "../img_banque/BMP/tigre.bmp";

    /* Ouverture du fichier en mode binaire
    Le mode binaire est essentiel ici car vous travaillez avec un fichier BMP (image), qui contient des données binaires brutes.
    En mode binaire :
    - Les données sont lues exactement telles qu'elles sont stockées (octet par octet)
    - Aucune conversion ou interprétation n'est effectuée
    - Les caractères spéciaux comme , `\r` ne sont pas transformés `\n` */
    ifstream file(inputPath, std::ios::binary);
    if (!file)
    {
        cerr << "Erreur : impossible d'ouvrir " << inputPath << std::endl;
        return ;
    }

    // Ce code C++ permet de lire l'intégralité d'un fichier en mémoire en une seule instruction.
    vector<unsigned char> data((std::istreambuf_iterator<char>(file)),
                                std::istreambuf_iterator<char>());

    // Fermer le fichier
    file.close();

    cout << "Fichier lu : " << data.size() << " octets.\n";

    // limiter à 64 octets
    size_t n = std::min<size_t>(64, data.size() - headerSize);

    // Affichage avant modification
    cout << "\n--- Bits forts | Bits faibles avant modification ---\n";
    for (size_t i = 0; i < n; ++i)
    {
        bitset<8> bits(data[headerSize + i]);
        cout << bits.to_string().substr(0,4) << "|" << bits.to_string().substr(4,4) << " ";
        if ((i+1) % 8 == 0) cout << "\n";
    }

    // Convertit et stock le message en binaire dans messageBinaire
    string messageBinaire;
    for (int i = 0; i < taillechaine; ++i) {
        // Prend chaque caractère et le convertit en 8 bits continus
        bitset<8> b(message[i]);
        messageBinaire += b.to_string(); // Pas d'espaces, juste les bits
    }

    // Change le LSB de chaque octet de l'image par chaque bit du message
    vector<unsigned char> modifiedData = data;
    for (size_t i = 0; i < messageBinaire.size(); ++i){
        modifiedData[headerSize + i] &= 0xFE; // Met à 0 le LSB
        modifiedData[headerSize + i] |= (messageBinaire[i] - '0'); // Change le LSB avec le bit du message
    }

    // Affichage après modification
    cout << "\n--- Bits forts | Bits faibles apres modification ---\n";
    for (size_t i = 0; i < n; ++i)
    {
        bitset<8> bits(modifiedData[headerSize + i]);
        cout << bits.to_string().substr(0,4) << "|" << bits.to_string().substr(4,4) << " ";
        if ((i+1) % 8 == 0) cout << "\n";
    }

    // Sauvegarder l'image modifiée
    ofstream outFile("../out/tigre_LSB.bmp", ios::binary);
    outFile.write(reinterpret_cast<char*>(modifiedData.data()), modifiedData.size());
    outFile.close();
}


string bmpRecup(string inputPath) {

    // Taille de l'entète format BMP
    size_t headerSize = 54;

    // Ouverture du fichier en mode binaire

    ifstream file(inputPath, std::ios::binary);
    if (!file)
    {
        cerr << "Erreur : impossible d'ouvrir " << inputPath << std::endl;
        return "ERROR";
    }

    // Ce code C++ permet de lire l'intégralité d'un fichier en mémoire en une seule instruction.
    vector<unsigned char> data((std::istreambuf_iterator<char>(file)),
                                    std::istreambuf_iterator<char>());

    // Fermer le fichier
    file.close();

    // limiter à 64 octets
    size_t n = std::min<size_t>(64, data.size() - headerSize);

    string message;

    /**
     * 11101111 11111111 00001111 00110011 00001111 11111111
     *  1           1        1       1           1    1
     */

    // Lire tous les LSB (1 bit par octet)
    string messageBinaire;
    size_t bitsALire = 64; // 🔹 Limite à 60 bits
    size_t maxIndex = headerSize + bitsALire;

    // 🔸 Lire les bits faibles
    for (size_t i = headerSize; i < headerSize + bitsALire; ++i) {
        char bitLSB = (data[i] & 1) ? '1' : '0'; // plus sûr que bitset
        messageBinaire += bitLSB;
    }

    // 🔸 Conversion
    message = binaireVersTexte(messageBinaire);

    return message;
}