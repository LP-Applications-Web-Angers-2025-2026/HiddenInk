#include "encrypt.h"

#include <fstream>
#include <random>
#include <sstream>
#include <iomanip>
using namespace std;

/**
 * Génère une clé aléatoire de n caractères.
 * @param n La longueur de la clé à générer
 * @return Une chaîne de caractères contenant la clé générée aléatoirement
 */
string generate_key(size_t n)
{
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<int> d(0, 255);
    string k;
    k.resize(n);
    for (size_t i = 0; i < n; ++i) k[i] = static_cast<char>(d(gen));
    return k;
}

/**
 * Chiffre ou déchiffre des données en utilisant l'opération XOR avec une clé.
 * @param data Les données à chiffrer/déchiffrer
 * @param key La clé de chiffrement/déchiffrement
 * @return Les données chiffrées/déchiffrées
 */
string xor_encrypt(const string& data, const string& key)
{
    if (key.empty()) return data;
    string out;
    out.resize(data.size());

    for (size_t i = 0; i < data.size(); ++i)
    {
        unsigned char d = static_cast<unsigned char>(data[i]);
        unsigned char k = static_cast<unsigned char>(key[i % key.size()]);
        out[i] = static_cast<char>(d ^ k);
    }

    return out;
}

/**
 * Convertit une chaîne de caractères en sa représentation hexadécimale.
 * @param s La chaîne à convertir
 * @return La représentation hexadécimale de la chaîne
 */
string to_hex(const string& s)
{
    ostringstream os;
    os << hex << setfill('0');
    for (unsigned char c : s) os << setw(2) << (int)c;
    return os.str();
}

/**
 * Convertit une chaîne hexadécimale en clé binaire.
 * @param hex La chaîne hexadécimale à convertir
 * @return La clé binaire correspondante
 */
string hex_to_key(const string& hex)
{
    string key;
    for (size_t i = 0; i < hex.length(); i += 2)
    {
        string byteString = hex.substr(i, 2);
        unsigned char byte = static_cast<unsigned char>(stoi(byteString, nullptr, 16));
        key += byte;
    }
    return key;
}

/**
 * Sauvegarde la clé de chiffrement dans un fichier.
 * @param outputPath Le chemin du fichier de sortie
 * @param key La clé de chiffrement à sauvegarder
 */
void saveKeyFile(const string& outputPath, const string& key)
{
    if (!key.empty())
    {
        size_t pos = outputPath.find_last_of(".");
        size_t lastSlash = outputPath.find_last_of("/\\");

        string keyFilePath;
        if (lastSlash != string::npos)
        {
            // Get directory path
            string dirPath = outputPath.substr(0, lastSlash + 1);
            // Get filename without extension
            string fileName = (pos != string::npos)
                                  ? outputPath.substr(lastSlash + 1, pos - lastSlash - 1)
                                  : outputPath.substr(lastSlash + 1);
            keyFilePath = dirPath + fileName + ".hiddenink";
        }
        else
        {
            // No directory path, just filename
            string fileName = (pos != string::npos) ? outputPath.substr(0, pos) : outputPath;
            keyFilePath = fileName + ".hiddenink";
        }

        ofstream keyFile(keyFilePath);
        if (keyFile.is_open())
        {
            keyFile << to_hex(key);
            keyFile.close();
        }
    }
}
