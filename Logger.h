#ifndef LOGGER_H
#define LOGGER_H

/*	Stupid simple console & file logging.

	Create an instance of Logger with a filename, then just use the public
	member function to output various levels. Message appears on stdout
	and in the file.

	Logger::debug() compiles out with DEBUG macro unset.
*/

#include <iostream>
#include <fstream>
#include <string>

using namespace std;

class Logger
{
public:
	typedef enum {
		DEBUG,
		INFO,
		WARNING,
		ERROR
	} levels;
	Logger(const string&);
	~Logger();
	void setLevel(levels);
	void debug(const string&);
	void info(const string&);
	void warning(const string&);
	void error(const string&);
protected:
	void logIt(const string&);
private:
	string logFilename;
	ofstream logFile;
	int logLevel;
	string s;
};

#endif // LOGGER_H
