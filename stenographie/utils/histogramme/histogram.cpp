#include "histogram.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <bitset>
#include <string>
#include <algorithm>
#include <filesystem>

using namespace std;

// Lire un fichier binaire et retourner les octets
vector<unsigned char> LireBMP(const string& chemin) {
    ifstream file(chemin, ios::binary);
    if (!file) {
        cerr << "Erreur : impossible d'ouvrir " << chemin << endl;
        return {};
    }
    vector<unsigned char> data((istreambuf_iterator<char>(file)),
                                istreambuf_iterator<char>());
    file.close();
    cout << "Fichier lu : " << data.size() << " octets.\n";
    return data;
}

// Créer un tableau histogramme des valeurs de pixel
vector<int> histogramme(const vector<unsigned char>& imageData, size_t headerSize) {
    vector<int> histo(256, 0);
    for (size_t i = headerSize; i < imageData.size(); ++i) {
        histo[imageData[i]]++;
    }
    return histo;
}

// Afficher l'histogramme en console
void printHistogram(const vector<int>& histo) {
    cout << "Histogramme des valeurs de pixel :" << endl;
    for (int i = 0; i < histo.size(); i++) {
        cout << i << ": " << " (" << histo[i] << ")" << endl;
    }
}

// Sauvegarde un histogramme dans une image BMP
void saveHistogramBMP(const vector<int>& histo, const string& filename, int width, int height) {
    int maxVal = *max_element(histo.begin(), histo.end());
    if (maxVal == 0) maxVal = 1;

    int barWidth = width / histo.size();
    vector<unsigned char> img(width * height * 3, 255); // fond blanc

    // Tracer chaque barre
    for (size_t i = 0; i < histo.size(); ++i) {
        int barHeight = static_cast<int>((double)histo[i] / maxVal * (height - 10));
        for (int y = 0; y < barHeight; ++y) {
            for (int x = 0; x < barWidth - 1; ++x) {
                /*int px = i * barWidth + x;
                int py = height - 1 - y;
                int idx = (py * width + px) * 3; //ou */
                int idx = ((height - 1 - y) * width + (i * barWidth + x)) * 3;
                img[idx + 0] = 0;   // Bleu
                img[idx + 1] = 0;   // Vert
                img[idx + 2] = 255; // Rouge (donc bleu vif)
            }
        }
    }

    // Création de l'entête BMP
    ofstream out(filename, ios::binary);
    int fileSize = 54 + img.size();
    unsigned char header[54] = {
        'B','M',
        (unsigned char)(fileSize), (unsigned char)(fileSize >> 8),
        (unsigned char)(fileSize >> 16), (unsigned char)(fileSize >> 24),
        0,0, 0,0, 54,0,0,0,
        40,0,0,0,
        (unsigned char)(width), (unsigned char)(width >> 8),
        (unsigned char)(width >> 16), (unsigned char)(width >> 24),
        (unsigned char)(height), (unsigned char)(height >> 8),
        (unsigned char)(height >> 16), (unsigned char)(height >> 24),
        1,0, 24,0
    };
    out.write((char*)header, 54);

    // Les lignes BMP sont stockées à l'envers
    for (int y = height - 1; y >= 0; --y)
        out.write((char*)&img[y * width * 3], width * 3);

    out.close();
    cout << "✅ Histogramme graphique sauvegardé dans : " << filename << endl;
}

// Fonction principale pour traiter l'histogramme
void processHistogram(const string& inputPath) {
    size_t headerSize = 54; // Taille de l'entête BMP
    vector<unsigned char> data = LireBMP(inputPath);
    if (data.empty()) return;

    vector<int> histo = histogramme(data, headerSize);

    // Afficher l'histogramme en console
    printHistogram(histo);

    // Générer et sauvegarder l'image graphique dans out/
    filesystem::path inputPathObj(inputPath); // chemin d'entrée
    string baseName = inputPathObj.stem().string();
    string outputDir = "out/"; // chemin de sortie
    filesystem::create_directories(outputDir);
    string outputPath = outputDir + baseName + "_histogram.bmp"; // chemin de sortie avec fichier
    saveHistogramBMP(histo, outputPath);
}
