#include "lava_advancedScan.h"

int scanFile(const char* filePath, struct cl_engine* engine, struct cl_scan_options options) {
    const char* virname;
    int ret = cl_scanfile(filePath, &virname, NULL, engine, &options); //scanning file using clamAV
    if (ret == CL_VIRUS) {
        printf("Virus detected: %s\n", virname);
        //QUORANTINE FILE IF NOT IN SYSTEM FOLDER
    }
    else {
        printf("No virus detected.\n");
        if (ret != CL_CLEAN) {
            printf("Error: %s\n", cl_strerror(ret)); //In case of scan error
        }
    }
    return ret; //returns scan output value: either CL_VIRUS, CL_CLEAN or CL_ERROR
}

void iterateDirectory(std::string directory, int mode, struct cl_engine* engine,
    struct cl_scan_options options, bool clean, bool cancel)
{
    //Source used: https://github.com/tronkko/dirent/blob/master/examples/ls.c

    //Prevents an infinite loop since ./ comes up as a folder for each directory
    if (directory.substr(directory.length() - 2) == ".\\")
    {
        return;
    }
    if (cancel == true) {
        return;
    }
    //If this is a subdirectory of C:/Windows then eleveate the mode to 1
    if (directory.substr(3, 7) == "Windows")
    {
        mode = 1;
    }
    //If the user wants to cancel the scanning process
    

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
                iterateDirectory(newDirectory, mode, engine, options, clean, cancel);
                
                //if user cancels will close current directory and return to parent directory
                if (cancel == true) {
                    closedir(dir);
                    return;
                }

            }
            //If the item is a file
            if (item->d_type == DT_REG)
            {
                std::string filePath = directory + item->d_name; // generate filepath
                std::cout << item->d_name << "\n";
                //scan the file using clamAV
                if (mode == 0)
                {
                    //This file is not in WINDOWS
                    if (scanFile(filePath.c_str(), engine, options) == CL_VIRUS) {
                        clean = false; //File in direcotry is infected
                        //quorantine(filePath);
                    }

                }
                else
                {
                    //This file *is* in WINDOWS
                    if (scanFile(item->d_name, engine, options) == CL_VIRUS) {
                        //DON'T QUORANTINE
                        printf("System file %s is infected!", item->d_name);
                    }

                }


            }
            //Check if the user cancels scan
            //TODO: ADD A CHECK FOR CANCEL PROCESS WHEN UI IS READY
            //If user cancels close current directory and return to parent
            if (cancel == true) {
                closedir(dir);
                return;
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
    int mode = 0; //Setting initial mode to 0
    bool cancel = false; //A cancel variable in case the user wants to cancel the scan
    iterateDirectory(dirPath, mode, engine, options, clean, cancel);//call recursive helper funciton
    if (cancel == true) printf("User canceled scan");
    return clean; //returns false if infected and true if clean
}