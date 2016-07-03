#ifndef FAM_H
#define FAM_H

#include "indi.h"
#include <vector>
#include <string>

class Indi;

class Fam {
private:
    std::string uniqueID_;
    int husb_;
    int wife_;
    std::vector< int > chil_;
    int marr_[3];
    int div_[3];
public:
    Fam() {
        uniqueID_ = "";
        husb_ = -1;
        wife_ = -1;
        chil_.clear();
        for (int i = 0; i < 3; ++i) {
            marr_[i] = 0;
            div_[i] = 0;
        }
    }
    // Setters:
    inline void set_id (std::string n) {
        this->uniqueID_ = n;
    }
    void set_marr (int d, int m, int y) {
        this->marr_[0] = d;
        this->marr_[1] = m;
        this->marr_[2] = y;
    }
    inline void set_husb (int h) {
        this->husb_ = h;
    }
    inline void set_wife (int w) {
        this->wife_ = w;
    }
    void set_div (int d, int m, int y) {
        this->div_[0] = d;
        this->div_[1] = m;
        this->div_[2] = y;
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
    inline int * get_div () {
        return this->div_;
    }
    // Other functions
    inline void add_chil (int c) {
        (this->chil_).push_back(c);
    }

    bool checkChild(int childID) {
        for (std::vector<int>::iterator it = chil_.begin(); it != chil_.end(); ++it) {
            if (*it == childID) {
                return true;
            }
        }
        return false;
    }
};

#endif /* Fam */
