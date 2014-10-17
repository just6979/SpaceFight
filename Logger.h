#ifndef LOGGER_H
#define LOGGER_H

#include <iostream>
#include <fstream>
#include <string>

using namespace std;

class Logger
{
	public:
		Logger(char*);
		~Logger();
		void info(string);
	protected:
	private:
		ofstream logFile;
};

#endif // LOGGER_H
