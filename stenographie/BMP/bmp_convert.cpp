#include "bmp_convert.h"
#include "../utils/utils_bin.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <bitset>
#include <cctype>
#include <filesystem>
#include <pstl/parallel_backend_utils.h>

#include "../utils/encrypt/encrypt.h"
using namespace std;

void bmpConvert(string inputPath, string fileToHide, string outputPath, int bitPos, string key)
{
    string binFile, messageBinaire, signatureBinaire;

    unsigned char mask;

    //CHIFFREMENT DU MESSAGE
    // Lire le contenu du fichier en tant que string (clair)
    ifstream fileToHideStream(fileToHide, ios::binary);
    string plainContent((istreambuf_iterator<char>(fileToHideStream)), istreambuf_iterator<char>());
    fileToHideStream.close();

    binFile = lireFichierKey(fileToHide, key);

    // Vérification des variables
    ifstream file(inputPath, std::ios::binary);
    if (!file)
    {
        cerr << "Erreur : impossible d'ouvrir " << inputPath << std::endl;
        return;
    }

    // Ce code C++ permet de lire l'intégralité d'un fichier en mémoire en une seule instruction.
    vector<unsigned char> data((std::istreambuf_iterator<char>(file)),
                               std::istreambuf_iterator<char>());
    // Femeture du fichier
    file.close();

    // Taille du header & de la signature
    size_t headerSize = 54; // par défaut
    if (data.size() >= 30)
    {
        unsigned short bitsPerPixel = data[28] | (data[29] << 8); // lire depuis l'image BMP
        if (bitsPerPixel == 8)
        {
            headerSize += 256 * 4; // ajouter la palette
        }
    }

    // Convertit et stock le message en binaire dans messageBinaire
    messageBinaire += getSignatureBinary();
    messageBinaire += getBaliseBinary(true);
    messageBinaire += binFile;
    messageBinaire += getBaliseBinary(false);

    // Vérification que le message avec la signature et les balises peut être inséré dans l'image
    if (!messageCanFitInImage(messageBinaire, data, headerSize))
    {
        return;
    }

    // Change le bit spécifié de chaque octet de l'image par chaque bit du message
    vector<unsigned char> modifiedData = data;
    mask = ~(1 << bitPos); // Masque pour effacer le bit
    for (size_t i = 0; i < messageBinaire.size(); ++i)
    {
        modifiedData[headerSize + i] &= mask;
        modifiedData[headerSize + i] |= ((messageBinaire[i] - '0') << bitPos);
    }

    // Sauvegarder l'image modifiée
    ofstream outFile(outputPath, ios::binary);
    outFile.write(reinterpret_cast<char*>(modifiedData.data()), modifiedData.size());
    outFile.close();

    // Sauvegarder la clé
    saveKeyFile(outputPath, key);


    cout << "[HiddenInk] Votre fichier a bien été caché" << endl;
}