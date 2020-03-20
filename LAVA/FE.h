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

/* Use this function to allocate space for text below an image. see how the back arrow is displayed in the DisplayScansPage function is used
*/
struct nk_rect SubRectTextBelow(struct nk_rect *big, struct nk_rect *sub ) {
	return nk_rect(sub->x, sub->h, sub->w, big->h-sub->h+sub->x);
}

struct pics {
	const char* squareLogo;
	const char* rectLogo;
	const char* trapLogo;
	const char* scan;
	const char* history;
	const char* backArrow;
	const char* chooseScan;
	const char* triangleButton;
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
	struct nk_command_buffer* canvas;
	// font stuff
	struct nk_font_atlas* atlas;
	struct nk_font* font;
	const char* font_path = "../Assets/font.ttf";
	//struct nk_font_config* fontCFG;// can be null so commenting out for mem
	struct nk_colorf bg;
	struct nk_colorf whiteFont;
	struct nk_rect total_space;
	// pics to be loaded (pngs)
	pics pp;
	struct nk_image scanImage;
	struct nk_image rectImage; 
	struct nk_image trapImage; 
	struct nk_image squareImage; 
	struct nk_image historyImage;
	struct nk_image backArrow;
	struct nk_image chooseScan;
	struct nk_image triangleLogo;
	// view screen switch. view ScanViews scanview member for furthger info
	unsigned int view;
	/*
		0 : logo screen
		1 : scans screen
		2 : history screen
	*/
	unsigned int m_scanViews;
	/*
		0 : plz maam pls choose a scan
		1 : coimplete scan
		2 : quick scan
		3 : advanced scan
	*/
	
