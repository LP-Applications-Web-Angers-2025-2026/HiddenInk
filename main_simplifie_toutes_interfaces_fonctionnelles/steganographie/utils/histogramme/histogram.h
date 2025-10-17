#ifndef HISTOGRAM_H
#define HISTOGRAM_H

#include <vector>
#include <string>

std::vector<unsigned char> LireBMP(const std::string& chemin);
std::vector<int> histogramme(const std::vector<unsigned char>& imageData, size_t headerSize);
void printHistogram(const std::vector<int>& histo);
void saveHistogramBMP(const std::vector<int>& histo, const std::string& filename, int width = 800, int height = 400);
void processHistogram(const std::string& inputPath);

#endif // HISTOGRAM_H
