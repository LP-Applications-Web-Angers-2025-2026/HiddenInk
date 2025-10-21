#ifndef PNG_HIDE_IMAGE_H
#define PNG_HIDE_IMAGE_H

#include <string>

// Cache une image dans une autre image PNG
int hidePNGImage(const std::string& carrierPath, const std::string& secretPath, 
                 const std::string& outputPath, int bitsPerChannel = 0);

#endif // PNG_HIDE_IMAGE_H
