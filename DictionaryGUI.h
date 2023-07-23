#pragma once
#include "MyDictionary.h"
#include <atomic>
#include "imgui/imgui.h"
#include "imgui/imgui_impl_sdl2.h"
#include "imgui/imgui_impl_sdlrenderer.h"
#include "imgui/imgui_stdlib.h"
void initDictionary(MyDictionary& myDictionary, string PATH, atomic<bool>& done);
void searchDefinition(MyDictionary& myDictionary, vector<string>& wordSearched, string definitionSearch, atomic<bool>& doneSearch);
void simpleConversionToVietnameseVNI(string& word);
int TextEditCallBack(ImGuiInputTextCallbackData* data);
void inputStringVietnameseVNI(const char* label, const char* hint, const char* popupLabel, string& word);
void inputStringMultilineVietnameseVNI(const char* label, ImVec2 size, const char* popupLabel, string& word);
int runGUI();


