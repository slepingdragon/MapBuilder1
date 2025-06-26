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
#include <shobjidl_core.h>
#include <commdlg.h>
#include <cstdlib>
#include "map.h"
#include <thread>




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
bool TextInput_Map_Height = false;
bool TextInput_Map_Pixel_Size = false;
std::string input_map_width_string;
std::string input_map_height_string;
std::string input_map_pixel_size_string;
bool create_first_or_new_map_bool = false;



// create map tools
bool FILL_BUCKET_TOOL = false;
bool LINE_TOOL = false;
bool SINGLE_CLICK = true;




// window settings
int window_width = 1084;
int window_height = 1000;
int iterations = 0;
int fps = 60;
const int frameDelay = 1000 / fps;
bool isFullscreen = false;
float window_player_center_x;
float window_player_center_y;



// SDL VARIABLES *****
SDL_Window* window = NULL;
SDL_Surface* screenSurface = NULL;
SDL_Surface* source;
SDL_Surface* destination;
SDL_Renderer* renderer;
SDL_Rect offset;
SDL_Texture* player_texture_loaded;
SDL_Surface* optimizedImg = NULL;



// WHAT THE IS THIS
float global_offset_x = 0;
float global_offset_y = 0;
float mouse_x, mouse_y;
int offset_after_window_change_x;
int offset_after_window_change_y;



// MAP VARIABLES
int grid_pixel_size_set = 32;



// user variables
int real_player_cord_x = window_width / 2;
int real_player_cord_y = window_height / 2;
int render_distance = 19;
int render_distance_pixel_distance = grid_pixel_size_set * render_distance;
int player_speed = 5;
int selected_texture = NULL; // 0 is default in this case



// key pressed keys
bool key_1_pressed = false;
bool lock_key_1_pressed = false;
bool mouse_button_down = false;
double calculated_angle_for_target = NULL;
const bool* keys = SDL_GetKeyboardState(NULL);

Uint32 frameStart;
int frameTime;



// load map bools
bool load_map = false;
bool load_map_initiate = false;
bool File_Dropdown = false;
bool Main_Menu_Pressed = false;



// create inits of textures
SDL_Texture* thirtytwo_pxGrid_texture = NULL;
SDL_Texture* Top_Bar_selector_texture = NULL;
SDL_Texture* Top_Bar_selector_Highlighter_texture = NULL;
SDL_Texture* Main_Menu_Texture = NULL;
SDL_Texture* Create_Map_Menu_Texture = NULL;
SDL_Texture* Help_page_Texture = NULL;
SDL_Texture* Create_map_page_error_texture = NULL;

// textures of ui for loading part 
SDL_Texture* OpeningLoad_texture = NULL;
SDL_Texture* Load_Map_Ui_Top_Bar_texture = NULL;
SDL_Texture* File_Load_Dropdown_texture = NULL;
SDL_Texture* Save_And_Exit_texture = NULL;


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
	if (TTF_Init() == NULL)
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


// MAX IS 3 weights for now can increase
// 1 = offblack
// 0 = white
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

	// off black
	if (color == 1)
	{
		SDL_SetRenderDrawColor(renderer, off_black[0], off_black[1], off_black[2], 255);
	}

	// white
	if (color == 0) 
	{
		SDL_SetRenderDrawColor(renderer, 255, 255, 255, 0);
	}
	if (color == 2)
	{
		SDL_SetRenderDrawColor(renderer, off_white[0], off_white[1], off_white[2], 255);
	}

	SDL_FRect Highlight_Button_Rect = { x_cord, y_cord, width, height };
	if (highlight_weight == 2)
	{
		SDL_FRect Highlight_Button_Rect_w1 = { x_cord + 1, y_cord + 1, width - 2, height - 2 };
		SDL_RenderRect(renderer, &Highlight_Button_Rect_w1);
		if (highlight_weight == 3)
		{
			SDL_FRect Highlight_Button_Rect_w2 = { x_cord + 2, y_cord + 2, width - 4, height - 4 };
			SDL_RenderRect(renderer, &Highlight_Button_Rect_w2);
		}
	}


	SDL_RenderRect(renderer, &Highlight_Button_Rect);
}

