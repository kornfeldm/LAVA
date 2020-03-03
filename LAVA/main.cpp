// ConsoleApplication1.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include "openssl/opensslv.h"
#include "clamav.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdarg.h>
#include <string.h>
#include <math.h>
#include <assert.h>
#include <math.h>
#include <limits.h>
#include <time.h>

#include "GL/glew.h"
//#include <GLFW/glfw3.h>

#include <SFML/OpenGL.hpp>
#include <SFML/Window.hpp>
#include <windows.h>
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

static struct nk_image
icon_load(const char* filename)
{
	int x, y, n;
	GLuint tex;
	unsigned char* data = stbi_load(filename, &x, &y, &n, 0);
	if (!data) printf("[SDL]: failed to load image: %s", filename);

	glGenTextures(1, &tex);
	glBindTexture(GL_TEXTURE_2D, tex);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, x, y, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
	glGenerateMipmap(GL_TEXTURE_2D);
	stbi_image_free(data);
	return nk_image_id((int)tex);
}


int main()
{
    std::cout << "Hello World!\n";

    int fd, ret;
    unsigned long int size = 0;
    unsigned int sigs = 0;
    long double mb;
    const char* virname;
    struct cl_engine* engine;

    if ((ret = cl_init(CL_INIT_DEFAULT)) != CL_SUCCESS) {
        printf("Can't initialize libclamav: %s\n", cl_strerror(ret));
        return 2;
    }
    else {
        printf("initialization successful");
    }


	/* platform */
	sf::ContextSettings settings(24,8,4,2,2);
	sf::Window win(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "LAVA", sf::Style::Default, settings);
	win.setVerticalSyncEnabled(true);
	win.setActive(true);
	glViewport(0, 0, win.getSize().x, win.getSize().y);

	/* GUI */
	struct nk_context* ctx;
	ctx = nk_sfml_init(&win);
	// load font here if want
	// load cursor here if want
	struct nk_font_atlas* atlas;
	nk_sfml_font_stash_begin(&atlas);
	nk_sfml_font_stash_end();
	// img shit
	//struct nk_rect rect;
	//struct nk_image* logos;
	struct nk_command_buffer* out = nk_window_get_canvas(ctx);

	// bg color
	struct nk_colorf bg;
	bg.r = .212; bg.g = .224f; bg.b = .243f; bg.a = 1.0f;
	while ( win.isOpen() ) {
		/* INPUT */
		sf::Event e;
		nk_input_begin(ctx);
		while (win.pollEvent(e)) {
			if (e.type == sf::Event::Closed)
				win.close();
			else if (e.type == sf::Event::Resized)
				glViewport(0,0, e.size.width, e.size.height);
			nk_sfml_handle_event(&e);
		}
		nk_input_end(ctx);

		/* GUI */
		//nk_draw_image(out, rect, &logos[1], nk_rgb(255, 255, 255));

		/* Draw */
		win.setActive(true);
		glClear(GL_COLOR_BUFFER_BIT);
		glClearColor(bg.r,bg.g,bg.b,bg.a);
		// save and retore OR reset state after drawing rendering the UI
		nk_sfml_render(NK_ANTI_ALIASING_ON);
		win.display();
	}
	nk_sfml_shutdown();
	return 0;
}
