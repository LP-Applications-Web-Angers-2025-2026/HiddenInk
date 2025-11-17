#include "utils_bin.h"

#include <algorithm>
#include <iostream>
#include <fstream>
#include <bitset>
#include <vector>
#include "encrypt/encrypt.h"

using namespace std;

/**
 * Retourne la représentation binaire sur 8 bits d'un caractère
 * @param letter Le caractère à convertir
 * @return La représentation binaire du caractère
 */
string BinForLetter(char letter) {return bitset<8>(static_cast<unsigned char>(letter)).to_string();}

/**
 * Converti une chaîne de caractères en une chaîne de bits 
 * @param message La chaîne de caractères à convertir
 * @return La chaîne de bits représentant le message (8 bits par caractère)
 */
string BinForString(string message) {
    string binaire;
    for (char c : message) {
        binaire += BinForLetter(c);
    }
    return binaire;
}

/**
 * Convertit le contenu d'un fichier en représentation binaire
 * @param filePath Chemin du fichier à convertir
 * @return La chaîne de bits représentant le contenu du fichier (8 bits par caractère)
 */
string BinForFile(const string& filePath) {
    ifstream file(filePath, std::ios::binary);
    if (!file) {
        cerr << "Erreur : impossible d'ouvrir " << filePath << std::endl;
        return "";
    }
    string binaire;
    char c;
    while (file.get(c)) {
        binaire += BinForLetter(c);
    }
    return binaire;
}

/**
 * Convertit une chaîne de bits en texte ASCII 
 * @param binaire La chaîne de bits à convertir
 * @return Le texte ASCII correspondant aux bits
 */
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

/**
 * Retourne une signature permettant d'identifier la présence d'un message dissimulé
 * @return La signature sous forme de chaine de caractères
 */
string getSignature() {
    return "!#@!";
}

/**
 * Retourne la taille en caractère de la signature
 * @return La taille de la signature
 */
size_t getSignatureSize() {
    return getSignature().size();
}

/**
 * Retourne la signature sous forme binaire
 * @return La signature en binaire sur 8 bits par caractère
 */
string getSignatureBinary() {
    string signatureBinaire;

    // Pour chaque caractère de la signature, on ajoute sa représentation binaire sur 8 bits
    return BinForString(getSignature());
}

/**
 * Calcule et renvoie la taille de la signature binaire en bits
 * @return La taille en bits de la signature binaire
 */
size_t getSignatureBinarySize() {
    // Taille en bits = nombre de caractères * 8
    return  getSignatureBinary().size();
}

/**
 * Retourne la balise d'ouverture ou de fermeture selon le booléen passé en paramètre
 * @param ouverture True pour la balise ouvrante, false pour la balise fermante 
 * @return La balise sous forme de chaîne de caractères
 */
string getBalise(bool ouverture) {

    // 1 = ouverture
    // 0 fermeture
    string balise;
    if (ouverture) { balise = "~{&";}
    else { balise = "&}~"; }

    return balise;
}

/**
 * Retourne la taille de la balise (ouvrante ou fermante) en caractères
 * @return La taille d'une balise en nombre de caractères
 */
size_t getBaliseSize() {
    return getBalise(0).size();
}

/**
 * Convertit une balise en sa représentation binaire
 * @param ouverture True pour la balise ouvrante, false pour la balise fermante
 * @return La balise en binaire sur 8 bits par caractère
 */
string getBaliseBinary(bool ouverture) {
    // Pour chaque caractère de la signature, on ajoute sa représentation binaire sur 8 bits
    return BinForString(getBalise(ouverture));
}

/**
 * Retourne la taille en bits de la balise binaire
 * @return La taille de la balise binaire en nombre de bits
 */
size_t getBaliseBinarySize() {
    return getBaliseBinary(0).size();
}

/**
 * Vérifie si le fichier a une extension supportée
 * @param filePath Le chemin du fichier à vérifier
 * @return True si l'extension est supportée, false sinon
 */
