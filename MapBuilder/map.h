#ifndef MAP__T_H
#define MAP__T_H

#include <string>
#include <map>
#include <list>
#include <array>
#include <vector>
#include <unordered_map>

extern std::unordered_map<std::string, std::unordered_map<std::string, std::unordered_map<std::string, int>>> data_map;
extern void data_map_create();

extern int grid_pixel_size_by_loaded_map;

#endif // !MAP__T_H