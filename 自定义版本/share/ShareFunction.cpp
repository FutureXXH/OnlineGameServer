#include "ShareFunction.h"

namespace share
{
	int LoadServerXML(const char* filename, const char* filepath, int &  saveport,int & safecode,int & IOthreadnum, int& Servicethreadnum,int &MaxConnect, int& MaxOnlinePlayer)
	{
		char FileExePath[128];
		if (filepath == nullptr)
		{
			memset(FileExePath, 0, 128);
			//获取当前exe路径 
			GetModuleFileNameA(NULL, (LPSTR)FileExePath, 128);
			std::string strPath(FileExePath);
			size_t pos = strPath.find_last_of('\\'); //除去最后的exe文件名 获取当前文件目录
			strPath = strPath.substr(0, pos + 1);
			memcpy(FileExePath, strPath.c_str(), 128);
		}

		std::string loadfilepath(FileExePath);
		loadfilepath += filename;
		
	
		SERVERPRINT_INFO << "正在加载" << loadfilepath.c_str() << std::endl;
		tinyxml2::XMLDocument xml;
		if (xml.LoadFile(loadfilepath.c_str()) != 0)
		{
			SERVERPRINT_WARNING << "加载服务器配置文件失败!" << std::endl;
			return -1;
		}
		//获取根节点
		tinyxml2::XMLElement* xmlRoot = xml.RootElement();
		if (xmlRoot == NULL)
		{
			SERVERPRINT_WARNING << "加载服务器配置文件时根节点不存在!" << std::endl;
			return -1;
		}
		
		//获取子节点 数据
		tinyxml2::XMLElement* xmlNode = xmlRoot->FirstChildElement("server");

		saveport = atoi(xmlNode->Attribute("Port"));
		safecode = atoi(xmlNode->Attribute("SafeCode"));
		IOthreadnum = atoi(xmlNode->Attribute("IOthreadnum"));
		Servicethreadnum = atoi(xmlNode->Attribute("Servicethreadnum"));
		MaxConnect = atoi(xmlNode->Attribute("MaxConnect"));
		MaxOnlinePlayer = atoi(xmlNode->Attribute("MaxOnlinePlayer"));


		SERVERPRINT_INFO << "加载服务器配置文件成功" << std::endl;
		SERVERPRINT_INFO << "====================================" << std::endl;
		SERVERPRINT_INFO << "服务器端口：" << saveport << std::endl;
		SERVERPRINT_INFO << "安全码：" << safecode << std::endl;
		SERVERPRINT_INFO << "IO线程数：" << IOthreadnum << std::endl;
		SERVERPRINT_INFO << "业务线程数："<< Servicethreadnum << std::endl;
		SERVERPRINT_INFO << "最大连接数："<<MaxConnect << std::endl;
		SERVERPRINT_INFO << "最大在线玩家数："<<MaxOnlinePlayer << std::endl;
		SERVERPRINT_INFO << "====================================" << std::endl;

		return 0;
	}



	bool InitData()
	{
		
		return false;
	}



}

