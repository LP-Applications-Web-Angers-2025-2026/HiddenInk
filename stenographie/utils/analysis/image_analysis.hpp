#pragma once
#include "../external/stb_image.h"
#include <iostream>
#include <vector>
#include <cmath>
#include <iomanip>
#include <map>
#include <algorithm>


using namespace std;

// ============ ANALYSE D'IMAGES ============

// Calculer le MSE (Mean Squared Error) entre deux images
inline double calculateMSE(unsigned char* img1, unsigned char* img2, int w, int h, int c)
{
    if (!img1 || !img2) return -1.0;

    double mse = 0.0;
    size_t totalPixels = w * h * c;

    for (size_t i = 0; i < totalPixels; ++i)
    {
        double diff = img1[i] - img2[i];
        mse += diff * diff;
    }

    mse /= totalPixels;
    return mse;
}

// Calculer le PSNR (Peak Signal-to-Noise Ratio)
inline double calculatePSNR(double mse, int maxValue = 255)
{
    if (mse == 0.0) return INFINITY;
    return 10.0 * log10((maxValue * maxValue) / mse);
}

// Analyser et comparer deux images
inline void compareImages(const string& path1, const string& path2)
{
    int w1, h1, c1, w2, h2, c2;

    auto img1 = stbi_load(path1.c_str(), &w1, &h1, &c1, 0);
    auto img2 = stbi_load(path2.c_str(), &w2, &h2, &c2, 0);

    if (!img1)
    {
        cerr << " Impossible de charger : " << path1 << "\n";
        return;
    }
    if (!img2)
    {
        cerr << " Impossible de charger : " << path2 << "\n";
        stbi_image_free(img1);
        return;
    }

    cout << "\n=== COMPARAISON D'IMAGES ===\n\n";

    // Vérifier les dimensions
    if (w1 != w2 || h1 != h2 || c1 != c2)
    {
        cout << "  Les images ont des dimensions différentes !\n";
        cout << "   Image 1 : " << w1 << "x" << h1 << " (" << c1 << " canaux)\n";
        cout << "   Image 2 : " << w2 << "x" << h2 << " (" << c2 << " canaux)\n";
        stbi_image_free(img1);
        stbi_image_free(img2);
        return;
    }

    cout << " Dimensions : " << w1 << "x" << h1 << " (" << c1 << " canaux)\n\n";

    // Calculer MSE
    double mse = calculateMSE(img1, img2, w1, h1, c1);
    double psnr = calculatePSNR(mse);

    cout << " MÉTRIQUES DE QUALITÉ :\n";
    cout << "   MSE (Mean Squared Error) : " << fixed << setprecision(4) << mse << "\n";
    cout << "   PSNR (Peak Signal-to-Noise Ratio) : ";

    if (isinf(psnr))
    {
        cout << " dB (images identiques)\n";
    }
    else
    {
        cout << fixed << setprecision(2) << psnr << " dB\n";
    }

    // Interprétation
    cout << "\n INTERPRÉTATION :\n";
    if (mse == 0.0)
    {
        cout << "    Les images sont IDENTIQUES (pixel par pixel)\n";
    }
    else if (psnr > 50)
    {
        cout << "    Différence IMPERCEPTIBLE à l'œil nu\n";
    }
    else if (psnr > 40)
    {
        cout << "    Excellente qualite, différence très difficile à percevoir\n";
    }
    else if (psnr > 30)
    {
        cout << "     Bonne qualite, différence légèrement visible\n";
    }
    else if (psnr > 20)
    {
        cout << "     Qualite moyenne, différence visible\n";
    }
    else
    {
        cout << "    Qualite faible, différence très visible\n";
    }

    // Calculer le pourcentage de pixels modifiés
    size_t pixelsModifies = 0;
    size_t totalPixels = w1 * h1 * c1;

    for (size_t i = 0; i < totalPixels; ++i)
    {
        if (img1[i] != img2[i])
        {
            pixelsModifies++;
        }
    }

    double pourcentage = (pixelsModifies * 100.0) / totalPixels;
    cout << "   Pixels modifiés : " << pixelsModifies << " / " << totalPixels
        << " (" << fixed << setprecision(2) << pourcentage << "%)\n";

    stbi_image_free(img1);
    stbi_image_free(img2);
}

