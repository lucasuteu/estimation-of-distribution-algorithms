#ifndef FREQUENCY_HPP
#define FREQUENCY_HPP

#include "individual.hpp"

#include <random>

struct Frequency {

    std::size_t n;
    std::vector<double> data;

    Frequency(std::size_t n);
    Frequency(std::size_t n, double p);
    
    double get(std::size_t i) const;
    void set(std::size_t i, double p);

    Individual create() const;

};

#endif // FREQUENCY_HPP