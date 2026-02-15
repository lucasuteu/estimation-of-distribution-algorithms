#ifndef HISTORY_HPP
#define HISTORY_HPP

#include <bits/stdc++.h>


struct History_s {

    std::size_t m = 0;
    std::size_t n = 0;
    std::size_t nb_ones = 0;
    
    std::size_t ones() const;
    std::size_t zeroes() const;
    void add(bool b);

};

struct List {
    
    History_s H;

    List* next = nullptr;
    
    std::size_t ones() const;
    std::size_t zeroes() const;
    void add(bool b);
    void consolidate();

    List(std::size_t n);
};

struct History_o {

    std::size_t m = 0;
    std::size_t n;
    double lnn;

    History_o(std::size_t n);
    ~History_o();
    
    History_o(const History_o&) = delete;
    History_o& operator=(const History_o&) = delete;
    History_o(History_o&& other) noexcept;
    History_o& operator=(History_o&& other) noexcept;

    List* L;


    void add(bool b);
};


#endif // HISTORY_HPP