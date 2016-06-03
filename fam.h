#ifndef _INDI_H
#define _INDI_H
#endif

class Fam {
    private:
        int * marr;
        Indi * husb;
        Indi * wife;
        vector <Indi> chil;
        int * div;
    public:
        // Setters:
        void set_marr (int d, int m, int y) {
            int arr[] = {d,m,y};
            this->marr = arr;
        };
        void set_husb (Indi h) {
            this->husb = h;
        };
        void set_wife (Indi w) {
            this->wife = w;
        };
        void set_chil (vector<Indi> c) {
            this->chil = c;
        };
        void set_div (int d, int m, int y) {
            int arr[] = {d,m,y};
            this->div = arr;
        };
        // Getters:
        int * get_marr () {
            return this->marr;
        };
        Indi * get_husb () {
            return this->husb;
        };
        Indi * get_wife () {
            return this->wife;
        };
        vector <Indi> get_chil () {
            return this->chil;
        };
        int * get_div () {
            return this->div;
        };
        
        // Other functions
        void add_chil (Indi c) {
            chil.push_back(c);
        };
};
