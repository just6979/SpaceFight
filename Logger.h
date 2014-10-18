#ifndef LOGGER_H
#define LOGGER_H

#include <iostream>
#include <fstream>
#include <string>

using namespace std;

class Logger
{
public:
	Logger(const string&);
	~Logger();
	void debug(const string&);
	void info(const string&);
	void warning(const string&);
	void error(const string&);
protected:
	void logIt(const string&);
private:
	string logFilename;
	ofstream logFile;
	string s;
};

#endif // LOGGER_H
