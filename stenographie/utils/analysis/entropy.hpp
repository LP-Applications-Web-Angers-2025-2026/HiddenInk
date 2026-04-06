#pragma once
#include <vector>
#include <string>
#include <array>
#include <cmath>
#include <algorithm>
#include <numeric>

// ─────────────────────────────────────────────
// Résultat de la carte d'entropie par blocs
// ─────────────────────────────────────────────
struct EntropyMap {
    std::vector<double> blocks; // entropie de chaque bloc
    int gridW = 0, gridH = 0;  // dimensions de la grille
    double globalEntropy = 0.0;
    double minE = 0.0, maxE = 0.0, meanE = 0.0;
};

// ─────────────────────────────────────────────
// Entropie de Shannon d'un tableau d'octets (0–8 bits)
// ─────────────────────────────────────────────
inline double shannonEntropy(const unsigned char* data, size_t size) {
    if (size == 0) return 0.0;
    std::array<int, 256> freq{};
    freq.fill(0);
    for (size_t i = 0; i < size; ++i) freq[data[i]]++;
    double h = 0.0;
    for (int f : freq) {
        if (f > 0) {
            double p = double(f) / double(size);
            h -= p * std::log2(p);
        }
    }
    return h;
}

// ─────────────────────────────────────────────
// Analyse bloc par bloc (blockSize en pixels)
// Retourne une EntropyMap avec la grille + stats
// ─────────────────────────────────────────────
inline EntropyMap analyzeEntropyMap(const unsigned char* data, int w, int h, int c, int blockSize = 16) {
    EntropyMap result;
    result.gridW = (w + blockSize - 1) / blockSize;
    result.gridH = (h + blockSize - 1) / blockSize;
    result.blocks.assign(size_t(result.gridW) * result.gridH, 0.0);

    std::vector<unsigned char> tmp;
    tmp.reserve(size_t(blockSize) * blockSize * c);

    for (int by = 0; by < result.gridH; ++by) {
        for (int bx = 0; bx < result.gridW; ++bx) {
            tmp.clear();
            for (int py = by * blockSize; py < std::min(h, (by+1)*blockSize); ++py)
                for (int px = bx * blockSize; px < std::min(w, (bx+1)*blockSize); ++px)
                    for (int ch = 0; ch < c; ++ch)
                        tmp.push_back(data[(py*w+px)*c+ch]);
            result.blocks[size_t(by)*result.gridW + bx] = shannonEntropy(tmp.data(), tmp.size());
        }
    }

    result.globalEntropy = shannonEntropy(data, size_t(w)*h*c);
    if (!result.blocks.empty()) {
        result.minE = *std::min_element(result.blocks.begin(), result.blocks.end());
        result.maxE = *std::max_element(result.blocks.begin(), result.blocks.end());
        result.meanE = std::accumulate(result.blocks.begin(), result.blocks.end(), 0.0)
                       / double(result.blocks.size());
    }
    return result;
}

// ─────────────────────────────────────────────
// Interprétation de l'entropie globale
// ─────────────────────────────────────────────
inline std::string interpretEntropy(double e) {
    if (e > 7.8) return "SUSPECT — données chiffrées ou compressées cachées (entropie très haute)";
    if (e > 7.2) return "Attention — entropie haute, possible payload chiffré";
    if (e > 5.5) return "Normale — image naturelle";
    if (e > 3.0) return "Basse — image uniforme";
    return "Très basse — zones parfaitement plates";
}

// ─────────────────────────────────────────────
// Test Chi-carré (Westfeld–Pfitzmann simplifié)
// Score bas ≈ paires équilibrées ≈ LSB modifié
// Score élevé ≈ distribution naturelle
// ─────────────────────────────────────────────
inline double chiSquareScore(const unsigned char* data, size_t size) {
    if (size < 256) return -1.0;
    std::array<double, 256> freq{};
    freq.fill(0.0);
    for (size_t i = 0; i < size; ++i) freq[data[i]] += 1.0;
    double chi2 = 0.0;
    for (int k = 0; k < 128; ++k) {
        double expected = (freq[2*k] + freq[2*k+1]) / 2.0;
        if (expected < 1.0) continue;
        double d0 = freq[2*k]   - expected;
        double d1 = freq[2*k+1] - expected;
        chi2 += (d0*d0 + d1*d1) / expected;
    }
    return chi2;
}

inline std::string interpretChiSquare(double chi2, size_t sampleSize) {
    if (chi2 < 0) return "Échantillon trop petit pour le test";
    // Normalise par la taille : valeur attendue ≈ 127 DOF pour données uniformes
    // On compare au rapport chi2 / (sampleSize/1000)
    double norm = chi2 * 1000.0 / double(sampleSize);
    if (norm < 0.5)  return "SUSPECT — distribution quasi-uniforme (LSB très probablement modifié)";
    if (norm < 2.0)  return "Potentiellement modifié";
    if (norm < 10.0) return "Distribution plausible — stéganographie possible mais non certaine";
    return "Distribution naturelle — pas de stéganographie LSB détectée";
}

// ─────────────────────────────────────────────
// Blocs suspects (entropie > seuil)
// ─────────────────────────────────────────────
inline int countSuspiciousBlocks(const EntropyMap& map, double threshold = 7.5) {
    int count = 0;
    for (double e : map.blocks) if (e >= threshold) ++count;
    return count;
}
