#ifndef LOGGER_H
#define LOGGER_H

namespace logger {
	/** newline is automatically emitted after the message */
	void info(char* format, ...);
	void error(char* format, ...);
}

#endif