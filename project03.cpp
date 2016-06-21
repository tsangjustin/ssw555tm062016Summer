/**
 * ssw555tm062016Summer
 * Harshit Singh, Justin Tsang, Weronika Zamlynny, Alexis Moore
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

// Array of valid tags
string levelZeroTags[] = {"INDI", "FAM"};
string IndiTags[] = {"NAME", "SEX", "BIRT", "DEAT", "FAMC", "FAMS"};
string FamTags[] = {"CHIL", "DIV", "HUSB", "WIFE"};
//"DATE", "HEAD", "TRLR"

// Global Variables
vector< Indi* > IndiArr;
vector< Fam* > FamArr;
int currDate[3];

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
	currDate[1] = localTime->tm_mon+1;
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

// Constructors
int createIndi(string &id) {
    int currID = getDigit(id);
    if (currID < 0) {
        return -1;
    }
    // Check if INDI id is unique
    if (IndiArr[currID] != NULL) {
        return -1;
    }
    // This INDI ID is unique
    Indi* uniqueIndi = new Indi();
    uniqueIndi->set_id(id);
    IndiArr[currID] = uniqueIndi;
    return currID;
}

int createFam(string &id) {
    int currID = getDigit(id);
    if (currID < 0) {
        return -1;
    }
    // Check if FAM id is unique
    if (FamArr[currID] != NULL) {
        return -1;
    }
    // This Fam ID is unique
    Fam* uniqueFam = new Fam();
    uniqueFam->set_id(id);
    FamArr[currID] = uniqueFam;
    return currID;
}

// Add values to constructors
int addIndi(int &index, string &line) {
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
                fullName += parsed[currName] + " ";
            }
            fullName += parsed[parsed.size() - 1];
            IndiArr[index]->set_name(fullName);
        }
    // Tag is Sez
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

bool addFam(int &index, string &line) {
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

bool addIndiDate(int &index, int &tag, string &line) {
    vector<string> parsed = parseLine(line);
    int day= 0;
    int mth = 0;
    int yr = 0;
    // If only provided year
    if (parsed.size() == 3) {
        istringstream buffer(parsed[2]);
        buffer >> yr;
    // If provided day, month, and year
    } else if (parsed.size() == 5) {
        stringstream buffer;
        buffer << parsed[2] << " " << parsed[4];
        buffer >> day >> yr;
        mth = convMonth(parsed[3]);
    } else {
        return false;
    }
    switch (tag) {
        // If tag that instigated date was BIRT
        case (1):
            IndiArr[index]->set_birth(day, mth, yr);
            break;
        // If tag that instigated date was DEAT
        case (2):
            IndiArr[index]->set_death(day, mth, yr);
            break;
        default:
            return false;
    }
    return true;
}

/*
 * Function adds date to attribute to the index at FamArr
 */
bool addFamDate(int &index, int &tag, string &line) {
    vector<string> parsed = parseLine(line);
    int day= 0;
    int mth = 0;
    int yr = 0;
    // If only provided year
    if (parsed.size() == 3) {
        istringstream buffer(parsed[2]);
        buffer >> yr;
    // If provided day, month, and year
    } else if (parsed.size() == 5) {
        stringstream buffer;
        buffer << parsed[2] << " " << parsed[4];
        buffer >> day >> yr;
        mth = convMonth(parsed[3]);
    } else {
        return false;
    }
    switch (tag) {
        // If tag instigated was MARR
        case (1):
            FamArr[index]->set_marr(day, mth, yr);
            break;
        // If tag instigated was DIV
        case (2):
            FamArr[index]->set_div(day, mth, yr);
            break;
        default:
            return false;
    }
    return true;
}

/*
 * Function compares dates of two given int array
 */