// Générer l'histogramme d'une image (par canal)
inline void generateHistogram(const string& path)
{
    int w, h, c, minVal, maxVal, minCount, maxCount, val;
    double pourcentage, diff, ecartType, variance, moyenne;

    auto img = stbi_load(path.c_str(), &w, &h, &c, 0);

    if (!img)
    {
        cerr << " Impossible de charger : " << path << "\n";
        return;
    }

    cout << "\n ANALYSE FRÉQUENTIELLE (HISTOGRAMME)\n\n";
    cout << " Dimensions : " << w << "x" << h << " (" << c << " canaux)\n\n";

    // Créer les histogrammes pour chaque canal
    vector<vector<int>> histograms(c, vector<int>(256, 0));

    for (int i = 0; i < w * h; ++i)
    {
        for (int ch = 0; ch < c; ++ch)
        {
            histograms[ch][img[i * c + ch]]++;
        }
    }

    // Noms des canaux
    vector<string> channelNames;
    if (c == 1) channelNames = {"Gris"};
    else if (c == 3) channelNames = {"Rouge", "Vert", "Bleu"};
    else if (c == 4) channelNames = {"Rouge", "Vert", "Bleu", "Alpha"};
    else
    {
        for (int i = 0; i < c; ++i)
        {
            channelNames.push_back("Canal " + to_string(i));
        }
    }

    // Afficher les statistiques par canal
    for (int ch = 0; ch < c; ++ch)
    {
        cout << " CANAL " << channelNames[ch] << " :\n";

        // Calculer min, max, moyenne
        minVal = 0, maxVal = 0;
        moyenne = 0.0;
        minCount = w * h, maxCount = 0;

        for (val = 0; val < 256; ++val)
        {
            if (histograms[ch][val] > 0)
            {
                if (minVal == 0) minVal = val;
                maxVal = val;
            }
            moyenne += val * histograms[ch][val];

            if (histograms[ch][val] > maxCount) maxCount = histograms[ch][val];
            if (histograms[ch][val] > 0 && histograms[ch][val] < minCount) minCount = histograms[ch][val];
        }
        moyenne /= (w * h);

        // Calculer l'écart-type
        variance = 0.0;
        for (val = 0; val < 256; ++val)
        {
            diff = val - moyenne;
            variance += diff * diff * histograms[ch][val];
        }
        variance /= (w * h);
        ecartType = sqrt(variance);

        cout << "   Valeur min : " << minVal << "\n";
        cout << "   Valeur max : " << maxVal << "\n";
        cout << "   Moyenne : " << fixed << setprecision(2) << moyenne << "\n";
        cout << "   Écart-type : " << fixed << setprecision(2) << ecartType << "\n";

        // Afficher un mini-histogramme textuel pour les valeurs les plus fréquentes
        cout << "   Top 5 valeurs les plus fréquentes :\n";

        vector<pair<int, int>> topValues; // (valeur, fréquence)
        for (val = 0; val < 256; ++val)
        {
            topValues.push_back({val, histograms[ch][val]});
        }
        sort(topValues.begin(), topValues.end(),
             [](const pair<int, int>& a, const pair<int, int>& b) { return a.second > b.second; });

        for (int i = 0; i < min(5, (int)topValues.size()); ++i)
        {
            if (topValues[i].second == 0) break;
            pourcentage = (topValues[i].second * 100.0) / (w * h);
            cout << "      " << setw(3) << topValues[i].first << " : "
                << setw(8) << topValues[i].second << " pixels ("
                << fixed << setprecision(2) << pourcentage << "%)\n";
        }
        cout << "\n";
    }

    stbi_image_free(img);
}

// Analyser une image pour détecter d'éventuelles anomalies de stéganographie
inline void analyzeImageForSteganography(const string& path)
{
    int w, h, c;
    double ratio;

    auto img = stbi_load(path.c_str(), &w, &h, &c, 0);

    if (!img)
    {
        cerr << " Impossible de charger : " << path << "\n";
        return;
    }

    cout << "\n ANALYSE DE STÉGANOGRAPHIE \n\n";

    // Analyser la distribution des LSB (Least Significant Bits)
    int lsbCounts[2] = {0, 0}; // Compte des 0 et 1 dans les LSB

    for (int i = 0; i < w * h * c; ++i)
    {
        lsbCounts[img[i] & 1]++;
    }

    ratio = (double)lsbCounts[1] / (lsbCounts[0] + lsbCounts[1]);

    cout << " Distribution des LSB (Least Significant Bits) :\n";
    cout << "   Bits à 0 : " << lsbCounts[0] << " (" << fixed << setprecision(2) << (lsbCounts[0] * 100.0 / (w * h * c))
        << "%)\n";
    cout << "   Bits à 1 : " << lsbCounts[1] << " (" << fixed << setprecision(2) << (lsbCounts[1] * 100.0 / (w * h * c))
        << "%)\n";
    cout << "   Ratio : " << fixed << setprecision(4) << ratio << "\n\n";

    cout << " INTERPRÉTATION :\n";
    if (abs(ratio - 0.5) < 0.01)
    {
        cout << "    Distribution équilibrée (~50/50) - Naturelle ou stéganographie bien faite\n";
    }
    else if (abs(ratio - 0.5) < 0.05)
    {
        cout << "     Distribution légèrement déséquilibrée - Possible stéganographie\n";
    }
    else
    {
        cout << "     Distribution déséquilibrée - Suspect, mais peut être naturel\n";
    }

    cout << "\n   Note : Cette analyse n'est qu'indicative. Une image naturelle peut aussi\n";
    cout << "   avoir un ratio proche de 50/50. Pour confirmer, utilisez la comparaison\n";
    cout << "   avec l'image originale.\n";

    stbi_image_free(img);
}