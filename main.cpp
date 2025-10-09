#include <iostream>
#include <string>
#include "stegano_imageinimage.hpp"

int main() {
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

        // Calculer la capacité maximale
        size_t maxBits = (size_t)cw * ch * cc * 8;
        size_t maxBytes = maxBits / 8;
        std::cout << "\n💾 Capacité maximale de l'image porteuse : "
                  << maxBytes / 1024 << " Ko\n";

        // ✅ bitsPerChannel = 0 → mode automatique
        auto encoded = hideImageInImage(carrier, cw, ch, cc, secret, sw, sh, sc, 0);

        if (!encoded.empty() && saveImage(outPath, encoded.data(), cw, ch, cc))
            std::cout << "✅ Image cachée avec succès dans " << outPath << "\n";
        else
            std::cerr << "❌ Erreur : échec de l'encodage ou de la sauvegarde.\n";

        stbi_image_free(carrier);
        stbi_image_free(secret);
    }

    else if (choix == 2) {
        std::string inputPath, outPath;
        int bits;
        std::cout << "Image contenant l'image cachée : "; std::getline(std::cin, inputPath);
        std::cout << "Bits par canal utilisés (1–8) : "; std::cin >> bits;
        std::cout << "Nom de l'image extraite (.png) : "; std::cin.ignore(); std::getline(std::cin, outPath);

        int cw, ch, cc, w, h, c;
        auto carrier = loadImage(inputPath, cw, ch, cc);
        if (!carrier) return 1;

        auto secret = extractImageFromImage(carrier, cw, ch, cc, bits, w, h, c);
        if (!secret.empty()) {
            saveImage(outPath, secret.data(), w, h, c);
            std::cout << "✅ Image extraite avec succès -> " << outPath << "\n";
        } else {
            std::cerr << "❌ Erreur : impossible d'extraire l'image cachée.\n";
        }

        stbi_image_free(carrier);
    }

    return 0;
}
