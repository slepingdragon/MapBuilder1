﻿#include <glad/glad.h>
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
#include <filesystem>






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
float window_width = 1084;
float window_height = 1000;
int SDL_window_width = int(window_width);
int SDL_window_height = int(window_height);
int iterations = 0;
int fps = 60;
const int frameDelay = 1000 / fps;
bool isFullscreen = false;
float window_player_center_x;
float window_player_center_y;



// SDL VARIABLES *****
SDL_Window* window = NULL;
SDL_Surface* source;
SDL_Surface* destination;
//SDL_Renderer* renderer;
SDL_Rect offset;
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
bool Texture_Load__Dropdown = false;
bool textures_bar_right_load_show = false;
bool load_map_loaded_in_library = false;
bool tile_surfaces_loaded = false;
std::vector<SDL_Surface*> tile_surfaces;
std::vector<GLuint> texture_vector_loaded;
int map_width_from_bin;
int map_height_from_bin;
int map_gridpixelsize_from_bin;
bool load_textures_onto_data_gallery = false;
std::map<std::string, std::map<std::string, std::string>> gallery_data_map;
std::map<std::string, std::map<std::string, std::string>> gallery_data_map_for_render;
std::map<std::string, GLuint> texture_gallery_data_map;
bool textures_loaded_into_gallery = false;
std::vector<int> gallery_highlight_x_cords;
std::vector<int> gallery_highlight_y_cords;
bool layers_button_clicked = false;
bool layer_gallery_show = false;
bool terrain_layer_selected = true;
bool object_layer_selected = false;
bool obstacle_layer_selected = false;
bool top_terrain_layer_selected = false;
bool effects_layer_selected = false;



// create inits of textures
GLuint thirtytwo_pxGrid_texture = NULL;
GLuint Top_Bar_selector_texture = NULL;
GLuint Top_Bar_selector_Highlighter_texture = NULL;
GLuint Main_Menu_Texture = NULL;
GLuint Create_Map_Menu_Texture = NULL;
GLuint Help_page_Texture = NULL;
GLuint Create_map_page_error_texture = NULL;

// textures of ui for loading part 
GLuint OpeningLoad_texture = NULL;
GLuint Load_Map_Ui_Top_Bar_texture = NULL;
GLuint File_Load_Dropdown_texture = NULL;
GLuint Save_And_Exit_texture = NULL;
GLuint Texture_Load_Dropdown_texture = NULL;
GLuint gallery_Show_texture = NULL;
GLuint layer_galary_texture = NULL;
GLuint layers_dropdown_after_button_pressed_texture = NULL;


static bool init()
{
	if (SDL_Init(SDL_INIT_VIDEO) == false)
	{
		fprintf(stderr, "could not initialize sdl2: %s\n", SDL_GetError());
		return false;
	}

	// Request an OpenGL 3.3 Core context (or whatever version you want)
	SDL_SetHint(SDL_HINT_OPENGL_ES_DRIVER, "0");
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

	window = SDL_CreateWindow("Map Builder", window_width, window_height, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
	if (window == NULL)
	{
		fprintf(stderr, "could not create window: %s\n", SDL_GetError());
		return false;
	}

	// create glad context
	SDL_GLContext context = SDL_GL_CreateContext(window);
	if (!context)
	{
		std::cout << "Faile to create gl context: " << SDL_GetError() << "\n";
		return false;
	}

	if (!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress))
	{
		std::cerr << "Failed to initialize GLAD\n";
		return false;
	}

	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

	glEnable(GL_TEXTURE_2D);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0.0f, window_width, window_height, 0.0f, -1.0f, 1.0f);

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
		SDL_Log("could not load image: %s\n", SDL_GetError());
		return nullptr;
	}

	SDL_Surface* optimizedImg = SDL_ConvertSurface(img, SDL_PIXELFORMAT_RGBA32); // format enables transparency on image
	if (optimizedImg == NULL)
	{
		SDL_Log("Could not optimize img");
		return nullptr;
	}

	SDL_DestroySurface(img); // free memory
	return optimizedImg;
}

void DrawTextureOntoWindow(GLfloat x_cord, GLfloat y_cord, GLfloat texture_width_pixel, GLfloat texture_height_pixels, GLuint texture)
{
	glBegin(GL_QUADS);

	glTexCoord2f(0, 0); // top left corner
	glVertex3f(x_cord, y_cord, 0); // -> x, y, z

	glTexCoord2f(1, 0); // top right corner
	glVertex3f(x_cord + texture_width_pixel, y_cord, 0); // -> x, y, z  top right corner

	glTexCoord2f(1, 1); // bottom right corner
	glVertex3f(x_cord + texture_width_pixel, y_cord + texture_height_pixels, 0); // -> x, y, z   bottom right corner

	glTexCoord2f(0, 1); // bottom left corner
	glVertex3f(x_cord, y_cord + texture_height_pixels, 0); // -> x, y, z

	glEnd();
}

float vertices[] = {
	// positions   // tex coords
	0.0f, 0.0f,    0.0f, 0.0f,   // top-left
	100.0f, 0.0f,  1.0f, 0.0f,   // top-right
	100.0f, 100.0f,1.0f, 1.0f,   // bottom-right
	0.0f, 100.0f,  0.0f, 1.0f    // bottom-left
};
unsigned int indices[] = {
	0, 1, 2,
	2, 3, 0
};

void CreateTextureAndDrawOntoSurfaceRGBA8(SDL_Surface* surface, GLfloat x_cord, GLfloat y_cord, GLfloat texture_width_pixel, GLfloat texture_height_pixels, GLuint texture)
{
	if (!surface)
	{
		SDL_Log("Cannot Create Texture From Surface, surface given is invalid.");
		// return here;
	}
	float vertices[] = {
		// positions   // tex coords
		x_cord, y_cord, 0.0f, 0.0f,  // top-left
		x_cord + texture_width_pixel, y_cord,  1.0f, 0.0f,   // top-right
		x_cord + texture_width_pixel, y_cord + texture_height_pixels,1.0f, 1.0f,   // bottom-right
		x_cord, y_cord + texture_height_pixels,  0.0f, 1.0f    // bottom-left
	};
	unsigned int indices[] = {
		0, 1, 2,
		2, 3, 0
	};

	GLuint VAO, VBO, EBO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	// Position attribute
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	// TexCoord attribute
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glBindVertexArray(0);


}

