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

using namespace std;

// Array of valid tags
string tags[] = {"INDI", "NAME", "SEX", "BIRT", "DEAT", "FAMC", "FAMS", "FAM", "MARR", "HUSB", "WIFE", "CHIL", "DIV", "DATE", "HEAD", "TRLR"};

/**
 * Splits the input string by spaces, removing '\r'
 */
vector<string> parseLine(string line) {
    vector<string> res;
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
bool isValidTag(string &tag) {
    bool isValidTag = false;
    // Iterate through each element in tags array and check if there is match
    for (const string &currTag : tags) {
        if (currTag == tag) {
            isValidTag = true;
        }
    }
    return isValidTag;
}
 
int main() {
    string fileNameInput;
    // Take input of ged file to take in as input
    cout << "Enter ged file name: ";
    getline(cin, fileNameInput);
    fileNameInput += ".ged";
    ifstream gedFile (fileNameInput);
    ofstream outputFile;
    int levelNumber = -1;
    
    if (gedFile.is_open()) {
        outputFile.open("output.txt", ios::out);
        if (outputFile.is_open()) {
            vector< Indi* > IndiArr;
            vector< Fam* > FamArr;
            IndiArr.reserve(5000);
            FamArr.reserve(1000);
            for (int i = 0; i < 5000; ++i) {
                IndiArr[i] = NULL;
            }
            for (int i = 0; i < 1000; ++i) {
                FamArr[i] = NULL;
            }
            string line, level, tag;
            vector<string> parsed;
            getline(gedFile, line);
            parsed = parseLine (line);
            level = parsed[0];
            tag = parsed[1];
            while (parsed.size() > 0) {
                cout << line << "\n";
                if (isValidTag(parsed[1]) || isValidTag(parsed[2])) {
                    // Print the original line
                    outputFile << line << "\n";
                    
                    // Parse Line for details
                    // parsed = parseLine (line);
                    // level = parsed[0];
                    // tag = parsed[1];
                    if (level == "0") {
                        // If a unique Identifier
                        if (tag[0] == '@') {
                            tag = parsed[2];
                        }
                        // Individual Unique ID
                        if (tag == "INDI") {
                            Indi* uniqueIndi = new Indi();
                            IndiArr.push_back(uniqueIndi);
                            while ( getline (gedFile, line) ) {
                                // Print the original line
                                outputFile << line << "\n";
                                
                                // Parse Line for details
                                parsed = parseLine (line);
                                level = parsed[0];
                                tag = parsed[1];
                                // Indi has values
                                if (level != "0") {
                                    if (tag == "NAME") {
                                        uniqueIndi->set_name(parsed[2] + parsed[3]);
                                        cout << uniqueIndi->get_name() << "\n";
                                    } else if (tag == "SEX") {
                                        if (parsed[2] == "M") {
                                            uniqueIndi->set_sex(true);
                                        } else {
                                            uniqueIndi->set_sex(false);
                                        }
                                    } else if (tag == "BIRT") {

                                    } else if (tag == "DEAT") {

                                    } else if (tag == "FAMS") {

                                    } else if (tag == "FAMC") {

                                    // Invalid tag
                                    } else {

                                    }
                                } else {
                                    break;
                                }
                            }
                        // Family Unique ID
                        } else if (tag == "FAM") {
                            Fam* uniqueFam = new Fam();
                            // REGEX
                            FamArr.push_back(uniqueFam);
                            while ( getline (gedFile, line) ) {
                                // Print the original line
                                outputFile << line << "\n";
                                
                                // Parse Line for details
                                parsed = parseLine (line);
                                level = parsed[0];
                                tag = parsed[1];
                                // Indi has values
                                if (level != "0") {
                                    if (tag == "HUSB") {
                                        //uniqueFam->set_husb();
                                    } else if (tag == "WIFE") {
                                        //uniqueFam->set_wife();
                                    } else if (tag == "CHIL") {
                                        //uniqueFam->add_chil();
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
                            level = parsed[0];
                            tag = parsed[1];
                        }
                    }
                    
                    // // Print the level number of each line
                    // outputFile << level << "\r\n";
                    // // Print the tag of each line and check if valid
                    // outputFile << validTag(level, tag) << "\r\n";
                } else {
                    getline(gedFile, line);
                    parsed = parseLine (line);
                    level = parsed[0];
                    tag = parsed[1];
                }
            }
            // for (vector< Indi* >::iterator it = IndiArr.begin(); it != IndiArr.end(); ++it) {
            //     cout << (*it)->get_sex() << "\n";
            // }
            for (int z = 0; z < (int)IndiArr.size(); ++z) {
                cout << z << " " << IndiArr[z]->get_name() << "\n";
            }
            outputFile.close();
        }
        else cout << "Unable to open output file.";
        gedFile.close();
    }
    else cout << "Unable to open GEDCOM file.";
    return 0;
}