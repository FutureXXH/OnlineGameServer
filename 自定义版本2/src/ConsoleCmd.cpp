#include "../include/ConsoleCmd.h"

   ConsoleCMD * __ConsoleCMD = nullptr;
    ConsoleLog *__ConsoleLog = nullptr;



bool ConsoleCMD::parseConsoleCMD(const string& cmd)
{

   
  if(cmdmap.count(cmd) == 0)return false;

  cmdmap[cmd]();
  return true;

}



bool ConsoleCMD::Register_Console_CMD(const string& cmd,function<void()> fun, string cmdinfo)
{
    cmdmap[cmd] = fun;
    cmdInfo[cmd] = cmdinfo;
    return true;
     
}



//=================================默认控制台命令=================================================
void printCmdHelp()
{
    Log(INFO,"命令列表");
    int i = 0;
    for (auto it = __ConsoleCMD->cmdInfo.begin(); it != __ConsoleCMD->cmdInfo.end(); it++,i++)
    {
        Log(INFO,to_string(i) + "| "+it->first+":"+it->second);

    }
  
}

void stopServer()
{

    Log(INFO,"关闭服务器");
    this_thread::sleep_for(chrono::seconds(1));
    exit(0);
}



ConsoleCMD::ConsoleCMD()
{
  
   


   Register_Console_CMD("help",&printCmdHelp,"命令列表");
   Register_Console_CMD("stop",&stopServer,"关闭服务器");

}

//=========================================================================

function<void(char level,string str)> Log;

void ConsoleLog::push_ConsoleLog(char level,string str)
{
   console_Logs.push(new pair<char,string>(level,str));
    cv.notify_one();

}


 string ConsoleLog::pop_ConsoleLog()
{
   auto Level_str = console_Logs.pop();
   auto mt = chrono::system_clock::to_time_t(chrono::system_clock::now());
   tm* nowtime = localtime(&mt);
   string str = str(nowtime->tm_hour) + ":" + str(nowtime->tm_min) + ":" + str(nowtime->tm_sec);
  switch (Level_str->first)
  {
  case 0:
    str += "[INFO]";
      break;
   case 1:
    str += "[WARNING]";   
      break;
  case 2:
    str += "[ERROR]";   
      break; 
  default:
      break;
  }

  str += " ";
  str += Level_str->second;
  delete Level_str;
  return str;   

}

void ConsoleLog::ConsoleThreadRun()
{
    unique_lock<mutex> lock(m);
    string str;
    Log(INFO,"服务器控制台初始化完毕");
    while (true)
    {
        cv.wait(lock);
        while (console_Logs.size()> 0)
        {
            str =  pop_ConsoleLog();
            cout <<str << endl;
        }
   }


    
}

ConsoleLog::ConsoleLog()
{
   
}