#ifndef PNG_EXTRACT_H
#define PNG_EXTRACT_H

#include <string>

// Extrait un texte d'une image PNG
int extractPNGText(const std::string& inputPath, const std::string& outputPath = "");

// Extrait une image d'une image PNG
int extractPNGImage(const std::string& inputPath, const std::string& outputPath);


#endif // PNG_EXTRACT_H
