#ifndef FAM_H
#define FAM_H

#include "indi.h"
#include <vector>

class Indi;

class Fam {
private:
    int * marr_;
    int husb_;
    int wife_;
    std::vector< int > chil_;
    int * div_;
public:
    Fam() {
        marr_ = new int[3];
        div_ = new int[3];
    }
    // Setters:
    void set_marr (int d, int m, int y) {
        int arr[] = {d,m,y};
        this->marr_ = arr;
    }
    void set_husb (int h) {
        this->husb_ = h;
    }
    void set_wife (int w) {
        this->wife_ = w;
    }
    // void set_chil (std::vector<Indi> c) {
    //     this->chil = c;
    // }
    void set_div (int d, int m, int y) {
        int arr[] = {d,m,y};
        this->div_ = arr;
    }
    // Getters:
    int * get_marr () {
        return this->marr_;
    }
    inline int get_husb () {
        return this->husb_;
    }
    inline int get_wife () {
        return this->wife_;
    }
    std::vector< int > get_chil () {
        return this->chil_;
    }
    int * get_div () {
        return this->div_;
    }
    // Other functions
    void add_chil (int c) {
        (this->chil_).push_back(c);
    }
};

#endif /* Fam */
