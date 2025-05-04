#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <SDL3/SDL_main.h>
#include <SDL3_ttf/SDL_ttf.h>
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



// MAIN MENU BOOLS
bool Main_Menu_Open = true;
bool Create_Map = false;
bool Load_Map = false;
bool Exit_Map_Builder = false;
bool Help_Map_Builder = false;
bool main_menu_button_offline = false;
bool help_page = false;

// Create map menu bools
bool Create_Map_Menu = false;
bool map_width_bool_create = false;
bool map_height_bool_create = false;
bool pixel_size_create_map_menu = false;
bool Show_Create_Map_Error_Message = false;
char User_Input_Map_Width[256] = "";
bool TextInput_Map_Width = false;
std::string input_map_width_string;


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

bool isFullscreen = false;



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
	window = SDL_CreateWindow("Map Builder", window_width, window_height, SDL_WINDOW_RESIZABLE);
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
	if (TTF_Init() == -1)
	{
		fprintf(stderr, "COULRD NOT INITIALIZE FONT LOADER");
	}
	//if (SDL_SetHint(SDL_HINT_WINDOWS_DPI_AWARENESS, "permonitorv2")) PLEASE RESEE THIS FOR DPI PURPOSES MONITOR PIXEL OR QUALITY DIFFERENCES

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

static void button_highlight(int width, int height, int highlight_weight, int x_cord, int y_cord, int color = 1)
{
	/* availabe colors, red, green, black, white, blue */
	// some colors
	std::vector<int> white = { 255, 255, 255 };
	std::vector<int> off_white = { 230, 230, 230 };
	std::vector<int> red = { 255, 0, 0 };
	std::vector<int> green = { 0, 255, 0 };
	std::vector<int> blue = { 0, 255, 0 };
	std::vector<int> black = { 0, 0, 0 };
	std::vector<int> off_black = { 50, 50, 50 };
	// set color of rect
	if (color == 1)
	{
		SDL_SetRenderDrawColor(renderer, off_black[0], off_black[1], off_black[2], 255);
	}
	if (color == 0)
	{
		SDL_SetRenderDrawColor(renderer, green[0], green[1], green[2], 255);
	}
	if (color == 2)
	{
		SDL_SetRenderDrawColor(renderer, off_white[0], off_white[1], off_white[2], 255);
	}

	SDL_FRect Highlight_Button_Rect = { x_cord, y_cord, width, height };

	SDL_RenderRect(renderer, &Highlight_Button_Rect);
}

void save_Map_Binary(const std::vector<std::vector<int>>& map, const std::string& filename)
{
	std::ofstream outFile(filename, std::ios::binary);
	if (!outFile)
	{
		std::cout << "Failed to open file for writing: " << filename << "\n";
		return;
	}

	int rows = map.size();
	int cols = map.empty() ? 0 : map[0].size();

	outFile.write(reinterpret_cast<char*>(&rows), sizeof(int));
	outFile.write(reinterpret_cast<char*>(&cols), sizeof(int));

	for (const auto& row : map)
	{
		outFile.write(reinterpret_cast<const char*>(row.data()), cols * sizeof(int));
	}

	outFile.close();
}

