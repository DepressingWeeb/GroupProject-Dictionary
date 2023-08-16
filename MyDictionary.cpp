#include "MyDictionary.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <locale>
#include <codecvt>
#include "utils.h"
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
			replace(row[3], '|', '\n');
			if (!trie.insertWord(row[0], row[3])) dictSize++;
		}
		trie.getOperationsDone();
	}
	else {
		//TODO:Complete the option 15 : input from saved data structures
		//Must have every data members (trie,dictsize,favoriteWord,history,operationsDone) be set
		
		int check = Trie.deserialize(Trie.getRoot(),fin);
		if(check)
		{
			fin.close();
			return;
		}
		else
		{
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
		        for (vector<string>& v : operations)
			{
			     fin >> nWord;
			      v = vector<string>(nWord);
			      fin.ignore(256, '\n');
			       for (string& w : v)
				 getline(fin, w);
		         }
		        trie.setOperationsDone(operations);
		}
	
	}
	fin.close();
}

vector<string> MyDictionary::searchDefinition(string word) {
	vector<string>::iterator it = find(searchHistory.begin(), searchHistory.end(), word);
	if (it != searchHistory.end())
		searchHistory.erase(it);
	searchHistory.push_back(word);
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
			words.push_back(word);
	vector<pair<string,int>> ans;
	string currWord = "";
	trie.getWords(trie.getRoot(), currWord, nWord, ans, words);
	return ans;
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
	if (trie.deleteWord(trie.getRoot(), word,nullptr) != -1) dictSize--;
}

void MyDictionary::insertWord(string word,string definition) {
	if (!trie.insertWord(word, definition)) dictSize++;
}

vector<string> MyDictionary::getRandomWordAndDefinition() {
	return trie.getRandomWordAndDefinition();
}

void MyDictionary::addFavorite(string word) {
	favoriteWords.push_back(word);
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
		cout << to_string(i) << "." << searchHistory[i] << endl;
}

vector<string> MyDictionary::getHistory() {
	return searchHistory;
}

void MyDictionary::toFile() {
	//TODO
	//Function description:
	//Save the current Dictionary to file (must ensure that each time the saved file has unique name )
	//Recommend: using time since epoch (time(0)) or date and time format to make a different name for each file
	struct tm timeT;
	auto now = std::chrono::system_clock::now();
	auto localTime = std::chrono::system_clock::to_time_t(now);
	_localtime64_s(&timeT, &localTime);
	stringstream ss;
	ss << put_time(&timeT, "%Y-%m-%d %H.%M.%S") << ".txt";
	string fileName = ss.str();
	ofstream fout("SavedDataStructure/"+fileName);
	trie.serialize(trie.getRoot(), fout);
	fout << favoriteWords.size() << '\n';
	for (string w : favoriteWords)
		fout << w << "\n";
	fout << searchHistory.size() << '\n';
	for (string w :searchHistory)
		fout << w << "\n";
	vector<vector<string>>operations = trie.getOperationsDone();
	fout << operations.size() << '\n';
	for (vector<string>w : operations)
		{
		   fout << w.size() << '\n';
		    for (string w : v)
			fout << w << '\n';
	        }
	fout.close();
}

void MyDictionary::resetDictionary() {
	//Function description:
	//Reset dict to initial condition ( delete inserted words, insert deleted words, change back edited definitions etc.)
	operationsDone = trie.getOperationsDone();
	favoriteWords.clear();
	searchHistory.clear();
	while (operationsDone.size()) {
		vector<string> operation = operationsDone[operationsDone.size() - 1];
		operationsDone.pop_back();
		cout << operation[0] << endl;
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
			if (!node)
				insertWord(operation[1], operation[2]);
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
	trie.getOperationsDone();
	return;
}

void MyDictionary::freeDictionary() {
	trie.getOperationsDone();
	trie.freeTrie(trie.getRoot());
	cout << "Executed" << endl;
}
