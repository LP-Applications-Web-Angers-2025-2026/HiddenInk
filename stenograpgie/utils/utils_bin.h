#ifndef HIDDENINK_UTILS_BIN_H
#define HIDDENINK_UTILS_BIN_H.

#include <iostream>

using namespace std;

string BinForLetter(char letter);
string binaireVersTexte(const string& binaire);

string getSignature();
size_t getSignatureSize();
string getSignatureBinary();
size_t getSignatureBinarySize();


#endif //HIDDENINK_UTILS_BIN_H