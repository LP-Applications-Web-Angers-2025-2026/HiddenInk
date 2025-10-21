#include "utils_bin.h"

#include <algorithm>
#include <iostream>
#include <fstream>
#include <bitset>
#include <vector>
#include "../utils/encrypt/encrypt.h"


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
 * Convertit un vecteur d'octets en chaîne binaire
 * @param data Le vecteur d'octets à convertir
 * @return La chaîne binaire correspondante (8 bits par octet)
 */
string BinForBytes(const vector<unsigned char>& data)
{
    string bits;
    bits.reserve(data.size() * 8);
    for (unsigned char c : data)
        bits += bitset<8>(c).to_string();
    return bits;
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
    bool boolBalise;

    // on vérifie si la balise à coder et la balise ouvrante ou fermemante
    if (ouverture) { boolBalise = true;}
    else { boolBalise = false;}

    // Pour chaque caractère de la signature, on ajoute sa représentation binaire sur 8 bits
    return BinForString(getBalise(boolBalise));
}

/**
 * Retourne la taille en bits de la balise binaire
 * @return La taille de la balise binaire en nombre de bits
 */
size_t getBaliseBinarySize() {
    return getBaliseBinary(0).size();
}

/**
 * Vérifie si le fichier a une extension supportée (.txt, .bmp ou .png)
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
 * Lit le contenu d'un fichier et le convertit éventuellement en binaire chiffré
 * @param fileToHide Chemin du fichier à lire
 * @param chiffrer Indique si le contenu doit être chiffré avant la conversion en binaire
 * @return Le contenu du fichier converti en binaire (et chiffré si demandé)
 */
string lireFichierKey(string fileToHide, string key)
{
    string cipher;
    // Lire le contenu du fichier en tant que string (clair)
    ifstream fileToHideStream(fileToHide, ios::binary);
    if (!fileToHideStream)
    {
        cout << "[HiddenInk] Erreur : impossible de lire le fichier à cacher." << endl;
        return "ERROR";
    }

    string plainContent((istreambuf_iterator<char>(fileToHideStream)), istreambuf_iterator<char>());
    fileToHideStream.close();

    if (!key.empty())
    {
        // Chiffrer le contenu en clair avec la clé fournie
        cipher = xor_encrypt(plainContent, key);
        plainContent = cipher; // à améliorer
    }

    // Convertir le contenu chiffré en binaire
    return BinForString(plainContent);
}

/**
 * Vérifie si le message peut être inséré dans l'image
 * @param messageBinaire Le message à cacher en binaire
 * @param data Les données de l'image
 * @param headerSize La taille de l'en-tête de l'image
 * @return true si le message peut être inséré, false sinon
 */
bool messageCanFitInImage(const string& messageBinaire, const vector<unsigned char>& data, size_t headerSize)
{
    if (messageBinaire.size() > data.size() - headerSize)
    {
        cerr << "[HiddenInk] Erreur : le message est trop grand pour être caché dans cette image !" << endl;
        return false;
    }
    return true;
}

/**
 * Vérifie qu'un fichier source existe et peut être ouvert
 * @param inputPath Chemin du fichier à vérifier
 * @return true si le fichier est accessible, false sinon
 */
bool ouvrirfichierSource(string inputPath)
{
    ifstream file(inputPath, std::ios::binary);
    if (!file.is_open())
    {
        cout << "[HiddenInk] Erreur : le fichier image ne s'ouvre pas !" << inputPath << std::endl;
        // Femeture du fichier
        return false;
    }
    // Femeture du fichier
    file.close();
    return true;
}

/**
 * Vérifie si un fichier à cacher existe et peut être ouvert
 * @param fileToHide Chemin du fichier à vérifier
 * @return true si le fichier est accessible, false sinon
 */
bool ouvrirfichierMessage(string fileToHide)
{
    ifstream file(fileToHide, ios::binary);
    if (!file.is_open())
    {
        cout << "[HiddenInk] Erreur : le fichier image ne s'ouvre pas !" << fileToHide << endl;
        return false; // Femeture du fichier
    }
    // Femeture du fichier
    file.close();
    return true;
}

/**
 * Vérifie si un fichier est valide pour le traitement en stéganographie
 * 
 * @param fichier Le chemin du fichier à vérifier
 * @return false si le fichier est valide, true s'il y a une erreur
 *
 * Cette fonction effectue deux vérifications :
 * - Vérifie si l'extension du fichier est supportée (.txt, .bmp, .png)
 * - Vérifie si le fichier existe et peut être ouvert
 */
bool VerifFichier(string fichier)
{
    bool reussite = true;

    // Vérification du format du fichier à cacher
    if (!supportedFile(fichier))
    {
        cout << "[HiddenInk] Erreur : Le format du fichier n'est pas supporté." << endl;
        reussite = false;
    }

    // Vérification de l'ouverture du fichier source
    if (!ouvrirfichierMessage(fichier)) reussite = false;

    return reussite;
}


/**
 * Affiche les instructions d'utilisation et les options disponibles pour le programme de stéganographie avancée.
 *
 * Cette méthode présente sous forme détaillée les différents modes de fonctionnement du programme,
 * les commandes associées, ainsi que des exemples d'utilisation. Elle décrit les options pour
 * cacher ou extraire des images et du texte, comparer des images, analyser des histogrammes et
 * détecter la présence de stéganographie.
 */
// TODO : METTRE A JOUR
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

