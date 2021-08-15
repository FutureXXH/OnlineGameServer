#include "ShareFunction.h"

namespace share
{
	int LoadServerXML(const char* filename, const char* filepath, int &  saveport,int & safecode)
	{
		std::string loadfilepath(filepath);
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


		SERVERPRINT_INFO << "加载服务器配置文件成功" << std::endl;

		return 0;
	}



	bool InitData()
	{
		
		return false;
	}



}

