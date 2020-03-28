#include <iostream>
// clam av includes
#include "openssl/opensslv.h"
#include "clamav.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <SFML/OpenGL.hpp>
#include <SFML/Window.hpp>
#include <ShlObj.h>
#include <assert.h>
#include <atlstr.h>
#include <conio.h>
#include <dirent.h>
#include <dlgs.h>
#include <fcntl.h>
#include <fstream>
#include <io.h>
#include <limits.h>
#include <math.h>
#include <regex>
#include <set>
#include <sstream>
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
#include <vector>
#include <windows.h>
#include "LavaScan.h"
// nuklear includes
#define NK_INCLUDE_FIXED_TYPES
#define NK_INCLUDE_STANDARD_IO
#define NK_INCLUDE_STANDARD_VARARGS
#define NK_INCLUDE_DEFAULT_ALLOCATOR
#define NK_INCLUDE_VERTEX_BUFFER_OUTPUT
#define NK_INCLUDE_FONT_BAKING
#define NK_INCLUDE_DEFAULT_FONT
#define NK_IMPLEMENTATION
#define NK_SFML_GL2_IMPLEMENTATION

#include "nuklear.h"
#include "nuklear_sfml_gl2.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define WINDOW_WIDTH 1200
#define WINDOW_HEIGHT 800
#define IMG_COLOR nk_rgba(255, 255, 255, 255)

#include "FE.h"

//struct nk_command_buffer* canvas;
//struct nk_rect total_space;

int main()
{
	std::cout << "Hello World!\n";
	
	FE FrontEnd = FE();

	/* Platform */
	sf::ContextSettings settings(24, 8, 4, 2, 2);
	sf::Window win(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "LAVA", sf::Style::Close, settings);
	win.setVerticalSyncEnabled(true);
	win.setActive(true);
	glViewport(0, 0, win.getSize().x, win.getSize().y);
	FrontEnd.init(&win);

	while (win.isOpen())
	{
		/* Input */
		sf::Event evt;
		nk_input_begin(FrontEnd.ctx);
		while (win.pollEvent(evt)) {
			if (evt.type == sf::Event::Closed)
				win.close();
			nk_sfml_handle_event(&evt);
		}
		nk_input_end(FrontEnd.ctx);

		/* GUI */
		FrontEnd.Display();

		/* Draw */
		win.setActive(true);
		glClear(GL_COLOR_BUFFER_BIT);
		glClearColor(FrontEnd.bg.r, FrontEnd.bg.g, FrontEnd.bg.b, FrontEnd.bg.a);
		/* IMPORTANT: `nk_sfml_render` modifies some global OpenGL state
		* with blending, scissor, face culling and depth test and defaults everything
		* back into a default state. Make sure to either save and restore or
		* reset your own state after drawing rendering the UI. */
		nk_sfml_render(NK_ANTI_ALIASING_ON);
		win.display();
	}
	nk_sfml_shutdown();
	return 0;

}
