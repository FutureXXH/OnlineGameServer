#ifndef __CONSOLECMD_H
#define __CONSOLECMD_H

#include "ModuleManager.h"
#include <unordered_map>
#include <functional>
#include <iostream>
using namespace std;

class ConsoleCMD
{
   unordered_map<string,function<void()>> cmdmap;
   unordered_map<string,string> cmdInfo;

   friend void printCmdhelp();
   
public:
ConsoleCMD();
bool parseConsoleCMD(const string& cmd);
bool Register_Console_CMD(const string& cmd,function<void()> fun,string cmdinfo);


};

extern ConsoleCMD * __ConsoleCMD;

#endif 
