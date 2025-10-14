#ifndef HIDDENINK_UTILS_BIN_H
#define HIDDENINK_UTILS_BIN_H.

#include <iostream>

using namespace std;

string BinForLetter(char letter);
string BinForString(string message);
string binaireVersTexte(const string& binaire);

string getSignature();
size_t getSignatureSize();
string getSignatureBinary();
size_t getSignatureBinarySize();

string getBalise(bool ouverture);
size_t getBaliseSize();
string getBaliseBinary(bool ouverture);
size_t getBaliseBinarySize();


#endif //HIDDENINK_UTILS_BIN_H