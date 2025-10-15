#include <iostream>
#include "stenographie/utils/main_helpers.hpp"

using namespace MainHelpers;

int main(int argc, char* argv[])
{
    try
    {
        if (argc > 1)
        {
            return runCommandLineMode(argc, argv);
        }

        return runInteractiveMode();
    }
    catch (const std::exception& ex)
    {
        std::cerr << "Erreur inattendue : " << ex.what() << std::endl;
    }
    catch (...)
    {
        std::cerr << "Erreur inattendue inconnue." << std::endl;
    }

    return kFailure;
}
