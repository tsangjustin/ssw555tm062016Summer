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
        int* birth_;
        int* death_;
        std::vector< int > famc_;
        std::vector< int > fams_;
    public:
        Indi() {
            uniqueID_ = "";
            name_ = "";
            sex_ = true;
            famc_.clear();
            fams_.clear();
            birth_ = new int[3];
            death_ = new int[3];
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
            int arr[] = {d,m,y};
            this->birth_ = arr;
        }
        void set_death (int d, int m, int y) {
            int arr[] = {d,m,y};
            this->death_ = arr;
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
        inline int * get_death () {
            return this->death_;
        }
        inline std::vector< int > get_famc () {
            return this->famc_;
        }
        inline std::vector< int > get_fams () {
            return this->fams_;
        }
};

#endif /* Indi */