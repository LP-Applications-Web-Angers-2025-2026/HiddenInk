#include "wav_hide.h"
#include "../utils/utils_bin.h"
#include "../utils/encrypt/encrypt.h"

#include <iostream>
#include <fstream>
#include <vector>
#include <bitset>
#include <cstdint>

using namespace std;

// ─────────────────────────────────────────────
// Trouve l'offset du chunk "data" dans un WAV
// (gère les WAV non-standards avec chunks fmt étendus)
// ─────────────────────────────────────────────
static size_t findDataChunkOffset(const vector<unsigned char>& wav) {
    // Structure RIFF : skip "RIFF" (4) + size (4) + "WAVE" (4) = 12 bytes
    size_t pos = 12;
    while (pos + 8 <= wav.size()) {
        // Lire l'identifiant du chunk (4 octets)
        char id[5] = {};
        id[0] = char(wav[pos]);   id[1] = char(wav[pos+1]);
        id[2] = char(wav[pos+2]); id[3] = char(wav[pos+3]);

        std::uint32_t chunkSize = std::uint32_t(wav[pos+4])
                    | std::uint32_t(wav[pos+5]) << 8
                    | std::uint32_t(wav[pos+6]) << 16
                    | std::uint32_t(wav[pos+7]) << 24;

        if (string(id) == "data")
            return pos + 8; // skip id(4) + size(4)

        pos += 8 + chunkSize;
        // Alignement sur 2 octets (spec RIFF)
        if (chunkSize & 1) ++pos;
    }
    return 44; // fallback standard PCM
}

// ─────────────────────────────────────────────
// CACHER
// ─────────────────────────────────────────────
bool hideInWAV(const string& carrierPath, const string& filePath,
               const string& outputPath, int bitPos, const string& key)
{
    // Lecture du WAV porteur
    ifstream wavFile(carrierPath, ios::binary);
    if (!wavFile) {
        cerr << "[HiddenInk] Erreur : impossible d'ouvrir " << carrierPath << endl;
        return false;
    }
    vector<unsigned char> data((istreambuf_iterator<char>(wavFile)), {});
    wavFile.close();

    // Vérification header RIFF/WAVE
    if (data.size() < 44
        || data[0]!='R' || data[1]!='I' || data[2]!='F' || data[3]!='F'
        || data[8]!='W' || data[9]!='A' || data[10]!='V' || data[11]!='E') {
        cerr << "[HiddenInk] Erreur : " << carrierPath << " n'est pas un fichier WAV valide." << endl;
        return false;
    }

    size_t headerSize = findDataChunkOffset(data);

    // Construction du message binaire : signature + balise ouv + contenu + balise ferm
    string binMessage;
    binMessage += getSignatureBinary();
    binMessage += getBaliseBinary(true);
    binMessage += lireFichierKey(filePath, key);
    binMessage += getBaliseBinary(false);

    // Vérification de la capacité
    if (!messageCanFitInImage(binMessage, data, headerSize))
        return false;

    // Injection dans les bits des échantillons PCM
    unsigned char mask = static_cast<unsigned char>(~(1 << bitPos));
    vector<unsigned char> modified = data;
    for (size_t i = 0; i < binMessage.size(); ++i) {
        modified[headerSize + i] &= mask;
        modified[headerSize + i] |= static_cast<unsigned char>((binMessage[i] - '0') << bitPos);
    }

    // Écriture du WAV modifié
    ofstream out(outputPath, ios::binary);
    if (!out) {
        cerr << "[HiddenInk] Erreur : impossible d'écrire " << outputPath << endl;
        return false;
    }
    out.write(reinterpret_cast<const char*>(modified.data()), modified.size());
    out.close();

    // Sauvegarde de la clé si fournie
    saveKeyFile(outputPath, key);

    cout << "[HiddenInk] Fichier caché dans le WAV : " << outputPath << endl;
    return true;
}

