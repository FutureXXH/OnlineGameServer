#ifndef __PLAYERMANAGER_H
#define __PLAYERMANAGER_H
#include  "..\engine\IDefine.h"
#include <map>
#include "..\engine\TCPServer.h"
#include "Player.h"
#include <concurrent_queue.h>
#include <atomic>
using namespace std;
//玩家数据缓存管理器（在线）
class PlayerManager
{
private:
	//通过ID为键的红黑树
	map<int,Player*> Playerlist_ID;
	//通过socket为键的红黑树
	map<SOCKET, Player*> Playerlist_Socket;

	shared_mutex PlayerMutex;

	Concurrency::concurrent_queue<Player*> FreePlayerPool;

public:
	//当前在线玩家数
	atomic<int> CurOnlinePlayer = 0;
	//最大在线玩家
	int MaxPlayer;



	//插入玩家数据缓存 1 成功 -1 冲突 -2 达到最大值失败
	int InsertPlayerInfo(int id, SOCKET sock, Player* AddPlayer);

	//通过ID查询
	Player* ID_Find(int id);
	//通过Socket查询
	Player* SOCKET_Find(SOCKET sock);

	//检测玩家状态 如果为0 删除释放  如果为2进行全量保存后设置为3 
	void CheckPlayer();



	//删除释放玩家数据缓存
	int DeletePlayer(Player* playerinfo);

	//发送全量数据给DB保存
	int SaveAllToDB(Player* playerinfo);


	//推出一个玩家对象
	Player* PopFreePlayer();

	//初始化玩家管理器
	int init(int MaxPlayerNum);
};


extern PlayerManager* __PlayerManager;
#endif // !__PLAYERMANAGER_H

