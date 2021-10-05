#ifndef __PLAYER_H
#define __PLAYER_H
#include "..\engine\IDefine.h"
#include "..\engine\DBLink.h"
class Player
{
public:
	int ID;
	std::atomic<int> Type = 1; // 0 已断开 1 已连接 2需要保存  3 保存中
	int TCPIndex;
	SOCKET sock;

	//===========自定义玩家数据================
	//例子 玩家游戏币
	std::atomic<int> money;
	
	bool AddMoney(int AddNum);




	//===========================


};



#endif // !__PLAYER_H

