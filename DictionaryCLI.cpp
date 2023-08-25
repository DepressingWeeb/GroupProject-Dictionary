#pragma execution_character_set("utf-8")
#include "DictionaryCLI.h"
#include "MyDictionary.h"
#define MAX_INPUT_LENGTH 500
using namespace std;
void inputString(string& input) {
	wchar_t wstr[MAX_INPUT_LENGTH];
	char mb_str[MAX_INPUT_LENGTH * 3 + 1];

	unsigned long read;
	void* con = GetStdHandle(STD_INPUT_HANDLE);

	ReadConsole(con, wstr, MAX_INPUT_LENGTH, &read, NULL);

	int size = WideCharToMultiByte(CP_UTF8, 0, wstr, read, mb_str, sizeof(mb_str), NULL, NULL);
	mb_str[size - 2] = 0;
	input = mb_str;
}

void runCLI() {
	MyDictionary myDictionary;
	const string text = "---------------------------------------------------------DICTIONARY OPERATION----------------------------------------------\n1.\tInit dictionary from datasets.\n2.\tSearch for a keyword. \n3.\tSearch for a definition.\n4.\tAdd a new word and its definition.\n5.\tEdit the definition of an existing word.\n6.\tRemove a word from the dictionary.\n7.\tReset the dictionary to its original state.\n8.\tView a random word and its definition.\n9.\tView the history of searched words.\n-------------------------------------------------------------------QUIZ----------------------------------------------------------------\n10.\tMake random a word with four definitions, and guess its meaning.\n11.\tProvide a random definition with four keywords, and choose the correct word.\n-----------------------------------------------------------------FAVORITE-------------------------------------------------------------\n12.\tAdd a word to favorite list.\n13.\tRemove a word from favorite list.\n14.\tDisplay favorite list.\n-----------------------------------------------------------------I/O FILE-------------------------------------------------------------\n15.\tInit data structure from file\n16.\tSave data structure to file\n--------------------------------------------------------------------QUIT----------------------------------------------------------------\n17.\tQuit program\n\n\n";
	int input;
	string input2;
	string input3;
	int input4;
	vector<string> rep;
	vector<string> rep2;
	vector<string> rep3;
	vector<string> rep4;
	vector<pair<string, int>> rep5;
	vector<vector<string>> v;
	string word;
	string definition;
	int randomInt;
	while (true) {
		cout << text;
		cin >> input;
		cin.ignore(256, '\n');
		switch (input)
		{
		case 1:
			cout << "Type the path to the dataset:";
			cin >> input2;
			myDictionary.freeDictionary();
			myDictionary = MyDictionary(input2, 1);
			break;
		case 2:
			cout << "Type the word to search for:";
			inputString(input2);
			rep = myDictionary.searchDefinition(input2);

			cout << "The definition of the word is:" << endl;
			for (int i = 1; i <= rep.size(); i++) {
				cout << "Definition " << i << " : " << rep[i - 1] << endl;
			}
			break;
		case 3:
			cout << "Type the definition to search for:";
			inputString(input2);
			cout << input2 << " " << input2.size() << endl;
			rep5 = myDictionary.searchWords(input2, 10);
			cout << "The 3 most matched words are:" << endl;
			for (int i = 1; i <= rep5.size(); i++) {
				cout << "Word " << i << " : " << rep5[i - 1].first << "    " << rep5[i - 1].second << " word matched" << endl;
			}
			break;
		case 4:
			cout << "Type the word to insert:";
			inputString(input2);
			cout << "Type the definition to insert:\n";
			inputString(input3);
			myDictionary.insertWord(input2, input3);
			break;
		case 5:
			cout << "Type the word to change definition:";
			inputString(input2);
			cout << "Type the ith definition to change:";
			cin >> input4;
			cout << "Type the new definition to change to:";
			cin.ignore(256, '\n');
			inputString(input3);
			myDictionary.changeDefinition(input4, input2, input3);
			break;
		case 6:
			cout << "Type the word to remove from dictionary:";
			inputString(input2);
			myDictionary.deleteWord(input2);
			break;
		case 7:
			myDictionary.resetDictionary();
			cout << "Dictionary has reset";
			break;
		case 8:
			rep = myDictionary.getRandomWordAndDefinition();
			rep2 = myDictionary.searchDefinition(rep[0]);
			cout << "The random word is: " << rep[0] << endl;
			cout << "The definition of the word is:" << endl;
			for (int i = 1; i <= rep2.size(); i++) {
				cout << "Definition " << i << " : " << rep2[i - 1] << endl;
			}
			break;
		case 9:
			myDictionary.printHistory();
			break;
		case 10:
			rep = myDictionary.getRandomWordAndDefinition();
			rep2 = myDictionary.getRandomWordAndDefinition();
			rep3 = myDictionary.getRandomWordAndDefinition();
			rep4 = myDictionary.getRandomWordAndDefinition();
			v = { rep,rep2,rep3,rep4 };
			shuffle(v.begin(), v.end(), default_random_engine(time(0)));
			srand(time(0));
			randomInt = rand() % 4;
			word = v[randomInt][0];
			cout << "The random word is: " << word << endl;
			cout << "Four random definitions,including the definition of the word above is:" << endl;
			for (int i = 1; i <= v.size(); i++) {
				cout << "Definition " << i << " : " << v[i - 1][1] << endl;
			}
			cout << "Please type number from 1-4 to choose the correct definition : ";
			cin >> input;
			if (input == randomInt + 1) cout << "Correct!" << endl;
			else cout << "Incorrect. The answer is " << randomInt + 1 << " : " << v[randomInt][1] << endl;
			break;
		case 11:
			rep = myDictionary.getRandomWordAndDefinition();
			rep2 = myDictionary.getRandomWordAndDefinition();
			rep3 = myDictionary.getRandomWordAndDefinition();
			rep4 = myDictionary.getRandomWordAndDefinition();
			v = { rep,rep2,rep3,rep4 };
			shuffle(v.begin(), v.end(), default_random_engine(time(0)));
			srand(time(0));
			randomInt = rand() % 4;
			definition = v[randomInt][1];
			cout << "The random definition is: " << definition << endl;
			cout << "Four random words,including the word that has the definition above is:" << endl;
			for (int i = 1; i <= v.size(); i++) {
				cout << "Word " << i << " : " << v[i - 1][0] << endl;
			}
			cout << "Please type number from 1-4 to choose the correct word : ";
			cin >> input;
			if (input == randomInt + 1) cout << "Correct!" << endl;
			else cout << "Incorrect. The answer is " << randomInt + 1 << " : " << v[randomInt][0] << endl;
			break;
		case 12:
			cout << "Type the word to add to the favorite list:";
			inputString(input2);
			myDictionary.addFavorite(input2);
			break;
		case 13:
			cout << "Type the word to remove from the favorite list:";
			inputString(input2);
			myDictionary.removeFavorite(input2);
			break;
		case 14:
			myDictionary.printFavorite();
			break;
		case 15:
			cout << "Type the path to the serialized file: ";
			inputString(input2);
			myDictionary.freeDictionary();
			myDictionary = MyDictionary(input2, 15);
			break;
		case 16:
			myDictionary.toFile();
			break;
		case 17:
			return;
		default:
			break;
		}
		cout << "Done. Press any key to continue..." << endl;
		_getch();
		system("cls");
	}
}