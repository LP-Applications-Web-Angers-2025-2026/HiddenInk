#include "bmp_Recuperation.h"
#include "../utils/utils_bin.h"
#include "../encrypt/encrypt.h"

#include <iostream>
#include <fstream>
#include <vector>
#include <bitset>
#include <cctype>
#include <filesystem>

using namespace std;

string bmpRecup(const string& inputPath, int bitPos, const string& key)
{
    string bitsLus;

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
    string cipherBinaire = bitsLus.substr(
        posOuv + baliseOuvrante.size(),
        posFerm - (posOuv + baliseOuvrante.size())
    );


    //DECHIFFREMENT DU MESSAGE
    // Convertir le binaire chiffré en string
    string cipher;
    for (size_t i = 0; i + 8 <= cipherBinaire.length(); i += 8)
    {
        string octetStr = cipherBinaire.substr(i, 8);
        if (octetStr.length() == 8) {
            bitset<8> bits(octetStr);
            cipher += static_cast<char>(bits.to_ulong());
        }
    }

    string messageBinaire;

    if (!key.empty())
    {
        // Conversion de la clé hex en bytes
        string byteKey = hex_to_key(key);

        // Déchiffrement si clé fournie
        messageBinaire = xor_encrypt(cipher, byteKey);

        // Pour les images, on n'affiche pas le contenu déchiffré
        // cout << "Message déchiffré : " << messageBinaire << "\n";
    }
    else
    {
        // Affichage du message récupéré (chiffré)
        cout << "Message récupéré (chiffré, hex) : " << to_hex(cipher) << "\n";
    }
    //--- FIN DECHIFFREMENT DU MESSAGE ---


    // --- Détection du type de fichier ---
    string extension = ".txt"; // par défaut texte

    // Vérifie si le message commence par différentes signatures
    if (messageBinaire.size() >= 2 &&
        static_cast<unsigned char>(messageBinaire[0]) == 0x42 && // 'B'
        static_cast<unsigned char>(messageBinaire[1]) == 0x4D)   // 'M'
    {
        extension = ".bmp";
    }
    else if (messageBinaire.size() >= 8 &&
             messageBinaire.substr(0, 8) == "\x89PNG\r\n\x1a\n")
    {
        extension = ".png";
    }

    // --- Sauvegarde du contenu extrait ---
    const string outputPath = "out/fichier_extrait" + extension;
    std::filesystem::create_directories("out");
    ofstream outFile(outputPath, ios::binary);
    if (!outFile)
    {
        return "4";
    }

    if (extension == ".txt")
    {
        outFile << messageBinaire;
        cout << "Message extrait : " << messageBinaire << "\n";
    }
    else // fichier binaire (image)
    {
        // messageBinaire est déjà les données binaires sous forme de string
        outFile.write(messageBinaire.data(), messageBinaire.size());
    }

    outFile.close();
    // --- Retour console / texte ---
    return "[HiddenInk] " +
        std::string(extension == ".txt" ? "Texte"
            : (extension == ".png" ? "Image PNG" : "Image BMP")) +
        " extrait(e) avec succès dans " + outputPath;
}