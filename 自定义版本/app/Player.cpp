#include "Player.h"

bool Player::AddMoney(int AddNum)
{
    money += AddNum;


    //是否发送给DB修改数据？
    //__DBLink->SendDB()
    // ===========================
    return true;
}
