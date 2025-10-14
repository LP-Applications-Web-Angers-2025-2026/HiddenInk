#include <filesystem>
#include <iostream>
#include <fstream>
#include <ostream>
#include "stenograpgie/BMP/bmp_convert.h"
#include "stenograpgie/BMP/bmp_Recuperation.h"
#include "stenograpgie/utils/utils_bin.h"

using namespace std;

int main()
{
    string messageDecode;
    string pathFile = "../img_banque/PNG/test.png";

    if (pathFile == "" || !std::filesystem::exists(pathFile))
    {
        cout << "Erreur, le fichier n'existe pas." << endl;
    }
    else
    {
        bmpConvert(pathFile);
    }

    string path = "../out/tigre_LSB.bmp";

    messageDecode = bmpRecup(path);

     if (messageDecode == "1") {
        cout << "Erreur, le processus de récuperation du message dissimulé n'as pas réussit." << endl;
     }else if (messageDecode == "2") {
         cout << "Information, ce fichier ne possède pas de message dissimuler." << endl;
     } else if (messageDecode == "3") {
         cout << "Erreur, pas de balise correcte. message modifié ou inexistant."<< endl;
     } else {
        cout << messageDecode << endl;
     }

    return 0;
}
