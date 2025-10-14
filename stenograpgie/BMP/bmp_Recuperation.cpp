#include "bmp_Recuperation.h"
#include "../utils/utils_bin.h"

#include <iostream>
#include <fstream>
#include <vector>
#include <bitset>
#include <cctype>

using namespace std;

string bmpRecup(const string& inputPath)
{
    string bitsLus;
    string messageBinaire;

    // Taille de l'entête du format BMP
    const size_t headerSize = 54;

    // --- Ouverture du fichier ---
    ifstream file(inputPath, ios::binary);
    if (!file)
    {
        cerr << "Erreur : impossible d'ouvrir " << inputPath << endl;
        return "1";
    }

    // Lecture complète du fichier en mémoire
    vector<unsigned char> data((istreambuf_iterator<char>(file)), {});
    file.close();

    // --- Vérification de la signature ---
    const size_t nbBits = getSignatureBinarySize();
    for (size_t i = 0; i < nbBits && headerSize + i < data.size(); ++i)
    {
        unsigned char octet = data[headerSize + i];
        bitsLus += (octet & 0x01) + '0';
    }

    // --- Vérification de la signature ---
    const string signatureAttendue = getSignatureBinary();
    if (bitsLus != signatureAttendue)
        return "2";

    // --- Extraction du message complet ---
    bitsLus.clear();
    bitsLus.reserve(data.size() - headerSize);

    for (size_t i = headerSize; i < data.size(); ++i)
        bitsLus += (data[i] & 0x01) + '0';

    // Recherche des balises
    const string baliseOuvrante = getBaliseBinary(true);
    const string baliseFermante = getBaliseBinary(false);

    const size_t posOuv = bitsLus.find(baliseOuvrante);
    const size_t posFerm = bitsLus.find(baliseFermante, posOuv);

    if (posOuv == string::npos || posFerm == string::npos)
        return "3";

    // Extraction du contenu entre balises
    messageBinaire = bitsLus.substr(
        posOuv + baliseOuvrante.size(),
        posFerm - (posOuv + baliseOuvrante.size())
    );

    // --- Détection du type de fichier ---
    string extension = ".txt"; // par défaut texte

    // Vérifie si le message binaire commence par différentes signatures
    if (messageBinaire.size() >= 16)
    {
        string debut = messageBinaire.substr(0, 16);

        if (debut.substr(0, 8) == "01000010" && // 'B'
            debut.substr(8, 8) == "01001101") // 'M'
        {
            extension = ".bmp";
        }
        else if (debut.substr(0, 8) == "10001001" && // PNG signature
            debut.substr(8, 8) == "01010000")
        {
            extension = ".png";
        }
    }

    // --- Sauvegarde du contenu extrait ---
    const string outputPath = "../out/fichier_extrait" + extension;
    ofstream outFile(outputPath, ios::binary);
    if (!outFile)
    {
        return "4";
    }

    if (extension == ".txt")
    {
        outFile << binaireVersTexte(messageBinaire);
    }
    else // fichier binaire (image)
    {
        vector<unsigned char> binData;
        binData.reserve(messageBinaire.length() / 8);

        for (size_t i = 0; i + 8 <= messageBinaire.length(); i += 8)
        {
            bitset<8> bits(messageBinaire.substr(i, 8));
            binData.push_back(static_cast<unsigned char>(bits.to_ulong()));
        }

        outFile.write(reinterpret_cast<const char*>(binData.data()), binData.size());
    }

    outFile.close();
    // --- Retour console / texte ---
    return "[HiddenInk] " +
        std::string(extension == ".txt" ? "Texte"
            : (extension == ".png" ? "Image PNG" : "Image BMP")) +
        " extrait(e) avec succès dans " + outputPath;
}
