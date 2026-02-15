#ifndef INDIVIDUAL_HPP
#define INDIVIDUAL_HPP

#include <bits/stdc++.h>

struct MutationArrays {
    
    std::vector<std::size_t> permutation;
    std::vector<double> binomial_cdf;
    
    MutationArrays& init_mutation(size_t n);
};

struct Individual {

    std::size_t n;
    std::size_t nb_ones = 0;
    std::vector<bool> data;
    bool mutation_state_ready = false;

    Individual(std::size_t n);
    
    bool get(std::size_t i) const;
    void set(std::size_t i, bool b);
    void flip(std::size_t i);

    Individual mutate();

};

double rand01();

#endif // INDIVIDUAL_HPP