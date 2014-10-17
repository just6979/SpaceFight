#include "Logger.h"

Logger::Logger(char* filename)
{
	logFile.open(filename, ios_base::trunc);
}

Logger::~Logger()
{
	logFile.close();
}

void Logger::info(string msg) {
	cout << msg << endl;
	logFile << msg << endl;
}
