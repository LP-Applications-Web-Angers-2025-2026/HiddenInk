# 📁 Structure du projet HiddenInk

Dernière mise à jour : Octobre 2025

## 🏗️ Hiérarchie complète

```
HiddenInk/
│
├── 📂 src/                          # CODE SOURCE
│   ├── main.cpp                     # Point d'entrée (CLI + mode interactif)
│   └── stb_impl.cpp                 # Implémentation des bibliothèques STB
│
├── 📂 include/                      # HEADERS (.hpp)
│   │
│   ├── 📂 stegano/                  # Modules de stéganographie
│   │   ├── stegano_imageinimage.hpp # Image-in-image (signature: !#@!)
│   │   └── stegano_text.hpp         # Text-in-image (signature: !#TEXT#!)
│   │
│   ├── 📂 analysis/                 # Outils d'analyse
│   │   └── image_analysis.hpp       # MSE, PSNR, histogramme, détection LSB
│   │
│   └── 📂 external/                 # Bibliothèques tierces
│       ├── stb_image.h              # Chargement d'images (STB)
│       └── stb_image_write.h        # Sauvegarde d'images PNG (STB)
│
├── 📂 docs/                         # DOCUMENTATION
│   ├── README.md                    # Documentation technique complète
│   ├── EXAMPLES.md                  # 7 scénarios d'utilisation détaillés
│   └── LICENSE                      # Licence MIT
│
├── 📂 examples/                     # FICHIERS D'EXEMPLE
│   ├── img/                         # Images de test
│   │   ├── spongebob.png           # (vos images de test)
│   │   └── ...
│   └── test_message.txt             # Message d'exemple pour les tests
│
├── 📂 build/                        # DOSSIER DE BUILD (généré, ignoré par git)
│   ├── bin/                         # Exécutables compilés
│   │   └── hiddenink.exe           # Exécutable principal
│   └── ...                          # Fichiers CMake temporaires
│
├── 📄 CMakeLists.txt                # Configuration CMake
├── 📄 README.md                     # README principal (vue d'ensemble)
├── 📄 CONTRIBUTING.md               # Guide de contribution/développement
├── 📄 .gitignore                    # Fichiers ignorés par Git
├── 📄 build.sh                      # Script de build Linux/macOS
└── 📄 build.ps1                     # Script de build Windows PowerShell
```

## 📊 Statistiques

- **Modules de stéganographie** : 2 (image-in-image, text-in-image)
- **Outils d'analyse** : 5 fonctions (MSE, PSNR, comparaison, histogramme, détection)
- **Lignes de code** : ~1500 lignes
- **Fichiers source** : 2 (.cpp)
- **Fichiers header** : 5 (.hpp, .h)
- **Documentation** : 3 fichiers Markdown

## 🎯 Organisation par fonctionnalité

### 🔹 Stéganographie Image-in-Image
**Fichiers :**
- `include/stegano/stegano_imageinimage.hpp`

**Fonctions principales :**
- `hideImageInImage()` - Cache une image dans une autre
- `extractImageFromImage()` - Extrait l'image cachée
- `detectBitsPerChannel()` - Détection automatique (1-8 bits)
- `resizeImage()` - Redimensionnement bilinéaire
- `calculateOptimalSize()` - Calcul de la taille optimale

### 🔹 Stéganographie Text-in-Image
**Fichiers :**
- `include/stegano/stegano_text.hpp`

**Fonctions principales :**
- `hideTextInImage()` - Cache un message texte
- `extractTextFromImage()` - Extrait le message
- `detectTextBitsPerChannel()` - Détection automatique
- `saveMessageToFile()` - Sauvegarde dans un .txt

### 🔹 Analyse d'images
**Fichiers :**
- `include/analysis/image_analysis.hpp`

**Fonctions principales :**
- `calculateMSE()` - Mean Squared Error
- `calculatePSNR()` - Peak Signal-to-Noise Ratio
- `compareImages()` - Comparaison complète avec interprétation
- `generateHistogram()` - Analyse fréquentielle par canal
- `analyzeImageForSteganography()` - Détection d'anomalies LSB

### 🔹 Interface utilisateur
**Fichiers :**
- `src/main.cpp`