static GLuint createTextureFromSurfaceRGBA8(SDL_Surface* surface) {
	if (!surface) {
		SDL_Log("Cannot create texture: surface is null");
		return 0;
	}

	// 1) Decide what GL “format” your pixels really are.  --> Im using RGBA so it is 8888, 32 bit
	// In SDL3, surface->format is a VALUE, not a pointer.
	//int bpp = surface->format.BytesPerPixel;
	//GLenum pixelFormat;
	//if (bpp == 4) {
	//	// most IMG_Load PNGs will end up as SDL_PIXELFORMAT_RGBA8888 on little-endian,
	//	// which in memory is R,G,B,A so you can use GL_RGBA here.
	//	pixelFormat = GL_RGBA;
	//}
	//else if (bpp == 3) {
	//	pixelFormat = GL_RGB;
	//}
	//else {
	//	SDL_Log("Unsupported pixel format: %d bytes per pixel", bpp);
	//	return 0;
	//}

	// 2) Make sure the row alignment matches your surface pitch.
	//    By default OpenGL expects each row's start to be aligned to 4 bytes.
	//    If your width*BPP isn’t a multiple of 4, tell GL to pack rows tightly:
	// 
	// 
	// 
	// Since it's always RGBA8888, 4 bytes per pixel:
	// If your row‐pitch (surface->pitch) is a multiple of 4, 
	// you can even skip resetting the UNPACK_ALIGNMENT.
	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);

	// 3) Create & bind
	GLuint texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);

	// Set basic texture parameters
	// 4) Set your filtering & wrap modes
	// clamp + linear + mipmap
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	SDL_Log("IM FIRST BIUTH");

	// Upload to GPU
	// Here: internal format = GL_RGBA8, source format = GL_RGBA
	// 5) Upload the pixels into the bound texture
	glTexImage2D(
		GL_TEXTURE_2D,
		0,                  // mip level
		GL_RGBA8,			// internal format (8 bits per channel)
		surface->w,
		surface->h,
		0,                  // border (must be 0) --  cus its deprecated well really...
		GL_RGBA,			// source pixel layout
		GL_UNSIGNED_INT_8_8_8_8,   // source data type
		surface->pixels
	);

	glGenerateMipmap(GL_TEXTURE_2D);

	SDL_Log("ABABABL");

	return texture;
}



static void close()
{
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

	//// off black
	//if (color == 1)
	//{
	//	SDL_SetRenderDrawColor(renderer, off_black[0], off_black[1], off_black[2], 255);
	//}

	//// white
	//if (color == 0) 
	//{
	//	SDL_SetRenderDrawColor(renderer, 255, 255, 255, 0);
	//}
	//if (color == 2)
	//{
	//	SDL_SetRenderDrawColor(renderer, off_white[0], off_white[1], off_white[2], 255);
	//}

	//SDL_FRect Highlight_Button_Rect = { x_cord, y_cord, width, height };
	//if (highlight_weight == 2)
	//{
	//	SDL_FRect Highlight_Button_Rect_w1 = { x_cord + 1, y_cord + 1, width - 2, height - 2 };
	//	SDL_RenderRect(renderer, &Highlight_Button_Rect_w1);
	//	if (highlight_weight == 3)
	//	{
	//		SDL_FRect Highlight_Button_Rect_w2 = { x_cord + 2, y_cord + 2, width - 4, height - 4 };
	//		SDL_RenderRect(renderer, &Highlight_Button_Rect_w2);
	//	}
	//}


	//SDL_RenderRect(renderer, &Highlight_Button_Rect);
}

// function to open file dialog
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

