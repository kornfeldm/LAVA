#pragma once
#include <iostream>
#include <stdio.h>
#include <stdlib.h>  
#include <string.h>
#include <string>
#include <io.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <vector>
#include <fstream>
#include "clamav.h"
#include "openssl/opensslv.h"
#include "dirent.h"

int scanFile(const char* filePath, struct cl_engine* engine, struct cl_scan_options options);
bool scanDirectory(std::string dirPath, struct cl_engine* engine, struct cl_scan_options options, std::string antibodyfilelocation);
bool quickScan(std::string antibodyfilelocation, struct cl_engine* engine, struct cl_scan_options options);