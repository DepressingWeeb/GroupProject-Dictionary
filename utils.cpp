#include "utils.h"
//TODO
void replace(string& word, char from, char to) {
    for (int i = 0; i < word.size(); i++) {
        if (word[i] == from) {
            word[i] = to;
        }
    }
}

int find(vector<string> words, string targetWord) {
    for (int i = 0; i < words.size(); i++) {
        if (words[i] == targetWord) {
            return i;
        }
    }
    return -1;
}

void replace_if(string& s, string from, char to) {
    for (int i = 0; i < s.size(); i++) {
        for (int j = 0; j < from.size(); j++) {
            if (s[i] == from[j]) {
                s[i] = to;
            }
        }
    }
}

void heapify(vector<pair<string, int>>& arr, int i, int n) {
    int smallest = i;
    int l = 2 * i + 1;
    int r = 2 * i + 2;
    if (l < n && arr[l].second < arr[smallest].second)
        smallest = l;
    if (r < n && arr[r].second < arr[smallest].second)
        smallest = r;
    if (smallest != i) {
        swap(arr[i], arr[smallest]);
        heapify(arr, smallest, n);
    }
}

void buildHeap(vector<pair<string, int>>& arr, int n) {
    for (int i = n / 2 - 1; i >= 0; i--) {
        heapify(arr, i, n);
    }
}

void HeapSort(vector<pair<string, int>>& arr, int n) {
    buildHeap(arr, n);
    for (int i = n - 1; i >= 0; i--) {
        swap(arr[0], arr[i]);
        heapify(arr, 0, i);
    }
}

void sortByFunc(vector<pair<string, int>>& v) {
    HeapSort(v, v.size());
}