#define MARKER_STRING "{}"
#include "MyTrie.h"
#include <iostream>
#include <random>
#include <chrono> //to use default_random_generator
#include <sstream>
#include <utility> //to use std::pair
#include <fstream>
#include "utils.h"

TrieNode::TrieNode() {
	character = "";
	childWordCount = 0;
}

TrieNode::TrieNode(string character, string definition) {
	this->character = character;
	childWordCount = 0;
	if(definition!="")	this->definitions.emplace_back(definition);
}

Trie::Trie() {
	this->root = new TrieNode();
}
void Trie::freeTrie(TrieNode*& root) {
	if (!root) return;
	for (TrieNode*& child : root->childrens) {
		freeTrie(child);
	}
	delete root;
}

TrieNode* Trie::childNodeContainsChar(TrieNode* parentNode,string character) {
	if (!parentNode) return nullptr;
	for (TrieNode* child : parentNode->childrens) {
		if (child->character == character) return child;
	}
	return nullptr;
}

TrieNode* Trie::inputNodeFile(ifstream& in) {
	string chara;
	if (!(getline(in,chara))) {
		return nullptr;
	}
	if (chara == MARKER_STRING) {
		return nullptr;
	}
	TrieNode* newNode = new TrieNode();
	newNode->character = chara;
	string wordCount;
	getline(in, wordCount);
	newNode->childWordCount = stoi(wordCount);
	string nDef;
	getline(in, nDef);
	int n_def = stoi(nDef);
	newNode->definitions = vector<string>(n_def);
	for (int i = 0; i < n_def; i++) {
		getline(in, newNode->definitions[i]);
		replace(newNode->definitions[i], '|', '\n');
	}
	return newNode;
}

void Trie::outputNodeFile(TrieNode* node, ofstream& out) {
	out << node->character << '\n';
	out << node->childWordCount << '\n';
	out << node->definitions.size() << '\n';
	for (int i = 0; i < node->definitions.size(); i++) {
		string temp = node->definitions[i];
		replace(temp, '\n', '|');
		out << temp << '\n';
	}
}

void Trie::serialize(TrieNode* root, ofstream& out){
	// Base case
	if (root == nullptr) return;

	// Else, store current node and recur for its children
	outputNodeFile(root, out);
	for (int i = 0; i < root->childrens.size(); i++)
		serialize(root->childrens[i], out);

	// Store marker at the end of children
	out << MARKER_STRING << '\n';
}

int Trie::deSerialize(TrieNode*& root, ifstream& in){
	// Read next item from file. If there are no more items or next
	// item is marker, then return 1 to indicate same
	TrieNode* newNode = inputNodeFile(in);
	if (!newNode) {
		return 1;
	}
	// Else create node with this item and recur for children
	root = newNode;
	TrieNode* childNode;
	while (true) {
		childNode = nullptr;
		if (deSerialize(childNode, in)) {
			delete childNode;
			break;
		}
		root->childrens.emplace_back(childNode);
	}
	// Finally return 0 for successful finish
	return 0;
}

vector<vector<string>> Trie::getOperationsDone() {
	return operationsDone;
}

void Trie::setOperationsDone(vector<vector<string>> newVector) {
	operationsDone = newVector;
}

TrieNode* Trie::getNodeWord(string word) {
	TrieNode* curr = root;
	string ch = "";
	for (int i = 0; i < word.size(); i++) {
		ch = word[i];
		TrieNode* next = childNodeContainsChar(curr, ch);
		if (next == nullptr) {
			return nullptr;
		}
		curr = next;
	}
	return curr;
}

void Trie::getNChildUnderneath(TrieNode* curr, int n, vector<string>& childs,string currWord) {
	
	if (childs.size() == n) return;
	if (!curr) return;
	if (curr->definitions.size() > 0) childs.emplace_back(currWord);
	for (int i = 0; i < curr->childrens.size(); i++) {
		
		if (!curr->childrens[i]) continue;
		getNChildUnderneath(curr->childrens[i], n, childs, currWord + curr->childrens[i]->character);
	}
}
bool Trie::insertWord(string word, string definition) {
	//Function desciption:
	//Insert a word into trie
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
			curr->childrens.emplace_back(next);
			ans = false;
		}
		v.emplace_back(curr);
		curr = next;
	}
	if (curr->definitions.size() == 0) ans = false;
	curr->definitions.emplace_back(definition);
	v.emplace_back(curr);
	if (!ans) {
		for (TrieNode* node : v) node->childWordCount++;
		operationsDone.push_back({ "insert_word",word });
	}
	else {
		operationsDone.push_back({ "insert_def",word,definition,to_string(curr->definitions.size())});
	}
	return ans;
}

