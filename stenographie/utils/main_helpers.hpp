#pragma once
#include <filesystem>
#include <iostream>
#include <limits>
#include <stdexcept>
#include <string>
#include <vector>
#include <chrono>
#include <ctime>
#include <sstream>
#include <iomanip>
#include "../BMP/bmp_convert.h"
#include "../BMP/bmp_Recuperation.h"
#include "../PNG/fonction_menu.h"
#include "utils_bin.h"
#include "analysis/image_analysis.hpp"
#include "stegano/stegano_imageinimage.hpp"
#include "stegano/stegano_text.hpp"

namespace MainHelpers
{
using std::cerr;
using std::cout;
using std::endl;
using std::string;

constexpr int kSuccess = 0;
constexpr int kFailure = 1;

// Fonction pour créer le dossier out s'il n'existe pas
inline void ensureOutDirectoryExists()
{
    const std::filesystem::path outDir = "../out";
    if (!std::filesystem::exists(outDir))
    {
        std::filesystem::create_directories(outDir);
        cout << " Création du dossier 'out'..." << endl;
    }
}

// Fonction pour générer un nom de fichier de sortie avec timestamp
inline string generateOutputPath(const string& prefix, const string& extension)
{
    ensureOutDirectoryExists();
    
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    
    std::stringstream ss;
    ss << "../out/" << prefix << "_" << std::put_time(std::localtime(&time), "%Y%m%d_%H%M%S") << extension;
    
    return ss.str();
}

// Fonction pour extraire l'extension d'un fichier
inline string getFileExtension(const string& filepath)
{
    size_t dotPos = filepath.find_last_of('.');
    if (dotPos == string::npos) return "";
    
    string ext = filepath.substr(dotPos);
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    return ext;
}

// Fonction pour détecter le format optimal de sortie selon les entrées
inline string detectOptimalOutputFormat(const string& carrierPath, const string& secretPath = "")
{
    string carrierExt = getFileExtension(carrierPath);
    
    // Si c'est du BMP, on peut rester en BMP
    if (carrierExt == ".bmp")
    {
        return ".bmp";
    }
    
    // Pour tout le reste, on utilise PNG (meilleur pour la stéganographie)
    return ".png";
}

inline void reportBmpDecodeResult(const string& decoded)
{
    if (decoded == "1")
    {
        cerr << "Erreur, le processus de récupération du message dissimulé n'a pas réussi." << endl;
    }
    else if (decoded == "2")
    {
        cout << "Information, ce fichier ne possède pas de message dissimulé." << endl;
    }
    else if (decoded == "3")
    {
        cerr << "Erreur, pas de balise correcte. Message modifié ou inexistant." << endl;
    }
    else
    {
        cout << decoded << endl;
    }
}

inline int runBmpDemo()
{
    const string sourcePath = "../img_banque/BMP/test.bmp";
    if (!std::filesystem::exists(sourcePath))
    {
        cerr << "Erreur, le fichier " << sourcePath << " n'existe pas." << endl;
        return kFailure;
    }

    bmpConvert(sourcePath);

    const string outputPath = "../out/tigre_LSB.bmp";
    const string decoded = bmpRecup(outputPath);
    reportBmpDecodeResult(decoded);

    return (decoded == "1" || decoded == "3") ? kFailure : kSuccess;
}

inline int runHideImageMode(const string& carrierPath, const string& secretPath, const string& outPath, int bitsPerChannel)
{
    // Générer automatiquement le chemin de sortie si vide
    string finalOutPath = outPath;
    if (finalOutPath.empty() || finalOutPath == "auto")
    {
        string ext = detectOptimalOutputFormat(carrierPath, secretPath);
        finalOutPath = generateOutputPath("image_cachee", ext);
    }
    else
    {
        // S'assurer que le fichier va dans le dossier out
        if (finalOutPath.find("../out/") != 0 && finalOutPath.find("..\\out\\") != 0)
        {
            string ext = getFileExtension(finalOutPath);
            if (ext.empty()) ext = ".png";
            size_t lastSlash = finalOutPath.find_last_of("/\\");
            string filename = (lastSlash != string::npos) ? finalOutPath.substr(lastSlash + 1) : finalOutPath;
            finalOutPath = "../out/" + filename;
        }
    }
    
    cout << "=== CACHER UNE IMAGE ===\n";
    cout << "Image porteuse : " << carrierPath << endl;
    cout << "Image secrète : " << secretPath << endl;
    cout << "Sortie : " << finalOutPath << endl;
    if (bitsPerChannel == 0)
    {
        cout << "Mode : Automatique (calcul du nombre de bits optimal)" << endl << endl;
    }
    else
    {
        cout << "Bits par canal : " << bitsPerChannel << endl << endl;
    }

    int cw = 0, ch = 0, cc = 0, sw = 0, sh = 0, sc = 0;
    unsigned char* carrier = loadImage(carrierPath, cw, ch, cc);
    unsigned char* secret = loadImage(secretPath, sw, sh, sc);

    if (!carrier || !secret)
    {
        if (carrier) stbi_image_free(carrier);
        if (secret) stbi_image_free(secret);
        return kFailure;
    }

    std::vector<unsigned char> resizedBuffer;
    unsigned char* secretPtr = secret;

    if (!calculateOptimalSize(cw, ch, cc, sw, sh, sc, secretPtr, resizedBuffer))
    {
        stbi_image_free(carrier);
        stbi_image_free(secret);
        return kFailure;
    }

    const auto encoded = hideImageInImage(carrier, cw, ch, cc, secretPtr, sw, sh, sc, bitsPerChannel);

    stbi_image_free(carrier);
    stbi_image_free(secret);

    if (!encoded.empty() && saveImage(finalOutPath, encoded.data(), cw, ch, cc))
    {
        cout << "\n Image cachée avec succès dans " << finalOutPath << endl;
        return kSuccess;
    }

    cerr << "\n Erreur : échec de l'encodage ou de la sauvegarde." << endl;
    return kFailure;
}

inline int runExtractImageMode(const string& inputPath, const string& outPath, int bitsPerChannel)
{
    // Générer automatiquement le chemin de sortie si vide
    string finalOutPath = outPath;
    if (finalOutPath.empty() || finalOutPath == "auto")
    {
        finalOutPath = generateOutputPath("image_extraite", ".png");
    }
    else
    {
        // S'assurer que le fichier va dans le dossier out
        if (finalOutPath.find("../out/") != 0 && finalOutPath.find("..\\out\\") != 0)
        {
            string ext = getFileExtension(finalOutPath);
            if (ext.empty()) ext = ".png";
            size_t lastSlash = finalOutPath.find_last_of("/\\");
            string filename = (lastSlash != string::npos) ? finalOutPath.substr(lastSlash + 1) : finalOutPath;
            finalOutPath = "../out/" + filename;
        }
    }
    
    cout << "=== EXTRAIRE UNE IMAGE ===\n";
    cout << "Image source : " << inputPath << endl;
    cout << "Sortie : " << finalOutPath << endl;
    if (bitsPerChannel == 0)
    {
        cout << "Mode : Détection automatique" << endl << endl;
    }
    else
    {
        cout << "Bits par canal : " << bitsPerChannel << endl << endl;
    }

    int cw = 0, ch = 0, cc = 0, outW = 0, outH = 0, outC = 0;
    unsigned char* carrier = loadImage(inputPath, cw, ch, cc);
    if (!carrier)
    {
        return kFailure;
    }

    const auto secret = extractImageFromImage(carrier, cw, ch, cc, bitsPerChannel, outW, outH, outC);
    stbi_image_free(carrier);

    if (!secret.empty() && saveImage(finalOutPath, secret.data(), outW, outH, outC))
    {
        cout << "\n Image extraite avec succès -> " << finalOutPath << endl;
        return kSuccess;
    }

    cerr << "\n Erreur : impossible d'extraire l'image cachée." << endl;
    return kFailure;
}

inline int runHideTextMode(const string& carrierPath, const string& message, const string& outPath, int bitsPerChannel)
{
    // Générer automatiquement le chemin de sortie si vide
    string finalOutPath = outPath;
    if (finalOutPath.empty() || finalOutPath == "auto")
    {
        string ext = detectOptimalOutputFormat(carrierPath);
        finalOutPath = generateOutputPath("texte_cache", ext);
    }
    else
    {
        // S'assurer que le fichier va dans le dossier out
        if (finalOutPath.find("../out/") != 0 && finalOutPath.find("..\\out\\") != 0)
        {
            string ext = getFileExtension(finalOutPath);
            if (ext.empty()) ext = ".png";
            size_t lastSlash = finalOutPath.find_last_of("/\\");
            string filename = (lastSlash != string::npos) ? finalOutPath.substr(lastSlash + 1) : finalOutPath;
            finalOutPath = "../out/" + filename;
        }
    }
    
    cout << "=== CACHER UN MESSAGE TEXTE ===\n";
    cout << "Image porteuse : " << carrierPath << endl;
    cout << "Message : \"" << message << "\"" << endl;
    cout << "Sortie : " << finalOutPath << endl << endl;

    int w = 0, h = 0, c = 0;
    unsigned char* carrier = loadImage(carrierPath, w, h, c);
    if (!carrier)
    {
        return kFailure;
    }

    const auto encoded = hideTextInImage(carrier, w, h, c, message, bitsPerChannel);
    stbi_image_free(carrier);

    if (!encoded.empty() && saveImage(finalOutPath, encoded.data(), w, h, c))
    {
        cout << "\n Message caché avec succès dans " << finalOutPath << endl;
        return kSuccess;
    }

    cerr << "\n Erreur : échec de l'encodage ou de la sauvegarde." << endl;
    return kFailure;
}

inline int runExtractTextMode(const string& inputPath, const string& outputPath, int bitsPerChannel)
{
    // Générer automatiquement le chemin de sortie si demandé
    string finalOutputPath = outputPath;
    if (!finalOutputPath.empty() && finalOutputPath != "auto")
    {
        // S'assurer que le fichier .txt va dans le dossier out
        if (finalOutputPath.find("../out/") != 0 && finalOutputPath.find("..\\out\\") != 0)
        {
            string ext = getFileExtension(finalOutputPath);
            if (ext.empty()) ext = ".txt";
            size_t lastSlash = finalOutputPath.find_last_of("/\\");
            string filename = (lastSlash != string::npos) ? finalOutputPath.substr(lastSlash + 1) : finalOutputPath;
            finalOutputPath = "../out/" + filename;
        }
    }
    else if (finalOutputPath == "auto")
    {
        finalOutputPath = generateOutputPath("message_extrait", ".txt");
    }
    
    cout << "=== EXTRAIRE UN MESSAGE TEXTE ===\n";
    cout << "Image source : " << inputPath << endl;
    if (!finalOutputPath.empty())
    {
        cout << "Fichier de sortie : " << finalOutputPath << endl;
    }
    cout << endl;

    int w = 0, h = 0, c = 0;
    unsigned char* img = loadImage(inputPath, w, h, c);
    if (!img)
    {
        return kFailure;
    }

    const string message = extractTextFromImage(img, w, h, c, bitsPerChannel);
    stbi_image_free(img);

    if (message.empty())
    {
        cerr << "\n Erreur : impossible d'extraire le message." << endl;
        return kFailure;
    }

    cout << "\n MESSAGE EXTRAIT :\n";
    cout << "─────────────────────────────────────────\n";
    cout << message << endl;
    cout << "─────────────────────────────────────────\n";

    if (!finalOutputPath.empty())
    {
        saveMessageToFile(message, finalOutputPath);
    }

    return kSuccess;
}

inline int runCompareMode(const string& img1, const string& img2)
{
    compareImages(img1, img2);
    return kSuccess;
}

inline int runHistogramMode(const string& imgPath)
{
    generateHistogram(imgPath);
    return kSuccess;
}

inline int runDetectMode(const string& imgPath)
{
    analyzeImageForSteganography(imgPath);
    return kSuccess;
}

inline int runCommandLineMode(int argc, char* argv[])
{
    const string mode = argv[1];

    try
    {
        if (mode == "help" || mode == "--help" || mode == "-h")
        {
            afficherAide();
            return kSuccess;
        }

        if (mode == "hide-image" && argc >= 5)
        {
            const int bitsPerChannel = (argc >= 6) ? std::stoi(argv[5]) : 0;
            return runHideImageMode(argv[2], argv[3], argv[4], bitsPerChannel);
        }

        if (mode == "extract-image" && argc >= 4)
        {
            const int bitsPerChannel = (argc >= 5) ? std::stoi(argv[4]) : 0;
            return runExtractImageMode(argv[2], argv[3], bitsPerChannel);
        }

        if (mode == "hide-text" && argc >= 5)
        {
            const int bitsPerChannel = (argc >= 6) ? std::stoi(argv[5]) : 0;
            return runHideTextMode(argv[2], argv[3], argv[4], bitsPerChannel);
        }

        if (mode == "extract-text" && argc >= 3)
        {
            const string outputPath = (argc >= 4) ? argv[3] : "";
            const int bitsPerChannel = (argc >= 5) ? std::stoi(argv[4]) : 0;
            return runExtractTextMode(argv[2], outputPath, bitsPerChannel);
        }

        if (mode == "compare" && argc >= 4)
        {
            return runCompareMode(argv[2], argv[3]);
        }

        if (mode == "histogram" && argc >= 3)
        {
            return runHistogramMode(argv[2]);
        }

        if (mode == "detect" && argc >= 3)
        {
            return runDetectMode(argv[2]);
        }

        if (mode == "demo")
        {
            return runBmpDemo();
        }

        cerr << " Arguments invalides." << endl;
        afficherAide();
    }
    catch (const std::invalid_argument&)
    {
        cerr << " Erreur : argument numérique invalide." << endl;
        return kFailure;
    }
    catch (const std::out_of_range&)
    {
        cerr << " Erreur : valeur numérique hors limite." << endl;
        return kFailure;
    }

    return kFailure;
}

inline int runInteractiveMode()
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
        return kFailure;
    }

    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    return fonction_menu(choix);
}

} // namespace MainHelpers
