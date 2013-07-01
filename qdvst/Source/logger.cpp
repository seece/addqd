#include <iostream>
#include <cstdarg>
#include "logger.h"

void logger::info(char* format, ...)
{
	fprintf(stderr, "INFO: ");
	va_list argptr;
    va_start(argptr, format);
    vfprintf(stderr, format, argptr);
    va_end(argptr);
	puts("");
}

void logger::error(char* format, ...)
{
	fprintf(stderr, "ERROR: ");
	va_list argptr;
    va_start(argptr, format);
    vfprintf(stderr, format, argptr);
    va_end(argptr);
	puts("");
}