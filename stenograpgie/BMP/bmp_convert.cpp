#include "bmp_convert.h"
#include "../utils/utils_bin.h"

#include <iostream>
#include <fstream>
#include <vector>
#include <bitset>
#include <algorithm>
#include <cctype>

using namespace std;

void bmpConvert(string message)
{
    // Fichier d'input et dossier d'output
    string inputPath = "../img_banque/BMP/tigre.bmp";
    string outputPath = "../out/tigre_LSB.bmp";

    // Vérification des variables
    ifstream file(inputPath, std::ios::binary);
    if (!file)
    {
        cerr << "Erreur : impossible d'ouvrir " << inputPath << std::endl;
        return;
    }

    // Taille du header & de la signature
    size_t headerSize = 54;
    size_t signatureSize = getSignatureSize();

    // Récupération de la signature en binaire
    string signatureBinaire = getSignatureBinary();

    // Ce code C++ permet de lire l'intégralité d'un fichier en mémoire en une seule instruction.
    vector<unsigned char> data((std::istreambuf_iterator<char>(file)),
                               std::istreambuf_iterator<char>());
    // Femeture du fichier
    file.close();

    // On limite l'utilisations des octets
    size_t n = std::min<size_t>(signatureSize + (message.length()), data.size() - headerSize);

    for (size_t i = 0; i < n; ++i)
    {
        bitset<8> bits(data[headerSize + i]);
        if ((i + 1) % 8 == 0) cout << "\n";
    }

    // Convertit et stock le message en binaire dans messageBinaire
    string messageBinaire;
    messageBinaire += signatureBinaire;
    messageBinaire += getBaliseBinary(true);
    messageBinaire += message;
    messageBinaire += getBaliseBinary(false);

    // Vérifier que le message tient dans l'image
    if (messageBinaire.size() > data.size() - headerSize) {
        cerr << "Erreur : le message est trop grand pour être caché dans cette image !" << endl;
        return;
    }

    // Change le LSB de chaque octet de l'image par chaque bit du message
    vector<unsigned char> modifiedData = data;
    for (size_t i = 0; i < messageBinaire.size(); ++i) {
        modifiedData[headerSize + i] &= 0xFE;
        modifiedData[headerSize + i] |= (messageBinaire[i] - '0');
    }


    // Sauvegarder l'image modifiée
    ofstream outFile(outputPath, ios::binary);
    outFile.write(reinterpret_cast<char*>(modifiedData.data()), modifiedData.size());
    outFile.close();

    cout << "[HiddenInk] Votre fichier a bien été caché" << endl;
}
