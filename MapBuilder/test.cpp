#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <SDL3/SDL_main.h>
#include <iostream>
#include <map>
#include <list>
#include <tuple>
#include <vector>
#include <array>
#include <cmath>
#include <string>
#include <fstream>
#include <unordered_map>
#include "files_images.h"

std::unordered_map<std::string, std::unordered_map<std::string, std::unordered_map<std::string, int>>> data_map =
{ 0, { ""0"",{  { "x_cord", 0 }, { "y_cord", 0 }, { "texture_value", 555 } },{ ""1"",{  { "x_cord", 32 }, { "y_cord", 0 }, { "texture_value", 555 } },{ ""2"",{  { "x_cord", 64 }, { "y_cord", 0 }, { "texture_value", 555 } },{ ""3"",{  { "x_cord", 96 }, { "y_cord", 0 }, { "texture_value", 555 } },{ ""4"",{  { "x_cord", 128 }, { "y_cord", 0 }, { "texture_value", 555 } },} {
 1, { ""0"",{  { "x_cord", 0 }, { "y_cord", 32 }, { "texture_value", 555 } },{ ""1"",{  { "x_cord", 32 }, { "y_cord", 32 }, { "texture_value", 555 } },{ ""2"",{  { "x_cord", 64 }, { "y_cord", 32 }, { "texture_value", 555 } },{ ""3"",{  { "x_cord", 96 }, { "y_cord", 32 }, { "texture_value", 555 } },{ ""4"",{  { "x_cord", 128 }, { "y_cord", 32 }, { "texture_value", 555 } },} {
2, { ""0"",{  { "x_cord", 0 }, { "y_cord", 64 }, { "texture_value", 555 } },{ ""1"",{  { "x_cord", 32 }, { "y_cord", 64 }, { "texture_value", 555 } },{ ""2"",{  { "x_cord", 64 }, { "y_cord", 64 }, { "texture_value", 555 } },{ ""3"",{  { "x_cord", 96 }, { "y_cord", 64 }, { "texture_value", 555 } },{ ""4"",{  { "x_cord", 128 }, { "y_cord", 64 }, { "texture_value", 555 } },}};

std::unordered_map<std::string, std::unordered_map<std::string, std::unordered_map<std::string, int>>> data_map =
{ 0, { 0, { { }, }

// { 1 , { 0, { "x", 0 } , { } { } , 