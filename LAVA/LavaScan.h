#pragma once
#ifndef LAVASCAN_H
#define LAVASCAN_H
#pragma warning(disable : 4996) //_CRT_SECURE_NO_WARNINGS
extern std::string CurrentScanFile = std::string("");

class ProgressMonitor {
protected:
	//Internal variables
	float levelFiveFolderCount = 0;
	float levelFiveFoldersCompleted = 0;
	float progessPercentage = 0;

public:
	//Return the variables for use outside of the class
	float GetPercentage(){
		return progessPercentage;
	}
	float GetTotal() {
		return levelFiveFolderCount;
	}
	float GetCompleted() {
		return levelFiveFoldersCompleted;
	}
	//Call this every time a level five folder is scanned during a scan
	void FinishedDirectory(){
		levelFiveFoldersCompleted++;
		if (levelFiveFolderCount != 0){
			progessPercentage = levelFiveFoldersCompleted / levelFiveFolderCount;
		} else {
			progessPercentage = 0;
		}
		return;
	}
	
	//Call this for each directory added to a scan
	//Adds the amount of level five directories within a given directory to the total count
	void CountDirectories(std::string directory, int depth = 4, int level = 0) {
		//Source used: https://github.com/tronkko/dirent/blob/master/examples/ls.c
		//This is a modified version of iterateDirectory
		if (directory.substr(directory.length() - 2) == ".\\")
		{
			return;
		}
		struct dirent* item;
		const char* location = directory.c_str();
		DIR* dir = opendir(location);
		if (dir != NULL) {
			item = readdir(dir);
			while (item != NULL) {
				if (item->d_type == DT_DIR)
				{
					//If we're not at the fifth level yet, go a level deeper
					if (level < 4)
					{
						std::string newDirectory = directory + item->d_name + "\\";
						CountDirectories(newDirectory, depth, level++);
					}
					//If we are then add the amount of folders to the total count
					if (level == 4)
					{
						levelFiveFolderCount++;
					}
				}
				item = readdir(dir);
			}
		}
		closedir(dir);
		return;
	}
	
};