int dateCompare(int *arrA, int *arrB) {
    int retCmp = 0;
    // TODO YEAR 0
    //Compares years
    if(arrA[2] > arrB[2]) {
        retCmp = -1;
    } else if(arrA[2] < arrB[2]) {
        retCmp = 1;
    } else {
        //compare months
        if(arrA[1] > arrB[1]) {
            retCmp = -1;
        } else if(arrA[1] > arrB[1]) {
            retCmp = 1;
        } else {
            //compare dates
            if(arrA[0] > arrB[0]) {
                retCmp = -1;
            } else if(arrA[0] > arrB[0]) {
                retCmp = 1;
            } else {
                retCmp = 0;
            }
        }   
    }
    return retCmp;
    // Returns -1 if DateA is later than DateB
    // Returns  1 if DateA is earlier than DateB
    // Returns  0 if DateA is the same as DateB
}

/*
* Checks if an individual has a valid birth date.
*/
bool checkValidBirth(Indi &indi) {
    int* birth = indi.get_birth();

    // Individual was born before they died
    int* death = indi.get_death();
    if (dateCompare(birth, death) < 0) {
        cout << "Error: Individual cannot die before they are born.\n";
        return false;
    }

    // Individual was born before their parents died
    vector <int> famc = indi.get_famc();
    for (std::vector<int>::iterator f = famc.begin(); f != famc.end(); ++f) {
        Fam *fam = FamArr[*f];

        // Getting parents
        Indi* mom = IndiArr[fam->get_wife()];
        Indi* dad = IndiArr[fam->get_husb()];

        int* momDeath = mom->get_death();
        int* dadDeath = dad->get_death();

        // Cannot be born after death of mother
        if (dateCompare(birth, momDeath) < 0) {
            cout << "Error: Individual cannot be born after the death of mother.\n";
            return false;
        }
        // Cannot be born 9 months after death of father
        int* dDeath = new int[3];
        for (int i = 0; i < 3; i++) {
            //Creating a deep copy of dadDeath
            dDeath[i] = dadDeath[0];
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
            cout << "Error: Individual cannot be born 9 months after the death of father.\n";
            return false;
        }
    }

    // Individual was born before they got married
    vector<int> fams = indi.get_fams();
    for (std::vector<int>::iterator f = fams.begin(); f != fams.end(); ++f) {
        Fam* fam = FamArr[*f];

        int* marr = fam->get_marr();
        if (dateCompare(birth, marr) < 0){
            cout << "Error: Individual cannot be married before birth.\n";
            return false;
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
		if(family->get_marr()[0] != 0 && family->get_marr()[1] != 0 && family->get_marr()[2] != 0) {
			if(dateCompare(birth, family->get_marr()) == 1) {
				cout << "Error: Child born out of wedlock. \n";
			}
			else
			{
				if(family->get_div()[0] != 0 && family->get_div()[1] != 0 && family->get_div()[2] != 0) {
					if(dateCompare(birth, family->get_div()) == -1) {
						cout << "Error: Child born out of wedlock. \n";
					}
				}
			}
		}
		else {
			cout << "Error: Child born out of wedlock. \n";
		}
	}
}


// Checks to make sure no one is over 150 years old
bool olderThan150(int *birth, int *death)
{
	bool retComp;
	getCurrentDate();
	//Check if individual has died
	if(death[2] == 0) {
		//check against year
		if((currDate[2] - birth[2]) > 150) {
			retComp = true;
		}
		else if((currDate[2] - birth[2]) == 150) {
			//check against month
			if((currDate[1] - birth[1]) >= 0) {
				retComp = true;
			}
			else if((currDate[1] - birth[1]) == 0) {
				//check against day
				if((currDate[0] - birth[0]) >= 0) {
					retComp = true;
				}
				else {
					retComp = false;
				}
			}
			else {
				retComp = false;
			}
		}
		else {
			retComp = false;
		}
	}
	//check year of death against birth
	else if((death[2] - birth[2]) > 150) {
		retComp = true;
	}
	//check year of death against birth
	else if((death[2] - birth[2]) == 150) {
		//check month of death against birth
		if((death[1] - birth[1]) >= 0) {
			retComp = true;
		}
		//check date of death against birth
		else if((death[1] - birth[1]) == 0) {
			if((death[0] - birth[0]) >= 0) {
				retComp = true;
			}
			else {
				retComp = false;
			}
		}
		else {
			retComp = false;
		}
	}
	else {
		retComp = false;
	}
	if(retComp == true) {
		cout << "Error: Individual should be less than 150 years old.\n";
	}
	return retComp;
}

/*
 * Function prints value for Indi and Fam onto output screen
 */
void printScreen(ofstream &outputFile, int &maxIndi, int &maxFam) {
    cout << "Printing...\n";
    int currID;
    for (currID = 0; currID <= maxIndi; ++currID) {
        if (IndiArr[currID] != NULL) {
            cout << "INDI ID: " << IndiArr[currID]->get_id() << "\n";
            cout << "Name: " << IndiArr[currID]->get_name() << "\n";
            int* birthDate = IndiArr[currID]->get_birth();
            cout << "Birth: " << birthDate[0] << " " << birthDate[1] << " " << birthDate[2] << "\n";
			// Check Valid Birth
			checkValidBirth(*IndiArr[currID]);
			// Check for Younger Than 150
			olderThan150(IndiArr[currID]->get_birth(), IndiArr[currID]->get_death());
			// Check Individual Not Born Out of Wedlock
			checkWedlock(*IndiArr[currID]);
			//Add entries to output file
            outputFile << "INDI ID: " << IndiArr[currID]->get_id() << "\n";
            outputFile << "Name: " << IndiArr[currID]->get_name() << "\n";
            // Corresponding entries
            vector<int> family = IndiArr[currID]->get_famc();
            for (vector<int>::iterator it = family.begin(); it != family.end(); ++it) {
                if ((FamArr[*it] == NULL) || (!(FamArr[*it]->checkChild(currID)))) {
                    cout << IndiArr[currID]->get_name() << " is not corresponding child in family " << FamArr[*it]->get_id() << "\n";
                }
            }
            family = IndiArr[currID]->get_fams();
            for (vector<int>::iterator it = family.begin(); it != family.end(); ++it) {
                //cout << FamArr[*it]->get_husb() << " " << FamArr[*it]->get_wife() << "\n";
                if ((FamArr[*it] == NULL) || (!(currID == FamArr[*it]->get_husb())) && (!(currID == FamArr[*it]->get_wife()))) {
                    cout << IndiArr[currID]->get_name() << " is not corresponding spouse in family " << FamArr[*it]->get_id() << "\n";
                }
            }
        }
    }
    for (currID = 0; currID <= maxFam; ++currID) {
        if (FamArr[currID] != NULL) {
            cout << "FAM ID: " << FamArr[currID]->get_id() << "\n";
            outputFile << "FAM ID: " << FamArr[currID]->get_id() << "\n";
            int memberID = FamArr[currID]->get_husb(); 
            if ((IndiArr[memberID] != NULL) && (memberID > -1)) {
                cout << "Husband: " << IndiArr[memberID]->get_name() << "\n";
                outputFile << "Husband: " << IndiArr[memberID]->get_name() << "\n";
                // Check if spuse has corresponding Indi entry
                if (IndiArr[memberID] == NULL) {
                    cout << "Family " << FamArr[currID]->get_id() << " does not have corresponding husand record\n";
                } else if (!(IndiArr[memberID]->checkFamS(currID))) {
                    cout << IndiArr[memberID]->get_name() << " is not corresponding spouse in family " << FamArr[currID]->get_id() << "\n";
                }
            }
            memberID = FamArr[currID]->get_wife(); 
            if ((IndiArr[memberID] != NULL) && (memberID > -1)) {
                cout << "Wife: " << IndiArr[memberID]->get_name() << "\n";
                outputFile << "Wife: " << IndiArr[memberID]->get_name() << "\n";
                // Check if spuse has corresponding Indi entry
                if (IndiArr[memberID] == NULL) {
                    cout << "Family " << FamArr[currID]->get_id() << " does not have corresponding wife record\n";
                } else if (!(IndiArr[memberID]->checkFamS(currID))) {
                    cout << IndiArr[memberID]->get_name() << " is not corresponding spouse in family " << FamArr[currID]->get_id() << "\n";
                }
            }
            // Check unique family by spouse names and marriage date
            for (int restFamID = currID + 1; restFamID <= maxFam; ++restFamID) {
                if (FamArr[restFamID] != NULL) {
                    int currIndiID = FamArr[currID]->get_husb();
                    int nextIndiID = FamArr[restFamID]->get_husb();
                    if (currIndiID == -1 && nextIndiID == -1) {
                        currIndiID = FamArr[currID]->get_wife();
                        nextIndiID = FamArr[restFamID]->get_wife();
                        if (currIndiID == -1 && nextIndiID == -1) {
                            if (dateCompare(FamArr[currID]->get_marr(), FamArr[restFamID]->get_marr()) == 0) {
                                cout << "Families " << FamArr[currID]->get_id() << " and " << FamArr[restFamID]->get_id() << " have same marriage dates\n";
                            }
                        }
                    }
                    else if (currIndiID > 0 && nextIndiID > 0) {
                        Indi* currIndi = IndiArr[currIndiID];
                        Indi* nextIndi = IndiArr[nextIndiID];
                        if (currIndi != NULL && nextIndi != NULL) {
                            if (currIndi->get_name() == nextIndi->get_name()) {
                                currIndiID = FamArr[currID]->get_wife();
                                nextIndiID = FamArr[restFamID]->get_wife();
                                if (currIndiID > 0 && nextIndiID > 0) {
                                    Indi* currIndi = IndiArr[currIndiID];
                                    Indi* nextIndi = IndiArr[nextIndiID];
                                    if (currIndi != NULL && nextIndi != NULL) {
                                        if (currIndi->get_name() == nextIndi->get_name()) {
                                            if (dateCompare(FamArr[currID]->get_marr(), FamArr[restFamID]->get_marr()) == 0) {
                                                cout << "Families " << FamArr[currID]->get_id() << " and " << FamArr[restFamID]->get_id() << " have same spouse names and marriage dates\n";
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
            vector<int> childArr = FamArr[currID]->get_chil();
            int multBirthCount = 0;
            for (std::vector<int>::iterator it = childArr.begin(); it != childArr.end(); ++it) {
                // Check if corresponding entry for child in Indi entry
                if (IndiArr[*it] == NULL) {
                    cout << "Family " << FamArr[currID]->get_id() << " does not have corresponding child record\n";
                } else if (!(IndiArr[*it]->checkFamC(currID))) {
                    cout << IndiArr[*it]->get_name() << " is not a  corresponding child in family " << FamArr[currID]->get_id() << "\n";
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
                        }
                        else {
                            multBirthCount = 0;
                        }
                    }
                }
            }
            if (multBirthCount > 5) {
                cout << "Error: Too Many Children Born at Once. \n";
            }
        }
    }
}

int main() {
    string fileNameInput;
    // Take input of ged file to take in as input
    cout << "Enter ged file name: ";
    getline(cin, fileNameInput);
    fileNameInput += ".ged";
    ifstream gedFile (fileNameInput);
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
            // Local Variables
            string line;
            int maxIndi = 0;
            int maxFam = 0;
            bool isIndi = true;
            int currIndex = 0; // Index of current data point
            vector<string> parsed;

            getline(gedFile, line);
            int returnValue = 0;
            while (line != "") {
                if (returnValue > 0) {
                    if (line[0] == '2') {
                        if (isIndi) {
                            addIndiDate(currIndex, returnValue, line);
                        } else {
                            addFamDate(currIndex, returnValue, line);
                        }
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
                                currIndex = createIndi(parsed[1]);
                                if (currIndex > maxIndi) {
                                    maxIndi = currIndex;
                                }
                            } else if (parsed[2] == "FAM") {
                                isIndi = false;
                                currIndex = createFam(parsed[1]);
                                if (currIndex > maxFam) {
                                    maxFam = currIndex;
                                }
                            }
                        }
                    }
                } else if (line[0] == '1') {
                    if (isIndi) {
                        returnValue = addIndi(currIndex, line);
                    } else {
                        returnValue = addFam(currIndex, line);
                    }
                }
                getline(gedFile, line);
            }
			
			
            printScreen(outputFile, maxIndi, maxFam);
            outputFile.close();
        } else {
            cout << "Unable to open output file.";
        }
        gedFile.close();
    } else {
        cout << "Unable to open GEDCOM file.";
    }
    return 0;
}
