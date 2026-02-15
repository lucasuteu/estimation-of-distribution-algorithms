#include "algorithms.hpp"
#include <fstream>
#include <filesystem>

using namespace std;

void write_csv(const string& path, const vector<pair<int, size_t>>& rows) {
    ofstream out(path);
    for (const auto& row : rows) {
        out << row.first << "," << row.second << "\n";
    }
}

string make_name(const string& algo, const string& fitness, size_t T, size_t n, size_t M,
                 const string& extra) {
    return algo + "_fit-" + fitness +
           "_T-" + to_string(T) +
           "_n-" + to_string(n) +
           "_M-" + to_string(M) +
           extra + ".csv";
}

int main() {
    string fitness_name = "LeadingOnes";
    Fitness fitness = LeadingOnes; //[](Individual& x) {return Jump(x, 5);};
    const string out_dir = "data";

    std::filesystem::create_directories(out_dir);

    struct SigCase {
        size_t T;
        size_t n;
        size_t M;
        double eps;
    };
    struct CgaCase {
        size_t T;
        size_t n;
        size_t M;
        double K;
    };
    struct EaCase {
        size_t T;
        size_t n;
        size_t M;
    };

    const vector<SigCase> sig_cases = {
        // {100000, 100, 100, 0.5},
        // {100000, 100, 100, 1.0},
        // {100000, 100, 100, 2.0},
        // {100000, 100, 100, 4.0},
        // {100000, 100, 100, 13.0},
        {10000, 100, 100, 1.5},
        // {10000, 100, 100, 1.325},
        // {10000, 100, 100, 1.75},
        {10000, 100, 100, 0.5},
        {10000, 100, 100, 1.0},
        {10000, 100, 100, 2.0},
        {10000, 100, 100, 4.0},
        // {10000, 100, 100, 13.0},
        // {1000, 500, 500, 0.5},
        // {1000, 500, 500, 0.625},
        // {1000, 500, 500, 0.75},
        // {1000, 500, 500, 0.825},
        // {1000, 500, 500, 1.0},
        // {1000, 500, 500, 1.1},
        // {4000, 250, 250, 0.5},
        // {4000, 250, 250, 0.75},
        // {4000, 250, 250, 1.0},
        // {4000, 250, 250, 2.0}
    };
    const vector<CgaCase> cga_cases = {
        {10000, 100, 100, sqrt(100.0)*log(100.0)},
        {10000, 100, 100, 100.0},
        // {10000, 100, 100, sqrt(100.0)*log(100.0)},
        // {1000, 500, 500, 500.0},
        // {1000, 500, 500, sqrt(500.0)*log(500.0)},
        // {4000, 250, 250, 250.0},
        // {4000, 250, 250, sqrt(250.0)*log(250.0)},

    };
    const vector<EaCase> ea_cases = {
        // {100000, 100, 100},
        {10000, 100, 100},
        // {1000, 500, 500},
        // {4000, 250, 250}
    };

    for (const auto& c : sig_cases) {
        cout << "Started sig_cGA : T=" << c.T << ", n=" << c.n << ", M=" << c.M
             << ", eps=" << c.eps << endl;
        Termination termination = [c](Individual& x, const Fitness& f, size_t t) {
            return term(x, f, t, c.T);
        };
        string sig_cga_path = out_dir + "/" +
            make_name("sig_cga_test", fitness_name, c.T, c.n, c.M, "_eps-" + to_string(c.eps));
        vector<pair<int, size_t>> sig_cga_results;
        sig_cga_results.reserve(c.M);
        for (size_t i = 0; i < c.M; ++i) {
            if (i%10 == 0) cout << "Step " << i << endl;
            sig_cga_results.push_back(sig_cGA(c.n, fitness, termination, c.eps));
        }
        write_csv(sig_cga_path, sig_cga_results);
    }

    for (const auto& c : cga_cases) {
        cout << "Started cGA : T=" << c.T << ", n=" << c.n << ", M=" << c.M
             << ", K=" << c.K << endl;
        Termination termination = [c](Individual& x, const Fitness& f, size_t t) {
            return term(x, f, t, c.T);
        };
        string cga_path = out_dir + "/" +
            make_name("cga", fitness_name, c.T, c.n, c.M, "_K-" + to_string(c.K));
        if (std::filesystem::exists(cga_path)) {
            cout << "Skipping (already exists): " << cga_path << endl;
            continue;
        }
        vector<pair<int, size_t>> cga_results;
        cga_results.reserve(c.M);
        for (size_t i = 0; i < c.M; ++i) {
            if (i % 10 == 0) cout << "Step " << i << endl;
            cga_results.push_back(cGA(c.n, fitness, termination, c.K));
        }
        write_csv(cga_path, cga_results);
    }

    for (const auto& c : ea_cases) {
        cout << "Started EA(1,1) : T=" << c.T << ", n=" << c.n << ", M=" << c.M << endl;
        Termination termination = [c](Individual& x, const Fitness& f, size_t t) {
            return term(x, f, t, c.T);
        };
        string ea_path = out_dir + "/" +
            make_name("ea1_1", fitness_name, c.T, c.n, c.M, "");
        if (std::filesystem::exists(ea_path)) {
            cout << "Skipping (already exists): " << ea_path << endl;
            continue;
        }
        vector<pair<int, size_t>> ea_results;
        ea_results.reserve(c.M);
        for (size_t i = 0; i < c.M; ++i) {
            if (i % 10 == 0) cout << "Step " << i << endl;
            ea_results.push_back(EA1_1(c.n, fitness, termination));
        }
        write_csv(ea_path, ea_results);
    }

    return 0;
}
