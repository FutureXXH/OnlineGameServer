#include "PlayerManager.h"


PlayerManager* __PlayerManager = nullptr;
int PlayerManager::InsertPlayerInfo(int id, SOCKET sock, PlayerInfo* AddPlayerInfo)
{
    if (CurOnlinePlayer >= MaxPlayer)
    {
        SERVERPRINT_WARNING << "服务器在线玩家达到设定值" << endl;
        return -2;
    }

    auto p = Playerlist_ID.find(id);
    auto p2 = Playerlist_Socket.find(sock);
    if (p != Playerlist_ID.end() || p2 != Playerlist_Socket.end())
    {
        SERVERPRINT_ERROR << "玩家数据已存在或冲突 插入失败" << endl;
        return -1;
    }

    Playerlist_ID.insert( make_pair(id, AddPlayerInfo));
    Playerlist_Socket.insert(make_pair(sock, AddPlayerInfo));
    SERVERPRINT_INFO << "玩家数据插入成功" << endl;
    CurOnlinePlayer++;
    return 1;
}

PlayerInfo* PlayerManager::ID_Find(int id)
{
    auto p = Playerlist_ID.find(id);
    if (p == Playerlist_ID.end())
    {
        SERVERPRINT_ERROR << "ID查找玩家失败" << endl;
        return nullptr;
    }

    return p->second;
}

PlayerInfo* PlayerManager::SOCKET_Find(SOCKET sock)
{
    auto p = Playerlist_Socket.find(sock);
    if (p == Playerlist_Socket.end())
    {
        SERVERPRINT_ERROR << "Socket查找玩家失败" << endl;
        return nullptr;
    }

    return p->second;
}

void PlayerManager::CheckPlayer()
{
    for (auto it = Playerlist_ID.begin(); it != Playerlist_ID.end(); )
    {
        switch (it->second->Type)
        {
        case 0:
        {
            auto temp = it;
            it++;
            DeletePlayer(temp->second);
        }
            break;
        case 2:
            SaveAllToDB(it->second);
            it++;
            break;
        default:
            it++;
            break;
        }
    }



}

int PlayerManager::DeletePlayer(PlayerInfo* playerinfo )
{
    auto p = Playerlist_Socket.find(playerinfo->sock);
    if (p == Playerlist_Socket.end())
    {
        SERVERPRINT_ERROR << "删除玩家时socket查找玩家失败" << endl;
        return -1;
    }
    auto p2 = Playerlist_ID.find(playerinfo->ID);
    if (p2 == Playerlist_ID.end())
    {
        SERVERPRINT_ERROR << "删除玩家时ID查找玩家失败" << endl;
        return -1;
    }
    delete p->second;
    Playerlist_Socket.erase(p);
    Playerlist_ID.erase(p2);
    CurOnlinePlayer--;

    return 1;
}

int PlayerManager::SaveAllToDB(PlayerInfo* playerinfo)
{
    return 0;
}

int PlayerManager::init(int MaxPlayerNum)
{
    if (MaxPlayerNum <= 0)
    {
        SERVERPRINT_WARNING << "最大在线玩家设置异常，将设置为默认值1000" << endl;
        MaxPlayer = 1000;
    }
    else
    {
        MaxPlayer = MaxPlayerNum;
    }

    return 1;
}