std::string OpenFolderDialog() {
	HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
	std::string result = "";

	if (SUCCEEDED(hr)) {
		IFileOpenDialog* pFileOpen;

		// Create the FileOpenDialog object
		hr = CoCreateInstance(CLSID_FileOpenDialog, NULL, CLSCTX_ALL,
			IID_IFileOpenDialog, reinterpret_cast<void**>(&pFileOpen));

		if (SUCCEEDED(hr)) {
			// Set the options on the dialog to pick folders
			DWORD dwOptions;
			hr = pFileOpen->GetOptions(&dwOptions);
			if (SUCCEEDED(hr)) {
				pFileOpen->SetOptions(dwOptions | FOS_PICKFOLDERS);
			}

			// Show the folder picker
			hr = pFileOpen->Show(NULL);

			if (SUCCEEDED(hr)) {
				IShellItem* pItem;
				hr = pFileOpen->GetResult(&pItem);

				if (SUCCEEDED(hr)) {
					PWSTR pszFolderPath;
					hr = pItem->GetDisplayName(SIGDN_FILESYSPATH, &pszFolderPath);

					if (SUCCEEDED(hr)) {
						char charPath[MAX_PATH];
						wcstombs(charPath, pszFolderPath, MAX_PATH);
						result = std::string(charPath);

						CoTaskMemFree(pszFolderPath);
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
SDL_Surface* Texture_Load_Dropdown_img = loadImage("../MapBuilder/Image_Sprites/Ui_Sprites/Texture_Load_Dropdown.png");
SDL_Surface* Layer_Show_img = loadImage("../MapBuilder/Image_Sprites/Ui_Sprites/layer_show.png");
SDL_Surface* layer_gallery_img = loadImage("../MapBuilder/Image_Sprites/Ui_Sprites/layers_ui.png");
SDL_Surface* layers_dropdown_after_button_pressed_img = loadImage("../MapBuilder/Image_Sprites/Ui_Sprites/layers_dropdown_after_button_pressed.png");
SDL_Surface* OpeningLoad_img = loadImage("../MapBuilder/Image_Sprites/Ui_Sprites/OpeningLoad.png");


std::vector<SDL_Surface*> Load_Images_From_A_Vector(std::vector<std::string> vector)
{
	std::vector<SDL_Surface*> return_this_vector;
	
	for (int i = 0; i < vector.size(); i++)
	{
		SDL_Surface* use = loadImage(vector[i]);
		return_this_vector.push_back(use);
	}

	return return_this_vector;
}



static void reload_load_load_textures()
{
	GLuint textures[] = {
		OpeningLoad_texture,
		thirtytwo_pxGrid_texture,
		Top_Bar_selector_texture,
		Top_Bar_selector_Highlighter_texture,
		Main_Menu_Texture,
		Create_Map_Menu_Texture,
		Help_page_Texture,
		Create_map_page_error_texture,
		Help_page_Texture,
		Create_map_page_error_texture,
		Load_Map_Ui_Top_Bar_texture,
		Save_And_Exit_texture,
		Texture_Load_Dropdown_texture,
		gallery_Show_texture,
	};
	
	glDeleteTextures(sizeof(textures) / sizeof(textures[0]), textures);

	gallery_Show_texture = createTextureFromSurfaceRGBA8(Layer_Show_img);
	layers_dropdown_after_button_pressed_texture = createTextureFromSurfaceRGBA8(layers_dropdown_after_button_pressed_img);
	layer_galary_texture = createTextureFromSurfaceRGBA8(layer_gallery_img);
	Texture_Load_Dropdown_texture = createTextureFromSurfaceRGBA8(Texture_Load_Dropdown_img);
	Save_And_Exit_texture = createTextureFromSurfaceRGBA8(Save_And_Exit_img);
	File_Load_Dropdown_texture = createTextureFromSurfaceRGBA8(File_Load_Dropdown_img);
	Load_Map_Ui_Top_Bar_texture = createTextureFromSurfaceRGBA8(Load_Map_Ui_Top_Bar_img);
	OpeningLoad_texture = createTextureFromSurfaceRGBA8(OpeningLoad_img);
	thirtytwo_pxGrid_texture = createTextureFromSurfaceRGBA8(thirtytwo_pxGrid);
	Top_Bar_selector_texture = createTextureFromSurfaceRGBA8(Top_Bar_selector);
	Top_Bar_selector_Highlighter_texture = createTextureFromSurfaceRGBA8(Top_Bar_selector_Highlighter);
	Main_Menu_Texture = createTextureFromSurfaceRGBA8(Main_Page_image);
	Create_Map_Menu_Texture = createTextureFromSurfaceRGBA8(Create_Map_Menu_img);
	Help_page_Texture = createTextureFromSurfaceRGBA8(Help_page_img);
	Create_map_page_error_texture = createTextureFromSurfaceRGBA8(Create_map_page_error_img);

	// Unload the memory from said images
	SDL_DestroySurface(Layer_Show_img);
	SDL_DestroySurface(layers_dropdown_after_button_pressed_img);
	SDL_DestroySurface(layer_gallery_img);
	SDL_DestroySurface(Texture_Load_Dropdown_img);
	SDL_DestroySurface(Save_And_Exit_img);
	SDL_DestroySurface(File_Load_Dropdown_img);
	SDL_DestroySurface(Load_Map_Ui_Top_Bar_img);
	SDL_DestroySurface(OpeningLoad_img);
	SDL_DestroySurface(thirtytwo_pxGrid);
	SDL_DestroySurface(Top_Bar_selector);
	SDL_DestroySurface(Top_Bar_selector_Highlighter);
	SDL_DestroySurface(Main_Page_image);
	SDL_DestroySurface(Create_Map_Menu_img);
	SDL_DestroySurface(Help_page_img);
	SDL_DestroySurface(Create_map_page_error_img);

	/*if (tile_surfaces_loaded == true)
	{
		if (texture_vector_loaded.size() >= 0)
		{
			for (int i = 0; i < texture_vector_loaded.size(); i++)
			{
				SDL_DestroyTexture(texture_vector_loaded[i]);
			}
			texture_vector_loaded.clear();
		}


		for (SDL_Texture* tex : texture_vector_loaded) {
			SDL_DestroyTexture(tex);
		}
		texture_vector_loaded.clear();

		std::cout << "surface items: " << tile_surfaces.size() << std::endl;

		for (int i = 0; i < tile_surfaces.size(); i++)
		{
			std::cout << "Tile_Surfaces Size " << tile_surfaces.size() << "    Tile surfaces iteration -> " << tile_surfaces[i] << std::endl;
			SDL_Texture* Unstable_Variable = SDL_CreateTextureFromSurface(renderer, tile_surfaces[i]);
			texture_vector_loaded.push_back(Unstable_Variable);
			std::cout << "Texture Vector Loaded Iteration: " << texture_vector_loaded[i] << std::endl;
		}
	}*/
}





// LOAD MAP FUNCTIONS

std::map<std::string, std::map<std::string, int >> texture_value;
std::map<std::string, GLuint> Texture_Map_1;
bool maximized_window = false;
bool first_game_load = false;
bool update_display = false;



void update_display_()
{
	std::cout << "display change";
	if (maximized_window == true || first_game_load == true || update_display == true)
	{
		if (maximized_window == true || update_display == true)
		{	
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
			glDeleteTextures(4, &key.second);
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

			GLuint texture_loaded_ = createTextureFromSurfaceRGBA8(image_loading_surface_);
			if (!texture_loaded_)
			{
				SDL_Log("Failed to initialize texture_loaded_");
			}

			SDL_DestroySurface(image_loading_surface_);

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
	window_player_center_x = window_width / 2;
	window_player_center_y = window_height / 2;

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
			if (data_map[iii][iiii]["texture_value"] == 555)
			{
				DrawTextureOntoWindow(data_map[iii][iiii]["x_cord"], data_map[iii][iiii]["y_cord"], grid_pixel_size_set, grid_pixel_size_set, thirtytwo_pxGrid_texture);
			}
			else
			{
				std::string iiiii = std::to_string(data_map[iii][iiii]["texture_value"]);

				DrawTextureOntoWindow(data_map[iii][iiii]["x_cord"], data_map[iii][iiii]["y_cord"], grid_pixel_size_set, grid_pixel_size_set, Texture_Map_1[iiiii]);
			}
		}
	}
}

std::unordered_map<std::string, std::unordered_map<std::string, std::unordered_map<std::string, int>>> data_map_create_from_bin(int map_height, int map_width, std::vector<int> bin_data_vector, int tile_size_from_bin)
{
	std::unordered_map<std::string, std::unordered_map<std::string, std::unordered_map<std::string, int>>> data_map_for_and_from_bin;

	int grid_start_x = 0;
	int grid_start_y = 0;
	int iteration_width_map = 0;

	std::unordered_map<std::string, std::unordered_map<std::string, std::unordered_map<std::string, int>>> data_map_from_bin;
	std::unordered_map<std::string, std::unordered_map<std::string, int>> second_level_map_from_bin;
	std::unordered_map<std::string, int> low_level_map_from_bin;


	for (int i = 0; i < map_height; i++)
	{
		std::string ii = std::to_string(i);
		for (int iii = 0; iii < map_width; iii++)
		{
			std::string iiii = std::to_string(iii);
			second_level_map_from_bin.insert({ iiii, low_level_map_from_bin });
		}
		data_map.insert({ ii, second_level_map_from_bin });
	}

	for (int i = 0; i < map_height; i++)
	{
		for (int ii = 0; ii < map_width; ii++)
		{
			std::string iii = std::to_string(i);
			std::string iiii = std::to_string(ii);
			data_map[iii][iiii]["x_cord"] = grid_start_x;
			data_map[iii][iiii]["y_cord"] = grid_start_y;
			data_map[iii][iiii]["texture_value"] = bin_data_vector[ii + iteration_width_map];
			grid_start_x += tile_size_from_bin;
		}
		iteration_width_map += map_width;
		grid_start_x = 0;
		grid_start_y += tile_size_from_bin;
	}
	grid_start_x = 0;
	grid_start_y = 0;


	return data_map_for_and_from_bin;
}








int main(int argc, char* argv[])
{
	if (!init()) return 1;


	// TEXTURE LOADER FOR MAP NOT FOR APPLICATION START****************
	Create_Image_Vector();

	std::map<std::string, GLuint> Texture_Map_1;
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

		GLuint texture_loaded_ = createTextureFromSurfaceRGBA8(image_loading_surface_);
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
	int map_size_width;
	int map_size_height;
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
		glClear(GL_COLOR_BUFFER_BIT);
		glClearColor(1, 1, 1, 1);
		//glColor3f(0, 0, 1); // -> white


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
					// input characters using 0 for the input 0, this is for input box of create map
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
				glColor3f(1, 1, 1); // -> white
				SDL_Color color = { 32, 32, 32 }; // red



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
						if (data_map[iii][iiii]["texture_value"] == default_texture_value)
						{
							DrawTextureOntoWindow(data_map[iii][iiii]["x_cord"], data_map[iii][iiii]["y_cord"], grid_pixel_size_set, grid_pixel_size_set, thirtytwo_pxGrid_texture);
						}
						else
						{
							std::string iiiii = std::to_string(data_map[iii][iiii]["texture_value"]);

							DrawTextureOntoWindow(data_map[iii][iiii]["x_cord"], data_map[iii][iiii]["y_cord"], grid_pixel_size_set, grid_pixel_size_set, Texture_Map_1[iiiii]);
						}
					}
				}


				grid_start_x = 0;
				grid_start_y = 0;

				// ui onto screen
				DrawTextureOntoWindow(0, 0, 1084, 1000, Top_Bar_selector_texture);


				int texture_map_x = 32.0f;
				int texture_map_y = 32.0f;

				for (int i = 0; i < files_to_load_images.size(); i++)
				{
					std::string ii = std::to_string(i);

					// textures on texture map list
					DrawTextureOntoWindow(texture_value[ii]["x_cord"], texture_value[ii]["y_cord"], grid_pixel_size_set, grid_pixel_size_set, Texture_Map_1[ii]);
				}



				// 32 and 18 32 = y = 0 for start     highlight create map gallery.
				if (mouse_x + global_offset_x > 32 + global_offset_x && mouse_x + global_offset_x <= 1055 + global_offset_x)
				{
					if (mouse_y + global_offset_y >= 32 + global_offset_y && mouse_y + global_offset_y <= 222 + global_offset_y)
					{

						DrawTextureOntoWindow(highlighter_x_cords_top_bar[mouse_x / 32], highlighter_y_cords_top_bar[mouse_y / 32], 32, 32, Top_Bar_selector_Highlighter_texture);
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

							int map_size_width = std::stoi(input_map_width_string);
							int map_size_height = std::stoi(input_map_height_string);


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
			// highlighting of the Create Map Texture Gallery, or what I called it back then a "texture map"
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
						DrawTextureOntoWindow(texture_value[ii]["x_cord"], texture_value[ii]["y_cord"], grid_pixel_size_set, grid_pixel_size_set, Top_Bar_selector_Highlighter_texture);
					}
				}
			}



			// SKIP! SKIP! SKIP!
			// SKIP! SKIP! SKIP!
			// SKIP! SKIP! SKIP!
			// SKIP! SKIP! SKIP!
			// SKIP! SKIP! SKIP!
			// SKIP! SKIP! SKIP!
			// SKIP! SKIP! SKIP!



			// if you clicked load map in the main menu this will in fact start ->
			if (load_map == true)
			{
				// event check for movement
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



				// Render layers on the screen
				int center_y_value_number = real_player_cord_y / 32;
				int center_x_value_number = real_player_cord_x / 32;

				// ALL LAYERS NEED TO BE ADDED
				terrain_layer_selected = true;
				object_layer_selected = false;
				obstacle_layer_selected = false;
				top_terrain_layer_selected = false;
				effects_layer_selected = false;
				mouse_button_down = false;

				if (terrain_layer_selected == true)
				{
					// load tjhe terrain layer
				}
				if (object_layer_selected == true)
				{
					// load tjhe object layer
				}
				if (obstacle_layer_selected == true)
				{
					// load tjhe obstacle layer
				}
				if (top_terrain_layer_selected == true)
				{
					// load tjhe top terrain layer
				}
				if (effects_layer_selected == true)
				{
					// load tjhe effects layer
				}






				for (int i = center_y_value_number - render_distance; i < center_y_value_number + render_distance; i++)
				{
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
						if (data_map[iii][iiii]["texture_value"] == default_texture_value)
						{
							DrawTextureOntoWindow(data_map[iii][iiii]["x_cord"] + global_offset_x, data_map[iii][iiii]["y_cord"] + global_offset_y, grid_pixel_size_set, grid_pixel_size_set, thirtytwo_pxGrid_texture);
						}
						else
						{
							std::string iiiii = std::to_string(data_map[iii][iiii]["texture_value"]);

							DrawTextureOntoWindow(data_map[iii][iiii]["x_cord"] + global_offset_x, data_map[iii][iiii]["y_cord"] + global_offset_y, grid_pixel_size_set, grid_pixel_size_set, Texture_Map_1[iiiii]);
						}
					}
				}
				grid_start_x = 0;
				grid_start_y = 0;



				// LOAD OUTSIDE GUI
				// draw the ui interface after load map is clicked
				DrawTextureOntoWindow(0, 0, 2000, 1000, OpeningLoad_texture);
				
				// LOAD TOP BAR GUI
				int top_bar_load_width = 914;
				int top_bar_load_height = 65;



				// Render the map.
				if (load_map_loaded_in_library == true)
				{
					// This is where we render the map and do work with,
				}


				DrawTextureOntoWindow(0, 0, top_bar_load_width, top_bar_load_height, Load_Map_Ui_Top_Bar_texture);
				if (Main_Menu_Pressed == true)
				{
					int savfe_and_exit_x = (window_width / 2) - 300;
					int savfe_and_exit_y = (window_height / 2) - 111;

					DrawTextureOntoWindow((window_width / 2) - 300, (window_height / 2), 600, 170, Save_And_Exit_texture);



					// save an exit button
					if (mouse_x >= 21 + savfe_and_exit_x && mouse_x <= 226 + savfe_and_exit_x && mouse_y >= 111 + savfe_and_exit_y && mouse_y <= 155 + savfe_and_exit_y)
					{
						button_highlight(203, 43, 3, 22 + savfe_and_exit_x, 112 + savfe_and_exit_y, 1);
						load_map_loaded_in_library = false;
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
							load_map_loaded_in_library = false;
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
						int File_Dropdown_width = 300;
						int File_Dropdown_height = 400;

						DrawTextureOntoWindow(0, 0, File_Dropdown_width, File_Dropdown_height, File_Load_Dropdown_texture);
					}



					// clicking file opens these things ->>> THESE ARE BUTTONS FOR THE FILE, settings, open a file, etc.
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
								std::vector<int> Bin_Vector;
								int bin_data;



								std::ifstream inside_file(selected_file_path_name, std::ios::binary);
								if (!inside_file)
								{
									std::cout << "error loading bin file \"inside_file\"" << std::endl;
								}



								// get the first 3 values for the rest of the data we need.
								std::vector<int> get_first_3_values_from_bin;
								for (int i = 0; i <= 2; i++)
								{
									inside_file.read(reinterpret_cast<char*>(&bin_data), sizeof(bin_data));
									get_first_3_values_from_bin.push_back(bin_data);
								}
								map_width_from_bin = get_first_3_values_from_bin[0];
								map_height_from_bin = get_first_3_values_from_bin[1];
								map_gridpixelsize_from_bin = get_first_3_values_from_bin[2];


								// testing only
								for (int i = 0; i <= 2; i++)
								{
									std::cout << get_first_3_values_from_bin[i] << std::endl;
								}



								// we iterate through how many numbers or info are in this bin file,
								//
								// For example, if we had map_width * map_height, whatever that value is, 
								//
								// we then expect to find that many values in our bin file that we can use and put into a vector.
								//
								for (int i = 3; i < (map_width_from_bin * map_height_from_bin) + 3; i++)
								{
									inside_file.read(reinterpret_cast<char*>(&bin_data), sizeof(bin_data));
									Bin_Vector.push_back(bin_data);
								}



								// close the file
								inside_file.close();



								std::unordered_map<std::string, std::unordered_map<std::string, std::unordered_map<std::string, int>>> data_map_from_bin = data_map_create_from_bin(map_height_from_bin, map_width_from_bin, Bin_Vector, map_gridpixelsize_from_bin);


								load_map_loaded_in_library == true;

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
					if (mouse_x >= 108 && mouse_x <= 251 && mouse_y >= 4 && mouse_y <= 54)
					{
						button_highlight(251 - 108, 50, 1, 108, 4, 1);
						if (mouse_button_down == true)
						{
							if (layers_button_clicked == true)
							{
								layers_button_clicked = false;
								mouse_button_down = false;
							}
							else
							{
								layers_button_clicked = true;
								mouse_button_down = false;
							}
						}
					}
					


					// if you clicked Layers button this is true, this is the dropdown menu,
					//
					// this section of code is specific to how this works.
					if (layers_button_clicked == true)
					{
						int layers_dropdown_width = 252;
						int layers_dropdown_height = 269;
						int layers_dropdown_x_cord = 108;
						int layers_dropdown_y_cord = 37;

						DrawTextureOntoWindow(layers_dropdown_x_cord, layers_dropdown_y_cord, layers_dropdown_width, layers_dropdown_height, layers_dropdown_after_button_pressed_texture);


						// Open layers gallery button, if mouse hover over, will highlight, if clicked will make '  layer_gallery_show = true  '
						if (mouse_x >= layers_dropdown_x_cord + 7 && mouse_x <= layers_dropdown_x_cord + 219 && mouse_y >= 40 + layers_dropdown_y_cord && mouse_y <= layers_dropdown_y_cord + 76)
						{
							button_highlight(212, 36, 1, 7 + layers_dropdown_x_cord, 40 + layers_dropdown_y_cord);
							if (mouse_button_down == true)
							{
								if (layer_gallery_show == true)
								{
									layer_gallery_show = false;
									mouse_button_down = false;
								}
								else
								{
									layer_gallery_show = true;
									mouse_button_down = false;
								}
							}
						}



						// add a layer button, if mouse hover over, will highlight, if clicked will make '  layer_gallery_show = true  '
						if (mouse_x >= layers_dropdown_x_cord + 7 && mouse_x <= layers_dropdown_x_cord + 219 && mouse_y >= 79 + layers_dropdown_y_cord && mouse_y <= layers_dropdown_y_cord + 115)
						{
							button_highlight(212, 36, 1, 7 + layers_dropdown_x_cord, 79 + layers_dropdown_y_cord);
							if (mouse_button_down == true)
							{
								mouse_button_down = false;
							}
						}



						// delete a layer button, if mouse hover over, will highlight, if clicked will make '  layer_gallery_show = true  '
						if (mouse_x >= layers_dropdown_x_cord + 7 && mouse_x <= layers_dropdown_x_cord + 219 && mouse_y >= 118 + layers_dropdown_y_cord && mouse_y <= layers_dropdown_y_cord + 154)
						{
							button_highlight(212, 36, 1, 7 + layers_dropdown_x_cord, 118 + layers_dropdown_y_cord);
							if (mouse_button_down == true)
							{
								mouse_button_down = false;
							}
						}



						// empty button, if mouse hover over, will highlight, if clicked will make '  layer_gallery_show = true  '
						if (mouse_x >= layers_dropdown_x_cord + 7 && mouse_x <= layers_dropdown_x_cord + 219 && mouse_y >= 157 + layers_dropdown_y_cord && mouse_y <= layers_dropdown_y_cord + 193)
						{
							button_highlight(212, 36, 1, 7 + layers_dropdown_x_cord, 157 + layers_dropdown_y_cord);
							if (mouse_button_down == true)
							{
								mouse_button_down = false;
							}
						}



						// layer settings button, if mouse hover over, will highlight, if clicked will make '  layer_gallery_show = true  '
						if (mouse_x >= layers_dropdown_x_cord + 7 && mouse_x <= layers_dropdown_x_cord + 219 && mouse_y >= 196 + layers_dropdown_y_cord && mouse_y <= layers_dropdown_y_cord + 232)
						{
							button_highlight(212, 36, 1, 7 + layers_dropdown_x_cord, 196 + layers_dropdown_y_cord);
							if (mouse_button_down == true)
							{
								mouse_button_down = false;
							}
						}

					}
					

					
					// This deals with the layers_gallery
					if (layer_gallery_show == true)
					{
						int layers_gallery_width = 200;
						int layers_gallery_height = 400;
						int layers_gallery_x_start = window_width - layers_gallery_width;
						int layers_gallery_y_start = 0;
						int layers_gallery_button_height = 36;
						int layers_gallery_button_width = 187;
						
						DrawTextureOntoWindow(window_width - layers_gallery_width, layers_gallery_y_start, layers_gallery_width, layers_gallery_height, layer_galary_texture);


						// Then check the buttons etc. with highlighting.
						// 
						// Terrain layer button
						if (mouse_x >= window_width + 6 - layers_gallery_width && mouse_x <= window_width + 193 - layers_gallery_width && mouse_y >= layers_gallery_y_start + 56 && mouse_y <= layers_gallery_y_start + 91)
						{
							button_highlight(layers_gallery_button_width, layers_gallery_button_height, 1, 7 + layers_gallery_x_start, layers_gallery_y_start + 56);
							if (mouse_button_down == true)
							{
								terrain_layer_selected = true;
								object_layer_selected = false;
								obstacle_layer_selected = false;
								top_terrain_layer_selected = false;
								effects_layer_selected = false;
								mouse_button_down = false;
							}
						}



						// Object layer button
						if (mouse_x >= window_width + 6 - layers_gallery_width && mouse_x <= window_width + 193 - layers_gallery_width && mouse_y >= layers_gallery_y_start + 95 && mouse_y <= layers_gallery_y_start + 130)
						{
							button_highlight(layers_gallery_button_width, layers_gallery_button_height, 1, 7 + layers_gallery_x_start, layers_gallery_y_start + 95);
							if (mouse_button_down == true)
							{
								terrain_layer_selected = false;
								object_layer_selected = true;
								obstacle_layer_selected = false;
								top_terrain_layer_selected = false;
								effects_layer_selected = false;
								mouse_button_down = false;
							}
						}



						// Obstacle layer button
						if (mouse_x >= window_width + 6 - layers_gallery_width && mouse_x <= window_width + 193 - layers_gallery_width && mouse_y >= layers_gallery_y_start + 134 && mouse_y <= layers_gallery_y_start + 169)
						{
							button_highlight(layers_gallery_button_width, layers_gallery_button_height, 1, 7 + layers_gallery_x_start, layers_gallery_y_start + 134);
							if (mouse_button_down == true)
							{
								terrain_layer_selected = false;
								object_layer_selected = false;
								obstacle_layer_selected = true;
								top_terrain_layer_selected = false;
								effects_layer_selected = false;
								mouse_button_down = false;
							}
						}



						// Top Terrain layer button
						if (mouse_x >= window_width + 6 - layers_gallery_width && mouse_x <= window_width + 193 - layers_gallery_width && mouse_y >= layers_gallery_y_start + 173 && mouse_y <= layers_gallery_y_start + 208)
						{
							button_highlight(layers_gallery_button_width, layers_gallery_button_height, 1, 7 + layers_gallery_x_start, layers_gallery_y_start + 173);
							if (mouse_button_down == true)
							{
								terrain_layer_selected = false;
								object_layer_selected = false;
								obstacle_layer_selected = false;
								top_terrain_layer_selected = true;
								effects_layer_selected = false;
								mouse_button_down = false;
							}
						}



						// Effects layer button
						if (mouse_x >= window_width + 6 - layers_gallery_width && mouse_x <= window_width + 193 - layers_gallery_width && mouse_y >= layers_gallery_y_start + 212 && mouse_y <= layers_gallery_y_start + 247)
						{
							button_highlight(layers_gallery_button_width, layers_gallery_button_height, 1, 7 + layers_gallery_x_start, layers_gallery_y_start + 212);
							if (mouse_button_down == true)
							{
								terrain_layer_selected = false;
								object_layer_selected = false;
								obstacle_layer_selected = false;
								top_terrain_layer_selected = false;
								effects_layer_selected = true;
								mouse_button_down = false;
							}
						}
					}



					// Textures button
					if (mouse_x >= 489 && mouse_x <= 634 && mouse_y >= 4 && mouse_y <= 54)
					{
						button_highlight(145, 50, 1, 489, 4, 1);
						if (mouse_button_down == true)
						{
							if (Texture_Load__Dropdown == true)
							{
								Texture_Load__Dropdown = false;
								mouse_button_down = false;
							}
							else
							{
								Texture_Load__Dropdown = true;

								mouse_button_down = false;
							}
						}
					}
					else
					{
						if (mouse_button_down == true && Texture_Load__Dropdown == true)
						{
							if (mouse_x >= 489 && mouse_x <= 634 && mouse_y >= 72 && mouse_y <= 354)
							{



							}
							else
							{
								Texture_Load__Dropdown = false;
								mouse_button_down = false;
							}
						}
					}
					// if texture button clicked, this will play,
					if (Texture_Load__Dropdown == true)
					{
						int Texture_Dropdown_width = 300;
						int Texture_Dropdown_height = 400;

						DrawTextureOntoWindow(489, 0, Texture_Dropdown_width, Texture_Dropdown_height, Texture_Load_Dropdown_texture);
					}



					// now if texture button is clicked this will also play.  THESE ARE THE BUTTONS SECTION OF CODE for if texture is clicked
					if (mouse_x >= 500 && mouse_x <= 724 && mouse_y >= 72 && mouse_y <= 354 && Texture_Load__Dropdown == true)
					{
						// first bar
						//
						// This bar is for enabling the texture panel ui.
						if (mouse_x >= 500 && mouse_x <= 724 && mouse_y >= 78 && mouse_y <= 114)
						{
							button_highlight(212, 37, 3, 500, 78, 1);

							if (mouse_button_down == true)
							{
								if (textures_bar_right_load_show == true)
								{
									textures_bar_right_load_show = false;
									mouse_button_down = false;
								}
								else
								{
									textures_bar_right_load_show = true;
									mouse_button_down = false;
								}
							}
						}
						// second bar
						//
						// this bar or button is to open a texture folder, once clicked it will populate the tile picker, once 
						if (mouse_x >= 500 && mouse_x <= 724 && mouse_y >= 117 && mouse_y <= 153)
						{
							button_highlight(212, 37, 1, 500, 117, 1);
							if (mouse_button_down == true)
							{
								mouse_button_down = false;

								// This opens up the folder dialog and looks to find a folder to select before closing saving its path of the folder.
								std::string Folder_Path_To_Load_Textures_For_Tile_Pallete = OpenFolderDialog();
								// This will look through folder and save all the textures associated to that folder.
								std::vector<std::string> tilePaths;
								for (const auto& entry : std::filesystem::directory_iterator(Folder_Path_To_Load_Textures_For_Tile_Pallete)) {
									if (entry.is_regular_file()) {
										auto path = entry.path().string();
										if (path.ends_with(".png") || path.ends_with(".bmp")) {
											tilePaths.push_back(path);
										}
									}
								}
								// Here we load it onto a vector, and then we get the images and then we texturize them.
								tile_surfaces = Load_Images_From_A_Vector(tilePaths);
								tile_surfaces_loaded = true;
								// here we will update the textures to get these new textures into any new display changes
								reload_load_load_textures();
								load_textures_onto_data_gallery = true;
								textures_loaded_into_gallery = false;
							}

						}
						// third bar  -- > settings
						//
						// This bar or button is for settings or modyfying the textures.
						if (mouse_x >= 500 && mouse_x <= 724 && mouse_y >= 156 && mouse_y <= 192)
						{
							button_highlight(212, 37, 1, 500, 156, 1);
						}
						// fourth bar
						if (mouse_x >= 500 && mouse_x <= 724 && mouse_y >= 195 && mouse_y <= 231)
						{
							button_highlight(212, 37, 1, 500, 195, 1);
						}
						// fifth bar
						if (mouse_x >= 500 && mouse_x <= 724 && mouse_y >= 234 && mouse_y <= 270)
						{
							button_highlight(212, 37, 1, 500, 234, 1);
						}
					}



					// Textures Ui stuff
					if (textures_bar_right_load_show == true)
					{
						int gallery_ui_width = 200;
						int gallery_ui_height = 400;
						int texture_gallery_x_cord = (window_width - gallery_ui_width);
						int texture_gallery_y_cord = 400;

						DrawTextureOntoWindow(texture_gallery_x_cord, texture_gallery_y_cord, gallery_ui_width, gallery_ui_height, gallery_Show_texture);



						// take the sdl_texture data and put it into a map,
						if (load_textures_onto_data_gallery == true)
						{
							int x_start_gallery = 0;
							int y_start_gallery = 0;
							for (int i = 0; i < texture_vector_loaded.size(); i++)
							{
								std::string texture_id_variable_gallery = std::to_string(i);
								std::string x_start_gallery_string = std::to_string(x_start_gallery);
								std::string y_start_gallery_string = std::to_string(y_start_gallery);
								gallery_data_map_for_render[texture_id_variable_gallery]["x_value"] = x_start_gallery_string;
								gallery_data_map_for_render[texture_id_variable_gallery]["y_value"] = y_start_gallery_string;
								x_start_gallery += 32;
								//gallery_data_map.insert({ x_start_gallery_string, { y_start_gallery_string, texture_id_variable_gallery } });
							}
							// make it false immediately so we dont have issues laters.
							load_textures_onto_data_gallery = false;
							textures_loaded_into_gallery = true;
						}




						// This renders the textures onto the gallery
						if (textures_loaded_into_gallery == true)
						{
							for (int i = 0; i < texture_vector_loaded.size(); i++)
							{
								std::string texture_id = std::to_string(i);
								int texture_gallery_x_cord_of_texture = std::stoi(gallery_data_map_for_render[texture_id]["x_value"]);
								int texture_gallery_y_cord_of_texture = std::stoi(gallery_data_map_for_render[texture_id]["y_value"]);

								DrawTextureOntoWindow(((window_width - 400) - 25) + texture_gallery_x_cord_of_texture, 36 + texture_gallery_y_cord_of_texture, map_gridpixelsize_from_bin, map_gridpixelsize_from_bin, texture_vector_loaded[i]);

							}
						}



						// This will allow the cursor to highlight over the gallery textures, this will also enable us to select a texture.
						//
						// 
						// This fucking sucks. New idea: just create a texture atlas but for a gallery, and then
						// have it to where we can just click and highlight over any of the squares,'
						// this will be a little ugly but im sick and tired of doing this.
						//
						if (mouse_x >= texture_gallery_x_cord && mouse_x <= texture_gallery_x_cord + gallery_ui_width && mouse_y >= texture_gallery_y_cord && mouse_y <= texture_gallery_y_cord + gallery_ui_height)
						{
							// Init variables
							SDL_FRect Highlighting_Texture_in_gallery_SET;
							for (int i = 0; texture_vector_loaded.size(); i++)
							{
								std::string texture_id = std::to_string(i);
								std::string x_value_string_set = gallery_data_map_for_render[texture_id]["x_value"];
								std::string y_value_string_set = gallery_data_map_for_render[texture_id]["y_value"];
								if (!x_value_string_set.empty() && !y_value_string_set.empty()) {
									int x_cord = std::stoi(x_value_string_set);
									int y_cord = std::stoi(y_value_string_set);
									// Proceed...
								}
								else {
									std::cout << "Empty x_value or y_value for texture_id: " << texture_id << std::endl;
								}
								int x_cord = std::stoi(x_value_string_set);
								int y_cord = std::stoi(y_value_string_set);
								float texture_screen_x = ((window_width - 400.0f) - 25) + x_cord * 36.0f;
								float texture_screen_y = y_cord * map_gridpixelsize_from_bin;

								if (mouse_x >= texture_screen_x && mouse_x < texture_screen_x + 32 &&
									mouse_y >= texture_screen_y && mouse_y < texture_screen_y + 32)
								{
									Highlighting_Texture_in_gallery_SET = {
										texture_screen_x, texture_screen_y, 32, 32
									};
								}
							}
							//SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
							//SDL_RenderRect(renderer, &Highlighting_Texture_in_gallery_SET);
						}
					}
					mouse_button_down = false;
				}
			}
		}





		else // this is where the main menu stuff starts
		{
			// MAIN MENU ui
			DrawTextureOntoWindow(0, 0, 1084, 1000, Main_Menu_Texture);

			// CREATE MAP MENU
			if (Create_Map_Menu == true)
			{
				main_menu_button_offline = true;


				// Creeate Map Menu render in
				DrawTextureOntoWindow(271, 250, 542, 500, Create_Map_Menu_Texture);

				// create map menu render out


				// text renders
				// text font settings
				SDL_Color Input_Map_Text_Color = { 240,240,240 }; // color of font
				int font_width_size_input_create_map_menu = 13;
				int font_height_size_input_create_map_menu = 30;



				// width input text render --
				int size_of_width_map_input = input_map_width_string.size();

				SDL_Surface* textsurface_width_create_menu = TTF_RenderText_Solid(font, input_map_width_string.c_str(), size_of_width_map_input, Input_Map_Text_Color);

				GLuint textTexture_width_Create_menu = createTextureFromSurfaceRGBA8(textsurface_width_create_menu);

				DrawTextureOntoWindow(Map_Width_Create_Page[0] + 273, Map_Width_Create_Page[1] + 250, size_of_width_map_input* font_width_size_input_create_map_menu, font_height_size_input_create_map_menu, textTexture_width_Create_menu);
				// end width input render



				// height input text render --
				int size_of_height_map_input = input_map_height_string.size();

				SDL_Surface* textsurface_height_create_menu = TTF_RenderText_Solid(font, input_map_height_string.c_str(), size_of_height_map_input, Input_Map_Text_Color);

				GLuint textTexture_height_Create_menu = createTextureFromSurfaceRGBA8(textsurface_height_create_menu);

				DrawTextureOntoWindow(Map_Width_Create_Page[0] + 273, Map_Width_Create_Page[1] + 333, size_of_height_map_input* font_width_size_input_create_map_menu, font_height_size_input_create_map_menu, textTexture_height_Create_menu);
				// end height render



				// pixel size text render
				int pixel_size_input = input_map_pixel_size_string.size();

				SDL_Surface* textsurface_pixel_size_create_menu = TTF_RenderText_Solid(font, input_map_pixel_size_string.c_str(), pixel_size_input, Input_Map_Text_Color);

				GLuint textTexture_pixel_size_Create_menu = createTextureFromSurfaceRGBA8(textsurface_pixel_size_create_menu);

				DrawTextureOntoWindow(Map_Width_Create_Page[0] + 273, Map_Width_Create_Page[1] + 412, pixel_size_input* font_width_size_input_create_map_menu, font_height_size_input_create_map_menu, textTexture_pixel_size_Create_menu);
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
					DrawTextureOntoWindow(271, 250, 542, 500, Create_map_page_error_texture);
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

				DrawTextureOntoWindow(271, 250, 542, 500, Help_page_Texture);

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
		SDL_GL_SwapWindow(window);

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





//// ————————————————————————————————————————————
//// 1) Make sure you have an orthographic projection
////    that maps your window pixels 1:1.
////    e.g. using glm:
////      projection = glm::ortho(0.0f, winW, winH, 0.0f);
////    upload it once to your shader as “projection”.
//// ————————————————————————————————————————————
//// ————————————————————————————————————————————
//// 2) Create a VAO/VBO for a unit quad (0,0→1,1) + UVs
////    Do this ONCE in your init code:
//float quadVerts[] = {
//	// x,    y,    u,   v
//	 0.0f, 1.0f, 0.0f, 1.0f,
//	 1.0f, 0.0f, 1.0f, 0.0f,
//	 0.0f, 0.0f, 0.0f, 0.0f,
//
//	 0.0f, 1.0f, 0.0f, 1.0f,
//	 1.0f, 1.0f, 1.0f, 1.0f,
//	 1.0f, 0.0f, 1.0f, 0.0f,
//};
//GLuint quadVAO, quadVBO;
//glGenVertexArrays(1, &quadVAO);
//glGenBuffers(1, &quadVBO);
//glBindVertexArray(quadVAO);
//glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
//glBufferData(GL_ARRAY_BUFFER, sizeof(quadVerts), quadVerts, GL_STATIC_DRAW);
//// position
//glEnableVertexAttribArray(0);
//glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
//// uv
//glEnableVertexAttribArray(1);
//glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
//glBindBuffer(GL_ARRAY_BUFFER, 0);
//glBindVertexArray(0);
//// ————————————————————————————————————————————
//// ————————————————————————————————————————————
//// 3) In your render loop, instead of SDL_RenderTexture(...):
//// ————————————————————————————————————————————
//if (help_page) {
//	// compute screen-space model matrix:
//	float x = 271, y = 250, w = 542, h = 500;
//	glm::mat4 model = glm::translate(glm::mat4(1.0f), { x, y, 0.0f })
//		* glm::scale({ w, h, 0.0f });
//	// bind shader + uniforms:
//	shader.use();
//	shader.setMat4("projection", projection);
//	shader.setMat4("model", model);
//	shader.setInt("tex", 0);
//	// bind geometry + texture:
//	glActiveTexture(GL_TEXTURE0);
//	glBindTexture(GL_TEXTURE_2D, Help_page_Texture);
//	glBindVertexArray(quadVAO);
//	// draw:
//	glDrawArrays(GL_TRIANGLES, 0, 6);
//	// cleanup bind (optional):
//	glBindVertexArray(0);
//}
//// …after all your draws:
//SDL_GL_SwapWindow(window);