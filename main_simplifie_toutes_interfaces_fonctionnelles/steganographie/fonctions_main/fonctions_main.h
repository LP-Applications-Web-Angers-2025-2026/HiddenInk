#ifndef FONCTIONS_MAIN_H
#define FONCTIONS_MAIN_H

#include <string>
#include <filesystem>

int interact();
int hide(int argc, char* argv[]);
int extract(int argc, char* argv[]);
int bit_position(int argc, char* argv[], int arg_index);
bool validate_file_exists(const std::string& path);
void print_usage();
void processHistogram(const std::string& inputPath);

#endif // FONCTIONS_MAIN_H
