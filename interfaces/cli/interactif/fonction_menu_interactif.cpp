#include "fonction_menu_interactif.h"
#include "../../../stenographie/utils/stegano/stegano_imageinimage.hpp"
#include "../../../stenographie/utils/stegano/stegano_text.hpp"
#include "../../../stenographie/utils/analysis/image_analysis.hpp"
#include "../../../stenographie/utils/stegano/stegano_imageinimage.hpp"
#include "../../../stenographie/utils/stegano/stegano_text.hpp"
#include "../../../stenographie/BMP/bmp_convert.h"
#include "../../../stenographie/BMP/bmp_Recuperation.h"
#include "../../../stenographie/utils/utils_bin.h"
#include "../../../stenographie/utils/encrypt/encrypt.h"
#include <cstdlib>
#include <iostream>
#include <string>
#include <filesystem>
#include <chrono>
#include <sstream>
#include <iomanip>
#include <algorithm>

using namespace std;

/**
 * Cette fonction s'assure que le répertoire de sortie "out" existe.
 * Si ce n'est pas le cas, elle le crée avec tous les répertoires parents nécessaires.
 */
static void ensureOutDir()
{
    const std::filesystem::path outDir = "../out";
    if (!std::filesystem::exists(outDir))
    {
        std::filesystem::create_directories(outDir);
    }
}

/**
 * Génère un nom de fichier unique avec le préfix et l'extension spécifiés.
 * Le nom de fichier inclut la date et l'heure actuelles au format "YYYYMMDD_HHMMSS".
 * Le fichier sera créé dans le dossier "out".
 * @param prefix Le préfixe à utiliser pour le nom de fichier
 * @param ext L'extension du fichier (avec le point)
 * @return Le chemin complet du fichier généré
 */
static string generateUniqueFilename(const string& prefix, const string& ext)
{
    ensureOutDir();
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);

    std::stringstream ss;
    ss << "../out/" << prefix << "_" << std::put_time(std::localtime(&time), "%Y%m%d_%H%M%S") << ext;
    return ss.str();
}

/**
 * Obtient l'extension d'un fichier à partir de son chemin.
 * @param path Le chemin du fichier dont on veut extraire l'extension.
 * @return L'extension du fichier en minuscule (avec le point), ou ".png" si aucune extension trouvée.
 */
static string getExt(const string& path)
{
    size_t dotPos = path.find_last_of('.');
    if (dotPos == string::npos) return ".png";

    string ext = path.substr(dotPos);
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    return ext;
}

/**
 * Gère l'interface utilisateur pour cacher un texte dans une image.
 * Permet de:
 * - Choisir le format de l'image porteuse (PNG ou BMP)
 * - Sélectionner l'image porteuse et le texte à cacher
 * - Spécifier un nom de fichier de sortie (ou génération automatique)
 * - Optionnellement chiffrer le message avec une clé générée
 *
 * @return EXIT_SUCCESS si l'opération réussit, EXIT_FAILURE en cas d'erreur
 */
static int menuCacherTexte()
{
    int w, h, c;
    string carrierPath, message, outPath, key = "";
    char encryptChoice;

    cout << "    CACHER UN TEXTE DANS UNE IMAGE    \n";

    cout << "Choisissez le format de l'image porteuse :\n";
    cout << "  1. PNG (Recommandé - Qualité sans perte)\n";
    cout << "  2. BMP (Format brut)\n";
    cout << "\nChoix : ";

    int formatChoix;
    cin >> formatChoix;
    cin.ignore();

    string formatExt;
    switch (formatChoix)
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
    cout << "Fichier texte à cacher (.txt) : ";
    getline(cin, message);
    cout << "Nom du fichier de sortie (laisser vide pour auto) : ";
    getline(cin, outPath);
    cout << "Chiffrer le message ? (o/n) : ";
    cin >> encryptChoice;
    cin.ignore();

    if (encryptChoice == 'o' || encryptChoice == 'O')
    {
        key = generate_key(16);
        cout << "Clé (hex) : " << to_hex(key) << "\n";
    }

    if (outPath.empty())
    {
        outPath = generateUniqueFilename("texte_cache", formatExt); // change le nom du texte_cacher bof
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
        bmpConvert(carrierPath, message, outPath, 0, key); // bitPos = 0 par défaut
    }
    else
    {
        // Utiliser les fonctions PNG pour PNG/JPG
        auto carrier = loadImage(carrierPath, w, h, c);
        if (!carrier) return EXIT_FAILURE;

        int bitsPerChannel = 0;
        auto encoded = hideTextInImage(carrier, w, h, c, message, bitsPerChannel);

        if (!encoded.empty() && saveImage(outPath, encoded.data(), w, h, c))
            cout << "\n Message caché avec succès dans " << outPath << "\n";
        else
            cerr << "\n Erreur : échec de l'encodage ou de la sauvegarde.\n";

        stbi_image_free(carrier);
    }
    return EXIT_SUCCESS;
}