class LavaScan
{
public:
	// memburs
	std::string AntibodyFileLocation = "C:\\test.LavaAnti";
	int fd, ret, CurrentScanCount;
	unsigned long int size = 0; std::string start_time; std::string finish_time;
	unsigned int sigs = 0;
	long double mb;
	const char* virname;
	struct cl_engine* engine;
	struct cl_scan_options options;
	bool isScanDone;
	struct q_entry;
	std::string currentScanGoing;
	std::vector<std::vector<std::string>> PreviousScans;
	std::set<LavaScan::q_entry> QuarantineContents;
	int num_found;
	int num_removed;
	// constructor
	LavaScan(); // default
	/* scans */
	bool CompleteScan();
	bool QuickScan();
	bool AdvanceScan();
	bool AdvanceScanNow(std::set<std::string> s);
	void eicarTest(std::string path="C:\\test\\eicar.com.txt");
	std::set<LavaScan::q_entry> read_quarantine_contents();
	void quarantine_file(std::string filepath, std::string virus_name);
	bool remove_quarantined_files(std::set<std::string> to_remove);
	bool quarantineIsEmpty();
	void move_file(std::string file_path, std::string location_directory, std::string final_extension);
	void make_quarantine_directory();
	bool scanDirectory(std::string dirPath, int num_viruses_found);
	void iterateDirectory(std::string directory, bool clean, int num_viruses_found);
	int scanFile(std::string filePath);
	void AddToAntibody(std::string dirPath, std::string antibodyfilelocation);
	std::vector<std::string> ReadAntibody(std::string antibodyfilelocation);
	int FileCount(std::string dirPath);
	int TotalSetFileCount(std::set<std::string> p);
	std::set<std::string> countQuarantineContents();
	void log_scan();
	std::vector<std::vector<std::string>> read_log();
	std::string get_time();
	void UpdatePreviousScans();
	bool moveQuarantineHome(std::set<q_entry> q);
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
		printf("virus detected: %s\n", virname);
		getch();
		printf("--------------------------------------------------------------------------------------\n");*/
		//QUARANTINE FILE IF NOT IN SYSTEM FOLDER
		if (filePath.substr(3, 7) == "Windows")
		{
			printf("VIRUS DETECTED IN SYSTEM FOLDER! FILE %s IS INFECTED! IMMIDIATE ACTION REQUIRED!", filePath); //Infected file is in system folder
		}
		else {
			quarantine_file(filePath, virname); //Infected file is not in system folder
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

inline void LavaScan::iterateDirectory(std::string directory, bool clean, int num_viruses_found)
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
				iterateDirectory(newDirectory, clean, num_viruses_found);
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
					this->num_found++;//increase the virus count
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

inline bool LavaScan::scanDirectory(std::string dirPath, int num_viruses_found) {
	this->CurrentScanCount++; // 
	bool clean = true; //Setting directory as clean
	bool cancel = false; //A cancel variable in case the user wants to cancel the scan
	iterateDirectory(dirPath, clean, num_viruses_found);//call recursive helper funciton
	//if (cancel == true) printf("User canceled scan");
	//return clean; //returns false if infected and true if clean
	return true;
}

//The engine and options are just passed through to the directory scanner.
//The first parameter is a path the the antibody file
inline bool LavaScan::QuickScan()
{
	this->num_found = 0; //setting virus counter to 0
	this->start_time = get_time();//getting start time for scan
	//Keeps track of if malware is detected and where
	bool clean = true;
	//Read from the file
	std::vector<std::string> directorylist = ReadAntibody(this->AntibodyFileLocation);
	//For each directory:
	for (int i = 0; i < directorylist.size(); i++)
	{
		//Scan it
		clean = scanDirectory(directorylist[i], num_found);
	}
	this->finish_time = get_time();//get end time for scan
	//log_scan("Quick",start_time, finish_time, num_found, num_found); //log scan
	isScanDone = true;
	this->QuarantineContents = this->read_quarantine_contents();
	return true;
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
	this-> num_found = 0;//set the virus count to 0
	this-> start_time = get_time(); //get start time for scan
	for (auto path : ss) {
		//std::cout << "\t" << path << ".\n";
		struct stat s;
		if (stat(path.c_str(), &s) == 0)
		{	
			if (s.st_mode & S_IFDIR)
			{
				//it's a directory, use scandir
				//std::cout << path << " is a directory.";
				scanDirectory(std::string(path+"\\"), num_found);
			}
			else if (s.st_mode & S_IFREG)
			{
				//it's a file, use scanfile
				//std::cout << path << " is a file.";
				if (scanFile(path.c_str()) == CL_VIRUS) {
					this->num_found++;
				}
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
	this->finish_time = get_time(); //get end time
	//log_scan("Advanced",start_time, finish_time, num_found, num_found); //logging scan
	isScanDone = true;
	this->QuarantineContents = this->read_quarantine_contents();
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

inline struct LavaScan::q_entry {
	std::string old_file_name;
	std::string new_file_name;
	std::string origin_directory;
	std::string virus_name;
	bool operator <(const q_entry& q) const
	{
		return (origin_directory + new_file_name) < (q.origin_directory + q.new_file_name);
	}
};

inline std::set<LavaScan::q_entry> LavaScan::read_quarantine_contents() { //prints the contents of the quarantine file
	std::fstream quarantineFile;
	quarantineFile.open("quarantine.lava", std::ios::in | std::ios::out);
	std::set<struct q_entry> entries;
	std::string line;
	while (getline(quarantineFile, line)) {
		struct q_entry q;
		int found = 0, prev_found = 0, comma_num = 0;
		while (found != std::string::npos) { //extracting quarantined file info from quarantine entry
			prev_found = found;
			found = line.find(",", found + 1, 1);
			int size = found - prev_found;
			switch (comma_num)
			{
			case 0:
				q.old_file_name = line.substr(prev_found, size); //extract scan start time
			case 1:
				q.new_file_name = line.substr(prev_found + 1, size - 1); //exctract scan finish time
			case 2:
				q.origin_directory = line.substr(prev_found + 1, size - 1); //extract number of viruses found
				q.virus_name = line.substr(found + 1); //extract number of viruses removed
			}
			comma_num++;
		}
		entries.insert(q);
	}
	quarantineFile.close();
	return entries;
}

inline void LavaScan::quarantine_file(std::string filepath, std::string virus_name) { //quarantine file given filepath
	std::fstream quarantineFile;
	quarantineFile.open("quarantine.lava", std::ios::app);
	int pos = filepath.find_last_of("/\\"); // finding position of last "\"
	std::string file_name = filepath.substr(pos + 1); //original file name with extension
	std::string quarantine_file_name = file_name.substr(0, file_name.find(".")) + ".lava"; //new name of the file in quarantine
	std::string file_directory = filepath.substr(0, pos + 1); //original file directory path
	quarantineFile << file_name + "," + quarantine_file_name + "," + file_directory + "," + virus_name << std::endl; //adding it to the quarantine info file (quarantine.lava)
	quarantineFile.close();
	move_file(filepath, ".\\LAVA_Quarantine\\", ".lava"); //moving file to quarantine folder
}

inline void LavaScan::move_file(std::string file_path, std::string location_directory, std::string final_extension) {
	int pos = file_path.find_last_of("/\\"); // finding position of last "\"
	std::string file_name = file_path.substr(pos + 1); //original file name with extension
	std::ifstream from(file_path, std::ios::in | std::ios::binary);
	//changing the file extension to the desired final_extension
	std::ofstream to(location_directory + file_name.substr(0, file_name.find(".")) + final_extension, std::ios::out | std::ios::binary);
	to << from.rdbuf();
	from.close();
	to.close();
	std::remove(file_path.c_str()); //removing the original file
}

inline bool LavaScan::remove_quarantined_files(std::set<std::string> to_remove) { //returns false if errors occurred
	bool success = true;
	std::fstream quarantineFile;
	std::fstream temp;
	temp.open("temp.lava", std::fstream::out); //creating a temporary file to store the undeleted files
	quarantineFile.open("quarantine.lava", std::ios::in | std::ios::out); //opening the quarantine file
	temp << "";
	std::string line;
	while (getline(quarantineFile, line)) {
		int found = 0, prev_found = 0, comma_num = 0;
		std::string file_name, file_quarantine_name, file_origin, virus_name;
		while (found != std::string::npos) { //extracting quarantined file info from quarantine entry
			prev_found = found;
			found = line.find(",", found + 1, 1);
			int size = found - prev_found;
			switch (comma_num)
			{
			case 0:
				file_name = line.substr(prev_found, size); //extract scan start time
			case 1:
				file_quarantine_name = line.substr(prev_found + 1, size - 1); //exctract scan finish time
			case 2:
				file_origin = line.substr(prev_found + 1, size - 1); //extract number of viruses found
				virus_name = line.substr(found + 1); //extract number of viruses removed
			}
			comma_num++;
		}
		if (to_remove.find(file_name) == to_remove.end()) {
			//moving file back to its original location
			//move_file(".\\LAVA_Quarantine\\" + file_quarantine_name, file_origin, file_name.substr(file_name.find(".")));
			//temp << line << std::endl; //keeping it in quarantine //commenting this out for now. no need to write to file after removing.
		}
		else {
			if (std::remove((".\\LAVA_Quarantine\\" + file_quarantine_name).c_str()) != 0)
			{
				//Unable to remove file, keep it in the quarantine folder
				//error message
				temp << line << std::endl;
				success = false;
			}
			else {
				//removal successful
			}
		}
	}
	temp.close();
	quarantineFile.close();
	std::remove("quarantine.lava");
	std::rename("temp.lava", "quarantine.lava");
	return success;
}

inline void LavaScan::make_quarantine_directory() {
	struct stat buf;
	stat(".\\LAVA_Quarantine", &buf);
	//check if the quarantine directory exists
	if (!(buf.st_mode & S_IFDIR)) {
		//case if it doesn't exist
		printf("Quarantine directory not found.\n Attempting to make the Quarantine Directory...\n");
		if (int ret=CreateDirectoryA(".\\LAVA_Quarantine", NULL)==0) {
			if (ret == ERROR_ALREADY_EXISTS) {
				//direcotry already exists
				printf("Quarantine Direcotory Ready\n");
			}
			else if(ret== ERROR_PATH_NOT_FOUND){
				//idk how this could happen
			}
			else {
				//some other error
				std::cout << "Error making the quarantine directory, error: " + GetLastError() << std::endl;
			}
		}
		else printf("Quarantine Directory Ready\n");
	}
}

inline bool LavaScan::CompleteScan() {
	this->start_time = get_time();
	bool clean = true;
	int num_found = 0;
	// for now just scan c$
	//   later get func to get drive letters
	const char* dirs = "C:\\";
	clean = this->scanDirectory(dirs,num_found);
	this-> finish_time = get_time();
	//log_scan("Complete",start_time, finish_time, num_found, num_found);
	isScanDone = true;
	this->QuarantineContents = this->read_quarantine_contents();
	return clean;
}

inline void LavaScan::log_scan() {
	std::fstream log_file;
	log_file.open("scan_log.lava", std::ios::app);
	log_file << this->currentScanGoing + "," + this->start_time + "," + this->finish_time + "," + std::to_string(this->num_found) + "," + std::to_string(this->num_removed) + "\n";
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

inline std::vector<std::vector<std::string>> LavaScan::read_log() {
	std::vector<std::vector<std::string>> ret;

	std::fstream log_file;
	std::stack<std::string> log_order; // using stack to get chronological log order
	log_file.open("scan_log.lava");
	std::string line; //string to store line charaacters
	while (getline(log_file, line)) {
		log_order.push(line); //filling the stack oldest in first, last out
	}
	std::string scan_type, scan_strt, scan_fin, num_found, num_removed; // variable to store the virus info
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
				scan_type = line.substr(prev_found, size); //extract the scan type
			case 1:
				scan_strt = line.substr(prev_found + 1, size - 1); //extract scan start time
			case 2:
				scan_fin = line.substr(prev_found + 1, size - 1); //exctract scan finish time
			case 3:
				num_found = line.substr(prev_found + 1, size - 1); //extract number of viruses found
				num_removed = line.substr(found + 1); //extract number of viruses removed
			}
			comma_num++;
		}
		//values of line are extracted (USE THEM FOR GUI HERE)
		//std::cout << "Type of scan: " + scan_type + ", Start time: " + scan_strt + ", End time: " + scan_fin + ", Number of viruses found: " + num_found + ", Number of viruses removed: " + num_removed + "\n" << std::endl;
		if (scan_type != "") {
			// only add to ret if there is content
			std::vector<std::string> temp; temp.push_back(scan_type); temp.push_back(scan_strt);
			temp.push_back(scan_fin); temp.push_back(num_found); temp.push_back(num_removed);
			ret.push_back(temp);
			temp.clear();
		}
	}
	return ret;
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

inline void LavaScan::UpdatePreviousScans() {
	this->PreviousScans = this->read_log();
}

inline bool LavaScan::moveQuarantineHome(std::set<q_entry> q)
{
	for (auto thing : q) {
		move_file(".\\LAVA_Quarantine\\"+thing.new_file_name, thing.origin_directory, thing.old_file_name); //moving file to quarantine folder
	}
	return true;
}

inline LavaScan::LavaScan() {
	CurrentScanCount = 0;
	currentScanGoing = "";
	num_found = 0; num_removed = 0;
	isScanDone = false;
	start_time = std::string("");
	finish_time = std::string("");
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
		printf("ClamAV engine ready!\n");
	}

	//printf("\n\n Testing scanFile and scanDirectory functions:\n\n");
	//Testing scanFile function
	
	this->options.general = CL_SCAN_GENERAL_ALLMATCHES;

	//create the quarantine direcory if it doesn't exist
	make_quarantine_directory();

	//Testing Quarantine Functions
	//quarantine_file("C:\\Users\\dylan\\Downloads\\not_a_virus.txt", "too cool");
	//quarantine_file("C:\\Users\\dylan\\Downloads\\a_virus.txt", "trojan");
	//std::set<std::string> to_remove;
	//to_remove.insert("a_virus.txt");
	//remove_quarantined_files(to_remove);
	//std::set<struct q_entry> entries = read_quarantine_contents();
	//for (struct q_entry q :entries) {
		//std::cout << "File name: " + q.old_file_name + ", new name: " + q.new_file_name + ", origin directory: " + q.origin_directory + ", virus name: " + q.virus_name << std::endl;
	//}

	//std::string AntibodyFileLocation = "C:\\test.LavaAnti";
	////Testing scanDirectory function
	//bool clean = scanDirectory("C:\\Users\\tom\\Documents\\", engine, options, AntibodyFileLocation);
	//if (clean) printf("Directory is not infected");
	//else printf("Directory is not infected");

	//Testing quickScan function
	/*clean = quickScan(AntibodyFileLocation, engine, options);
	if (clean) printf("Directory is not infected");
	else printf("Directory is not infected");*/

	// have a seperate thread load shit into PreviousScans...race condition if many scans on a pc maybe...we will see
	std::thread t1 = std::thread([this] {this->UpdatePreviousScans(); });
	t1.detach();
}

#endif