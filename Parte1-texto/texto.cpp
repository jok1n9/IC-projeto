#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <algorithm>  
#include <cctype> 
#include <map>
#include <sstream>

#ifdef _WIN32  
#include <Windows.h> //if using windows
#endif


using namespace std;

class TextProcessor{
public:

    string processText(const string& input) {
        string processed = input;

        // Convert to lowercase
        transform(processed.begin(), processed.end(), processed.begin(), [](unsigned char c) {
            return tolower(c);
        });

        // Remove punctuation
        processed.erase(remove_if(processed.begin(), processed.end(), [](unsigned char c) {
            return ispunct(c);
        }), processed.end());

        return processed;
    }

    // Function to count char frequencies
    map<char, int> countCharacterFrequencies(const string& input) {
        map<char, int> frequencyMap;

        for (char c : input) {
            frequencyMap[c]++; // Increment frequency count
        }

        return frequencyMap;
    }

    // Function to count word frequencies
    map<string, int> countWordFrequencies(const string& input) {
        map<string, int> wordFrequencyMap;
        istringstream stream(input);
        string word;

        while (stream >> word) {
            wordFrequencyMap[word]++; // Increment word frequency count
        }
        return wordFrequencyMap;
    }
};



int main(int argc, char*argv[]){
    

    // Set console output to UTF-8 to handle Unicode characters properly in the console (if using windows)
    #ifdef _WIN32  
    SetConsoleOutputCP(CP_UTF8);
    #endif
    
    if (argc < 2) {
        cerr << "Usage: " << argv[0] << " <file_path>" << endl;
        return 1;
    }
    
    string file_path = argv[1];
    // Open the source file
    ifstream MyReadFile(file_path);
    if (!MyReadFile) {
        cerr << "Failed to open source file!" << endl;
        return 1;
    }


    vector<string> content;
    TextProcessor textProcessor;
    string line;
    map<char,int> charFreq;
    map<string,int> wordFreq;

    // Read the content of the file 

    while (getline(MyReadFile, line)) {

        string proccessLine = textProcessor.processText(line);

        content.push_back(proccessLine);  // Store each line in the vector

        auto frequencies = textProcessor.countCharacterFrequencies(proccessLine);

        for (const auto& pair : frequencies) {
            charFreq[pair.first] += pair.second;
        }

        auto wordFrequencies = textProcessor.countWordFrequencies(proccessLine);

        for (const auto& pair : wordFrequencies) {
            wordFreq[pair.first] += pair.second;
        }

    }

    // Output the content and frequencies to the console (optional)

    /*
    for (const auto& pair : charFreq) {
        cout << pair.first << ": " << pair.second << endl;
    }

     for (const auto& pair : wordFreq) {
        cout << pair.first << ": " << pair.second << endl;
    }
    */

    /*for (size_t i = 0; i < content.size(); ++i) {
        cout << content[i] << endl;
    }*/
    //cout << content << endl;
    
    // Open the destination file
    ofstream ProcessedFile("ProcessedFile.txt");
    if (!ProcessedFile) {
        cerr << "Failed to open destination file!" << endl;
        return 1;
    }

    // Write the content to the destination file

    for (size_t i = 0; i < content.size(); ++i) {
        ProcessedFile << content[i] << endl;
    }

    cout << "Destination file updated\n" << endl;

    // Open the character file

    ofstream charCountFile("charCount.txt");
    if (!charCountFile) {
        cerr << "Failed to open characters file!" << endl;
        return 1;
    }

    // Write the content to the character file

    charCountFile<<"Character,Frequency\n";

    for (const auto& pair : charFreq) {
        charCountFile << pair.first << ": " << pair.second << endl;
    }

    cout<<"Character file updated\n";

    // Open the word file

    ofstream wordCountFile("wordCount.txt");
    if (!wordCountFile) {
        cerr << "Failed to open characters file!" << endl;
        return 1;
    }

    // Write the content to the word file

    wordCountFile<<"Word,Frequency\n";

    for (const auto& pair : wordFreq) {
        wordCountFile << pair.first << ": " << pair.second << endl;
    }

    cout<<"Word file updated\n";

    // Close the files
    MyReadFile.close();
    ProcessedFile.close();
    charCountFile.close();
    wordCountFile.close();
    return 0;
}