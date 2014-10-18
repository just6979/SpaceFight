#include "Logger.h"

Logger::Logger(const string& filename)
{
	logFilename = filename;
	logFile.open(logFilename, ios_base::trunc);
	info((string("Opening ") += logFilename) += " for logging.");
}

Logger::~Logger()
{
	info((string("Closing log file ") += logFilename) += ".");
	logFile.close();
}

void Logger::logIt(const string& msg)
{
	cout << msg << endl;
	logFile << msg << endl;
}

void Logger::debug(const string& msg)
{
	logIt(string("DEBUG: ") += msg);
}

void Logger::info(const string& msg)
{
	logIt(string("DEBUG: ") += msg);
}

void Logger::warning(const string& msg)
{
	s = "WARNING: ";
	s += msg;
	logIt(s);
}

void Logger::error(const string& msg)
{
	s = "ERROR: ";
	s += msg;
	logIt(s);
}
