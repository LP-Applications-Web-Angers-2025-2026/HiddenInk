#include <filesystem>
#include <iostream>
#include <fstream>
#include <ostream>
#include "stenographie/utils/main_helpers.hpp"
#include "stenographie/BMP/bmp_convert.h"
#include "stenographie/BMP/bmp_Recuperation.h"
#include "stenographie/utils/utils_bin.h"
#include "stenographie/encrypt/encrypt.h"

using namespace MainHelpers;

int main(int argc, char* argv[])
{
    if (argc < 2)
    {
        std::cerr << "Merci de préciser un argument" << std::endl;
        return 1; // quitte le programme proprement
    }
    string mode = argv[1];

    if (mode != "HIDE" && mode != "EXTRACT" && mode != "INTERACT")
    {
        cerr << "Mode invalide. Utilisez HIDE ou EXTRACT ou INTERACT." << endl;
        return 1;
    }

    // non interactif
    if ((mode != "INTERACT") && (argc < 4 || argc > 6))
    {
        cerr << "Usage: " << argv[0] << " HIDE <input_bmp> <file_to_hide> <output_bmp> [bit_position]" << endl;
        cerr << "       " << argv[0] << " EXTRACT <input_bmp> <key> [bit_position]" << endl;
        return 1;
    }
    cout << mode << endl;

    std::string inputPath = "test";
    if (argc >= 3 && argv[2][0] != '\0')
    {
        inputPath = argv[2];
        std::cout << inputPath << std::endl;
    }

    // Position du bit à utiliser (1-8)
    int bitNum = 1; // Par défaut LSB

    if (mode == "HIDE")
    {
        if (argc < 5 || argc > 6)
        {
            cerr << "Usage pour HIDE: " << argv[0] << " HIDE <input_bmp> <file_to_hide> <output_bmp> [bit_position]" <<
                endl;
            return 1;
        }
        string fileToHide = argv[3];
        string outputPath = argv[4];
        if (argc == 6)
        {
            try
            {
                bitNum = stoi(string(argv[5]));
            }
            catch (...)
            {
                cerr << "Position de bit invalide : " << argv[5] << endl;
                return 1;
            }
        }
        if (bitNum < 1 || bitNum > 8)
        {
            cerr << "Position de bit invalide (1-8)." << endl;
            return 1;
        }
        int bitPos = bitNum - 1; // 0 pour LSB, 7 pour MSB

        if (fileToHide == "" || !std::filesystem::exists(fileToHide))
        {
            cout << "Erreur, le fichier à cacher n'existe pas." << endl;
            return 1;
        }

        // Générer une clé
        std::string key = generate_key(16);
        cout << "Clé (hex) : " << to_hex(key) << "\n";

        // Cacher le message dans le bit spécifié
        bmpConvert(inputPath, fileToHide, outputPath, bitPos, key);
    }

    // EXTRACT //
    else if (mode == "EXTRACT")
    {
        if (argc < 4 || argc > 5)
        {
            cerr << "Usage pour EXTRACT: " << argv[0] << " EXTRACT <input_bmp> <key> [bit_position]" << endl;
            return 1;
        }
        string keyHex = argv[3];
        if (argc == 5)
        {
            try
            {
                bitNum = stoi(string(argv[4]));
            }
            catch (...)
            {
                cerr << "Position de bit invalide : " << argv[4] << endl;
                return 1;
            }
        }
        if (bitNum < 1 || bitNum > 8)
        {
            cerr << "Position de bit invalide (1-8)." << endl;
            return 1;
        }
        int bitPos = bitNum - 1; // 0 pour LSB, 7 pour MSB

        // Convertir la clé hex en string
        std::string key;
        for (size_t i = 0; i < keyHex.length(); i += 2)
        {
            std::string byteString = keyHex.substr(i, 2);
            char byte = (char)strtol(byteString.c_str(), nullptr, 16);
            key += byte;
        }

        // Récupérer le message dans le bit spécifié avec déchiffrement
        string messageDecode = bmpRecup(inputPath, bitPos, key);

        if (messageDecode == "1")
        {
            cout << "Erreur, le processus de récuperation du message dissimulé n'as pas réussit." << endl;
        }
        else if (messageDecode == "2")
        {
            cout << "Information, ce fichier ne possède pas de message dissimuler." << endl;
        }
        else if (messageDecode == "3")
        {
            cout << "Erreur, pas de balise correcte. message modifié ou inexistant." << endl;
        }
        else
        {
            cout << messageDecode << endl;
        }
    }
    else if (mode == "INTERACT")
    {
        // interactif
        try
        {
            if (argc > 2)
            {
                return runCommandLineMode(argc, argv);
            }

            return runInteractiveMode();
        }
        catch (const std::exception& ex)
        {
            std::cerr << "Erreur inattendue : " << ex.what() << std::endl;
        }
        catch (...)
        {
            std::cerr << "Erreur inattendue inconnue." << std::endl;
        }
    }
    return kFailure;
}
