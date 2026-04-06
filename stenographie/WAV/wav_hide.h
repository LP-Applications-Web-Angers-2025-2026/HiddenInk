#ifndef WAV_HIDE_H
#define WAV_HIDE_H

#include <string>

/**
 * Cache un fichier dans un audio WAV via modification LSB des échantillons PCM.
 * @param carrierPath  Fichier WAV porteur
 * @param filePath     Fichier à cacher (.txt / .bmp / .png / .sh / ...)
 * @param outputPath   WAV de sortie
 * @param bitPos       Position du bit modifié (0 = LSB, qualité max)
 * @param key          Clé hex pour chiffrement XOR (vide = pas de chiffrement)
 * @return true si succès
 */
bool hideInWAV(const std::string& carrierPath,
               const std::string& filePath,
               const std::string& outputPath,
               int bitPos = 0,
               const std::string& key = "");

/**
 * Extrait un fichier caché dans un WAV stéganographié.
 * @param inputPath   WAV source
 * @param bitPos      Position du bit utilisé lors du cachage
 * @param key         Clé hex (vide si aucun chiffrement)
 * @param outputPath  Chemin de sortie (auto-détecté si vide)
 * @return Message de résultat (commence par "[HiddenInk]") ou code erreur ("1"–"4")
 */
std::string extractFromWAV(const std::string& inputPath,
                            int bitPos = 0,
                            const std::string& key = "",
                            const std::string& outputPath = "");

#endif // WAV_HIDE_H
