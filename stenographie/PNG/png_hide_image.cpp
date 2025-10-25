#include "png_hide_image.h"
#include "../utils/stegano/stegano_imageinimage.hpp"
#include <iostream>
#include <vector>

using namespace std;

int hidePNGImage(const string& carrierPath, const string& secretPath, 
                 const string& outputPath, int bitsPerChannel)
{
    int cw, ch, cc, sw, sh, sc;
    
    // Charger l'image porteuse
    auto carrier = loadImage(carrierPath, cw, ch, cc);
    if (!carrier)
    {
        cerr << "Erreur : impossible de charger l'image porteuse " << carrierPath << endl;
        return EXIT_FAILURE;
    }
    
    // Charger l'image secrète
    auto secret = loadImage(secretPath, sw, sh, sc);
    if (!secret)
    {
        cerr << "Erreur : impossible de charger l'image secrète " << secretPath << endl;
        stbi_image_free(carrier);
        return EXIT_FAILURE;
    }
    
    // Redimensionner si nécessaire
    vector<unsigned char> resizedBuffer;
    unsigned char* secretPtr = secret;
    
    if (!calculateOptimalSize(cw, ch, cc, sw, sh, sc, secretPtr, resizedBuffer))
    {
        cerr << "Erreur : impossible de calculer la taille optimale" << endl;
        stbi_image_free(carrier);
        stbi_image_free(secret);
        return EXIT_FAILURE;
    }
    
    // Cacher l'image dans l'image porteuse
    auto encoded = hideImageInImage(carrier, cw, ch, cc, secretPtr, sw, sh, sc, bitsPerChannel);
    
    // Sauvegarder l'image encodée
    bool success = false;
    if (!encoded.empty())
    {
        success = saveImage(outputPath, encoded.data(), cw, ch, cc);
    }
    
    stbi_image_free(carrier);
    stbi_image_free(secret);
    
    if (success)
    {
        cout << "\n Image cachée avec succès dans " << outputPath << endl;
        return EXIT_SUCCESS;
    }
    else
    {
        cerr << "\n Erreur : échec de l'encodage ou de la sauvegarde." << endl;
        return EXIT_FAILURE;
    }
}
