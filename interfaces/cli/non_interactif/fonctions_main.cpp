#include "fonctions_main.h"
#include <filesystem>
#include <iostream>
#include <ostream>
#include <string>
#include "../../../stenographie/BMP/bmp_convert.h"
#include "../../../stenographie/BMP/bmp_Recuperation.h"
#include "../../../stenographie/utils/stegano/stegano_text.hpp"
#include "../interactif/fonction_menu_interactif.h"


using namespace std;

/**
 * Fonction principale qui gère la dissimulation d'un message dans une image.
 * @param argc Nombre d'arguments passés en ligne de commande
 * @param argv Tableau contenant les arguments passés en ligne de commande
 * @return 0 si succès, 1 si erreur
 */
int hide(int argc, char* argv[]) {
    string inputPath = argv[2];
    string fileToHide = argv[3];
    string outputPath = argv[4];
    string key = argv[5];
    int bitPos = bit_position(argc, argv, 5);
    if (bitPos == -1) return 1;

    if (!validate_file_exists(fileToHide)) {
        cout << "Erreur, le fichier à cacher n'existe pas." << endl;
        return 1;
    }

    // Cacher le message dans le bit spécifié
    bmpConvert(inputPath, fileToHide, outputPath, bitPos, key);
    return 0;
}

/**
 * Extrait un message caché d'une image BMP en utilisant la stéganographie.
 * 
 * @param argc Nombre d'arguments de la ligne de commande
 * @param argv Tableau des arguments de la ligne de commande contenant:
 *            - argv[2]: Chemin vers l'image BMP source
 *            - argv[3]: Clé de déchiffrement en format hexadécimal
 *            - argv[4]: (Optionnel) Position du bit de dissimulation (1-8, défaut=1)
 * 
 * @return Code de retour :
 *         - 0 : Extraction réussie
 *         - 1 : Erreur lors de l'extraction
 * 
 * @note Le message extrait est affiché sur la sortie standard.
 *       Des messages d'erreur spécifiques sont affichés si :
 *       - Le processus d'extraction échoue
 *       - Aucun message n'est trouvé dans l'image
 *       - Les balises de début/fin sont invalides/corrompues
 */
int extract(int argc, char* argv[]) {
    string inputPath = argv[2];
    string keyHex = argv[3];
    int bitPos = bit_position(argc, argv, 4);
    if (bitPos == -1) return 1;

    // Récupérer le message dans le bit spécifié avec déchiffrement
    string messageDecode = bmpRecup(inputPath, bitPos, keyHex);

    if (messageDecode == "1") {
        cout << "Erreur, le processus de récuperation du message dissimulé n'as pas réussit." << endl;
    } else if (messageDecode == "2") {
        cout << "Information, ce fichier ne possède pas de message dissimuler." << endl;
    } else if (messageDecode == "3") {
        cout << "Erreur, pas de balise correcte. message modifié ou inexistant." << endl;
    } else {
        cout << messageDecode << endl;
    }
    return 0;
}


/**
 * Lance le mode interactif du programme permettant de choisir différentes options de stéganographie
 * @return 0 si succès, 1 si erreur
 */
int interact() {
    return runInteractiveMode();
}

/**
 * Détermine la position du bit où les données seront cachées ou extraites dans une image
 * @param argc Nombre d'arguments passés en ligne de commande
 * @param argv Tableau contenant les arguments passés en ligne de commande
 * @param arg_index Position dans argv[] de l'argument optionnel spécifiant la position du bit (1-8)
 * @return Position du bit (0-7), ou -1 en cas d'erreur. Par défaut retourne 0 (LSB)
 */
int bit_position(int argc, char* argv[], int arg_index) {
    int bitNum = 1; // Par défaut LSB
    if (argc > arg_index) {
        try {
            bitNum = stoi(string(argv[arg_index]));
        } catch (...) {
            cerr << "Position de bit invalide : " << argv[arg_index] << endl;
            return -1;
        }
    }
    if (bitNum < 1 || bitNum > 8) {
        cerr << "Position de bit invalide (1-8)." << endl;
        return -1;
    }
    return bitNum - 1; // 0 pour LSB, 7 pour MSB
}

/**
 * Vérifie si un fichier existe dans le système de fichiers
 * @param path Chemin du fichier à vérifier
 * @return true si le fichier existe, false sinon
 */
bool validate_file_exists(const std::string& path) {
    return path != "" && std::filesystem::exists(path);
}

/**
 * Affiche les instructions d'utilisation du programme.
 * Cette fonction affiche la syntaxe correcte et les options qui peuvent être utilisées
 * lors de l'exécution du programme, incluant les modes "HIDE", "EXTRACT", "HISTO", et "INTERACT".
 */
void print_usage() {
    cerr << "Usage: " << " HIDE <input_bmp> <file_to_hide> <output_bmp> [bit_position]" << endl;
    cerr << "       " << " EXTRACT <input_bmp> <key> [bit_position]" << endl;
    cerr << "       " << " HISTO <input_bmp>" << endl;
    cerr << "       " << " INTERACT" << endl;
}

/**
 * Lance le mode interactif permettant de choisir les différentes opérations de stéganographie disponibles.
 * Affiche un menu avec 4 options :
 * 1. Cacher un texte dans une image
 * 2. Cacher une image dans une image 
 * 3. Extraire des données
 * 4. Analyser une image
 * @return 0 si succès, 1 si erreur lors de la saisie utilisateur
 */
int runInteractiveMode()
{
    cout << "    STEGANOGRAPHIE AVANCEE             \n";
    cout << "  1.  Cacher un texte dans une image\n";
    cout << "  2.   Cacher une image dans une image\n";
    cout << "  3.   Extraire des donnees\n";
    cout << "  4.   Analyser une image\n";
    cout << "\nChoix : ";

    int choix = 0;
    if (!(std::cin >> choix))
    {
        cerr << " Entrée invalide." << endl;
        return 1;
    }

    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    return fonction_menu(choix);
}