/**
 * Gère l'interface utilisateur pour cacher une image dans une autre image.
 * Permet de:
 * - Choisir le format de l'image porteuse (PNG ou BMP)
 * - Sélectionner l'image porteuse et l'image à cacher
 * - Spécifier un nom de fichier de sortie (ou génération automatique)
 * - Optionnellement chiffrer l'image avec une clé générée
 * - Sauvegarder le résultat dans le dossier "out"
 *
 * @return EXIT_SUCCESS si l'opération réussit, EXIT_FAILURE en cas d'erreur
 */
static int menuCacherImage()
{
    int cw, ch, cc, sw, sh, sc;
    string carrierPath, secretPath, outPath, key = "";
    char encryptChoice;


    cout << "   CACHER UNE IMAGE DANS UNE IMAGE    \n";

    cout << "Choisissez le format de l'image porteuse :\n";
    cout << "  1. PNG (Recommandé - Qualité sans perte)\n";
    cout << "  2. BMP (Format brut)\n";
    cout << "\nChoix : ";

    int formatChoix;
    cin >> formatChoix;
    cin.ignore();

    string formatExt;
    switch (formatChoix)
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
    cout << "Chiffrer le message ? (o/n) : ";
    cin >> encryptChoice;
    cin.ignore();

    if (encryptChoice == 'o' || encryptChoice == 'O')
    {
        key = generate_key(16);
        cout << "Clé (hex) : " << to_hex(key) << "\n";
    }

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
        bmpConvert(carrierPath, secretPath, outPath, 0, key); // bitPos = 0 par défaut
    }
    else
    {
        // Utiliser les fonctions PNG pour PNG/JPG
        auto carrier = loadImage(carrierPath, cw, ch, cc);
        auto secret = loadImage(secretPath, sw, sh, sc);

        if (!carrier || !secret) return EXIT_FAILURE;

        std::vector<unsigned char> resizedBuffer;
        unsigned char* secretPtr = secret;
        calculateOptimalSize(cw, ch, cc, sw, sh, sc, secretPtr, resizedBuffer);

        int bitsPerChannel = 0;
        auto encoded = hideImageInImage(carrier, cw, ch, cc, secretPtr, sw, sh, sc, bitsPerChannel);

        if (!encoded.empty() && saveImage(outPath, encoded.data(), cw, ch, cc))
            cout << "\n Image cachée avec succès dans " << outPath << "\n";
        else
            cerr << "\n Erreur : échec de l'encodage ou de la sauvegarde.\n";

        stbi_image_free(carrier);
        stbi_image_free(secret);
    }
    return EXIT_SUCCESS;
}

/**
 * Gère l'interface utilisateur pour extraire des données (texte ou image) d'une image.
 * Permet de:
 * - Choisir le type de données à extraire (texte ou image)
 * - Sélectionner l'image source contenant les données cachées
 * - Optionnellement sauvegarder l'extraction dans un fichier
 * - Spécifier un nom de fichier de sortie (ou génération automatique)
 * - Déchiffrer les données si une clé a été utilisée lors du cachage
 *
 * @return EXIT_SUCCESS si l'opération réussit, EXIT_FAILURE en cas d'erreur
 */
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
            // Utiliser les fonctions PNG pour PNG/JPG
            auto img = loadImage(inputPath, w, h, c);
            if (!img) return EXIT_FAILURE;

            int bits = 0;
            message = extractTextFromImage(img, w, h, c, bits);
            if (!message.empty())
            {
                cout << "\n MESSAGE EXTRAIT :\n";

                cout << message << "\n";


                if (!outputPath.empty())
                {
                    saveMessageToFile(message, outputPath);
                }
            }
            else
            {
                cerr << "\n Erreur : impossible d'extraire le message.\n";
            }

            stbi_image_free(img);
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
            // Utiliser les fonctions PNG pour PNG/JPG
            auto carrier = loadImage(inputPath, cw, ch, cc);
            if (!carrier) return EXIT_FAILURE;

            int bits = 0;
            auto secret = extractImageFromImage(carrier, cw, ch, cc, bits, w, h, c);
            if (!secret.empty())
            {
                saveImage(outPath, secret.data(), w, h, c);
                cout << "\n Image extraite avec succès -> " << outPath << "\n";
            }
            else
            {
                cerr << "\n Erreur : impossible d'extraire l'image cachée.\n";
            }

            stbi_image_free(carrier);
        }
    }
    else
    {
        cerr << " Choix invalide.\n";
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

/**
 * Gère l'interface utilisateur pour analyser une image.
 * Permet de:
 * - Calculer le MSE (Mean Squared Error)
 * - Calculer le PSNR (Peak Signal-to-Noise Ratio)
 * - Comparer deux images (MSE + PSNR)
 * - Analyser l'histogramme d'une image
 * - Détecter une stéganographie dans une image
 *
 * @return EXIT_SUCCESS si l'opération réussit, EXIT_FAILURE en cas d'erreur
 */
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

    switch (analyseChoix)
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

/**
 * Traite le choix de l'utilisateur dans le menu principal et appelle la fonction correspondante.
 * @param choix Le numéro de l'option choisie par l'utilisateur (1-4)
 * @return EXIT_SUCCESS si l'opération réussit, EXIT_FAILURE en cas d'erreur
 */
int fonction_menu(int choix)
{
    // Menu hiérarchique avec sous-menus
    switch (choix)
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
