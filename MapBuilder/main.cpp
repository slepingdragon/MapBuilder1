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
#include <shlobj.h>
#include <Windows.h>

// Texture map what is it?
// the texture map is where the textures are loaded in 
// these textures if a texture is clicked, the user
// can then place this texture.

int window_width = 1084;
int window_height = 1000;

int iterations = 0;
int fps = 60;
const int frameDelay = 1000 / fps;

SDL_Window* window = NULL;
SDL_Surface* screenSurface = NULL;
SDL_Surface* source;
SDL_Surface* destination;
SDL_Renderer* renderer;
SDL_Rect offset;
SDL_Texture* player_texture_loaded;
SDL_Surface* optimizedImg = NULL;

float global_offset_x = 0;
float global_offset_y = 0;

float mouse_x, mouse_y;

// key pressed keys
bool key_1_pressed = false;
bool lock_key_1_pressed = false;

bool mouse_button_down = false;

double calculated_angle_for_target = NULL;

const bool* keys = SDL_GetKeyboardState(NULL);

Uint32 frameStart;
int frameTime;

static bool init()
{
	if (SDL_Init(SDL_INIT_VIDEO) == false)
	{
		fprintf(stderr, "could not initialize sdl2: %s\n", SDL_GetError());
		return false;
	}
	window = SDL_CreateWindow("Map Builder", window_width, window_height, 0);
	if (window == NULL)
	{
		fprintf(stderr, "could not create window: %s\n", SDL_GetError());
		return false;
	}
	screenSurface = SDL_GetWindowSurface(window);
	if (screenSurface == NULL)
	{
		fprintf(stderr, "could not get window: %s\n", SDL_GetError());
		return false;
	}
	renderer = SDL_CreateSoftwareRenderer(screenSurface);
	if (renderer == NULL)
	{
		fprintf(stderr, "COULRD NOT INITIALIZE RENDERER");
	}
	return true;
}

static SDL_Surface* loadImage(std::string path)
{
	SDL_Surface* img = IMG_Load(path.c_str());
	if (img == NULL)
	{
		fprintf(stderr, "could not load image: %s\n", SDL_GetError());
		return NULL;
	}

	SDL_Surface* optimizedImg = SDL_ConvertSurface(img, SDL_PIXELFORMAT_RGBA32); // format enables transparency on image
	if (optimizedImg == NULL)
	{
		SDL_Log("Could not optimize img");
		return NULL;
	}

	SDL_DestroySurface(img);
	return optimizedImg;
}

static void close()
{
	SDL_DestroySurface(screenSurface); screenSurface = NULL;
	SDL_DestroyWindow(window); window = NULL;
	SDL_Quit();
}

// load ui
SDL_Surface* Right_Picker = loadImage("../MapBuilder/Image_Sprites/Ui_Sprites/Right_Picker.png");
SDL_Surface* Right_Side_Separater = loadImage("../MapBuilder/Image_Sprites/Ui_Sprites/Right_Side_Separater.png");
SDL_Surface* Right_Side_Separater_bottom = loadImage("../MapBuilder/Image_Sprites/Ui_Sprites/Right_Side_Separator_bottom.png");
//SDL_Surface* sixteen_pxGrid = loadImage("C:/Users/Brady J Bania/Desktop/dev/MapBuilder/Image_Sprites/Grid_Sprites/16pxGrid.png");
SDL_Surface* thirtytwo_pxGrid = loadImage("../MapBuilder/Image_Sprites/Grid_Sprites/32pxGrid.png");
//SDL_Surface* sixtyfour_pxGrid = loadImage("C:/Users/Brady J Bania/Desktop/dev/MapBuilder/Image_Sprites/Grid_Sprites/64pxGrid.png");
SDL_Surface* Top_Bar_selector = loadImage("../MapBuilder/Image_Sprites/Ui_Sprites/Top_Bar_Ui_Selector.png");
SDL_Surface* Top_Bar_selector_Highlighter = loadImage("../MapBuilder/Image_Sprites/Ui_Sprites/Ui_Selector_Highlighter_SIDES.png");

