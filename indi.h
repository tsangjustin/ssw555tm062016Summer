#ifndef INDI_H
#define INDI_H

#include "fam.h"
#include <string>
#include <vector>

class Fam;

class Indi {
    private:
        std::string uniqueID_;
        std::string name_;
        bool sex_;
        int birth_[3];
        int death_[3];
        std::vector< int > famc_;
        std::vector< int > fams_;
    public:
        Indi() {
            uniqueID_ = "";
            name_ = "";
            sex_ = true;
            famc_.clear();
            fams_.clear();
            for (int i = 0; i < 3; ++i) {
                birth_[i] = 0;
                death_[i] = 0;
            }
        }

        // Setters:
        inline void set_id (std::string n) {
            this->uniqueID_ = n;
        }
        inline void set_name (std::string n) {
            this->name_ = n;
        }
        inline void set_sex (bool s) {
            this->sex_ = s;
        }
        void set_birth (int d, int m, int y) {
            this->birth_[0] = d;
            this->birth_[1] = m;
            this->birth_[2] = y;
        }
        void set_death (int d, int m, int y) {
            this->death_[0] = d;
            this->death_[1] = m;
            this->death_[2] = y;
        }
        inline void add_famc (int f){
            (this->famc_).push_back(f);
        }
        inline void add_fams (int f) {
            (this->fams_).push_back(f);
        }
        
        // Getters:
        inline std::string get_id () {
            return this->uniqueID_;
        }
        inline std::string get_name () {
            return this->name_;
        }
        inline bool get_sex () {
            return this->sex_;
        }
        inline int* get_birth () {
            return this->birth_;
        }
        inline int* get_death () {
            return this->death_;
        }
        inline std::vector< int > get_famc () {
            return this->famc_;
        }
        inline std::vector< int > get_fams () {
            return this->fams_;
        }

        bool checkFamC(int famID) {
            for (std::vector<int>::iterator it = famc_.begin(); it != famc_.end(); ++it) {
                if (*it == famID) {
                    return true;
                }
            }
            return false;
        }

        bool checkFamS(int famID) {
            for (std::vector<int>::iterator it = fams_.begin(); it != fams_.end(); ++it) {
                if (*it == famID) {
                    return true;
                }
            }
            return false;
        }
};

#endif /* Indi */