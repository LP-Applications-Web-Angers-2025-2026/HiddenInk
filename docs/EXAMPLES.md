# 📚 Exemples d'utilisation de HiddenInk

Ce document présente des exemples concrets d'utilisation de HiddenInk avec des explications détaillées.

---

## 🖼️ Scénario 1 : Cacher une photo de profil dans une image de couverture

**Contexte** : Vous voulez cacher votre photo de profil dans votre image de couverture.

```bash
# Étape 1 : Cacher la photo de profil
./main hide-image couverture.png profil.png resultat.png

# Sortie attendue :
# 📊 INFORMATIONS :
#    Image porteuse : 1920x1080 (3 canaux)
#    Image secrète : 400x400 (3 canaux)
#    Capacité disponible : 6220800 bits
#    Capacité nécessaire : 384000 bits
#    Utilisation : 6.18%
#    Bits par canal : 1
# ✅ Image cachée avec succès dans resultat.png

# Étape 2 : Vérifier la qualité
./main compare couverture.png resultat.png

# Sortie attendue :
# === COMPARAISON D'IMAGES ===
# 📐 Dimensions : 1920x1080 (3 canaux)
# 📊 MÉTRIQUES DE QUALITÉ :
#    MSE : 0.0625
#    PSNR : 60.17 dB
# 💡 INTERPRÉTATION :
#    ✅ Différence IMPERCEPTIBLE à l'œil nu
#    Pixels modifiés : 384000 / 6220800 (6.17%)

# Étape 3 : Extraire la photo de profil
./main extract-image resultat.png profil_extrait.png

# Sortie attendue :
# ✅ Image extraite avec succès -> profil_extrait.png

# Étape 4 : Vérifier que l'extraction est parfaite
./main compare profil.png profil_extrait.png

# Sortie attendue :
# 💡 INTERPRÉTATION :
#    ✅ Les images sont IDENTIQUES (pixel par pixel)
```

---

## 📝 Scénario 2 : Message secret dans une photo de vacances

**Contexte** : Vous voulez envoyer un message secret à un ami via une photo innocente.

```bash
# Cacher le message
./main hide-text vacances.png "Rendez-vous demain 14h Place de la République" message_cache.png

# Sortie attendue :
# === CACHER UN MESSAGE TEXTE ===
# Image porteuse : vacances.png
# Message : "Rendez-vous demain 14h Place de la République"
# 
# 📊 INFORMATIONS :
#    Image porteuse : 1024x768 (3 canaux)
#    Longueur du message : 48 caractères
#    Capacité disponible : 2359296 bits
#    Capacité nécessaire : 552 bits
#    Utilisation : 0.02%
#    Bits par canal : 1
# ✅ Message caché avec succès dans message_cache.png

# Analyser l'image pour voir si c'est détectable
./main detect message_cache.png

# Sortie attendue :
# === ANALYSE DE STÉGANOGRAPHIE ===
# 📊 Distribution des LSB :
#    Bits à 0 : 1179820 (50.01%)
#    Bits à 1 : 1179476 (49.99%)
#    Ratio : 0.4999
# 💡 INTERPRÉTATION :
#    ✅ Distribution équilibrée (~50/50) - Naturelle ou stéganographie bien faite

# Extraire le message
./main extract-text message_cache.png

# Sortie attendue :
# ✅ MESSAGE EXTRAIT :
# ─────────────────────────────────────────
# Rendez-vous demain 14h Place de la République
# ─────────────────────────────────────────
```

---

## 🔍 Scénario 3 : Analyse comparative avant/après stéganographie

**Contexte** : Vous voulez comprendre l'impact de la stéganographie sur votre image.

```bash
# 1. Analyser l'image originale
./main histogram original.png

# Sortie :
# === ANALYSE FRÉQUENTIELLE (HISTOGRAMME) ===
# 📐 Dimensions : 800x600 (3 canaux)
# 
# 📊 CANAL Rouge :
#    Valeur min : 12
#    Valeur max : 243
#    Moyenne : 127.34
#    Écart-type : 45.67
#    Top 5 valeurs les plus fréquentes :
#      128 :     3456 pixels (2.40%)
#      127 :     3421 pixels (2.38%)
#      ...

# 2. Cacher une image
./main hide-image original.png secret.png stego.png

# 3. Analyser l'image stéganographiée
./main histogram stego.png

# 4. Comparer les deux
./main compare original.png stego.png

# === COMPARAISON D'IMAGES ===
# 📐 Dimensions : 800x600 (3 canaux)
# 
# 📊 MÉTRIQUES DE QUALITÉ :
#    MSE : 0.1234
#    PSNR : 57.21 dB
# 
# 💡 INTERPRÉTATION :
#    ✅ Différence IMPERCEPTIBLE à l'œil nu
#    Pixels modifiés : 48000 / 1440000 (3.33%)
```

---

## 🎯 Scénario 4 : Image trop grande - redimensionnement automatique

**Contexte** : Vous essayez de cacher une grande image dans une petite image.

