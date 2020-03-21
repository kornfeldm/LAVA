// ConsoleApplication1.cpp : This file contains the 'main' function. Program execution begins and ends there.

#pragma once
#include <iostream>
#include <stdio.h>
#include <stdlib.h>  
#include <string.h>
#include <io.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <vector>
#include "clamav.h"
#include "openssl/opensslv.h"
#include "dirent.h"
#include "lava_advancedScan.h"



int main()
{

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

    printf("\n\n Testing scanFile and scanDirectory functions:\n");
    
    //Testing scanFile function
    struct cl_scan_options options;
    options.general = CL_SCAN_GENERAL_ALLMATCHES;
    scanFile("C:\\Users\\dylan\\Documents\\Availability_Cenotto_Spring_2020.png", engine, options);

    std::string AntibodyFileLocation = "C:\\test.LavaAnti";
    //Testing scanDirectory function
    bool clean = scanDirectory("C:\\Users\\dylan\\Documents\\", engine, options, AntibodyFileLocation);
    if (clean) printf("Directory is not infected");
    else printf("Directory is not infected");

    //Testing quickScan function
    clean = quickScan(AntibodyFileLocation, engine, options);
    if (clean) printf("Directory is not infected");
    else printf("Directory is not infected");
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