// ConsoleApplication1.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <stdio.h>
#include <stdlib.h>  
#include <string.h>
#include <io.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
<<<<<<< Updated upstream
#include "openssl/opensslv.h"
#include "clamav.h"
=======
#include <vector>
#include "dirent.h"
#include "lava_quarantine.h"
#include "lava_advancedScan.h"
#include "LavaScan.h"
#include "lava_log.h"
#include <stack>

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
>>>>>>> Stashed changes

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

}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
