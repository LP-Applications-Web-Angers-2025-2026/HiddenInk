#ifndef HIDDENINK_UTILS_BIN_H
#define HIDDENINK_UTILS_BIN_H

#include <iostream>

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
bool supportedFile(const string& filePath);

string lireFichier(string filetohide, bool chiffrer);
void afficherAide();


#endif //HIDDENINK_UTILS_BIN_H