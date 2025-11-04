#ifndef HIDDENINK_UTILS_BIN_H
#define HIDDENINK_UTILS_BIN_H

#include <iostream>
#include <vector>

using namespace std;

string BinForLetter(char letter);
string BinForString(string message);
string binaireVersTexte(const string& binaire);
string BinForFile(const string& filePath);

string getSignature();
size_t getSignatureSize();
string getSignatureBinary();
size_t getSignatureBinarySize();
string getBalise(bool ouverture);
size_t getBaliseSize();
string getBaliseBinary(bool ouverture);
size_t getBaliseBinarySize();
string lireFichierKey(string filetohide, string key);

void afficherAide();

bool supportedFile(const string& filePath);
bool messageCanFitInImage(const string& messageBinaire, const vector<unsigned char>& data, size_t headerSize);
bool VerifFichier(string fichier);


#endif //HIDDENINK_UTILS_BIN_H