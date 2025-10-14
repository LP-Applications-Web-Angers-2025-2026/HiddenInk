# HiddenInk 🖼️🔒

Application C++ de stéganographie avancée permettant de cacher et extraire des images et des messages textes dans des images PNG, avec des outils d'analyse.

## ✨ Fonctionnalités

### 🖼️ Image-in-Image
- **Cacher une image** dans une autre image
- **Extraire une image** cachée
- Détection automatique des paramètres d'encodage
- Redimensionnement automatique avec interpolation bilinéaire
- Limite stricte de 2 bits/canal pour préserver la qualité visuelle

### 📝 Text-in-Image
- **Cacher un message texte** dans une image
- **Extraire un message texte** avec validation
- Détection automatique du nombre de bits utilisés
- Gestion des capacités et statistiques d'utilisation

### 🔍 Analyse d'Images
- **Comparaison MSE/PSNR** entre image originale et stéganographiée
- **Analyse d'histogramme** par canal (RGB/RGBA)
- **Détection de stéganographie** via analyse des LSB
- Statistiques détaillées (min, max, moyenne, écart-type)

## 🛠️ Compilation

```bash
mkdir build && cd build
cmake ..
make
```

## 🚀 Utilisation

### Mode interactif

Lancez simplement le programme sans arguments :

```bash
./main
```

Vous verrez alors un menu avec 7 options :
1. Cacher une image dans une image
2. Extraire une image cachée
3. Cacher un message texte dans une image
4. Extraire un message texte
5. Comparer deux images (MSE/PSNR)
6. Analyser l'histogramme d'une image
7. Détecter une éventuelle stéganographie

### Mode ligne de commande

#### Cacher une image
```bash
./main hide-image carrier.png secret.png output.png
```

#### Extraire une image
```bash
./main extract-image stego.png extracted.png
```

#### Cacher un texte
```bash
./main hide-text carrier.png "Mon message secret" output.png
```

#### Extraire un texte
```bash
./main extract-text stego.png
```

#### Comparer deux images
```bash
./main compare original.png stego.png
```

Affiche :
- MSE (Mean Squared Error)
- PSNR (Peak Signal-to-Noise Ratio)
- Pourcentage de pixels modifiés
- Interprétation de la qualité

#### Analyser l'histogramme
```bash
./main histogram image.png
```

Affiche :
- Distribution des valeurs par canal
- Statistiques (min, max, moyenne, écart-type)
- Top 5 des valeurs les plus fréquentes

#### Détecter la stéganographie
```bash
./main detect image.png
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

