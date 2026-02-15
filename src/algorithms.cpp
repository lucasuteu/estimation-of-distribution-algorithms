#include "individual.hpp"
#include "frequency.hpp"
#include "algorithms.hpp"
#include "history.hpp"

using namespace std;

Decision sig(double p, History_s& H, double eps) {

    if (p == 0.5 || p == 1.0/H.n) {
        if (H.ones() >= H.m * p + eps * max(sqrt(H.m * p * log(H.n)), log(H.n))) return up;
    }
    if (p == 0.5 || p == 1 - 1.0/H.n) {
        if (H.zeroes() >= H.m * (1-p) + eps * max(sqrt(H.m * (1-p) * log(H.n)), log(H.n))) return down;
    }

    return stay;
}


pair<int, size_t> sig_cGA(size_t n, const Fitness& f, Termination termination, double eps) {
    
    Frequency p(n, 0.5);
    

    vector<History_o> H;
    H.reserve(n);
    for (size_t i = 0; i < n; ++i) {
        H.emplace_back(n);
    }

    Individual x1 = p.create();
    Individual x2 = p.create();

    int max_score = f(x1);
    size_t t = 0;
    size_t up_flips = 0;
    size_t down_flips = 0;
    while (!termination(x1, f, t)) {
        x1 = p.create();
        x2 = p.create();

        if (f(x1) < f(x2)) swap(x1, x2);

        max_score = max(max_score, f(x1));


        for (size_t i = 0; i < n; ++i) {
            H[i].add(x1.get(i));

            List* L = H[i].L;

            History_s prefix;
            prefix.n = n;

            while (L != nullptr) {
                prefix.m += L->H.m;
                prefix.nb_ones += L->H.nb_ones;
                Decision s = sig(p.get(i), prefix, eps);
                if (s == up) {
                    p.set(i, 1 - 1.0/n);
                    H[i] = History_o(n);
                    ++up_flips;
                    break;
                } else if (s == down) {
                    p.set(i, 1.0/n);
                    H[i] = History_o(n);
                    ++down_flips;
                    break;
                }

                L = L->next;
            }

        }

        ++t;
    }

    cout << "sig_cGA flips: up=" << up_flips << ", down=" << down_flips << "\n";

    return {max_score, t};

}


pair<int, size_t> cGA(size_t n, const Fitness& f, Termination termination, double K) {

    Frequency p(n, 0.5);

    size_t t = 0;
    Individual x1 = p.create();
    Individual x2 = p.create();
    int max_score = f(x1);
    while (!termination(x1, f, t)) {
        x1 = p.create();
        x2 = p.create();

        if (f(x1) < f(x2)) swap(x1, x2);
        
        max_score = max(max_score, f(x1));
        
        for (size_t i = 0; i < n; ++i) {
            p.set(i, p.get(i) + (x1.get(i) - x2.get(i)) / K);
        }

        ++t;
    
    }


    return {max_score, t};
}


pair<int, size_t> EA1_1(size_t n, const Fitness& f, Termination termination) {

    Individual x(n);

    for (size_t i = 0; i < n; ++i) {
        x.set(i, (rand01() > 0.5));
    }
    size_t t = 0;
    int max_score = f(x);
    while (!termination(x, f, t)) {
        Individual offspring = x.mutate();

        if (f(x) < f(offspring)) swap(x, offspring);

        max_score = max(max_score, f(x));
        ++t;
    }


    return {max_score, t};
}

int OneMax(Individual& x) {
    return x.nb_ones;
}

int LeadingOnes(Individual& x) {

    size_t i = 0;
    while (i < x.n && x.get(i) == 1) {
        ++i;
    }

    return i;
}

int Jump(Individual& x, int k) { // Modified so that max jump = n
    if (x.nb_ones <= x.n - k || x.nb_ones == x.n) return x.nb_ones;
    return x.n - k - x.nb_ones;
}

int OnAndOff(Individual& x) {
    int score = 0;

    for (size_t i = 0; i+1 < x.n; ++i) {
        score += (x.get(i) == true && x.get(i+1) == false);
        score += (x.get(x.n-(i+1)) == true && x.get(x.n - (i+2)) == false);
    }

    return score + 1; // The +1 is there to make it return n if max score achieved.
}

bool term(Individual& x, const Fitness& f, size_t t, size_t T) {
    if (t >= T) return true;
    if (f(x) == (int) x.n) return true;
    return false;
}
