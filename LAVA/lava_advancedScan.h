#pragma once
// #include <iostream>
// #include <stdio.h>
// #include <stdlib.h>  
// #include <string.h>
// #include <io.h>
// #include <sys/types.h>
// #include <sys/stat.h>
// #include <fcntl.h>
// #include <vector>
// #include <fstream>
// #include "clamav.h"

class LavaScan
{

    //int scanFile(std::string filePath, struct cl_engine* engine, struct cl_scan_options options);
    //bool scanDirectory(std::string dirPath, struct cl_engine* engine, struct cl_scan_options options);

    int scanFile(std::string filePath, struct cl_engine* engine, struct cl_scan_options options) {
        const char* virname;
        int ret = cl_scanfile(filePath.c_str(), &virname, NULL, engine, &options); //scanning file using clamAV
        if (ret == CL_VIRUS) {
            printf("--------------------------------------------------------------------------------------\n");
            printf("Virus detected: %s\n", virname);
            printf("--------------------------------------------------------------------------------------\n");
            //QUARANTINE FILE IF NOT IN SYSTEM FOLDER
            if (filePath.substr(3, 7) == "Windows")
            {
                printf("VIRUS DETECTED IN SYSTEM FOLDER! FILE %s IS INFECTED! IMMIDIATE ACTION REQUIRED!", filePath); //Infected file is in system folder
            }
            else {
                quarantineFile(filePath); //Infected file is not in system folder
            }
        }
        else {
            printf("No virus detected.\n");
            if (ret != CL_CLEAN) {
                printf("Error: %s\n", cl_strerror(ret)); //In case of scan error
            }
        }
        return ret; //returns scan output value: either CL_VIRUS, CL_CLEAN or CL_ERROR
    }

    void iterateDirectory(std::string directory, struct cl_engine* engine,
        struct cl_scan_options options, bool clean)
    {
        //Source used: https://github.com/tronkko/dirent/blob/master/examples/ls.c

        //Prevents an infinite loop since ./ comes up as a folder for each directory
        if (directory.substr(directory.length() - 2) == ".\\")
        {
            return;
        }


        //If this is a subdirectory of C:/Windows then eleveate the mode to 1
        //if (directory.substr(3, 7) == "Windows")
        //{
        //    mode = 1;
        //}


        //Initilize the dirent class
        struct dirent* item;
        const char* location = directory.c_str();
        DIR* dir = opendir(location);

        //If the directory is value
        if (dir != NULL)
        {
            //Set "item" to the first item in the directory
            item = readdir(dir);
            //It it's valid (i.e. not the end of the directory)
            while (item != NULL)
            {
                //If the item is a folder
                if (item->d_type == DT_DIR)
                {
                    //Check all the subdirectories in it
                    std::string newDirectory = directory + item->d_name + "\\";

                    //std::cout << newDirectory << "\n";
                    iterateDirectory(newDirectory, engine, options, clean);
                    //CHECK FOR USER CANCEL GOES HERE
                    //closedir(directory)
                    //return

                }
                //If the item is a file
                if (item->d_type == DT_REG)
                {
                    std::string filePath = directory + item->d_name; // generate filepath
                    std::cout << filePath << "\n";
                    //scan the file using clamAV

                    if (scanFile(filePath, engine, options) == CL_VIRUS) {
                        clean = false; //File in direcotry is infected
                    }
                    //CHECK FOR USER CANCEL GOES HERE
                    //closedir(directory)
                    //return
                }
                //Shift the item to look at the next item
                item = readdir(dir);

            }
        }
        //Close the directory. Will cause temporary write issues if this does not happen.
        //In its current state, do not interrupt the process. We will need to fix this before release
        closedir(dir);
    }

    bool scanDirectory(std::string dirPath, struct cl_engine* engine, struct cl_scan_options options) {
        bool clean = true; //Setting directory as clean
        struct dirent* item;
        const char* location = dirPath.c_str();
        DIR* dir = opendir(location);

        if (dir == NULL) { //checks if directory exists
            printf("Directory does not exist!");
            closedir(dir); //closes directory and leaves the function
            return true;
        }

        closedir(dir);

        iterateDirectory(dirPath, engine, options, clean);//call recursive helper funciton
        return clean; //returns false if infected and true if clean
    }

};