# 🖼️🔒 HiddenInk# HiddenInk 🖼️🔒



Application C++ de stéganographie avancée permettant de cacher et extraire des images et des messages textes dans des images PNG, avec des outils d'analyse.Application C++ de stéganographie avancée permettant de cacher et extraire des images et des messages textes dans des images PNG, avec des outils d'analyse.



## 🚀 Démarrage rapide## ✨ Fonctionnalités



### Compilation### 🖼️ Image-in-Image

- **Cacher une image** dans une autre image

```bash- **Extraire une image** cachée

# Créer le dossier de build- Détection automatique des paramètres d'encodage

mkdir build && cd build- Redimensionnement automatique avec interpolation bilinéaire

- Limite stricte de 2 bits/canal pour préserver la qualité visuelle

# Configurer avec CMake

cmake ..### 📝 Text-in-Image

- **Cacher un message texte** dans une image

# Compiler- **Extraire un message texte** avec validation

cmake --build .- Détection automatique du nombre de bits utilisés

- Gestion des capacités et statistiques d'utilisation

# L'exécutable se trouve dans build/bin/

./bin/hiddenink### 🔍 Analyse d'Images

```- **Comparaison MSE/PSNR** entre image originale et stéganographiée

- **Analyse d'histogramme** par canal (RGB/RGBA)

### Utilisation simple- **Détection de stéganographie** via analyse des LSB

- Statistiques détaillées (min, max, moyenne, écart-type)

```bash

# Cacher un message texte## 🛠️ Compilation

./bin/hiddenink hide-text photo.png "Mon message secret" output.png

```bash

# Extraire le messagemkdir build && cd build

./bin/hiddenink extract-text output.png message.txtcmake ..

make

# Mode interactif```

./bin/hiddenink

```## 🚀 Utilisation



## 📚 Documentation complète### Mode interactif



- **[Documentation complète](docs/README.md)** - Toutes les fonctionnalités et détails techniquesLancez simplement le programme sans arguments :

- **[Exemples d'utilisation](docs/EXAMPLES.md)** - Scénarios pratiques et cas d'usage

- **[Licence](docs/LICENSE)** - Termes et conditions```bash

./main

## ✨ Fonctionnalités principales```



- 🖼️ **Image-in-Image** - Cacher une image dans une autreVous verrez alors un menu avec 7 options :

- 📝 **Text-in-Image** - Cacher des messages textes1. Cacher une image dans une image

- 🔍 **Analyse MSE/PSNR** - Mesurer la qualité de l'image stéganographiée2. Extraire une image cachée

- 📊 **Analyse d'histogramme** - Distribution des valeurs de pixels3. Cacher un message texte dans une image

- 🕵️ **Détection de stéganographie** - Analyse des LSB4. Extraire un message texte

5. Comparer deux images (MSE/PSNR)

## 📁 Structure du projet6. Analyser l'histogramme d'une image

7. Détecter une éventuelle stéganographie

```

HiddenInk/### Mode ligne de commande

├── src/                    # Code source

│   ├── main.cpp           # Point d'entrée#### Cacher une image

│   └── stb_impl.cpp       # Implémentation STB```bash

├── include/               # Headers./main hide-image carrier.png secret.png output.png

│   ├── stegano/          # Modules de stéganographie```

│   ├── analysis/         # Outils d'analyse

│   └── external/         # Bibliothèques externes (STB)#### Extraire une image

├── docs/                  # Documentation```bash

├── examples/              # Fichiers d'exemple./main extract-image stego.png extracted.png

│   └── img/              # Images de test```

├── build/                 # Dossier de compilation (généré)

└── CMakeLists.txt        # Configuration CMake#### Cacher un texte

``````bash

./main hide-text carrier.png "Mon message secret" output.png

## 🛠️ Prérequis```



- **CMake** 3.20 ou supérieur#### Extraire un texte

- **Compilateur C++20** (GCC 10+, Clang 10+, MSVC 2019+)```bash

- **Windows, Linux ou macOS**./main extract-text stego.png

```

## 🎯 Exemple complet

#### Comparer deux images

```bash```bash

# 1. Cacher une image dans une autre./main compare original.png stego.png

