#include <iostream>

 using namespace std;

int main() {

    cout << "Bienvenue dans le programme de stéganographie!" << endl;

    cout << "Menu:" << endl;
    

    int userChoice;
    cin >> userChoice;
    switch (userChoice)
    {
    case 1:
        cout << "1. Insérer un message dans une image" << endl;
        break;
    case 2:
        cout << "2. Extraire un message d'une image" << endl;
        break;
    case 3:
        cout << "3. Comparer la qualité d'une image" << endl;
        break;
    case 4:
        cout << "4. Quitter" << endl;
        break;

    default:
        break;
    }

    if(userChoice > 4 || userChoice < 1){
        cout << "Choix invalide. Veuillez réessayer." << endl;
    }
    else{
        cout << "Vous avez choisi l'option " << userChoice << endl;
    }

    
    return 0;
}