int main(int argc, char* argv[])
{
	if (!init()) return 1;


	// TEXTURE LOADER FOR MAP NOT FOR APPLICATION START****************
	Create_Image_Vector();

	std::map<std::string, SDL_Texture*> Texture_Map_1;
	std::map<std::string, std::map<std::string, int >> texture_value;

	int texture_map_x = 32;
	int texture_map_y = 32;

	for (int i = 0; i < files_to_load_images.size(); i++)
	{
		std::string ii = std::to_string(i);

		std::map<std::string, int > Texture_Map_1_info;

		SDL_Surface* image_loading_surface_ = loadImage(files_to_load_images[i]);
		if (!image_loading_surface_)
		{
			SDL_Log("Failed to initialize image_loading_surface_");
		}

		SDL_Texture* texture_loaded_ = SDL_CreateTextureFromSurface(renderer, image_loading_surface_);
		if (!texture_loaded_)
		{
			SDL_Log("Failed to initialize texture_loaded_");
		}

		// texture_map_info set the x_cord starting position of the texture map, with y_cord, texture_value is the value of the texture, selected is the highlighter to check if the texture is selected in the texture map
		Texture_Map_1_info.insert({ {"x_cord", texture_map_x }, { "y_cord", texture_map_y }, {"texture_value", i }, {"selected", 0} });
		texture_value.insert({ ii, Texture_Map_1_info });
		Texture_Map_1.insert({ ii, texture_loaded_ });

		texture_map_x += 32;
	}
	// END ********************************************

	// grid variables
	int sixteen_pixel = 16;
	int thirteen_pixel = 32;
	int sixtyfour_pixel = 64;
	// grid settings -- under variables
	int grid_start_x = 0;
	int grid_start_y = 0;
	int grid_pixel_size = 32;
	// setup grid map
	// settings
	int render_distance = 19;
	int map_size_width = 20;
	int map_size_height = 20;
	int default_texture_value = 555;

	// SAVE AS PIXEL SIZE -->
	int save_pixel_size = 64;

	// map init
	// create map
	std::unordered_map<std::string, std::vector<int>> Map;
	// create data map   ***IMPORTANT***
	std::unordered_map<std::string, std::unordered_map<std::string, std::unordered_map<std::string, int>>> data_map; // height"0" = width"0" = map of x and y cords and a value
	std::unordered_map<std::string, std::unordered_map<std::string, int>> second_level_map_;
	std::unordered_map<std::string, int> low_level_map_ =
	{
		{"x_cord", 0 },
		{"y_cord", 0 },
		{"texture_value", default_texture_value }
	};
	// end*

	for (int i = 0; i < map_size_height; i++)
	{
		std::vector<int> CONTAINER_BLANK;
		std::string ii = std::to_string(i);

		Map.insert({ ii, CONTAINER_BLANK });

		for (int iii = 0; iii < map_size_width; iii++)
		{
			CONTAINER_BLANK.push_back(55);

			std::string iiii = std::to_string(iii);

			second_level_map_.insert({ iiii, low_level_map_ });
		}
		Map.at(ii) = CONTAINER_BLANK;

		data_map.insert({ ii, second_level_map_ });
	}
	for (int i = 0; i < map_size_height; i++)
	{
		for (int ii = 0; ii < map_size_width; ii++)
		{
			std::string iii = std::to_string(i);
			std::string iiii = std::to_string(ii);
			data_map[iii][iiii]["x_cord"] = grid_start_x;
			data_map[iii][iiii]["y_cord"] = grid_start_y;
			grid_start_x += grid_pixel_size;

		}
		grid_start_x = 0;
		grid_start_y += grid_pixel_size;
	}
	grid_start_x = 0;
	grid_start_y = 0;

	for (int i = 0; i < map_size_height; i++)
	{
		std::string ii = std::to_string(i);

		std::vector<int> container_width;

		for (int iii = 0; iii < map_size_width; iii++)
		{
			int default_map_value = 0;

			container_width.push_back(default_map_value);
		}
		Map[ii] = container_width;
	}
	//SDL_Texture* sixteen_pxGrid_texture = SDL_CreateTextureFromSurface(renderer, sixteen_pxGrid);
	SDL_Texture* thirtytwo_pxGrid_texture = SDL_CreateTextureFromSurface(renderer, thirtytwo_pxGrid);
	//SDL_Texture* sixtyfour_pxGrid_texture = SDL_CreateTextureFromSurface(renderer, sixtyfour_pxGrid);
	SDL_Texture* Top_Bar_selector_texture = SDL_CreateTextureFromSurface(renderer, Top_Bar_selector);
	SDL_Texture* Top_Bar_selector_Highlighter_texture = SDL_CreateTextureFromSurface(renderer, Top_Bar_selector_Highlighter);
	// create texture map
	std::map<std::string, std::map<std::string, int>> texture_map;

	// user variables
	int real_player_cord_x = window_width / 2;
	int real_player_cord_y = window_height / 2;
	int render_distance_pixel_distance = grid_pixel_size * render_distance;
	int player_speed = 5; 
	int selected_texture = NULL; // 0 is default in this case

	// highlighter stuff
	int start_x_top_bar = 32;
	int end_x_top_bar = 1024;
	int start_y_top_bar = 0;
	int stop_y_top_bar = 192;
	std::vector<int> highlighter_x_cords_top_bar;
	std::vector<int> highlighter_y_cords_top_bar;
	for (int i = 0; i < 33; i++)
	{
		highlighter_x_cords_top_bar.push_back(i * 32);
	}
	for (int i = 0; i <= 6; i++)
	{
		highlighter_y_cords_top_bar.push_back(i * 32);
	}

	bool quit = false;
	while (!quit)
	{
		frameStart = SDL_GetTicks(); // Get the start time for the frame

		SDL_SetRenderDrawColor(renderer, 32, 32, 32, 255);
		SDL_RenderClear(renderer);

		SDL_Color color = { 255, 255, 255 }; // white
		SDL_FillSurfaceRect(screenSurface, NULL, 0xFFFFFFFF);

		// ui rects
		SDL_FRect Top_Bar_Rect_SET = { 0.0f, 0.0f, 1084, 1000 };
		SDL_FRect Top_Bar_Rect_MANAGE = { 0.0f, 0.0f, 1084, 1000 };

		Uint32 mouseState = SDL_GetMouseState(&mouse_x, &mouse_y);

		SDL_Event event;
		while (SDL_PollEvent(&event))
		{
			switch (event.type)
			{
			case SDL_EVENT_QUIT:
				SDL_Log("SDL_QUIT event");
				quit = true;
				break;
			case SDL_EVENT_MOUSE_BUTTON_DOWN:
				mouse_button_down = true;

			case SDL_EVENT_KEY_DOWN:
				if (event.key.key == SDLK_ESCAPE)
				{
					SDL_Log("Escape key pressed");
					quit = 1;
				}
				break;
			}
		}

		if (keys[SDL_SCANCODE_W])
		{
			global_offset_y += player_speed;
			real_player_cord_y -= player_speed;
		}
		if (keys[SDL_SCANCODE_S])
		{
			global_offset_y -= player_speed;
			real_player_cord_y += player_speed;
		}
		if (keys[SDL_SCANCODE_A])
		{
			global_offset_x += player_speed;
			real_player_cord_x -= player_speed;
		}
		if (keys[SDL_SCANCODE_D])
		{
			global_offset_x -= player_speed;
			real_player_cord_x += player_speed;
		}
		// grid load
		int center_y_value_number = real_player_cord_y / 32;
		int center_x_value_number = real_player_cord_x / 32;

		for (int i = center_y_value_number - render_distance; i < center_y_value_number + render_distance; i++)
		{
			SDL_FRect sixteen_pixel_rect_SET = { 0.0f, 0.0f, grid_pixel_size, grid_pixel_size };

			for (int ii = center_x_value_number - render_distance; ii < center_x_value_number + render_distance; ii++)
			{
				std::string iii = std::to_string(i);
				std::string iiii = std::to_string(ii);

				// render handle
				if (real_player_cord_x > data_map[iii][iiii]["x_cord"] + render_distance_pixel_distance || real_player_cord_x + render_distance_pixel_distance < data_map[iii][iiii]["x_cord"])
				{
					continue;
				}
				if (real_player_cord_y > data_map[iii][iiii]["y_cord"] + render_distance_pixel_distance || real_player_cord_y + render_distance_pixel_distance < data_map[iii][iiii]["y_cord"])
				{
					continue;
				}
				SDL_FRect sixteen_pixel_rect_MANAGE = { data_map[iii][iiii]["x_cord"] + global_offset_x, data_map[iii][iiii]["y_cord"] + global_offset_y, grid_pixel_size, grid_pixel_size };
				if (data_map[iii][iiii]["texture_value"] == default_texture_value)
				{
					SDL_RenderTexture(renderer, thirtytwo_pxGrid_texture, &sixteen_pixel_rect_SET, &sixteen_pixel_rect_MANAGE);
				}
				else
				{
					std::string iiiii = std::to_string(data_map[iii][iiii]["texture_value"]);

					SDL_RenderTexture(renderer, Texture_Map_1[iiiii], &sixteen_pixel_rect_SET, &sixteen_pixel_rect_MANAGE);
				}
			}
		}


		grid_start_x = 0;
		grid_start_y = 0;

		// ui onto screen
		SDL_RenderTexture(renderer, Top_Bar_selector_texture, &Top_Bar_Rect_SET, &Top_Bar_Rect_MANAGE);

		int texture_map_x = 32.0f;
		int texture_map_y = 32.0f;

		for (int i = 0; i < files_to_load_images.size(); i++)
		{
			std::string ii = std::to_string(i);

			// textures on texture map list
			SDL_FRect Texture_List_Visible_SET = { 0.0f, 0.0f, grid_pixel_size, grid_pixel_size };
			SDL_FRect Texture_List_Visible_MANAGE = { texture_value[ii]["x_cord"], texture_value[ii]["y_cord"], grid_pixel_size, grid_pixel_size };

			SDL_RenderTexture(renderer, Texture_Map_1[ii], &Texture_List_Visible_SET, &Texture_List_Visible_MANAGE);
		}

		// 32 and 18 32 = y = 0 for start
		if (mouse_x + global_offset_x > 32 + global_offset_x && mouse_x + global_offset_x <= 1055 + global_offset_x)
		{
			if (mouse_y + global_offset_y >= 32 + global_offset_y && mouse_y + global_offset_y <= 222 + global_offset_y)
			{
				SDL_FRect Highlighter_Rect_Top_Bar_SET = { 0.0f, 0.0f, 32, 32 };
				SDL_FRect Highlighter_Rect_Top_Bar_MANAGE = { highlighter_x_cords_top_bar[mouse_x / 32], highlighter_y_cords_top_bar[mouse_y / 32], 32, 32 };

				SDL_RenderTexture(renderer, Top_Bar_selector_Highlighter_texture, &Highlighter_Rect_Top_Bar_SET, &Highlighter_Rect_Top_Bar_MANAGE);
			}
		}
		// highlighter for load map data button
		if (mouse_x + global_offset_x > 742 + global_offset_x && mouse_x + global_offset_x <= 983 + global_offset_x)
		{
			if (mouse_y + global_offset_y >= 1 + global_offset_y && mouse_y + global_offset_y <= 29 + global_offset_y)
			{
				SDL_FRect Highlighter_Rect_Top_Bar_SET = { 0.0f, 0.0f, 239, 30 };
				SDL_FRect Highlighter_Rect_Top_Bar_MANAGE = { 742, 1, 239, 30 };

				SDL_RenderTexture(renderer, Top_Bar_selector_Highlighter_texture, &Highlighter_Rect_Top_Bar_SET, &Highlighter_Rect_Top_Bar_MANAGE);
			}
		}

		if (mouse_button_down == true)
		{
			// logic for clicking a texture from the texture map
			if (mouse_y + global_offset_y < 255 + global_offset_y)
			{
				if (mouse_x + global_offset_x > 32 + global_offset_x && mouse_x + global_offset_x < 1056 + global_offset_x)
				{
					for (int i = 0; i < texture_value.size(); i++)
					{
						std::string ii = std::to_string(i);
						if (texture_value[ii]["selected"] == 1)
						{
							texture_value[ii]["selected"] = 0; // resets so no highlighter box over is as selected
						}

						if (mouse_x > texture_value[ii]["x_cord"] && mouse_x - 33 < texture_value[ii]["x_cord"])
						{
							selected_texture = texture_value[ii]["texture_value"];

							SDL_Log("texture value %d", selected_texture);

							texture_value[ii]["selected"] = 1; // basically sets the selected to true so there is a box around the selected texture so you know what texture you can place down.
						}
					}
				}
			}
			if (mouse_y + global_offset_y > 256 + global_offset_y)
			{
				for (int i = (mouse_y - global_offset_y) / grid_pixel_size; i < (mouse_y - global_offset_y) / grid_pixel_size; i++)
				{
					for (int ii = (mouse_x - global_offset_x) / grid_pixel_size; ii < (mouse_x - global_offset_x) / grid_pixel_size; ii++)
					{
						std::string iii = std::to_string(i);
						std::string iiii = std::to_string(ii);

						data_map[iii][iiii]["texture_value"] = selected_texture;

						mouse_button_down = false;
					}
				}
			}
			if (mouse_x + global_offset_x > 742 + global_offset_x && mouse_x + global_offset_x <= 983 + global_offset_x)
			{
				if (mouse_y + global_offset_y >= 1 + global_offset_y && mouse_y + global_offset_y <= 29 + global_offset_y)
				{
					// so we need to give an output of everything i need to load a map in another application.
					// soo we need the data_map for values that are stored.
					// also for an application to work, it needs the required lines from files_images.cpp for loading in, 
					// so it needs the texture value maker too that i made.
					// now all we need from this application is the data map.
					// creates a file on your desktop named map__t.cpp
					char path[MAX_PATH];
					if (SUCCEEDED(SHGetFolderPathA(NULL, CSIDL_DESKTOP, NULL, 0, path))) {
						std::string fullPath = std::string(path) + "\\map__t.cpp";
						std::ofstream file(fullPath);
						if (file.is_open()) {
							
							file << "#include <iostream>\n";
							file << "#include <string>\n";
							file << "#include <vector>\n";
							file << "#include <unordered_map>\n\n";



							file << "// Required map build --> std::unordered_map<std::string, std::unordered_map<std::string, std::unordered_map<std::string, int>>> data_map;\n";
							file << "// Here is a code snippet for best performance running this map.\n";
							file << "\n";
							file << "// for (int i = center_y_value_number - render_distance; i < center_y_value_number + render_distance; i++) {\n";
							file << "//     SDL_FRect sixteen_pixel_rect_SET = { 0.0f, 0.0f, grid_pixel_size, grid_pixel_size };\n";
							file << "//     for (int ii = center_x_value_number - render_distance; ii < center_x_value_number + render_distance; ii++) {\n";
							file << "//         std::string iii = std::to_string(i);\n";
							file << "//         std::string iiii = std::to_string(ii);\n";
							file << "//         if (real_player_cord_x > data_map[iii][iiii][\"x_cord\"] + render_distance_pixel_distance ||\n";
							file << "//             real_player_cord_x + render_distance_pixel_distance < data_map[iii][iiii][\"x_cord\"]) continue;\n";
							file << "//         if (real_player_cord_y > data_map[iii][iiii][\"y_cord\"] + render_distance_pixel_distance ||\n";
							file << "//             real_player_cord_y + render_distance_pixel_distance < data_map[iii][iiii][\"y_cord\"]) continue;\n";
							file << "//         SDL_FRect sixteen_pixel_rect_MANAGE = {\n";
							file << "//             data_map[iii][iiii][\"x_cord\"] + global_offset_x,\n";
							file << "//             data_map[iii][iiii][\"y_cord\"] + global_offset_y,\n";
							file << "//             grid_pixel_size,\n";
							file << "//             grid_pixel_size\n";
							file << "//         };\n";
							file << "//         if (data_map[iii][iiii][\"texture_value\"] == default_texture_value) {\n";
							file << "//             SDL_RenderTexture(renderer, thirtytwo_pxGrid_texture, &sixteen_pixel_rect_SET, &sixteen_pixel_rect_MANAGE);\n";
							file << "//         } else {\n";
							file << "//             std::string iiiii = std::to_string(data_map[iii][iiii][\"texture_value\"]);\n";
							file << "//             SDL_RenderTexture(renderer, Texture_Map_1[iiiii], &sixteen_pixel_rect_SET, &sixteen_pixel_rect_MANAGE);\n";
							file << "//         }\n";
							file << "//     }\n";
							file << "// }\n";
							file << "\n";

							// build process
							file << "std::unordered_map<std::string, std::unordered_map<std::string, std::unordered_map<std::string, int>>> data_map = ";

							file << "{";
							for (int i = 0; i < map_size_height; i++) // map height 
							{
								std::string iii = std::to_string(i);
								file << "{ " << "\"" << iii << "\"" << ", { ";
								for (int ii = 0; ii < map_size_width; ii++)
								{
									std::string iiii = std::to_string(ii);
									// { {"0", { { "0", { { "x_cord", 0 }, { "y_cord", 0 }, { "texture_value", 555 }, }},}},};
									file << "{ " << "\"" << iiii << "\"" << ", " << "{ ";
									file << "{ " << "\"x_cord\", " << data_map[iii][iiii]["x_cord"] << " },";
									file << " " << "{ " << "\"y_cord\", " << data_map[iii][iiii]["y_cord"] << " },";
									file << " " << "{ " << "\"texture_value\", " << data_map[iii][iiii]["texture_value"] << " }, }},";
								}
								file << "}}," << std::endl;
							}
							file << "};";

							file.close();
							std::cout << "File created on Desktop.\n";
						}
						else {
							std::cerr << "Could not open file.\n";
						}
					}
					else {
						std::cerr << "Failed to get Desktop path.\n";
					}

				}
			}
			else
			{
				mouse_button_down = false;
			}
		}

		for (int i = 0; i < texture_value.size(); i++)
		{
			std::string ii = std::to_string(i);

			if (texture_value[ii]["selected"] == 1)
			{
				SDL_FRect selected_texture_highlighted_set = { 0.0f,0.0f,grid_pixel_size,grid_pixel_size };
				SDL_FRect selected_texture_highlighted_manage = { texture_value[ii]["x_cord"], texture_value[ii]["y_cord"], grid_pixel_size, grid_pixel_size };

				SDL_RenderTexture(renderer, Top_Bar_selector_Highlighter_texture, &selected_texture_highlighted_set, &selected_texture_highlighted_manage);
			}
		}

		mouse_button_down = false;

		SDL_RenderPresent(renderer);
		SDL_UpdateWindowSurface(window);

		frameTime = SDL_GetTicks() - frameStart; // calculate how long frame took
		if (frameDelay > frameTime)
		{
			SDL_Delay(frameDelay - frameTime); // delay for the remaining time
		}

		iterations += 1;
		//SDL_Log("Number of iterations: %d", iterations);

	}

	SDL_Log("SDL3 Window is exiting....");
	close();
	return 0;
}