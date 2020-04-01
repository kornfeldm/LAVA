#pragma once
#include <iostream>
#include <string>
#include <fstream> //for working with file
#include <time.h>
#include <windows.h>
#include <stack>

void log_scan(std::string start, std::string finish, int found, int removed);
void read_log();
std::string get_time();

void log_scan(std::string start, std::string finish, int found, int removed) {
	std::fstream log_file;
	log_file.open("scan_log.lava", std::ios::app);
	log_file << start + "," + finish + "," + std::to_string(found) + "," + std::to_string(removed) + "\n";
	log_file.close();
}

std::string get_time() {
	time_t rawtime;
	struct tm timeinfo;
	char buffer[40];

	time(&rawtime);
	localtime_s(&timeinfo, &rawtime);

	strftime(buffer, 40, "%c", &timeinfo);
	return (std::string)buffer;
}

void read_log() {
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