**Modes :**
- **CLI** : 7 commandes (hide-image, extract-image, hide-text, extract-text, compare, histogram, detect)
- **Interactif** : Menu avec 7 options

## 🔧 Compilation et build

### Option 1 : Script PowerShell (Windows)
```powershell
.\build.ps1 debug      # Compile en mode Debug
.\build.ps1 release    # Compile en mode Release
.\build.ps1 run        # Compile et exécute
.\build.ps1 clean      # Nettoie le projet
```

### Option 2 : Script Bash (Linux/macOS)
```bash
./build.sh debug       # Compile en mode Debug
./build.sh release     # Compile en mode Release
./build.sh run         # Compile et exécute
./build.sh clean       # Nettoie le projet
```

### Option 3 : CMake manuel
```bash
mkdir build && cd build
cmake ..
cmake --build .
./bin/hiddenink
```

## 📦 Fichiers de configuration

### CMakeLists.txt
- Projet : **HiddenInk v1.0.0**
- Standard : **C++20**
- Compilateurs supportés : GCC, Clang, MSVC
- Output : `build/bin/hiddenink`

### .gitignore
Ignore :
- Dossiers de build (`build/`, `cmake-build-*/`)
- IDEs (`.idea/`, `.vscode/`, `.vs/`)
- Fichiers compilés (`*.o`, `*.exe`, etc.)
- Fichiers temporaires (`*.tmp`, `*.log`)

## 🚀 Utilisation rapide

```bash
# Compiler
.\build.ps1

# Cacher un message
.\build\bin\hiddenink hide-text examples\img\spongebob.png "Bonjour!" output.png

# Extraire le message
.\build\bin\hiddenink extract-text output.png message.txt

# Analyser l'image
.\build\bin\hiddenink compare examples\img\spongebob.png output.png
```

## 📚 Documentation

- **README.md** (racine) : Vue d'ensemble et démarrage rapide
- **docs/README.md** : Documentation technique complète
- **docs/EXAMPLES.md** : 7 scénarios d'utilisation détaillés
- **CONTRIBUTING.md** : Guide de développement et contribution

## 🔄 Changements par rapport à l'ancienne structure

### ✅ Améliorations
- Code source séparé des headers (`src/` vs `include/`)
- Headers organisés par fonctionnalité (stegano, analysis, external)
- Documentation regroupée dans `docs/`
- Scripts de build automatisés
- Structure professionnelle et maintenable

### 🗑️ Fichiers supprimés
- `stegano.hpp` (ancien, remplacé)
- `stegano_image.hpp` (ancien, remplacé)
- `final`, `final.png`, `main.txt` (fichiers de test temporaires)

### 📁 Fichiers déplacés
| Ancien emplacement | Nouveau emplacement |
|-------------------|---------------------|
| `main.cpp` | `src/main.cpp` |
| `stb_impl.cpp` | `src/stb_impl.cpp` |
| `stegano_imageinimage.hpp` | `include/stegano/stegano_imageinimage.hpp` |
| `stegano_text.hpp` | `include/stegano/stegano_text.hpp` |
| `image_analysis.hpp` | `include/analysis/image_analysis.hpp` |
| `stb_image.h` | `include/external/stb_image.h` |
| `stb_image_write.h` | `include/external/stb_image_write.h` |
| `README.md` | `docs/README.md` (+ nouveau README.md à la racine) |
| `EXAMPLES.md` | `docs/EXAMPLES.md` |
| `LICENSE` | `docs/LICENSE` |
| `img/` | `examples/img/` |

## 🎓 Principes d'organisation appliqués

1. **Séparation des responsabilités** : src/ pour le code, include/ pour les interfaces
2. **Modularité** : Chaque fonctionnalité dans son propre module
3. **Documentation au même niveau** : docs/ séparé du code
4. **Exemples isolés** : examples/ pour les fichiers de test
5. **Build séparé** : build/ généré, jamais versionné
6. **Standards professionnels** : Structure type d'un projet C++ moderne

---

**Note :** Cette structure suit les bonnes pratiques C++ et facilite la maintenance, l'extension et la collaboration sur le projet.
