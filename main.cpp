#include <iostream>
#include <string>
#include "stegano_imageinimage.hpp"

void afficherAide() {
    std::cout << "=== IMAGE-IN-IMAGE STEGANOGRAPHIE ===\n\n";
    std::cout << "Usage:\n";
    std::cout << "  Mode interactif : ./main\n";
    std::cout << "  Cacher une image : ./main hide <image_porteuse> <image_secrete> <sortie.png>\n";
    std::cout << "  Extraire une image : ./main extract <image_avec_secret> <sortie.png>\n\n";
    std::cout << "Exemples:\n";
    std::cout << "  ./main hide carrier.png secret.png output.png\n";
    std::cout << "  ./main extract output.png extracted.png\n\n";
    std::cout << "Note: Le nombre de bits par canal est détecté automatiquement.\n";
}

int main(int argc, char* argv[]) {
    // Mode ligne de commande
    if (argc > 1) {
        std::string mode = argv[1];
        
        if (mode == "help" || mode == "--help" || mode == "-h") {
            afficherAide();
            return 0;
        }
        
        // Mode HIDE : cacher une image
        if (mode == "hide" && argc >= 5) {
            std::string carrierPath = argv[2];
            std::string secretPath = argv[3];
            std::string outPath = argv[4];
            int bitsPerChannel = (argc >= 6) ? std::stoi(argv[5]) : 0; // 0 = auto
            
            std::cout << "=== CACHER UNE IMAGE ===\n";
            std::cout << "Image porteuse : " << carrierPath << "\n";
            std::cout << "Image secrète : " << secretPath << "\n";
            std::cout << "Sortie : " << outPath << "\n";
            if (bitsPerChannel == 0) {
                std::cout << "Mode : Automatique (calcul du nombre de bits optimal)\n\n";
            } else {
                std::cout << "Bits par canal : " << bitsPerChannel << "\n\n";
            }
            
            int cw, ch, cc, sw, sh, sc;
            auto carrier = loadImage(carrierPath, cw, ch, cc);
            auto secret = loadImage(secretPath, sw, sh, sc);
            
            if (!carrier || !secret) return 1;
            
            // Buffer pour l'image redimensionnée
            std::vector<unsigned char> resizedBuffer;
            unsigned char* secretPtr = secret;
            
            // Calculer et redimensionner si nécessaire
            calculateOptimalSize(cw, ch, cc, sw, sh, sc, secretPtr, resizedBuffer);
            
            auto encoded = hideImageInImage(carrier, cw, ch, cc, secretPtr, sw, sh, sc, bitsPerChannel);
            
            if (!encoded.empty() && saveImage(outPath, encoded.data(), cw, ch, cc))
                std::cout << "\n✅ Image cachée avec succès dans " << outPath << "\n";
            else
                std::cerr << "\n❌ Erreur : échec de l'encodage ou de la sauvegarde.\n";
            
            stbi_image_free(carrier);
            stbi_image_free(secret);
            return 0;
        }
        
        // Mode EXTRACT : extraire une image
        if (mode == "extract" && argc >= 4) {
            std::string inputPath = argv[2];
            std::string outPath = argv[3];
            int bits = (argc >= 5) ? std::stoi(argv[4]) : 0; // 0 = auto
            
            std::cout << "=== EXTRAIRE UNE IMAGE ===\n";
            std::cout << "Image source : " << inputPath << "\n";
            std::cout << "Sortie : " << outPath << "\n";
            if (bits == 0) {
                std::cout << "Mode : Détection automatique\n\n";
            } else {
                std::cout << "Bits par canal : " << bits << "\n\n";
            }
            
            int cw, ch, cc, w, h, c;
            auto carrier = loadImage(inputPath, cw, ch, cc);
            if (!carrier) return 1;
            
            auto secret = extractImageFromImage(carrier, cw, ch, cc, bits, w, h, c);
            if (!secret.empty()) {
                saveImage(outPath, secret.data(), w, h, c);
                std::cout << "\n✅ Image extraite avec succès -> " << outPath << "\n";
            } else {
                std::cerr << "\n❌ Erreur : impossible d'extraire l'image cachée.\n";
            }
            
            stbi_image_free(carrier);
            return 0;
        }
        
        // Arguments invalides
        std::cerr << "❌ Arguments invalides.\n";
        afficherAide();
        return 1;
    }
    
    // Mode interactif (sans arguments)
    int choix;
    std::cout << "=== IMAGE-IN-IMAGE STEGANOGRAPHIE ===\n";
    std::cout << "1. Cacher une image\n2. Extraire une image\nChoix : ";
    std::cin >> choix;
    std::cin.ignore();

    if (choix == 1) {
        std::string carrierPath, secretPath, outPath;
        
        std::cout << "Image porteuse : "; std::getline(std::cin, carrierPath);
        std::cout << "Image à cacher : "; std::getline(std::cin, secretPath);
        std::cout << "Image de sortie (.png) : "; std::getline(std::cin, outPath);

        int cw, ch, cc, sw, sh, sc;
        auto carrier = loadImage(carrierPath, cw, ch, cc);
        auto secret = loadImage(secretPath, sw, sh, sc);

        if (!carrier || !secret) return 1;

        // Buffer pour l'image redimensionnée
        std::vector<unsigned char> resizedBuffer;
        unsigned char* secretPtr = secret;
        
        // Calculer et redimensionner si nécessaire
        calculateOptimalSize(cw, ch, cc, sw, sh, sc, secretPtr, resizedBuffer);

        // Mode automatique : utiliser 0 pour calculer automatiquement le nombre de bits nécessaire
        int bitsPerChannel = 0;
        auto encoded = hideImageInImage(carrier, cw, ch, cc, secretPtr, sw, sh, sc, bitsPerChannel);

        if (!encoded.empty() && saveImage(outPath, encoded.data(), cw, ch, cc))
            std::cout << "\n✅ Image cachée avec succès dans " << outPath << "\n";
        else
            std::cerr << "\n❌ Erreur : échec de l'encodage ou de la sauvegarde.\n";

        stbi_image_free(carrier);
        stbi_image_free(secret);
    }

    else if (choix == 2) {
        std::string inputPath, outPath;
        
        std::cout << "Image contenant l'image cachée : "; std::getline(std::cin, inputPath);
        std::cout << "Nom de l'image extraite (.png) : "; std::getline(std::cin, outPath);

        int cw, ch, cc, w, h, c;
        auto carrier = loadImage(inputPath, cw, ch, cc);
        if (!carrier) return 1;

        // Mode automatique : 0 = détection automatique du nombre de bits
        int bits = 0;
        auto secret = extractImageFromImage(carrier, cw, ch, cc, bits, w, h, c);
        if (!secret.empty()) {
            saveImage(outPath, secret.data(), w, h, c);
            std::cout << "\n✅ Image extraite avec succès -> " << outPath << "\n";
        } else {
            std::cerr << "\n❌ Erreur : impossible d'extraire l'image cachée.\n";
        }

        stbi_image_free(carrier);
    }

    return 0;
}
