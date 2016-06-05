/**
 * ssw555tm062016Summer
 * Harshit Singh, Justin Tsang, Weronika Zamlynny
 */

// #ifndef INDI_H
// #define INDI_H
// #endif
// #ifndef FAM_H
// #define FAM_H
// #endif
#include "indi.h"
#include "fam.h"
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>

using namespace std;

// Array of valid tags
string levelZeroTags[] = {"INDI", "FAM"};
string levelOneTags[] = {"NAME", "SEX", "BIRT", "DEAT", "FAMC", "FAMS", "MARR", "HUSB", "WIFE", "CHIL", "DIV", "DATE", "HEAD", "TRLR"};

/**
 * Splits the input string by spaces, removing '\r'
 */
vector<string> parseLine(string line) {
    vector<string> res;
    res.clear();
    string buffer = "";
    int strLen = line.length();
    for (int i = 0; i < strLen; i++) {
        if (line[i] == ' ') {
            // Add to vector and clear buffer
            res.push_back(buffer);
            buffer = "";
        }
        else if (line[i] != '\r') {
            buffer += line[i];
        }
    }
    if (buffer != "") {
        res.push_back(buffer);
    }
    return res;
}

/**
 * Checks that the tag is in the list of valid tags for its level 
 */
bool isValidTag(bool isLevelOneTag, string &tag) {
    bool isValidTag = false;
    // Check if user/fam info tag
    if (isLevelOneTag) {
        // Iterate through each element in tags array and check if there is match
        for (const string &currTag : levelOneTags) {
            if (currTag == tag) {
                isValidTag = true;
            }
        }
    // Check if unique ID Tag
    } else {
        // Iterate through each element in tags array and check if there is match
        for (const string &currTag : levelZeroTags) {
            if (currTag == tag) {
                isValidTag = true;
            }
        }
    }
    return isValidTag;
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
            // Ceate vector pointer to Indi and Fam
            vector< Indi* > IndiArr;
            vector< Fam* > FamArr;
            IndiArr.reserve(5001);
            FamArr.reserve(1001);
            for (int i = 0; i <= 5000; ++i) {
                IndiArr[i] = NULL;
            }
            for (int i = 0; i <= 1000; ++i) {
                FamArr[i] = NULL;
            }
            // Begin reading line of ged file
            string line, level, tag;
            int maxIndi = 0;
            int maxFam = 0;
            vector<string> parsed;
            getline(gedFile, line);
            parsed = parseLine (line);
            // While there is more line to read
            while (parsed.size() > 0) {
                // Print the original line
                outputFile << line << "\n";
                //parsed = parseLine (line);
                // Look for unique id tags
                if (parsed.size() > 2) {
                    level = parsed[0];
                    tag = parsed[2];
                    if ((level == "0") && (isValidTag(false, tag))) {
                        // Parse Line for details
                        // parsed = parseLine (line);
                        // level = parsed[0];
                        // tag = parsed[1];
                        // Individual Unique ID
                        if (tag == "INDI") {
                            Indi* uniqueIndi = new Indi();
                            uniqueIndi->set_id(parsed[1]);
                            int indexID = getDigit(parsed[1]);
                            if (indexID > maxIndi) {
                                maxIndi = indexID;
                            }
                            IndiArr[indexID] = uniqueIndi;
                            while ( getline (gedFile, line) ) {
                                // Print the original line
                                outputFile << line << "\n";
                                
                                // Parse Line for details
                                parsed = parseLine (line);
								if (parsed.size() > 2) {
									level = parsed[0];
									tag = parsed[1];
									// Indi has values
									if ((level != "0") && (isValidTag(true, tag))) {
										if (tag == "NAME") {
                                            string fullName = "";
                                            for (int currName = 2; currName < parsed.size(); ++currName) {
                                                fullName += parsed[currName] + " ";
                                            }
											uniqueIndi->set_name(fullName);
										} else if (tag == "SEX") {
											if (parsed[2] == "M") {
												uniqueIndi->set_sex(true);
											} else {
												uniqueIndi->set_sex(false);
											}
										} else if (tag == "BIRT") {

										} else if (tag == "DEAT") {

										} else if (tag == "FAMS") {
                                            if ((indexID = getDigit(parsed[2])) > -1) {
                                                uniqueIndi->add_fams(indexID);
                                            }
										} else if (tag == "FAMC") {
                                            if ((indexID = getDigit(parsed[2])) > -1) {
                                                uniqueIndi->add_famc(indexID);
                                            }
										// Invalid tag
										} else {

										}
									} else {
										break;
									}
								}
                            }
                        // Family Unique ID
                        } else if (tag == "FAM") {
                            Fam* uniqueFam = new Fam();
                            uniqueFam->set_id(parsed[1]);
                            int indexID = getDigit(parsed[1]);
                            if (indexID > maxFam) {
                                maxFam = indexID;
                            }
                            FamArr[indexID] = uniqueFam;
                            while ( getline (gedFile, line) ) {
                                // Print the original line
                                outputFile << line << "\n";
                                
                                // Parse Line for details
                                parsed = parseLine (line);
                                level = parsed[0];
                                tag = parsed[1];
                                // Fam has values
                                if ((level != "0") && (isValidTag(true, tag))) {
                                    if (tag == "HUSB") {
                                        if ((indexID = getDigit(parsed[2])) > -1) {
                                            uniqueFam->set_husb(indexID);
                                        }
                                    } else if (tag == "WIFE") {
                                        if ((indexID = getDigit(parsed[2])) > -1) {
                                            uniqueFam->set_wife(indexID);
                                        }
                                    } else if (tag == "CHIL") {
                                        if ((indexID = getDigit(parsed[2])) > -1) {
                                            uniqueFam->add_chil(indexID);
                                        }
                                    } else if (tag == "DIV") {

                                    } else if (tag == "MARR") {

                                    // Invalid tag
                                    } else {

                                    }
                                } else {
                                    break;
                                }
                            }
                        } else {
                            getline(gedFile, line);
							parsed = parseLine (line);
							// level = parsed[0];
							// tag = parsed[1];
                        }
                    } else {
						getline(gedFile, line);
						parsed = parseLine (line);
						// level = parsed[0];
						// tag = parsed[1];
					}
                    // // Print the level number of each line
                    // outputFile << level << "\r\n";
                    // // Print the tag of each line and check if valid
                    // outputFile << validTag(level, tag) << "\r\n";
                } else {
                    getline(gedFile, line);
                    parsed = parseLine (line);
                }
            }
            // for (vector< Indi* >::iterator it = IndiArr.begin(); it != IndiArr.end(); ++it) {
            //     cout << (*it)->get_sex() << "\n";
            // }
            int currID;
            for (currID = 0; currID <= maxIndi; ++currID) {
                if (IndiArr[currID] != NULL) {
                    cout << "INDI ID: " << IndiArr[currID]->get_id() << "\n";
                    cout << "Name: " << IndiArr[currID]->get_name() << "\n";
                }
            }
            for (currID = 0; currID <= maxFam; ++currID) {
                if (FamArr[currID] != NULL) {
                    cout << "FAM ID: " << FamArr[currID]->get_id() << "\n";
                    int memberID = FamArr[currID]->get_husb(); 
                    if (memberID > -1) {
                        cout << "Husband: " << IndiArr[memberID]->get_name() << "\n";
                    }
                    memberID = FamArr[currID]->get_wife(); 
                    if (memberID > -1) {
                        cout << "Wife: " << IndiArr[memberID]->get_name() << "\n";
                    }
                }
            }
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