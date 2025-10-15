# 🎨 Guide d'utilisation - HiddenInk v2.0

## 📁 Structure des dossiers

```
HiddenInk2/
├── img_banque/        # Dossier contenant vos images sources
│   ├── BMP/           # Images au format BMP
│   ├── PNG/           # Images au format PNG
│   └── JPG/           # Images au format JPG
│
└── out/               # Dossier de sortie (créé automatiquement)
    └── [Tous les fichiers générés sont sauvegardés ici]
```

## 🚀 Nouvelles fonctionnalités

### ✨ Sauvegarde automatique dans le dossier `out/`

**Tous les fichiers générés sont automatiquement sauvegardés dans `../out/`**

- ✅ Création automatique du dossier `out/` si inexistant
- ✅ Noms de fichiers automatiques avec timestamp
- ✅ Redirection automatique si vous oubliez de mettre `out/`

### 📝 Noms de fichiers automatiques

Si vous laissez le nom de fichier vide, le système génère automatiquement :

- `image_cachee_AAAAMMJJ_HHMMSS.png` - Pour cacher une image
- `image_extraite_AAAAMMJJ_HHMMSS.png` - Pour extraire une image
- `texte_cache_AAAAMMJJ_HHMMSS.png` - Pour cacher un texte
- `message_extrait_AAAAMMJJ_HHMMSS.txt` - Pour extraire un texte

### 🎯 Détection automatique du format

Le système choisit automatiquement le meilleur format :

- **BMP → BMP** : Si l'image porteuse est en BMP
- **PNG/JPG → PNG** : PNG est optimal pour la stéganographie
- **Auto-adaptation** : Le format s'adapte à votre image source

## 💡 Exemples d'utilisation

### Mode Interactif

```bash
./HiddenInk

=== STEGANOGRAPHIE AVANCEE ===

1. Cacher une image dans une image
2. Extraire une image cachée
3. Cacher un message texte dans une image
4. Extraire un message texte
5. Comparer deux images (MSE/PSNR)
6. Analyser l'histogramme d'une image
7. Détecter une éventuelle stéganographie

Choix : 1
```

#### Option 1 : Cacher une image

```
Image porteuse : img_banque/PNG/nature.png
Image à cacher : img_banque/PNG/secret.png
Nom du fichier de sortie (laisser vide pour auto) : 
```

Si vous laissez vide → `📁 Sauvegarde automatique : ../out/image_cachee_20251015_143022.png`

#### Option 3 : Cacher un texte

```
Image porteuse : img_banque/BMP/photo.bmp
Message à cacher : Ceci est mon message secret !
Nom du fichier de sortie (laisser vide pour auto) : mon_image.bmp
```

Résultat → `📁 Redirection vers : ../out/mon_image.bmp`

### Mode Ligne de commande

#### Cacher une image (avec auto-save)

```bash
./HiddenInk hide-image img_banque/PNG/carrier.png img_banque/PNG/secret.png auto
```
→ Sauvegarde dans `out/image_cachee_[timestamp].png`

#### Cacher un texte

```bash
./HiddenInk hide-text img_banque/PNG/photo.png "Mon secret" auto
```
→ Sauvegarde dans `out/texte_cache_[timestamp].png`

#### Extraire un texte dans un fichier

```bash
./HiddenInk extract-text out/texte_cache_20251015_143022.png auto
```
→ Sauvegarde dans `out/message_extrait_[timestamp].txt`

## 🔍 Formats supportés

### Images

- ✅ **PNG** (recommandé pour stéganographie)
- ✅ **BMP** (compatible, conservation du format)
- ✅ **JPG/JPEG** (lecture uniquement, sortie en PNG)

### Fichiers texte

- ✅ **TXT** (extraction de messages)

## 📊 Avantages de la nouvelle version

| Avant | Maintenant |
|-------|------------|
| Fichiers dispersés | ✅ Tout dans `out/` |
| Noms manuels obligatoires | ✅ Noms auto avec timestamp |
| Format fixe | ✅ Auto-détection du format optimal |
| Risque d'écrasement | ✅ Timestamps uniques |

## 🎓 Bonnes pratiques

1. **Laissez les noms vides** pour une gestion automatique
2. **Le dossier `out/` est votre espace de travail** - tous vos fichiers y seront
3. **Les timestamps évitent les conflits** - pas besoin de choisir des noms uniques
4. **Format BMP → BMP** si vous voulez garder le même format
5. **Format PNG pour tout le reste** - meilleure qualité stéganographique

## 🔧 Commandes rapides

```bash
# Mode démo (test rapide)
./HiddenInk demo

# Aide complète
./HiddenInk --help

# Comparer deux images
./HiddenInk compare original.png stego.png

# Analyser une image
./HiddenInk detect out/image_cachee_20251015_143022.png
```

## 🎉 C'est tout !

Profitez de la stéganographie simplifiée ! 🚀
