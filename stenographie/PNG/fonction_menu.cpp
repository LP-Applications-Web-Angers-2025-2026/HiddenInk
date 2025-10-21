// Created by perod on 14/10/2025.
//

#include "fonction_menu.h"
#include "png_hide_text.h"
#include "png_hide_image.h"
#include "png_extract.h"
#include "../utils/analysis/image_analysis.hpp"
#include "../BMP/bmp_convert.h"
#include "../BMP/bmp_Recuperation.h"
#include "../utils/utils_bin.h"
#include "../utils/encrypt/encrypt.h"
#include <cstdlib>
#include <iostream>
#include <string>
#include <filesystem>
#include <chrono>
#include <sstream>
#include <iomanip>
#include <algorithm>

using namespace std;

// Fonction helper pour créer le dossier out
static void ensureOutDir()
{
    const std::filesystem::path outDir = "../out";
    if (!std::filesystem::exists(outDir))
    {
        std::filesystem::create_directories(outDir);
    }
}

// Fonction helper pour générer un nom de fichier unique
static string generateUniqueFilename(const string& prefix, const string& ext)
{
    ensureOutDir();
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);

    std::stringstream ss;
    ss << "../out/" << prefix << "_" << std::put_time(std::localtime(&time), "%Y%m%d_%H%M%S") << ext;
    return ss.str();
}

// Fonction helper pour extraire l'extension
static string getExt(const string& path)
{
    size_t dotPos = path.find_last_of('.');
    if (dotPos == string::npos) return ".png";

    string ext = path.substr(dotPos);
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    return ext;
}

// Fonction pour gérer le sous-menu "Cacher un texte"
static int menuCacherTexte()
{
    int w, h, c;
    string carrierPath, message, outPath;

    cout << "    CACHER UN TEXTE DANS UNE IMAGE    \n";

    cout << "Choisissez le format de l'image porteuse :\n";
    cout << "  1. PNG (Recommandé - Qualité sans perte)\n";
    cout << "  2. BMP (Format brut)\n";
    cout << "\nChoix : ";

    int formatChoix;
    cin >> formatChoix;
    cin.ignore();

    string formatExt;
    switch(formatChoix)
    {
        case 1: formatExt = ".png"; break;
    case 2: formatExt = ".bmp";
        break;
    default:
            cerr << " Choix invalide.\n";
            return EXIT_FAILURE;
    }

    cout << "\nImage porteuse : ";
    getline(cin, carrierPath);
    cout << "Fichier texte à cacher (.txt) : ";
    getline(cin, message);
    cout << "Nom du fichier de sortie (laisser vide pour auto) : ";
    getline(cin, outPath);

    if (outPath.empty())
    {
        outPath = generateUniqueFilename("texte_cache", formatExt);
        cout << " Sauvegarde automatique : " << outPath << "\n";
    }
    else if (outPath.find("out/") != 0)
    {
        string ext = getExt(outPath);
        if (ext.empty()) ext = formatExt;
        size_t lastSlash = outPath.find_last_of("/\\");
        string filename = (lastSlash != string::npos) ? outPath.substr(lastSlash + 1) : outPath;
        outPath = "../out/" + filename;
        cout << " Redirection vers : " << outPath << "\n";
    }

    if (formatExt == ".bmp")
    {
        // Utiliser bmp_convert pour BMP
        bmpConvert(carrierPath, message, outPath, 0); // bitPos = 0 par défaut
        return EXIT_SUCCESS;
    }
    else
    {
        // Appeler la fonction PNG dédiée
        return hidePNGText(carrierPath, message, outPath, 0);
    }
}