std::string OpenFileDialog() {
	HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
	std::string result = "";

	if (SUCCEEDED(hr)) {
		IFileOpenDialog* pFileOpen;

		hr = CoCreateInstance(CLSID_FileOpenDialog, NULL, CLSCTX_ALL,
			IID_IFileOpenDialog, reinterpret_cast<void**>(&pFileOpen));

		if (SUCCEEDED(hr)) {
			hr = pFileOpen->Show(NULL);

			if (SUCCEEDED(hr)) {
				IShellItem* pItem;
				hr = pFileOpen->GetResult(&pItem);

				if (SUCCEEDED(hr)) {
					PWSTR pszFilePath;
					hr = pItem->GetDisplayName(SIGDN_FILESYSPATH, &pszFilePath);

					if (SUCCEEDED(hr)) {
						// Convert wide string (PWSTR) to std::string
						char charPath[MAX_PATH];
						wcstombs(charPath, pszFilePath, MAX_PATH);
						result = std::string(charPath);

						CoTaskMemFree(pszFilePath);
					}
					pItem->Release();
				}
			}
			pFileOpen->Release();
		}
		CoUninitialize();
	}

	return result;
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

// LOAD TEXTURES OR RELOAD / ALSO LOAD IMAGES SECTION TEXTURES / IMAGES GO HERE

// IMAGES
SDL_Surface* Right_Picker = loadImage("../MapBuilder/Image_Sprites/Ui_Sprites/Right_Picker.png");
SDL_Surface* Right_Side_Separater = loadImage("../MapBuilder/Image_Sprites/Ui_Sprites/Right_Side_Separater.png");
SDL_Surface* Right_Side_Separater_bottom = loadImage("../MapBuilder/Image_Sprites/Ui_Sprites/Right_Side_Separator_bottom.png");
SDL_Surface* Main_Page_image = loadImage("../MapBuilder/Image_sprites/Ui_Sprites/Main_Page.png");
SDL_Surface* thirtytwo_pxGrid = loadImage("../MapBuilder/Image_Sprites/Grid_Sprites/32pxGrid.png");
SDL_Surface* Top_Bar_selector = loadImage("../MapBuilder/Image_Sprites/Ui_Sprites/Top_Bar_Ui_Selector.png");
SDL_Surface* Top_Bar_selector_Highlighter = loadImage("../MapBuilder/Image_Sprites/Ui_Sprites/Ui_Selector_Highlighter_SIDES.png");
SDL_Surface* Create_Map_Menu_img = loadImage("../MapBuilder/Image_Sprites/Ui_Sprites/Create_Map.png");
SDL_Surface* Help_page_img = loadImage("../MapBuilder/Image_Sprites/Ui_Sprites/Help_Page.png");
SDL_Surface* Create_map_page_error_img = loadImage("../MapBuilder/Image_Sprites/Ui_Sprites/Create_Map_Page_Error.png");
SDL_Surface* Load_Map_Ui_Top_Bar_img = loadImage("../MapBuilder/Image_Sprites/Ui_Sprites/Load_Map_Ui_Top_Bar.png");
SDL_Surface* File_Load_Dropdown_img = loadImage("../MapBuilder/Image_Sprites/Ui_Sprites/File_Load_Dropdown.png");
SDL_Surface* Save_And_Exit_img = loadImage("../MapBuilder/Image_Sprites/Ui_Sprites/Save_And_Exit.png");


// UI FOR LOADING MAP LOADING // AND PIXEL SIZES
SDL_Surface* OpeningLoad_img = loadImage("../MapBuilder/Image_Sprites/Ui_Sprites/OpeningLoad.png");


static void reload_load_load_textures()
{
	if (!OpeningLoad_img)
	{
		std::cout << "ERRORLIOADING IUMAGE";
	}

	SDL_DestroyTexture(OpeningLoad_texture);
	SDL_DestroyTexture(thirtytwo_pxGrid_texture);
	SDL_DestroyTexture(Top_Bar_selector_texture);
	SDL_DestroyTexture(Top_Bar_selector_Highlighter_texture);
	SDL_DestroyTexture(Main_Menu_Texture);
	SDL_DestroyTexture(Create_Map_Menu_Texture);
	SDL_DestroyTexture(Help_page_Texture);
	SDL_DestroyTexture(Create_map_page_error_texture);
	SDL_DestroyTexture(Load_Map_Ui_Top_Bar_texture);
	SDL_DestroyTexture(Save_And_Exit_texture);

	Save_And_Exit_texture = SDL_CreateTextureFromSurface(renderer, Save_And_Exit_img);
	File_Load_Dropdown_texture = SDL_CreateTextureFromSurface(renderer, File_Load_Dropdown_img);
	Load_Map_Ui_Top_Bar_texture = SDL_CreateTextureFromSurface(renderer, Load_Map_Ui_Top_Bar_img);
	OpeningLoad_texture = SDL_CreateTextureFromSurface(renderer, OpeningLoad_img);
	thirtytwo_pxGrid_texture = SDL_CreateTextureFromSurface(renderer, thirtytwo_pxGrid);
	Top_Bar_selector_texture = SDL_CreateTextureFromSurface(renderer, Top_Bar_selector);
	Top_Bar_selector_Highlighter_texture = SDL_CreateTextureFromSurface(renderer, Top_Bar_selector_Highlighter);
	Main_Menu_Texture = SDL_CreateTextureFromSurface(renderer, Main_Page_image);
	Create_Map_Menu_Texture = SDL_CreateTextureFromSurface(renderer, Create_Map_Menu_img);
	Help_page_Texture = SDL_CreateTextureFromSurface(renderer, Help_page_img);
	Create_map_page_error_texture = SDL_CreateTextureFromSurface(renderer, Create_map_page_error_img);
}





// LOAD MAP FUNCTIONS

std::map<std::string, std::map<std::string, int >> texture_value;
std::map<std::string, SDL_Texture*> Texture_Map_1;
bool maximized_window = false;
bool first_game_load = false;
bool update_display = false;

//void update_display_(int player_x, int player_y)
//{
//	int real_player_cord_x = player_x;
//	int real_player_cord_y = player_y;
//	int x_player_cord = player_x;
//	int y_player_cord = player_y;
//	int offset_after_window_change_x;
//	int offset_after_window_change_y;
//
//	int original_player_cordinate_y = real_player_cord_y;
//
//	if (maximized_window == true || first_game_load == true || update_display == true)
//	{
//		if (maximized_window == true || update_display == true)
//		{
//			// gets the surface of the window
//			screenSurface = SDL_GetWindowSurface(window);
//			if (screenSurface == NULL)
//			{
//				fprintf(stderr, "could not get window: %s\n", SDL_GetError());
//			}
//
//			// gets the renderer of the surface
//			renderer = SDL_CreateSoftwareRenderer(screenSurface);
//			if (renderer == NULL)
//			{
//				fprintf(stderr, "COULRD NOT INITIALIZE RENDERER");
//			}
//			int original_player_cordinate_x = real_player_cord_x;
//
//			offset_after_window_change_x = original_player_cordinate_x - x_player_cord;
//			offset_after_window_change_y = original_player_cordinate_y - y_player_cord;
//
//			std::cout << offset_after_window_change_x << std::endl;
//
//		}
//
//		// updates textures
//		reload_load_load_textures();
//
//
//
//		// TEXTURE LOADER FOR MAP NOT FOR APPLICATION START****************
//
//		int texture_map_x = 32;
//		int texture_map_y = 32;
//
//		for (auto const& key : Texture_Map_1)
//		{
//			SDL_DestroyTexture(key.second);
//		}
//
//
//		Texture_Map_1.clear();
//		texture_value.clear();
//
//
//		for (int i = 0; i < files_to_load_images.size(); i++)
//		{
//			std::string ii = std::to_string(i);
//
//			std::map<std::string, int > Texture_Map_1_info;
//
//			SDL_Surface* image_loading_surface_ = loadImage(files_to_load_images[i]);
//			if (!image_loading_surface_)
//			{
//				SDL_Log("Failed to initialize image_loading_surface_");
//			}
//
//			SDL_Texture* texture_loaded_ = SDL_CreateTextureFromSurface(renderer, image_loading_surface_);
//			if (!texture_loaded_)
//			{
//				SDL_Log("Failed to initialize texture_loaded_");
//			}
//
//			// texture_map_info set the x_cord starting position of the texture map, with y_cord, texture_value is the value of the texture, selected is the highlighter to check if the texture is selected in the texture map
//			Texture_Map_1_info.insert({ {"x_cord", texture_map_x }, { "y_cord", texture_map_y }, {"texture_value", i }, {"selected", 0} });
//			texture_value.insert({ ii, Texture_Map_1_info });
//			Texture_Map_1.insert({ ii, texture_loaded_ });
//
//			texture_map_x += 32;
//		}
//
//
//		update_display = false;
//		first_game_load = false;
//		maximized_window = false;
//	}
//}




void update_display_()
{
	std::cout << "display change";
	if (maximized_window == true || first_game_load == true || update_display == true)
	{
		if (maximized_window == true || update_display == true)
		{
			// gets the surface of the window
			screenSurface = SDL_GetWindowSurface(window);
			if (screenSurface == NULL)
			{
				fprintf(stderr, "could not get window: %s\n", SDL_GetError());
			}

			// gets the renderer of the surface
			renderer = SDL_CreateSoftwareRenderer(screenSurface);
			if (renderer == NULL)
			{
				fprintf(stderr, "COULRD NOT INITIALIZE RENDERER");
			}

			std::cout << window_width;

			offset_after_window_change_x = real_player_cord_x - window_player_center_x;
			offset_after_window_change_y = real_player_cord_y - window_player_center_y;

			std::cout << offset_after_window_change_x << std::endl;

		}

		// updates textures
		reload_load_load_textures();



		// TEXTURE LOADER FOR MAP NOT FOR APPLICATION START****************

		int texture_map_x = 32;
		int texture_map_y = 32;

		for (auto const& key : Texture_Map_1)
		{
			SDL_DestroyTexture(key.second);
		}


		Texture_Map_1.clear();
		texture_value.clear();


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


		update_display = false;
		first_game_load = false;
		maximized_window = false;
	}
}

void update_player_settings_for_window_resolution()
{
	window_player_center_x = window_width / 2.0f;
	window_player_center_y = window_height / 2.0f;

	// this here
	real_player_cord_x = window_player_center_x - global_offset_x;
	real_player_cord_y = window_player_center_y - global_offset_y;
}



void render_surrounding_map(int player_x, int player_y, int render_y, int render_x, int grid_size_pixel)
{
	int map_start_x = 0;
	int map_start_y = 0;
	int real_player_cord_x = player_x;
	int real_player_cord_y = player_y;
	int render_distance_width = render_x;
	int render_distance_height = render_y;
	int grid_pixel_size_set = grid_size_pixel;
	int center_y_value_number;
	int center_x_value_number;
	int render_distance_pixel_distance_width = render_distance_width * grid_pixel_size_set;
	int render_distance_pixel_distance_height = render_distance_height * grid_pixel_size_set;

	// map render varis
	map_start_x = 0;
	map_start_y = 0;

	center_y_value_number = real_player_cord_y / 32;
	center_x_value_number = real_player_cord_x / 32;

	for (int i = center_y_value_number - render_distance_height; i < center_y_value_number + render_distance_height; i++)
	{
		SDL_FRect sixteen_pixel_rect_SET = { 0.0f, 0.0f, grid_pixel_size_set, grid_pixel_size_set };

		for (int ii = center_x_value_number - render_distance_width; ii < center_x_value_number + render_distance_width; ii++)
		{
			std::string iii = std::to_string(i);
			std::string iiii = std::to_string(ii);

			// render handle
			if (real_player_cord_x > data_map[iii][iiii]["x_cord"] + render_distance_pixel_distance_width || real_player_cord_x + render_distance_pixel_distance_width < data_map[iii][iiii]["x_cord"])
			{
				continue;
			}
			if (real_player_cord_y > data_map[iii][iiii]["y_cord"] + render_distance_pixel_distance_height || real_player_cord_y + render_distance_pixel_distance_height < data_map[iii][iiii]["y_cord"])
			{
				continue;
			}
			SDL_FRect sixteen_pixel_rect_MANAGE = { data_map[iii][iiii]["x_cord"] + global_offset_x, data_map[iii][iiii]["y_cord"] + global_offset_y, grid_pixel_size_set, grid_pixel_size_set };
			if (data_map[iii][iiii]["texture_value"] == 555)
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
}



int main(int argc, char* argv[])
{
	if (!init()) return 1;


	// TEXTURE LOADER FOR MAP NOT FOR APPLICATION START****************
	Create_Image_Vector();

	std::map<std::string, SDL_Texture*> Texture_Map_1;
	std::map<std::string, std::map<std::string, int >> texture_value;

	int texture_map_x = 32;
	int texture_map_y = 32;

	reload_load_load_textures();


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
	// setup grid map
	// settings
	// 5 is default values
	int map_size_width = 100;
	int map_size_height = 100;
	int default_texture_value = 555; // 555 is set at default because...

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

	thirtytwo_pxGrid_texture = SDL_CreateTextureFromSurface(renderer, thirtytwo_pxGrid);
	Top_Bar_selector_texture = SDL_CreateTextureFromSurface(renderer, Top_Bar_selector);
	Top_Bar_selector_Highlighter_texture = SDL_CreateTextureFromSurface(renderer, Top_Bar_selector_Highlighter);
	Main_Menu_Texture = SDL_CreateTextureFromSurface(renderer, Main_Page_image);
	Create_Map_Menu_Texture = SDL_CreateTextureFromSurface(renderer, Create_Map_Menu_img);
	Help_page_Texture = SDL_CreateTextureFromSurface(renderer, Help_page_img);
	Create_map_page_error_texture = SDL_CreateTextureFromSurface(renderer, Create_map_page_error_img);




	// create texture map
	std::map<std::string, std::map<std::string, int>> texture_map;

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







	// start of map builder loops
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
					if (TextInput_Map_Width == true || TextInput_Map_Height == true || TextInput_Map_Pixel_Size == true && load_map == false)
					{
						if (event.key.key == SDLK_0)
						{
							if (TextInput_Map_Width == true)
							{
								input_map_width_string = input_map_width_string + "0";
							}
							if (TextInput_Map_Height == true)
							{
								input_map_height_string = input_map_height_string + "0";
							}
							if (TextInput_Map_Pixel_Size == true)
							{
								input_map_pixel_size_string = input_map_pixel_size_string + "0";
							}
						}
						if (event.key.key == SDLK_1)
						{
							if (TextInput_Map_Width == true)
							{
								input_map_width_string = input_map_width_string + "1";
							}
							if (TextInput_Map_Height == true)
							{
								input_map_height_string = input_map_height_string + "1";
							}
							if (TextInput_Map_Pixel_Size == true)
							{
								input_map_pixel_size_string = input_map_pixel_size_string + "1";
							}
						}
						if (event.key.key == SDLK_2)
						{
							if (TextInput_Map_Width == true)
							{
								input_map_width_string = input_map_width_string + "2";
							}
							if (TextInput_Map_Height == true)
							{
								input_map_height_string = input_map_height_string + "2";
							}
							if (TextInput_Map_Pixel_Size == true)
							{
								input_map_pixel_size_string = input_map_pixel_size_string + "2";
							}
						}
						if (event.key.key == SDLK_3)
						{
							if (TextInput_Map_Width == true)
							{
								input_map_width_string = input_map_width_string + "3";
							}
							if (TextInput_Map_Height == true)
							{
								input_map_height_string = input_map_height_string + "3";
							}
							if (TextInput_Map_Pixel_Size == true)
							{
								input_map_pixel_size_string = input_map_pixel_size_string + "3";
							}
						}
						if (event.key.key == SDLK_4)
						{
							if (TextInput_Map_Width == true)
							{
								input_map_width_string = input_map_width_string + "4";
							}
							if (TextInput_Map_Height == true)
							{
								input_map_height_string = input_map_height_string + "4";
							}
							if (TextInput_Map_Pixel_Size == true)
							{
								input_map_pixel_size_string = input_map_pixel_size_string + "4";
							}
						}
						if (event.key.key == SDLK_5)
						{
							if (TextInput_Map_Width == true)
							{
								input_map_width_string = input_map_width_string + "5";
							}
							if (TextInput_Map_Height == true)
							{
								input_map_height_string = input_map_height_string + "5";
							}
							if (TextInput_Map_Pixel_Size == true)
							{
								input_map_pixel_size_string = input_map_pixel_size_string + "5";
							}
						}
						if (event.key.key == SDLK_6)
						{
							if (TextInput_Map_Width == true)
							{
								input_map_width_string = input_map_width_string + "6";
							}
							if (TextInput_Map_Height == true)
							{
								input_map_height_string = input_map_height_string + "6";
							}
							if (TextInput_Map_Pixel_Size == true)
							{
								input_map_pixel_size_string = input_map_pixel_size_string + "6";
							}
						}
						if (event.key.key == SDLK_7)
						{
							if (TextInput_Map_Width == true)
							{
								input_map_width_string = input_map_width_string + "7";
							}
							if (TextInput_Map_Height == true)
							{
								input_map_height_string = input_map_height_string + "7";
							}
							if (TextInput_Map_Pixel_Size == true)
							{
								input_map_pixel_size_string = input_map_pixel_size_string + "7";
							}
						}
						if (event.key.key == SDLK_8)
						{
							if (TextInput_Map_Width == true)
							{
								input_map_width_string = input_map_width_string + "8";
							}
							if (TextInput_Map_Height == true)
							{
								input_map_height_string = input_map_height_string + "8";
							}
							if (TextInput_Map_Pixel_Size == true)
							{
								input_map_pixel_size_string = input_map_pixel_size_string + "8";
							}
						}
						if (event.key.key == SDLK_9)
						{
							if (TextInput_Map_Width == true)
							{
								input_map_width_string = input_map_width_string + "9";
							}
							if (TextInput_Map_Height == true)
							{
								input_map_height_string = input_map_height_string + "9";
							}
							if (TextInput_Map_Pixel_Size == true)
							{
								input_map_pixel_size_string = input_map_pixel_size_string + "9";
							}
						}
						if (event.key.key == SDLK_BACKSPACE)
						{
							if (TextInput_Map_Width == true)
							{
								if (input_map_width_string.size() > 0)
								{
									input_map_width_string.pop_back();
								}
							}
							if (TextInput_Map_Height == true)
							{
								if (input_map_height_string.size() > 0)
								{
									input_map_height_string.pop_back();
								}
							}
							if (TextInput_Map_Pixel_Size == true)
							{
								if (input_map_pixel_size_string.size() > 0)
								{
									input_map_pixel_size_string.pop_back();
								}
							}
						}
					}

					// make fill bucket equipped
					if (event.key.key == SDLK_G)
					{
						if (load_map = false)
						{
							if (FILL_BUCKET_TOOL == true)
							{
								SINGLE_CLICK = true;

								FILL_BUCKET_TOOL = false;
								LINE_TOOL = false;
							}
							FILL_BUCKET_TOOL = true;

							LINE_TOOL = false;
							SINGLE_CLICK = false;
						}
					}

					// make line tool equipped
					if (event.key.key == SDLK_L)
					{
						if (load_map = false)
						{
							if (LINE_TOOL == true)
							{
								SINGLE_CLICK = true;

								FILL_BUCKET_TOOL = false;
								LINE_TOOL = false;
							}
							LINE_TOOL = true;

							FILL_BUCKET_TOOL = false;
							SINGLE_CLICK = false;
						}
					}

					break;
				case SDL_EVENT_WINDOW_RESIZED:
					if (maximized_window == true)
					{
						window_width = 1000; // 1000 is default size for window width

						window_height = 800; // 800 is default size for window height

						maximized_window = false;
						update_display = true;
						break;
					}

					window_width = event.window.data1;

					window_height = event.window.data2;

					maximized_window = true;
					update_display = true;
					// update all variables
					update_player_settings_for_window_resolution();

					update_display_();

					//std::cout << "resized with these new parameters:" << std::endl;

					//std::cout << window_width << std::endl;
					//std::cout << window_height << std::endl;

					break;

			}
		}

		if (Main_Menu_Open == false)
		{



			if (Create_Map == true)
			{
				SDL_SetRenderDrawColor(renderer, 32, 32, 32, 255);
				SDL_Color color = { 32, 32, 32 }; // red
				SDL_FillSurfaceRect(screenSurface, NULL, 0xFFFFFFFF);



				if (create_first_or_new_map_bool == true)
				{
					// create map first time or a new map
					
					// map sets

					// conversions and setter
					int map_size_width = std::stoi(input_map_width_string);
					int map_size_height = std::stoi(input_map_height_string);



					std::cout << map_size_width;



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



							// THIS VALUE CAN BE DELETED FOR THE DEFAULT MAP MADE TEXTURE
							data_map[iii][iiii]["texture_value"] = 0;



							grid_start_x += grid_pixel_size_set;

						}
						grid_start_x = 0;
						grid_start_y += grid_pixel_size_set;
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



					create_first_or_new_map_bool = false;
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
					SDL_FRect sixteen_pixel_rect_SET = { 0.0f, 0.0f, grid_pixel_size_set, grid_pixel_size_set };

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
						SDL_FRect sixteen_pixel_rect_MANAGE = { data_map[iii][iiii]["x_cord"] + global_offset_x, data_map[iii][iiii]["y_cord"] + global_offset_y, grid_pixel_size_set, grid_pixel_size_set };
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
					SDL_FRect Texture_List_Visible_SET = { 0.0f, 0.0f, grid_pixel_size_set, grid_pixel_size_set };
					SDL_FRect Texture_List_Visible_MANAGE = { texture_value[ii]["x_cord"], texture_value[ii]["y_cord"], grid_pixel_size_set, grid_pixel_size_set };

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



				// HIGHLIGHTER FOR TOOLS

				// fill bucket
				if (mouse_x >= 1020 && mouse_x <= 1055 && mouse_y >= 967 && mouse_y <= 997)
				{
					button_highlight(35, 30, 1, 1020, 967);
					if (mouse_button_down == true)
					{
						if (FILL_BUCKET_TOOL == true)
						{
							SINGLE_CLICK = true;

							FILL_BUCKET_TOOL = false;
							LINE_TOOL = false;
						}

						// make this tool equipped
						FILL_BUCKET_TOOL = true;


						// make all other tools disabled
						LINE_TOOL = false;
						LINE_TOOL = false;
					}
				}

				// line tool
				if (mouse_x >= 985 && mouse_x <= 1019 && mouse_y >= 967 && mouse_y <= 997)
				{
					button_highlight(35, 30, 1, 985, 967);
					if (mouse_button_down == true)
					{
						if (LINE_TOOL == true)
						{
							SINGLE_CLICK = true;

							FILL_BUCKET_TOOL = false;
							LINE_TOOL = false;
						}

						// make this tool equipped
						LINE_TOOL = true;


						// make all other tools disabled
						FILL_BUCKET_TOOL = false;
						LINE_TOOL = false;
					}
				}

				// 3rd slot 
				if (mouse_x >= 951 && mouse_x <= 984 && mouse_y >= 967 && mouse_y <= 997)
				{
					button_highlight(35, 30, 1, 951, 967);
				}

				// 2 slot
				if (mouse_x >= 916 && mouse_x <= 950 && mouse_y >= 967 && mouse_y <= 997)
				{
					button_highlight(35, 30, 1, 916, 967);
				}

				// 1 slot
				if (mouse_x >= 880 && mouse_x <= 915 && mouse_y >= 967 && mouse_y <= 997)
				{
					button_highlight(35, 30, 1, 880, 967);
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
							

							// reset map.
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

									int map_width_for_binary = map_size_width;
									int map_height_for_binary = map_size_height;
									int map_grid_pixel_size_for_binary = grid_pixel_size_set;

									file.write(reinterpret_cast<const char*>(&map_width_for_binary), sizeof(map_width_for_binary));
									file.write(reinterpret_cast<const char*>(&map_height_for_binary), sizeof(map_height_for_binary));
									file.write(reinterpret_cast<const char*>(&map_grid_pixel_size_for_binary), sizeof(map_grid_pixel_size_for_binary));


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
									file << "int grid_pixel_size_set = " << grid_pixel_size_set << ";\n";
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
									file << "            grid_start_x += grid_pixel_size_set;\n";
									file << "        }\n";
									file << "        iteration_width_map += map_width;\n";
									file << "        grid_start_x = 0;\n";
									file << "        grid_start_y += grid_pixel_size_set;\n";
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



			if (load_map == true)
			{
				// LOAD OUTSIDE GUI
				SDL_FRect Opening_Load_SET = { 0.0f,0.0f,2000.0f,1000.0f };
				SDL_FRect Opening_Load_MANAGE = { 0, 0, window_width, window_height };

				SDL_RenderTexture(renderer, OpeningLoad_texture, &Opening_Load_SET, &Opening_Load_MANAGE);
				// LOAD TOP BAR GUI
				float top_bar_load_width = 514.0;
				float top_bar_load_height = 65.0;

				SDL_FRect Top_Bar_load_SET = { 0.0f, 0.0f, top_bar_load_width, top_bar_load_height };
				SDL_FRect Top_Bar_load_MANAGE = { 0, 0, top_bar_load_width, top_bar_load_height };

				SDL_RenderTexture(renderer, Load_Map_Ui_Top_Bar_texture, &Top_Bar_load_SET, &Top_Bar_load_MANAGE);
				if (Main_Menu_Pressed == true)
				{
					SDL_FRect Save_And_Exit_SET = { 0.0f,0.0f,600,170 };
					SDL_FRect Save_And_Exit_MANAGE = { (window_width / 2) - 300, (window_height / 2) - 111,600,170 };

					int savfe_and_exit_x = (window_width / 2) - 300;
					int savfe_and_exit_y = (window_height / 2) - 111;

					SDL_RenderTexture(renderer, Save_And_Exit_texture, &Save_And_Exit_SET, &Save_And_Exit_MANAGE); // 21 111



					// save an exit button
					if (mouse_x >= 21 + savfe_and_exit_x && mouse_x <= 226 + savfe_and_exit_x && mouse_y >= 111 + savfe_and_exit_y && mouse_y <= 155 + savfe_and_exit_y)
					{
						button_highlight(203, 43, 3, 22 + savfe_and_exit_x, 112 + savfe_and_exit_y, 1);
					}



					// not save and exit button
					if (mouse_x >= 246 + savfe_and_exit_x && mouse_x <= 575 + savfe_and_exit_x && mouse_y >= 111 + savfe_and_exit_y && mouse_y <= 155 + savfe_and_exit_y)
					{
						button_highlight(328, 43, 3, 247 + savfe_and_exit_x, 112 + savfe_and_exit_y, 1);
						if (mouse_button_down == true)
						{
							Main_Menu_Open = true;
							load_map = false;
							Main_Menu_Pressed = false;
							mouse_button_down = false;
						}
					}



					// x button to go back to the map.
					if (mouse_x >= 565 + savfe_and_exit_x && mouse_x <= 599 + savfe_and_exit_x && mouse_y >= 0 + savfe_and_exit_y && mouse_y <= 28 + savfe_and_exit_y)
					{
						button_highlight(33, 26, 3, 566 + savfe_and_exit_x, 1 + savfe_and_exit_y, 1);
						if (mouse_button_down == true)
						{
							Main_Menu_Pressed = false;
							mouse_button_down = false;
						}
					}


					mouse_button_down = false;


				}
				else
				{
					//if (load_map_initiate == true)
					//{								
					//	load_map_initiate = false;
					//	data_map_create();
					//}
					//// render map
					//render_surrounding_map(real_player_cord_x, real_player_cord_y, render_distance, render_distance, grid_pixel_size_set);

					// File button
					if (mouse_x >= 7 && mouse_x <= 99 && mouse_y >= 4 && mouse_y <= 54)
					{
						button_highlight(92, 50, 1, 7, 4, 1);
						if (mouse_button_down == true)
						{
							if (File_Dropdown == true)
							{
								File_Dropdown = false;
								mouse_button_down = false;
							}
							else
							{
								File_Dropdown = true;

								mouse_button_down = false;
							}
						}
					}
					else
					{
						if (mouse_button_down == true && File_Dropdown == true)
						{
							if (mouse_x >= 4 && mouse_x <= 229 && mouse_y >= 72 && mouse_y <= 354)
							{
								


							}
							else
							{
								File_Dropdown = false;
								mouse_button_down = false;
							}
						}
					}

					if (File_Dropdown == true)
					{
						float File_Dropdown_width = 300.0;
						float File_Dropdown_height = 400.0;

						SDL_FRect File_Dropdown_SET = { 0.0f, 0.0f, File_Dropdown_width, File_Dropdown_height };
						SDL_FRect File_Dropdown_MANAGE = { 0.0f, 0.0f, File_Dropdown_width, File_Dropdown_height };

						SDL_RenderTexture(renderer, File_Load_Dropdown_texture, &File_Dropdown_SET, &File_Dropdown_MANAGE);
					}

					// clicking file opens these things ->>>
					if (mouse_x >= 4 && mouse_x <= 229 && mouse_y >= 72 && mouse_y <= 354 && File_Dropdown == true)
					{
						// first bar
						if (mouse_x >= 11 && mouse_x <= 223 && mouse_y >= 78 && mouse_y <= 114)
						{
							button_highlight(212, 37, 3, 11, 78, 1);
						}
						// second bar
						if (mouse_x >= 11 && mouse_x <= 223 && mouse_y >= 117 && mouse_y <= 153)
						{
							button_highlight(212, 37, 1, 11, 117, 1);
						}
						// third bar  -- > settings
						if (mouse_x >= 11 && mouse_x <= 223 && mouse_y >= 156 && mouse_y <= 192)
						{
							button_highlight(212, 37, 1, 11, 156, 1);
						}
						// fourth bar
						if (mouse_x >= 11 && mouse_x <= 223 && mouse_y >= 195 && mouse_y <= 231)
						{
							button_highlight(212, 37, 1, 11, 195, 1);
						}
						// fifth bar
						if (mouse_x >= 11 && mouse_x <= 223 && mouse_y >= 234 && mouse_y <= 270)
						{
							button_highlight(212, 37, 1, 11, 234, 1);
						}
						// sixth bar  -- > Open File
						if (mouse_x >= 11 && mouse_x <= 223 && mouse_y >= 273 && mouse_y <= 309)
						{
							button_highlight(212, 37, 1, 11, 273, 1);
							if (mouse_button_down == true)
							{
								std::string selected_file_path_name = OpenFileDialog();

								std::cout << "File: " << selected_file_path_name << std::endl;

								int map_wdith_from_bin;
								int map_height_from_bin;
								int map_gridpixelsize_from_bin;


								std::vector<int> texture_vector_from_bin;
								int texture_value_from_bin;
								std::ifstream inside_file(selected_file_path_name, std::ios::binary);
								if (!inside_file)
								{
									std::cout << "error loading bin file \"inside_file\"" << std::endl;
								}
								inside_file.read(reinterpret_cast<char*>(&map_wdith_from_bin), sizeof(map_wdith_from_bin));
								std::cout << map_wdith_from_bin;

								//for (int i = 0; i < map_wdith_from_bin * map_height_from_bin; i++)
								//{
								//	inside_file.read(reinterpret_cast<char*>(&texture_value_from_bin), sizeof(texture_value_from_bin));
								//	texture_vector_from_bin.push_back(texture_value_from_bin);
								//}




								inside_file.close();




								//for (int i = 0; i < map_height_from_bin; i++)
								//{
								//	std::string ii = std::to_string(i);
								//	for (int iii = 0; iii < map_wdith_from_bin; iii++)
								//	{
								//		std::string iiii = std::to_string(iii);
								//		second_level_map_.insert({ iiii, low_level_map_ });
								//	}
								//	data_map.insert({ ii, second_level_map_ });
								//}
								//for (int i = 0; i < map_height_from_bin; i++)
								//{
								//	for (int ii = 0; ii < map_wdith_from_bin; ii++)
								//	{
								//		std::string iii = std::to_string(i);
								//		std::string iiii = std::to_string(ii);
								//		data_map[iii][iiii]["x_cord"] = grid_start_x;
								//		data_map[iii][iiii]["y_cord"] = grid_start_y;
								//		data_map[iii][iiii]["texture_value"] = texture_vector_from_bin[ii + iteration_width_map];
								//		grid_start_x += map_gridpixelsize_from_bin;
								//	}
								//	iteration_width_map += map_wdith_from_bin;
								//	grid_start_x = 0;
								//	grid_start_y += map_gridpixelsize_from_bin;
								//}
								//grid_start_x = 0;
								//grid_start_y = 0;



								mouse_button_down = false;
							}



						}
						// seventh bar .. ? Save File
						if (mouse_x >= 11 && mouse_x <= 223 && mouse_y >= 312 && mouse_y <= 348)
						{
							button_highlight(212, 37, 1, 11, 312, 1);
						}
					}

					// Main Menu Button
					if (mouse_x >= 260 && mouse_x <= 480 && mouse_y >= 4 && mouse_y <= 54)
					{
						button_highlight(220, 50, 1, 260, 4, 1);
						if (mouse_button_down == true)
						{
							Main_Menu_Pressed = true;


							//std::cout << "so confused";
							//load_map = false;
							//Main_Menu_Open = true;


							mouse_button_down = false;
						}
					}

					// Layers button













					mouse_button_down = false;
				}

				



			}


			if (mouse_button_down == true && load_map == false)
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
							mouse_button_down = false;
						}
					}
				}

				// this determins if a click is needed, by all means i mean this section of code will update the data map where these values are placed by this.
				if (mouse_y + global_offset_y > 256 + global_offset_y)
				{
					for (int i = (mouse_y - global_offset_y) / grid_pixel_size_set; i < (mouse_y - global_offset_y) / grid_pixel_size_set; i++)
					{
						for (int ii = (mouse_x - global_offset_x) / grid_pixel_size_set; ii < (mouse_x - global_offset_x) / grid_pixel_size_set; ii++)
						{
							if (FILL_BUCKET_TOOL == true)
							{
								//for (int w = 0; w < map_size_height; w++)
								//{
								//	std::string ww = std::to_string(w);

								//	for (int www = 0; www < map_size_width; www++)
								//	{
								//		std::string wwww = std::to_string(www);

								//		data_map[www][wwww]["texture_value"] = selected_texture;
								//	}
								//}
								mouse_button_down = false;
							}
							else
							{
								std::string iii = std::to_string(i);
								std::string iiii = std::to_string(ii);

								data_map[iii][iiii]["texture_value"] = selected_texture;

								mouse_button_down = false;
							}
						}
					}
				}
			}

			if (load_map == false)
			{
				for (int i = 0; i < texture_value.size(); i++)
				{
					std::string ii = std::to_string(i);

					if (texture_value[ii]["selected"] == 1)
					{
						SDL_FRect selected_texture_highlighted_set = { 0.0f,0.0f,grid_pixel_size_set,grid_pixel_size_set };
						SDL_FRect selected_texture_highlighted_manage = { texture_value[ii]["x_cord"], texture_value[ii]["y_cord"], grid_pixel_size_set, grid_pixel_size_set };

						SDL_RenderTexture(renderer, Top_Bar_selector_Highlighter_texture, &selected_texture_highlighted_set, &selected_texture_highlighted_manage);
					}
				}
			}
		}










		else // this is where the main menu stuff starts
		{
			SDL_RenderTexture(renderer, Main_Menu_Texture, &Main_Menu_Set, &Main_Menu_Set);

			// CREATE MAP MENU
			if (Create_Map_Menu == true)
			{
				main_menu_button_offline = true;


				// Creeate Map Menu render in
				SDL_FRect Create_Map_Menu_SET = { 0, 0, 542, 500 };
				SDL_FRect Create_Map_Menu_MANAGE = { 271, 250, 542, 500 };

				SDL_RenderTexture(renderer, Create_Map_Menu_Texture, &Create_Map_Menu_SET, &Create_Map_Menu_MANAGE);
				// create map menu render out


				// text renders
				// text font settings
				SDL_Color Input_Map_Text_Color = { 240,240,240 }; // color of font
				int font_width_size_input_create_map_menu = 13;
				int font_height_size_input_create_map_menu = 30;



				// width input text render --
				int size_of_width_map_input = input_map_width_string.size();

				SDL_Surface* textsurface_width_create_menu = TTF_RenderText_Solid(font, input_map_width_string.c_str(), size_of_width_map_input, Input_Map_Text_Color);

				SDL_Texture* textTexture_width_Create_menu = SDL_CreateTextureFromSurface(renderer, textsurface_width_create_menu);

				SDL_FRect textRect_width_input_box = { Map_Width_Create_Page[0] + 273, Map_Width_Create_Page[1] + 250, size_of_width_map_input * font_width_size_input_create_map_menu, font_height_size_input_create_map_menu };

				SDL_RenderTexture(renderer, textTexture_width_Create_menu, nullptr, &textRect_width_input_box);
				// end width input render



				// height input text render --
				int size_of_height_map_input = input_map_height_string.size();

				SDL_Surface* textsurface_height_create_menu = TTF_RenderText_Solid(font, input_map_height_string.c_str(), size_of_height_map_input, Input_Map_Text_Color);

				SDL_Texture* textTexture_height_Create_menu = SDL_CreateTextureFromSurface(renderer, textsurface_height_create_menu);

				SDL_FRect textRect_height_input_box = { Map_Width_Create_Page[0] + 273, Map_Width_Create_Page[1] + 333, size_of_height_map_input * font_width_size_input_create_map_menu, font_height_size_input_create_map_menu };

				SDL_RenderTexture(renderer, textTexture_height_Create_menu, nullptr, &textRect_height_input_box);
				// end height render



				// pixel size text render
				int pixel_size_input = input_map_pixel_size_string.size();

				SDL_Surface* textsurface_pixel_size_create_menu = TTF_RenderText_Solid(font, input_map_pixel_size_string.c_str(), pixel_size_input, Input_Map_Text_Color);

				SDL_Texture* textTexture_pixel_size_Create_menu = SDL_CreateTextureFromSurface(renderer, textsurface_pixel_size_create_menu);

				SDL_FRect textRect_pixel_size_input_box = { Map_Width_Create_Page[0] + 273, Map_Width_Create_Page[1] + 412, pixel_size_input * font_width_size_input_create_map_menu, font_height_size_input_create_map_menu };

				SDL_RenderTexture(renderer, textTexture_pixel_size_Create_menu, nullptr, &textRect_pixel_size_input_box);
				// end pixel size text render





				// Create Map Button
				if (mouse_x >= Create_Map_Button_Create_Page[0] + 271 && mouse_y >= Create_Map_Button_Create_Page[1] + 250)
				{
					if (mouse_x <= Create_Map_Button_Create_Page[0] + 196 + 271 && mouse_y <= Create_Map_Button_Create_Page[1] + 50 + 250)
					{
						button_highlight(197, 51, 1, Create_Map_Button_Create_Page[0] + 271, Create_Map_Button_Create_Page[1] + 250, 2);
						if (mouse_button_down == true)
						{
							// mouse down
							if (size_of_width_map_input <= 0 && size_of_height_map_input <= 0 || pixel_size_input <= 0)
							{
								Show_Create_Map_Error_Message = true;
							}
							else
							{
								Show_Create_Map_Error_Message = false;
								Main_Menu_Open = false;
								Create_Map = true;
								create_first_or_new_map_bool = true;

							}
							mouse_button_down = false;
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
							TextInput_Map_Pixel_Size = false;
							TextInput_Map_Height = false;

							// reset text data structs
							input_map_width_string = "";
							input_map_height_string = "";
							input_map_pixel_size_string = "";

							mouse_button_down = false;
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

							// make these bools true
							TextInput_Map_Width = true;
							map_width_bool_create = true;


							// make these bools false
							TextInput_Map_Height = false;
							Show_Create_Map_Error_Message = false;
							TextInput_Map_Pixel_Size = false;

							mouse_button_down = false;
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

							// make these bools true
							map_height_bool_create = true;
							TextInput_Map_Height = true;

							// make these bools false
							Show_Create_Map_Error_Message = false;
							TextInput_Map_Width = false;
							TextInput_Map_Pixel_Size = false;

							mouse_button_down = false;
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

							// make these bools true
							pixel_size_create_map_menu = true;
							TextInput_Map_Pixel_Size = true;

							// make these bools false
							TextInput_Map_Width = false;
							Show_Create_Map_Error_Message = false;
							TextInput_Map_Height = false;

							mouse_button_down = false;
						}
					}
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



			// Main Menu Page setup *******************************************************************************************
			if (main_menu_button_offline == false)
			{
				// Create Map Menu Button
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

							mouse_button_down = false;
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

							load_map = true;
							Main_Menu_Open = false;
							load_map_initiate = true;

							mouse_button_down = false;
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

							mouse_button_down = false;
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

							mouse_button_down = false;
							quit = true;
						}
					}
				}
			}
		}
		// end of main menu loop
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