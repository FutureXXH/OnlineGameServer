#pragma once


#ifndef __SHAREFUNCTION_H
#define __SHAREFUNCTION_H
#include "tinyxml2.h"
#include "..\engine\IDefine.h"
#include <string>

using namespace std;
namespace share
{
	extern int LoadServerXML(const char* filename, const char* filepath, int& saveport, int& safecode, int& IOthreadnum, int& Servicethreadnum, int& MaxConnect,int& MaxOnlinePlayer, string& DB_IP, int& DB_port);
	extern bool InitData();

}


#endif // !__SHAREFUNCTION_H

