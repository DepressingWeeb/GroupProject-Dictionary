#pragma once
#include "MyTrie.h"

class MyDictionary {
private:
	int dictSize;//number of words
	Trie trie;//the dictionary trie to hold the definitions of word
	vector<string> favoriteWords;
	vector<string> searchHistory;
public:
	MyDictionary();
	MyDictionary(const string path,int option);
	vector<string> searchDefinition(string word);
	vector<pair<string,int>> searchWords(string definition,int nWord);
	void changeDefinition(int nthDefinition,string word, string newDefinition);
	void deleteWord(string word);
	bool insertWord(string word,string definition);
	vector<string> getRandomWordAndDefinition();
	void addFavorite(string word);
	void removeFavorite(string word);
	void printFavorite();
	void printHistory();
	void toFile();
};
