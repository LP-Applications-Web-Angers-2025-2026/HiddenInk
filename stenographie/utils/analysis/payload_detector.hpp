#pragma once
#include <string>
#include <vector>
#include <cctype>
#include <algorithm>

// ─────────────────────────────────────────────
// Types de payload reconnus par magic bytes
// ─────────────────────────────────────────────
enum class PayloadType {
    Unknown,  // données binaires non identifiées
    ELF,      // exécutable Linux
    PE,       // exécutable Windows (.exe / .dll)
    Script,   // script shell / Python (#!)
    ZIP,      // archive ZIP / JAR / APK / DOCX
    PDF,      // document PDF
    PNG,      // image PNG
    BMP,      // image BMP
    JPEG,     // image JPEG
    WAV,      // audio WAV PCM
    Text      // texte lisible
};

// ─────────────────────────────────────────────
// Détection par magic bytes
// ─────────────────────────────────────────────
inline PayloadType detectPayloadType(const std::vector<unsigned char>& data) {
    const size_t sz = data.size();

    // Exécutable ELF  : 0x7F 'E' 'L' 'F'
    if (sz >= 4 && data[0]==0x7F && data[1]=='E' && data[2]=='L' && data[3]=='F')
        return PayloadType::ELF;

    // Exécutable PE   : 'M' 'Z'
    if (sz >= 2 && data[0]=='M' && data[1]=='Z')
        return PayloadType::PE;

    // Archive ZIP     : 'P' 'K' 0x03 0x04
    if (sz >= 4 && data[0]=='P' && data[1]=='K' && data[2]==0x03 && data[3]==0x04)
        return PayloadType::ZIP;

    // PDF             : '%' 'P' 'D' 'F'
    if (sz >= 4 && data[0]=='%' && data[1]=='P' && data[2]=='D' && data[3]=='F')
        return PayloadType::PDF;

    // PNG             : 0x89 'P' 'N' 'G' \r \n 0x1A \n
    if (sz >= 8 && data[0]==0x89 && data[1]=='P' && data[2]=='N' && data[3]=='G')
        return PayloadType::PNG;

    // BMP             : 'B' 'M'
    if (sz >= 2 && data[0]=='B' && data[1]=='M')
        return PayloadType::BMP;

    // JPEG            : 0xFF 0xD8 0xFF
    if (sz >= 3 && data[0]==0xFF && data[1]==0xD8 && data[2]==0xFF)
        return PayloadType::JPEG;

    // WAV             : 'R' 'I' 'F' 'F' + 'W' 'A' 'V' 'E'
    if (sz >= 12 && data[0]=='R' && data[1]=='I' && data[2]=='F' && data[3]=='F'
                 && data[8]=='W' && data[9]=='A' && data[10]=='V' && data[11]=='E')
        return PayloadType::WAV;

    // Script shebang  : '#' '!'
    if (sz >= 2 && data[0]=='#' && data[1]=='!')
        return PayloadType::Script;

    // Texte lisible   : ≥80% de caractères imprimables
    size_t check = std::min(sz, size_t(512));
    size_t printable = 0;
    for (size_t i = 0; i < check; ++i) {
        unsigned char c = data[i];
        if ((c >= 32 && c < 127) || c=='\n' || c=='\r' || c=='\t') ++printable;
    }
    if (printable > check * 8 / 10) return PayloadType::Text;

    return PayloadType::Unknown;
}

// ─────────────────────────────────────────────
// Overload depuis un fichier sur disque
// ─────────────────────────────────────────────
#include <fstream>
inline PayloadType detectPayloadTypeFromFile(const std::string& path) {
    std::ifstream f(path, std::ios::binary);
    if (!f) return PayloadType::Unknown;
    std::vector<unsigned char> header(512);
    f.read(reinterpret_cast<char*>(header.data()), 512);
    header.resize(size_t(f.gcount()));
    return detectPayloadType(header);
}

// ─────────────────────────────────────────────
// Représentation textuelle
// ─────────────────────────────────────────────
inline std::string payloadTypeToString(PayloadType t) {
    switch (t) {
        case PayloadType::ELF:    return "Exécutable ELF (Linux)";
        case PayloadType::PE:     return "Exécutable PE (Windows .exe/.dll)";
        case PayloadType::Script: return "Script (#! shebang — shell/Python/Perl…)";
        case PayloadType::ZIP:    return "Archive ZIP / JAR / APK / DOCX";
        case PayloadType::PDF:    return "Document PDF";
        case PayloadType::PNG:    return "Image PNG";
        case PayloadType::BMP:    return "Image BMP";
        case PayloadType::JPEG:   return "Image JPEG";
        case PayloadType::WAV:    return "Audio WAV PCM";
        case PayloadType::Text:   return "Texte lisible";
        default:                  return "Inconnu (données binaires brutes)";
    }
}

// ─────────────────────────────────────────────
// Niveau de danger
// ─────────────────────────────────────────────
inline bool isExecutablePayload(PayloadType t) {
    return t == PayloadType::ELF || t == PayloadType::PE || t == PayloadType::Script;
}

inline std::string dangerLevel(PayloadType t) {
    if (isExecutablePayload(t))  return "DANGER — payload exécutable détecté !";
    if (t == PayloadType::ZIP)   return "Attention — archive (peut contenir des exécutables)";
    if (t == PayloadType::PDF)   return "Attention — PDF (possible macro / exploit)";
    if (t == PayloadType::Unknown) return "Inconnu — analyser manuellement";
    return "OK — media ou texte a priori inoffensif";
}

// ─────────────────────────────────────────────
// Hex dump (16 premiers octets)
// ─────────────────────────────────────────────
inline std::string hexDump(const std::vector<unsigned char>& data, size_t maxBytes = 16) {
    const char* hex = "0123456789ABCDEF";
    std::string result;
    size_t n = std::min(data.size(), maxBytes);
    for (size_t i = 0; i < n; ++i) {
        if (i > 0) result += ' ';
        result += hex[(data[i] >> 4) & 0xF];
        result += hex[data[i] & 0xF];
    }
    if (data.size() > maxBytes) result += " ...";
    return result;
}
