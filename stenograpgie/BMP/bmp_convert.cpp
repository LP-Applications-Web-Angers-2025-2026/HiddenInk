#include "bmp_convert.h"
#include "../utils/utils_bin.h"

#include <iostream>
#include <fstream>
#include <vector>
#include <bitset>
#include <algorithm>
#include <cctype>

using namespace std;


void bmpConvert(string message) {

    // Fichier d'input et dossier d'output
    string inputPath = "../img_banque/BMP/tigre.bmp";
    string outputPath = "../out/tigre_LSB.bmp";

    // Vérification des variables
    ifstream file(inputPath, std::ios::binary);
    if (!file)
    {
        cerr << "Erreur : impossible d'ouvrir " << inputPath << std::endl;
        return ;
    }

    // Taille du header & de la signature
    size_t headerSize = 54;
    size_t signatureSize = getSignatureSize();

    // Récupération de la signature en binaire
    string signatureBinaire = getSignatureBinary();

    int taillechaine = message.size();
    string TabBinascii[taillechaine + 1];

    // Convertit chaque caractère du message en binaire et le stocke dans le tableau TabBinascii
    for (int i=0; i<taillechaine; i++) {
        TabBinascii[i] = BinForLetter(message[i]);
    }

    // Ce code C++ permet de lire l'intégralité d'un fichier en mémoire en une seule instruction.
    vector<unsigned char> data((std::istreambuf_iterator<char>(file)),
                                std::istreambuf_iterator<char>());

    // Fermer le fichier
    file.close();

    // On limite l'utilisations des octets
    size_t n = std::min<size_t>(signatureSize + (message.size() * 8), data.size() - headerSize);

    for (size_t i = 0; i < n; ++i)
    {
        bitset<8> bits(data[headerSize + i]);
        cout << bits.to_string().substr(0,4) << "|" << bits.to_string().substr(4,4) << " ";
        if ((i+1) % 8 == 0) cout << "\n";
    }

    // Convertit et stock le message en binaire dans messageBinaire
    string messageBinaire;
    messageBinaire += signatureBinaire;

    //balise ouverture
    messageBinaire += getBaliseBinary(true);

    for (int i = 0; i < taillechaine; ++i) {
        // Prend chaque caractère et le convertit en 8 bits continus
        bitset<8> b(message[i]);
        messageBinaire += b.to_string();
    }

    // balise fermantes
    messageBinaire += getBaliseBinary(false);

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
