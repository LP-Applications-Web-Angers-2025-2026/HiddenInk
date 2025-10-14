# 📝 Changelog - HiddenInk

Toutes les modifications notables du projet sont documentées ici.

Le format est basé sur [Keep a Changelog](https://keepachangelog.com/fr/1.0.0/),
et ce projet adhère au [Semantic Versioning](https://semver.org/lang/fr/).

---

## [1.0.0] - 2025-10-14

### 🎉 Version initiale complète

### ✨ Ajouté
- **Stéganographie Image-in-Image**
  - Cacher une image dans une autre image PNG
  - Extraction automatique avec détection des paramètres
  - Redimensionnement automatique avec interpolation bilinéaire
  - Limite de qualité à 2 bits/canal maximum
  - Signature unique : `!#@!`
  - Système de balises : `~{&` et `&}~`

- **Stéganographie Text-in-Image**
  - Cacher des messages textes dans des images PNG
  - Extraction avec validation complète
  - Sauvegarde du message extrait dans un fichier .txt
  - Signature unique : `!#TEXT#!`
  - Balises spécifiques : `~<TXT>` et `</TXT>~`
  - Gestion automatique de la capacité

- **Analyse d'images**
  - Calcul MSE (Mean Squared Error)
  - Calcul PSNR (Peak Signal-to-Noise Ratio)
  - Comparaison d'images avec interprétation automatique
  - Analyse d'histogramme par canal RGB/RGBA
  - Détection de stéganographie via analyse LSB
  - Statistiques détaillées (min, max, moyenne, écart-type)

- **Interface utilisateur**
  - Mode ligne de commande avec 7 commandes
  - Mode interactif avec menu à 7 options
  - Messages d'aide détaillés
  - Affichage des statistiques d'utilisation
  - Gestion complète des erreurs

- **Infrastructure**
  - Configuration CMake professionnelle
  - Scripts de build PowerShell et Bash
  - Structure de projet modulaire et organisée
  - Documentation complète (README, EXAMPLES, CONTRIBUTING)
  - Support multi-plateforme (Windows, Linux, macOS)

### 📁 Structure du projet
```
HiddenInk/
├── src/                 # Code source
├── include/             # Headers organisés
│   ├── stegano/        # Modules de stéganographie
│   ├── analysis/       # Outils d'analyse
│   └── external/       # Bibliothèques STB
├── docs/                # Documentation
├── examples/            # Fichiers d'exemple
└── build/               # Compilation (généré)
```

### 🔧 Commandes disponibles
```bash
hiddenink hide-image <carrier> <secret> <output>
hiddenink extract-image <input> <output>
hiddenink hide-text <carrier> <message> <output>
hiddenink extract-text <input> [output.txt]
hiddenink compare <image1> <image2>
hiddenink histogram <image>
hiddenink detect <image>
```

### 📚 Documentation
- Guide de démarrage rapide (README.md)
- Documentation technique complète (docs/README.md)
- 7 scénarios d'utilisation détaillés (docs/EXAMPLES.md)
- Guide de contribution et développement (CONTRIBUTING.md)
- Structure du projet documentée (PROJECT_STRUCTURE.md)

### 🛠️ Technologies
- C++20
- CMake 3.20+
- STB Image Libraries (stb_image.h, stb_image_write.h)
- Support PNG uniquement (format sans perte)

### 📊 Métriques
- ~1500 lignes de code
- 2 modules de stéganographie
- 5 outils d'analyse
- 7 commandes CLI
- 100% de détection automatique

---

## [À venir]

### 🚀 Fonctionnalités prévues (v1.1.0)
- [ ] Support du format BMP
- [ ] Compression des données avant stéganographie
- [ ] Chiffrement AES optionnel
- [ ] Mode batch (traiter plusieurs fichiers)
- [ ] Interface graphique (Qt)
- [ ] Tests unitaires automatisés

### 💡 Idées pour le futur
- [ ] Support audio-in-image
- [ ] Stéganographie vidéo
- [ ] Application web (WebAssembly)
- [ ] API REST pour intégration
- [ ] Plugin pour logiciels de retouche photo

---

## Types de changements

- **Ajouté** : nouvelles fonctionnalités
- **Modifié** : changements dans les fonctionnalités existantes
- **Déprécié** : fonctionnalités qui seront supprimées
- **Supprimé** : fonctionnalités supprimées
- **Corrigé** : corrections de bugs
- **Sécurité** : corrections de vulnérabilités

---

**Note :** Les versions suivent le format MAJOR.MINOR.PATCH
- **MAJOR** : changements incompatibles avec les versions précédentes
- **MINOR** : ajout de fonctionnalités rétrocompatibles
- **PATCH** : corrections de bugs rétrocompatibles