// Fonction pour gérer le sous-menu "Cacher une image"
static int menuCacherImage()
{
    int cw, ch, cc, sw, sh, sc;
    string carrierPath, secretPath, outPath;


    cout << "   CACHER UNE IMAGE DANS UNE IMAGE    \n";

    cout << "Choisissez le format de l'image porteuse :\n";
    cout << "  1. PNG (Recommandé - Qualité sans perte)\n";
    cout << "  2. BMP (Format brut)\n";
    cout << "\nChoix : ";

    int formatChoix;
    cin >> formatChoix;
    cin.ignore();

    string formatExt;
    switch(formatChoix)
    {
        case 1: formatExt = ".png";
        break;
    case 2: formatExt = ".bmp";
        break;
        default:
            cerr << " Choix invalide.\n";
            return EXIT_FAILURE;
    }

    cout << "\nImage porteuse : ";
    getline(cin, carrierPath);
    cout << "Image à cacher : ";
    getline(cin, secretPath);
    cout << "Nom du fichier de sortie (laisser vide pour auto) : ";
    getline(cin, outPath);

    if (outPath.empty())
    {
        outPath = generateUniqueFilename("image_cachee", formatExt);
        cout << " Sauvegarde automatique : " << outPath << "\n";
    }
    else if (outPath.find("out/") != 0)
    {
        string ext = getExt(outPath);
        if (ext.empty()) ext = formatExt;
        size_t lastSlash = outPath.find_last_of("/\\");
        string filename = (lastSlash != string::npos) ? outPath.substr(lastSlash + 1) : outPath;
        outPath = "../out/" + filename;
        cout << " Redirection vers : " << outPath << "\n";
    }

    if (formatExt == ".bmp")
    {
        // Pour BMP, utiliser bmp_convert avec l'image secrète comme "fichier à cacher"
        bmpConvert(carrierPath, secretPath, outPath, 0); // bitPos = 0 par défaut
        return EXIT_SUCCESS;
    }
    else
    {
        // Appeler la fonction PNG dédiée
        return hidePNGImage(carrierPath, secretPath, outPath, 0);
    }
}

