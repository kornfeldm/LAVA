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
#include <limits.h>
#include <time.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <SFML/OpenGL.hpp>
#include <SFML/Window.hpp>
#include <windows.h>
#include <ShlObj.h>
#include <sstream>
#include <io.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <vector>
#include "dirent.h"
#include "lava_advancedScan.h"

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

	int fd, ret;
    unsigned long int size = 0;
    unsigned int sigs = 0;
    long double mb;
    const char* virname;
    struct cl_engine* engine;

    if ((ret = cl_init(CL_INIT_DEFAULT)) != CL_SUCCESS) { //Initializing clamav
        printf("Can't initialize libclamav: %s\n", cl_strerror(ret));//returns the error name in case of error
        return 2;
    }
    else {
        printf("initialization successful\n");
    }

    if (!(engine = cl_engine_new())) { //Creating new engine instance
        printf("Can't create new engine\n");
        return 1;
    }
    else {
        printf("ClamAV engine initialized\n");
    }

    printf("Inititalizing signature database...\n");
    printf("Default database path: %s\n", cl_retdbdir());
	
    if ((ret = cl_load(cl_retdbdir(), engine, &sigs, CL_DB_STDOPT)) != CL_SUCCESS) { //Loads the database file from the default db folder
        printf("Can't initialize signature database: %s\n", cl_strerror(ret)); //returns the error name in case of error
        return 2;
    }
    else {
        printf("Signature database initialization successful\n %u signatures loaded\n", sigs);
    }

    if ((ret = cl_engine_compile(engine)) != CL_SUCCESS) { //Compiles the ClamAV engine
        printf("cl_engine_compile() error: %s\n", cl_strerror(ret));//returns the error name in case of error
        cl_engine_free(engine);
        return 1;
    }
    else {
        printf("ClamAV engine ready!");
    }

    printf("\n\n Testing scanFile and scanDirectory functions:\n\n");
    
    //Testing scanFile function
    struct cl_scan_options options;
    options.general = CL_SCAN_GENERAL_ALLMATCHES;
    //scanFile("C:/test/eicar.com.txt", engine, options);

    //std::string AntibodyFileLocation = "C:\\test.LavaAnti";
    ////Testing scanDirectory function
    //bool clean = scanDirectory("C:\\Users\\tom\\Documents\\", engine, options, AntibodyFileLocation);
    //if (clean) printf("Directory is not infected");
    //else printf("Directory is not infected");

    ////Testing quickScan function
    //clean = quickScan(AntibodyFileLocation, engine, options);
    //if (clean) printf("Directory is not infected");
    //else printf("Directory is not infected");
	
	/* Platform */
	sf::ContextSettings settings(24, 8, 4, 2, 2);
	sf::Window win(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "LAVA", sf::Style::Close, settings);
	win.setVerticalSyncEnabled(true);
	win.setActive(true);
	glViewport(0, 0, win.getSize().x, win.getSize().y);

	FE FrontEnd = FE(&win);

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
