#include <iostream>
#include <vector>
#include <string>
#include <cmath> // for sin, cos
#include <limits> // for INFINITY
#include "correlation.hpp" // for any further defs, although I can't think of any

#if defined(__MINGW64__) || defined(__MINGW_32)
#include "mingw-std-threads/mingw.thread.h"
#include "mingw-std-threads/mingw.mutex.h"
#pragma message("Using mingw-std-threads for std::mutex and std::thread")
#else
#include <thread>
#include <mutex>
#endif


using namespace std;

// Correlation example where C++11 additions are used.
std::mutex g_mutex;
uint16_t g_thread_finished;
std::vector<double> g_x, g_y;
double g_peak_value = -INFINITY;
int g_peak_offset = -1;
bool g_error = false;


auto correlation = [](const std::vector<double> x, const std::vector<double> y, int length, int offset, double& result) -> bool { // lambda function
    if((length < 1) || (offset >= length))
        return false;
    result = 0.0;
    for(auto i=offset; i<length-offset; i++) // automatic type assignment; couldn't think of a way to use range unfortunately
        result += x.at(i-offset) * y.at(i);
    return true;
};

void from_thread(int pid, int start, int end) {
    for(int i=start; i<=end; i++) {
        double result = 0.0;

        if(correlation(g_x, g_y, g_x.size(), i, result)) {
            if(result > g_peak_value) {
                g_mutex.lock();
                g_peak_value = result;
                g_peak_offset = i;
                g_mutex.unlock();
            }
        } else {
            cout << "Error calculating correlation" << endl;
            g_error = true;
        }
    }
    g_thread_finished |= 1 << pid;
}

int main()
{
    static_assert(__cplusplus > 201103L, "Minimum requirement missing: C++11.");

    // Initialize vectors
    for(int i=0; i<360; i++) {
        g_x.push_back(sin(i*M_PI/180));
        g_y.push_back(cos(i*M_PI/180)); // could also use sin(((i+180)%360)*M_PI/180)
    }

    int grain = 360/MAX_THREADS;
    std::vector<std::thread> threads(MAX_THREADS);
    for(int i=0; i<MAX_THREADS; i++) {
        threads[i] = std::thread(from_thread, i, i*grain, ((i+1)*grain)-1);
        threads[i].join();

        // This allows us to see what's happening in the watch. Alternatively: std::thread(from_thread, i, i*grain, ((i+1)*grain)-1).join();
    }
    

    while(g_thread_finished < ((1<<MAX_THREADS)-1))
        if(g_error)
            return -1;
        else
            cout << "Still calculating" << endl;

    cout << "Correlation value / offset: " << g_peak_value << " / " << g_peak_offset << endl;
    return g_peak_offset;
}