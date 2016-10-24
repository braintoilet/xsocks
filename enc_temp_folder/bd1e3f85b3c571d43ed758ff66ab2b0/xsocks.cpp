﻿#include "stdafx.h"
#include <map>
#include <string>
#include "Tunnel.h"
#include "dns.h"
#include "SocksMgr.h"

using namespace std;

typedef map<wstring,wstring> CMD_MAP;

void Version()
{
	return;
}

void Usage()
{
	printf("\nUsage    : xsocks  [-l port] [-t] [-p1 port] [-p2 port] [-s ip:port] \n");
	printf("                   [-r ip:port] [-u username] [-p password]          \n\n");

	printf("Options  : -l  Set forward mode.						               \n");
	printf("           -r  Set reverse mode.							           \n");
	printf("           -t  Build tunnel on socks5.                                 \n");
	printf("           -s  Redirect another socks server.                          \n");
	printf("           -u  Login username.				                           \n");
	printf("           -p  Login password.					                       \n");
	printf("           -p1 Accept of XSOCKS client's port.                         \n");
	printf("           -p2 Accept of proxy  client's port.                       \n\n");

	printf("Examples : xsocks -l 8085 -u root -p 123456                            \n");
	printf("           xsocks -t -p1 8085 -p2 8086                                 \n");
	printf("           xsocks -r 192.168.1.10:8085 -u root -p 123456               \n");
	printf("           xsocks -s 192.168.1.11:8085 -r 192.168.1.10:8086          \n\n");
}

void LoadCommand(int argc, _TCHAR* argv[] ,CMD_MAP& map)
{
	for(int i = 1 ; i < argc ; i++)
	{
		if (argv[i][0] == '-')
		{
			if (i+1 != argc && argv[i+1][0] != '-')
			{
				map[argv[i]] = argv[i+1];
			}
			else
			{
				map[argv[i]] = _T("");
			}
		}
	}
}

int _tmain(int argc, _TCHAR* argv[])
{
	WSADATA wsaData;
	WSAStartup(MAKEWORD(2,2),&wsaData);
	wstring user = _T("");
	wstring pwd = _T("");

	CMD_MAP cmd;

	LoadCommand(argc,argv,cmd);
	bool ret = FALSE;

	do 
	{
		//Lassen Sie überprüfen das Konto und Kennwort

		CMD_MAP::iterator it = cmd.find(_T("-p"));
		if ( it != cmd.end())
			pwd = it->second;

		it = cmd.find(_T("-u"));
		if ( it != cmd.end())
			user = it->second;

		if (user.length() != 0 && pwd.length() != 0)
			CSocksMgr::GetInstanceRef().SetAuth(user.c_str(),pwd.c_str());


		//Keine Parameter
		if (cmd.size() == 0)
		{
			Usage();
			ret = TRUE;
			break;
		}

		//Reverse-Modus
		it = cmd.find(_T("-r"));

		if (it != cmd.end())
		{
			int split = it->second.find(':');

			if (split == -1)
			{
				break;
			}

			string ip = t2a(it->second.substr(0,split).c_str());
			int port  = atoi(t2a(it->second.substr(split+1,it->second.length()).c_str()));

			it = cmd.find(_T("-s"));

			if (it == cmd.end())
			{
				Version();
				CSocksMgr::GetInstanceRef().Begin(ip.c_str(),port);
				ret = TRUE;
			}
			else
			{
				int split = it->second.find(':');
				if (split == -1)
				{
					break;
				}
				string ip2 = t2a(it->second.substr(0,split).c_str());
				int port2  = atoi(t2a(it->second.substr(split+1,it->second.length()).c_str()));

				Version();
				CSocksMgr::GetInstanceRef().Begin(ip.c_str(),port,ip2.c_str(),port2);
				ret = TRUE;
			}

			break;
		}

		//Tunnelmodus
		it = cmd.find(_T("-t"));

		if (it != cmd.end())
		{
			it = cmd.find(_T("-p1"));
			if (it == cmd.end() )
				break;

			int port1 = atoi(t2a(it->second.c_str()));

			it = cmd.find(_T("-p2"));
			if (it == cmd.end() )
				break;

			int port2 = atoi(t2a(it->second.c_str()));

			Version();
			CTunnel::GetInstanceRef().Begin(port1,port2);
			ret = TRUE;
			break;
		}

		//Forward Modus
		it = cmd.find(_T("-l"));

		if (it != cmd.end())
		{
			int port = atoi(t2a(it->second.c_str()));

			Version();
			CSocksMgr::GetInstanceRef().Begin(port);
			ret = TRUE;
			break;
		}



	} while (FALSE);

	if ( !ret )
	{
		Usage();
	}

	return 0;
}

