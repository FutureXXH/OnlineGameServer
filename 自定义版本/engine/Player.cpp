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
