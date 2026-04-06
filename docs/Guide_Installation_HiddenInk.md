# HiddenInk - Guide d'Installation (Edition Pro)

---

Document: Guide d'Installation  
Version: 2.0 (Edition Pro)  
Public: Utilisateurs, integrateurs, developpeurs  
Date: Avril 2026

---

## Couverture

Ce guide explique une installation complete et reproductible de HiddenInk:
- build CLI via CMake;
- build GUI via qmake/Qt;
- verification fonctionnelle post-installation;
- correction des erreurs de build les plus frequentes.

## Sommaire visuel

| Bloc | Resultat attendu |
|---|---|
| Prerequis | Outils systeme valides (`cmake`, compilateur, `qmake`) |
| Build CLI | Binaire `HiddenInk` compile et executable |
| Build GUI | Binaire `HiddenInkGUI` compile et executable |
| Validation | Smoke tests CLI + GUI passes |
| PDF docs | Guides PDF generes dans `docs/` |

## Checklist installation propre

- [ ] Verifier versions outils
- [ ] Cloner le projet
- [ ] Compiler CLI (`cmake .. && make`)
- [ ] Compiler GUI (`qmake && make`)
- [ ] Executer un test injection/extraction simple
- [ ] Verifier les onglets GUI disponibles

## Encadre important (Qt Creator)

> Apres modification du fichier `GUI.pro`, faites `Run qmake` avant `Build`.

## 1. Objectif

Ce guide explique l'installation de HiddenInk:
- mode CLI (CMake);
- mode GUI (Qt / qmake);
- validation post-installation;
- résolution des problèmes fréquents.

## 2. Prérequis

## 2.1 Système

- Linux recommandé (testé en environnement Linux).
- macOS possible avec adaptation Qt/CMake.
- Windows possible via Qt + compilateur compatible.

## 2.2 Outils minimum

- Compilateur C++20 (`g++` ou `clang++`)
- `cmake`
- `make` (ou Ninja)
- Qt (pour GUI): `qmake`, modules Widgets

Vérification rapide:

```bash
g++ --version
cmake --version
qmake --version
```

## 3. Récupération du projet

```bash
git clone <url-du-repo> HiddenInk
cd HiddenInk
```

## 4. Build CLI (CMake)

## 4.1 Configuration

```bash
mkdir -p build
cd build
cmake ..
```

## 4.2 Compilation

```bash
make -j$(nproc)
```

## 4.3 Exécution

```bash
./HiddenInk
```

Note:
- le binaire produit est défini par `CMakeLists.txt` (actuellement `HiddenInk`).

## 5. Build GUI (qmake)

Le fichier projet est:
- `GUI/GUI.pro`

## 5.1 Préparation

```bash
cd GUI
qmake GUI.pro
```

## 5.2 Compilation

```bash
make -j$(nproc)
```

## 5.3 Lancement GUI

```bash
./HiddenInkGUI
```

## 5.4 Vérification des dépendances liées

Le projet GUI référence des sources du dossier principal (PNG/BMP/WAV/utils).  
Si une erreur de lien apparait:
- vérifier les entrées `SOURCES` et `HEADERS` dans `GUI/GUI.pro`;
- vérifier que tous les fichiers existent aux chemins relatifs attendus.

## 6. Structure de build recommandée

- Build CMake dans `build/`
- Build GUI dans `GUI/` (artefacts qmake locaux)
- Documentation dans `docs/`

Exemple:

```text
HiddenInk/
  build/                 # CMake
  GUI/                   # qmake + exécutable GUI
  docs/                  # guides et PDF
```

## 7. Validation après installation

## 7.1 Smoke test CLI

- Lancer le binaire CLI.
- Vérifier que le menu/mode attendu apparait.

## 7.2 Smoke test GUI

- Lancer `HiddenInkGUI`.
- Vérifier la présence des onglets:
  - Injection
  - Extraction
  - Analyse
  - Histogramme
  - Outils
  - Journal

## 7.3 Test fonctionnel minimal

1. Injection texte PNG
2. Extraction texte PNG
3. Vérifier le contenu extrait
4. Vérifier le journal et l'absence de crash

## 8. Génération PDF de la documentation

Ce projet fournit des guides Markdown dans `docs/`.  
Vous pouvez générer des PDF de deux façons.

## 8.1 Option A: Pandoc (recommandé)

Installation (Ubuntu/Debian):

```bash
sudo apt update
sudo apt install -y pandoc texlive-latex-base texlive-latex-recommended texlive-fonts-recommended
```

Génération:

```bash
cd docs
pandoc Guide_Utilisateur_HiddenInk.md -o Guide_Utilisateur_HiddenInk.pdf
pandoc Guide_Installation_HiddenInk.md -o Guide_Installation_HiddenInk.pdf
pandoc Guide_Technique_HiddenInk.md -o Guide_Technique_HiddenInk.pdf
```

## 8.2 Option B: Export navigateur

- Ouvrir le fichier `.md` converti en HTML (ou prévisualisation markdown)
- Imprimer vers PDF (`Ctrl+P` > Enregistrer en PDF)

## 9. Résolution de problèmes

## 9.1 `uint32_t was not declared in this scope`

Cause:
- include manquant `<cstdint>`

Correctif:
- ajouter `#include <cstdint>`
- utiliser `std::uint32_t`

## 9.2 Erreurs de linkage GUI

Causes possibles:
- source oubliée dans `GUI.pro`
- headers/sources désynchronisés

Correctif:
- vérifier `SOURCES +=` et `HEADERS +=`
- relancer `qmake` puis `make clean && make`

## 9.3 Qt introuvable

Symptôme:
- `qmake: command not found`

Correctif:
- installer Qt Development Tools
- ajouter le chemin Qt au `PATH`

## 9.4 Conflit de caches build

Correctif standard:

```bash
# CMake
rm -rf build
mkdir build && cd build && cmake .. && make -j$(nproc)

# GUI
cd ../GUI
make clean
qmake GUI.pro
make -j$(nproc)
```

## 10. Bonnes pratiques d'installation

- Utiliser des environnements propres (nouveau shell / container / VM)
- Verrouiller les versions de compilateur et Qt en équipe
- Conserver un script de build reproductible
- Vérifier systématiquement avec un test fonctionnel minimal

## 11. Désinstallation

Le projet n'installe pas (par défaut) dans le système via `make install`.  
Pour nettoyer:
- supprimer les dossiers `build/` et artefacts `GUI/` générés.

## 12. Support interne

En cas de bug:
- joindre les logs de compilation complets;
- préciser OS, version compilateur, version Qt;
- joindre la commande exacte exécutée.
