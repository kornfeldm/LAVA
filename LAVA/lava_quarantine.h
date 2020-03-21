#pragma once
#include <iostream>;
#include <string>;
#include <fstream> //for working with file
#include <stdio.h> //for removing files

void printQuarantineContents();
void quarantineFile(std::string filepath);
bool removeQuarantinedFiles();
bool quarantineIsEmpty();

bool quarantineIsEmpty() { //checks whether quarantined file is empty
	std::fstream q_file;
	q_file.open("quarantine.lava", std::ios::in | std::ios::out);
	return q_file.peek() == std::ifstream::traits_type::eof();
}


void printQuarantineContents() { //prints the contents of the quarantine file
	std::fstream quarantineFile;
	quarantineFile.open("quarantine.lava", std::ios::in | std::ios::out);
	std::string line;
	while (getline(quarantineFile,line)) {
		std::cout << line << std::endl;
	}
	quarantineFile.close();
}

void quarantineFile(std::string filepath) { //quarantine file given filepath
	std::fstream quarantineFile;
	quarantineFile.open("quarantine.lava", std::ios::app);
	quarantineFile << filepath << std::endl;
	quarantineFile.close();
}

bool removeQuarantinedFiles() { //returns false if errors occurred
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