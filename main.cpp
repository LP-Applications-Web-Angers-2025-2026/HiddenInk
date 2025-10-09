#include <iostream>
#include <ostream>
#include "stenograpgie/BMP/bmp_convert.h"
#include "stenograpgie/BMP/bmp_Recuperation.h"

using namespace std;

int main()
{
    std::string message;
    cout << "entrez message à dissimuler!" << endl;
    cin >> message;

    bmpConvert(message);
    return 0;
}
