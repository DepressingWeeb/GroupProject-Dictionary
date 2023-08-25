#include "MyDictionary.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <locale>
#include <codecvt>
#include <chrono>
#include <iomanip>
#include <ctime>
#include "utils.h"
MyDictionary::MyDictionary() {
	dictSize = 0;
}
MyDictionary::MyDictionary(const string path,int option) {
	dictSize = 0;
	ifstream fin;
	fin.open(path);
	if (option == 1) {
		vector<string> row;
		string line, word, temp;
		while (getline(fin, line)) {
			row.clear();
			stringstream s(line);
			while (getline(s, word, ',')) {
				row.emplace_back(word);
			}
			replace(row[3], '|', '\n');
			if (!trie.insertWord(row[0], row[3])) dictSize++;
		}
		trie.setOperationsDone(vector<vector<string>>());
	}
	else{
		int isfail=trie.deSerialize(trie.getRoot(), fin);
		if (isfail) {
			fin.close();
			return;
		}
		dictSize = trie.getRoot()->childWordCount;
		int nWord,nOperations;
		fin >> nWord;
		favoriteWords = vector<string>(nWord);
		fin.ignore(256, '\n');
		for (int i = 0; i < nWord; i++)
			getline(fin, favoriteWords[i]);
		fin >> nWord;
		searchHistory = vector<string>(nWord);
		fin.ignore(256, '\n');
		for (int i = 0; i < nWord; i++)
			getline(fin, searchHistory[i]);
		fin >> nOperations;
		vector<vector<string>> operations(nOperations);
		fin.ignore(256, '\n');
		for (vector<string>& v : operations) {
			fin >> nWord;
			v = vector<string>(nWord);
			fin.ignore(256, '\n');
			for (string& w : v)
				getline(fin, w);
		}
		trie.setOperationsDone(operations);
	}
	fin.close();
}

vector<string> MyDictionary::searchDefinition(string word) {
	vector<string>::iterator it = find(searchHistory.begin(), searchHistory.end(), word);
	if (it != searchHistory.end())
		searchHistory.erase(it);
	searchHistory.emplace_back(word);
	return trie.getDefinitions(word);
}

vector<pair<string,int>> MyDictionary::searchWords(string definition, int nWord) {
	//Function desciption:
	//Return nWord words that have the highest match with the input definition
	replace_if(definition, ",.;:'/",' ');
	vector<string> words;
	stringstream ss(definition);
	string word;
	while (ss >> word) 
		if(word.size()>2)	
			words.emplace_back(word);
	vector<pair<string, int>> res;
	string currWord = "";
	trie.getWords(trie.getRoot(), currWord, nWord, res, words);
	return res;
}

vector<string> MyDictionary::getNWordsUnderneath(string word,int n) {
	vector<string> ans;
	trie.getNChildUnderneath(trie.getNodeWord(word), n, ans,word);
	return ans;
}

void MyDictionary::insertDefinition(int nthDefinition, string word, string definition) {
	trie.insertDefinition(nthDefinition, word, definition);
}

void MyDictionary::changeDefinition(int nthDefinition,string word, string newDefinition) {
	trie.changeDefinition(nthDefinition,word, newDefinition);
}

void MyDictionary::deleteDefinition(int nthDefinition, string word) {
	trie.deleteDefinition(nthDefinition, word);
}

void MyDictionary::deleteWord(string word) {
	if (trie.deleteWord(trie.getRoot(), word,nullptr,word) != -1) dictSize--;
}

void MyDictionary::insertWord(string word,string definition) {
	if (!trie.insertWord(word, definition)) dictSize++;
}

vector<string> MyDictionary::getRandomWordAndDefinition() {
	return trie.getRandomWordAndDefinition();
}

void MyDictionary::addFavorite(string word) {
	favoriteWords.emplace_back(word);
}

void MyDictionary::removeFavorite(string word) {
	int idxFound = find(favoriteWords,word);
	if (idxFound != -1) favoriteWords.erase(favoriteWords.begin()+idxFound);
}

