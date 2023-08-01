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

void Maxheapify(vector<pair<string, int>>& arr, int n, int l) {
    int a = n;
    if (2 * n + 1 <= l - 1 && arr[a].second < arr[2 * n + 1].second) {
        a = 2 * n + 1;
    }
    if (2 * n + 2 <= l - 1 && arr[a].second < arr[2 * n + 2].second) {
        a = 2 * n + 2;
    }
    if (a != n) {
        swap(arr[a], arr[n]);
        Maxheapify(arr, a, l);
    }
}

void buildMaxHeap(vector<pair<string, int>>& arr, int n) {
    for (int i = n / 2 - 1; i >= 0; i--) {
        Maxheapify(arr, i, n);
    }
}

void HeapSort(vector<pair<string, int>>& arr, int n) {
    buildMaxHeap(arr, n);
    for (int i = n - 1; i >= 0; i--) {
        swap(arr[0], arr[i]);
        Maxheapify(arr, 0, i);
    }
}

void sortByFunc(vector<pair<string, int>>& v) {
    HeapSort(v, v.size());
}