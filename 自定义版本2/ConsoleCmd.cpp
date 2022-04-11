#include "ConsoleCmd.h"
ConsoleCMD * __ConsoleCMD = new ConsoleCMD();




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
        cout << "命令列表" << endl;
        int i = 0;
    for (auto it = __ConsoleCMD->cmdInfo.begin(); it != __ConsoleCMD->cmdInfo.end(); it++,i++)
    {
        cout  << i << "  " << it->first << " : " << it->second << endl; 
    }
  
}

void stopServer()
{
    cout << "关闭服务器" << endl;
    exit(0);
}


//=================================================================================
ConsoleCMD::ConsoleCMD()
{
   Register_Console_CMD("help",&printCmdHelp,"命令列表");
  Register_Console_CMD("stop",&stopServer,"关闭服务器");

}
