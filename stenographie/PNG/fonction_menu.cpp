//
// Created by perod on 14/10/2025.
//

#include "fonction_menu.h"
#include "../utils/analysis/image_analysis.hpp"
#include "../utils/stegano/stegano_imageinimage.hpp"
#include "../utils/stegano/stegano_text.hpp"
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
    cout << "  2. JPG (Compression avec perte)\n";
    cout << "  3. BMP (Format brut)\n";
    cout << "\nChoix : ";
    
    int formatChoix;
    cin >> formatChoix;
    cin.ignore();
    
    string formatExt;
    switch(formatChoix)
    {
        case 1: formatExt = ".png"; break;
        case 2: formatExt = ".jpg"; break;
        case 3: formatExt = ".bmp"; break;
        default:
            cerr << " Choix invalide.\n";
            return EXIT_FAILURE;
    }
    
    cout << "\nImage porteuse : ";
    getline(cin, carrierPath);
    cout << "Message à cacher : ";
    getline(cin, message);
    cout << "Nom du fichier de sortie (laisser vide pour auto) : ";
    getline(cin, outPath);
    
    if (outPath.empty())
    {
        outPath = generateUniqueFilename("texte_cache", formatExt);
        cout << " Sauvegarde automatique : " << outPath << "\n";
    }
    else if (outPath.find("../out/") != 0)
    {
        string ext = getExt(outPath);
        if (ext.empty()) ext = formatExt;
        size_t lastSlash = outPath.find_last_of("/\\");
        string filename = (lastSlash != string::npos) ? outPath.substr(lastSlash + 1) : outPath;
        outPath = "../out/" + filename;
        cout << " Redirection vers : " << outPath << "\n";
    }
    
    auto carrier = loadImage(carrierPath, w, h, c);
    if (!carrier) return EXIT_FAILURE;
    
    int bitsPerChannel = 0;
    auto encoded = hideTextInImage(carrier, w, h, c, message, bitsPerChannel);
    
    if (!encoded.empty() && saveImage(outPath, encoded.data(), w, h, c))
        cout << "\n Message caché avec succès dans " << outPath << "\n";
    else
        cerr << "\n Erreur : échec de l'encodage ou de la sauvegarde.\n";
    
    stbi_image_free(carrier);
    return EXIT_SUCCESS;
}

