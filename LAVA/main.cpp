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
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <SFML/OpenGL.hpp>
#include <SFML/Window.hpp>

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

static struct nk_image
icon_load(const char* filename, bool flip = false) {
	int x, y, n;
	GLuint tex;
	if (flip) {
		stbi_set_flip_vertically_on_load(true);
	}
	unsigned char* data = stbi_load(filename, &x, &y, &n, STBI_rgb_alpha);
	if (!data) std::cerr << "[SDL]: failed to load image\n";
	glGenTextures(1, &tex);
	glBindTexture(GL_TEXTURE_2D, tex);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, x, y, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
	glGenerateMipmap(GL_TEXTURE_2D);
	stbi_image_free(data);
	return nk_image_id((int)tex);
}

struct nk_command_buffer* canvas;

const unsigned int WIDTH_IMG = 80;
const unsigned int HEIGHT_IMG = 80;

unsigned char imageData[WIDTH_IMG * HEIGHT_IMG * 4];

struct nk_rect total_space;

struct pics {
	const char* squareLogo;
	const char* rectLogo;
	const char* trapLogo;
	const char* scan;
};

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

	/* INIT IMAGES */
	pics pp;
	struct nk_image scanImage;  pp.scan = "../Assets/scan2.png";
	struct nk_image rectImage; pp.rectLogo = "../Assets/rectLogo.png";
	struct nk_image trapImage; pp.trapLogo = "../Assets/trapLogo.png";
	struct nk_image squareImage; pp.squareLogo = "../Assets/squareLogo.png";

	/* Platform */
	sf::ContextSettings settings(24, 8, 4, 2, 2);
	sf::Window win(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "LAVA", sf::Style::Default, settings);
	win.setVerticalSyncEnabled(true);
	win.setActive(true);
	glViewport(0, 0, win.getSize().x, win.getSize().y);

	glewExperimental = 1;
	if (glewInit() != GLEW_OK) {
		fprintf(stderr, "Failed to setup GLEW\n");
		std::cout << glewGetErrorString(glewInit());
		exit(1);
	}

	/* GUI */
	struct nk_context* ctx;
	ctx = nk_sfml_init(&win);
	// load font here if want
	// load cursor here if want
	struct nk_font_atlas* atlas;
	nk_sfml_font_stash_begin(&atlas);
	nk_sfml_font_stash_end();

	// load some image stuff
	struct nk_buffer cmds;
	nk_buffer_init_default(&cmds);
	glEnable(GL_TEXTURE_2D);


	// bg color
	struct nk_colorf bg;
	bg.r = 45 / 255.0f; bg.g = 45 / 255.0f; bg.b = 45 / 255.0f; bg.a = 1.0f;


	scanImage = icon_load(pp.scan);
	squareImage = icon_load(pp.squareLogo);
	while (win.isOpen())
	{
		/* Input */
		sf::Event evt;
		nk_input_begin(ctx);
		while (win.pollEvent(evt)) {
			if (evt.type == sf::Event::Closed)
				win.close();
			else if (evt.type == sf::Event::Resized)
				glViewport(0, 0, evt.size.width, evt.size.height);
			nk_sfml_handle_event(&evt);
		}
		nk_input_end(ctx);

		/* GUI */
		/*glEnable(GL_TEXTURE_2D);
		if (nk_begin(ctx, "LAVA", nk_rect(500, 500, 230, 250),
			NK_WINDOW_BORDER | NK_WINDOW_MOVABLE | NK_WINDOW_SCALABLE |
			NK_WINDOW_MINIMIZABLE | NK_WINDOW_TITLE))
		{
			enum { EASY, HARD };
			static int op = EASY;
			static int property = 20;
			nk_layout_row_static(ctx, 30, 80, 1);
			if (nk_button_label(ctx, "button"))
				fprintf(stdout, "button pressed\n");

			nk_layout_row_dynamic(ctx, 30, 2);
			if (nk_option_label(ctx, "easy", op == EASY)) op = EASY;
			if (nk_option_label(ctx, "hard", op == HARD)) op = HARD;

			nk_layout_row_dynamic(ctx, 25, 1);
			nk_property_int(ctx, "Compression:", 0, &property, 100, 10, 1);

			nk_layout_row_dynamic(ctx, 20, 1);
			nk_label(ctx, "background:", NK_TEXT_LEFT);
			nk_layout_row_dynamic(ctx, 25, 1);
			if (nk_combo_begin_color(ctx, nk_rgb_cf(bg), nk_vec2(nk_widget_width(ctx), 400))) {
				nk_layout_row_dynamic(ctx, 120, 1);
				bg = nk_color_picker(ctx, bg, NK_RGBA);
				nk_layout_row_dynamic(ctx, 25, 1);
				bg.r = nk_propertyf(ctx, "#R:", 0, bg.r, 1.0f, 0.01f, 0.005f);
				bg.g = nk_propertyf(ctx, "#G:", 0, bg.g, 1.0f, 0.01f, 0.005f);
				bg.b = nk_propertyf(ctx, "#B:", 0, bg.b, 1.0f, 0.01f, 0.005f);
				bg.a = nk_propertyf(ctx, "#A:", 0, bg.a, 1.0f, 0.01f, 0.005f);
				nk_combo_end(ctx);
			}
		}
		nk_end(ctx);
		*/


		/* LOGO */
		if (nk_begin(ctx, "lavalogo", nk_rect(25, 25, WINDOW_HEIGHT * .33, WINDOW_HEIGHT * .33),
			NULL))
		{
			canvas = nk_window_get_canvas(ctx);
			total_space = nk_window_get_content_region(ctx);


			nk_draw_image(canvas, total_space, &squareImage, IMG_COLOR);
		}
		nk_end(ctx);

		/* SCAN ICON */
		if (nk_begin(ctx, "scan", nk_rect(WINDOW_WIDTH * .2, WINDOW_HEIGHT * .49, WINDOW_HEIGHT * .3, WINDOW_HEIGHT * .3),
			NULL))
		{
			canvas = nk_window_get_canvas(ctx);
			total_space = nk_window_get_content_region(ctx);

			//myImage = p.scan;
			nk_draw_image(canvas, total_space, &scanImage, IMG_COLOR);
		}
		nk_end(ctx);

		/* Draw */
		win.setActive(true);
		glClear(GL_COLOR_BUFFER_BIT);
		glClearColor(bg.r, bg.g, bg.b, bg.a);
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