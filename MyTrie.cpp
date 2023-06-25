#include "MyTrie.h"
#include <iostream>
#include <random>
#include <chrono>
#include <sstream>
#include <utility>

bool cmp(pair<string, int>p1, pair<string, int> p2) {
	return p1.second > p2.second;
}

TrieNode::TrieNode() {
	character = "";
	childWordCount = 0;
}

TrieNode::TrieNode(string character, string definition) {
	this->character = character;
	childWordCount = 0;
	if(definition!="")	this->definitions.push_back(definition);
}

Trie::Trie() {
	this->root = new TrieNode();
}

Trie::~Trie() {
	//TODO:Complete the function
	//Function desciption:
	//Free the space allocated for the trie
}
TrieNode* Trie::childNodeContainsChar(TrieNode* parentNode,string character) {
	if (!parentNode) return nullptr;
	for (TrieNode* child : parentNode->childrens) {
		if (child->character == character) return child;
	}
	return nullptr;
}
bool Trie::insertWord(string word, string definition) {
	//Function desciption:
	// insert a word into trie
	//Return true if word already presented in trie,else false
	bool ans = true;
	TrieNode* curr = root;
	string ch = "";
	vector<TrieNode*> v;
	for (int i = 0; i < word.size(); i++) {
		ch = word[i];
		TrieNode* next = childNodeContainsChar(curr, ch);
		if (next == nullptr) {
			next = new TrieNode(ch, "");
			curr->childrens.push_back(next);
			ans = false;
		}
		v.push_back(curr);
		curr = next;
	}
	curr->definitions.push_back(definition);
	v.push_back(curr);
	if (!ans) {
		for (TrieNode* node : v) node->childWordCount++;
	}
	return ans;
}

int Trie::deleteWord(TrieNode*&curr,string word) {
	//Function desciption:
	//Delete the word and all of its definition in the trie 
	//1:true
	//0:false
	//-1: no such words
	if (curr == nullptr) {
		return -1;
	}
	if (word.size()) {
		TrieNode* next = childNodeContainsChar(curr, word.substr(0, 1));
		if (next == nullptr) {
			return -1;
		}
		else {
			if (deleteWord(next, word.substr(1))==1 && curr->definitions.size() == 0 && curr->childrens.size() == 1) {
				curr->childrens.clear();
				delete curr;
				curr = nullptr;
				return 1;
			}
			else {
				curr->childWordCount--;
				return 0;
			}
		}
	}
	else if(word.size()==0 && curr->definitions.size()>0) {
		if (curr->childrens.size() == 0) {
			delete curr;
			curr = nullptr;
			return 1;
		}
		else {
			curr->definitions.clear();
			return 0;
		}
	}
	return -1;
}

void Trie::getWords(TrieNode* curr,string currWord, int nWord,vector<pair<string,int>>& ans,vector<string>& definition) {
	//TODO:Implement sort() function and Pair structure in utils.cpp and utils.h because the project doesn't allow pair and sort in std library 
	// Function description:
	//Return nWord words whose definition have the highest matching percentage to the input definition
	currWord += curr->character;
	int match = 0;
	if (curr->definitions.size() != 0) {
		for (string def : curr->definitions) {
			match = 0;
			for (string w : definition) {
				if (def.find(w)!=string::npos) match++;
			}
			if (ans.size() < nWord) {
				ans.push_back(make_pair(currWord, match));
				if (ans.size() == nWord) sort(ans.begin(), ans.end(), cmp);
			}
			else {
				if (match > ans[ans.size() - 1].second) {
					ans.pop_back();
					ans.push_back(make_pair(currWord, match));
					sort(ans.begin(), ans.end(), cmp);
					break;
				}
			}
			
		}
	}
	for (TrieNode* node : curr->childrens) {
		getWords(node,currWord,nWord,ans,definition);
	}
	


}

vector<string> Trie::getDefinitions(string word) {
	// Function description:
	//Return the vector<string> which is all of the definitions of the word
	//cout << root->definitions[0];
	TrieNode* curr = root;
	vector<string> error = {"No word found"};
	string ch = "";
	for (int i = 0; i < word.size(); i++) {
		ch = word[i];
		TrieNode* next = childNodeContainsChar(curr, ch);
		if (next == nullptr) {
			return error;
		}
		curr = next;
	}
	return curr->definitions;
}

void Trie::changeDefinition(int nthDefinition,string word,string definition) {
	// Function description:
	//Change the nth definition of the word input (a word may or maynot has several definitions).
	//If the nthDefinition is not in the range of the vector size or the word is not found,change nothing.
	TrieNode* curr = root;
	string ch = "";
	for (int i = 0; i < word.size(); i++) {
		ch = word[i];
		TrieNode* next = childNodeContainsChar(curr, ch);
		if (next == nullptr) {
			return;
		}
		curr = next;
	}
	if (nthDefinition >= 1 && nthDefinition <= curr->definitions.size()) {
		curr->definitions[nthDefinition - 1] = definition;
	}
}

vector<string> Trie::getRandomWordAndDefinition() {
	// Function description:
	// Return a random word and one of its definition
	//Return vector with the following format:
	//v[0]: the random word
	//v[1]: a random definition of that word
	vector<string> ans;
	TrieNode* curr = root;
	if (!curr) {
		ans.push_back("None");
		ans.push_back("None");
		return ans;
	}
	default_random_engine generator(std::chrono::duration_cast<std::chrono::nanoseconds>(
		std::chrono::high_resolution_clock::now().time_since_epoch())
		.count());
	uniform_int_distribution<int> distribution(1, root->childWordCount);
	int random = distribution(generator);
	//cout << random << endl;
	bool flag = false;
	TrieNode* next = nullptr;
	string word = "";
	while (true) {
		for (TrieNode* node : curr->childrens) {
			if (random == 1 && node->definitions.size() != 0) {
				flag = true;
				ans.push_back(word + node->character);
				uniform_int_distribution<int> defRandom(0, node->definitions.size()-1);
				ans.push_back(node->definitions[defRandom(generator)]);
				break;
			}
			else if (random <= (node->childWordCount)) {
				word += node->character;
				next = node;
				if (node->definitions.size() != 0) random--;
				break;
			}
			else {
				random -= node->childWordCount;
			}
		}
		if (flag) break;
		curr = next;
	}
	return ans;
}