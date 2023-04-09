#pragma once

#include <map>
#include <stdio.h>
#include <string>
#include <vector>

using namespace std;

class CFGParser
{
    struct CFGItem
    {
        string name;
        string value;
    };

  private:
    vector<pair<string, vector<CFGItem>>> items;
    FILE* cfgFile = nullptr;
    vector<char> cfgData;

  public:
    CFGParser(const char* path);
    ~CFGParser();

    void Parse();
    auto& GetItems() { return items; };
};
