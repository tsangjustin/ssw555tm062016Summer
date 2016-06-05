#ifndef FAM_H
#define FAM_H

#include "indi.h"
#include <vector>

class Indi;

class Fam {
private:
    int * marr;
    Indi * husb;
    Indi * wife;
    std::vector< Indi* > chil;
    int * div;
public:
    
    
    // Setters:
    void set_marr (int d, int m, int y) {
        int arr[] = {d,m,y};
        this->marr = arr;
    }
    void set_husb (Indi* h) {
        this->husb = h;
    }
    void set_wife (Indi* w) {
        this->wife = w;
    }
    // void set_chil (std::vector<Indi> c) {
    //     this->chil = c;
    // }
    void set_div (int d, int m, int y) {
        int arr[] = {d,m,y};
        this->div = arr;
    }
    // Getters:
    int * get_marr () {
        return this->marr;
    }
    inline Indi * get_husb () {
        return this->husb;
    }
    inline Indi * get_wife () {
        return this->wife;
    }
    std::vector< Indi* > get_chil () {
        return this->chil;
    }
    int * get_div () {
        return this->div;
    }
    // Other functions
    void add_chil (Indi* c) {
        chil.push_back(c);
    }
};

#endif /* Fam */
