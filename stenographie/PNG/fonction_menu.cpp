//
// Created by perod on 14/10/2025.
//

#include "fonction_menu.h"
#include "../utils/analysis/image_analysis.hpp"
#include "../utils/stegano/stegano_imageinimage.hpp"
#include "../utils/stegano/stegano_text.hpp"
#include <iostream>
#include <string>

using namespace std;

int fonction_menu(int choix)
{
    int w, h, c, cw, ch, cc, sw, sh, sc;

    std::string imgPath, inputPath, outputPath, carrierPath, secretPath, outPath, message, img1, img2;

    // ===== OPTION 1: CACHER UNE IMAGE =====
    if (choix == 1)
    {
        //std::string carrierPath, secretPath, outPath;

        std::cout << "\n=== CACHER UNE IMAGE ===\n";
        std::cout << "Image porteuse : ";
        std::getline(std::cin, carrierPath);
        std::cout << "Image à cacher : ";
        std::getline(std::cin, secretPath);
        std::cout << "Image de sortie (.png) : ";
        std::getline(std::cin, outPath);

        //int cw, ch, cc, sw, sh, sc;
        auto carrier = loadImage(carrierPath, cw, ch, cc);
        auto secret = loadImage(secretPath, sw, sh, sc);

        if (!carrier || !secret) return 1;

        std::vector<unsigned char> resizedBuffer;
        unsigned char* secretPtr = secret;
        calculateOptimalSize(cw, ch, cc, sw, sh, sc, secretPtr, resizedBuffer);

        int bitsPerChannel = 0;
        auto encoded = hideImageInImage(carrier, cw, ch, cc, secretPtr, sw, sh, sc, bitsPerChannel);

        if (!encoded.empty() && saveImage(outPath, encoded.data(), cw, ch, cc))
            std::cout << "\n✅ Image cachée avec succès dans " << outPath << "\n";
        else
            std::cerr << "\n❌ Erreur : échec de l'encodage ou de la sauvegarde.\n";

        stbi_image_free(carrier);
        stbi_image_free(secret);
    }

    // ===== OPTION 2: EXTRAIRE UNE IMAGE =====
    else if (choix == 2)
    {
        //std::string inputPath, outPath;
        //int cw, ch, cc, w, h, c;

        std::cout << "\n=== EXTRAIRE UNE IMAGE ===\n";
        std::cout << "Image contenant l'image cachée : ";
        std::getline(std::cin, inputPath);
        std::cout << "Nom de l'image extraite (.png) : ";
        std::getline(std::cin, outPath);

        auto carrier = loadImage(inputPath, cw, ch, cc);
        if (!carrier) return 2;

        int bits = 0;
        auto secret = extractImageFromImage(carrier, cw, ch, cc, bits, w, h, c);
        if (!secret.empty())
        {
            saveImage(outPath, secret.data(), w, h, c);
            std::cout << "\n✅ Image extraite avec succès -> " << outPath << "\n";
        }
        else
        {
            std::cerr << "\n❌ Erreur : impossible d'extraire l'image cachée.\n";
        }

        stbi_image_free(carrier);
    }

    // ===== OPTION 3: CACHER UN TEXTE =====
    else if (choix == 3)
    {
        //std::string carrierPath, message, outPath;

        std::cout << "\n=== CACHER UN MESSAGE TEXTE ===\n";
        std::cout << "Image porteuse : ";
        std::getline(std::cin, carrierPath);
        std::cout << "Message à cacher : ";
        std::getline(std::cin, message);
        std::cout << "Image de sortie (.png) : ";
        std::getline(std::cin, outPath);

        auto carrier = loadImage(carrierPath, w, h, c);
        if (!carrier) return 3;

        int bitsPerChannel = 0;
        auto encoded = hideTextInImage(carrier, w, h, c, message, bitsPerChannel);

        if (!encoded.empty() && saveImage(outPath, encoded.data(), w, h, c))
            std::cout << "\n✅ Message caché avec succès dans " << outPath << "\n";
        else
            std::cerr << "\n❌ Erreur : échec de l'encodage ou de la sauvegarde.\n";

        stbi_image_free(carrier);
    }

    // ===== OPTION 4: EXTRAIRE UN TEXTE =====
    else if (choix == 4)
    {
        //int w, h, c;
        //std::string inputPath, outputPath;

        std::cout << "\n=== EXTRAIRE UN MESSAGE TEXTE ===\n";
        std::cout << "Image contenant le message : ";
        std::getline(std::cin, inputPath);
        std::cout << "Sauvegarder dans un fichier .txt ? (o/n) : ";
        char save;
        std::cin >> save;
        std::cin.ignore();

        if (save == 'o' || save == 'O')
        {
            std::cout << "Nom du fichier de sortie (.txt) : ";
            std::getline(std::cin, outputPath);
        }

        auto img = loadImage(inputPath, w, h, c);
        if (!img) return 4;

        int bits = 0;
        message = extractTextFromImage(img, w, h, c, bits);
        if (!message.empty())
        {
            std::cout << "\n✅ MESSAGE EXTRAIT :\n";
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
            std::cerr << "\n❌ Erreur : impossible d'extraire le message.\n";
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
    return 1;
}
