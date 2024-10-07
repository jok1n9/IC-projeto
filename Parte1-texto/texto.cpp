// c++ learning
// reading and writing files


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
*/
#include <iostream>
#include <fstream>
#include <string>

using namespace std;

int main(){

    string myText;

    ifstream MyReadFile;

    ofstream ProcessedFile;

    MyReadFile.open("pt/ep-00-01-17.txt");

    ProcessedFile.open("ep1.txt");
    
    //while (getline (MyReadFile, myText)){ le de linha a linha

    while (!MyReadFile.eof()){ // le de palavra a palavra (presumo que considera palavra quando ha espaco antes ou depois)


        MyReadFile >> myText;

        for (size_t j=0; j < myText.length();j++)
        {
            myText[j] = tolower(myText[j]);
        }

        ProcessedFile << myText << endl;

        cout << myText;
    }

    MyReadFile.close();

    ProcessedFile.close();

    return 0;
}