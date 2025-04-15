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

int window_width = 1000;
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

double angle_to_target_calculator(double width, double height)
{
	double angle_radians = (atan2(height, width));
	double angle_rotation = angle_radians * (180.0f / 3.14159265358979323846);
	return angle_rotation;
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

int main(int argc, char* argv[])
{
	if (!init()) return 1;

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
	int map_size_width = 50;
	int map_size_height = 50;
	int default_texture_value = 0;
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

	// user variables
	int real_player_cord_x = window_width / 2;
	int real_player_cord_y = window_height / 2;
	int render_distance_pixel_distance = grid_pixel_size * render_distance;
	int player_speed = 5;



	bool quit = false;
	while (!quit)
	{
		frameStart = SDL_GetTicks(); // Get the start time for the frame

		SDL_SetRenderDrawColor(renderer, 32, 32, 32, 255);
		SDL_RenderClear(renderer);

		SDL_Color color = { 255, 255, 255 }; // white
		SDL_FillSurfaceRect(screenSurface, NULL, 0xFFFFFFFF);

		// texture loader per ui
		SDL_Texture* Right_Picker_texture = SDL_CreateTextureFromSurface(renderer, Right_Picker);
		SDL_Texture* Right_Side_Separater_texture = SDL_CreateTextureFromSurface(renderer, Right_Side_Separater);
		SDL_Texture* Right_Side_Separater_bottom_texture = SDL_CreateTextureFromSurface(renderer, Right_Side_Separater_bottom);
		//SDL_Texture* sixteen_pxGrid_texture = SDL_CreateTextureFromSurface(renderer, sixteen_pxGrid);
		SDL_Texture* thirtytwo_pxGrid_texture = SDL_CreateTextureFromSurface(renderer, thirtytwo_pxGrid);
		//SDL_Texture* sixtyfour_pxGrid_texture = SDL_CreateTextureFromSurface(renderer, sixtyfour_pxGrid);

		// ui rects
		// right bar
		SDL_FRect Right_Picker_SET_rect = { 0.0f, 0.0f, 128, 512 };  // keep first two values 0, next 2 are your pixel size that you made
		SDL_FRect Right_Picker_MANAGE_rect = { window_width - 128, 0, 128, 512 }; // here the first two values are location and then the next two values are your what size it should be.
		// right bar sep
		SDL_FRect Right_Picker_Bar_SET_rect = { 0.0f, 0.0f, 16, 512 };
		SDL_FRect Right_Picker_Bar_MANAGE_rect = { window_width - 144, 0, 16, 512 };
		SDL_FRect Right_Picker_Bar_SET_rect_bottom = { 0.0f, 0.0f, 144, 16 };
		SDL_FRect Right_Picker_Bar_MANAGE_rect_bottom = { window_width - 144, 512, 144, 16 };
		// Grid Setup
		//// 16 px grid
		//SDL_FRect sixteen_pixel_rect_SET = { 0.0f, 0.0f, grid_pixel_size, grid_pixel_size };
		//SDL_FRect sixteen_pixel_rect_MANAGE = { grid_start_x + grid_start_y, 0 + global_offset_y, grid_pixel_size, grid_pixel_size };

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
				SDL_Log("wo %f", mouse_x + (-(global_offset_x)) );

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
			}
		}
		grid_start_x = 0;
		grid_start_y = 0;

		// ui onto screen
		SDL_RenderTexture(renderer, Right_Picker_texture, &Right_Picker_SET_rect, &Right_Picker_MANAGE_rect);
		SDL_RenderTexture(renderer, Right_Side_Separater_texture, &Right_Picker_Bar_SET_rect, &Right_Picker_Bar_MANAGE_rect);
		SDL_RenderTexture(renderer, Right_Side_Separater_bottom_texture, &Right_Picker_Bar_SET_rect_bottom, &Right_Picker_Bar_MANAGE_rect_bottom);


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