#include "individual.hpp"

using namespace std;


int random_int(int a, int b) {
    if (a == b) return a;
    static mt19937 gen(random_device{}());
    uniform_int_distribution<int> dist(a, b-1);
    return dist(gen);
}

double rand01() {
    static std::mt19937 gen(std::random_device{}());
    static std::uniform_real_distribution<double> dist(0.0, 1.0);
    return dist(gen);   // in [0,1)
}

Individual::Individual(size_t n) {
    this->data = vector<bool>(n, false);
    this->n = n;

}

bool Individual::get(size_t i) const {
    return this->data[i];
}


void Individual::set(size_t i, bool b) {

    if (this->data[i] == b) return;

    if (this->data[i] == true) this->nb_ones--;
    else this->nb_ones++;

    this->data[i] = b;
}
void Individual::flip(std::size_t i) {
    this->set(i, !this->data[i]);
}

MutationArrays& init_mutation(size_t n) {
    static bool initialized = false;
    static size_t cached_n = 0;
    static MutationArrays arrays;

    if (!initialized || cached_n != n) {
        cached_n = n;
        arrays.permutation.resize(n);
        for (size_t i = 0; i < n; ++i) arrays.permutation[i] = i;

        arrays.binomial_cdf.resize(n + 1);
        arrays.binomial_cdf[0] = pow(1.0 - 1.0 / n, n);
        for (size_t k = 1; k < n + 1; ++k) {
            arrays.binomial_cdf[k] =
                arrays.binomial_cdf[k - 1] * (n - k) / (k + 1) / n / (1 - 1.0 / n);
        }
        for (size_t i = 1; i < n + 1; ++i) {
            arrays.binomial_cdf[i] += arrays.binomial_cdf[i - 1];
        }

        initialized = true;
    }

    return arrays;
}


Individual Individual::mutate() {
    MutationArrays& arrays = init_mutation(n);

    Individual mutant(n);
    mutant.data = this->data;
    mutant.nb_ones = this->nb_ones;

    double k_prob = rand01();
    size_t K = n;

    for (size_t k = 0; k < n + 1; ++k) { // Only runs K+1 times so O(K)
        if (k_prob <= arrays.binomial_cdf[k]) {
            K = k;
            break;
        }
    }

    for (size_t k = 0; k < K; ++k) {
        size_t i = random_int(static_cast<int>(k), static_cast<int>(n)); // [k, n)
        swap(arrays.permutation[i], arrays.permutation[k]);
        mutant.flip(arrays.permutation[k]);
    }

    return mutant;
}