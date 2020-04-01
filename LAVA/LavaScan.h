#pragma once
#ifndef LAVASCAN_H
#define LAVASCAN_H
#pragma warning(disable : 4996) //_CRT_SECURE_NO_WARNINGS
class LavaScan
{
public:
	// memburs
	std::string AntibodyFileLocation = "C:\\test.LavaAnti";
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
	bool AdvancedScan();

	void printQuarantineContents();
	void quarantineFile(std::string filepath);
	bool removeQuarantinedFiles();
	bool quarantineIsEmpty();
	bool scanDirectory(std::string dirPath);
	void iterateDirectory(std::string directory, bool clean, int viruses_found);
	int scanFile(std::string filePath, bool scanningDirectory);
	void AddToAntibody(std::string dirPath, std::string antibodyfilelocation);
	std::vector<std::string> ReadAntibody(std::string antibodyfilelocation);
	void log_scan(std::string start, std::string finish, int found, int removed);
	void read_log();
	std::string get_time();

};

inline int LavaScan::scanFile(std::string filePath, bool scanningDirectory) {
	const char* virname;
	std::string start_time = get_time();
	bool infected = false;
	int ret = cl_scanfile(filePath.c_str(), &virname, NULL, engine, &options); //scanning file using clamAV
	if (ret == CL_VIRUS) {
		printf("--------------------------------------------------------------------------------------\n");
		printf("Virus detected: %s\n", virname);
		printf("--------------------------------------------------------------------------------------\n");
		infected = true;
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
	std::string finish_time = get_time();
	if (!scanningDirectory) {
		log_scan(start_time, finish_time, infected, infected); //log scan if not scanning directory
	}
	return ret; //returns scan output value: either CL_VIRUS, CL_CLEAN or CL_ERROR
}

inline void LavaScan::iterateDirectory(std::string directory, bool clean, int viruses_found)
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
				iterateDirectory(newDirectory, clean, viruses_found);
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

				if (scanFile(filePath,true) == CL_VIRUS) { //scanning with the iterate directory variable set as true
					clean = false; //File in direcotry is infected
					viruses_found++;
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

//Read each line in the specified file and output a string vector containing each directory
inline std::vector<std::string> LavaScan::ReadAntibody(std::string antibodyfilelocation)
{
	std::vector<std::string> directorylist;
	std::ifstream antibody;
	std::string listedirectory;
	antibody.open(antibodyfilelocation);
	if (antibody.is_open())
	{
		while (!antibody.eof())
		{
			getline(antibody, listedirectory);
			directorylist.push_back(listedirectory);
			std::cout << listedirectory << "\n";
		}
		antibody.close();
		return directorylist;
	}
	else
	{
		//Error opening file
	}
	return directorylist;
}

//Write new directories to the antibody file. Parameters are <vector containting strings of diretories>, path to antibody file
void WriteAntibody(std::vector<std::string> directorylist, std::string antibodyfilelocation)
{
	std::ofstream antibody;
	antibody.open(antibodyfilelocation);
	if (antibody.is_open())
	{
		for (int i = 0; i < directorylist.size(); i++)
		{
			antibody << directorylist[i] << std::endl;
		}
		antibody.close();
	}
}

//Check if a directory is in the antibody file. Add it if it isn't.
//Ignore it if it's a subdirectory.
//Replace any subdirectories
inline void LavaScan::AddToAntibody(std::string dirPath, std::string antibodyfilelocation)
{
	std::vector<std::string> existing = ReadAntibody(antibodyfilelocation);
	bool newDirectory = true;
	int replace = 0;
	for (int i = 0; i < existing.size(); i++)
	{
		if (strstr(existing[i].c_str(), dirPath.c_str()) != NULL)
		{
			newDirectory = false;
		}
		if (strstr(dirPath.c_str(), existing[i].c_str()) != NULL)
		{
			existing[i] = dirPath;
		}
	}
	//Check for duplicates --This code should be tested more, it's just a temporary fix.
	for (int i = 0; i < existing.size(); i++)
	{
		for (int j = 0; j < existing.size(); j++)
		{
			if (existing[i] == existing[j])
			{
				existing.erase(existing.begin() + i);
			}
		}
	}

	if (newDirectory == true)
	{
		std::vector<std::string> directoryToAdd;
		directoryToAdd.push_back(dirPath);
		WriteAntibody(directoryToAdd, antibodyfilelocation);
	}
	remove(antibodyfilelocation.c_str());
	WriteAntibody(existing, antibodyfilelocation);
}

inline bool LavaScan::scanDirectory(std::string dirPath) {
	std::string start_time = get_time();
	bool clean = true; //Setting directory as clean
	bool cancel = false; //A cancel variable in case the user wants to cancel the scan
	int viruses_found = 0;
	iterateDirectory(dirPath, clean, viruses_found);//call recursive helper funciton
	std::string finish_time = get_time();
	log_scan(start_time, finish_time, viruses_found, viruses_found);
	if (cancel == true) printf("User canceled scan");
	return clean; //returns false if infected and true if clean
}

//The engine and options are just passed through to the directory scanner.
//The first parameter is a path the the antibody file
inline bool LavaScan::QuickScan()
{
	//Keeps track of if malware is detected and where
	bool clean = true;
	//Read from the file
	std::vector<std::string> directorylist = ReadAntibody(this->AntibodyFileLocation);
	//For each directory:
	for (int i = 0; i < directorylist.size(); i++)
	{
		//Scan it
		clean = scanDirectory(directorylist[i]);

	}
	return clean;
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
	const char* dirs = "C:\\";
	return this->scanDirectory(dirs);
}

//inline bool LavaScan::QuickScan() {
//	// for now just scan docs and downloads 
//	//return this->scanDirectory(dirs, this->engine, this->options);
//	TCHAR my_profile[MAX_PATH];
//	SHGetFolderPath(NULL, CSIDL_PROFILE, NULL, SHGFP_TYPE_CURRENT, my_profile);
//	//std::wcout << "\nscanning " << my_profile << " and \n" << my_profile << "\\..\\Downloads\\\n";
//	char home[MAX_PATH];
//	wcstombs(home, my_profile, MAX_PATH);
//	// convert to string..
//	std::string s_home = std::string(home); std::string docs = s_home + "\\Documents"; std::string dl = s_home + "\\Downloads";
//	scanDirectory(docs, this->engine, this->options);
//	scanDirectory(dl, this->engine, this->options);
//	return true;
//}

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

	//Testing scanFile function
	
	this->options.general = CL_SCAN_GENERAL_ALLMATCHES;
	printf("\n\n Testing scanFile and scanDirectory functions:\n\n");
}
inline void LavaScan::log_scan(std::string start, std::string finish, int found, int removed) {
	std::fstream log_file;
	log_file.open("scan_log.lava", std::ios::app);
	log_file << start + "," + finish + "," + std::to_string(found) + "," + std::to_string(removed) + "\n";
	log_file.close();
}

inline std::string LavaScan::get_time() {
	time_t rawtime;
	struct tm timeinfo;
	char buffer[40];

	time(&rawtime);
	localtime_s(&timeinfo, &rawtime);

	strftime(buffer, 40, "%c", &timeinfo);
	return (std::string)buffer;
}

inline void LavaScan::read_log() {
	std::fstream log_file;
	std::stack<std::string> log_order; // using stack to get chronological log order
	log_file.open("scan_log.lava");
	std::string line; //string to store line charaacters
	while (getline(log_file, line)) {
		log_order.push(line); //filling the stack oldest in first, last out
	}
	std::string scan_strt, scan_fin, num_found, num_removed; // variable to store the virus info
	while (!log_order.empty()) {
		line = log_order.top(); //get value from log stack
		log_order.pop();//remove value from top
		int found = 0, prev_found = 0, comma_num = 0;
		while (found != std::string::npos) {
			prev_found = found;
			found = line.find(",", found + 1, 1);
			int size = found - prev_found;
			switch (comma_num)
			{
			case 0:
				scan_strt = line.substr(prev_found, size); //extract scan start time
			case 1:
				scan_fin = line.substr(prev_found + 1, size - 1); //exctract scan finish time
			case 2:
				num_found = line.substr(prev_found + 1, size - 1); //extract number of viruses found
				num_removed = line.substr(found + 1); //extract number of viruses removed
			}
			comma_num++;
		}
		//values of line are extracted (USE THEM FOR GUI HERE)
		std::cout << "Start time: " + scan_strt + ", End time: " + scan_fin + ", Number of viruses found: " + num_found + ", Number of viruses removed: " + num_removed + "\n" << std::endl;
	}
}
#endif