#pragma once
#ifndef FE_H
#define FE_H

//#include <iostream>
//#include "openssl/opensslv.h"
//#include "clamav.h"
//#include <stdio.h>
//#include <stdlib.h>
//#include <stdint.h>
//#include <stdarg.h>
//#include <string.h>
//#include <math.h>
//#include <assert.h>
//#include <math.h>
//#include <limits.h>
//#include <time.h>
//#include <GL/glew.h>
//#include <GLFW/glfw3.h>
//#include <SFML/OpenGL.hpp>
//#include <SFML/Window.hpp>
//
//#define NK_INCLUDE_FIXED_TYPES
//#define NK_INCLUDE_STANDARD_IO
//#define NK_INCLUDE_STANDARD_VARARGS
//#define NK_INCLUDE_DEFAULT_ALLOCATOR
//#define NK_INCLUDE_VERTEX_BUFFER_OUTPUT
//#define NK_INCLUDE_FONT_BAKING
//#define NK_INCLUDE_DEFAULT_FONT
//#define NK_IMPLEMENTATION
//#define NK_SFML_GL2_IMPLEMENTATION
//
//#include "nuklear.h"
//#include "nuklear_sfml_gl2.h"
//
//#define STB_IMAGE_IMPLEMENTATION
//#include "stb_image.h"
//
//#define WINDOW_WIDTH 1200
//#define WINDOW_HEIGHT 800
//#define IMG_COLOR nk_rgba(255, 255, 255, 255)

struct pics {
	const char* squareLogo;
	const char* rectLogo;
	const char* trapLogo;
	const char* scan;
	const char* history;
};
struct nk_command_buffer* canvas;
struct nk_rect total_space;

class FE
{
private:
public:
	/* CONSTRUCTORS */
	FE(sf::Window *win);

	/* MEMBER VARS */
	struct nk_context* ctx;
	struct nk_font_atlas* atlas;
	struct nk_command_buffer* canvas;
	struct nk_colorf bg;
	struct nk_rect total_space;
	pics pp;
	struct nk_image scanImage;
	struct nk_image rectImage; 
	struct nk_image trapImage; 
	struct nk_image squareImage; 
	struct nk_image historyImage;
	unsigned int view;
	/*
		0 : logo screen
		1 : scans screen
		2 : history screen
		3 : 
		4 :
		5 : 
		6 :
	*/

	/* MEMBER FUNCTIONS */
	static struct nk_image icon_load(const char* filename, bool flip = false);
	bool drawImage(struct nk_image *img);
	bool DrawMainPage();
	bool DrawScansPage();
	bool Display();
};
inline struct nk_image FE::icon_load(const char* filename, bool flip)
{
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

inline bool FE::drawImage(struct nk_image *img)
{
	canvas = nk_window_get_canvas(this->ctx);
	total_space = nk_window_get_content_region(this->ctx);
	nk_draw_image(canvas, total_space, img, IMG_COLOR);
	return true;
}

inline bool FE::Display() {
	if (this->view == 0)
		this->DrawMainPage();
	else if (this->view == 1)
		this->DrawScansPage();

	else {
		std::cout << "ERRRRRRORRRRR\n";
	}
	return true;
}

inline bool FE::DrawMainPage()
{
	/* LOGO */
	if (nk_begin(this->ctx, "lavalogo", nk_rect(25, 25, WINDOW_HEIGHT * .33, WINDOW_HEIGHT * .33),
		NK_WINDOW_NO_SCROLLBAR)) {
		this->drawImage(&this->squareImage);
	}
	nk_end(this->ctx);

	/* SCAN ICON */
	if (nk_begin(this->ctx, "scan", nk_rect(WINDOW_WIDTH * .2, WINDOW_HEIGHT * .49, WINDOW_HEIGHT * .3, WINDOW_HEIGHT * .3),
		NK_WINDOW_NO_SCROLLBAR)) {
		// hidden button to press behind icon
		nk_layout_row_static(ctx, WINDOW_HEIGHT * .3, WINDOW_HEIGHT * .3, 1);
		if (nk_button_label(ctx, "")) {
			fprintf(stdout, "scans pressed\n");
			this->view = 1;
			nk_clear(this->ctx);
		}
			
		this->drawImage(&this->scanImage);
	}
	nk_end(this->ctx);

	/* history ICON */
	if (nk_begin(this->ctx, "history", nk_rect(WINDOW_WIDTH * .6, WINDOW_HEIGHT * .49, WINDOW_HEIGHT * .3, WINDOW_HEIGHT * .3),
		NK_WINDOW_NO_SCROLLBAR)) {
		// hidden button behind icon to press
		nk_layout_row_static(ctx, WINDOW_HEIGHT * .3, WINDOW_HEIGHT * .3, 1);
		if (nk_button_label(ctx, "")) {
			fprintf(stdout, "history pressed\n");
			this->view = 2;
			nk_clear(this->ctx);
		}
		this->drawImage(&this->historyImage);
	}
	nk_end(this->ctx);

	return true;
}

inline bool FE::DrawScansPage()
{
	/* LOGO */
	if (nk_begin(this->ctx, "lavalogo", nk_rect(25, 25, WINDOW_HEIGHT * .33, WINDOW_HEIGHT * .33),
		NK_WINDOW_NO_SCROLLBAR)) {
		this->drawImage(&this->squareImage);
	}
	nk_end(this->ctx);


	return true;
}

inline FE::FE(sf::Window *win) {
	this->view = 0;

	/* INIT IMAGES */
	this->pp.scan = "../Assets/scan2.png";
	this->pp.rectLogo = "../Assets/rectLogo.png";
	this->pp.trapLogo = "../Assets/trapLogo.png";
	this->pp.squareLogo = "../Assets/squareLogo.png";
	this->pp.history = "../Assets/historylarger.png";

	glewExperimental = 1;
	if (glewInit() != GLEW_OK) {
		fprintf(stderr, "Failed to setup GLEW\n");
		std::cout << glewGetErrorString(glewInit());
		exit(1);
	}

	/* GUI */
	//struct nk_context* ctx;
	this->ctx = nk_sfml_init(win);
	// load font here if want
	// load cursor here if want
	//struct nk_font_atlas* atlas;
	nk_sfml_font_stash_begin(&this->atlas);
	nk_sfml_font_stash_end();

	// load some image stuff
	//struct nk_buffer cmds;
	////nk_buffer_init_default(&cmds);
	glEnable(GL_TEXTURE_2D);

	// bg color
	struct nk_colorf bg;
	this->bg.r = 45 / 255.0f; this->bg.g = 45 / 255.0f; this->bg.b = 45 / 255.0f; this->bg.a = 1.0f;

	this->scanImage = this->icon_load(pp.scan);
	this->squareImage = this->icon_load(pp.squareLogo);
	this->historyImage = this->icon_load(pp.history);
}

#endif