﻿#pragma execution_character_set("utf-8")
#include "DictionaryGUI.h"
#include <stdio.h>
#include <SDL.h>
#include <string>
#include <vector>
#include "MyDictionary.h"
#include "IconsFontAwesome6.h"
#include <atomic>
#include <thread>
#include <fstream>
#include <iostream>
using namespace std;
#if !SDL_VERSION_ATLEAST(2,0,17)
#error This backend requires SDL 2.0.17+ because of SDL_RenderGeometry() function
#endif

void initDictionary(MyDictionary& myDictionary,string PATH, atomic<bool>& done) {
    myDictionary = MyDictionary(PATH, 1);
    done = true;
}

void searchDefinition(MyDictionary& myDictionary, vector<string>&wordSearched,string definitionSearch,atomic<bool>& doneSearch) {
    vector<pair<string, int>> ans = myDictionary.searchWords(definitionSearch, 10);
    wordSearched.clear();
    for (auto p : ans) if(p.first!="")  wordSearched.push_back(p.first);
    doneSearch = true;
}

void simpleConversionToVietnameseVNI(string& word) {
    vector<string>utfChar = { u8"á", u8"à", u8"ả", u8"ã", u8"ạ",u8"â",u8"ấ", u8"ầ", u8"ẩ", u8"ẫ", u8"ậ",u8"ă",u8"ắ", u8"ằ", u8"ẳ", u8"ẵ", u8"ặ", u8"í", u8"ì", u8"ỉ", u8"ĩ", u8"ị",u8"ú", u8"ù", u8"ủ", u8"ũ", u8"ụ",u8"ư",u8"ứ", u8"ừ", u8"ử", u8"ữ", u8"ự",u8"é", u8"è", u8"ẻ", u8"ẽ", u8"ẹ",u8"ê",u8"ế", u8"ề", u8"ể", u8"ễ", u8"ệ",u8"ó", u8"ò", u8"ỏ", u8"õ", u8"ọ",u8"ô",u8"ố", u8"ồ", u8"ổ", u8"ỗ", u8"ộ",u8"ơ",u8"ớ", u8"ờ", u8"ở", u8"ỡ", u8"ợ",u8"ý",u8"ỳ", u8"ỷ", u8"ỹ", u8"ỵ", u8"đ" };
    vector<string> base = { "a","a","a","a","a","a",u8"â",u8"â", u8"â",u8"â", u8"â","a",u8"ă",u8"ă", u8"ă", u8"ă", u8"ă","i","i", "i", "i", "i","u","u", "u", "u", "u", "u",u8"ư",u8"ư", u8"ư", u8"ư",u8"ư","e","e", "e", "e", "e", "e", u8"ê",u8"ê", u8"ê", u8"ê", u8"ê", "o","o", "o", "o", "o", "o", u8"ô",u8"ô", u8"ô", u8"ô", u8"ô","o",u8"ơ",u8"ơ", u8"ơ", u8"ơ", u8"ơ","y","y", "y", "y","y","d" };
    vector<string> num = { "1","2","3","4","5","6","1","2","3","4","5","8","1","2","3","4","5", "1","2","3","4","5", "1","2","3","4","5","7","1","2","3","4","5", "1","2","3","4","5", "6","1","2","3","4","5", "1","2","3","4","5", "6","1","2","3","4","5", "7","1","2","3","4","5","1","2","3","4","5","9" };
    char lastChar = word[word.size() - 1];
    if (lastChar >= 49 && lastChar <= 57) {
        string lastLetter(1, word[word.size() - 2]);
        string last2Letter = word.size() > 2 ? word.substr(word.size() - 3, 2) : "";
        for (int i = 0; i < utfChar.size(); i++) {
            if (lastLetter == base[i] && num[i][0] == lastChar) {

                string newChar = utfChar[i];
                word.pop_back();
                word.pop_back();
                word += newChar;
                break;
            }
            else if (last2Letter == base[i] && num[i][0] == lastChar) {
                string newChar = utfChar[i];
                word.pop_back();
                word.pop_back();
                word.pop_back();
                word += newChar;
                break;
            }
        }
    }
}

