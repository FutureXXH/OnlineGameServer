
print("[Lua]: lua文件打开成功!")
ID = -1
--使用这种来使重载时保留原值
i = i or 0

--注册消息
--ServerLuaLib.RegMessage(消息ID,模块ID)
--关闭模块
--ServerLuaLib.CloseModule(模块ID);
--加载模块
--ServerLuaLib.LoadNewModule(模块ID,模块文件名)
--发送消息
--ServerLuaLib.SendMessage(消息ID,数据,发送源模块ID);


--在模块加载后调用 传入了模块ID号
function OnInit(id)
    ID = id
    print("[Lua]: luaManager初始化成功  ID: "..ID)
    --在lua中加载模块
    --ServerLuaLib.LoadNewModule(2000,"lua1.lua")
    --ServerLuaLib.LoadNewModule(3000,"lua1.lua") 


end

--一直调用
function OnUpdate()
    
 
end

--在有消息达到时调用
function OnParseMessage(MessageID,data,srcModuleID)

end

--模块删除时调用
function OnExit()
    print("[Lua]: 模块: "..ID.. "退出")
end