```bash
./main hide-image petite.png grande.png resultat.png

# Sortie :
# ⚠️  L'image secrète (2048x1536) est trop grande pour l'image porteuse (800x600)
# 
# 🔄 REDIMENSIONNEMENT AUTOMATIQUE :
#    Taille originale : 2048x1536
#    Nouvelle taille : 566x424
#    Méthode : Interpolation bilinéaire (haute qualité)
# 
# 📊 INFORMATIONS :
#    Image porteuse : 800x600 (3 canaux)
#    Image secrète redimensionnée : 566x424 (3 canaux)
#    Capacité disponible : 1440000 bits
#    Capacité nécessaire : 720672 bits
#    Utilisation : 50.05%
#    Bits par canal : 1
# 
# ✅ Image cachée avec succès dans resultat.png
```

---

## 🚨 Scénario 5 : Tentative d'extraction sans stéganographie

**Contexte** : Vous essayez d'extraire un message d'une image normale (sans stéganographie).

```bash
./main extract-text photo_normale.png

# Sortie :
# === EXTRAIRE UN MESSAGE TEXTE ===
# Image source : photo_normale.png
# 
# 🔍 Détection automatique...
# ❌ Erreur : impossible d'extraire le message.
```

---

## 💡 Scénario 6 : Workflow complet avec validation

**Contexte** : Workflow professionnel avec toutes les étapes de validation.

```bash
# Étape 1 : Analyser l'image porteuse
echo "=== 1. Analyse de l'image porteuse ==="
./main histogram carrier.png

# Étape 2 : Cacher le message
echo -e "\n=== 2. Encodage du message ==="
./main hide-text carrier.png "Information confidentielle" stego.png

# Étape 3 : Comparer original vs stego
echo -e "\n=== 3. Comparaison qualité ==="
./main compare carrier.png stego.png

# Étape 4 : Détecter d'éventuelles anomalies
echo -e "\n=== 4. Détection d'anomalies ==="
./main detect stego.png

# Étape 5 : Analyser l'histogramme du stego
echo -e "\n=== 5. Analyse de l'histogramme ==="
./main histogram stego.png

# Étape 6 : Extraire et valider
echo -e "\n=== 6. Extraction du message ==="
./main extract-text stego.png
```

---

## 🎨 Scénario 7 : Mode interactif pour débutants

```bash
./main

# Affichage :
# === STEGANOGRAPHIE AVANCEE ===
# 
# 1. Cacher une image dans une image
# 2. Extraire une image cachée
# 3. Cacher un message texte dans une image
# 4. Extraire un message texte
# 5. Comparer deux images (MSE/PSNR)
# 6. Analyser l'histogramme d'une image
# 7. Détecter une éventuelle stéganographie
# 
# Choix : 3

# === CACHER UN MESSAGE TEXTE ===
# Image porteuse : photos/beach.png
# Message à cacher : Ceci est un test
# Image de sortie (.png) : output/test.png
# 
# [... processus d'encodage ...]
# 
# ✅ Message caché avec succès dans output/test.png
```

---

## 📊 Interprétation des métriques

### MSE (Mean Squared Error)
- **0** : Images identiques pixel par pixel
- **< 0.1** : Imperceptible, qualité parfaite
- **< 1** : Excellente qualité, quasi-invisible
- **< 10** : Bonne qualité, légèrement visible
- **> 10** : Qualité dégradée, visible

### PSNR (Peak Signal-to-Noise Ratio)
- **∞ dB** : Images identiques
- **> 50 dB** : Imperceptible à l'œil nu
- **40-50 dB** : Excellente qualité
- **30-40 dB** : Bonne qualité
- **20-30 dB** : Qualité moyenne
- **< 20 dB** : Qualité faible

### Distribution des LSB
- **Ratio ≈ 0.50** (50/50) : Normal ou stéganographie bien faite
- **Ratio < 0.45 ou > 0.55** : Suspect, possible anomalie
- **Ratio < 0.30 ou > 0.70** : Très suspect

---

## ⚙️ Conseils et bonnes pratiques

### Pour un résultat optimal :

1. **Utilisez le mode automatique** : Le programme calcule automatiquement les meilleurs paramètres
2. **Privilégiez les images avec beaucoup de détails** : Plus de variations = meilleure dissimulation
3. **Évitez les images unies** : Ciel bleu, murs blancs, etc. = stéganographie plus visible
4. **Limitez la taille** : Plus le message est petit, moins c'est détectable
5. **Vérifiez toujours la qualité** : Utilisez `compare` après l'encodage
6. **Sauvegardez en PNG** : Format sans perte, préserve la stéganographie

### Limitations :

- Maximum 2 bits/canal pour préserver la qualité visuelle
- L'image secrète peut être redimensionnée automatiquement si trop grande
- Les formats avec compression (JPEG) détruisent la stéganographie
- Plus l'utilisation est élevée (%), plus c'est détectable

---

## 🔗 Voir aussi

- [README.md](README.md) - Documentation principale
- [LICENSE](LICENSE) - Licence du projet
