#ifndef __PLAYERMANAGER_H
#define __PLAYERMANAGER_H
#include  "..\engine\IDefine.h"
#include <map>
#include "..\engine\TCPServer.h"
#include <atomic>
using namespace std;
//玩家数据缓存管理器
class PlayerManager
{
private:
	//通过ID为键的红黑树
	map<int,PlayerInfo*> Playerlist_ID;
	//通过socket为键的红黑树
	map<SOCKET, PlayerInfo*> Playerlist_Socket;

public:

	atomic<int> CurOnlinePlayer = 0;




	//插入玩家数据缓存
	bool InsertPlayerInfo(int id, SOCKET sock, PlayerInfo* AddPlayerInfo);

	//通过ID查询
	PlayerInfo* ID_Find(int id);
	//通过Socket查询
	PlayerInfo* SOCKET_Find(SOCKET sock);

	//检测玩家状态 如果为0 删除释放  如果为2进行全量保存后设置为3 
	void CheckPlayer();



	//删除释放玩家数据缓存
	int DeletePlayer(PlayerInfo* playerinfo);

	//发送全量数据给DB保存
	int SaveAllToDB(PlayerInfo* playerinfo);


};


extern PlayerManager* __PlayerManager;
#endif // !__PLAYERMANAGER_H

