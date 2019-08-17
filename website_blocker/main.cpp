#define _CRT_SECURE_NO_WARNINGS

#include <ctime>	// time_t, struct tm, difftime, time, mktime
#include <vector>	// std::vector
#include <string>	// std::string
#include <iostream> // std::cout
#include <thread>	// std::this_thread::sleep_for
#include <chrono>	// std::chrono::milliseconds
#include <fstream>	// std::ofstream std::ifstream

#ifdef __linux__
#	define HOSTS "/etc/hosts"
#elif _WIN32
#	define HOSTS "C:\\Windows\\System32\\drivers\\etc\\hosts"
#else
#	define HOSTS "/etc/hosts"
#endif

bool time_lessthen( struct tm& less, struct tm& great ) 
{
	bool rtn = false;
	if (less.tm_year < great.tm_year)
		rtn = true;
	if (less.tm_mon < great.tm_mon)
		rtn = true;
	if (less.tm_mday < great.tm_mday)
		rtn = true;
	if (less.tm_hour < great.tm_hour)
		rtn = true;
	if (less.tm_min < great.tm_min)
		rtn = true;
	return rtn;
}

bool getFileContent( std::string fileName, std::vector<std::string> & vecOfStrs )
{
	std::ifstream in(fileName.c_str());

	if (!in)
	{
		std::cerr << "Cannot open the File : " << fileName << std::endl;
		return false;
	}

	std::string str;
	// Read the next line from File until it reaches the end.
	while (std::getline(in, str))
	{
		// Line contains string of length > 0 then save it in vector
		if (str.size() > 0)
			vecOfStrs.push_back(str);
	}
	//Close The File
	in.close();
	return true;
}

bool appendFileContent( std::string fileName, std::string line )
{
	std::ofstream out(fileName.c_str(), std::ios_base::app);

	if (!out)
	{
		std::cerr << "Cannot open the File : " << fileName << std::endl;
		return false;
	}

	out << line << std::endl;
	out.close();
	return true;
}

bool createFileContent( std::string fileName, std::vector<std::string> & vecOfStrs )
{
	std::ofstream out(fileName.c_str());

	if (!out)
	{
		std::cerr << "Cannot open the File : " << fileName << std::endl;
		return false;
	}

	for (std::vector<std::string>::iterator line = vecOfStrs.begin(); line != vecOfStrs.end(); ++line) {
		out << *line << std::endl;
	}

	out.close();
	return true;
}

int main()
{
	std::string host_test = "hosts";
	std::string host_path = HOSTS;
	std::string redirect = "127.0.0.1";

	// website to be blocked
	std::vector<std::string> website_list = { "www.facebook.com", "facebook.com", "mail.google.com" };
	std::vector<std::string> file_lines;

	time_t timer;
	time(&timer);
	struct tm workHoursStart = *localtime(&timer);
	struct tm workHoursEnd = *localtime(&timer);
	
	// blocking is in force between...
	workHoursStart.tm_hour = 9;
	workHoursEnd.tm_hour = 16;

	struct tm now;

	std::chrono::duration<int, std::milli> timespan(5000);

	while (true) {
		time(&timer);
		now = *localtime(&timer);
		if (time_lessthen(workHoursStart, now) && time_lessthen(now, workHoursEnd)) {
			std::cout << "Working hours..." << std::endl;
			file_lines.clear();
			getFileContent(host_path, file_lines);
			for (std::vector<std::string>::iterator website = website_list.begin(); website != website_list.end(); ++website) {
				bool found = false;
				for (std::vector<std::string>::iterator line = file_lines.begin(); line != file_lines.end(); ++line) {
					if (line->find(*website) != std::string::npos)
						found = true;
				}
				if (found == false) {
					std::string line = redirect + " " + *website;
					appendFileContent(host_path, line);
				}
			}
			

		}
		else {
			std::cout << "Not Working hours..." << std::endl;
			file_lines.clear();
			getFileContent(host_path, file_lines);
			size_t size = file_lines.size();
			for (std::vector<std::string>::iterator line = file_lines.begin(); line != file_lines.end(); ++line) {
				for (std::vector<std::string>::iterator website = website_list.begin(); website != website_list.end(); ++website) {
					if (line->find(*website) != std::string::npos) {
						std::vector<std::string>::iterator dlt = line--;
						file_lines.erase(dlt);
						break;
					}
				}
			}
			// Lets check that there was a change. if there is write the file.
			if (size > file_lines.size())
				createFileContent(host_path, file_lines);
		}
		
		std::this_thread::sleep_for(timespan);
	}
	
	return 0;
}