// Fonction pour gérer le sous-menu "Cacher une image"
static int menuCacherImage()
{
    int cw, ch, cc, sw, sh, sc;
    string carrierPath, secretPath, outPath;
    

    cout << "   CACHER UNE IMAGE DANS UNE IMAGE    \n";

    cout << "Choisissez le format de l'image porteuse :\n";
    cout << "  1. PNG (Recommandé - Qualité sans perte)\n";
    cout << "  2. JPG (Compression avec perte)\n";
    cout << "  3. BMP (Format brut)\n";
    cout << "\nChoix : ";
    
    int formatChoix;
    cin >> formatChoix;
    cin.ignore();
    
    string formatExt;
    switch(formatChoix)
    {
        case 1: formatExt = ".png"; break;
        case 2: formatExt = ".jpg"; break;
        case 3: formatExt = ".bmp"; break;
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
    else if (outPath.find("../out/") != 0)
    {
        string ext = getExt(outPath);
        if (ext.empty()) ext = formatExt;
        size_t lastSlash = outPath.find_last_of("/\\");
        string filename = (lastSlash != string::npos) ? outPath.substr(lastSlash + 1) : outPath;
        outPath = "../out/" + filename;
        cout << " Redirection vers : " << outPath << "\n";
    }
    
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
    return EXIT_SUCCESS;
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
            else if (outputPath.find("../out/") != 0)
            {
                string ext = getExt(outputPath);
                if (ext.empty()) ext = ".txt";
                size_t lastSlash = outputPath.find_last_of("/\\");
                string filename = (lastSlash != string::npos) ? outputPath.substr(lastSlash + 1) : outputPath;
                outputPath = "../out/" + filename;
                cout << " Redirection vers : " << outputPath << "\n";
            }
        }
        
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
        else if (outPath.find("../out/") != 0)
        {
            string ext = getExt(outPath);
            if (ext.empty()) ext = ".png";
            size_t lastSlash = outPath.find_last_of("/\\");
            string filename = (lastSlash != string::npos) ? outPath.substr(lastSlash + 1) : outPath;
            outPath = "../out/" + filename;
            cout << " Redirection vers : " << outPath << "\n";
        }
        
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

// ========== CODE LEGACY (à conserver pour référence) ==========
// Ancienne implémentation du menu avec 7 options
/*
int fonction_menu_old(int choix)
{
    int w, h, c, cw, ch, cc, sw, sh, sc;

    std::string imgPath, inputPath, outputPath, carrierPath, secretPath, outPath, message, img1, img2;

    // ===== OPTION 1: CACHER UNE IMAGE =====
    if (choix == 1)
    {
        std::cout << "\n=== CACHER UNE IMAGE ===\n";
        std::cout << "Image porteuse : ";
        std::getline(std::cin, carrierPath);
        std::cout << "Image à cacher : ";
        std::getline(std::cin, secretPath);
        std::cout << "Nom du fichier de sortie (laisser vide pour auto) : ";
        std::getline(std::cin, outPath);
        
        // Générer un nom automatique si vide
        if (outPath.empty())
        {
            string ext = (getExt(carrierPath) == ".bmp") ? ".bmp" : ".png";
            outPath = generateUniqueFilename("image_cachee", ext);
            std::cout << " Sauvegarde automatique : " << outPath << "\n";
        }
        else if (outPath.find("../out/") != 0)
        {
            string ext = getExt(outPath);
            if (ext.empty()) ext = ".png";
            size_t lastSlash = outPath.find_last_of("/\\");
            string filename = (lastSlash != string::npos) ? outPath.substr(lastSlash + 1) : outPath;
            outPath = "../out/" + filename;
            std::cout << " Redirection vers : " << outPath << "\n";
        }

        auto carrier = loadImage(carrierPath, cw, ch, cc);
        auto secret = loadImage(secretPath, sw, sh, sc);

        if (!carrier || !secret) return EXIT_FAILURE;

        std::vector<unsigned char> resizedBuffer;
        unsigned char* secretPtr = secret;
        calculateOptimalSize(cw, ch, cc, sw, sh, sc, secretPtr, resizedBuffer);

        int bitsPerChannel = 0;
        auto encoded = hideImageInImage(carrier, cw, ch, cc, secretPtr, sw, sh, sc, bitsPerChannel);

        if (!encoded.empty() && saveImage(outPath, encoded.data(), cw, ch, cc))
            std::cout << "\n Image cachée avec succès dans " << outPath << "\n";
        else
            std::cerr << "\n Erreur : échec de l'encodage ou de la sauvegarde.\n";

        stbi_image_free(carrier);
        stbi_image_free(secret);
    }

    // ===== OPTION 2: EXTRAIRE UNE IMAGE =====
    else if (choix == 2)
    {
        std::cout << "\n=== EXTRAIRE UNE IMAGE ===\n";
        std::cout << "Image contenant l'image cachée : ";
        std::getline(std::cin, inputPath);
        std::cout << "Nom de l'image extraite (laisser vide pour auto) : ";
        std::getline(std::cin, outPath);
        
        // Générer un nom automatique si vide
        if (outPath.empty())
        {
            outPath = generateUniqueFilename("image_extraite", ".png");
            std::cout << "Sauvegarde automatique : " << outPath << "\n";
        }
        else if (outPath.find("../out/") != 0)
        {
            string ext = getExt(outPath);
            if (ext.empty()) ext = ".png";
            size_t lastSlash = outPath.find_last_of("/\\");
            string filename = (lastSlash != string::npos) ? outPath.substr(lastSlash + 1) : outPath;
            outPath = "../out/" + filename;
            std::cout << " Redirection vers : " << outPath << "\n";
        }

        auto carrier = loadImage(inputPath, cw, ch, cc);
    if (!carrier) return EXIT_FAILURE;

        int bits = 0;
        auto secret = extractImageFromImage(carrier, cw, ch, cc, bits, w, h, c);
        if (!secret.empty())
        {
            saveImage(outPath, secret.data(), w, h, c);
            std::cout << "\n Image extraite avec succès -> " << outPath << "\n";
        }
        else
        {
            std::cerr << "\n Erreur : impossible d'extraire l'image cachée.\n";
        }

        stbi_image_free(carrier);
    }

    // ===== OPTION 3: CACHER UN TEXTE =====
    else if (choix == 3)
    {
        std::cout << "\n=== CACHER UN MESSAGE TEXTE ===\n";
        std::cout << "Image porteuse : ";
        std::getline(std::cin, carrierPath);
        std::cout << "Message à cacher : ";
        std::getline(std::cin, message);
        std::cout << "Nom du fichier de sortie (laisser vide pour auto) : ";
        std::getline(std::cin, outPath);
        
        // Générer un nom automatique si vide
        if (outPath.empty())
        {
            string ext = (getExt(carrierPath) == ".bmp") ? ".bmp" : ".png";
            outPath = generateUniqueFilename("texte_cache", ext);
            std::cout << " Sauvegarde automatique : " << outPath << "\n";
        }
        else if (outPath.find("../out/") != 0)
        {
            string ext = getExt(outPath);
            if (ext.empty()) ext = ".png";
            size_t lastSlash = outPath.find_last_of("/\\");
            string filename = (lastSlash != string::npos) ? outPath.substr(lastSlash + 1) : outPath;
            outPath = "../out/" + filename;
            std::cout << " Redirection vers : " << outPath << "\n";
        }

        auto carrier = loadImage(carrierPath, w, h, c);
        if (!carrier) return EXIT_FAILURE;

        int bitsPerChannel = 0;
        auto encoded = hideTextInImage(carrier, w, h, c, message, bitsPerChannel);

        if (!encoded.empty() && saveImage(outPath, encoded.data(), w, h, c))
            std::cout << "\n Message caché avec succès dans " << outPath << "\n";
        else
            std::cerr << "\n Erreur : échec de l'encodage ou de la sauvegarde.\n";

        stbi_image_free(carrier);
    }

    // ===== OPTION 4: EXTRAIRE UN TEXTE =====
    else if (choix == 4)
    {
        std::cout << "\n=== EXTRAIRE UN MESSAGE TEXTE ===\n";
        std::cout << "Image contenant le message : ";
        std::getline(std::cin, inputPath);
        std::cout << "Sauvegarder dans un fichier .txt ? (o/n) : ";
        char save;
        std::cin >> save;
        std::cin.ignore();

        if (save == 'o' || save == 'O')
        {
            std::cout << "Nom du fichier de sortie (laisser vide pour auto) : ";
            std::getline(std::cin, outputPath);
            
            // Générer un nom automatique si vide
            if (outputPath.empty())
            {
                outputPath = generateUniqueFilename("message_extrait", ".txt");
                std::cout << " Sauvegarde automatique : " << outputPath << "\n";
            }
            else if (outputPath.find("../out/") != 0)
            {
                string ext = getExt(outputPath);
                if (ext.empty()) ext = ".txt";
                size_t lastSlash = outputPath.find_last_of("/\\");
                string filename = (lastSlash != string::npos) ? outputPath.substr(lastSlash + 1) : outputPath;
                outputPath = "../out/" + filename;
                std::cout << " Redirection vers : " << outputPath << "\n";
            }
        }

        auto img = loadImage(inputPath, w, h, c);
        if (!img) return EXIT_FAILURE;

        int bits = 0;
        message = extractTextFromImage(img, w, h, c, bits);
        if (!message.empty())
        {
            std::cout << "\n MESSAGE EXTRAIT :\n";
            std::cout << "─────────────────────────────────────────\n";
            std::cout << message << "\n";
            std::cout << "─────────────────────────────────────────\n";

            // Sauvegarder si demandé
            if (!outputPath.empty())
            {
                saveMessageToFile(message, outputPath);
            }
        }
        else
        {
            std::cerr << "\n Erreur : impossible d'extraire le message.\n";
        }

        stbi_image_free(img);
    }

    // ===== OPTION 5: COMPARER DEUX IMAGES =====
    else if (choix == 5)
    {
        //std::string img1, img2;

        std::cout << "\n=== COMPARER DEUX IMAGES ===\n";
        std::cout << "Première image (originale) : ";
        std::getline(std::cin, img1);
        std::cout << "Deuxième image (modifiée) : ";
        std::getline(std::cin, img2);

        compareImages(img1, img2);
    }

    // ===== OPTION 6: ANALYSER HISTOGRAMME =====
    else if (choix == 6)
    {
        //std::string imgPath;

        std::cout << "\n=== ANALYSER L'HISTOGRAMME ===\n";
        std::cout << "Image à analyser : ";
        std::getline(std::cin, imgPath);

        generateHistogram(imgPath);
    }

    // ===== OPTION 7: DETECTER STEGANOGRAPHIE =====
    else if (choix == 7)
    {
        //std::string imgPath;

        std::cout << "\n=== DETECTER STEGANOGRAPHIE ===\n";
        std::cout << "Image à analyser : ";
        std::getline(std::cin, imgPath);

        analyzeImageForSteganography(imgPath);
    }
    return EXIT_SUCCESS;
}
*/
// ========== FIN CODE LEGACY ==========
