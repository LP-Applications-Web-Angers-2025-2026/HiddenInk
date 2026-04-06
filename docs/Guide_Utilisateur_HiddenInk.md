# HiddenInk - Guide Utilisateur (Edition Pro)

---

Document: Guide Utilisateur  
Version: 2.0 (Edition Pro)  
Public: Utilisateurs finaux (debutant a intermediaire)  
Date: Avril 2026

---

## Couverture

Ce document vous accompagne pas a pas pour:
- utiliser HiddenInk rapidement sans connaissance technique avancee;
- choisir le bon mode d'injection/extraction selon votre besoin;
- analyser la qualite et diagnostiquer les erreurs frequentes.

## Sommaire visuel

| Partie | Ce que vous allez faire |
|---|---|
| Demarrage | Lancer l'application et realiser une premiere operation |
| Injection | Cacher texte/image/fichier dans PNG/BMP/WAV |
| Extraction | Recuperer proprement les donnees cachees |
| Analyse | Evaluer discretion, qualite, signatures de payload |
| Outils | Generer/copier la cle XOR |
| Depannage | Resoudre les erreurs les plus courantes |

## Checklist de prise en main (2 minutes)

- [ ] Ouvrir `HiddenInkGUI`
- [ ] Aller dans `Injection`
- [ ] Choisir un scenario simple (texte dans PNG)
- [ ] Lancer l'injection
- [ ] Verifier le `Journal`
- [ ] Aller dans `Extraction` et recuperer le contenu

## Charte de lecture

Encadre Bonnes pratiques:
> Utilisez toujours une copie de vos fichiers originaux avant toute injection.

Encadre Important:
> Conservez la meme position de bit et la meme cle (si activee) pour l'extraction.

## 1. Vue d'ensemble

HiddenInk est un outil de stéganographie qui permet de:
- injecter du contenu (texte/fichier/image) dans des médias porteurs;
- extraire ce contenu ultérieurement;
- analyser les médias pour évaluer la qualité et la discrétion.

Le projet propose:
- un mode interface graphique (GUI Qt);
- un mode CLI (ligne de commande) selon les fonctionnalités disponibles.

Formats principaux:
- PNG: injection/extraction texte et image;
- BMP: injection/extraction fichier;
- WAV: injection/extraction fichier;
- Analyse: histogramme, MSE/PSNR, analyse LSB, entropie, détection de payload.

## 2. Concepts utiles (sans jargon)

- Media porteur: fichier dans lequel on cache des données (image/audio).
- Payload: données cachées (texte, image, fichier).
- LSB: méthode qui modifie les bits les moins visibles pour limiter l'impact visuel/sonore.
- Clé (hex): valeur optionnelle utilisée pour masquer/chiffrer le payload (XOR dans ce projet).

## 3. Démarrage rapide

1. Lancez `HiddenInkGUI`.
2. Ouvrez l'onglet `Injection`.
3. Choisissez un scénario:
- Injection texte PNG
- Injection image PNG
- Injection BMP
- Injection WAV
4. Sélectionnez vos fichiers source et destination.
5. Cliquez sur le bouton principal `Injecter ...`.
6. Vérifiez le résultat dans `Journal`.
7. Passez à l'onglet `Extraction` pour récupérer le contenu.

## 3.1 Important si vous utilisez Qt Creator

Avant de compiler le projet GUI dans Qt Creator, faites cette étape une fois:

1. Dans l'arborescence du projet, faites un clic droit sur le dossier GUI.
2. Cliquez sur Run qmake.
3. Ensuite seulement, lancez Build (ou Rebuild).

Pourquoi:
- cela régénère correctement les fichiers Makefile du projet Qt;
- cela évite des erreurs de compilation/lien après modification du fichier GUI.pro.

## 4. Utilisation détaillée par scénario

### 4.1 Injection texte dans PNG

Objectif: cacher un texte dans une image PNG.

Champs:
- `Image source`: image porteuse PNG.
- `Fichier texte à injecter`: fichier `.txt`.
- `Bits/canal`: profondeur d'injection (1-4).
- `Image sortie`: PNG de sortie.

Conseils:
- 1 bit/canal: meilleure discrétion, capacité plus faible.
- 2-4 bits/canal: plus de capacité, risque visuel plus élevé.

### 4.2 Injection image dans PNG

Objectif: cacher une image PNG dans une autre.

