#include "fonctions_main.h"
#include <filesystem>
#include <iostream>
#include <ostream>
#include <string>

#include "../stenographie/main_helpers.hpp"
#include "../stenographie/BMP/bmp_convert.h"
#include "../stenographie/BMP/bmp_Recuperation.h"
#include "../stenographie/utils/utils_bin.h"
#include "../stenographie/utils/analysis/image_analysis.hpp"
#include "../stenographie/utils/stegano/stegano_text.hpp"
#include "../stenographie/utils/stegano/stegano_imageinimage.hpp"
#include "../stenographie/utils/histogramme/histogram.h"
#include "../stenographie/PNG/fonction_menu.h"
#include "../stenographie/utils/encrypt/encrypt.h"
#include "../stenographie/utils/utils_bin.h"


using namespace std;

int hide(int argc, char* argv[]) {
    string inputPath = argv[2];
    string fileToHide = argv[3];
    string outputPath = argv[4];
    int bitPos = bit_position(argc, argv, 5);
    if (bitPos == -1) return 1;

    if (!validate_file_exists(fileToHide)) {
        cout << "Erreur, le fichier à cacher n'existe pas." << endl;
        return 1;
    }

    // Cacher le message dans le bit spécifié
    bmpConvert(inputPath, fileToHide, outputPath, bitPos);
    return 0;
}

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

int interact() {
    return runInteractiveMode();
}

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

bool validate_file_exists(const std::string& path) {
    return path != "" && std::filesystem::exists(path);
}

void print_usage() {
    cerr << "Usage: " << " HIDE <input_bmp> <file_to_hide> <output_bmp> [bit_position]" << endl;
    cerr << "       " << " EXTRACT <input_bmp> <key> [bit_position]" << endl;
    cerr << "       " << " HISTO <input_bmp>" << endl;
    cerr << "       " << " INTERACT" << endl;
}

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

