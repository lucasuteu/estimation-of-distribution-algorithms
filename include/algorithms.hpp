#ifndef ALGORITHMS_HPP
#define ALGORITHMS_HPP
#include "individual.hpp"
#include "history.hpp"


using Fitness = std::function<int(Individual&)>;
using Termination = std::function<bool(Individual&, const Fitness&, size_t)>;

enum Decision { up, stay, down };

Decision sig(double p, History_s& H, double eps);


std::pair<int, std::size_t> sig_cGA(std::size_t n, const Fitness& f, Termination termination, double eps);

std::pair<int, std::size_t> cGA(std::size_t n, const Fitness& f, Termination termination, double K);

std::pair<int, std::size_t> EA1_1(std::size_t n, const Fitness& f, Termination termination);

int OneMax(Individual& x);

int LeadingOnes(Individual& x);

int Jump(Individual& x, int k);
int OnAndOff(Individual& x);

bool term(Individual& x, const Fitness& f, std::size_t t, std::size_t T);


#endif // ALGORITHMS_HPP