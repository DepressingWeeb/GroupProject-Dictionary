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
	vector<vector<string>>operationsDone;// The operations done that EDIT the dictionary (for functions like reset )
	TrieNode* childNodeContainsChar(TrieNode* parentNode,string character);
        TrieNode* inputNodeFile(ifstream& in);
	void outputNodeFile(TrieNode* node, ofstream& out);
public:
	Trie();//default constructor
	//~Trie();//Destructor
	void freeTrie(TrieNode* root);
	TrieNode*& getRoot() { return root; }
	vector<vector<string>> getOperationsDone();
	TrieNode* getNodeWord(string word);
	void getNChildUnderneath(TrieNode* curr, int n, vector<string>& childs, string currWord);
	bool insertWord(string word,string definition);
	int deleteWord(TrieNode*& curr,string word,TrieNode* parent);
	vector<string> getDefinitions(string word);
	void getWords(TrieNode* curr,string currWord, int nWord, vector<pair<string, int>>& ans, vector<string>& definition);
	void insertDefinition(int nthDefinition, string word, string definition);
	void changeDefinition(int nthDefinition,string word,string definition);
	void deleteDefinition(int nthDefinition, string word);
	vector<string> getRandomWordAndDefinition();
        void serialize(TrieNode* root,ofstream& fout);
        int deserialize(TrieNode* &root,ifstream& fin);
};