// load ui
SDL_Surface* Right_Picker = loadImage("../MapBuilder/Image_Sprites/Ui_Sprites/Right_Picker.png");
SDL_Surface* Right_Side_Separater = loadImage("../MapBuilder/Image_Sprites/Ui_Sprites/Right_Side_Separater.png");
SDL_Surface* Right_Side_Separater_bottom = loadImage("../MapBuilder/Image_Sprites/Ui_Sprites/Right_Side_Separator_bottom.png");
SDL_Surface* Main_Page_image = loadImage("../MapBuilder/Image_sprites/Ui_Sprites/Main_Page.png");
//SDL_Surface* sixteen_pxGrid = loadImage("C:/Users/Brady J Bania/Desktop/dev/MapBuilder/Image_Sprites/Grid_Sprites/16pxGrid.png");
SDL_Surface* thirtytwo_pxGrid = loadImage("../MapBuilder/Image_Sprites/Grid_Sprites/32pxGrid.png");
//SDL_Surface* sixtyfour_pxGrid = loadImage("C:/Users/Brady J Bania/Desktop/dev/MapBuilder/Image_Sprites/Grid_Sprites/64pxGrid.png");
SDL_Surface* Top_Bar_selector = loadImage("../MapBuilder/Image_Sprites/Ui_Sprites/Top_Bar_Ui_Selector.png");
SDL_Surface* Top_Bar_selector_Highlighter = loadImage("../MapBuilder/Image_Sprites/Ui_Sprites/Ui_Selector_Highlighter_SIDES.png");
SDL_Surface* Create_Map_Menu_img = loadImage("../MapBuilder/Image_Sprites/Ui_Sprites/Create_Map.png");
SDL_Surface* Help_page_img = loadImage("../MapBuilder/Image_Sprites/Ui_Sprites/Help_Page.png");
SDL_Surface* Create_map_page_error_img = loadImage("../MapBuilder/Image_Sprites/Ui_Sprites/Create_Map_Page_Error.png");

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
	int map_size_width = 10;
	int map_size_height = 10;
	int default_texture_value = 555;

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


	// MAIN MENU RECT
	SDL_FRect Main_Menu_Set = { 0, 0, 1084, 1000 };
	// Main Menu Button Cords
	std::vector<int> Create_Map_Button_Main_Page = { 374, 303 };
	std::vector<int> Load_Map_Button_Main_Page = { 374, 359 };
	std::vector<int> Help_Button_Main_Page = { 374, 415 };
	std::vector<int> Exit_Button_Main_Page = { 374, 472 };
	std::vector<int> Go_Back_Button_Create_Map_Page_MAP = { 9, 965 };
	std::vector<int> Save_Map_Button_Create_Map_Page_MAP = { 171, 965 };
	std::vector<int> Create_Map_Button_Create_Page = { 174, 416 };
	std::vector<int> Go_Back_Button_Create_Page = { 174, 354 };
	std::vector<int> Map_Width_Create_Page = { 159, 138 };
	std::vector<int> Map_Height_Create_Page = { 158, 219 };
	std::vector<int> Pixel_Slider_Create_Page = { 159, 301 };

	// FONT STUFF
	TTF_Font* font = TTF_OpenFont("../MapBuilder/Fonts/opensans_non_italic.ttf", 14); // You need a TTF font file
	if (!font)
	{
		SDL_Log("COULD NOT INITIALIZE FONT");
	}


	// SAVE AS PIXEL SIZE -->
	int save_pixel_size = 64;
	//SDL_Texture* sixteen_pxGrid_texture = SDL_CreateTextureFromSurface(renderer, sixteen_pxGrid);
	SDL_Texture* thirtytwo_pxGrid_texture = SDL_CreateTextureFromSurface(renderer, thirtytwo_pxGrid);
	//SDL_Texture* sixtyfour_pxGrid_texture = SDL_CreateTextureFromSurface(renderer, sixtyfour_pxGrid);
	SDL_Texture* Top_Bar_selector_texture = SDL_CreateTextureFromSurface(renderer, Top_Bar_selector);
	SDL_Texture* Top_Bar_selector_Highlighter_texture = SDL_CreateTextureFromSurface(renderer, Top_Bar_selector_Highlighter);
	SDL_Texture* Main_Menu_Texture = SDL_CreateTextureFromSurface(renderer, Main_Page_image);
	SDL_Texture* Create_Map_Menu_Texture = SDL_CreateTextureFromSurface(renderer, Create_Map_Menu_img);
	SDL_Texture* Help_page_Texture = SDL_CreateTextureFromSurface(renderer, Help_page_img);
	SDL_Texture* Create_map_page_error_texture = SDL_CreateTextureFromSurface(renderer, Create_map_page_error_img);
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
		SDL_RenderClear(renderer);


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

			case SDL_EVENT_TEXT_INPUT:
				//// Use strncat to safely append the new text
				//strncat(User_Input_Map_Width, event.text.text, sizeof(User_Input_Map_Width) - strlen(User_Input_Map_Width) - 1);
				//printf("Current Input: %s\n", User_Input_Map_Width);

			case SDL_EVENT_KEY_DOWN:
				if (event.key.key == SDLK_ESCAPE)
				{
					SDL_Log("Escape key pressed");
					quit = 1;
				}
				if (event.key.key == SDLK_F11)
				{
					isFullscreen = !isFullscreen;
					if (isFullscreen) {
						const SDL_DisplayMode* mode = SDL_GetCurrentDisplayMode(SDL_GetPrimaryDisplay());
						SDL_SetWindowFullscreenMode(window, mode);
						if (!mode) {
							SDL_Log("Failed to get display mode: %s", SDL_GetError());
						}
						else {
							if (SDL_SetWindowFullscreenMode(window, mode) != 0) {
								SDL_Log("Failed to go fullscreen: %s", SDL_GetError());
							}
						}
						SDL_Surface* screenSurface = SDL_GetWindowSurface(window);
						if (!screenSurface) {
							SDL_Log("Failed to get window surface: %s", SDL_GetError());
						}

					}
					else {
						SDL_SetWindowFullscreenMode(window, nullptr);
					}
				}

				if (TextInput_Map_Width == true)
				{
					if (event.key.key == SDLK_0)
					{
						input_map_width_string = input_map_width_string + "0";
					}
					if (event.key.key == SDLK_1)
					{
						input_map_width_string = input_map_width_string + "1";
					}
					if (event.key.key == SDLK_2)
					{
						input_map_width_string = input_map_width_string + "2";
					}
					if (event.key.key == SDLK_3)
					{
						input_map_width_string = input_map_width_string + "3";
					}
					if (event.key.key == SDLK_4)
					{
						input_map_width_string = input_map_width_string + "4";
					}
					if (event.key.key == SDLK_5)
					{
						input_map_width_string = input_map_width_string + "5";
					}
					if (event.key.key == SDLK_6)
					{
						input_map_width_string = input_map_width_string + "6";
					}
					if (event.key.key == SDLK_7)
					{
						input_map_width_string = input_map_width_string + "7";
					}
					if (event.key.key == SDLK_8)
					{
						input_map_width_string = input_map_width_string + "8";
					}
					if (event.key.key == SDLK_9)
					{
						input_map_width_string = input_map_width_string + "9";
					}
					//if (event.key.key == SDLK_ESCAPE)
					//{
					//	std::cout << "exit this input";
					//	TextInput_Map_Width = false;
					//}
				}
				break;
			}
		}

		if (Main_Menu_Open == false)
		{
			if (Create_Map == true)
			{
				//SDL_SetRenderDrawColor(renderer, 32, 32, 32, 255);
				//SDL_Color color = { 255, 255, 255 }; // white
				//SDL_FillSurfaceRect(screenSurface, NULL, 0xFFFFFFFF);

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

				// ui rects
				SDL_FRect Top_Bar_Rect_SET = { 0.0f, 0.0f, 1084, 1000 };
				SDL_FRect Top_Bar_Rect_MANAGE = { 0.0f, 0.0f, 1084, 1000 };

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


				// BUTTONS
				// Go Back Button
				if (mouse_x >= Go_Back_Button_Create_Map_Page_MAP[0] && mouse_y >= Go_Back_Button_Create_Map_Page_MAP[1])
				{
					if (mouse_x <= Go_Back_Button_Create_Map_Page_MAP[0] + 159 && mouse_y <= Go_Back_Button_Create_Map_Page_MAP[1] + 32)
					{
						button_highlight(159, 32, 1, Go_Back_Button_Create_Map_Page_MAP[0], Go_Back_Button_Create_Map_Page_MAP[1]);
						if (mouse_button_down == true)
						{
							// mouse down
							Main_Menu_Open = true;
							Create_Map = false;
							main_menu_button_offline = false;
						}
					}
				}
				// Save Map Button
				if (mouse_x >= Save_Map_Button_Create_Map_Page_MAP[0] && mouse_y >= Save_Map_Button_Create_Map_Page_MAP[1])
				{
					if (mouse_x <= Save_Map_Button_Create_Map_Page_MAP[0] + 159 && mouse_y <= Save_Map_Button_Create_Map_Page_MAP[1] + 32)
					{
						button_highlight(159, 32, 1, Save_Map_Button_Create_Map_Page_MAP[0], Save_Map_Button_Create_Map_Page_MAP[1]);
						if (mouse_button_down == true)
						{
							// so we need to give an output of everything i need to load a map in another application.
							// soo we need the data_map for values that are stored.
							// also for an application to work, it needs the required lines from files_images.cpp for loading in, 
							// so it needs the texture value maker too that i made.
							// now all we need from this application is the data map.
							// creates a file on your desktop named map__t.cpp
							char path[MAX_PATH];
							if (SUCCEEDED(SHGetFolderPathA(NULL, CSIDL_DESKTOP, NULL, 0, path)))
							{
								std::string fullPath = std::string(path) + "\\map_3.bin";
								std::ofstream file(fullPath);
								if (file.is_open())
								{
									std::cout << "made" << std::endl;

									for (int i = 0; i < map_size_height; i++)
									{
										for (int ii = 0; ii < map_size_width; ii++)
										{
											std::string iii = std::to_string(i);
											std::string iiii = std::to_string(ii);
											int texture_value = data_map[iii][iiii]["texture_value"];

											file.write(reinterpret_cast<const char*>(&texture_value), sizeof(texture_value));
										}
										//file << "\n";
									}

									file.close();
									std::cout << "File created on Desktop.\n";
								}
								else
								{
									std::cerr << "Could not open file.\n";
								}
							}
							else
							{
								std::cerr << "Failed to get Desktop path.\n";
							}

							if (SUCCEEDED(SHGetFolderPathA(NULL, CSIDL_DESKTOP, NULL, 0, path)))
							{
								std::string fullPath = std::string(path) + "\\map__t.cpp";
								std::ofstream file(fullPath);
								if (file.is_open())
								{
									file << "#include <iostream>\n";
									file << "#include <string>\n";
									file << "#include <vector>\n";
									file << "#include <unordered_map>\n";
									file << "#include \"map.h\"\n";
									file << "#include <fstream>\n\n";
									file << "// build process\n";
									file << "// map init\n";
									file << "// create map\n";
									file << "std::unordered_map<std::string, std::vector<int>> Map;\n";
									file << "// create data map   ***IMPORTANT***\n";
									file << "int map_width = " << map_size_width << ";\n";
									file << "int map_height = " << map_size_height << ";\n";
									file << "int grid_start_x = 0;\n";
									file << "int grid_start_y = 0;\n";
									file << "int grid_pixel_size = " << grid_pixel_size << ";\n";
									file << "int iteration_width_map = 0;\n";
									file << "int default_texture_value = 555;\n";
									file << "std::unordered_map<std::string, std::unordered_map<std::string, std::unordered_map<std::string, int>>> data_map;\n";
									file << "std::unordered_map<std::string, std::unordered_map<std::string, int>> second_level_map_;\n";
									file << "std::unordered_map<std::string, int> low_level_map_ =\n";
									file << "{\n";
									file << "    { \"x_cord\", 0 },\n";
									file << "    { \"y_cord\", 0 },\n";
									file << "    { \"texture_value\", default_texture_value }\n";
									file << "};\n";
									file << "// end*\n\n";
									file << "void data_map_create()\n";
									file << "{\n";
									file << "    std::vector<int> texture_vector_from_bin;\n";
									file << "    int texture_value_from_bin;\n";
									file << "    std::ifstream inside_file(\"../Top_Down_Zombie_Game/map_3.bin\", std::ios::binary);\n";
									file << "    if (!inside_file)\n";
									file << "    {\n";
									file << "        std::cout << \"error loading bin file \\\"inside_file\\\"\";\n";
									file << "    }\n";
									file << "    for (int i = 0; i < map_width * map_height; i++)\n";
									file << "    {\n";
									file << "        inside_file.read(reinterpret_cast<char*>(&texture_value_from_bin), sizeof(texture_value_from_bin));\n";
									file << "        texture_vector_from_bin.push_back(texture_value_from_bin);\n";
									file << "    }\n";
									file << "    inside_file.close();\n\n";
									file << "    for (int i = 0; i < map_height; i++)\n";
									file << "    {\n";
									file << "        std::string ii = std::to_string(i);\n";
									file << "        for (int iii = 0; iii < map_width; iii++)\n";
									file << "        {\n";
									file << "            std::string iiii = std::to_string(iii);\n";
									file << "            second_level_map_.insert({iiii, low_level_map_});\n";
									file << "        }\n";
									file << "        data_map.insert({ii, second_level_map_});\n";
									file << "    }\n\n";
									file << "    for (int i = 0; i < map_height; i++)\n";
									file << "    {\n";
									file << "        for (int ii = 0; ii < map_width; ii++)\n";
									file << "        {\n";
									file << "            std::string iii = std::to_string(i);\n";
									file << "            std::string iiii = std::to_string(ii);\n";
									file << "            data_map[iii][iiii][\"x_cord\"] = grid_start_x;\n";
									file << "            data_map[iii][iiii][\"y_cord\"] = grid_start_y;\n";
									file << "            data_map[iii][iiii][\"texture_value\"] = texture_vector_from_bin[ii + iteration_width_map];\n";
									file << "            grid_start_x += grid_pixel_size;\n";
									file << "        }\n";
									file << "        iteration_width_map += map_width;\n";
									file << "        grid_start_x = 0;\n";
									file << "        grid_start_y += grid_pixel_size;\n";
									file << "    }\n";
									file << "    grid_start_x = 0;\n";
									file << "    grid_start_y = 0;\n";
									file << "}\n";
									file.close();
									std::cout << "File created on Desktop.\n";
								}
								else
								{
									std::cerr << "Could not open file.\n";
								}
							}
							else
							{
								std::cerr << "Failed to get Desktop path.\n";
							}

						}
					}
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
		// end of create map loop
		}
		else
		{
			SDL_RenderTexture(renderer, Main_Menu_Texture, &Main_Menu_Set, &Main_Menu_Set);

			// CREATE MAP MENU
			if (Create_Map_Menu == true)
			{
				main_menu_button_offline = true;

				SDL_FRect Create_Map_Menu_SET = { 0, 0, 542, 500 };
				SDL_FRect Create_Map_Menu_MANAGE = { 271, 250, 542, 500 };

				SDL_RenderTexture(renderer, Create_Map_Menu_Texture, &Create_Map_Menu_SET, &Create_Map_Menu_MANAGE);

				SDL_Color Input_Map_Text_Color = { 255,0,0 };
				std::string text_totest = "123123";
				SDL_Surface* textsurface = TTF_RenderText_Solid(font, text_totest.c_str(), 6, Input_Map_Text_Color);
				SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textsurface);
				//SDL_DestroySurface(textsurface);

				SDL_FRect textRect = { Create_Map_Button_Create_Page[0] + 271, Create_Map_Button_Create_Page[1] - 27, 100, 30 };

				SDL_RenderTexture(renderer, textTexture, nullptr, &textRect);


				// Create Map Button
				if (mouse_x >= Create_Map_Button_Create_Page[0] + 271 && mouse_y >= Create_Map_Button_Create_Page[1] + 250)
				{
					if (mouse_x <= Create_Map_Button_Create_Page[0] + 196 + 271 && mouse_y <= Create_Map_Button_Create_Page[1] + 50 + 250)
					{
						button_highlight(197, 51, 1, Create_Map_Button_Create_Page[0] + 271, Create_Map_Button_Create_Page[1] + 250, 2);
						if (mouse_button_down == true)
						{
							// mouse down
							if (map_width_bool_create != true || map_height_bool_create != true || pixel_size_create_map_menu != true)
							{
								Show_Create_Map_Error_Message = true;
							}
							else
							{
								Show_Create_Map_Error_Message = false;
							}
						}
					}
				}
				if (Show_Create_Map_Error_Message == true)
				{
					SDL_RenderTexture(renderer, Create_map_page_error_texture, &Create_Map_Menu_SET, &Create_Map_Menu_MANAGE);
				}
				// Go Back Map Button
				if (mouse_x >= Go_Back_Button_Create_Page[0] + 271 && mouse_y >= Go_Back_Button_Create_Page[1] + 250)
				{
					if (mouse_x <= Go_Back_Button_Create_Page[0] + 196 + 271 && mouse_y <= Go_Back_Button_Create_Page[1] + 50 + 250)
					{
						button_highlight(197, 51, 1, Go_Back_Button_Create_Page[0] + 271, Go_Back_Button_Create_Page[1] + 250, 2);
						if (mouse_button_down == true)
						{
							// mouse down
							main_menu_button_offline = false;
							Create_Map_Menu = false;
							mouse_button_down = false;
							map_width_bool_create = false;
							map_height_bool_create = false;
							pixel_size_create_map_menu = false;
							Show_Create_Map_Error_Message = false;
							TextInput_Map_Width = false;
						}
					}
				}
				int slider_size_width = 218;
				int slider_size_height = 30;
				// Map Width Button
				if (mouse_x >= Map_Width_Create_Page[0] + 271 && mouse_y >= Map_Width_Create_Page[1] + 250)
				{
					if (mouse_x <= Map_Width_Create_Page[0] + 196 + 271 && mouse_y <= Map_Width_Create_Page[1] + 30 + 250)
					{
						button_highlight(slider_size_width, slider_size_height, 1, Map_Width_Create_Page[0] + 271, Map_Width_Create_Page[1] + 250, 2);
						if (mouse_button_down == true)
						{
							// mouse down
							TextInput_Map_Width = true;


							map_width_bool_create = true;
							Show_Create_Map_Error_Message = false;
						}
					}
				}
				else
				{
					if (mouse_button_down == true)
					{
						TextInput_Map_Width = false;
					}
				}
				// Map Height Button
				if (mouse_x >= Map_Height_Create_Page[0] + 271 && mouse_y >= Map_Height_Create_Page[1] + 250)
				{
					if (mouse_x <= Map_Height_Create_Page[0] + 196 + 271 && mouse_y <= Map_Height_Create_Page[1] + 30 + 250)
					{
						button_highlight(slider_size_width, slider_size_height, 1, Map_Height_Create_Page[0] + 271, Map_Height_Create_Page[1] + 250, 2);
						if (mouse_button_down == true)
						{
							// mouse down
							map_height_bool_create = true;
							Show_Create_Map_Error_Message = false;
							TextInput_Map_Width = false;
						}
					}
				}
				// Pixel Size Button
				if (mouse_x >= Pixel_Slider_Create_Page[0] + 271 && mouse_y >= Pixel_Slider_Create_Page[1] + 250)
				{
					if (mouse_x <= Pixel_Slider_Create_Page[0] + 196 + 271 && mouse_y <= Pixel_Slider_Create_Page[1] + 30 + 250)
					{
						button_highlight(slider_size_width, slider_size_height, 1, Pixel_Slider_Create_Page[0] + 271, Pixel_Slider_Create_Page[1] + 250, 2);
						if (mouse_button_down == true)
						{
							// mouse down
							pixel_size_create_map_menu = true;
							Show_Create_Map_Error_Message = false;
						}
					}
				}
				if (TextInput_Map_Width == true)
				{
					std::cout << input_map_width_string;
				}
			}
			else
			{
				TextInput_Map_Width = false;
			}
			// END CREATE MAP MENU
			// HELP MENU
			if (help_page == true)
			{
				main_menu_button_offline = true;

				SDL_FRect Help_Page_SET = { 0, 0, 542, 500 };
				SDL_FRect Help_Page_MANAGE = { 271, 250, 542, 500 };

				SDL_RenderTexture(renderer, Help_page_Texture, &Help_Page_SET, &Help_Page_MANAGE);

				// Go Back Button
				if (mouse_x >= Create_Map_Button_Create_Page[0] + 271 && mouse_y >= Create_Map_Button_Create_Page[1] + 250)
				{
					if (mouse_x <= Create_Map_Button_Create_Page[0] + 196 + 271 && mouse_y <= Create_Map_Button_Create_Page[1] + 50 + 250)
					{
						button_highlight(197, 51, 1, Create_Map_Button_Create_Page[0] + 271, Create_Map_Button_Create_Page[1] + 250, 2);
						if (mouse_button_down == true)
						{
							// mouse down
							main_menu_button_offline = false;
							help_page = false;
							mouse_button_down = false;
						}
					}
				}

				
			}
			// END HELP MENU **************************************************************************************************
			//
			//
			//
			// Main Menu Page setup *******************************************************************************************
			if (main_menu_button_offline == false)
			{
				// Create Map Button
				if (mouse_x >= Create_Map_Button_Main_Page[0] && mouse_y >= Create_Map_Button_Main_Page[1])
				{
					if (mouse_x <= Create_Map_Button_Main_Page[0] + 333 && mouse_y <= Create_Map_Button_Main_Page[1] + 51)
					{
						button_highlight(333, 51, 1, Create_Map_Button_Main_Page[0], Create_Map_Button_Main_Page[1]);
						if (mouse_button_down == true)
						{
							// mouse down
							Create_Map_Menu = true;
							std::string input_map_width_string;

						}
					}
				}
				// Load Map Button
				if (mouse_x >= Load_Map_Button_Main_Page[0] && mouse_y >= Load_Map_Button_Main_Page[1])
				{
					if (mouse_x <= Load_Map_Button_Main_Page[0] + 333 && mouse_y <= Load_Map_Button_Main_Page[1] + 51)
					{
						button_highlight(333, 51, 1, Load_Map_Button_Main_Page[0], Load_Map_Button_Main_Page[1]);
						if (mouse_button_down == true)
						{
							// mouse down
						}
					}
				}
				// Help Button
				if (mouse_x >= Help_Button_Main_Page[0] && mouse_y >= Help_Button_Main_Page[1])
				{
					if (mouse_x <= Help_Button_Main_Page[0] + 333 && mouse_y <= Help_Button_Main_Page[1] + 51)
					{
						button_highlight(333, 51, 1, Help_Button_Main_Page[0], Help_Button_Main_Page[1]);
						if (mouse_button_down == true)
						{
							// mouse down
							help_page = true;
						}
					}
				}
				// Exit Button
				if (mouse_x >= Exit_Button_Main_Page[0] && mouse_y >= Exit_Button_Main_Page[1])
				{
					if (mouse_x <= Exit_Button_Main_Page[0] + 333 && mouse_y <= Exit_Button_Main_Page[1] + 51)
					{
						button_highlight(333, 51, 1, Exit_Button_Main_Page[0], Exit_Button_Main_Page[1]);
						if (mouse_button_down == true)
						{
							// mouse down
							quit = true;
						}
					}
				}
			}

			// main menu loop
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