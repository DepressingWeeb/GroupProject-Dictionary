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
bool cmp1(pair<string, int>p1, pair<string, int> p2) {
	return p1.second > p2.second;
}
void sortByFunc(vector<pair<string,int>>& v);//Sort vector by the cmp function above
void conversionVietnameseVNI(string& s);
/*
Convert last word of string to Vietnamese characters using VNI
Example:
	1.fix bug mệt qua1->fix bug mệt quá
	2.ảo thật day9->ảo thật đay
	3.ảo thật d9ay->ảo thật đay
	4.đang đi trên đuong7->đang đi trên đương
	5.đang đi trên đương2->đang đi trên đường
Simple conversion (only last char to Vietnamese) is in DictionaryGUI.cpp::void simpleConversionToVietnameseVNI(string& word)
*/

void conversionVietnameseTelex(string& s);
/*
Same as above but in Telex
*/


//TODO:in MyTrie.cpp::Trie::~Trie()
//TODO:in MyDictionary.cpp::MyDictionary::MyDictionary()
//TODO:in MyDictionary.cpp::void MyDictionary::toFile()