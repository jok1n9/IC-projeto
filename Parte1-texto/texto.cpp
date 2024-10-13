// c++ learning
/*Objectives

    final- histograma de letras e palavras, etc...
            calculo da entropia

        transformar texto
        remover tags
        usar map
        simbolo peça de info, se for palavras é palavras, se for letras é letras
        para fazer histograma - gerar .csv ou o proprio programa gerar histograma e talvez guardar em formato de imagem

    
    ParteA: 1- Open and read a text file line by line or as a stream of characters.
            2- Store the content in a suitable data structure.
            3- Make sure the file opens successfully, and handle any errors
            4- Print the content to check if it has been read correctly.

    ParteB: Before performing statistical analysis, you can normalize the text by applying basic transformations
            such as:
            o Converting to lowercase: This ensures that case differences don’t affect the word counts.
            o Removing punctuation: Punctuation marks are typically ignored when calculating word
            frequencies.

    ParteC: Counting character frequencies (to calculate the frequency of characters, use a data structure such as
            std::map to count occurrences of each character).
            - Iterate through the text and count how many times each character appears.
            - Store this information in a data structure.
            - Print out the character frequencies.
            - Although not mandatory, you can use a plotting library to visualize the frequencies.

    ParteD:  Counting word frequencies
            - Tokenize the text into words using whitespace as the delimiter.
            - Store the word counts.
            - Print out the character frequencies.
            - Although not mandatory, you can use a plotting library to visualize the frequencies.


*/
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <algorithm>  
#include <cctype> 
#include <map>
#include <sstream>
#include <Windows.h> //if using windows



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



int main(){
    

    // Set console output to UTF-8 to handle Unicode characters properly in the console (if using windows)
    SetConsoleOutputCP(CP_UTF8);
    
    // Open the source file
    ifstream MyReadFile("pt/ep-00-01-17.txt");
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

    // Output the content and frequencies to the console 

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
    ofstream ProcessedFile("ProcessedFile33.txt");
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