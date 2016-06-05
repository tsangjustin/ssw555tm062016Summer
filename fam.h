#ifndef FAM_H
#define FAM_H

#include "indi.h"
#include <vector>
#include <string>

class Indi;

class Fam {
private:
    std::string uniqueID_;
    int * marr_;
    int husb_;
    int wife_;
    std::vector< int > chil_;
    int * div_;
public:
    Fam() {
        uniqueID_ = "";
        husb_ = -1;
        wife_ = -1;
        chil_.clear();
        marr_ = new int[3];
        div_ = new int[3];
    }
    // Setters:
    inline void set_id (std::string n) {
        this->uniqueID_ = n;
    }
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
    inline std::string get_id () {
        return this->uniqueID_;
    }
    inline int * get_marr () {
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
