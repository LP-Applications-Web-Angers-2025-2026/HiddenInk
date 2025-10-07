#include <filesystem>
#include "stegano.hpp"

int main() {
    std::filesystem::path root = std::filesystem::current_path().parent_path();
    string inFile = (root / "demo_in.txt").string();
    string outFile = (root / "demo_out.txt").string();

    cout << "=== MENU STEGANOGRAPHIE ===\n";
    cout << "1. Cacher un message dans demo_out.txt\n";
    cout << "2. Extraire un message depuis demo_out.txt\n";
    cout << "3. Quitter\n";
    cout << "-----------------------------\n";
    cout << "Choix : ";

    int choix;
    cin >> choix;
    cin.ignore();

    if (choix == 1) {
        string contenu;
        cout << "\nTexte du fichier support (demo_in.txt) :\n> ";
        getline(cin, contenu);
        ecrireFichier(inFile, contenu);

        auto bytes = lireFichier(inFile);
        if (bytes.empty()) {
            cerr << "Erreur : impossible de lire demo_in.txt.\n";
            return 1;
        }

        string secret;
        cout << "\nMessage secret à cacher :\n> ";
        getline(cin, secret);

        int bitsPerByte = 8; // chaque octet du fichier stocke 1 octet du message
        embedLSB(bytes, secret, bitsPerByte);

        writeFileBytes(outFile, bytes);

        cout << "\n✅ Message caché dans demo_out.txt avec succès.\n";
    }
    else if (choix == 2) {
        auto bytes = lireFichier(outFile);
        if (bytes.empty()) {
            cerr << "Erreur : impossible de lire demo_out.txt.\n";
            return 1;
        }

        int bitsPerByte = 8;
        string extrait = extractLSB(bytes, bitsPerByte);

        cout << "\n💬 Message extrait : \"" << extrait << "\"\n";
    }
    else {
        cout << "Fin du programme.\n";
    }

    return 0;
}
