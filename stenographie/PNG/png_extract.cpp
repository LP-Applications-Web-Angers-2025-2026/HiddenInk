#include "png_extract.h"
#include "../utils/stegano/stegano_text.hpp"
#include "../utils/stegano/stegano_imageinimage.hpp"
#include <iostream>
#include <fstream>

using namespace std;

int extractPNGText(const string& inputPath, const string& outputPath)
{
    int w, h, c;
    
    // Charger l'image
    auto img = loadImage(inputPath, w, h, c);
    if (!img)
    {
        cerr << "Erreur : impossible de charger l'image " << inputPath << endl;
        return EXIT_FAILURE;
    }
    
    // Extraire le texte
    string extractedText = extractTextFromImage(img, w, h, c);
    
    stbi_image_free(img);
    
    if (extractedText.empty())
    {
        cerr << "\n Aucun texte trouvé ou erreur d'extraction." << endl;
        return EXIT_FAILURE;
    }
    
    // Si un chemin de sortie est spécifié, sauvegarder dans un fichier
    if (!outputPath.empty())
    {
        ofstream outFile(outputPath);
        if (outFile)
        {
            outFile << extractedText;
            outFile.close();
            cout << "\n Message extrait et sauvegardé dans " << outputPath << endl;
        }
        else
        {
            cerr << "Erreur : impossible de créer le fichier " << outputPath << endl;
            return EXIT_FAILURE;
        }
    }
    else
    {
        // Afficher le texte extrait
        cout << "\n=== MESSAGE EXTRAIT ===\n";
        cout << extractedText << endl;
        cout << "=======================\n";
    }
    
    return EXIT_SUCCESS;
}

int extractPNGImage(const string& inputPath, const string& outputPath)
{
    int w, h, c;
    
    // Charger l'image
    auto img = loadImage(inputPath, w, h, c);
    if (!img)
    {
        cerr << "Erreur : impossible de charger l'image " << inputPath << endl;
        return EXIT_FAILURE;
    }
    
    // Extraire l'image cachée
    int extractedW = 0, extractedH = 0, extractedC = 0;
    int bitsPerChannel = 0; // Détection automatique
    auto extracted = extractImageFromImage(img, w, h, c, bitsPerChannel, extractedW, extractedH, extractedC);
    
    stbi_image_free(img);
    
    if (extracted.empty())
    {
        cerr << "\n Aucune image trouvée ou erreur d'extraction." << endl;
        return EXIT_FAILURE;
    }
    
    // Sauvegarder l'image extraite
    bool success = saveImage(outputPath, extracted.data(), extractedW, extractedH, extractedC);
    
    if (success)
    {
        cout << "\n Image extraite et sauvegardée dans " << outputPath << endl;
        cout << "   Dimensions : " << extractedW << "x" << extractedH << " (" << extractedC << " canaux)" << endl;
        return EXIT_SUCCESS;
    }
    else
    {
        cerr << "\n Erreur : échec de la sauvegarde." << endl;
        return EXIT_FAILURE;
    }
}
