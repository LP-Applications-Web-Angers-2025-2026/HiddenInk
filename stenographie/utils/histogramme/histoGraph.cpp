#include <iostream>
#include <fstream>
#include <vector>
#include <bitset>
#include <string>
#include <algorithm>

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

// Sauvegarde un histogramme dans une image BMP
void saveHistogramBMP(const vector<int>& histo, const string& filename, int width = 800, int height = 400) {
    int maxVal = *max_element(histo.begin(), histo.end());
    if (maxVal == 0) maxVal = 1;

    int barWidth = width / histo.size();
    vector<unsigned char> img(width * height * 3, 255); // fond blanc

    // Tracer chaque barre
    for (size_t i = 0; i < histo.size(); ++i) {
        int barHeight = static_cast<int>((double)histo[i] / maxVal * (height - 10));
        for (int y = 0; y < barHeight; ++y) {
            for (int x = 0; x < barWidth - 1; ++x) {
                int px = i * barWidth + x;
                int py = height - 1 - y;
                int idx = (py * width + px) * 3;
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

int main(int argc, char* argv[]) {
    if (argc != 3) {
        cerr << "Usage: " << argv[0] << " <input_bmp>" << endl;
        return 1;
    }

    size_t headerSize = 54; // Taille de l'en-tête BMP
    string inputPath = argv[1];
    string outpout = argv[2];
    vector<unsigned char> data = LireBMP(inputPath);
    if (data.empty()) return 1;

    // Calcul histogramme
    vector<int> histo = histogramme(data, headerSize);

    // Affichage console
    cout << "Histogramme des valeurs de pixel :" << endl;
    for (int i = 0; i < 256; ++i) {
        cout << i << ": " << histo[i] << endl;
    }

    // Génération image BMP de l’histogramme
    saveHistogramBMP(histo, outpout);

    return 0;
}