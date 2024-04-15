#include "log.h"
#include <fstream>
#include <chrono>
#include <iomanip>

void samurai::LogManager::init(std::string name)
{

	this->name = name;
	bool firstLog = 0;
	
}

void samurai::LogManager::log(const char *l, int type)
{

#ifdef PIKA_DEVELOPMENT
	logInternally(l, type);
	logToFile(l, type);
#endif


}

std::stringstream formatLog(const char *l, int type)
{
	auto time = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
	std::stringstream s;
	s << "#" << std::put_time(std::localtime(&time), "%Y-%m-%d %X");
	
	if (type == samurai::logWarning)
	{
		s << "[warning]";
	}
	else if (type == samurai::logError)
	{
		s << "[error]";
	}
	
	s << ": ";
	s << l << "\n";
	return s;
}

void samurai::LogManager::logToFile(const char *l, int type)
{
	//todo unlickely atribute
	if (!firstLog)
	{
		firstLog = 1;
		std::ofstream file(name);	//no need to check here
		file.close(); //clear the file content
	}

	samurai::logToFile(name.c_str(), l, type);
}

void samurai::LogManager::logInternally(const char *l, int type)
{
	if (internalLogs.size() >= maxInternalLogCount)
	{
		internalLogs.pop_front();
	}

	internalLogs.push_back(formatLog(l, type).str());
}


void samurai::logToFile(const char *fileName, const char *l, int type)
{
	std::ofstream file(fileName, std::ofstream::app);
	if (!file.is_open()) { return; }

	file << formatLog(l, type).rdbuf();
	file.close();
}
