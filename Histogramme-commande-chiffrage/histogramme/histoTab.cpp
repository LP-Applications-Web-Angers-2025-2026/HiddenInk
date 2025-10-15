#include <iostream>
#include <fstream>
#include <vector>
#include <bitset>
#include <string>

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

//créer un tableau histogramme des valeurs de pixel
vector<int> histogramme(const vector<unsigned char>& imageData, size_t headerSize) {
    vector<int> histo(256, 0);
    for (size_t i = headerSize; i < imageData.size(); ++i) {
        histo[imageData[i]]++;
    }
    return histo;
}

int main(int argc, char* argv[]){
    if (argc != 2) {
        cerr << "Usage: " << argv[0] << " <input_bmp>" << endl;
        return 1;
    }
    size_t headerSize = 54; // Taille de l'entête BMP
    string inputPath = argv[1];
    vector<unsigned char> data = LireBMP(inputPath);
    if (data.empty()) return 1;

    cout << "Histogramme des valeurs de pixel :" << endl;
    vector<int> histo = histogramme(data, headerSize);

    for(int i = 0; i < histo.size(); i++) {
        cout << i << ": " << " (" << histo[i] << ")" << endl;
    }
    return 0;
}
