#pragma execution_character_set("utf-8")
#define  _NO_CRT_STDIO_INLINE
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
#include <random>
#include <chrono>
#include <filesystem>
#include "utils.h"
using namespace std;
using namespace std::chrono;
#if !SDL_VERSION_ATLEAST(2,0,17)
#error This backend requires SDL 2.0.17+ because of SDL_RenderGeometry() function
#endif
void initDictionary(MyDictionary& myDictionary,string PATH, atomic<bool>& done) {
    myDictionary.freeDictionary();
    myDictionary = MyDictionary(PATH, 1);
    done = true;
}

void searchDefinition(MyDictionary& myDictionary, vector<string>&wordSearched,string definitionSearch,atomic<bool>& doneSearch) {
    vector<pair<string, int>> ans = myDictionary.searchWords(definitionSearch, 10);
    wordSearched.clear();
    for (auto p : ans) if(p.first!="")  wordSearched.emplace_back(p.first);
    doneSearch = true;
}

void clearDictionary(MyDictionary& myDictionary, atomic<bool>& doneClear) {
    myDictionary.freeDictionary();
    doneClear = true;
}

void saveDictionary(MyDictionary& myDictionary, atomic<bool>& doneSave) {
    myDictionary.toFile();
    doneSave = true;
}

void loadDictionary(MyDictionary& myDictionary, string path, atomic<bool>& doneLoad) {
    myDictionary.freeDictionary();
    myDictionary = MyDictionary(path, 15);
    doneLoad = true;
}

