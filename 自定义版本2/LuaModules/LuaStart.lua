
print("[Lua]: lua文件打开成功!")
ID = -1
--使用这种来使重载时保留原值
i = i or 0




--在模块加载后调用 传入了模块ID号
function OnInit(id)
    ID = id
    print("[Lua]: luaManager初始化成功  ID: "..ID)
    --在lua中加载模块
    ServerLuaLib.LoadNewModule(2000,"lua1.lua")
    ServerLuaLib.LoadNewModule(3000,"lua1.lua") 
    --关闭模块
    ServerLuaLib.CloseModule(ID);
end


function OnUpdate()
    
 
end

--在有消息达到时调用
function OnParseMessage(MessageID,data,srcModuleID)

end

--模块删除时调用
function OnExit()
    print("[Lua]: 模块: "..ID.. "退出")
end

