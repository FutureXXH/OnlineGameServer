

#ifndef __PLAYER_H
#define __PLAYER_H
#include"IDefine.h"

//这里对业务层玩家数据进行定义

class Player
{
public:

	int ID;
	bool islogin;
	bool isConnect;
	SOCKET PlayerSOCK;
	double starttime;

	Player(SOCKET sock);
	~Player();
};



#endif // !__PLAYER_H