// ─────────────────────────────────────────────
// EXTRAIRE
// ─────────────────────────────────────────────
string extractFromWAV(const string& inputPath, int bitPos,
                      const string& key, const string& outputPath)
{
    ifstream file(inputPath, ios::binary);
    if (!file) return "1";

    vector<unsigned char> data((istreambuf_iterator<char>(file)), {});
    file.close();

    if (data.size() < 44
        || data[0]!='R' || data[1]!='I' || data[2]!='F' || data[3]!='F'
        || data[8]!='W' || data[9]!='A' || data[10]!='V' || data[11]!='E')
        return "1";

    size_t headerSize = findDataChunkOffset(data);

    // Vérification de la signature
    const size_t sigSize = getSignatureBinarySize();
    string sigBits;
    for (size_t i = 0; i < sigSize && headerSize + i < data.size(); ++i)
        sigBits += char(((data[headerSize + i] >> bitPos) & 0x01) + '0');

    if (sigBits != getSignatureBinary()) return "2"; // pas de signature

    // Lecture de tous les bits utiles
    string bitsLus;
    bitsLus.reserve(data.size() - headerSize);
    for (size_t i = headerSize; i < data.size(); ++i)
        bitsLus += char(((data[i] >> bitPos) & 0x01) + '0');

    // Recherche des balises
    const string balOuv  = getBaliseBinary(true);
    const string balFerm = getBaliseBinary(false);
    size_t posOuv  = bitsLus.find(balOuv);
    size_t posFerm = bitsLus.find(balFerm, posOuv);
    if (posOuv == string::npos || posFerm == string::npos) return "3";

    string cipherBits = bitsLus.substr(posOuv + balOuv.size(),
                                       posFerm - posOuv - balOuv.size());

    // Conversion bits → octets
    string cipher;
    for (size_t i = 0; i + 8 <= cipherBits.size(); i += 8) {
        bitset<8> b(cipherBits.substr(i, 8));
        cipher += static_cast<char>(b.to_ulong());
    }

    // Déchiffrement optionnel
    string message = key.empty() ? cipher : xor_encrypt(cipher, hex_to_key(key));

    // Détection automatique de l'extension par magic bytes
    string extension = ".bin";
    auto ub = [&](int i) { return static_cast<unsigned char>(message[i]); };
    if (message.size() >= 4 && ub(0)==0x7F && ub(1)=='E' && ub(2)=='L' && ub(3)=='F')
        extension = ".elf";
    else if (message.size() >= 2 && ub(0)=='M' && ub(1)=='Z')
        extension = ".exe";
    else if (message.size() >= 2 && ub(0)=='#' && ub(1)=='!')
        extension = ".sh";
    else if (message.size() >= 4 && ub(0)=='P' && ub(1)=='K' && ub(2)==0x03 && ub(3)==0x04)
        extension = ".zip";
    else if (message.size() >= 4 && ub(0)=='%' && ub(1)=='P' && ub(2)=='D' && ub(3)=='F')
        extension = ".pdf";
    else if (message.size() >= 2 && ub(0)==0x42 && ub(1)==0x4D)
        extension = ".bmp";
    else if (message.size() >= 8 && message.substr(0,8) == "\x89PNG\r\n\x1a\n")
        extension = ".png";
    else {
        size_t n = std::min(message.size(), size_t(256));
        size_t printable = 0;
        for (size_t i = 0; i < n; ++i) {
            unsigned char c = ub(int(i));
            if ((c >= 32 && c < 127) || c=='\n' || c=='\r' || c=='\t') ++printable;
        }
        if (printable > n * 8 / 10) extension = ".txt";
    }

    const string effectiveOut = outputPath.empty() ? "./wav_extrait" + extension : outputPath;
    ofstream outFile(effectiveOut, ios::binary);
    if (!outFile) return "4";
    outFile.write(message.data(), message.size());
    outFile.close();

    return "[HiddenInk] Extrait dans " + effectiveOut;
}