int TextEditCallBack(ImGuiInputTextCallbackData* data) {
    
    string word = data->Buf;
    if (word.size() > 1) {
        simpleConversionToVietnameseVNI(word);
        memcpy(data->Buf, word.c_str(), word.size() + 1);
        data->BufTextLen = word.size();
        data->BufDirty = true;
        data->CursorPos = word.size();
        
    }
    return 0;
}
void inputStringVietnameseVNI(const char* label, const char* hint,const char* popupLabel,string& word) {
    ImGui::InputTextWithHint(label, hint, &word, ImGuiInputTextFlags_CallbackAlways, TextEditCallBack);
    
}
void inputStringMultilineVietnameseVNI(const char* label, ImVec2 size, const char* popupLabel, string& word) {
    ImGui::InputTextMultiline(label, &word,size, ImGuiInputTextFlags_CallbackAlways, TextEditCallBack);
}


// Main code
int runGUI()
{
    // Setup SDL
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_GAMECONTROLLER) != 0)
    {
        printf("Error: %s\n", SDL_GetError());
        return -1;
    }

    SDL_SetHint(SDL_HINT_IME_SHOW_UI, "1");

    // Create window with SDL_Renderer graphics context
    SDL_WindowFlags window_flags = (SDL_WindowFlags)(SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);
    SDL_Window* window = SDL_CreateWindow("Dictionary - Group 8", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1280, 720, window_flags);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_ACCELERATED);
    if (renderer == nullptr)
    {
        SDL_Log("Error creating SDL_Renderer!");
        return 0;
    }

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    // Setup Dear ImGui style
    //ImGui::StyleColorsDark();
    ImGui::StyleColorsLight();

    // Setup Platform/Renderer backends
    ImGui_ImplSDL2_InitForSDLRenderer(window, renderer);
    ImGui_ImplSDLRenderer_Init(renderer);

    bool show_demo_window = true;
    bool show_another_window = false;
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
    ImVector<ImWchar> ranges;
    ImVector<ImWchar> ranges_emoticon;
    ImVector<ImWchar> ranges_icon;
    ImFontGlyphRangesBuilder builder,emoticonBuilder,iconBuilder;
    builder.AddRanges(io.Fonts->GetGlyphRangesDefault()); // Add one of the default ranges
    builder.AddRanges(io.Fonts->GetGlyphRangesVietnamese());
    builder.BuildRanges(&ranges);
    //icon font build
    iconBuilder.AddText(ICON_FA_MAGNIFYING_GLASS);
    iconBuilder.AddText(ICON_FA_CIRCLE_XMARK);
    iconBuilder.AddText(ICON_FA_HEART);
    iconBuilder.AddText(ICON_FA_PEN_TO_SQUARE);
    iconBuilder.AddText(ICON_FA_PLUS);
    iconBuilder.AddText(ICON_FA_SQUARE_PLUS);
    iconBuilder.AddText(ICON_FA_LIST);
    iconBuilder.AddText(ICON_FA_CLOCK_ROTATE_LEFT);
    iconBuilder.AddText(ICON_FA_BOOK);
    iconBuilder.AddText(ICON_FA_FLOPPY_DISK);
    iconBuilder.AddText(ICON_FA_LEFT_LONG);
    iconBuilder.BuildRanges(&ranges_icon);
    //end icon font build
    ImFontConfig config,mergeConfig;
    config.MergeMode = true;
    config.GlyphMinAdvanceX = 13.0f; // Use if you want to make the icon monospaced
    mergeConfig.MergeMode = true;
    ifstream fo;
    fo.open("Datasets/emoticon_edited.txt");
    string tmp;
    while (getline(fo,tmp)) {
        emoticonBuilder.AddText(tmp.c_str());
    }
    fo.close();
    emoticonBuilder.BuildRanges(&ranges_emoticon);
    ImFont* font1 = io.Fonts->AddFontFromFileTTF("CODE2000.TTF", 18.0f,nullptr,ranges.Data);
    ImFont* icon = io.Fonts->AddFontFromFileTTF(FONT_ICON_FILE_NAME_FAS, 18.0f*2.0f/3.0f, &config, ranges_icon.Data);
    ImFont* emoticon_font = io.Fonts->AddFontFromFileTTF("CODE2000.TTF", 18.0f, &mergeConfig, ranges_emoticon.Data);
    ImFont* font2 = io.Fonts->AddFontFromFileTTF("CODE2000.TTF", 40.0f, nullptr, ranges.Data);
    ImFont* icon2 = io.Fonts->AddFontFromFileTTF(FONT_ICON_FILE_NAME_FAS, 40.0f * 2.0f / 3.0f, &config, ranges_icon.Data);
    ImFont* emoticon_font2 = io.Fonts->AddFontFromFileTTF("CODE2000.TTF", 40.0f * 2.0f / 3.0f, &mergeConfig, ranges_emoticon.Data);
    
    io.Fonts->Build();
    bool quit = false;
    string word;
    string definition;
    bool openPopup = false;
    string PATH = "Datasets/";
    static int selectedDict = -1;
    const char* files[] = { "en-en.txt", "en-vi.txt", "vi-en.txt", "vi-vi.txt","emoticon_dict.txt"};
    string pathSelected;
    MyDictionary myDictionary;
    atomic<bool> done(true);
    atomic<bool> doneSearch(true);
    thread initDict;
    thread search;
    vector<string>  wordSearched;
    string wordToDisplayDefinition;
    vector<string> definitionsToDisplay;
    int isDefinitionEditing = -1;
    bool openInsertWindow = false;
    while (!quit)
    {
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            ImGui_ImplSDL2_ProcessEvent(&event);
            if (event.type == SDL_QUIT)
                quit = true;
            if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_CLOSE && event.window.windowID == SDL_GetWindowID(window))
                quit = true;
        }

        // Start the Dear ImGui frame
        ImGui_ImplSDLRenderer_NewFrame();
        ImGui_ImplSDL2_NewFrame();
        ImGui::NewFrame();

        // 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()!
        //if (show_demo_window)
            //ImGui::ShowDemoWindow(&show_demo_window);

        {
            ImGui::SetNextWindowSize({ 550,720 }, ImGuiCond_Once);
            ImGui::SetNextWindowPos({ 0,0 }, ImGuiCond_Once);
            ImGui::Begin("Control",nullptr,ImGuiWindowFlags_NoCollapse|ImGuiWindowFlags_NoMove);
            
            if(ImGui::Button(ICON_FA_LIST" Init Dictionary"))
                ImGui::OpenPopup("Directory");
            ImGui::SameLine();
            if(selectedDict==-1)
                ImGui::Text("Current dictionary size : %d",myDictionary.getSize());
            else
                ImGui::Text("Loading %s %c", files[selectedDict], "|/-\\"[(int)(ImGui::GetTime() / 0.05f) & 3]);
            if (ImGui::BeginPopup("Directory"))
            {
                ImGui::SeparatorText("All file");
                for (int i = 0; i < IM_ARRAYSIZE(files); i++)
                    if (ImGui::Selectable(files[i]))
                        selectedDict = i;
                
                ImGui::EndPopup();
            }
            if (ImGui::Button(ICON_FA_SQUARE_PLUS" Insert entry")) {
                openInsertWindow = true;
            }
            ImGui::SameLine();
            if (ImGui::Button(ICON_FA_CLOCK_ROTATE_LEFT" Reset Dictionary")) {
                myDictionary.resetDictionary();
                wordSearched.clear();
                wordToDisplayDefinition = "";
                definitionsToDisplay.clear();
                isDefinitionEditing = -1;
                openInsertWindow = false;
            }
            inputStringVietnameseVNI("##Search word", "Type the word here","Word input",word);
            
            ImGui::SameLine();
            if (ImGui::Button(ICON_FA_MAGNIFYING_GLASS" Search Word")) {
                
                wordSearched=myDictionary.getNWordsUnderneath(word, 50);
                cout << word << endl;
                if (wordSearched.size() > 0) {
                    wordToDisplayDefinition = wordSearched[0];
                    definitionsToDisplay = myDictionary.searchDefinition(wordSearched[0]);
                }
                
            }
            inputStringVietnameseVNI("##Search definition", "Type the definition here", "Definition input",definition);
            ImGui::SameLine();
            if (ImGui::Button(ICON_FA_MAGNIFYING_GLASS" Search Definition")) {
                doneSearch = false;
                search = thread(searchDefinition, ref(myDictionary), ref(wordSearched), definition, ref(doneSearch));
            }
            if(!doneSearch) ImGui::Text("Loading %c", "|/-\\"[(int)(ImGui::GetTime() / 0.05f) & 3]);
            else {
                if (search.joinable()) search.join();
            }
            ImGui::SeparatorText("Words List");
            if (ImGui::BeginTabBar("MyTabBar", ImGuiTabBarFlags_None)) {
                if (ImGui::BeginTabItem(ICON_FA_MAGNIFYING_GLASS" Word Searched")) {
                    for (int i = 0; i < wordSearched.size(); i++) {
                        if (ImGui::Selectable(wordSearched[i].c_str(), wordToDisplayDefinition == wordSearched[i])) {
                            definitionsToDisplay = myDictionary.searchDefinition(wordSearched[i]);
                            wordToDisplayDefinition = wordSearched[i];
                        }
                    }
                    ImGui::EndTabItem();
                }
                if (ImGui::BeginTabItem(ICON_FA_HEART" Favorite Words")) {
                    vector<string> favoriteWords = myDictionary.getFavorite();
                    for (int i = 0; i < favoriteWords.size(); i++) {
                        if (ImGui::Selectable(favoriteWords[i].c_str(), wordToDisplayDefinition == favoriteWords[i])) {
                            definitionsToDisplay = myDictionary.searchDefinition(favoriteWords[i]);
                            wordToDisplayDefinition = favoriteWords[i];
                        }
                    }
                    ImGui::EndTabItem();
                }
                if (ImGui::BeginTabItem(ICON_FA_BOOK" Search History")) {
                    vector<string> searchHistory = myDictionary.getHistory();
                    for (int i = 0; i < searchHistory.size(); i++) {
                        if (ImGui::Selectable(searchHistory[i].c_str(), wordToDisplayDefinition == searchHistory[i])) {
                            definitionsToDisplay = myDictionary.searchDefinition(searchHistory[i]);
                            wordToDisplayDefinition = searchHistory[i];
                        }
                    }
                    ImGui::EndTabItem();
                }
                ImGui::EndTabBar();
            }
            
            ImGui::End();
        }
        if (selectedDict != -1 && !initDict.joinable()) {
            done = false;
            pathSelected = PATH + files[selectedDict];
            initDict=thread(initDictionary, ref(myDictionary), pathSelected, ref(done));
        }
        if (done && initDict.joinable()) {
            initDict.join();
            selectedDict = -1;
        }
        ImGui::SetNextWindowSize({ 725,720 }, ImGuiCond_Once);
        ImGui::SetNextWindowPos({ 551,0 }, ImGuiCond_Once);
        {
            ImGui::Begin("Display Definition", nullptr,ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove);
            if (definitionsToDisplay.size() != 0) {
                ImGui::PushFont(font2);
                ImGui::Text("%s",wordToDisplayDefinition.c_str());
                ImGui::SameLine();
                ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor(0.95f, 0.95f, 0.95f));
                ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor(0.90f, 0.90f, 0.90f));
                ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)ImColor(0.85f, 0.85f, 0.85f));
                if (ImGui::Button(ICON_FA_CIRCLE_XMARK)) {
                    myDictionary.deleteWord(wordToDisplayDefinition);
                    myDictionary.removeFavorite(wordToDisplayDefinition);
                    if(find(wordSearched.begin(), wordSearched.end(), wordToDisplayDefinition)!=wordSearched.end())
                        wordSearched.erase(find(wordSearched.begin(),wordSearched.end(),wordToDisplayDefinition));
                    wordToDisplayDefinition = "";
                    definitionsToDisplay.clear();

                }
                ImGui::PopStyleColor(3);
                ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor(0.95f, 0.95f, 0.95f));
                ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor(0.95f, 0.95f, 0.95f));
                ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)ImColor(0.95f, 0.95f, 0.95f));
                if (myDictionary.checkInFavorite(wordToDisplayDefinition)) {
                    ImGui::PushStyleColor(ImGuiCol_Text, (ImVec4)ImColor(1.0f, 0.05f, 0.05f));
                    ImGui::SameLine();
                    if (ImGui::Button(ICON_FA_HEART)) myDictionary.removeFavorite(wordToDisplayDefinition);
                    ImGui::PopStyleColor(4);
                }
                else {
                    ImGui::SameLine();
                    if (ImGui::Button(ICON_FA_HEART)) myDictionary.addFavorite(wordToDisplayDefinition);
                    ImGui::PopStyleColor(3);
                }
                ImGui::PopFont();
                ImGui::Separator();
                for (int i = 0; i < definitionsToDisplay.size(); i++) {
                    ImGui::Text("Definition #%d", i + 1);
                   ImGui::SameLine();
                   string iconLabel = ICON_FA_PEN_TO_SQUARE + (string)"##" + to_string(i);
                   string iconDeleteLabel = ICON_FA_CIRCLE_XMARK + (string)"##" + to_string(i);
                    if (ImGui::Button(iconLabel.c_str()) || isDefinitionEditing == i) {
                        isDefinitionEditing = i;
                        ImGui::InputTextMultiline("##Edit Definition", &definitionsToDisplay[i]);
                        if (ImGui::Button("Save")) {
                            myDictionary.changeDefinition(i + 1, wordToDisplayDefinition, definitionsToDisplay[i]);
                            isDefinitionEditing = -1;
                        }
                        if (ImGui::Button("Cancel")) {
                            definitionsToDisplay = myDictionary.searchDefinition(wordToDisplayDefinition);
                            isDefinitionEditing = -1;
                        }
                    }
                    else {
                        ImGui::SameLine();
                        if (ImGui::Button(iconDeleteLabel.c_str())) {
                            myDictionary.deleteDefinition(i + 1, wordToDisplayDefinition);
                            definitionsToDisplay.erase(definitionsToDisplay.begin() + i);
                            continue;
                        }
                        ImGui::TextWrapped(definitionsToDisplay[i].c_str());
                    }
                    
                
                }
                if (ImGui::Button(ICON_FA_PLUS" Add Definition")) {
                    myDictionary.insertWord(wordToDisplayDefinition, "");
                    definitionsToDisplay.push_back("");
                    isDefinitionEditing = definitionsToDisplay.size() - 1;
                }
            }
            

            ImGui::End();
        }
        if(openInsertWindow)
        {
            static string wordInsert = "";
            static vector<string> definitionsInsert = { "" };
            ImGui::Begin("Insert Word", &openInsertWindow);
            inputStringVietnameseVNI("Insert word", "Type the word to be inserted...", "Vietnamese input", wordInsert);
            for (int i = 0; i < definitionsInsert.size(); i++) {
                string label = "Definition " + to_string(i);
                string popupLabel = "Vietmanese input " + to_string(i);
                inputStringMultilineVietnameseVNI(label.c_str(), { 300,50 }, popupLabel.c_str(), definitionsInsert[i]);
            }
            if (ImGui::Button(ICON_FA_SQUARE_PLUS" Add Definition ")) {
                definitionsInsert.push_back("");
            }
            if (ImGui::Button(ICON_FA_FLOPPY_DISK" Save to Dictionary")) {
                for (int i = 0; i < definitionsInsert.size(); i++) {
                    myDictionary.insertWord(wordInsert, definitionsInsert[i]);
                    openInsertWindow = false;
                    
                }
            }
            ImGui::SameLine();
            if (ImGui::Button(ICON_FA_LEFT_LONG" Cancel")) {
                wordInsert = "";
                definitionsInsert = { "" };
                openInsertWindow = false;
            }
            ImGui::End();

        }
        // Rendering
        ImGui::Render();
        SDL_RenderSetScale(renderer, io.DisplayFramebufferScale.x, io.DisplayFramebufferScale.y);
        SDL_SetRenderDrawColor(renderer, (Uint8)(clear_color.x * 255), (Uint8)(clear_color.y * 255), (Uint8)(clear_color.z * 255), (Uint8)(clear_color.w * 255));
        SDL_RenderClear(renderer);
        ImGui_ImplSDLRenderer_RenderDrawData(ImGui::GetDrawData());
        SDL_RenderPresent(renderer);
    }

    // Cleanup
    ImGui_ImplSDLRenderer_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}