#ifndef PNG_HIDE_TEXT_H
#define PNG_HIDE_TEXT_H

#include <string>

// Cache un texte dans une image PNG
int hidePNGText(const std::string &carrierPath,
                const std::string &textOrFilePath,
                const std::string &outputPath, int bitsPerChannel = 0,
                const std::string &key = "");

#endif // PNG_HIDE_TEXT_H
