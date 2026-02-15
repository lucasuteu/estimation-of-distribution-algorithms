#include "frequency.hpp"

using namespace std;


Frequency::Frequency(size_t n) {
    this->n = n;
    this->data = vector<double>(n, 1.0/n);
}
Frequency::Frequency(size_t n, double p) {
    this->n = n;
    this->data = vector<double>(n, max(min(p, 1-1.0/n), 1.0/n));
}

double Frequency::get(size_t i) const {
    return this->data[i];
}
void Frequency::set(size_t i, double p) {
    this->data[i] = max(min(p, 1-1.0/n), 1.0/n);
}

Individual Frequency::create() const {
    Individual x(this->n);

    for (size_t i = 0; i < this->n; ++i) {

        if (rand01() < this->data[i]) x.set(i, true);

    }
    return x;

}