#include "Logger.h"

Logger::Logger(const string& filename)
{
	logFilename = filename;
	logFile.open(logFilename, ios_base::trunc);
	setLevel(INFO);
	debug((string("Opening ") += logFilename) += " for logging.");
}

Logger::~Logger()
{
	debug((string("Closing log file ") += logFilename) += ".");
	logFile.close();
}

void Logger::setLevel(levels newLevel)
{
	logLevel = newLevel;
}

void Logger::logIt(const string& msg)
{
	cout << msg << endl;
	logFile << msg << endl;
}

void Logger::debug(const string& msg)
{
#ifdef DEBUG
	if(logLevel <= DEBUG) {
		logIt(string("DEBUG: ") += msg);
	}
#endif
}

void Logger::info(const string& msg)
{
	if(logLevel <= INFO) {
		logIt(string("INFO: ") += msg);
	}
}

void Logger::warning(const string& msg)
{
	if(logLevel <= WARNING) {
		logIt(string("WARNING: ") += msg);
	}
}

void Logger::error(const string& msg)
{
	if(logLevel <= ERROR) {
		logIt(string("ERROR: ") += msg);
	}
}
