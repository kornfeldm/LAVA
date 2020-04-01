#pragma once
#ifndef LAVASCAN_H
#define LAVASCAN_H
#pragma warning(disable : 4996) //_CRT_SECURE_NO_WARNINGS
extern std::string CurrentScanFile = std::string("");
class LavaScan
{
public:
	// memburs
	std::string AntibodyFileLocation = "C:\\test.LavaAnti";
	int fd, ret, CurrentScanCount;
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
	bool AdvanceScanNow(std::set<std::string> s);
	void eicarTest(std::string path="C:\\test\\eicar.com.txt");
	void printQuarantineContents();
	void quarantineFile(std::string filepath);
	bool removeQuarantinedFiles();
	bool quarantineIsEmpty();
	bool scanDirectory(std::string dirPath);
	void iterateDirectory(std::string directory, bool clean);
	int scanFile(std::string filePath);
	void AddToAntibody(std::string dirPath, std::string antibodyfilelocation);
	std::vector<std::string> ReadAntibody(std::string antibodyfilelocation);
	int FileCount(std::string dirPath);
	int TotalSetFileCount(std::set<std::string> p);
	std::set<std::string> countQuarantineContents();
};

inline int LavaScan::scanFile(std::string filePath) {
	// update scan count
	CurrentScanCount++;
	// update current scan dir for GUI
	CurrentScanFile = filePath;
	//std::cout << "\n\tfile:" << filePath;
	const char* virname;
	int ret = cl_scanfile(filePath.c_str(), &virname, NULL, engine, &options); //scanning file using clamAV
	if (ret == CL_VIRUS) {
		/*printf("--------------------------------------------------------------------------------------\n");
		printf("Virus detected: %s\n", virname);
		printf("--------------------------------------------------------------------------------------\n");
		*///QUARANTINE FILE IF NOT IN SYSTEM FOLDER
		if (filePath.substr(3, 7) == "Windows")
		{
			printf("VIRUS DETECTED IN SYSTEM FOLDER! FILE %s IS INFECTED! IMMIDIATE ACTION REQUIRED!", filePath); //Infected file is in system folder
		}
		else {
			quarantineFile(filePath); //Infected file is not in system folder
		}
	}
	else {
		//printf("No virus detected.\n");
		if (ret != CL_CLEAN) {
			printf("Error: %s\n", cl_strerror(ret)); //In case of scan error
		}
	}
	return ret; //returns scan output value: either CL_VIRUS, CL_CLEAN or CL_ERROR
}

inline void LavaScan::iterateDirectory(std::string directory, bool clean)
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
				iterateDirectory(newDirectory, clean);
				//CHECK FOR USER CANCEL GOES HERE
				//closedir(directory)
				//return

			}
			//If the item is a file
			if (item->d_type == DT_REG)
			{
				std::string filePath = directory + item->d_name; // generate filepath

				//std::cout << filePath << "\n";
				//scan the file using clamAV

				if (scanFile(filePath) == CL_VIRUS) {
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

namespace fs = std::filesystem;

inline int LavaScan::FileCount( std::string dirPath )
{
	getch();
	int count = 0;
	for (auto& p : fs::recursive_directory_iterator(dirPath))
		count++;
	
	return count;
}

inline int LavaScan::TotalSetFileCount(std::set<std::string> p) {
	int count = 0;
	for (auto path : p) {
		//std::cout << "\t" << path << ".\n";
		struct stat s;
		if (stat(path.c_str(), &s) == 0)
		{
			if (s.st_mode & S_IFDIR)
			{
				count += FileCount(path);
			}
			else if (s.st_mode & S_IFREG)
			{
				count++;
			}
			else
			{
				//something else
				std::cout << "\n ok wtf is this\n";
			}
		}
		else
		{
			//error
			return -4;
		}
	}

	return count;
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
	this->CurrentScanCount++; // 
	bool clean = true; //Setting directory as clean
	int mode = 0; //Setting initial mode to 0
	bool cancel = false; //A cancel variable in case the user wants to cancel the scan
	iterateDirectory(dirPath, clean);//call recursive helper funciton
	//if (cancel == true) printf("User canceled scan");
	//return clean; //returns false if infected and true if clean
	return true;
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

std::string& replace(std::string& s, const std::string& from, const std::string& to)
{
	if (!from.empty())
		for (size_t pos = 0; (pos = s.find(from, pos)) != std::string::npos; pos += to.size())
			s.replace(pos, from.size(), to);
	return s;
}

inline bool LavaScan::AdvanceScanNow(std::set<std::string> ss)
{
	// set ss is the set of shit we are gonna scan...simple foreach loop for now
	for (auto path : ss) {
		//std::cout << "\t" << path << ".\n";
		struct stat s;
		if (stat(path.c_str(), &s) == 0)
		{	
			if (s.st_mode & S_IFDIR)
			{
				//it's a directory, use scandir
				//std::cout << path << " is a directory.";
				scanDirectory(std::string(path+"\\"));
			}
			else if (s.st_mode & S_IFREG)
			{
				//it's a file, use scanfile
				//std::cout << path << " is a file.";
				scanFile(path.c_str());
			}
			else
			{
				//something else
				std::cout<<"\n ok wtf is this\n";
			}
		}
		else
		{
			//error
			return false;
		}
	}
	return true;
}

inline bool LavaScan::quarantineIsEmpty() { //checks whether quarantined file is empty
	std::fstream q_file;
	q_file.open("quarantine.lava", std::ios::in | std::ios::out);
	return q_file.peek() == std::ifstream::traits_type::eof();
}

inline void LavaScan::eicarTest(std::string path)
{
	scanFile(path);
	return;
}

inline std::set<std::string> LavaScan::countQuarantineContents() { //prints the contents of the quarantine file
	std::set<std::string> s;
	std::fstream quarantineFile;
	quarantineFile.open("quarantine.lava", std::ios::in | std::ios::out);
	std::string line;
	while (getline(quarantineFile, line)) {
		//std::cout << line << std::endl;
		s.insert(line);
	}
	quarantineFile.close();
	return s;
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
	CurrentScanCount = 0;
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
		printf("Signature database initialization successful\n %u Signatures loaded\n", sigs);
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