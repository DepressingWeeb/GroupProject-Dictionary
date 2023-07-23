#pragma once
#include "MyTrie.h"

class MyDictionary {
private:
	int dictSize;//number of words
	Trie trie;//the dictionary trie to hold the definitions of word
	vector<string> favoriteWords;
	vector<string> searchHistory;
	vector<vector<string>> operationsDone;
public:
	MyDictionary();
	MyDictionary(const string path,int option);
	int getSize() { return dictSize; }
	vector<string> searchDefinition(string word);
	vector<pair<string,int>> searchWords(string definition,int nWord);
	vector<string> getNWordsUnderneath(string word,int n);
	void insertDefinition(int nthDefinition, string word, string definition);
	void changeDefinition(int nthDefinition,string word, string newDefinition);
	void deleteDefinition(int nthDefinition, string word);
	void deleteWord(string word);
	void insertWord(string word,string definition);
	vector<string> getRandomWordAndDefinition();
	void addFavorite(string word);
	void removeFavorite(string word);
	void printFavorite();
	bool checkInFavorite(string word);
	vector<string> getFavorite();
	void printHistory();
	vector<string> getHistory();
	void toFile();
	void resetDictionary();
};
