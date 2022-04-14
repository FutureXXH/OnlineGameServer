

print("[Lua]: lua文件打开成功!")
--使用这种来使重载时保留原值
i = i or 0
--========================
ID = -1
function OnInit(id)
    ID = id
    print("[Lua]: lua模块初始化成功  ID: "..ID)
    GameServerLuaLib.RegMessage(5,ID)
end


function OnUpdate()
    


end

function OnParseMessage(MessageID,data,srcModuleID)
    i = i+1
    str = string.unpack("c10",data)
    print("[Lua]:收到第"..i.. "条消息 ID:"..MessageID .. " : "..str .." 来自模块ID:"..srcModuleID)

end


function OnExit()
    print("[Lua]: 模块: "..ID.. "退出")
end

