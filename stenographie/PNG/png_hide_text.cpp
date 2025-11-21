#include "png_hide_text.h"
#include "../utils/stegano/stegano_imageinimage.hpp"
#include "../utils/stegano/stegano_text.hpp"
#include "../utils/utils_bin.h"
#include <fstream>
#include <iostream>

using namespace std;

/**
 * Cache un texte ou le contenu d'un fichier dans une image PNG en utilisant la
 * stéganographie.
 *
 * @param carrierPath     Chemin vers l'image PNG porteuse qui va contenir le
 * message caché
 * @param textOrFilePath  Chemin vers le fichier texte ou le texte à cacher dans
 * l'image
 * @param outputPath      Chemin où sauvegarder l'image PNG résultante avec le
 * message caché
 * @param bitsPerChannel  Nombre de bits à utiliser par canal de couleur (1-2
 * bits). Si 0, le minimum nécessaire sera calculé automatiquement
 * @return               EXIT_SUCCESS (0) en cas de succès, EXIT_FAILURE (1) en
 * cas d'erreur
 */
int hidePNGText(const string &carrierPath, const string &textOrFilePath,
                const string &outputPath, int bitsPerChannel,
                const string &key) {
  int w, h, c;

  // Charger l'image porteuse
  auto carrier = loadImage(carrierPath, w, h, c);
  if (!carrier) {
    cerr << "Erreur : impossible de charger l'image porteuse " << carrierPath
         << endl;
    return EXIT_FAILURE;
  }

  string messageToHide;
  if (VerifFichier(textOrFilePath)) {
    // C'est un fichier, on lit son contenu en texte brut
    ifstream file(textOrFilePath);
    if (!file) {
      cerr << "Erreur : impossible de lire le fichier " << textOrFilePath
           << endl;
      stbi_image_free(carrier);
      return EXIT_FAILURE;
    }
    // Lire tout le contenu du fichier
    messageToHide.assign((istreambuf_iterator<char>(file)),
                         istreambuf_iterator<char>());
    file.close();
  } else {
    // C'est un texte brut
    messageToHide = textOrFilePath;
  }

  // Cacher le texte dans l'image avec la clé de chiffrement
  auto encoded =
      hideTextInImage(carrier, w, h, c, messageToHide, bitsPerChannel, key);

  // Sauvegarder l'image encodée
  bool success = false;
  if (!encoded.empty()) {
    success = saveImage(outputPath, encoded.data(), w, h, c);
  }

  stbi_image_free(carrier);

  if (success) {
    cout << "\n✓ Message caché avec succès dans " << outputPath << endl;
    return EXIT_SUCCESS;
  } else {
    cerr << "\n✗ Erreur : échec de l'encodage ou de la sauvegarde." << endl;
    return EXIT_FAILURE;
  }
}