bool supportedFile(const string& filePath)
{
    // listes des extensions prise en charge
    const vector<string> supportedExtensions = {
        ".txt", ".bmp", ".png"
    };

    // Obtenir les extensions
    size_t dotPos = filePath.find_last_of('.');
    if (dotPos == string::npos) return false;

    string extension = filePath.substr(dotPos);

    // Convertir les extension en lowercase
    transform(extension.begin(), extension.end(), extension.begin(), ::tolower);

    // vérifier que l'extension est supporté
    return find(supportedExtensions.begin(), supportedExtensions.end(), extension) != supportedExtensions.end();
}

/**
 * Lit le contenu d'un fichier, le chiffre avec une clé XOR, et retourne la représentation binaire
 * @param filetohide Chemin du fichier à lire et chiffrer
 * @param key Clé de chiffrement XOR
 * @return La chaîne de bits représentant le fichier chiffré (8 bits par caractère)
 */
string lireFichierKey(string filetohide, string key) {
    ifstream file(filetohide, ios::binary);
    if (!file) {
        cerr << "Erreur : impossible d'ouvrir " << filetohide << endl;
        return "";
    }
    string content((istreambuf_iterator<char>(file)), istreambuf_iterator<char>());
    file.close();

    string encrypted = xor_encrypt(content, key);
    return BinForString(encrypted);
}

/**
 * Nettoie le chemin en supprimant les guillemets et espaces inutiles
 * @param s Le chemin à nettoyer
 * @return Le chemin nettoyé
 */
string cleanPath(string s) {
    // Supprimer les guillemets au début et à la fin
    if (!s.empty() && s.front() == '"') s.erase(0, 1);
    if (!s.empty() && s.back() == '"') s.pop_back();
    // Supprimer les espaces au début et à la fin
    size_t start = s.find_first_not_of(" \t");
    if (start == string::npos) return "";
    size_t end = s.find_last_not_of(" \t");
    return s.substr(start, end - start + 1);
}

/**
 * Vérifie si le fichier existe et peut être ouvert
 * @param fichier Chemin du fichier à vérifier
 * @return True si le fichier existe et est accessible, false sinon
 */
bool VerifFichier(string fichier) {
    ifstream file(fichier);
    if (file.good()) {
        file.close();
        return true;
    }
    return false;
}

/**
 * Vérifie si le message binaire peut tenir dans l'image
 * @param messageBinaire Le message en binaire
 * @param data Les données de l'image
 * @param headerSize Taille de l'en-tête de l'image
 * @return True si le message peut être inséré, false sinon
 */
bool messageCanFitInImage(const string& messageBinaire, const vector<unsigned char>& data, size_t headerSize) {
    size_t availableBits = (data.size() - headerSize) * 8; // Chaque octet peut stocker 1 bit LSB
    size_t messageBits = messageBinaire.size();
    return messageBits <= availableBits;
}

void afficherAide()
{
    std::cout << "=== STEGANOGRAPHIE AVANCEE ===\n\n";
    std::cout << "Usage:\n";
    std::cout << "  Mode interactif : ./main\n";
    std::cout << "  Mode démonstration : ./main demo\n\n";
    std::cout << "  Cacher une image : ./main hide-image <image_porteuse> <image_secrete> <sortie.png>\n";
    std::cout << "  Extraire une image : ./main extract-image <image_avec_secret> <sortie.png>\n\n";
    std::cout << "  Cacher un texte : ./main hide-text <image_porteuse> <message> <sortie.png>\n";
    std::cout << "  Extraire un texte : ./main extract-text <image_avec_secret> [sortie.txt]\n\n";
    std::cout << "  Comparer deux images : ./main compare <image1> <image2>\n";
    std::cout << "  Analyser histogramme : ./main histogram <image>\n";
    std::cout << "  Détecter stéganographie : ./main detect <image>\n\n";
    std::cout << "Exemples:\n";
    std::cout << "  ./main hide-image carrier.png secret.png output.png\n";
    std::cout << "  ./main extract-image output.png extracted.png\n";
    std::cout << "  ./main hide-text carrier.png \"Mon message secret\" output.png\n";
    std::cout << "  ./main extract-text output.png message.txt\n";
    std::cout << "  ./main compare original.png stego.png\n";
    std::cout << "  ./main histogram image.png\n\n";
    std::cout << "Note: Le nombre de bits par canal est détecté automatiquement.\n";
    std::cout << "      Pour extract-text, le fichier .txt est optionnel (affichage uniquement si omis).\n";
}

