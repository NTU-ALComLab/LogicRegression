#include <chrono>

using namespace std;

float getElapsedTime(chrono::time_point<chrono::high_resolution_clock> time);
void LogWithTime(const char * format, ...);
