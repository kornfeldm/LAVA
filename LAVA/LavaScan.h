#pragma once
#ifndef LAVASCAN_H
#define LAVASCAN_H

class LavaScan
{
public:
	// memburs
	int fd, ret;
	unsigned long int size = 0;
	unsigned int sigs = 0;
	long double mb;
	const char* virname;
	struct cl_engine* engine;
	struct cl_scan_options options;
	// constructor
	LavaScan(); // default
	/* scans */
	bool CompleteScan();
	bool QuickScan();
	bool AdvanceScan();

	void printQuarantineContents();
	void quarantineFile(std::string filepath);
	bool removeQuarantinedFiles();
	bool quarantineIsEmpty();
	bool scanDirectory(std::string dirPath, struct cl_engine* engine, struct cl_scan_options options);
	void iterateDirectory(std::string directory, struct cl_engine* engine,
		struct cl_scan_options options, bool clean);
	int scanFile(std::string filePath, struct cl_engine* engine, struct cl_scan_options options);
};

inline int LavaScan::scanFile(std::string filePath, struct cl_engine* engine, struct cl_scan_options options) {
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

inline void LavaScan::iterateDirectory(std::string directory, struct cl_engine* engine,
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

inline bool LavaScan::scanDirectory(std::string dirPath, struct cl_engine* engine, struct cl_scan_options options) {
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

inline bool LavaScan::quarantineIsEmpty() { //checks whether quarantined file is empty
	std::fstream q_file;
	q_file.open("quarantine.lava", std::ios::in | std::ios::out);
	return q_file.peek() == std::ifstream::traits_type::eof();
}

inline void LavaScan::printQuarantineContents() { //prints the contents of the quarantine file
	std::fstream quarantineFile;
	quarantineFile.open("quarantine.lava", std::ios::in | std::ios::out);
	std::string line;
	while (getline(quarantineFile, line)) {
		std::cout << line << std::endl;
	}
	quarantineFile.close();
}

inline void LavaScan::quarantineFile(std::string filepath) { //quarantine file given filepath
	std::fstream quarantineFile;
	quarantineFile.open("quarantine.lava", std::ios::app);
	quarantineFile << filepath << std::endl;
	quarantineFile.close();
}

inline bool LavaScan::removeQuarantinedFiles() { //returns false if errors occurred
	std::cout << "Do you want to remove the files in quarantine? [y/n]:" << std::endl;
	bool success = true;
	std::string answer;
	std::cin >> answer;
	if (answer[0] == 'y') {
		std::fstream quarantineFile;
		std::fstream temp;
		temp.open("temp.lava", std::fstream::out); //creating a temporary file to store the undeleted files
		quarantineFile.open("quarantine.lava", std::ios::in | std::ios::out); //opening the quarantine file
		temp << "";
		std::string line;
		while (getline(quarantineFile, line)) {
			if (remove(line.c_str()) != 0) {
				temp << line << std::endl;
				std::cout << "error deleting file " << line << std::endl;
				success = false;
			}
			else {
				std::cout << "successfully deleted file %s" << line << std::endl;
			}
		}
		temp.close();
		quarantineFile.close();
		remove("quarantine.lava");
		rename("temp.lava", "quarantine.lava");
	}
	else {
		std::cout << "Files not deleted, but still in quarantine" << std::endl;
	}
	return success;
}

inline bool LavaScan::CompleteScan() {
	// for now just scan c$
	//   later get func to get drive letters
	const char* dirs = "c$";
	this->scanDirectory(dirs, this->engine, this->options);
}

inline LavaScan::LavaScan() {

	if ((ret = cl_init(CL_INIT_DEFAULT)) != CL_SUCCESS) { //Initializing clamav
		printf("Can't initialize libclamav: %s\n", cl_strerror(ret));//returns the error name in case of error
		exit(2);
	}
	else {
		printf("initialization successful\n");
	}

	if (!(engine = cl_engine_new())) { //Creating new engine instance
		printf("Can't create new engine\n");
		exit(1);
	}
	else {
		printf("ClamAV engine initialized\n");
	}

	printf("Inititalizing signature database...\n");
	printf("Default database path: %s\n", cl_retdbdir());

	if ((ret = cl_load(cl_retdbdir(), engine, &sigs, CL_DB_STDOPT)) != CL_SUCCESS) { //Loads the database file from the default db folder
		printf("Can't initialize signature database: %s\n", cl_strerror(ret)); //returns the error name in case of error
		exit(2);
	}
	else {
		printf("Signature database initialization successful\n %u signatures loaded\n", sigs);
	}

	if ((ret = cl_engine_compile(engine)) != CL_SUCCESS) { //Compiles the ClamAV engine
		printf("cl_engine_compile() error: %s\n", cl_strerror(ret));//returns the error name in case of error
		cl_engine_free(engine);
		exit(1);
	}
	else {
		printf("ClamAV engine ready!");
	}

	printf("\n\n Testing scanFile and scanDirectory functions:\n\n");
	//Testing scanFile function
	
	this->options.general = CL_SCAN_GENERAL_ALLMATCHES;
	//scanFile("C:/test/eicar.com.txt", engine, options);

	//std::string AntibodyFileLocation = "C:\\test.LavaAnti";
	////Testing scanDirectory function
	//bool clean = scanDirectory("C:\\Users\\tom\\Documents\\", engine, options, AntibodyFileLocation);
	//if (clean) printf("Directory is not infected");
	//else printf("Directory is not infected");

	//Testing quickScan function
	/*clean = quickScan(AntibodyFileLocation, engine, options);
	if (clean) printf("Directory is not infected");
	else printf("Directory is not infected");*/
}

#endif