Champs:
- `Conteneur`: image porteuse.
- `Image à injecter`: image secrète.
- `Bits/canal`: 1-4.
- `Sortie`: PNG final.

Comportement utile:
- le système peut redimensionner l'image secrète si la capacité est insuffisante;
- le journal précise si un redimensionnement a été appliqué.

### 4.3 Extraction PNG

Deux flux:
- extraction de texte;
- extraction d'image.

Pour chacun:
1. sélectionner l'image stéganographiée;
2. choisir le fichier de sortie;
3. cliquer sur `Extraire ...`.

Résultat:
- message de succès dans `Journal`;
- aperçu du texte extrait dans `Analyse` quand applicable.

### 4.4 Injection BMP

Objectif: cacher un fichier dans une image BMP.

Champs:
- `Image BMP`: porteur;
- `Fichier à injecter`: `.txt`, `.bmp`, `.png`;
- `Position bit`: 0-7 (0 = LSB, recommandé);
- `Clé (hex)`: optionnelle;
- `Sortie BMP`.

Bonnes pratiques:
- commencez avec bit=0;
- gardez une copie de l'original;
- conservez la clé exacte si utilisée.

### 4.5 Extraction BMP

Champs:
- `Image BMP`;
- `Position bit`;
- `Clé (hex)` si utilisée à l'injection;
- `Sortie` (optionnelle selon mode).

Important:
- une mauvaise clé ou une mauvaise position de bit peut produire un fichier corrompu.

### 4.6 Injection WAV

Objectif: cacher un fichier dans un audio WAV.

Champs:
- `Audio WAV`;
- `Fichier à injecter`;
- `Position bit` 0-7;
- `Clé (hex)` optionnelle;
- `Sortie WAV`.

Conseils audio:
- privilégier bit=0 pour minimiser l'impact sonore;
- tester la qualité du son de sortie avant diffusion.

### 4.7 Extraction WAV

Même logique que BMP:
- même `Position bit` qu'à l'injection;
- même clé (si utilisée);
- sortie choisie ou auto-générée.

## 5. Onglet Analyse

### 5.1 Comparaison MSE/PSNR

Utilité:
- comparer visuellement et mathématiquement original vs stégo.

Interprétation rapide:
- MSE faible = faible différence;
- PSNR élevé = bonne qualité.

### 5.2 Détection LSB

Utilité:
- repérer une distribution potentiellement anormale des bits LSB.

Limite:
- ce n'est pas une preuve absolue; c'est un indicateur.

### 5.3 Entropie

Utilité:
- estimer la complexité du fichier (texte clair vs compressé/chiffré).

### 5.4 Détection payload

Utilité:
- reconnaître le type de fichier extrait via signatures (magic bytes).

## 6. Onglet Histogramme

Page dédiée pour:
- charger une image;
- générer/afficher la distribution.

Usage:
- utile pour comparer avant/après injection;
- utile pour détecter des anomalies de distribution.

## 7. Onglet Outils

- Génération de clé XOR en hexadécimal.
- Copie rapide de la clé dans le presse-papier.

Attention:
- XOR n'est pas du chiffrement fort moderne.

## 8. Onglet Journal

Le journal affiche:
- opérations réalisées;
- succès/erreurs;
- informations contextuelles (fichier, extraction, etc.).

En cas de problème, commencez par ici.

## 9. Dépannage rapide

### Problème: l'extraction échoue

Vérifiez:
- le bon fichier source;
- la bonne `Position bit`;
- la même clé (si utilisée);
- le format pris en charge.

### Problème: qualité dégradée

Actions:
- réduire `Bits/canal` (PNG) ou `Position bit` (BMP/WAV = 0 recommandé);
- utiliser un porteur plus grand.

### Problème: fichier extrait illisible

Causes probables:
- clé incorrecte;
- bit position incorrect;
- payload tronqué (capacité insuffisante).

## 10. Recommandations de sécurité

- Ne partagez pas la clé dans le même canal que le média.
- Ne considérez pas XOR comme une protection cryptographique forte.
- Préférez des fichiers porteurs de taille confortable.
- Conservez des originaux non modifiés.

## 11. Glossaire

- Injection: action de cacher des données.
- Extraction: action de récupérer les données cachées.
- Porteur: fichier hôte (image/audio).
- Payload: données cachées.
- LSB: bits de poids faible.
