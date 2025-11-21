#include <filesystem>
#include <iostream>
#include <fstream>
#include <ostream>
#include "interfaces/cli/non_interactif/fonction_menu_non_interactif.h"

using namespace std;

int main(int argc, char* argv[])
{
    if (argc < 2)
    {
        print_usage();
        return 1;
    }

    string mode = argv[1];

    if (mode == "INTERACT")
    {
        if (argc != 2)
        {
            print_usage();
            return 1;
        }
        return interact();
    }
    else if (mode == "HIDE")
    {
        if (argc < 6 || argc > 7)
        {
            print_usage();
            return 1;
        }
        return hide(argc, argv);
    }
    else if (mode == "EXTRACT")
    {
        if (argc < 3 || argc > 5)
        {
            print_usage();
            return 1;
        }
        return extract(argc, argv);
    }
    else if (mode == "HISTO")
    {
        if (argc != 3) {
            print_usage();
            return 1;
        }
        string inputPath = argv[2];
        processHistogram(inputPath);
        return 0;
    }
    else
    {
        print_usage();
        return 1;
    }
    return 0;
}