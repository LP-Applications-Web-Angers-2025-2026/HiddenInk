<p align="center">
  <img src="hiddenInk.webp" alt="HiddenInk Logo" width="250">
</p>

# HiddenInk

HiddenInk est un outil de stéganographie avancé écrit en C++ (C++20). Il permet de dissimuler du texte ou des images à l'intérieur d'images BMP et PNG, avec des options de chiffrement et d'analyse.

## Fonctionnalités

- **Dissimulation (Hide)** : Cacher un fichier texte ou une image dans une image porteuse (BMP ou PNG).
- **Extraction (Extract)** : Récupérer les données cachées d'une image stéganographiée.
- **Chiffrement** : Option pour chiffrer les données cachées avec une clé générée automatiquement.
- **Choix du bit** : Possibilité de choisir le plan de bit (1-8) utilisé pour la dissimulation (LSB par défaut).
- **Analyse** : Génération d'histogrammes pour analyser les images.
- **Mode Interactif** : Un menu convivial pour guider l'utilisateur à travers les différentes opérations.

## Prérequis

- Compilateur C++ compatible C++20 (GCC, Clang, MSVC).
- [CMake](https://cmake.org/) (version 3.10 ou supérieure recommandée).

## Installation et Compilation

Vous pouvez compiler le projet en utilisant les scripts fournis ou manuellement avec CMake.

### Via les scripts de build

**Windows (PowerShell) :**
```powershell
./build.ps1
```

**Linux / macOS (Bash) :**
```bash
./build.sh
```

### Compilation manuelle avec CMake

```bash
mkdir build
cd build
cmake ..
cmake --build .
```

L'exécutable `HiddenInk` (ou `HiddenInk.exe` sur Windows) sera généré.

## Utilisation

Le programme peut être utilisé en ligne de commande (CLI) ou via un mode interactif.

### Mode Interactif

Lancez simplement l'exécutable sans arguments ou avec l'argument `INTERACT` :

```bash
./HiddenInk INTERACT
```
Suivez ensuite les instructions à l'écran.

### Ligne de Commande (CLI)

#### Cacher des données (HIDE)

```bash
./HiddenInk HIDE <input_bmp> <file_to_hide> <output_bmp> <encrypt> [bit_position]
```

- `<input_bmp>` : Chemin de l'image source.
- `<file_to_hide>` : Chemin du fichier à cacher.
- `<output_bmp>` : Chemin de l'image de sortie.
- `<encrypt>` : `o` pour activer le chiffrement (génère une clé), `n` pour désactiver.
- `[bit_position]` : (Optionnel) Le bit à utiliser (1-8). Défaut : 1 (LSB).

**Exemple :**
```bash
./HiddenInk HIDE image.bmp secret.txt image_steg.bmp o 1
```

#### Extraire des données (EXTRACT)

```bash
./HiddenInk EXTRACT <input_bmp> [key] [bit_position]
```

- `<input_bmp>` : Chemin de l'image contenant les données cachées.
- `[key]` : (Optionnel) La clé de déchiffrement (si chiffré).
- `[bit_position]` : (Optionnel) Le bit utilisé (1-8).

**Exemple :**
```bash
./HiddenInk EXTRACT image_steg.bmp ma_cle_secrete 1
```

#### Analyser une image (HISTO)

```bash
./HiddenInk HISTO <input_bmp>
```

## Structure du Projet

- `main.cpp` : Point d'entrée du programme.
- `stenographie/` : Cœur du moteur de stéganographie (BMP, PNG, Utils).
- `interfaces/` : Gestion de l'interface utilisateur (CLI et menus).
- `docs/` : Documentation supplémentaire.

## Auteurs

Mathéo Perodeau, Alban Pouchon
Vincent Gamblin, Lucas Strullu