	/* MEMBER FUNCTIONS */
	static struct nk_image icon_load(const char* filename, bool flip = false);
	bool drawImage(struct nk_image *img);
	bool drawImageSubRect(struct nk_image* img, struct nk_rect* r);
	bool DrawMainPage();
	bool DrawScansPage();
	bool Display();
	bool NoScanView();
	bool CompleteScanView();
	bool viewSwap();
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

inline bool FE::drawImageSubRect(struct nk_image* img, struct nk_rect *r)
{
	canvas = nk_window_get_canvas(this->ctx);
	nk_draw_image(canvas, *r, img, IMG_COLOR);
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
			this->view = 1; // scans page
			this->m_scanViews = 0; // choose a scan sub view
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

bool FE::NoScanView() {
	int filled_width = WINDOW_WIDTH * .08 * 2 + WINDOW_WIDTH * .075; // remaining width ofscreen after side menu
	int delta = WINDOW_WIDTH - filled_width;
	/* CHOOSE A SCAN */
	struct nk_rect center = nk_rect(filled_width + delta * .2, WINDOW_HEIGHT * .20-36-50, delta * .6-50, WINDOW_HEIGHT * .6 -36+50);
	struct nk_rect centerAndText = nk_rect(center.x, center.h+90, center.w+50, 80 + 80+30); //36 for font size!
	if (nk_begin(this->ctx, "ChooseScan", center, NK_WINDOW_NO_SCROLLBAR)) {
		this->drawImageSubRect(&this->chooseScan, &center);
	}
	nk_end(this->ctx);
	/* TEXT TO CHOOSE A SCAN*/
	if (nk_begin(this->ctx, "ChooseScantxt", centerAndText, NK_WINDOW_NO_SCROLLBAR))
	{
		/*nk_layout_row_dynamic(this->ctx, 40, 1);
		nk_label_wrap(this->ctx, "                                                        ");
		nk_layout_row_dynamic(this->ctx, 80, 1);
		nk_label_wrap(this->ctx, "Chose a Scan, Please!");*/
		// old way of drawing txt...low level api
		nk_draw_text(nk_window_get_canvas(this->ctx), centerAndText, "  Choose A Scan, Please!", 24, &this->atlas->fonts->handle, nk_rgb(255, 255, 255), nk_rgb(255, 255, 255));
	}
	nk_end(this->ctx);

	return true;
}

bool FE::CompleteScanView() {
	int filled_width = WINDOW_WIDTH * .08 * 2 + WINDOW_WIDTH * .075; // remaining width ofscreen after side menu
	int delta = WINDOW_WIDTH - filled_width;

	/* quick scan header and text */
	if (nk_begin(this->ctx, "Complete Scan", nk_rect(300, WINDOW_HEIGHT * .06, 600, 300),
		NK_WINDOW_TITLE | NK_WINDOW_NO_SCROLLBAR))
	{
		nk_layout_row_dynamic(this->ctx, 40, 1);
		nk_label_wrap(this->ctx, "                                                        ");
		nk_layout_row_dynamic(this->ctx, 80, 1);
		nk_label_wrap(this->ctx, "This scan will traverse all files on the current file-system!");

		// scan now button here ??
	}
	nk_end(this->ctx);

	return true;
}

bool FE::viewSwap() {
	if (this->m_scanViews == 0) {
		//fprintf(stdout, "we is in default scan view bruh\n");
		NoScanView();
	}
	else if (this->m_scanViews == 1) {
		//fprintf(stdout, "we is in compelte scan view bruh\n");
		CompleteScanView();
	}
	else if (this->m_scanViews == 2) {
		fprintf(stdout, "we is in quick scan view bruh\n");
	}
	else if (this->m_scanViews == 3) {
		fprintf(stdout, "we is in advanced scan view bruh\n");
	}
	else {
		fprintf(stderr, "this shouldnt of happened, mr. freeman\n");
	}
	return true;
}

inline bool FE::DrawScansPage()
{
	/* BACK ARROW ICON */
	struct nk_rect bar = nk_rect(0, 0, WINDOW_WIDTH * .08, WINDOW_HEIGHT * .08);
	struct nk_rect backArrowAndText = nk_rect(bar.x, bar.y, bar.w, bar.h + 36); //36 for font size!
	if (nk_begin(this->ctx, "barrow", backArrowAndText,
		NK_WINDOW_NO_SCROLLBAR)) {

		/* hidden button behind icon to press */
		nk_layout_row_static(ctx, bar.y + bar.h + 36, bar.x + bar.w, 2);
		if (nk_button_label(ctx, "")) {
			fprintf(stdout, "back arrow\n");
			this->view = 0;
			nk_clear(this->ctx);
		}
		this->drawImageSubRect(&this->backArrow, &bar);
		nk_draw_text(nk_window_get_canvas(this->ctx), SubRectTextBelow(&backArrowAndText, &bar), " BACK ", 6, &this->atlas->fonts->handle, nk_rgb(255, 255, 255), nk_rgb(255, 255, 255));
	}
	nk_end(this->ctx);

	// vertical line
	nk_stroke_line(this->canvas, bar.w * 2 + WINDOW_WIDTH * .075, WINDOW_HEIGHT - WINDOW_HEIGHT * .06,
		bar.w * 2 + WINDOW_WIDTH * .075, 0+WINDOW_HEIGHT * .06, 2, nk_rgb(255, 255, 255));

	int w_space = bar.w * 2 + WINDOW_WIDTH * .075;
	/* COMPLETE SCAN */
	struct nk_rect completeScanTextRect = nk_rect(.025*w_space, WINDOW_HEIGHT*.30, w_space*.95, 36);
	if (nk_begin(this->ctx, "CompleteScan", completeScanTextRect, NK_WINDOW_NO_SCROLLBAR)) {
		nk_layout_row_static(this->ctx, completeScanTextRect.y + completeScanTextRect.h, completeScanTextRect.x + completeScanTextRect.w, 2);
		if (nk_button_label(this->ctx, "")) {
			fprintf(stdout, "Complete Scan\n");
			this->m_scanViews = 1;
			nk_clear(this->ctx);
		}
		nk_draw_text(nk_window_get_canvas(this->ctx), completeScanTextRect, " Complete Scan", 14, &this->atlas->fonts->handle, nk_rgb(255, 255, 255), nk_rgb(255, 255, 255));
	}
	nk_end(this->ctx);

	/* QUICK SCAN */
	struct nk_rect quickScanTextRect = nk_rect(.025 * w_space, WINDOW_HEIGHT * .45, w_space * .95, 36);
	if (nk_begin(this->ctx, "QuickScan", quickScanTextRect, NK_WINDOW_NO_SCROLLBAR)) {
		nk_layout_row_static(this->ctx, quickScanTextRect.y + quickScanTextRect.h, quickScanTextRect.x + quickScanTextRect.w, 2);
		if (nk_button_label(this->ctx, "")) {
			fprintf(stdout, "Quick Scan\n");
			this->m_scanViews = 2;
			nk_clear(this->ctx);
		}
		nk_draw_text(nk_window_get_canvas(this->ctx), quickScanTextRect, " Quick Scan   ", 14, &this->atlas->fonts->handle, nk_rgb(255, 255, 255), nk_rgb(255, 255, 255));
	}
	nk_end(this->ctx);

	/* ADVANCED SCAN */
	struct nk_rect advScanTextRect = nk_rect(.025 * w_space, WINDOW_HEIGHT * .60, w_space * .95, 36);
	if (nk_begin(this->ctx, "AdvancedScan", advScanTextRect, NK_WINDOW_NO_SCROLLBAR)) {
		nk_layout_row_static(this->ctx, advScanTextRect.y + advScanTextRect.h, advScanTextRect.x + advScanTextRect.w, 2);
		if (nk_button_label(this->ctx, "")) {
			fprintf(stdout, "Advanced Scan\n");
			this->m_scanViews = 3;
			nk_clear(this->ctx);
		}
		nk_draw_text(nk_window_get_canvas(this->ctx), advScanTextRect, " Advanced Scan", 14, &this->atlas->fonts->handle, nk_rgb(255, 255, 255), nk_rgb(255, 255, 255));
	}
	nk_end(this->ctx);

	this->viewSwap();

	return true;
}

inline FE::FE(sf::Window *win) {
	// set view to logo pAGe
	this->view = 0;
	this->m_scanViews = 0;
	/* INIT IMAGES */
	this->pp.scan = "../Assets/scan2.png";
	this->pp.rectLogo = "../Assets/rectLogo.png";
	this->pp.trapLogo = "../Assets/trapLogo.png";
	this->pp.squareLogo = "../Assets/squareLogo.png";
	this->pp.history = "../Assets/historylarger.png";
	this->pp.backArrow = "../Assets/back_arrow.png";
	this->pp.chooseScan = "../Assets/chooseScan.png";
	this->pp.triangleButton = "../Assets/triangle.png";

	glewExperimental = 1;
	if (glewInit() != GLEW_OK) {
		fprintf(stderr, "Failed to setup GLEW\n");
		std::cout << glewGetErrorString(glewInit());
		exit(1);
	}

	/* GUI */
	//struct nk_context* ctx;
	this->ctx = nk_sfml_init(win);
	/* set font shit--kinda confused by nuklear api on this one chiefton */
	{//struct nk_font_atlas* atlas;
		nk_sfml_font_stash_begin(&this->atlas);
		//this->font = nk_font_atlas_add_from_file(this->atlas, this->font_path, 18, NULL);
		struct nk_font* droid = nk_font_atlas_add_from_file(atlas, "../Assets/font.ttf", 36, 0);
		nk_sfml_font_stash_end();
		nk_style_set_font(this->ctx, &droid->handle);
		//nk_init_default(this->ctx, &font->handle);
	}

	// load some image stuff
	//struct nk_buffer cmds;
	////nk_buffer_init_default(&cmds);
	glEnable(GL_TEXTURE_2D);

	// bg color
	this->bg.r = 45 / 255.0f; this->bg.g = 45 / 255.0f; this->bg.b = 45 / 255.0f; this->bg.a = 1.0f;
	// white txt color
	this->whiteFont.r = 0; this->whiteFont.g =0; this->whiteFont.b = 0; this->whiteFont.a = 1;
	
	// load pngs 
	this->scanImage = this->icon_load(pp.scan);
	this->squareImage = this->icon_load(pp.squareLogo);
	this->historyImage = this->icon_load(pp.history);
	this->backArrow = this->icon_load(pp.backArrow);
	this->chooseScan = this->icon_load(pp.chooseScan);
	this->triangleLogo = this->icon_load(pp.triangleButton);
}

#endif