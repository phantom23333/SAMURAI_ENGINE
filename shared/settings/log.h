#pragma once
#include <string>
#include <deque>
#include <sstream>

namespace samurai
{
	enum logType
	{
		logNormal = 0,
		logWarning = 1,
		logError = 2,
	};

	struct LogManager
	{

		static constexpr const char *DefaultLogFile = SAMURAI_RESOURCES_PATH "logs.txt";

		//a null name will just log to a internal structure
		void init(std::string name);

		

		//this will be dependent on the configuration of the project. 
		void log(const char *l, int type = samurai::logNormal);
		

		std::string name = "";
		bool firstLog = 0;

		std::deque<std::string> internalLogs;
		static constexpr int maxInternalLogCount = 200;


	private:
		//used only interally.
		void logToFile(const char *l, int type = samurai::logNormal);
		void logInternally(const char *l, int type = samurai::logNormal);

	};


	void logToFile(const char *fileName, const char *l, int type = samurai::logNormal);


}