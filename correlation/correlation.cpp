#include <iostream>
#include <vector>
#include <string>
#include <cmath> // for sin, cos
#include <limits> // for INFINITY
#include "correlation.hpp" // for any further defs, although I can't think of any

using namespace std;

// Correlation example where C++11 additions are used.

int main()
{
    static_assert(__cplusplus > 201103L, "Minimum requirement missing: C++11");
    double result = 0.0, peakValue = -INFINITY;

    auto correlation = [&result](const std::vector<double> x, const std::vector<double> y, int length, int offset) -> bool { // lambda function; would rather pass 
        if((length < 1) || (offset >= length))
            return false;
        result = 0.0;
        for(auto i=offset; i<length-offset; i++) // automatic type assignment; couldn't think of a way to use range unfortunately
            result += x.at(i-offset) * y.at(i);
        return true;
    };

    std::vector<double> x, y;

    for(int i=0; i<360; i++) {
        x.push_back(sin(i*M_PI/180));
        y.push_back(cos(i*M_PI/180)); // could also use sin(((i+180)%360)*M_PI/180)
    }

    int peakOffset = -1;

    for(int i=0; i<360; i++) {
        if(correlation(x, y, x.size(), i)) {
            if(result > peakValue) {
                peakValue = result;
                peakOffset = i;
            }
        } else {
            cout << "Error calculating correlation" << endl;
            return -1;
        }
    }
    
    cout << "Correlation value / offset: " << peakValue << " / " << peakOffset << endl;
    return peakOffset;
}