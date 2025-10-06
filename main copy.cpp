#include <iostream>
#include <limits>

using namespace std;

int main() {

    cout << "Bienvenue dans le programme de stéganographie!" << endl;

    int userChoice = 0;
    do {
        cout << "\nMenu:\n";
        cout << "1. Insérer un message dans une image" << endl;
        cout << "2. Extraire un message d'une image" << endl;
        cout << "3. Comparer la qualité d'une image" << endl;
        cout << "4. Quitter" << endl;

        cout << "Entrez votre choix (1-4) : ";
        if (!(cin >> userChoice)) {
            
            cin.clear();
            cout << "Entrée invalide. Veuillez entrer un nombre entre 1 et 4." << endl;
            continue;
        }

        switch (userChoice)
        {
        case 1:
            cout << "-> Vous avez choisi : Insérer un message dans une image." << endl;
            // faire fonction
            
            break;
        case 2:
            cout << "-> Vous avez choisi : Extraire un message d'une image." << endl;
            // faire fonction
            break;
        case 3:
            cout << "-> Vous avez choisi : Comparer la qualité d'une image." << endl;
            // faire fonction
            break;
        case 4:
            cout << "Au revoir !" << endl;
            break;
        default:
            cout << "Choix invalide. Veuillez sélectionner une option entre 1 et 4." << endl;
            break;
        }

    } while (userChoice != 4);

    return 0;
}