#ifndef INDI_H
#define INDI_H

#include "fam.h"
#include <string>
#include <vector>

class Fam;

class Indi {
    private:
        std::string name;
        bool sex;
        int* birth;
        int* death;
        int famc;
        std::vector< int > fams;
    public:
        Indi() {
            name = "";
            sex = true;
            birth = new int[3];
            death = new int[3];
        }

        // Setters:
        inline void set_name (std::string n) {
            this->name = n;
        }
        inline void set_sex (bool s) {
            this->sex = s;
        }
        void set_birth (int d, int m, int y) {
            int arr[] = {d,m,y};
            this->birth = arr;
        }
        void set_death (int d, int m, int y) {
            int arr[] = {d,m,y};
            this->death = arr;
        }
        inline void set_famc (int f){
            this->famc = f;
        }
        inline void add_fams (int f) {
            (this->fams).push_back(f);
        }
        
        // Getters:
        inline std::string get_name () {
            return this->name;
        }
        inline bool get_sex () {
            return this->sex;
        }
        inline int * get_birth () {
            return this->birth;
        }
        inline int * get_death () {
            return this->death;
        }
        inline int get_famc () {
            return this->famc;
        }
        inline std::vector< int > get_fams () {
            return this->fams;
        }
};

#endif /* Indi */