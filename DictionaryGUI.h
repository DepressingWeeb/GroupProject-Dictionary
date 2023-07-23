#pragma once
#include "MyDictionary.h"
#include <atomic>
#include "imgui/imgui.h"
#include "imgui/imgui_impl_sdl2.h"
#include "imgui/imgui_impl_sdlrenderer.h"
#include "imgui/imgui_stdlib.h"
void initDictionary(MyDictionary& myDictionary, string PATH, atomic<bool>& done);
void searchDefinition(MyDictionary& myDictionary, vector<string>& wordSearched, string definitionSearch, atomic<bool>& doneSearch);
int runGUI();


