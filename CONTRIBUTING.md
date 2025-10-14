# 📖 Guide de développement - HiddenInk

Ce document explique la structure du projet et comment contribuer.

## 📁 Architecture du projet

```
HiddenInk/
├── src/                          # Code source C++
│   ├── main.cpp                 # Point d'entrée, CLI et mode interactif
│   └── stb_impl.cpp             # Implémentation des bibliothèques STB
│
├── include/                      # Fichiers header (.hpp)
│   ├── stegano/                 # Modules de stéganographie
│   │   ├── stegano_imageinimage.hpp  # Stéganographie image-in-image
│   │   └── stegano_text.hpp          # Stéganographie text-in-image
│   │
│   ├── analysis/                # Outils d'analyse
│   │   └── image_analysis.hpp        # MSE, PSNR, histogramme, détection
│   │
│   └── external/                # Bibliothèques tierces
│       ├── stb_image.h               # Chargement d'images (STB)
│       └── stb_image_write.h         # Sauvegarde d'images (STB)
│
├── docs/                         # Documentation
│   ├── README.md                # Documentation complète
│   ├── EXAMPLES.md              # Exemples d'utilisation
│   └── LICENSE                  # Licence MIT
│
├── examples/                     # Fichiers d'exemple et de test
│   └── img/                     # Images de test
│
├── build/                        # Dossier de build (généré, ignoré par git)
│   └── bin/                     # Exécutables compilés
│
├── CMakeLists.txt               # Configuration CMake
├── .gitignore                   # Fichiers ignorés par git
└── README.md                    # README principal (vue d'ensemble)
```

## 🔧 Compilation

### Première compilation

```bash
# Créer le dossier de build
mkdir build
cd build

# Configurer le projet
cmake ..

# Compiler
cmake --build .

# L'exécutable est dans build/bin/
./bin/hiddenink
```

### Recompilation après modifications

```bash
cd build
cmake --build .
```

### Nettoyage complet

```bash
# Supprimer le dossier build
rm -rf build

# Puis recommencer la compilation
mkdir build && cd build
cmake .. && cmake --build .
```

## 📝 Conventions de code

### Style de code

- **Indentation** : 4 espaces (pas de tabulations)
- **Accolades** : Style K&R (accolade ouvrante sur la même ligne)
- **Noms de variables** : camelCase pour les variables locales
- **Noms de fonctions** : camelCase
- **Constantes** : UPPER_CASE avec underscores

### Exemple

```cpp
inline bool saveImage(const string& path, unsigned char* data, int w, int h, int c) {
    if (!data) {
        cerr << "❌ Erreur : données invalides\n";
        return false;
    }
    
    return stbi_write_png(path.c_str(), w, h, c, data, w * c) != 0;
}
```

### Organisation des headers

```cpp
#pragma once

// Includes externes (bibliothèques standard)
#include <iostream>
#include <vector>
#include <string>

// Includes internes (STB, autres modules)
#include "external/stb_image.h"

// Namespace
using namespace std;

// Déclarations de fonctions
inline string getSignature();

// Implémentations
inline string getSignature() {
    return "!#@!";
}
```

## 🏗️ Ajouter une nouvelle fonctionnalité

### 1. Module de stéganographie

Pour ajouter un nouveau type de stéganographie (ex: audio-in-image) :

1. Créer `include/stegano/stegano_audio.hpp`
2. Implémenter les fonctions :
   - `hideAudioInImage()`
   - `extractAudioFromImage()`
   - `detectAudioBitsPerChannel()`
3. Ajouter l'include dans `src/main.cpp`
4. Ajouter les options CLI et menu interactif

### 2. Outil d'analyse

Pour ajouter une analyse (ex: analyse de bruit) :

1. Créer ou modifier `include/analysis/image_analysis.hpp`
2. Ajouter la fonction inline
3. Intégrer dans le menu du `main.cpp`

### 3. Tests

