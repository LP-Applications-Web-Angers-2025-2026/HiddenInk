#include "png_hide_text.h"
#include "../utils/stegano/stegano_text.hpp"
#include "../utils/stegano/stegano_imageinimage.hpp"
#include <iostream>

using namespace std;

int hidePNGText(const string& carrierPath, const string& textOrFilePath, 
                const string& outputPath, int bitsPerChannel)
{
    int w, h, c;
    
    // Charger l'image porteuse
    auto carrier = loadImage(carrierPath, w, h, c);
    if (!carrier)
    {
        cerr << "Erreur : impossible de charger l'image porteuse " << carrierPath << endl;
        return EXIT_FAILURE;
    }
    
    // Cacher le texte dans l'image
    auto encoded = hideTextInImage(carrier, w, h, c, textOrFilePath, bitsPerChannel);
    
    // Sauvegarder l'image encodée
    bool success = false;
    if (!encoded.empty())
    {
        success = saveImage(outputPath, encoded.data(), w, h, c);
    }
    
    stbi_image_free(carrier);
    
    if (success)
    {
        cout << "\n✓ Message caché avec succès dans " << outputPath << endl;
        return EXIT_SUCCESS;
    }
    else
    {
        cerr << "\n✗ Erreur : échec de l'encodage ou de la sauvegarde." << endl;
        return EXIT_FAILURE;
    }
}
