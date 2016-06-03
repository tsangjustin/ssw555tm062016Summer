#ifndef _FAM_H
#define _FAM_H
#endif

class Indi {
    private:
        string name;
        bool sex;
        int * birth;
        int * death;
        Fam * famc;
        Fam * fams;
    public:
        // Setters:
        void set_name (string n) {
            this->name = n;
        };
        void set_sex (bool s) {
            this->sex = s;
        };
        void set_birth (int d, int m, int y) {
            int arr[] = {d,m,y};
            this->birth = arr;
        };
        void set_death (int d, int m, int y) {
            int arr[] = {d,m,y};
            this->death = arr;
        };
        void set_famc (Fam f){
            this->famc = f;
        };
        void set_fams (Fam f) {
            this->fams = f;
        };
        
        // Getters:
        string get_name () {
            return this->name;
        };
        bool get_sex () {
            return this->sex;
        };
        int * get_birth () {
            return this->birth;
        };
        int * get_death () {
            return this->death;
        };
        Fam * get_famc () {
            return this->famc;
        };
        Fam * get_fams () {
            return this->fams;
        };
};
