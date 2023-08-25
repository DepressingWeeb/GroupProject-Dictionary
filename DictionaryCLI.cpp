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

