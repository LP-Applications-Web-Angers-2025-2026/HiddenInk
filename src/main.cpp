#include <iostream>
#include <string>
#include "stegano/stegano_imageinimage.hpp"
#include "stegano/stegano_text.hpp"
#include "analysis/image_analysis.hpp"

void afficherAide() {
    std::cout << "=== STEGANOGRAPHIE AVANCEE ===\n\n";
    std::cout << "Usage:\n";
    std::cout << "  Mode interactif : ./main\n\n";
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

int main(int argc, char* argv[]) {
    // Mode ligne de commande
    if (argc > 1) {
        std::string mode = argv[1];
        
        if (mode == "help" || mode == "--help" || mode == "-h") {
            afficherAide();
            return 0;
        }
        
        // ===== MODE HIDE IMAGE =====
        if (mode == "hide-image" && argc >= 5) {
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
            
            std::vector<unsigned char> resizedBuffer;
            unsigned char* secretPtr = secret;
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
        
        // ===== MODE EXTRACT IMAGE =====
        if (mode == "extract-image" && argc >= 4) {
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
        
        // ===== MODE HIDE TEXT =====
        if (mode == "hide-text" && argc >= 5) {
            std::string carrierPath = argv[2];
            std::string message = argv[3];
            std::string outPath = argv[4];
            int bitsPerChannel = (argc >= 6) ? std::stoi(argv[5]) : 0; // 0 = auto
            
            std::cout << "=== CACHER UN MESSAGE TEXTE ===\n";
            std::cout << "Image porteuse : " << carrierPath << "\n";
            std::cout << "Message : \"" << message << "\"\n";
            std::cout << "Sortie : " << outPath << "\n\n";
            
            int w, h, c;
            auto carrier = loadImage(carrierPath, w, h, c);
            if (!carrier) return 1;
            
            auto encoded = hideTextInImage(carrier, w, h, c, message, bitsPerChannel);
            
            if (!encoded.empty() && saveImage(outPath, encoded.data(), w, h, c))
                std::cout << "\n✅ Message caché avec succès dans " << outPath << "\n";
            else
                std::cerr << "\n❌ Erreur : échec de l'encodage ou de la sauvegarde.\n";
            
            stbi_image_free(carrier);
            return 0;
        }
        
        // ===== MODE EXTRACT TEXT =====
        if (mode == "extract-text" && argc >= 3) {
            std::string inputPath = argv[2];
            std::string outputPath = (argc >= 4) ? argv[3] : ""; // Fichier .txt optionnel
            int bits = (argc >= 5) ? std::stoi(argv[4]) : 0; // 0 = auto
            
            std::cout << "=== EXTRAIRE UN MESSAGE TEXTE ===\n";
            std::cout << "Image source : " << inputPath << "\n";
            if (!outputPath.empty()) {
                std::cout << "Fichier de sortie : " << outputPath << "\n";
            }
            std::cout << "\n";
            
            int w, h, c;
            auto img = loadImage(inputPath, w, h, c);
            if (!img) return 1;
            
            std::string message = extractTextFromImage(img, w, h, c, bits);
            if (!message.empty()) {
                std::cout << "\n✅ MESSAGE EXTRAIT :\n";
                std::cout << "─────────────────────────────────────────\n";
                std::cout << message << "\n";
                std::cout << "─────────────────────────────────────────\n";
                
                // Sauvegarder dans un fichier si spécifié
                if (!outputPath.empty()) {
                    saveMessageToFile(message, outputPath);
                }
            } else {
                std::cerr << "\n❌ Erreur : impossible d'extraire le message.\n";
            }
            
            stbi_image_free(img);
            return 0;
        }
        
        // ===== MODE COMPARE =====
        if (mode == "compare" && argc >= 4) {
            std::string img1 = argv[2];
            std::string img2 = argv[3];
            compareImages(img1, img2);
            return 0;
        }
        
        // ===== MODE HISTOGRAM =====
        if (mode == "histogram" && argc >= 3) {
            std::string imgPath = argv[2];
            generateHistogram(imgPath);
            return 0;
        }
        
        // ===== MODE DETECT =====
        if (mode == "detect" && argc >= 3) {
            std::string imgPath = argv[2];
            analyzeImageForSteganography(imgPath);
            return 0;
        }
        
        // Arguments invalides
        std::cerr << "❌ Arguments invalides.\n";
        afficherAide();
        return 1;
    }
    
    // ===== MODE INTERACTIF =====
    int choix;
    std::cout << "=== STEGANOGRAPHIE AVANCEE ===\n\n";
    std::cout << "1. Cacher une image dans une image\n";
    std::cout << "2. Extraire une image cachée\n";
    std::cout << "3. Cacher un message texte dans une image\n";
    std::cout << "4. Extraire un message texte\n";
    std::cout << "5. Comparer deux images (MSE/PSNR)\n";
    std::cout << "6. Analyser l'histogramme d'une image\n";
    std::cout << "7. Détecter une éventuelle stéganographie\n";
    std::cout << "\nChoix : ";
    std::cin >> choix;
    std::cin.ignore();

    // ===== OPTION 1: CACHER UNE IMAGE =====
    if (choix == 1) {
        std::string carrierPath, secretPath, outPath;
        
        std::cout << "\n=== CACHER UNE IMAGE ===\n";
        std::cout << "Image porteuse : "; std::getline(std::cin, carrierPath);
        std::cout << "Image à cacher : "; std::getline(std::cin, secretPath);
        std::cout << "Image de sortie (.png) : "; std::getline(std::cin, outPath);

        int cw, ch, cc, sw, sh, sc;
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
    else if (choix == 2) {
        std::string inputPath, outPath;
        
        std::cout << "\n=== EXTRAIRE UNE IMAGE ===\n";
        std::cout << "Image contenant l'image cachée : "; std::getline(std::cin, inputPath);
        std::cout << "Nom de l'image extraite (.png) : "; std::getline(std::cin, outPath);

        int cw, ch, cc, w, h, c;
        auto carrier = loadImage(inputPath, cw, ch, cc);
        if (!carrier) return 1;

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

    // ===== OPTION 3: CACHER UN TEXTE =====
    else if (choix == 3) {
        std::string carrierPath, message, outPath;
        
        std::cout << "\n=== CACHER UN MESSAGE TEXTE ===\n";
        std::cout << "Image porteuse : "; std::getline(std::cin, carrierPath);
        std::cout << "Message à cacher : "; std::getline(std::cin, message);
        std::cout << "Image de sortie (.png) : "; std::getline(std::cin, outPath);

        int w, h, c;
        auto carrier = loadImage(carrierPath, w, h, c);
        if (!carrier) return 1;

        int bitsPerChannel = 0;
        auto encoded = hideTextInImage(carrier, w, h, c, message, bitsPerChannel);

        if (!encoded.empty() && saveImage(outPath, encoded.data(), w, h, c))
            std::cout << "\n✅ Message caché avec succès dans " << outPath << "\n";
        else
            std::cerr << "\n❌ Erreur : échec de l'encodage ou de la sauvegarde.\n";

        stbi_image_free(carrier);
    }

    // ===== OPTION 4: EXTRAIRE UN TEXTE =====
    else if (choix == 4) {
        std::string inputPath, outputPath;
        
        std::cout << "\n=== EXTRAIRE UN MESSAGE TEXTE ===\n";
        std::cout << "Image contenant le message : "; std::getline(std::cin, inputPath);
        std::cout << "Sauvegarder dans un fichier .txt ? (o/n) : ";
        char save;
        std::cin >> save;
        std::cin.ignore();
        
        if (save == 'o' || save == 'O') {
            std::cout << "Nom du fichier de sortie (.txt) : "; std::getline(std::cin, outputPath);
        }

        int w, h, c;
        auto img = loadImage(inputPath, w, h, c);
        if (!img) return 1;

        int bits = 0;
        std::string message = extractTextFromImage(img, w, h, c, bits);
        if (!message.empty()) {
            std::cout << "\n✅ MESSAGE EXTRAIT :\n";
            std::cout << "─────────────────────────────────────────\n";
            std::cout << message << "\n";
            std::cout << "─────────────────────────────────────────\n";
            
            // Sauvegarder si demandé
            if (!outputPath.empty()) {
                saveMessageToFile(message, outputPath);
            }
        } else {
            std::cerr << "\n❌ Erreur : impossible d'extraire le message.\n";
        }

        stbi_image_free(img);
    }

    // ===== OPTION 5: COMPARER DEUX IMAGES =====
    else if (choix == 5) {
        std::string img1, img2;
        
        std::cout << "\n=== COMPARER DEUX IMAGES ===\n";
        std::cout << "Première image (originale) : "; std::getline(std::cin, img1);
        std::cout << "Deuxième image (modifiée) : "; std::getline(std::cin, img2);

        compareImages(img1, img2);
    }

    // ===== OPTION 6: ANALYSER HISTOGRAMME =====
    else if (choix == 6) {
        std::string imgPath;
        
        std::cout << "\n=== ANALYSER L'HISTOGRAMME ===\n";
        std::cout << "Image à analyser : "; std::getline(std::cin, imgPath);

        generateHistogram(imgPath);
    }

    // ===== OPTION 7: DETECTER STEGANOGRAPHIE =====
    else if (choix == 7) {
        std::string imgPath;
        
        std::cout << "\n=== DETECTER STEGANOGRAPHIE ===\n";
        std::cout << "Image à analyser : "; std::getline(std::cin, imgPath);

        analyzeImageForSteganography(imgPath);
    }

    return 0;
}
