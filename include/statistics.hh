#ifndef STATISTICS_HH
#define STATISTICS_HH 1

#include <cmath>
#include <iterator>

template<typename _IterTp>
double mean(_IterTp begin, _IterTp end)
{
    double sum = 0;
    double num = std::distance(begin, end);
    while(begin != end) {
        sum += *begin++;
    }
    return sum/num;
}

template<typename _IterTp>
double stdev(_IterTp begin, _IterTp end)
{
    double avg = mean(begin, end);
    double sum = 0;
    double num = std::distance(begin, end);
    while(begin != end) sum += pow((*begin++ - avg),2);
    sum /= num;
    return sqrt(sum);
}

#endif
