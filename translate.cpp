#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include <algorithm>
#include <map>
#include <iomanip>
#include <vector>
#include <tuple>
#include <utility>


#include <sys/resource.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
using namespace std;

long getMemoryUsage() {
    struct rusage myusage;

    getrusage(RUSAGE_SELF, &myusage);
    return myusage.ru_maxrss;
}

int main() {
    clock_t start, end;
    start = clock();
    fstream inputFile, outputFile, dictFile, frequencyFile, performanceFile;
    string line, word;
    string inputFileName, outputFileName, dictFileName, frequencyFileName, performanceFileName;
  
    inputFileName = "t8.shakespeare.txt";
    outputFileName = "t8.shakespeare.translated.txt";
    dictFileName = "french_dictionary.csv";
    frequencyFileName = "frequency.csv";
    performanceFileName = "performance.txt";

    inputFile.open(inputFileName, ios::in);
    outputFile.open(outputFileName, ios::out);
    dictFile.open(dictFileName, ios::in);

    map<string, string> dictionary;
    map<pair<string, string>, int> freqeuncy;
    bool index = 0;
    string key, value;
    while (getline(dictFile, line)) {
        stringstream s(line);
        while (getline(s, word, ',')) {
            if (index)
                value = word;
            else
                key = word;
            index = !index;
        }
        dictionary.insert(pair<string, string>(key, value));
    }

    while (getline(inputFile, line)) {
        istringstream iss(line);
        string orgLine = line;
        while (iss >> word) {
            string orgWord = word;
            transform(word.begin(), word.end(), word.begin(), ::tolower);
            word.erase(remove_if(word.begin(), word.end(), []( auto const& c ) -> bool { return !isalpha(c); } ), word.end());
            map<string, string>::iterator key = dictionary.find(word);
            if (key != dictionary.end()) {
                string translatedWord = key->second; 
                size_t index = 0;
                while ((index = orgLine.find(orgWord, index)) != string::npos) {

                    map<pair<string, string>, int>::iterator freqeuncyKey = freqeuncy.find(pair<string, string>(word, translatedWord));
                    if (freqeuncyKey != freqeuncy.end())
                        freqeuncyKey->second++;
                    else
                        freqeuncy.insert(pair<pair<string, string>, int>(pair<string, string>(word, translatedWord), 0));

                    if (all_of(orgWord.begin(), orgWord.end(), [](unsigned char c){ return ::isupper(c); }))
                        transform(translatedWord.begin(), translatedWord.end(), translatedWord.begin(), ::toupper);
                    else if (isupper(orgWord[0]))
                        translatedWord[0] = toupper(translatedWord[0]);
                    orgLine.replace(orgLine.find(orgWord), orgWord.length(), translatedWord);
                    index += key->second.length();
                }
            }
        }
        outputFile << orgLine << endl;
    }

    inputFile.close();
    outputFile.close();

    frequencyFile.open(frequencyFileName, ios::out);
    
    map<pair<string, string>, int>::iterator iter;
    for(iter = freqeuncy.begin(); iter!= freqeuncy.end(); iter++)
        frequencyFile << iter->first.first << "," << iter->first.second << "," << iter->second << endl;

    frequencyFile.close();

    end = clock();
    long memoryUsage = getMemoryUsage();

    performanceFile.open(performanceFileName, ios::out);
    performanceFile << "Time to process : " << double(end - start) / double(CLOCKS_PER_SEC) << " seconds" << endl;
    performanceFile << "Memory used : " << double(getMemoryUsage()) / 1000 << " MB" << endl;

    performanceFile.close();

    return 0;
}