#include "MyDictionary.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
MyDictionary::MyDictionary() {
	dictSize = 0;
}
MyDictionary::MyDictionary(const string path,int option) {
	dictSize = 0;
	if (option == 1) {
		ifstream fin;
		fin.open(path);
		vector<string> row;
		string line, word, temp;
		while (getline(fin, line)) {
			row.clear();
			stringstream s(line);
			while (getline(s, word, ',')) {
				row.push_back(word);
			}
			if (!trie.insertWord(row[0], row[3])) dictSize++;
		}
	}
	else {
		//TODO:Complete the option 15 : input from saved data structures
		//Must have every data members (trie,dictsize,favoriteWord,history) be set

	}
}

vector<string> MyDictionary::searchDefinition(string word) {
	searchHistory.push_back(word);
	return trie.getDefinitions(word);
}

vector<pair<string,int>> MyDictionary::searchWords(string definition, int nWord) {
	//Function desciption:
	//Return nWord words that have the highest match with the input definition
	replace_if(definition.begin(), definition.end(), [](char ch) {return (ch == ',' || ch == '.' || ch == ';' || ch == ':' || ch == '\''); }, ' ');
	vector<string> words;
	stringstream ss(definition);
	string word;
	while (ss >> word) 
		if(word.size()>2)	
			words.push_back(word);
	sort(words.begin(), words.end());
	vector<pair<string,int>> ans;
	string currWord = "";
	trie.getWords(trie.getRoot(), currWord, nWord, ans, words);
	return ans;
}

void MyDictionary::changeDefinition(int nthDefinition,string word, string newDefinition) {
	trie.changeDefinition(nthDefinition,word, newDefinition);
}

void MyDictionary::deleteWord(string word) {
	trie.deleteWord(trie.getRoot(),word);
}

bool MyDictionary::insertWord(string word,string definition) {
	return trie.insertWord(word,definition);
}

vector<string> MyDictionary::getRandomWordAndDefinition() {
	return trie.getRandomWordAndDefinition();
}

void MyDictionary::addFavorite(string word) {
	favoriteWords.push_back(word);
}

void MyDictionary::removeFavorite(string word) {
	vector<string>::iterator it = find(favoriteWords.begin(), favoriteWords.end(),word);
	if (it != favoriteWords.end()) favoriteWords.erase(it);
}

void MyDictionary::printFavorite() {
	for (int i = 1; i <= favoriteWords.size(); i++)
		cout << to_string(i) << "." << favoriteWords[i] << endl;
}

void MyDictionary::printHistory() {
	for(int i=searchHistory.size()-1;i>-1;i--)
		cout << to_string(i) << "." << searchHistory[i] << endl;
}

void MyDictionary::toFile() {
	//TODO
	//Function description:
	//Save the current Dictionary to file (must ensure that each time the saved file has unique name )
	//Recommend: using time since epoch (time(0)) to make a different name for each file
}