string getTime(Uint32 totalSeconds) {
    string minutes = to_string(totalSeconds / 60);
    string seconds = to_string(totalSeconds % 60);
    if (minutes.size() == 1) minutes = "0" + minutes;
    if (seconds.size() == 1) seconds = "0" + seconds;
    return minutes + ":" + seconds;
}
vector<string> getRandomWordAndDefinitionWithDiff(int diff, MyDictionary& myDictionary) {
    int minn = 2 + (diff - 1) * 3;
    int maxx = 2 + diff * 3;
    while (true) {
        vector<string> wordWithDef = myDictionary.getRandomWordAndDefinition();
        if (wordWithDef[0].size() >= minn && wordWithDef[0].size() <= maxx) return wordWithDef;
    }
}
vector<vector<string>> generateQuiz(int typeOne, int typeTwo, int diff,MyDictionary& myDictionary) {
    vector<vector<string>> ans;
    for (int i = 0; i < typeOne; i++) {
        vector<string>rep = getRandomWordAndDefinitionWithDiff(diff,myDictionary);
        vector<string>rep2 = getRandomWordAndDefinitionWithDiff(diff, myDictionary);
        vector<string>rep3 = getRandomWordAndDefinitionWithDiff(diff, myDictionary);
        vector<string>rep4 = getRandomWordAndDefinitionWithDiff(diff, myDictionary);
        vector<vector<string>>v = { rep,rep2,rep3,rep4 };
        shuffle(v.begin(), v.end(), default_random_engine(time(0)));
        int randomInt = rand() % 4;
        string word = v[randomInt][0];
        ans.push_back({ "1",word,v[0][1],v[1][1],v[2][1],v[3][1],to_string(randomInt)});
    }
    for (int i = 0; i < typeTwo; i++) {
        vector<string>rep = getRandomWordAndDefinitionWithDiff(diff, myDictionary);
        vector<string>rep2 = getRandomWordAndDefinitionWithDiff(diff, myDictionary);
        vector<string>rep3 = getRandomWordAndDefinitionWithDiff(diff, myDictionary);
        vector<string>rep4 = getRandomWordAndDefinitionWithDiff(diff, myDictionary);
        vector<vector<string>>v = { rep,rep2,rep3,rep4 };
        shuffle(v.begin(), v.end(), default_random_engine(time(0)));
        int randomInt = rand() % 4;
        string def = v[randomInt][1];
        ans.push_back({ "2",def,v[0][0],v[1][0],v[2][0],v[3][0],to_string(randomInt)});
    }
    return ans;
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
    ImFontGlyphRangesBuilder builder,emoticonBuilder,iconBuilder,emojiBuilder;
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
    iconBuilder.AddText(ICON_FA_BOOK_OPEN_READER);
    iconBuilder.AddText(ICON_FA_GEAR);
    iconBuilder.AddText(ICON_FA_FILE_EXPORT);
    iconBuilder.AddText(ICON_FA_UPLOAD);
    iconBuilder.AddText(ICON_FA_ERASER);
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
    ImFont* font3 = io.Fonts->AddFontFromFileTTF("Quantico-Bold.ttf", 24.0f, nullptr, io.Fonts->GetGlyphRangesDefault());
    ImFont* font4 = io.Fonts->AddFontFromFileTTF("E1234.ttf", 36.0f, nullptr, io.Fonts->GetGlyphRangesDefault());
    ImFont* font5 = io.Fonts->AddFontFromFileTTF("FasterOne-Regular.ttf", 48.0f, nullptr, io.Fonts->GetGlyphRangesDefault());
    io.Fonts->Build();
    ImGuiStyle& style = ImGui::GetStyle();
    style.TabRounding = 12.0;
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
    atomic<bool> doneSave(true);
    atomic<bool>doneLoad(true);
    atomic<bool> doneClear(true);
    thread initDict;
    thread search;
    thread save;
    thread load;
    thread clear;
    vector<string>  wordSearched;
    string wordToDisplayDefinition;
    vector<string> definitionsToDisplay;
    int isDefinitionEditing = -1;
    bool openInsertWindow = false;
    bool openQuizSettingWindow = false;
    bool openStyleEditor = false;
    bool isDoingQuiz = false;
    int typeOneQuiz = 25;
    int typeTwoQuiz = 25;
    int diff = 1;
    int currQuiz = 0;
    vector<vector<string>> quizzes;
    vector<int>timesInSecond;
    vector<int>userAns;
    Uint64 startTime = time(0);
    double timeMark = 0;
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
        //if(!isDoingQuiz)
        {
            ImGui::SetNextWindowSize({ 550,720 }, ImGuiCond_Once);
            ImGui::SetNextWindowPos({ 0,0 }, ImGuiCond_Once);
            ImGui::Begin("Control",nullptr,ImGuiWindowFlags_NoCollapse|ImGuiWindowFlags_NoMove);
            if(ImGui::Button(ICON_FA_LIST" Init Dictionary"))
                ImGui::OpenPopup("Directory");
            ImGui::SameLine();
            if(selectedDict==-1)
                ImGui::Text("Current dictionary size : %d",myDictionary.getSize());
            else {
                ImGui::Text("Loading %s %c", files[selectedDict], "|/-\\"[(int)(ImGui::GetTime() / 0.05f) & 3]);
                ImGui::SameLine();
                ImGui::SetCursorPosX(350);
                ImGui::Text("%.2fs", ImGui::GetTime() - timeMark);
            }
            if (ImGui::BeginPopup("Directory"))
            {
                ImGui::SeparatorText("All files");
                for (int i = 0; i < IM_ARRAYSIZE(files); i++)
                    if (ImGui::Selectable(files[i])) {
                        selectedDict = i;
                        timeMark = ImGui::GetTime();
                    }
                
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
            ImGui::SameLine();
            if (ImGui::Button(ICON_FA_BOOK_OPEN_READER" Create Quiz")&& myDictionary.getSize() > 5000 ) {
                openQuizSettingWindow = true;
            }
            ImGui::SameLine();
            if (ImGui::Button(ICON_FA_GEAR" Style Editor")) {
                openStyleEditor = true;
            }
            if (ImGui::Button(ICON_FA_ERASER" Clear Current Dictionary")) {

                doneClear = false;
                clear = thread(clearDictionary, ref(myDictionary), ref(doneClear));
                timeMark = ImGui::GetTime();
            }
            if (!doneClear) {
                ImGui::SameLine();
                ImGui::Text("Clearing %c", "|/-\\"[(int)(ImGui::GetTime() / 0.05f) & 3]);
                ImGui::SameLine();
                ImGui::SetCursorPosX(300);
                ImGui::Text("%.2fs", ImGui::GetTime() - timeMark);
            }
            else {
                if (clear.joinable()) clear.join();
            }
            if (ImGui::Button(ICON_FA_FILE_EXPORT" Save Current Dictionary")) {
                
                doneSave = false;
                save = thread(saveDictionary, ref(myDictionary), ref(doneSave));
                timeMark = ImGui::GetTime();
            }
            if (!doneSave) { 
                ImGui::SameLine();
                ImGui::Text("Saving %c", "|/-\\"[(int)(ImGui::GetTime() / 0.05f) & 3]);
                ImGui::SameLine();
                ImGui::SetCursorPosX(300);
                ImGui::Text("%.2fs", ImGui::GetTime() - timeMark);
            }
            else {
                if (save.joinable()) save.join();
            }
            if (ImGui::Button(ICON_FA_UPLOAD" Load Previous Dictionary")) {
                ImGui::OpenPopup("DS Directory");
            }
            if (ImGui::BeginPopup("DS Directory"))
            {
                ImGui::SeparatorText("All files");
                for (const auto& entry : filesystem::directory_iterator("SavedDataStructure/")) {
                    if (ImGui::Selectable(entry.path().string().c_str())) {
                        doneLoad = false;
                        load = thread(loadDictionary, ref(myDictionary), entry.path().string(), ref(doneLoad));
                        timeMark = ImGui::GetTime();
                    }
                }
                ImGui::EndPopup();
            }
            if (!doneLoad) {
                ImGui::SameLine();
                ImGui::Text("Loading %c", "|/-\\"[(int)(ImGui::GetTime() / 0.05f) & 3]);
                ImGui::SameLine();
                ImGui::SetCursorPosX(300);
                ImGui::Text("%.2fs", ImGui::GetTime() - timeMark);
            }
            else {
                if (load.joinable()) load.join();
            }
            ImGui::InputTextWithHint("##Search word", "Type the word here",&word,ImGuiInputTextFlags_EnterReturnsTrue);
            
            ImGui::SameLine();
            if (ImGui::Button(ICON_FA_MAGNIFYING_GLASS" Search Word")) {
                
                wordSearched=myDictionary.getNWordsUnderneath(word, 50);
                if (wordSearched.size() > 0) {
                    wordToDisplayDefinition = wordSearched[0];
                    definitionsToDisplay = myDictionary.searchDefinition(wordSearched[0]);
                }
                
            }
            ImGui::InputTextWithHint("##Search definition", "Type the definition here",&definition, ImGuiInputTextFlags_EnterReturnsTrue);
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
                    for (int i = searchHistory.size()-1; i >=0; i--) {
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
        {
            ImGui::SetNextWindowSize({ 725,720 }, ImGuiCond_Once);
            ImGui::SetNextWindowPos({ 551,0 }, ImGuiCond_Once);
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
                    int idxFound = find(wordSearched, wordToDisplayDefinition);
                    if (idxFound != -1)
                        wordSearched.erase(wordSearched.begin() + idxFound);
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
                    ImGui::PushFont(font3);
                    ImGui::Text("Definition #%d", i + 1);
                    ImGui::PopFont();
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
                    ImGui::Text("");
                
                }
                if (ImGui::Button(ICON_FA_PLUS" Add Definition")) {
                    myDictionary.insertWord(wordToDisplayDefinition, "");
                    definitionsToDisplay.emplace_back("");
                    isDefinitionEditing = definitionsToDisplay.size() - 1;
                }
            }
            

            ImGui::End();
        }
        if (openStyleEditor) {
            ImGui::Begin("Style Editor", &openStyleEditor);
            ImGui::ShowStyleEditor();
            ImGui::End();
        }
        if(openInsertWindow)
        {
            static string wordInsert = "";
            static vector<string> definitionsInsert = { "" };
            ImGui::Begin("Insert Word", &openInsertWindow);
            ImGui::InputTextWithHint("Insert word", "Type the word to be inserted...", &wordInsert,ImGuiInputTextFlags_EnterReturnsTrue);
            for (int i = 0; i < definitionsInsert.size(); i++) {
                string label = "Definition " + to_string(i);
                string popupLabel = "Vietmanese input " + to_string(i);
                ImGui::InputTextMultiline(label.c_str(), &definitionsInsert[i],{ 300,50 },ImGuiInputTextFlags_EnterReturnsTrue);
            }
            if (ImGui::Button(ICON_FA_SQUARE_PLUS" Add Definition ")) {
                definitionsInsert.emplace_back("");
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
        if (openQuizSettingWindow) {
            ImGui::Begin("Quiz Creation");
            ImGui::SliderInt("##1", &typeOneQuiz, 0, 50);
            ImGui::Text("Number of type 1 questions");
            ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 20);
            ImGui::SliderInt("##2", &typeTwoQuiz, 0, 50);
            ImGui::Text("Number of type 2 questions");
            ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 20);
            ImGui::SliderInt("##3", &diff, 1, 5);
            ImGui::Text("Difficulty");
            ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 20);
            if (ImGui::Button("Cancel")) {
                openQuizSettingWindow = false;
            }
            ImGui::SameLine();
            ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 200);
            if (ImGui::Button("Enter Quiz")) {
                openQuizSettingWindow = false;
                isDoingQuiz = true;
                quizzes= generateQuiz(typeOneQuiz, typeTwoQuiz, diff, myDictionary);
                timesInSecond = vector<int>(quizzes.size()+1, 0);
                userAns = vector<int>(quizzes.size(), -1);
                startTime = time(0);
                currQuiz = 0;
            }
            ImGui::End();
        }
        if (isDoingQuiz) {
            
            
            ImGui::SetNextWindowSize({ 700,400 },ImGuiCond_FirstUseEver);
            ImGui::Begin("Quiz", &isDoingQuiz);
            if (currQuiz < quizzes.size()) {
                ImGui::PushFont(font4);
                ImVec2 pos = ImGui::GetWindowPos();
                ImDrawList* draw_list = ImGui::GetWindowDrawList();
                draw_list->AddCircleFilled({ pos[0]+ImGui::GetWindowWidth() / 2,pos[1]+8}, 85, ImGui::GetColorU32(IM_COL32(59, 102, 219, 255)), 40);
                ImGui::SetCursorPosX(ImGui::GetWindowWidth() / 2 - 36.0 * 1.8);
                ImGui::TextWrapped(getTime(time(0) - startTime).c_str());
                ImGui::PopFont();
                ImGui::Text("");
                string question = "Question "+to_string(currQuiz + 1) + " : " + "What is the " + (quizzes[currQuiz][0] == "1" ? "definition " : "word ") + "for the " + (quizzes[currQuiz][0] == "1" ? "word \"" : "definition \n\"") + quizzes[currQuiz][1]+"\"";
                ImGui::TextWrapped(question.c_str());
                for (int i = 1; i <= 4; i++) {
                    string ans = to_string(i) + " . " + quizzes[currQuiz][i + 1];
                    if (userAns[currQuiz] != -1) {
                        if (to_string(i - 1) == quizzes[currQuiz][6]) {
                            ImGui::PushStyleColor(ImGuiCol_Text, { 0,0.9f,0,1.0f });
                            ImGui::TextWrapped(ans.c_str());
                            ImGui::PopStyleColor();
                        }
                        else if (i - 1 == userAns[currQuiz]) {
                            ImGui::PushStyleColor(ImGuiCol_Text, { 0.9f,0,0,1.0f });
                            ImGui::TextWrapped(ans.c_str());
                            ImGui::PopStyleColor();
                        }
                        else {
                            ImGui::TextWrapped(ans.c_str());
                        }
                    }
                    else {
                        ImGui::TextWrapped(ans.c_str());
                    }
                }
                ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, style.FrameRounding+30.0f);
                ImGui::PushFont(font2);
                ImGui::SetCursorPosX(ImGui::GetWindowWidth() / 2.0 - 80 * 2);
                if (ImGui::Button("1",{60,60}))
                    userAns[currQuiz] = 0;
                ImGui::SameLine();
                if (ImGui::Button("2",{60,60}))
                    userAns[currQuiz] = 1;
                ImGui::SameLine();
                if (ImGui::Button("3",{60,60}))
                    userAns[currQuiz] = 2;
                ImGui::SameLine();
                if (ImGui::Button("4",{60,60}))
                    userAns[currQuiz] = 3;
                ImGui::PopStyleVar();
                ImGui::PopFont();
                if (userAns[currQuiz] != -1) {
                    ImGui::SetCursorPosX(325);
                    if (to_string(userAns[currQuiz]) == quizzes[currQuiz][6]) {
                        ImGui::TextColored({ 0,200,0,255 }, "CORRECT");
                    }
                    else {
                        ImGui::TextColored({ 200,0,0,255 }, "WRONG");
                    }
                    ImGui::SetCursorPosX(310);
                    if (currQuiz > 0 && ImGui::Button("Prev")) {
                        timesInSecond[currQuiz] = time(0) - startTime;
                        startTime = time(0) - timesInSecond[currQuiz - 1];
                        currQuiz--;
                        
                    }
                    if (currQuiz > 0)   ImGui::SameLine();
                    if (ImGui::Button("Next")) {
                        timesInSecond[currQuiz] = time(0) - startTime;
                        startTime = time(0);
                        currQuiz++;
                    }
                }
            }
            else {
                ImDrawList* draw_list = ImGui::GetWindowDrawList();
                ImVec2 wSize = ImGui::GetWindowSize();
                ImVec2 wPos = ImGui::GetWindowPos();
                ImU32 col_a = ImGui::GetColorU32(IM_COL32(239, 239, 187, 255));
                ImU32 col_b = ImGui::GetColorU32(IM_COL32(212, 211, 221, 255));
                draw_list->AddRectFilledMultiColor(wPos, {wSize[0]+wPos[0],wSize[1]+wPos[1]}, col_a, col_b, col_b, col_a);
                if (timesInSecond[timesInSecond.size() - 1] == 0) {
                    int summ = 0;
                    for (int val : timesInSecond) summ += val;
                    timesInSecond[timesInSecond.size() - 1] = summ;
                }
                int correct = 0;
                for (int i = 0; i < quizzes.size(); i++) {
                    if (to_string(userAns[i]) == quizzes[i][6])
                        correct++;
                }
                ImGui::SetCursorPosY(100);
                ImGui::SetCursorPosX(200);
                ImGui::Text("︵‿︵‿︵‿︵‿︵‿︵‿︵‿︵‿︵‿︵‿︵‿︵‿");
                ImGui::PushFont(font2);
                ImGui::SetCursorPosX(200);
                ImGui::Text("Time taken   : %s", getTime(timesInSecond[timesInSecond.size() - 1]).c_str());
                ImGui::SetCursorPosX(200);
                ImGui::Text("Correct        : %d/%d",correct,quizzes.size());
                ImGui::SetCursorPosX(200);
                ImGui::Text("Performance :");
                ImGui::SameLine();
                ImGui::PopFont();
                ImGui::PushFont(font5);
                float percentage = (float)correct / (float)quizzes.size();
                int goodTime = 10 * quizzes.size();
                int standardTime = 20 * quizzes.size();
                const float arr[8] = { 0,0.5f,0.75f,0.9f,1.0f,1.01f };
                const string ranks[5] = {"D","C","B","A","S"};
                const ImVec4 colors[5] = {
                    {0,0,0,0.58f},
                    {0.08f,0.68f,0.36,1},
                    {0.3f,0.54f,0.89f,1},
                    {1,0.78f,0.004f,1},
                    {0.95f,0.3f,0.12f,1}
                };
                string rank;
                ImVec4 color;
                string bonus = "";
                for (int i = 0; i < 5; i++) {
                    if (percentage >= arr[i] && percentage < arr[i + 1]) {
                        rank = ranks[i];
                        color = colors[i];
                        break;
                    }
                }
                if (timesInSecond[timesInSecond.size() - 1] <= goodTime)
                    bonus="+";
                else if (timesInSecond[timesInSecond.size() - 1] <= standardTime) {}
                else bonus= "-";
                ImGui::TextColored(color, rank.c_str());
                ImGui::PopFont();
                ImGui::SameLine();
                ImGui::PushFont(font2);
                ImVec2 curPos = ImGui::GetCursorPos();
                ImGui::SetCursorPos({ curPos[0]-8 ,curPos[1]-5});
                ImGui::TextColored(color, bonus.c_str());
                ImGui::PopFont();
                ImGui::SetCursorPosX(200);
                ImGui::Text("︵‿︵‿︵‿︵‿︵‿︵‿︵‿︵‿︵‿︵‿︵‿︵‿");
                ImGui::Text("");
                ImGui::PushFont(font3);
                ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor(0.53f, 0.03f, 0.94f));
                ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor(0.43f, 0.13f, 0.94f));
                ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)ImColor(0.38f, 0.23f, 0.94f));
                ImGui::PushStyleColor(ImGuiCol_Text, (ImVec4)ImColor(1.0f, 1.0f,1.0f));
                ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, style.FrameRounding + 10.0f);

                if (ImGui::Button("<-- Back to Dictionary")) {
                    currQuiz = 0;
                    isDoingQuiz = false;
                }
                ImGui::PopFont();
                ImGui::PopStyleColor(4);
                ImGui::PopStyleVar();
            }
            ImGui::End();

        }
        //if (show_demo_window)
            //ImGui::ShowDemoWindow(&show_demo_window);
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