void MyDictionary::printFavorite() {
	for (int i = 1; i <= favoriteWords.size(); i++)
		cout << to_string(i) << "." << favoriteWords[i] << endl;
}
bool MyDictionary::checkInFavorite(string word) {
	for (string w : favoriteWords)
		if (w == word)
			return true;
	return false;
}

vector<string> MyDictionary::getFavorite() {
	return favoriteWords;
}

void MyDictionary::printHistory() {
	for(int i=searchHistory.size()-1;i>-1;i--)
		cout << to_string(i) << "." << searchHistory[i] << '\n';
}

vector<string> MyDictionary::getHistory() {
	return searchHistory;
}

void MyDictionary::toFile() {
	//Function description:
	//Save the current Dictionary to file (must ensure that each time the saved file has unique name )
	//Recommend: using time since epoch (time(0)) or date and time format to make a different name for each file
	//Create file name
	struct tm timeT;
	auto now = std::chrono::system_clock::now();
	auto localTime = std::chrono::system_clock::to_time_t(now);
	_localtime64_s(&timeT, &localTime);
	//Warning: _localtime64_s is able only in VS
	stringstream ss;
	ss << put_time(&timeT, "%Y-%m-%d %H.%M.%S") << ".txt";
	string fileName = ss.str();
	//Serialize Trie
	ofstream out("SavedDataStructure/"+fileName);
	trie.serialize(trie.getRoot(), out);
	//Serialize favoriteWord
	out << favoriteWords.size() << '\n';
	for (string w : favoriteWords)
		out << w << "\n";
	//serialize searchHistory
	out << searchHistory.size() << '\n';
	for (string w : searchHistory)
		out << w << "\n";
	//serialize operationsDone
	vector<vector<string>>operations = trie.getOperationsDone();
	out << operations.size() << '\n';
	for (vector<string>v : operations) {
		out << v.size() << '\n';
		for (string s : v)
			out << s << '\n';
	}
	out.close();
}

void MyDictionary::resetDictionary() {
	//Function description:
	//Reset dict to initial condition ( delete inserted words, insert deleted words, change back edited definitions etc.)
	operationsDone = trie.getOperationsDone();
	vector<vector<string>> v;
	trie.setOperationsDone(v);
	favoriteWords.clear();
	searchHistory.clear();
	while (operationsDone.size()) {
		vector<string> operation = operationsDone[operationsDone.size() - 1];
		operationsDone.pop_back();
		if (operation[0] == "insert_word") {
			deleteWord(operation[1]);
		}
		else if (operation[0] == "insert_def") {
			deleteDefinition(stoi(operation[3]), operation[1]);
		}
		else if (operation[0] == "change_def") {
			changeDefinition(stoi(operation[4]), operation[1], operation[2]);
		}
		else if (operation[0] == "delete_def") {
			TrieNode* node = trie.getNodeWord(operation[1]);
			if (!node) {
				insertWord(operation[1], operation[2]);
			}
			else {
				if (node->definitions.size() >= stoi(operation[3]) - 1) {
					insertDefinition(stoi(operation[3]), operation[1], operation[2]);
				}
				else {
					
					for (int i = operationsDone.size() - 1; i > -1; i--) {
						if (operationsDone[i][0] == "delete_def" && operationsDone[i][1] == operation[1] && node->definitions.size() >= stoi(operationsDone[i][3]) - 1) {
							insertDefinition(stoi(operationsDone[i][3]), operation[1], operationsDone[i][2]);
							operationsDone.erase(operationsDone.begin() + i);
							break;
						}
					}
				}
			}
		}
	}
	trie.setOperationsDone(vector<vector<string>>());
	return;
}

void MyDictionary::freeDictionary() {
	trie.setOperationsDone(vector<vector<string>>());
	trie.freeTrie(trie.getRoot());
	searchHistory.clear();
	favoriteWords.clear();
	operationsDone.clear();
	dictSize = 0;
}