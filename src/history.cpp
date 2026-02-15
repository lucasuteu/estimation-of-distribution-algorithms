#include "history.hpp"

using namespace std;

size_t History_s::ones() const {
    return this->nb_ones;
}

size_t History_s::zeroes() const {
    return this->m - this->ones();
}

void History_s::add(bool b) {
    this->m++;
    this->nb_ones += b;
}

List::List(size_t n) {
    this->H.n = n;
}

size_t List::ones() const {
    return this->H.ones();
}

size_t List::zeroes() const {
    return this->H.m - this->ones();
}

void List::add(bool b) {
    this->H.add(b);
}

void List::consolidate() {
    List* curr = this;
    List* next = this->next;
    bool alreadySeenDouble = false;

    while (next != nullptr) {
        if (curr->H.m == next->H.m) {
            if (alreadySeenDouble) {
                curr->H.m += next->H.m;
                curr->H.nb_ones += next->H.nb_ones;
                curr->next = next->next;
                delete next;          
                next = curr->next;       
                alreadySeenDouble = false;
                continue;
            } else {
                alreadySeenDouble = true;
                curr = next;
            }
        } else {
            alreadySeenDouble = false;
            curr = next;
        }
        next = next->next;
    }

}

History_o::History_o(size_t n) {
    this->n = n;
    this->lnn = log(n);
    this->L = new List(n);
}

History_o::~History_o() {
    while (L) {
        List* next = L->next;
        delete L;
        L = next;
    }
}

History_o::History_o(History_o&& other) noexcept
    : m(other.m), n(other.n), lnn(other.lnn), L(other.L) {
    other.L = nullptr;
}

History_o& History_o::operator=(History_o&& other) noexcept {
    if (this != &other) {
        while (L) {
            List* next = L->next;
            delete L;
            L = next;
        }
        m = other.m;
        n = other.n;
        lnn = other.lnn;
        L = other.L;
        other.L = nullptr;
    }
    return *this;
}


void History_o::add(bool b) {
    
    if (this->L->H.m < 1) { //this->lnn) {
        this->L->add(b);
    } else {
        List* head = new List(this->n);
        head->add(b);
        head->next = this->L;
        this->L = head;
        this->L->consolidate();
    }
}
