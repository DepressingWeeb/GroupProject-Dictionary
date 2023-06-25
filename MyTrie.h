#pragma once
#include <string>
#include <vector>
using namespace std;

class TrieNode {
public:
	string character;//character contains in that node
	vector<string> definitions;
	vector<TrieNode*> childrens;
	int childWordCount;
	TrieNode();//Default Constructor
	TrieNode(string character, string definition);
};

class Trie {
private:
	TrieNode* root;
	TrieNode* childNodeContainsChar(TrieNode* parentNode,string character);
public:
	Trie();//default constructor
	~Trie();//Destructor
	TrieNode*& getRoot() { return root; }
	bool insertWord(string word,string definition);
	int deleteWord(TrieNode*& curr,string word);
	vector<string> getDefinitions(string word);
	void getWords(TrieNode* curr,string currWord, int nWord, vector<pair<string, int>>& ans, vector<string>& definition);
	void changeDefinition(int nthDefinition,string word,string definition);
	vector<string> getRandomWordAndDefinition();
};