Ajouter des images de test dans `examples/img/` pour valider la fonctionnalité.

## 🐛 Debugging

### Afficher les valeurs pendant l'exécution

```cpp
#ifdef DEBUG
    cout << "DEBUG: w=" << w << ", h=" << h << ", c=" << c << "\n";
#endif
```

Compiler en mode debug :
```bash
cmake -DCMAKE_BUILD_TYPE=Debug ..
cmake --build .
```

### Vérifier les erreurs de compilation

```bash
cmake --build . 2>&1 | grep "error:"
```

## 📚 Modules existants

### 🔹 stegano_imageinimage.hpp

**Fonctions principales :**
- `hideImageInImage()` - Cache une image dans une autre
- `extractImageFromImage()` - Extrait l'image cachée
- `detectBitsPerChannel()` - Détecte automatiquement les paramètres
- `resizeImage()` - Redimensionnement avec interpolation bilinéaire

**Signature :** `!#@!`  
**Balises :** `~{&` et `&}~`

### 🔹 stegano_text.hpp

**Fonctions principales :**
- `hideTextInImage()` - Cache un message texte
- `extractTextFromImage()` - Extrait le message
- `detectTextBitsPerChannel()` - Détection automatique
- `saveMessageToFile()` - Sauvegarde dans un .txt

**Signature :** `!#TEXT#!`  
**Balises :** `~<TXT>` et `</TXT>~`

### 🔹 image_analysis.hpp

**Fonctions principales :**
- `calculateMSE()` - Mean Squared Error
- `calculatePSNR()` - Peak Signal-to-Noise Ratio
- `compareImages()` - Comparaison complète avec interprétation
- `generateHistogram()` - Analyse fréquentielle
- `analyzeImageForSteganography()` - Détection d'anomalies LSB

## 🔄 Workflow Git

```bash
# Créer une branche pour votre fonctionnalité
git checkout -b feature/nouvelle-fonction

# Faire vos modifications
# ... éditer les fichiers ...

# Commiter
git add .
git commit -m "feat: ajout de la fonctionnalité X"

# Pousser
git push origin feature/nouvelle-fonction

# Créer une Pull Request sur GitHub
```

### Convention de commits

- `feat:` - Nouvelle fonctionnalité
- `fix:` - Correction de bug
- `docs:` - Documentation
- `refactor:` - Refactoring
- `test:` - Tests
- `chore:` - Maintenance

## 📊 Métriques de qualité

### MSE (Mean Squared Error)
- 0 = images identiques
- < 1 = imperceptible
- < 10 = excellente qualité

### PSNR (Peak Signal-to-Noise Ratio)
- ∞ dB = images identiques
- \> 50 dB = imperceptible
- \> 40 dB = excellente qualité

### Distribution LSB
- Ratio ≈ 0.5 (50/50) = normal
- Ratio < 0.45 ou > 0.55 = suspect

## 🎓 Ressources

- [STB Libraries](https://github.com/nothings/stb)
- [LSB Steganography](https://en.wikipedia.org/wiki/Steganography)
- [CMake Documentation](https://cmake.org/documentation/)
- [C++20 Reference](https://en.cppreference.com/)

## 💡 Bonnes pratiques

1. **Toujours tester** avant de commit
2. **Documenter** les nouvelles fonctions
3. **Utiliser des noms explicites** pour les variables
4. **Gérer les erreurs** avec des messages clairs
5. **Limiter la taille des fonctions** (< 50 lignes idéalement)
6. **Commenter le code complexe**

## 🚀 Améliorations futures possibles

- [ ] Support du format JPEG (avec limitation due à la compression)
- [ ] Interface graphique (Qt/GTK)
- [ ] Chiffrement des données avant stéganographie
- [ ] Support audio-in-image
- [ ] Mode batch (traiter plusieurs fichiers)
- [ ] Compression des données avant injection
- [ ] Tests unitaires automatisés

---

**Questions ?** Ouvrir une issue sur GitHub !
