/**
 * ssw555tm062016Summer
 * Alexis Moore, Harshit Singh, Justin Tsang, Weronika Zamlynny
 */

#include "indi.h"
#include "fam.h"
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <ctime>

using namespace std;

#ifdef _WIN32
#include <Windows.h>
vector<string> getFiles() {
    vector<string> names;
    string searchPath = ".\\GED_Files\\*.ged";
    WIN32_FIND_DATA fd;
    HANDLE hFind = ::FindFirstFile(searchPath.c_str(), &fd);
    if (hFind != INVALID_HANDLE_VALUE) {
        do {
            if (!(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
                names.push_back(fd.cFileName);
            }
        } while(::FindNextFile(hFind, &fd));
        ::FindClose(hFind);
    }
    return names;
}
#endif

vector<string> files = getFiles();

string levelZeroTags[] = {"INDI", "FAM"};
string IndiTags[] = {"NAME", "SEX", "BIRT", "DEAT", "FAMC", "FAMS"};
string FamTags[] = {"CHIL", "MARR", "DIV", "HUSB", "WIFE"};
//"DATE", "HEAD", "TRLR"

// Global Variables
vector< Indi* > IndiArr;
vector< Fam* > FamArr;
int currDate[3];
int IndiAge;
int errorline = 1;
int colID, colName, colGender, colBirth, colDeath;
int colHusb, colWife, colChil, colMarr, colDiv;

/**
 * Splits the input string by spaces, removing '\r'
 */
vector< string > parseLine(string &line) {
    vector< string > res;
    res.clear();
    string buffer = "";
    int strLen = line.length();
    for (int i = 0; i < strLen; i++) {
        if (line[i] == ' ') {
            // Add to vector and clear buffer
            res.push_back(buffer);
            buffer = "";
        } else if (line[i] != '\r') {
            buffer += line[i];
        }
    }
    if (buffer != "") {
        res.push_back(buffer);
    }
    return res;
}

/*
* Gets the current Date
*/
void getCurrentDate() {
	time_t currentTime;
	struct tm *localTime;
	time(&currentTime);
	localTime = localtime(&currentTime);
	currDate[0] = localTime->tm_mday;
	currDate[1] = localTime->tm_mon + 1;
	currDate[2] = localTime->tm_year + 1900;
}

/**
 * Checks that the tag is in the list of valid tags for its level
 */
bool isValidTag(bool isLevelOneTag, bool isIndi, string &tag) {
    // Check if user/fam info tag
    if (isLevelOneTag) {
        // Check if tag belongs to Indi
        if (isIndi) {
            for (const string &currTag : IndiTags) {
                if (currTag == tag) {
                    return true;
                }
            }
        // Check if belongs to Fam
        } else {
            for (const string &currTag : FamTags) {
                if (currTag == tag) {
                    return true;
                }
            }
        }
    // Check if unique ID Tag
    } else {
        // Iterate through each element in tags array and check if there is match
        for (const string &currTag : levelZeroTags) {
            if (currTag == tag) {
                return true;
            }
        }
    }
    return false;
}

/**
 * Function extracts digits in ID
 */
int getDigit(string &id) {
    // Regex to grab digit
    string idNum = "";
    int lenStr = id.length();
    for (int letter = 0; letter < lenStr; ++letter) {
        if (isdigit(id[letter])) {
            idNum += id[letter];
        }
    }
    int idVal = -1;
    if (idNum != "") {
        istringstream buffer(idNum);
        buffer >> idVal;
    }
    return idVal;
}

/*
 * Function converts month initial to the number value
 */
int convMonth(string &mth) {
    if (mth == "JAN") {
        return 1;
    } else if (mth == "FEB") {
        return 2;
    } else if (mth == "MAR") {
        return 3;
    } else if (mth == "APR") {
        return 4;
    } else if (mth == "MAY") {
        return 5;
    } else if (mth == "JUN") {
        return 6;
    } else if (mth == "JUL") {
        return 7;
    } else if (mth == "AUG") {
        return 8;
    } else if (mth == "SEP") {
        return 9;
    } else if (mth == "OCT") {
        return 10;
    } else if (mth == "NOV") {
        return 11;
    } else if (mth == "DEC") {
        return 12;
    }
    return 0;
}

bool checkValidID(bool &isIndi, int &id) {
    bool isValid = true;
    if (id < 0) {
        isValid = false;
    }
    if (isIndi) {
        if (IndiArr[id] != NULL) {
            isValid = false;
        }
    } else {
        if (FamArr[id] != NULL) {
            isValid = false;
        }
    }
    return isValid;
}

// Constructors
int createIndiFam(bool &isIndi, string &id) {
    int currID = getDigit(id);
    // Check if INDI id is unique
    if (!(checkValidID(isIndi, currID))) {
        return -1;
    }
    int start = 0;
    int end = id.size() - 1;
    while (id[start] == id[end]) {
        ++start;
        --end;
    }
    // This INDI ID is unique
    if (isIndi) {
        Indi* uniqueIndi = new Indi();
        uniqueIndi->set_id(id.substr(start, end - start + 1));
        IndiArr[currID] = uniqueIndi;
    } else {
        Fam* uniqueFam = new Fam();
        uniqueFam->set_id(id.substr(start, end - start + 1));
        FamArr[currID] = uniqueFam;
    }
    return currID;
}

// Add values to constructors
int addIndi(int &index, string &line, int &longestName) {
    // TODO: Implement check for longest name for tabulate output
    if (index < 0) {
        return -1;
    }
    vector< string > parsed = parseLine(line);
    if (!isValidTag(true, true, parsed[1])) {
        return -1;
    }
    // Tag is Name
    if (parsed[1] == "NAME") {
        string fullName = "";
        if (parsed.size() > 2) {
            for (int currName = 2; currName < parsed.size() - 1; ++currName) {
                string subName = parsed[currName];
                for (string::iterator c = subName.begin(); c != subName.end(); ++c) {
                    if (((*c) != '/') && ((*c) != '?') && ((*c) != '*') && ((*c) != '!') && ((*c) != '+')
                        && ((*c) != '$') && ((*c) != '#') && ((*c) != '^')) {
                            fullName += *c;
                    }
                }
                fullName += ' ';
            }
            for (string::iterator c = parsed[parsed.size() -1].begin(); c != parsed[parsed.size() -1].end(); ++c) {
                if (((*c) != '/') && ((*c) != '?') && ((*c) != '*') && ((*c) != '!') && ((*c) != '+')
                    && ((*c) != '$') && ((*c) != '#') && ((*c) != '^')) {
                        fullName += *c;
                }
            }
            if (fullName.length() > longestName) {
                longestName = fullName.length();
            }
            IndiArr[index]->set_name(fullName);
        }
    // Tag is Sex
    } else if (parsed[1] == "SEX") {
        if (parsed[2] == "M") {
            IndiArr[index]->set_sex(true);
        } else {
            IndiArr[index]->set_sex(false);
        }
    // Tag is birth of Indi
    } else if (parsed[1] == "BIRT") {
        return 1;
    // Tag is death of INDI
    } else if (parsed[1] == "DEAT") {
        return 2;
    // Tag labels family Indi souse is/was in
    } else if (parsed[1] == "FAMS") {
        int indexID;
        if ((indexID = getDigit(parsed[2])) > -1) {
            IndiArr[index]->add_fams(indexID);
        }
    // Tag labels family Indi child in
    } else if (parsed[1] == "FAMC") {
        int indexID;
        if ((indexID = getDigit(parsed[2])) > -1) {
            IndiArr[index]->add_famc(indexID);
        }
    }
    return 0;
}

int addFam(int &index, string &line) {
    if (index < 0) {
        return -1;
    }
    vector< string > parsed = parseLine(line);
    if (!isValidTag(true, false, parsed[1])) {
        return -2;
    }
    // Tag includes Indi child in
    if (parsed[1] == "CHIL") {
        int indexID;
        if ((indexID = getDigit(parsed[2])) > -1) {
            FamArr[index]->add_chil(indexID);
        }
    // Tag labels marriage date for family
    } else if (parsed[1] == "MARR") {
        return 1;
    // Tag labels divorce date for family
    } else if (parsed[1] == "DIV") {
        return 2;
    // Tag includes Indi as husband in family
    } else if (parsed[1] == "HUSB") {
        int indexID;
        if ((indexID = getDigit(parsed[2])) > -1) {
            FamArr[index]->set_husb(indexID);
        }
    // Tag labels Indi as wife in family
    } else if (parsed[1] == "WIFE") {
        int indexID;
        if ((indexID = getDigit(parsed[2])) > -1) {
            FamArr[index]->set_wife(indexID);
        }
    }
    return 0;
}

/*
 * Function compares date section of two given int array
 * Returns -1 if DateA is later than DateB
 * Returns  1 if DateA is earlier than DateB
 * Returns  0 if DateA is the same as DateB
 */
int getGreaterDate(int &date1, int &date2) {
    // Both dates have same value
    if (date1 == date2) {
        return 0;
    // If date 1 is later than date 2
    } else if (date1 > date2) {
        return -1;
    // If date 1 is earlier than date 2
    } else {
        return 1;
    }
}

/*
 * Function compares dates of two given int array
 * Returns -1 if DateA is later than DateB
 * Returns  1 if DateA is earlier than DateB
 * Returns  0 if DateA is the same as DateB
 * Returns  0 if either DateA or Date B is missing
 */
int dateCompare(int* arrA, int* arrB) {
    // Compare years
    // Check if a date was actually provided
	if ((arrA[2] == 0) || (arrB[2] == 0)) {
        return 0;
	}
    int retCmp = 0;
    for (int datePart = 2; ((datePart >= 0) && (retCmp == 0)); --datePart) {
        retCmp = getGreaterDate(arrA[datePart], arrB[datePart]);
    }
    return retCmp;
}

/*US01
Dates before current date
Dates (birth, marriage, divorce, death) should not be after the current date*/
bool checkFutureDates(int *birth, int *death, int *marr, int *div){
    bool birthComp;
    bool deathComp;
    bool marrComp;
    bool divComp;
    if(currDate[2] < birth[2]){
        birthComp = true;
    } else if(currDate[2] < death[2]){
        deathComp = true;
    } else if(currDate[2] < marr[2]){
        marrComp = true;
    } else if(currDate[2] < div[2]){
        divComp = true;
    }
    if (birthComp == true){
        cout << "Error US01: Date of Birth is in future, please correct the error\n"<<errorline<<"\n";
    } else if(deathComp == true){
        cout << "Error US01: Date of Death is in future, please correct the error\n"<<errorline<<"\n";
    } else if(marrComp == true){
        cout << "Error US01: Date of Marriage is in future, please correct the error\n"<<errorline<<"\n";
    } else if(divComp == true){
        cout << "Error US01: Date of Divorce is in future, please correct the error\n"<<errorline<<"\n";
    }
}

/*US10
Marriage after 14
Marriage should be at least 14 years after birth of both spouses*/

bool MarriedUnder14(Indi &indi){
    bool isError = false;
    vector<int> famc = indi.get_famc();
    for (vector<int>::iterator f = famc.begin(); f != famc.end(); ++f){
        Fam *fam = FamArr[*f];
        if(fam->get_wife() != -1){
              Indi* mom = IndiArr[fam->get_wife()];
              int* momBirth = mom->get_birth();
              int* momMarr =  fam->get_marr();
              if((momMarr[2] - momBirth[2]) < 14){
                isError = true;
              }
        }
        if(fam->get_husb() != -1){
            Indi* dad = IndiArr[fam->get_husb()];
            int* dadBirth = dad->get_birth();
            int* dadMarr = fam->get_marr();
            if(dadMarr[2] - dadBirth[2] < 14){
                isError =  true;
              }
        }
    } if(isError == true){
        cout << "Error US10: Individual cannot be married while under 14 \n"<<errorline<<"\n";
    }
}

/*US23
Unique name and birth date
No more than one individual with the same name and birth date should appear in a GEDCOM files*/

void checkUniqueIndi(int &currID, int &maxIndi){
    for(int restIndiID = currID + 1; restIndiID <= maxIndi; ++restIndiID){
        if(IndiArr[restIndiID] != NULL){
            if((IndiArr[currID]->get_name() == IndiArr[restIndiID]->get_name())){
                  cout<< "Individuals" << IndiArr[currID]->get_id() << "and" << IndiArr[restIndiID]->get_id() << "Error US23: have same name\n"<<errorline<<"\n";
            } else{
                if(((IndiArr[currID]->get_birth() == IndiArr[restIndiID]-> get_birth()))){
                    cout<< "Individuals" << IndiArr[currID]->get_id() << "and" << IndiArr[restIndiID]->get_id() << "Error US23: have same date of birth\n"<<errorline<<"\n";
                }
            }
        }
    }
}

/*US25
Unique first names in families
No more than one child with the same name and birth date should appear in a family*/

bool checkSameName(Fam &fam){
    Indi * mom;
    Indi * dad;
    string momName;
    string dadName;
    string Elder;
    string Younger;

    if(fam.get_wife() != -1){
        mom = IndiArr[fam.get_wife()];
        momName = mom->get_name();
    }
    if(fam.get_husb() != -1){
        dad = IndiArr[fam.get_husb()];
        dadName = dad->get_name();
    }
    vector <int> chil = fam.get_chil();
    if(chil.size() <= 1){
        return true;
    }
    for(std::vector<int>::iterator c = chil.begin(); c != chil.end(); c++){
        Indi * child = IndiArr[*c];
        if(fam.get_wife() != -1){
            if(momName == child->get_name()){
                cout << "Error US25: Mother and child cannot have same name\n"<<errorline<<"\n";
            }
        }
        if(fam.get_husb() != -1){
            if(dadName == child->get_name()){
                cout << "Error US25: Father and child cannot have same name\n"<<errorline<<"\n";
            }
        }
        for(std::vector<int>::iterator S = chil.begin(); S != --chil.end(); S++){
         for(std::vector<int>::iterator J = S + 1; J != chil.end(); J++){
         Indi * Senior = IndiArr[*S];
         Indi * Junior = IndiArr[*J];
         Elder = Senior->get_name();
         Younger = Junior->get_name();

         if(Elder == Younger){
            cout<<"Error US25: Siblings cannot have same name\n"<<errorline<<"\n";
         }
         }
        }
    }
}

/*US27
Include individual ages
Include person's current age when listing individuals*/

int PersonsAge(Indi &indi, int currID, int &maxIndi){
    int* birth = indi.get_birth();
    int* death = indi.get_death();
    //for(currID = 0; currID <= maxIndi; ++currID){
        if(IndiArr[currID] != NULL){
            if(death[2] == 0){
                IndiAge = currDate[2] - birth[2];
            }else{
                IndiAge = death[2] - birth[2];
            }
        }
return IndiAge;
}
/*US35
List recent birth
List individuals who were born in last 30 days*/
void listRecentBirth(int &maxIndi) {
    // Get 1 month back and run dateCompare() to check that death date is later than 1 month before date
    int MonthBack[3];
    if (((currDate[1] >= 1) && (currDate[1] <= 12)) &&
       ((currDate[0] >= 1) && (currDate[0] <= 31)) &&
       (currDate[2] >= 1)) {
        MonthBack[0] = currDate[0];
        MonthBack[1] = currDate[1];
        MonthBack[2] = currDate[2];

        MonthBack[0] -= 30;
        if (MonthBack[0] <= 0) {
            // Check if January
            if (MonthBack[1] == 1) {
                MonthBack[1] = 12;
                MonthBack[2] -= 1;
            } else {
                MonthBack[1] -= 1;
            }
        }
        switch (MonthBack[1]) {
            // February
            case (2):
                if ((MonthBack[2] % 400 == 0) || ((MonthBack[2] % 100 != 0) && (MonthBack[2] % 4 == 0))) {
                    MonthBack[0] = 29 - MonthBack[0];
                } else {
                    MonthBack[0] = 28 - MonthBack[0];
                }
                break;
            // January, March, May, July, August, October, December
            case (1):
            case (3):
            case (5):
            case (7):
            case (8):
            case (10):
            case (12):
                MonthBack[0] = 31 - MonthBack[0];
                break;
            // April, June, September, November
            case (4):
            case (6):
            case (9):
            case (11):
                MonthBack[0] = 30 - MonthBack[0];
                break;
        }
        for (int i = 0; i <= maxIndi; ++i) {
            if (IndiArr[i] != NULL) {
                if (IndiArr[i]->get_death()[2] > 0) {
                    if (dateCompare(MonthBack, IndiArr[i]->get_birth()) > 0) {
                                cout << "US35 Name: " << IndiArr[i]->get_name() << "\n";
                                cout << "US35 Birth: " << IndiArr[i]->get_birth() << "\n";
                            }
                        }
                    }
                }
            }
        }
/*US36
List recent deaths
List individuals who died in last 30 days*/
void listRecentDeath(int &maxIndi) {
    // Get 1 month back and run dateCompare() to check that death date is later than 1 month before date
    int MonthBack[3];
    if (((currDate[1] >= 1) && (currDate[1] <= 12)) &&
       ((currDate[0] >= 1) && (currDate[0] <= 31)) &&
       (currDate[2] >= 1)) {
        MonthBack[0] = currDate[0];
        MonthBack[1] = currDate[1];
        MonthBack[2] = currDate[2];

        MonthBack[0] -= 30;
        if (MonthBack[0] <= 0) {
            // Check if January
            if (MonthBack[1] == 1) {
                MonthBack[1] = 12;
                MonthBack[2] -= 1;
            } else {
                MonthBack[1] -= 1;
            }
        }
        switch (MonthBack[1]) {
            // February
            case (2):
                if ((MonthBack[2] % 400 == 0) || ((MonthBack[2] % 100 != 0) && (MonthBack[2] % 4 == 0))) {
                    MonthBack[0] = 29 - MonthBack[0];
                } else {
                    MonthBack[0] = 28 - MonthBack[0];
                }
                break;
            // January, March, May, July, August, October, December
            case (1):
            case (3):
            case (5):
            case (7):
            case (8):
            case (10):
            case (12):
                MonthBack[0] = 31 - MonthBack[0];
                break;
            // April, June, September, November
            case (4):
            case (6):
            case (9):
            case (11):
                MonthBack[0] = 30 - MonthBack[0];
                break;
        }
        for (int i = 0; i <= maxIndi; ++i) {
            if (IndiArr[i] != NULL) {
                if (IndiArr[i]->get_death()[2] > 0) {
                     if (dateCompare(MonthBack, IndiArr[i]->get_death()) > 0) {
                                cout << "US35 Name: " << IndiArr[i]->get_name() << "\n";
                                cout << "US36 Death: " << IndiArr[i]->get_death() << "\n";
                            }
                        }
                    }
                }
            }
        }

bool validateDate(int &day, int &mth, int &yr) {
    int permDay = day;
    int permMth = mth;
    int permYr = yr;
    bool isValid = true;
    // Invalid year
    if ((yr < 0) || (yr > currDate[2])) {
        // Remove day, month, and year
        yr = 0;
        mth = 0;
        day = 0;
        isValid = false;
    // Invalid Month
    } else if ((mth < 0) || (mth > 12)) {
        // Remove day and month
        mth = 0;
        day = 0;
        isValid = false;
    // Invalid Day
    } else if ((day < 0) || (day > 31)) {
        // Remove day
        day = 0;
        isValid = false;
    }
    // General valid day, month, and year
    switch (mth) {
        // February
        case (2):
            if ((yr % 400 == 0) || ((yr % 100 != 0) && (yr % 4 == 0))) {
                if ((day < 0) || (day > 29)) {
                    day = 0;
                    isValid = false;
                }
            } else {
                if ((day < 0) || (day > 28)) {
                    day = 0;
                    isValid = false;
                }
            }
            break;
        // January, March, May, July, August, October, December
        case (1):
        case (3):
        case (5):
        case (7):
        case (8):
        case (10):
        case (12):
            if ((day < 0) || (day > 31)) {
                day = 0;
                isValid = false;
            }
            break;
        // April, June, September, November
        case (4):
        case (6):
        case (9):
        case (11):
            if ((day < 0) || (day > 30)) {
                day = 0;
                isValid = false;
            }
            break;
    }
    if (!(isValid)) {
        cout << "Error US42: Invalid date (" << permMth << "/" << permDay << "/" <<
                permYr << ")\n"<<errorline<<"\n";
    }
    return isValid;
}

bool addDate(bool &isIndi, int &index, int &tag, string &line) {
    vector<string> parsed = parseLine(line);
    int day= 0;
    int mth = 0;
    int yr = 0;
    stringstream buffer;
    // If only provided year
    if (parsed.size() == 3) {
        buffer << parsed[2];
        buffer >> yr;
    // If only provided month and year
    } else if (parsed.size() == 4) {
        buffer << parsed[3];
        buffer >> yr;
        mth = convMonth(parsed[2]);
    // If provided day, month, and year
    } else if (parsed.size() == 5) {
        stringstream buffer;
        buffer << parsed[2] << " " << parsed[4];
        buffer >> day >> yr;
        mth = convMonth(parsed[3]);
    } else {
        return false;
    }
    // Check if "valid" date provided
    if (!(validateDate(day, mth, yr))) {
        if (isIndi) {
            cout << "Error US42: " << ((tag == 1) ? "Birth" : "Death") << " date of " <<
                    IndiArr[index]->get_name() << "(" << IndiArr[index]->get_id() <<
                    ")" << " is an invalid date.\n"<<errorline<<"\n";
        } else {
            cout << "Error US42: " << ((tag == 1) ? "Marriage" : "Divorce") << " date of " <<
                    FamArr[index]->get_id() << " is an invalid date.\n"<<"\n"<<errorline<<"\n";
        }
    }
    switch (tag) {
        // If tag that instigated date was BIRT
        case (1):
            if (isIndi) {
                IndiArr[index]->set_birth(day, mth, yr);
            } else {
                FamArr[index]->set_marr(day, mth, yr);
            }
            break;
        // If tag that instigated date was DEAT
        case (2):
            if (isIndi) {
                IndiArr[index]->set_death(day, mth, yr);
            } else {
                FamArr[index]->set_div(day, mth, yr);
            }
            break;
        default:
            return false;
    }
    return true;
}

/*
* Checks if an individual has a valid birth date.
*/
bool checkValidBirth(Indi &indi) {
    bool isError = false;
    int* birth = indi.get_birth();
    int* death = indi.get_death();
    // cout << "Birth: " << birth[0] << " " << birth[1] << " " << birth[2] << "\n";
    // cout << "Death: " << death[0] << " " << death[1] << " " << death[2] << "\n";

    // Individual was born before they died
    if (dateCompare(birth, death) < 0) {
        //cout << "Error US03: Individual cannot die before they are born.\n";
        cout << "Error US03: " << indi.get_name() << " (" << indi.get_id() << ") cannot die before they are born.\n"<<errorline<<"\n";
        isError = true;
    }

    // Individual was born before their parents died
    vector<int> famc = indi.get_famc();
    for (vector<int>::iterator f = famc.begin(); f != famc.end(); ++f) {
        Fam* fam = FamArr[*f];

        // Cannot be born after death of mother
        if (fam->get_wife() != -1) { //Check if mom on record
            Indi* mom = IndiArr[fam->get_wife()];

            int* momDeath = mom->get_death();
            if (dateCompare(birth, momDeath) < 0) {
                //cout << "Error US09: Individual cannot be born after the death of mother.\n";
                cout << "Error US09: " << indi.get_name() << " (" << indi.get_id() \
                    << ") cannot be born after the death of mother " << mom->get_name() \
                    << " (" << mom->get_id() << ").\n"<<errorline<<"\n";
                isError = true;
            }
        }

        // Cannot be born 9 months after death of father
        if (fam->get_husb() != -1) { //Check if dad on record
            Indi* dad = IndiArr[fam->get_husb()];

            int* dadDeath = dad->get_death();
            int* dDeath = new int[3];
            for (int i = 0; i < 3; i++) {
                //Creating a deep copy of dadDeath
                dDeath[i] = dadDeath[i];
            }
            int monthOffset = 9;
            if (dDeath[1] + monthOffset <= 12) {
                dDeath[1] += monthOffset;
            } else {
                monthOffset -= (12 - dDeath[1]);
                dDeath[1] = monthOffset;
                dDeath[2] += 1;
            }
            if (dateCompare(birth, dDeath) < 0) {
                //cout << "Error: Individual cannot be born 9 months after the death of father.\n";
                cout << "Error US09: " << indi.get_name() << " (" << indi.get_id() \
                    << ") cannot be born 9 months after the death of father " \
                    << dad->get_name() << " (" << dad->get_id() << ").\n"<<errorline<<"\n";
                isError = true;
            }
        }
    }

    // Individual was born before they got married
    vector <int> fams = indi.get_fams();
    for (std::vector<int>::iterator f = fams.begin(); f != fams.end(); ++f) {
        Fam* fam = FamArr[*f];

        int* marr = fam->get_marr();
        // Check that individual got married
        if (marr != NULL) {
            if (dateCompare(birth, marr) < 0){
                // cout << "Error: Individual cannot be married before birth.\n";
                cout << "Error US02: " << indi.get_name() << " (" << indi.get_id() << ") cannot be married before birth.\n"<<errorline<<"\n";
                isError = true;
            }
        }
    }

    if (isError)
        return false;
    return true;
}

/**
* Checks that the mother is less than 60 years older than her children, and that the father is less than 80 years older than his children.
*/
bool parentsNotTooOld(Fam &fam) {
    bool isError = false;
    Indi* mom;
    int * momBirth;
    int * momFinal;
    Indi * dad;
    int * dadBirth;
    int * dadFinal;

    // Get Mother's information
    if (fam.get_wife() != -1) {
        mom = IndiArr[fam.get_wife()];
        momBirth = mom->get_birth();
        momFinal = new int[3];
        for (int i = 0; i < 3; i++) {
            //Creating a deep copy of dadDeath
            momFinal[i] = momBirth[i];
        }
        momFinal[2] += 60; // Can have kids up to 60 years after birth
    }
    // Get Father's information
    if (fam.get_husb() != -1) {
        dad = IndiArr[fam.get_husb()];
        dadBirth = dad->get_birth();
        dadFinal = new int[3];
        for (int i = 0; i < 3; i++) {
            //Creating a deep copy of dadDeath
            dadFinal[i] = dadBirth[i];
        }
        dadFinal[2] += 80; // Can have kids up to 80 years after birth
    }

    vector <int> chil = fam.get_chil();
    for (std::vector<int>::iterator c = chil.begin(); c != chil.end(); c++) {
        Indi * child = IndiArr[*c];
        if (fam.get_wife() != -1) {
            // Compare mother to child
            if (dateCompare(momFinal, child->get_birth()) > 0) {
                isError = true;
                cout << "Error US12: Mother " << mom->get_name() << " (" << mom->get_id() << ") was too old when child " \
                    << child->get_name() << " (" << child->get_id() << ") was born in family " << fam.get_id() <<".\n"<<errorline<<"\n";
            }
        }
        if (fam.get_husb() != -1) {
            // Compare father to child
            if (dateCompare(dadFinal, child->get_birth()) > 0) {
                isError = true;
                cout << "Error US12: Father " << dad->get_name() << " (" << dad->get_id() << ") was too old when child " \
                    << child->get_name() << " (" << child->get_id() << ") was born in family " << fam.get_id() <<".\n"<<errorline<<"\n";
            }
        }
    }

    if (isError)
        return false;
    return true;
}

/**
* Checks that all siblings in a family are born at least 9 months apart, or less than 2 days apart.
*/
bool checkSiblingSpacing(Fam &fam) {
    bool isError = false;
    bool localError = false;
    int * birth1 = new int[3]();
    int * birth2 = new int[3]();

    vector <int> chil = fam.get_chil();
    if (chil.size() <= 1) {
        return true;
    }
    for (std::vector<int>::iterator c1 = chil.begin(); c1 != --chil.end(); c1++) {
        for (std::vector<int>::iterator c2 = c1 + 1; c2 != chil.end(); c2++) {
            localError = false;

            Indi * child1 = IndiArr[*c1];
            Indi * child2 = IndiArr[*c2];
            birth1 = child1->get_birth();
            birth2 = child2->get_birth();

            if (dateCompare(birth1,birth2) <= 0) {
                // birth 1 is less that birth2 - switch them
                int *temp = new int[3]();
                temp = birth1;
                birth1 = birth2;
                birth2 = temp;
            }
            if (birth1[2] - birth2[2] == 1) {
                if ((birth1[1] + 9 % 12) <= birth2[2]) {
                    localError = true;
                }
            } else if (birth1[2] - birth2[2] == 0) {
                // Check if twins
                if (birth1[1] - birth2[1] == 0) {
                    if (birth1[1] - birth2[1] > 2) {
                        // more than two days apart, not twins
                        localError = true;
                    }
                // Not twins
                } else if (birth1[1] - birth2[1] < 9) {
                    localError = true;
                }
            }
            if (localError) {
                isError = localError;
                cout << "Error US13: Siblings " << child1->get_name() << " (" << child1->get_id() \
                    << ") and " << child2->get_name() << " (" << child2->get_id() << ") in family " \
                    << fam.get_id() << " are born less than 9 months apart, and not twins.\n"<<errorline<<"\n";
            }
        }
    }
    if (isError)
        return false;
    return true;
}

/*
* Checks that individual was not married to two people at any point in time.
*/
bool checkBigamy(Indi &indi) {
    vector <int> fams = indi.get_fams();
    bool isError = false;
    bool localError = false;

    // Only need to test if spouse in more than one family
    if (fams.size() > 1) {
        for (std::vector<int>::iterator f1 = fams.begin(); f1 != --fams.end(); f1++) {
            Fam* fam1 = FamArr[*f1];

            // Get marriage and divorce dates for first family
            int * marr1 = fam1->get_marr();
            int * div1 = fam1->get_div();

            for (vector<int>::iterator f2 = f1 + 1; f2 != fams.end(); f2++) {
                localError = false;
                Fam* fam2 = FamArr[*f2];

                // Get marriage and divorce dates for second family
                int * marr2 = fam2->get_marr();
                int * div2 = fam2->get_div();

                if (marr1[2] == 0 || marr2[2] == 0) {
                    // One was not a marriage, don't need to check
                    break;
                }

                // First marrage occurred first
                if (dateCompare(marr1, marr2) > 0) {
                    // Check that first divorce occurred before second marriage
                    if (div1[2] == 0) {
                        localError = true;
                    } else if (dateCompare( div1, marr2 ) < 0) {
                        localError = true;
                    }
                } else {
                    // Check that second divorce occurred before first marriage
                    if (div2[2] == 0) {
                        localError = true;
                    } else if (dateCompare( div2, marr1) < 0) {
                        localError = true;
                    }
                }

                if (localError) {
                    cout << "Anomality US11: " << indi.get_name() << " (" << indi.get_id() <<
                            ") has been in two or more marriages at the same time.\n"<<errorline<<"\n";
                    isError = true;
                }
            }
        }
    }
    if (isError)
        return false;
    return true;
}

/*
* Checks that siblings are not married
*/
bool checkSiblingMarriage (Fam &fam) {
    if (fam.get_wife() == -1 || fam.get_husb() == -1) {
        return true;
    }

    Indi* wife = IndiArr[fam.get_wife()];
    Indi* husb = IndiArr[fam.get_husb()];

    vector <int> wifeFam = wife->get_famc();
    vector <int> husbFam = husb->get_famc();

    // Famc arrays should usually only be one long
    for (std::vector<int>::iterator w = wifeFam.begin(); w != wifeFam.end(); w++) {
        for (std::vector<int>::iterator h = husbFam.begin(); h != husbFam.end(); h++) {
            if (*w == *h) {
                cout << "Anomality US18: Siblings " << wife->get_name() << " (" << wife->get_id() \
                    << ") and " << husb->get_name() << " (" << husb->get_id() << ") are married.\n"<<errorline<<"\n";
                return false;
            }
        }
    }
    return true;
}

/*
* Checks if an individual was born before parents were married or after they were divorced
*/
void checkWedlock(Indi &indi) {
	int* birth = indi.get_birth();
	// Individual was born before their parents were married
    vector <int> famc = indi.get_famc();
	for (std::vector<int>::iterator f = famc.begin(); f != famc.end(); ++f) {
        Fam *family = FamArr[*f];
		if (family == NULL) {
			if (family->get_marr()[2] != 0) { //Only check for valid year per US41
				if (dateCompare(birth, family->get_marr()) == 1) {
					cout << "Error US08: " << indi.get_name() << " (" << indi.get_id() << ") born out of wedlock.\n";
				} else {
					if (family->get_div()[2] != 0) { // Only check for valid year per US41
						if (dateCompare(birth, family->get_div()) == -1) {
							cout << "Error US08: " << indi.get_name() << " (" << indi.get_id() << ") born out of wedlock.\n";
						}
					}
				}
			} else {
				cout << "Error US08: " << indi.get_name() << " (" << indi.get_id() << ") born out of wedlock.\n"<<errorline<<"\n";
			}
		}
	}
}

// Checks that parents aren't married to their descendants
void checkParentDescendantMarriage(Indi *parent, Indi *descendant)
{
	vector <int> famS = parent->get_fams();
	for (vector<int>::iterator currFamS = famS.begin(); currFamS != famS.end(); currFamS++) {
		vector<int> chil = FamArr[*currFamS]->get_chil();
		for (std::vector<int>::iterator c = chil.begin(); c != chil.end(); c++) {

			Indi* child = IndiArr[*c];
			if (child->get_id() == descendant->get_id()) {
				cout << "Error US17: Parent is married to descendant. \n"<<errorline<<"\n";
				break;
			} else {
				checkParentDescendantMarriage(child, descendant);
			}
		}
	}
}

/* Function returns a vector of vector<int> of all nieces and nephews */
vector< vector<int> > getNieceNephew(vector<int> siblings, int &parentIndex) {
    vector< vector<int> > allNieceNephew;
    allNieceNephew.clear();
    for (vector<int>::iterator s = siblings.begin(); s != siblings.end(); ++s) {
        if ((*s) != parentIndex) {
            vector<int> siblingFamSpouse = IndiArr[*s]->get_fams();
            for (vector<int>::iterator currFamS = siblingFamSpouse.begin(); currFamS != siblingFamSpouse.end(); ++currFamS) {
                allNieceNephew.push_back(FamArr[*currFamS]->get_chil());
            }
        }
    }
    return allNieceNephew;
}

/* Check if family is marriage between uncle/aunt & niece/nephew */
bool checkAuntUncleRelation(Fam &fam, bool isUncle) {
    bool err20 = false;
    Indi* AuntUncle = ((isUncle) ? IndiArr[fam.get_husb()] : IndiArr[fam.get_wife()]);
    int spouseIndex = ((isUncle) ? fam.get_wife() : fam.get_husb());
    if ((AuntUncle->get_famc().size()) <= 0) {
        return false;
    }
    int AuntUnclefamC = (AuntUncle->get_famc()).at(0);
    vector<int> AuntUncleSiblings = FamArr[AuntUnclefamC]->get_chil();
    // Not only child
    if (AuntUncleSiblings.size() > 1) {
        int AuntUncleIndex = ((isUncle) ? fam.get_husb() : fam.get_wife());
        vector< vector<int> > siblingFamSpouse = getNieceNephew(AuntUncleSiblings, AuntUncleIndex);
        for (vector< vector<int> >::iterator currFamChild = siblingFamSpouse.begin(); currFamChild != siblingFamSpouse.end(); ++currFamChild) {
            for (vector<int>::iterator currNieceNephew = (*currFamChild).begin(); currNieceNephew != (*currFamChild).end(); ++currNieceNephew) {
                if ((*currNieceNephew) == spouseIndex) {
                    cout << "Error US20: " << ((AuntUncle->get_sex()) ? "Uncle " : "Aunt ")  << AuntUncle->get_name() << " (" << AuntUncle->get_id() <<
                            ") is married to " << ((IndiArr[*currNieceNephew]->get_sex()) ? "nephew " : "niece ") << IndiArr[*currNieceNephew]->get_name() <<
                            "(" << IndiArr[*currNieceNephew]->get_id() << ")\n"<<errorline<<"\n";
                    err20 = true;
                }
            }
        }
    }
    return err20;
}

/* Check if family is marriage between cousins */
bool checkCousinRelation(Fam &fam, bool isHusb) {
    Indi* cousin = ((isHusb) ? IndiArr[fam.get_husb()] : IndiArr[fam.get_wife()]);
    if ((cousin->get_famc()).size() <= 0) {
        return false;
    }
    bool err19 = false;
    int spouseIndex = ((isHusb) ? fam.get_wife() : fam.get_husb());
    // Get famc parents
    vector<int> parents;
    parents.clear();
    parents.push_back(FamArr[(cousin->get_famc()).at(0)]->get_husb());
    parents.push_back(FamArr[(cousin->get_famc()).at(0)]->get_wife());
    for (vector<int>::iterator parent = parents.begin(); parent != parents.end(); ++parent) {
        if ((*parent > -1) && ((IndiArr[*parent]->get_famc()).size() > 0)) {
            vector<int> parentsSiblings = FamArr[(IndiArr[*parent]->get_famc()).at(0)]->get_chil();
            vector< vector<int> > allCousins = getNieceNephew(parentsSiblings, *parent);
            for (vector< vector<int> >::iterator currFamChild = allCousins.begin(); currFamChild != allCousins.end(); ++currFamChild) {
                for (vector<int>::iterator currCousin = (*currFamChild).begin(); currCousin != (*currFamChild).end(); ++currCousin) {
                    if ((*currCousin) == spouseIndex) {
                        cout << "Error US19: " << cousin->get_name() << "(" << cousin->get_id() << ") and " <<
                                IndiArr[*currCousin]->get_name() <<"(" << IndiArr[*currCousin]->get_id() << ") are married cousins\n"<<errorline<<"\n";
                        err19 = true;
                    }
                }
            }
        }
    }
    return err19;
}

/* Function checks if any direct relatives are married */
bool checkFirstRelativeMarriage(Fam &fam) {
    if (fam.get_wife() == -1 || fam.get_husb() == -1) {
        return true;
    }
    bool hasRelation = false;
    // Check Aunt/Uncle & Niece/Nephew relations
    checkAuntUncleRelation(fam, true);
    checkAuntUncleRelation(fam, false);

    // Check cousin relations
    checkCousinRelation(fam, true);
    // checkCousinRelation(fam, false);

    return hasRelation;
}


// Checks to make sure no one is over 150 years old
bool olderThan150(int *birth, int *death) {
	bool retComp;
	//Check if individual has died
	if (death[2] == 0) {
		//check against year
		if ((currDate[2] - birth[2]) > 150) {
			retComp = true;
		} else if ((currDate[2] - birth[2]) == 150) {
			//check against month
			if ((currDate[1] - birth[1]) >= 0) {
				retComp = true;
			} else if ((currDate[1] - birth[1]) == 0) {
				//check against day
				if ((currDate[0] - birth[0]) >= 0) {
					retComp = true;
				} else {
					retComp = false;
				}
			} else {
				retComp = false;
			}
		} else {
			retComp = false;
		}
	//check year of death against birth
    } else if ((death[2] - birth[2]) > 150) {
		retComp = true;
	//check year of death against birth
    } else if ((death[2] - birth[2]) == 150) {
		//check month of death against birth
		if((death[1] - birth[1]) >= 0) {
			retComp = true;
		//check date of death against birth
        } else if ((death[1] - birth[1]) == 0) {
			if ((death[0] - birth[0]) >= 0) {
				retComp = true;
			} else {
				retComp = false;
			}
		} else {
			retComp = false;
		}
	} else {
		retComp = false;
	}
	return retComp;
}

void checkUniqueFamS(int &currID, int &maxFam) {
    for (int restFamID = currID + 1; restFamID <= maxFam; ++restFamID) {
        if (FamArr[restFamID] != NULL) {
            // Same spouses
            if ((FamArr[currID]->get_husb() == FamArr[restFamID]->get_husb()) &&
                (FamArr[currID]->get_wife() == FamArr[restFamID]->get_wife())) {
                // Same marriage date
                if (dateCompare(FamArr[currID]->get_marr(), FamArr[restFamID]->get_marr()) == 0) {
                    cout << "Families " << FamArr[currID]->get_id() << " and " << FamArr[restFamID]->get_id() << " have same marriage dates\n"<<errorline<<"\n";
                }
            }
        }
    }
}

/**
 * Function checks if Indi has corresponding entry in each FamC's chil_
 */
void isCorrespondingFamC(int &currID) {
    vector<int> family = IndiArr[currID]->get_famc();
    for (vector<int>::iterator it = family.begin(); it != family.end(); ++it) {
        if ((FamArr[*it] == NULL) || (!(FamArr[*it]->checkChild(currID)))) {
            cout << "Error US26: " << IndiArr[currID]->get_name() << "(" << IndiArr[currID]->get_id() <<
                    ") is not a corresponding child in family " << FamArr[*it]->get_id() << "\n"<<errorline<<"\n";
        }
    }
}

/**
 * Function checks if Indi has corresponding entry in each FamS's husb_ or wife_
 */
void isCorrespondingFamS(int &currID) {
    vector<int> family = IndiArr[currID]->get_fams();
    for (vector<int>::iterator it = family.begin(); it != family.end(); ++it) {
        if ((FamArr[*it] == NULL) || (!(currID == FamArr[*it]->get_husb())) && (!(currID == FamArr[*it]->get_wife()))) {
            cout << "Error US26: " << IndiArr[currID]->get_name() << "(" << IndiArr[currID]->get_id() <<
                    ") is not a corresponding child in family " << FamArr[*it]->get_id() << "\n"<<errorline;
        }
    }
}

int getAmtIndi(int &num) {
    int len = 0;
    int amtIndi = num;
    while (amtIndi > 0) {
        amtIndi /= 10;
        ++len;
    }
    return len;
}

/* Function lists all living married couples */
void listLivingMarried(int &maxFam) {
    cout << "| HUSB ID | WIFE ID |\n";
    cout << "|_________|_________|\n";
    for (int f = 0; f <= maxFam; ++f) {
        if (FamArr[f] != NULL) {
            // If no divorce
            if (FamArr[f]->get_div()[2] == 0) {
                int husb = FamArr[f]->get_husb();
                int wife = FamArr[f]->get_wife();
                if ((husb > -1) && (wife > -1)) {
                    if ((IndiArr[husb] != NULL) && (IndiArr[wife] != NULL)) {
                        if ((IndiArr[husb]->get_death()[2] == 0) && (IndiArr[wife]->get_death()[2] == 0)) {
                            cout << "| " << husb;
                            for (int i = getAmtIndi(husb); i < 7; ++i) {
                                cout << " ";
                            }
                            cout << " | ";
                            cout << wife;
                            for (int i = getAmtIndi(wife); i < 7; ++i) {
                                cout << " ";
                            }
                            cout << " |\n";
                        }
                    }
                }
            }
        }
    }
    cout << "|_________|_________|\n";
}

void printDescendants(Fam* f) {
    vector<int> chil = f->get_chil();
    for (vector<int>::iterator c = chil.begin(); c != chil.end(); ++c) {
        cout << IndiArr[*c]->get_name() << "\n";
        vector<int> descFam = IndiArr[*c]->get_fams();
        for (vector<int>::iterator dc = descFam.begin(); dc != descFam.end(); ++dc) {
            printDescendants(FamArr[*dc]);
        }
    }
}

/* Function lists all living spouses and descendants of people who died in last 30 days */
void listRecentSurvivors(int &maxIndi) {
    // Get 1 month back and run dateCompare() to check that death date is later than 1 month before date
    int MonthBack[3];
    if (((currDate[1] >= 1) && (currDate[1] <= 12)) &&
       ((currDate[0] >= 1) && (currDate[0] <= 31)) &&
       (currDate[2] >= 1)) {
        MonthBack[0] = currDate[0];
        MonthBack[1] = currDate[1];
        MonthBack[2] = currDate[2];

        MonthBack[0] -= 30;
        if (MonthBack[0] <= 0) {
            // Check if January
            if (MonthBack[1] == 1) {
                MonthBack[1] = 12;
                MonthBack[2] -= 1;
            } else {
                MonthBack[1] -= 1;
            }
        }
        switch (MonthBack[1]) {
            // February
            case (2):
                if ((MonthBack[2] % 400 == 0) || ((MonthBack[2] % 100 != 0) && (MonthBack[2] % 4 == 0))) {
                    MonthBack[0] = 29 - MonthBack[0];
                } else {
                    MonthBack[0] = 28 - MonthBack[0];
                }
                break;
            // January, March, May, July, August, October, December
            case (1):
            case (3):
            case (5):
            case (7):
            case (8):
            case (10):
            case (12):
                MonthBack[0] = 31 - MonthBack[0];
                break;
            // April, June, September, November
            case (4):
            case (6):
            case (9):
            case (11):
                MonthBack[0] = 30 - MonthBack[0];
                break;
        }
        for (int i = 0; i <= maxIndi; ++i) {
            if (IndiArr[i] != NULL) {
                if (IndiArr[i]->get_death()[2] > 0) {
                    if (dateCompare(MonthBack, IndiArr[i]->get_death()) > 0) {
                        vector<int> family = IndiArr[i]->get_fams();
                        for (vector<int>::iterator fs = family.begin(); fs != family.end(); ++fs) {
                            if (IndiArr[i]->get_sex()) {
                                cout << "Wife: " << IndiArr[FamArr[*fs]->get_wife()]->get_name() << "\n";
                            } else {
                                cout << "Husband: " << IndiArr[FamArr[*fs]->get_husb()]->get_name() << "\n";
                            }
                            cout << "Descendants:\n";
                            printDescendants(FamArr[*fs]);
                        }
                    }
                }
            }
        }
    }
}


/* Function lists all deceased individuals */
void listDeceased(int &maxIndi) {
    for (int i = 0; i <= maxIndi; ++i) {
        if(IndiArr[i] != NULL) {
            //If individual is dead
			if(IndiArr[i]->get_death()[2] > 0) {
				cout << "US29: " << IndiArr[i]->get_name() << "\n";
			}
		}
	}
}

void listOrphans(int &maxFam) {
	for(int f=0; f <= maxFam; ++f) {
		if(FamArr[f] != NULL) {
			int husb = FamArr[f]->get_husb();
            int wife = FamArr[f]->get_wife();
			if ((husb > -1) && (wife > -1)) {
				if ((IndiArr[husb] != NULL) && (IndiArr[wife] != NULL)) {
					if ((IndiArr[husb]->get_death()[2] > 0) && (IndiArr[wife]->get_death()[2] > 0)) {
						vector <int> chil = FamArr[f]->get_chil();
						for (std::vector<int>::iterator c = chil.begin(); c != chil.end(); c++) {
							Indi * child = IndiArr[*c];
							if((currDate[2] - child->get_birth()[2]) < 18) {
								cout << "US33: " << child->get_name() << "\n";
							}
							else if((currDate[2] - child->get_birth()[2]) == 18) {
								if((currDate[1] - child->get_birth()[1]) < 0) {
									cout << "US33: " << child->get_name() << "\n";
								}
								else if((currDate[1] - child->get_birth()[1]) == 0) {
									if ((currDate[0] - child->get_birth()[0]) < 0) {
										cout << "US33: " << child->get_name() << "\n";
									}
								}
							}
						}
					}
				}
			}
		}
	}
}

/**
 * Function prints data of Indi onto console and outputFile
 */
void printIndiStats(ofstream &outputFile, int &currID) {
    int* birth = IndiArr[currID]->get_birth();
    int* death = IndiArr[currID]->get_death();
    int remainSpace;
    cout << "| "<< currID;
    int lenID = getAmtIndi(currID);
    for (remainSpace = lenID; remainSpace < colID; ++remainSpace) {
        cout << " ";
    }
    cout << " | " << IndiArr[currID]->get_name();
    for (remainSpace = IndiArr[currID]->get_name().length(); remainSpace < colName; ++remainSpace) {
        cout << " ";
    }
    cout << " | " << ((IndiArr[currID]->get_sex()) ? " Male " : "Female");
    stringstream buffer;
    string stringDate;
    if ((birth[0] == 0) && (birth[1] == 0) && (birth[2] == 0)) {
        stringDate = "NULL";
    } else {
        buffer << birth[1] << "/" << birth[0] << "/" << birth[2];
        buffer >> stringDate;
        buffer.clear();
    }
    cout << " | " << stringDate;
    for (remainSpace = stringDate.length(); remainSpace < colBirth; ++remainSpace) {
        cout << " ";
    }
    if ((death[0] == 0) && (death[1] == 0) && (death[2] == 0)) {
        stringDate = "NULL";
    } else {
        buffer << death[1] << "/" << death[0] << "/" << death[2];
        buffer >> stringDate;
        buffer.clear();
    }
    cout << " | " << stringDate;
    for (remainSpace = stringDate.length(); remainSpace < colDeath; ++remainSpace) {
        cout << " ";
    }
    cout << " |\n";
    //Add entries to output file
    outputFile << "\nINDI ID: " << IndiArr[currID]->get_id() << "\n";
    outputFile << "Name: " << IndiArr[currID]->get_name() << "\n";
    outputFile << "Gender: " << ((IndiArr[currID]->get_sex()) ? "Male" : "Female") << "\n";
    outputFile << "Birth date: " << birth[1] << "/" << birth[0] << "/" << birth[2] << "\n";
    outputFile << "Age: "<<IndiAge<<"\n";
}

void printFamilyMembers(ofstream &outputFile, Fam* family) {
    // Print father's name
    int memberID = family->get_husb();
    if ((memberID > -1) && (IndiArr[memberID] != NULL)) {
        cout << "Husband: " << IndiArr[memberID]->get_name() << "\n";
        outputFile << "Husband: " << IndiArr[memberID]->get_name() << "\n";
    }

    // Print mother's name
    memberID = family->get_wife();
    if ((memberID > -1) && (IndiArr[memberID] != NULL)) {
        cout << "Wife: " << IndiArr[memberID]->get_name() << "\n";
        outputFile << "Wife: " << IndiArr[memberID]->get_name() << "\n";
    }

    vector<int> children = family->get_chil();
    for (vector<int>::iterator chil = children.begin(); chil != children.end(); ++chil) {
        Indi* kid = IndiArr[*chil];
        cout << ((kid->get_sex()) ? "Son" : "Daughter") << ": " << kid->get_name() << "\n";
        outputFile << ((kid->get_sex()) ? "Son" : "Daughter") << ": " << kid->get_name() << "\n";
    }
}

int getLongestFamily(int &maxFam) {
    int longestFam = 0;
    for (int f = 0; f <= maxFam; ++f) {
        if (FamArr[f] != NULL) {
            int lenChil = 0;
            vector<int> children = FamArr[f]->get_chil();
            if (children.size() > 0) {
                for (vector<int>::iterator c = children.begin(); c != children.end(); ++c) {
                    lenChil += getAmtIndi(*c);
                }
                lenChil += children.size() - 1;
                if (longestFam < lenChil) {
                    longestFam = lenChil;
                }
            }
        }
    }
    return longestFam;
}

void printFamStats(ofstream &outputFile, int &currID) {
    int* marr = FamArr[currID]->get_marr();
    int* divorce = FamArr[currID]->get_div();
    int remainSpace;
    cout << "| "<< currID;
    int lenID = getAmtIndi(currID);
    for (remainSpace = lenID; remainSpace < colID; ++remainSpace) {
        cout << " ";
    }
    int id = FamArr[currID]->get_husb();
    cout << " | ";
    if (id < 0) {
        cout << "NULL";
        lenID = 4;
    } else {
        cout << id;
        lenID = getAmtIndi(id);
    }
    for (remainSpace = lenID; remainSpace < colHusb; ++remainSpace) {
        cout << " ";
    }
    id = FamArr[currID]->get_wife();
    cout << " | ";
    if (id < 0) {
        cout << "NULL";
        lenID = 4;
    } else {
        cout << id;
        lenID = getAmtIndi(id);
    }
    for (remainSpace = lenID; remainSpace < colWife; ++remainSpace) {
        cout << " ";
    }
    cout << " |";
    // TODO: Print all children
    stringstream buffer;
    vector<int> children = FamArr[currID]->get_chil();
    string strChildren = "";
    if (children.size() <= 0) {
        strChildren = " NULL";
    } else {
        for (vector<int>::iterator c = children.begin(); c != children.end(); ++c) {
            string temp;
            buffer << *c;
            buffer >> temp;
            buffer.clear();
            strChildren += " " + temp;
        }
    }
    cout << strChildren;
    for (remainSpace = strChildren.length() - 1; remainSpace < colChil; ++remainSpace) {
        cout << " ";
    }
    string stringDate;
    if ((marr[0] == 0) && (marr[1] == 0) && (marr[2] == 0)) {
        stringDate = "NULL";
    } else {
        buffer << marr[1] << "/" << marr[0] << "/" << marr[2];
        buffer >> stringDate;
        buffer.clear();
    }
    cout << " | " << stringDate;
    for (remainSpace = stringDate.length(); remainSpace < colBirth; ++remainSpace) {
        cout << " ";
    }
    if ((divorce[0] == 0) && (divorce[1] == 0) && (divorce[2] == 0)) {
        stringDate = "NULL";
    } else {
        buffer << divorce[1] << "/" << divorce[0] << "/" << divorce[2];
        buffer >> stringDate;
        buffer.clear();
    }
    cout << " | " << stringDate;
    for (remainSpace = stringDate.length(); remainSpace < colDeath; ++remainSpace) {
        cout << " ";
    }
    cout << " |\n";
}

/*
 * Function prints value for Indi and Fam onto output screen
 */
// TODO: Tabulate the INDI and FAM info print to screen
void printIndiHeader(ofstream &outputFile, int longestName, int &maxIndi) {
    int lenColumn = 0;
    // Get amount of Indi ID
    lenColumn = getAmtIndi(maxIndi);
    if ((lenColumn % 2) == 1) {
        ++lenColumn;
    }
    int longestNum = lenColumn + 2;
    colID = lenColumn;
    lenColumn = (lenColumn - 2) / 2;
    cout << "| ";
    for (int i = 0; i < lenColumn; ++i) {
        cout << " ";
    }
    cout << "ID";
    for (int i = 0; i < lenColumn; ++i) {
        cout << " ";
    }
    cout << " | ";
    if ((longestName % 2) == 1) {
        ++longestName;
    }
    colName = longestName;
    lenColumn = (longestName - 4) / 2;
    for (int i = 0; i < lenColumn; ++i) {
        cout << " ";
    }
    cout << "NAME";
    for (int i = 0; i < lenColumn; ++i) {
        cout << " ";
    }
    cout << " | ";
    cout << "GENDER";
    cout << " | ";
    cout << "BIRTH DATE";
    cout << " | ";
    cout << "DEATH DATE";
    cout << " |";
    cout << "|\n";
    int i;
    if (longestNum + 2 < 4) {
        longestNum = 4;
        colID = 4;
    }
    for (i = 0; i < longestNum; ++i) {
        cout << "_";
    }
    cout << "|";
    if (longestName < 6) {
        longestName = 6;
        colName = 6;
    }
    for (i  = 0; i < longestName + 2; ++i) {
        cout << "_";
    }
    cout << "|";
    for (i  = 0; i < 8; ++i) {
        cout << "_";
    }
    cout << "|";
    for (i  = 0; i < 12; ++i) {
        cout << "_";
    }
    cout << "|";
    for (i  = 0; i < 12; ++i) {
        cout << "_";
    }
    cout << "|\n";
}

void printFamHeader(ofstream &outputFile, int &maxFam) {
    int lenColumn = 0;
    // Find length of longest ID
    lenColumn = getAmtIndi(maxFam);
    if ((lenColumn % 2) == 1) {
        ++lenColumn;
    }
    colID = lenColumn;
    lenColumn = (lenColumn - 2) / 2;
    cout << "| ";
    for (int i = 0; i < lenColumn; ++i) {
        cout << " ";
    }
    cout << "ID";
    for (int i = 0; i < lenColumn; ++i) {
        cout << " ";
    }
    cout << " | HUSB ID  |";
    cout << " WIFE ID  | ";
    // TODO: Reformat CHILDREN ID BAsed on # Children
    colChil = getLongestFamily(maxFam);
    if ((colChil % 2) == 1) {
        ++colChil;
    }
    lenColumn = (colChil - 12) / 2;
    for (int i = 0; i < lenColumn; ++i) {
        cout << " ";
    }
    cout << "CHILDREN ID ";
    for (int i = 0; i < lenColumn; ++i) {
        cout << " ";
    }
    cout << " | MARR DATE  |";
    cout << "  DIV DATE  |\n";
    int i;
    if (colID < 2) {
        colID = 2;
    }
    cout << "|_";
    for (i = 0; i < colID; ++i) {
        cout << "_";
    }
    cout << "_|_";
    for (i = 0; i < colHusb; ++i) {
        cout << "_";
    }
    cout << "_|_";
    for (i = 0; i < colWife; ++i) {
        cout << "_";
    }
    cout << "_|_";
    for (i = 0; i < colChil; ++i) {
        cout << "_";
    }
    cout << "_|_";
    for (i = 0; i < colMarr; ++i) {
        cout << "_";
    }
    cout << "_|_";
    for (i = 0; i < colDiv; ++i) {
        cout << "_";
    }
    cout << "_|\n";
}

void closeTable(bool isIndi) {
    int remainSpace;
    if (isIndi) {
        cout << "|_";
        for (remainSpace = 0; remainSpace < colID; ++remainSpace) {
            cout << "_";
        }
        cout << "_|_";
        for (remainSpace = 0; remainSpace < colName; ++remainSpace) {
            cout << "_";
        }
        cout << "_|_";
        for (remainSpace = 0; remainSpace < colGender; ++remainSpace) {
            cout << "_";
        }
        cout << "_|_";
        for (remainSpace = 0; remainSpace < colBirth; ++remainSpace) {
            cout << "_";
        }
        cout << "_|_";
        for (remainSpace = 0; remainSpace < colDeath; ++remainSpace) {
            cout << "_";
        }
        cout << "_|\n";
    } else {
        cout << "|_";
        for (remainSpace = 0; remainSpace < colID; ++remainSpace) {
            cout << "_";
        }
        cout << "_|_";
        for (remainSpace = 0; remainSpace < colHusb; ++remainSpace) {
            cout << "_";
        }
        cout << "_|_";
        for (remainSpace = 0; remainSpace < colWife; ++remainSpace) {
            cout << "_";
        }
        cout << "_|_";
        for (remainSpace = 0; remainSpace < colChil; ++remainSpace) {
            cout << "_";
        }
        cout << "_|_";
        for (remainSpace = 0; remainSpace < colMarr; ++remainSpace) {
            cout << "_";
        }
        cout << "_|_";
        for (remainSpace = 0; remainSpace < colDiv; ++remainSpace) {
            cout << "_";
        }
        cout << "_|\n";
    }
}

void printAllIndi(ofstream &outputFile, int &maxIndi, int &longestName) {
    printIndiHeader(outputFile, longestName, maxIndi);
    int currID;
    for (currID = 0; currID <= maxIndi; ++currID) {
        if (IndiArr[currID] != NULL) {
            printIndiStats(outputFile, currID);
        }
    }
    closeTable(true);
}

void printAllFam(ofstream &outputFile, int &maxFam) {
    printFamHeader(outputFile, maxFam);
    int currID;
    for (currID = 0; currID <= maxFam; ++currID) {
        if (FamArr[currID] != NULL) {
            printFamStats(outputFile, currID);
        }
    }
    closeTable(false);
}

void printScreen(ofstream &outputFile, int &maxIndi, int &maxFam, int &longestName) {
    printIndiHeader(outputFile, longestName, maxIndi);
    int currID;
    for (currID = 0; currID <= maxIndi; ++currID) {
        if (IndiArr[currID] != NULL) {
            PersonsAge(*IndiArr[currID], currID, maxIndi);
            printIndiStats(outputFile, currID);
        }
    }
    closeTable(true);
    for (currID = 0; currID <= maxIndi; ++currID) {
        if (IndiArr[currID] != NULL) {
            // Check Valid Birth
            checkValidBirth(*IndiArr[currID]);
            // Check for Younger Than 150
            if (olderThan150(IndiArr[currID]->get_birth(), IndiArr[currID]->get_death())) {
                cout << "Error US07: " << IndiArr[currID]->get_name() << " (" << IndiArr[currID]->get_id() <<
                        ") should be less than 150 years old.\n"<<errorline<<"\n";
            }
            // Check Individual Not Born Out of Wedlock
            checkWedlock(*IndiArr[currID]);
            // Corresponding entries
            isCorrespondingFamC(currID);
            isCorrespondingFamS(currID);
            checkBigamy(*IndiArr[currID]);
        }
    }
    cout << "\n";
    printFamHeader(outputFile, maxFam);
    for (currID = 0; currID <= maxFam; ++currID) {
        if (FamArr[currID] != NULL) {
            printFamStats(outputFile, currID);
        }
    }
    closeTable(false);
    for (currID = 0; currID <= maxFam; ++currID) {
        if (FamArr[currID] != NULL) {
            int memberID = FamArr[currID]->get_husb();
            if ((IndiArr[memberID] != NULL) && (memberID > -1)) {
                // Check if spouse has corresponding Indi entry
                if (IndiArr[memberID] == NULL) {
                    cout << "Error US26: Family " << FamArr[currID]->get_id() << " does not have corresponding husand record\n";
                } else if (!(IndiArr[memberID]->checkFamS(currID))) {
                    cout << "Error US26: " << IndiArr[memberID]->get_name() << " is not corresponding spouse in family " << FamArr[currID]->get_id() << "\n"<<errorline<<"\n";
                }
                //Check if husband was alive at time of marriage
                if (dateCompare(FamArr[currID]->get_marr(), IndiArr[memberID]->get_death()) == -1) {
                    cout << "Error US05: Husband died before being married.\n"<<errorline<<"\n";
                }
                if(dateCompare(FamArr[currID]->get_div(), IndiArr[memberID]->get_death()) == -1) {
                    cout << "Error US06: Divorce cannot happen, husband died before separation.\n"<<errorline<<"\n";
                }
                //Check if husband is male
                if (IndiArr[memberID]->get_sex() != true) {
                    cout << "Error US21: Husband is not male.\n"<<errorline<<"\n";
                }
                //Checks Husband isn't married to descendant
                if (FamArr[currID]->get_wife() != -1) {
                    checkParentDescendantMarriage(IndiArr[memberID], IndiArr[FamArr[currID]->get_wife()]);
                }
            }
            memberID = FamArr[currID]->get_wife();
            if ((IndiArr[memberID] != NULL) && (memberID > -1)) {
                // Check if spouse has corresponding Indi entry
                if (IndiArr[memberID] == NULL) {
                    cout << "Error US26: Family " << FamArr[currID]->get_id() << " does not have corresponding wife record\n"<<errorline<<"\n";
                } else if (!(IndiArr[memberID]->checkFamS(currID))) {
                    cout << "Error US26: " << IndiArr[memberID]->get_name() << " is not corresponding spouse in family " << FamArr[currID]->get_id() <<errorline<<"\n";
                }
                //Check if wife was alive at time of marriage
                if (dateCompare(FamArr[currID]->get_marr(), IndiArr[memberID]->get_death()) == -1) {
                    cout << "Error US05: Wife died before being married.\n"<<errorline<<"\n";
                }
                if(dateCompare(FamArr[currID]->get_div(), IndiArr[memberID]->get_death()) == -1) {
                    cout << "Error US06: Divorce cannot happen, wife died before separation.\n"<<errorline<<"\n";
                }
                //Check if wife is female
                if (IndiArr[memberID]->get_sex() != false) {
                    cout << "Error US21: Wife is not female.\n"<<errorline<<"\n";
                }
                //Checks Wife isn't married to descendant
                if (FamArr[currID]->get_husb() != -1) {
                    checkParentDescendantMarriage(IndiArr[memberID], IndiArr[FamArr[currID]->get_husb()]);
                }
            }
            //Checks for marriage before divorce
            if (dateCompare(FamArr[currID]->get_marr(), FamArr[currID]->get_div()) == -1) {
                cout << "Error US04: Family divorced before being married.\n"<<errorline<<"\n";
            }

            // Check unique family by spouse names and marriage date
            checkUniqueFamS(currID, maxFam);

            vector<int> childArr = FamArr[currID]->get_chil();
            int multBirthCount = 0;
            for (std::vector<int>::iterator it = childArr.begin(); it != childArr.end(); ++it) {
                // Check if corresponding entry for child in Indi entry
                if (IndiArr[*it] == NULL) {
                    cout << "Error US26: Family " << FamArr[currID]->get_id() << " does not have corresponding child record\n"<<errorline<<"\n";
                } else if (!(IndiArr[*it]->checkFamC(currID))) {
                    cout << "Error US26: " << IndiArr[*it]->get_name() << " is not a  corresponding child in family " << FamArr[currID]->get_id() <<errorline<<"\n";
                }
                for (std::vector<int>::iterator itCmp = childArr.begin(); itCmp != childArr.end(); ++itCmp) {
                    if ((IndiArr[*it] != NULL) && (IndiArr[*itCmp] != NULL)) {
                        if (IndiArr[*it]->get_id() != IndiArr[*itCmp]->get_id()) {
                            if (dateCompare(IndiArr[*it]->get_birth(), IndiArr[*itCmp]->get_birth()) == 0) {
                                multBirthCount++;
                                if (multBirthCount > 5) {
                                    break;
                                }
                            }
                        } else {
                            multBirthCount = 0;
                        }
                    }
                }
            }
            if (multBirthCount > 5) {
                cout << "Error US14: Family " << FamArr[currID]->get_id() << " has too many children born at once.\n"<<errorline<<"\n";
            }
            parentsNotTooOld(*FamArr[currID]);
            checkSiblingSpacing(*FamArr[currID]);
            checkSiblingMarriage(*FamArr[currID]);
            checkFirstRelativeMarriage(*FamArr[currID]);
            checkFutureDates(IndiArr[currID]->get_birth(), IndiArr[currID]->get_death(), FamArr[currID]->get_marr(), FamArr[currID]->get_div());
            checkUniqueIndi(currID, maxIndi);
            checkSameName(*FamArr[currID]);
            MarriedUnder14(*IndiArr[currID]);
        }
    }
    ++errorline;
}
int main() {
    // vector<string> files = get_all_files_names_within_folders("./GED");
    string fileNameInput;
    // Take input of ged file to take in as input
    cout << "Enter ged file name: ";
    getline(cin, fileNameInput);
    fileNameInput += ".ged";
    ifstream gedFile ("GED_Files/"+fileNameInput);
    ofstream outputFile;
    //int levelNumber = -1;

    if (gedFile.is_open()) {
        outputFile.open("output.txt", ios::out);
        if (outputFile.is_open()) {
            // Instantiate Fam and Indi Vector to NULL
            IndiArr.reserve(5001);
            FamArr.reserve(1001);
            for (int i = 0; i <= 5000; ++i) {
                IndiArr[i] = NULL;
            }
            for (int i = 0; i <= 1000; ++i) {
                FamArr[i] = NULL;
            }
            getCurrentDate();
            // Local Variables
            string line;
            int maxIndi = 0;
            int maxFam = 0;
            int longestName = 0;
            bool isIndi = true;
            int currIndex = 0; // Index of current data point
            colID = 0;
            colName = 0;
            colGender = 6;
            colBirth = 10;
            colDeath = 10;
            colHusb = 8;
            colWife = 8;
            colChil = 0;
            colMarr = 10;
            colDiv = 10;
            vector<string> parsed;

            getline(gedFile, line);
            int returnValue = 0;
            while (line != "") {
                if (returnValue > 0) {
                    if (line[0] == '2') {
                        addDate(isIndi, currIndex, returnValue, line);
                    }
                    returnValue = 0;
                }
                if (line[0] == '0') {
                    // Either INDI or FAM or HEAD/TAIL
                    parsed = parseLine(line);
                    // Look for unique id tags
                    if (parsed.size() > 2) {
                        if (isValidTag(false, false, parsed[2])) {
                            if (parsed[2] == "INDI") {
                                isIndi = true;
                                currIndex = createIndiFam(isIndi, parsed[1]);
                                if (currIndex > maxIndi) {
                                    maxIndi = currIndex;
                                }
                            } else if (parsed[2] == "FAM") {
                                isIndi = false;
                                currIndex = createIndiFam(isIndi, parsed[1]);
                                if (currIndex > maxFam) {
                                    maxFam = currIndex;
                                }
                            }
                        }
                    }
                } else if (line[0] == '1') {
                    if (isIndi) {
                        returnValue = addIndi(currIndex, line, longestName);
                    } else {
                        returnValue = addFam(currIndex, line);
                    }
                }
                getline(gedFile, line);
            }
            //printScreen(outputFile, maxIndi, maxFam, longestName);
            int option = 0;
            while (option != 8) {
                cout << "\nWhat would you like to do?\n";
                cout << "1. Show all INDI\n";
                cout << "2. Show all FAM\n";
                cout << "3. List all errors\n";
                cout << "4. List living spouses\n";
                cout << "5. List recent survivors\n";
                cout << "6. List recent birth\n";
                cout << "7. List recent death\n";
				cout << "8. List deceased\n";
				cout << "9. List orphans\n";
                cout << "10. Exit program\n";
                cout << "Input: ";
                cin >> option;
                while (cin.fail()) {
                    cout << "Failed to recognize input. Please try again...\n";
                    cout << "Input: ";
                    cin.clear();
                    cin.ignore(256, '\n');
                    cin >> option;
                }
                switch (option) {
                    case (1):
                        printAllIndi(outputFile, maxIndi, longestName);
                        break;
                    case (2):
                        printAllFam(outputFile, maxFam);
                        break;
                    case (3):
                        break;
                    case (4):
                        listLivingMarried(maxFam);
                        break;
                    case (5):
                        listRecentSurvivors(maxIndi);
                        break;
					case (6):
                        listRecentBirth(maxIndi);
                        break;
                    case (7):
                        listRecentDeath(maxIndi);
                        break;
		    case (8):
		        listDeceased(maxIndi);
			break;
		    case (9):
		        listOrphans(maxFam);
			break;
                    default:
                        break;
                }
            }
            cout << "\n";
            outputFile.close();
        } else {
            cout << "Unable to open output file.\n";
        }
        gedFile.close();
    } else {
        cout << "Unable to open GEDCOM file.\n";
    }
    return 0;
}