int Trie::deleteWord(TrieNode*&curr,string word,TrieNode* parent,string firstWord) {
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
			if (deleteWord(next, word.substr(1),curr,firstWord)==1 && curr->definitions.size() == 0 && curr->childrens.size() == 1) {
				if (!parent) { return 1; } //Do not delete the root
				
				for (int i = 0; i < parent->childrens.size();i++) {
					if (curr == parent->childrens[i]) {
						parent->childrens[i]->childrens.clear();
						delete parent->childrens[i];
						parent->childrens[i] = nullptr;
						parent->childrens.erase(parent->childrens.begin() + i);
						break;
					}
				}
				return 1;
			}
			else {
				curr->childWordCount--;
				return 0;
			}
		}
	}
	else if(word.size()==0 && curr->definitions.size()>0) {
		for (int i = curr->definitions.size()-1; i >=0; i--) {
			operationsDone.push_back({ "delete_def",firstWord,curr->definitions[i],to_string(i+1)});
		}
		if (curr->childrens.size() == 0) {
			for (int i = 0; i < parent->childrens.size(); i++) {
				if (curr == parent->childrens[i]) {
					parent->childrens[i]->childrens.clear();
					delete parent->childrens[i];
					parent->childrens[i] = nullptr;
					parent->childrens.erase(parent->childrens.begin() + i);
					break;
				}
			}
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
	//TODO:Implement sort() function in utils.cpp and utils.h because the project doesn't allow sort in std library 
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
				ans.emplace_back(make_pair(currWord, match));
				if (ans.size() == nWord) sortByFunc(ans);
				break;
			}
			else {
				if (match > ans[ans.size() - 1].second) {
					ans.pop_back();
					ans.emplace_back(make_pair(currWord, match));
					sortByFunc(ans);
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

void Trie::insertDefinition(int nthDefinition, string word, string definition) {
	// Function description:
	//Insert to the nth definition of the word input (a word may or maynot has several definitions).
	//If the nthDefinition is not in the range of the vector size or the word is not found,change nothing.
	//When using this, note that the word must be already in the Trie.
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
	
	curr->definitions.insert(curr->definitions.begin() + (nthDefinition - 1),definition);
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
		string prevDefinition = curr->definitions[nthDefinition - 1];
		curr->definitions[nthDefinition - 1] = definition;
		operationsDone.push_back({ "change_def",word,prevDefinition,definition,to_string(nthDefinition) });
	}
}

void Trie::deleteDefinition(int nthDefinition, string word) {
	// Function description:
	//Delete the nth definition of the word input (a word may or maynot has several definitions).
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
		operationsDone.push_back({ "delete_def",word,curr->definitions[nthDefinition-1],to_string(nthDefinition)});
		curr->definitions.erase(curr->definitions.begin() + nthDefinition-1);
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
		ans.emplace_back("None");
		ans.emplace_back("None");
		return ans;
	}
	default_random_engine generator(std::chrono::duration_cast<std::chrono::nanoseconds>(
		std::chrono::high_resolution_clock::now().time_since_epoch())
		.count());
	uniform_int_distribution<int> distribution(1, root->childWordCount);
	int random = distribution(generator);
	bool flag = false;
	TrieNode* next = nullptr;
	string word = "";
	while (true) {
		for (TrieNode* node : curr->childrens) {
			if (random == 1 && node->definitions.size() != 0) {
				flag = true;
				ans.emplace_back(word + node->character);
				uniform_int_distribution<int> defRandom(0, node->definitions.size()-1);
				ans.emplace_back(node->definitions[defRandom(generator)]);
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