#ifndef __CONSOLECMD_H
#define __CONSOLECMD_H

#include "ModuleManager.h"
#include <unordered_map>
#include <functional>
#include <iostream>
#include "MyTool.h"
#include <condition_variable>
#include <mutex>
#include <chrono>
using namespace std;


class ConsoleCMD
{
   unordered_map<string,function<void()>> cmdmap;
   unordered_map<string,string> cmdInfo;


   
   friend void printCmdHelp();
   
public:
ConsoleCMD();
//解析控制台指令
bool parseConsoleCMD(const string& cmd);
//注册控制台指令
bool Register_Console_CMD(const string& cmd,function<void()> fun,string cmdinfo);



};

extern ConsoleCMD * __ConsoleCMD;





#define INFO 0
#define WARNING 1
#define ERROR 2

class ConsoleLog
{
   mutex m;
   condition_variable cv;
   ThreadSafe_Queue<pair<char,string>*> console_Logs;
public:
   ConsoleLog();
   void push_ConsoleLog(char level,string str);

   string pop_ConsoleLog();

   void ConsoleThreadRun();
};
extern ConsoleLog * __ConsoleLog;

extern function<void(char level,string str)> Log;



#endif 
