#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include "map.h"
#include <fstream>

// build process
// map init
// create map
std::unordered_map<std::string, std::vector<int>> Map;
// create data map   ***IMPORTANT***
int map_width = 100;
int map_height = 100;
int grid_start_x = 0;
int grid_start_y = 0;
int grid_pixel_size = 32;
int iteration_width_map = 0;
int default_texture_value = 555;
std::unordered_map<std::string, std::unordered_map<std::string, std::unordered_map<std::string, int>>> data_map;
std::unordered_map<std::string, std::unordered_map<std::string, int>> second_level_map_;
std::unordered_map<std::string, int> low_level_map_ =
{
    { "x_cord", 0 },
    { "y_cord", 0 },
    { "texture_value", default_texture_value }
};
// end*

void data_map_create()
{
    std::vector<int> texture_vector_from_bin;
    int texture_value_from_bin;
    std::ifstream inside_file("../What -p_/map_3.bin", std::ios::binary);
    if (!inside_file)
    {
        std::cout << "error loading bin file \"inside_file\"";
    }
    for (int i = 0; i < map_width * map_height; i++)
    {
        inside_file.read(reinterpret_cast<char*>(&texture_value_from_bin), sizeof(texture_value_from_bin));
        texture_vector_from_bin.push_back(texture_value_from_bin);
    }
    inside_file.close();

    for (int i = 0; i < map_height; i++)
    {
        std::string ii = std::to_string(i);
        for (int iii = 0; iii < map_width; iii++)
        {
            std::string iiii = std::to_string(iii);
            second_level_map_.insert({ iiii, low_level_map_ });
        }
        data_map.insert({ ii, second_level_map_ });
    }

    for (int i = 0; i < map_height; i++)
    {
        for (int ii = 0; ii < map_width; ii++)
        {
            std::string iii = std::to_string(i);
            std::string iiii = std::to_string(ii);
            data_map[iii][iiii]["x_cord"] = grid_start_x;
            data_map[iii][iiii]["y_cord"] = grid_start_y;
            data_map[iii][iiii]["texture_value"] = texture_vector_from_bin[ii + iteration_width_map];
            grid_start_x += grid_pixel_size;
        }
        iteration_width_map += map_width;
        grid_start_x = 0;
        grid_start_y += grid_pixel_size;
    }
    grid_start_x = 0;
    grid_start_y = 0;
}
