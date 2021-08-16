#include "Player.h"
#include <time.h>

Player::Player(SOCKET sock)
{
	ID = NULL;
	isConnect = true;
	islogin = false;
	PlayerSOCK = sock;

	starttime = clock() / CLOCKS_PER_SEC;


}

Player::~Player()
{

	SERVERPRINT_INFO << "已删除玩家业务层数据" << std::endl;
}
