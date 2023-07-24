#include "DictionaryCLI.h"
#include "DictionaryGUI.h" //Must download SDL and has SDL linked

int main(int, char**) {
	SetConsoleOutputCP(CP_UTF8);
	setvbuf(stdout, nullptr, _IONBF, 0);
	//runCLI();
	runGUI(); //Must download SDL and has SDL linked
	return 0;
}