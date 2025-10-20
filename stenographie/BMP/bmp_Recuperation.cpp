#include "bmp_Recuperation.h"
#include "../utils/utils_bin.h"
#include "../utils/encrypt/encrypt.h"

#include <iostream>
#include <fstream>
#include <vector>
#include <bitset>
#include <cctype>
#include <filesystem>

using namespace std;

string bmpRecup(const string& inputPath, int bitPos, const string& key)
{
    // --- Ouverture du fichier ---
    ifstream file(inputPath, ios::binary);
    if (!file)
    {
        cerr << "Erreur : impossible d'ouvrir " << inputPath << endl;
        return "1";
    }

    string bitsLus, cipher, octetStr, byteKey, extension, messageBinaire, cipherBinaire;

    unsigned char octet;

    // Taille de l'entête du format BMP
    const size_t headerSize = 54;

    // Lecture complète du fichier en mémoire
    vector<unsigned char> data((istreambuf_iterator<char>(file)), {});
    file.close();

    // --- Vérification de la signature ---
    const size_t nbBits = getSignatureBinarySize();
    for (size_t i = 0; i < nbBits && headerSize + i < data.size(); ++i)
    {
        octet = data[headerSize + i];
        bitsLus += ((octet >> bitPos) & 0x01) + '0';
    }

    // --- Vérification de la signature ---
    const string signatureAttendue = getSignatureBinary();
    if (bitsLus != signatureAttendue)
        return "2";

    // --- Extraction du message complet ---
    bitsLus.clear();
    bitsLus.reserve(data.size() - headerSize);

    for (size_t i = headerSize; i < data.size(); ++i)
        bitsLus += ((data[i] >> bitPos) & 0x01) + '0';

    // Recherche des balises
    const string baliseOuvrante = getBaliseBinary(true);
    const string baliseFermante = getBaliseBinary(false);

    const size_t posOuv = bitsLus.find(baliseOuvrante);
    const size_t posFerm = bitsLus.find(baliseFermante, posOuv);

    if (posOuv == string::npos || posFerm == string::npos)
        return "3";

    // Extraction du contenu entre balises
    cipherBinaire = bitsLus.substr(
        posOuv + baliseOuvrante.size(),
        posFerm - (posOuv + baliseOuvrante.size())
    );


    //DECHIFFREMENT DU MESSAGE
    // Convertir le binaire chiffré en string
    for (size_t i = 0; i + 8 <= cipherBinaire.length(); i += 8)
    {
        octetStr = cipherBinaire.substr(i, 8);
        if (octetStr.length() == 8)
        {
            bitset<8> bits(octetStr);
            cipher += static_cast<char>(bits.to_ulong());
        }
    }


    if (!key.empty())
    {
        // Conversion de la clé hex en bytes
        byteKey = hex_to_key(key);

        // Déchiffrement si clé fournie 
        messageBinaire = xor_encrypt(cipher, byteKey);
    }
    else
    {
        messageBinaire = cipher;
        cout << messageBinaire << endl;
    }
    //--- FIN DECHIFFREMENT DU MESSAGE ---


    // --- Détection du type de fichier ---
    extension = ".txt"; // par défaut texte

    // Vérifie si le message commence par différentes signatures
    if (messageBinaire.size() >= 2 &&
        static_cast<unsigned char>(messageBinaire[0]) == 0x42 && // 'B'
        static_cast<unsigned char>(messageBinaire[1]) == 0x4D) // 'M'
    {
        extension = ".bmp";
    }
    else if (messageBinaire.size() >= 8 &&
        messageBinaire.substr(0, 8) == "\x89PNG\r\n\x1a\n")
    {
        extension = ".png";
    }

    // --- Sauvegarde du contenu extrait ---
    const string outputPath = "../out/fichier_extrait" + extension;
    std::filesystem::create_directories("../out");
    ofstream outFile(outputPath, ios::binary);
    if (!outFile)
    {
        return "4";
    }

    // messageBinaire est déjà les données binaires sous forme de string
    outFile.write(messageBinaire.data(), messageBinaire.size());

    outFile.close();
    // --- Retour console / texte ---
    return "[HiddenInk] " +
        std::string(extension == ".txt"
                        ? "Texte"
                        : (extension == ".png" ? "Image PNG" : "Image BMP")) +
        " extrait(e) avec succès dans " + outputPath;
}