./bin/hiddenink hide-image examples/img/carrier.png examples/img/secret.png output.png```



# 2. Vérifier la qualitéAffiche :

./bin/hiddenink compare examples/img/carrier.png output.png- MSE (Mean Squared Error)

- PSNR (Peak Signal-to-Noise Ratio)

# 3. Extraire l'image cachée- Pourcentage de pixels modifiés

./bin/hiddenink extract-image output.png extracted.png- Interprétation de la qualité

```

#### Analyser l'histogramme

## 📄 Licence```bash

./main histogram image.png

MIT License - voir [docs/LICENSE](docs/LICENSE) pour plus de détails.```



## 🙏 CréditsAffiche :

- Distribution des valeurs par canal

- [STB Libraries](https://github.com/nothings/stb) par Sean Barrett- Statistiques (min, max, moyenne, écart-type)

- Développé à l'Université d'Angers (2025-2026)- Top 5 des valeurs les plus fréquentes



---#### Détecter la stéganographie

```bash

**Note:** Pour une documentation détaillée, consultez [docs/README.md](docs/README.md)./main detect image.png

```

Analyse la distribution des LSB pour détecter d'éventuelles anomalies.

## 📋 Exemples d'utilisation

### Exemple complet : Image-in-Image

```bash
# 1. Cacher une image
./main hide-image photos/beach.png photos/cat.png output/stego.png

# 2. Comparer l'original et le stego
./main compare photos/beach.png output/stego.png

# 3. Extraire l'image cachée
./main extract-image output/stego.png output/recovered.png

# 4. Vérifier que l'extraction est correcte
./main compare photos/cat.png output/recovered.png
```

### Exemple complet : Text-in-Image

```bash
# 1. Cacher un message
./main hide-text photo.png "Rendez-vous à minuit!" output/stego.png

# 2. Analyser l'image produite
./main histogram output/stego.png
./main detect output/stego.png

# 3. Extraire le message
./main extract-text output/stego.png
```

## 🔧 Détails techniques

### Méthode de stéganographie

- **LSB (Least Significant Bit)** : Les données sont cachées dans les bits de poids faible des pixels
- **Signature** : Chaque type de contenu a sa signature unique
  - Image : `!#@!`
  - Texte : `!#TEXT#!`
- **Balises** : Délimitent le contenu (`~{&` et `&}~`)
- **Auto-détection** : Le programme détecte automatiquement le nombre de bits utilisés (1-8)
- **Limite de qualité** : Maximum 2 bits/canal pour éviter les artefacts visibles

### Format d'encodage

#### Image-in-Image
```
[Signature] [Balise ouvrante] [Header 96 bits] [Données image] [Balise fermante]
```

Header (96 bits) :
- 32 bits : largeur
- 32 bits : hauteur
- 32 bits : nombre de canaux

#### Text-in-Image
```
[Signature] [Balise ouvrante] [Longueur message] [Message binaire] [Balise fermante]
```

### Métriques de qualité

#### MSE (Mean Squared Error)
- MSE = 0 : Images identiques
- MSE < 1 : Différence imperceptible
- MSE < 10 : Excellente qualité

#### PSNR (Peak Signal-to-Noise Ratio)
- PSNR > 50 dB : Imperceptible
- PSNR > 40 dB : Excellente qualité
- PSNR > 30 dB : Bonne qualité
- PSNR < 30 dB : Qualité dégradée

## 📁 Structure du projet

```
HiddenInk/
├── main.cpp                      # Point d'entrée (CLI + mode interactif)
├── stegano_imageinimage.hpp      # Stéganographie image-in-image
├── stegano_text.hpp              # Stéganographie text-in-image
├── image_analysis.hpp            # Outils d'analyse (MSE, histogramme, détection)
├── stb_image.h                   # Bibliothèque de lecture d'images
├── stb_image_write.h             # Bibliothèque d'écriture d'images
├── stb_impl.cpp                  # Implémentation STB
└── CMakeLists.txt                # Configuration CMake
```

## 📄 Licence

Ce projet est sous licence MIT - voir le fichier [LICENSE](LICENSE) pour plus de détails.

## 🙏 Remerciements

- [STB Libraries](https://github.com/nothings/stb) pour la gestion des images

