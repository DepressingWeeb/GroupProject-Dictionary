#pragma once
#include <string>
#include <vector>
#include <utility>
using namespace std;
//TODO
//replace(row[3].begin(), row[3].end(), '|', '\n');
//find(wordSearched.begin(), wordSearched.end(), wordToDisplayDefinition) != wordSearched.end()
//replace_if(definition.begin(), definition.end(), [](char ch) {return (ch == ',' || ch == '.' || ch == ';' || ch == ':' || ch == '\''); }, ' ');
//sort(ans.begin(), ans.end(), cmp);
void replace(string& word, char from, char to);
int find(vector<string> words, string targetWord);//return index of first target found,if not found return -1
void replace_if(string& s, string from, char to); //replace the char if char is in string from
void sortByFunc(vector<pair<string,int>>& v);//Sort vector by the cmp function above


//TODO:in MyTrie.cpp::Trie::~Trie()
//TODO:in MyDictionary.cpp::MyDictionary::MyDictionary()
//TODO:in MyDictionary.cpp::void MyDictionary::toFile()