#include "util.h"

#include <cstdarg>
#include <cstdio>

extern chrono::time_point<chrono::high_resolution_clock> program_start_time;

float getElapsedTime(chrono::time_point<chrono::high_resolution_clock> time) {
    return (chrono::high_resolution_clock::now() - time).count() * 1e-9;
}

void LogWithTime(const char * format, ...) {
    float elapsed_time = getElapsedTime( program_start_time );
    printf("[%6.1f] ", elapsed_time);
    va_list args;
    va_start(args, format);
    vprintf(format, args);
    va_end(args);
}

