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

/**
 * Splits the input string by spaces, removing '\r'
 */
vector<string> parseLine(string line) {
    vector<string> res;
    int whitespaceIndex;
    string paramToken;
    for (int paramIndex = 0; paramIndex < 2; ++paramIndex) {
        whitespaceIndex = line.find(' ');
        paramToken = line.substr(0, whitespaceIndex);
        line = line.substr(whitespaceIndex + 1);
        res.push_back(paramToken);
    }
    res.push_back(line);
    return res;
}

/**
 * Checks that the tag is in the list of valid tags for its level 
 */
string validTag(string level, string tag) {
    string error = "Invalid Tag";
    if (level == "0") {
        string validTags [] = {"INDI","FAM","HEAD","TRLR","NOTE"};
        for (int i = 0; i < (int)sizeof(validTags)/(int)sizeof(validTags[0]); i++) {
            if (tag == validTags[i]){
                return tag;
            }
        }
        return error;
    }
    if (level == "1") {
        string validTags [] = {"NAME","SEX","BIRT","DEAT","FAMC","FAMS","MARR","HUSB","WIFE","CHIL","DIV"};
        for (int i = 0; i < (int)sizeof(validTags)/(int)sizeof(validTags[0]); i++) {
            if (tag == validTags[i]){
                return tag;
            }
        }
        return error;
    }
    if (level == "2") {
        string validTags [] = {"DATE"};
        for (int i = 0; i < (int)sizeof(validTags)/(int)sizeof(validTags[0]); i++) {
            if (tag == validTags[i]){
                return tag;
            }
        }
        return error;
        
    }
    else {
        return error;
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
    
    vector< Indi* > IndiArr;
    vector< Fam* > FamArr;
    string line, level, tag;
    
    if (gedFile.is_open()) {
        outputFile.open("output.txt", ios::out);
        if (outputFile.is_open()) {
            vector<string> parsed;
            while ( getline (gedFile,line) ) {
                // Print the original line
                outputFile << line << "\n";
                
                // Parse Line for details
                parsed = parseLine (line);
                level = parsed[0];
                tag = parsed[1];
                if (level == "0") {
                    // If a unique Identifier
                    if (tag[0] == '@') {
                        tag = parsed[2];
                    }
                    // Individual Unique ID
                    if (tag == "INDI") {
                        Indi* uniqueIndi =  new Indi();
                        while ( getline (gedFile, line) ) {
                            // Print the original line
                            outputFile << line << "\n";
                            
                            // Parse Line for details
                            parsed = parseLine (line);
                            level = parsed[0];
                            tag = parsed[1];
                            // Indi has values
                            if (level == "0") {
                                IndiArr.push_back(uniqueIndi);
                                break;
                            }
                            if (tag == "NAME") {
                                uniqueIndi->set_name(parsed[2] + parsed[3]);
                                cout << uniqueIndi->get_name() << "\n";
                            } else if (tag == "SEX") {

                            } else if (tag == "BIRT") {

                            } else if (tag == "DEAT"){

                            } else if (tag == "FAMS") {

                            } else if (tag == "FAMC") {

                            // Invalid tag
                            } else {

                            }
                        }
                    // Family Unique ID
                    } else {

                    }
                }
                
                // Print the level number of each line
                outputFile << level << "\r\n";
                // Print the tag of each line and check if valid
                outputFile << validTag(level, tag) << "\r\n";
            }
            
            outputFile.close();
        }
        else cout << "Unable to open output file.";
        gedFile.close();
    }
    else cout << "Unable to open GEDCOM file."; 
    // for (vector< Indi* >::iterator it = IndiArr.begin(); it != IndiArr.end(); ++it) {
    //     cout << (*it)->get_sex() << "\n";
    // }
    for (int z = 0; z < (int)IndiArr.size(); ++z) {
        cout << z << " " << IndiArr[z]->get_name() << "\n";
    }
    return 0;
}