// Fonction pour gérer le sous-menu "Extraire"
static int menuExtraire()
{
    int w, h, c, cw, ch, cc;
    string inputPath, outPath, outputPath, message;

    cout << "          EXTRAIRE DES DONNÉES         \n";

    cout << "Que voulez-vous extraire ?\n";
    cout << "  1. Extraire un texte\n";
    cout << "  2. Extraire une image\n";
    cout << "\nChoix : ";

    int extractChoix;
    cin >> extractChoix;
    cin.ignore();

    if (extractChoix == 1)
    {
        // Extraire texte
        cout << "\n=== EXTRAIRE UN MESSAGE TEXTE ===\n";
        cout << "Image contenant le message : ";
        getline(cin, inputPath);
        cout << "Sauvegarder dans un fichier .txt ? (o/n) : ";
        char save;
        cin >> save;
        cin.ignore();

        if (save == 'o' || save == 'O')
        {
            cout << "Nom du fichier de sortie (laisser vide pour auto) : ";
            getline(cin, outputPath);

            if (outputPath.empty())
            {
                outputPath = generateUniqueFilename("message_extrait", ".txt");
                cout << " Sauvegarde automatique : " << outputPath << "\n";
            }
            else if (outputPath.find("out/") != 0)
            {
                string ext = getExt(outputPath);
                if (ext.empty()) ext = ".txt";
                size_t lastSlash = outputPath.find_last_of("/\\");
                string filename = (lastSlash != string::npos) ? outputPath.substr(lastSlash + 1) : outputPath;
                outputPath = "../out/" + filename;
                cout << " Redirection vers : " << outputPath << "\n";
            }
        }

        // Déterminer le format de l'image
        string ext = getExt(inputPath);
        if (ext == ".bmp")
        {
            // Utiliser bmpRecup pour BMP
            string key;
            cout << "Clé (hex) utilisée lors du cachage : ";
            getline(cin, key);
            string result = bmpRecup(inputPath, 0, key); // bitPos = 0 par défaut
            cout << result << "\n";

            // Le message est affiché par bmpRecup(), et le fichier est créé
            // On ne fait qu'afficher un message de confirmation pour la sauvegarde
            if (!outputPath.empty())
            {
                cout << "💾 Message sauvegardé dans : " << outputPath << "\n";
            }
        }
        else
        {
            // Appeler la fonction PNG dédiée pour extraire le texte
            extractPNGText(inputPath, outputPath);
        }
    }
    else if (extractChoix == 2)
    {
        // Extraire image
        cout << "\n=== EXTRAIRE UNE IMAGE ===\n";
        cout << "Image contenant l'image cachée : ";
        getline(cin, inputPath);
        cout << "Nom de l'image extraite (laisser vide pour auto) : ";
        getline(cin, outPath);

        if (outPath.empty())
        {
            outPath = generateUniqueFilename("image_extraite", ".png");
            cout << " Sauvegarde automatique : " << outPath << "\n";
        }
        else if (outPath.find("out/") != 0)
        {
            string ext = getExt(outPath);
            if (ext.empty()) ext = ".png";
            size_t lastSlash = outPath.find_last_of("/\\");
            string filename = (lastSlash != string::npos) ? outPath.substr(lastSlash + 1) : outPath;
            outPath = "../out/" + filename;
            cout << " Redirection vers : " << outPath << "\n";
        }

        // Déterminer le format de l'image
        string ext = getExt(inputPath);
        if (ext == ".bmp")
        {
            // Pour BMP, utiliser bmpRecup qui gère les images cachées
            string key;
            cout << "Clé (hex) utilisée lors du cachage (laisser vide si aucune) : ";
            getline(cin, key);
            string result = bmpRecup(inputPath, 0, key); // bitPos = 0 par défaut
            cout << result << "\n";
        }
        else
        {
            // Appeler la fonction PNG dédiée pour extraire l'image
            extractPNGImage(inputPath, outPath);
        }
    }
    else
    {
        cerr << " Choix invalide.\n";
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

// Fonction pour gérer le sous-menu "Analyser image"
static int menuAnalyserImage()
{
    string img1, img2, imgPath;


    cout << "        ANALYSER UNE IMAGE             \n";
    cout << "Quel type d'analyse voulez-vous effectuer ?\n";
    cout << "  1. MSE (Mean Squared Error)\n";
    cout << "  2. PSNR (Peak Signal-to-Noise Ratio)\n";
    cout << "  3. Comparer deux images (MSE + PSNR)\n";
    cout << "  4. Analyser l'histogramme\n";
    cout << "  5. Détecter une stéganographie\n";
    cout << "\nChoix : ";

    int analyseChoix;
    cin >> analyseChoix;
    cin.ignore();

    switch(analyseChoix)
    {
        case 1:
        case 2:
        case 3:
            cout << "\n=== COMPARER DEUX IMAGES ===\n";
            cout << "Première image (originale) : ";
            getline(cin, img1);
            cout << "Deuxième image (modifiée) : ";
            getline(cin, img2);
            compareImages(img1, img2);
            break;

        case 4:
            cout << "\n=== ANALYSER L'HISTOGRAMME ===\n";
            cout << "Image à analyser : ";
            getline(cin, imgPath);
            generateHistogram(imgPath);
            break;

        case 5:
            cout << "\n=== DETECTER STEGANOGRAPHIE ===\n";
            cout << "Image à analyser : ";
            getline(cin, imgPath);
            analyzeImageForSteganography(imgPath);
            break;

        default:
            cerr << " Choix invalide.\n";
            return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

int fonction_menu(int choix)
{
    // Menu hiérarchique avec sous-menus
    switch(choix)
    {
        case 1:
            return menuCacherTexte();

        case 2:
            return menuCacherImage();

        case 3:
            return menuExtraire();

        case 4:
            return menuAnalyserImage();

        default:
            std::cerr << " Choix invalide.\n";
            return EXIT_FAILURE;
    }
}

