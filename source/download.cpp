#include <iostream>
#include "download.hpp"
#include <string>
#include <iomanip>
#include <3ds.h>
#include "fs.hpp"
#include "gfx.hpp"
#include "util.hpp"
#include "extract_zip.hpp"
#include <cstdlib>
using namespace std;
Result http_download(string url,string loca)
{
	Result ret = 0;
	u32 statuscode=0;
	u32 contentsize=0, readsize=0, size=0x1000;
	char a[2048];
	string strNew;
	httpcContext context;
	extern PrintConsole top,bottom;
	consoleSelect(&top);
	cout<<"\x1b[2J";
	pBar bar;
	fs out;
	consoleSelect(&bottom);
	cout<<"\x1b[2J";
	bar.length_set(contentsize);
	bar.update(0);
	bar.print();
	consoleSelect(&top);
	cout<<"\x1b[33;1mDownloading : \x1b[37;1m"<<url<<endl;
	ret = httpcOpenContext(&context, HTTPC_METHOD_GET, url.c_str(), 0);
	if (ret != 0)
	{
		cout<<"\x1b[31;1mError 0x"<<hex<<setw(8)<<ret<<endl;
		return 1;
	}
	ret = httpcAddRequestHeaderField(&context, (char*)"User-Agent", (char*)"MULTIDOWNLOAD++");
	if (ret != 0)
	{
		cout<<"\x1b[31;1mError 0x"<<hex<<setw(8)<<ret<<endl;
		return 1;
	}
	ret = httpcSetSSLOpt(&context, 1 << 9);
	if (ret != 0)
	{
		cout<<"\x1b[31;1mError 0x"<<hex<<setw(8)<<ret<<endl;
		return 1;
	}
	ret = httpcBeginRequest(&context);
	if (ret != 0)
	{
		cout<<"\x1b[31;1mError 0x"<<hex<<setw(8)<<ret<<endl;
		return 1;
	}
	ret = httpcGetResponseStatusCodeTimeout(&context, &statuscode,6000000000);
	if (ret != 0)
	{	cout<<"\x1b[31;1mError 0x"<<hex<<setw(8)<<ret<<endl;
		cout<<"Wrong protocol/Internet not connected "<<url<<endl;
		httpcCloseContext(&context);
		return 1;
	}
		cout<<"\x1b[33;1mStatuscode:\x1b[37;1m"<<statuscode<<endl;
	if ((statuscode >= 301 && statuscode <= 303) || (statuscode >= 307 && statuscode <= 308)) 
	{
		char newurl[1024];
		httpcGetResponseHeader(&context, (char*)"Location", newurl, 1024);
		httpcCloseContext(&context);
		return http_download(newurl,loca);
	}
	if (statuscode != 200)
		return 1;
	ret=httpcGetResponseHeader(&context, (char*)"Content-Disposition", a, 2048);
	if(ret!=0)
	{
		int pos = url.find_last_of("/");
		strNew = url.substr(pos+1);
	}	
	else
	{
		string contents(a,strlen(a));
		int pos = contents.find("=");
		strNew = contents.substr (pos+1);
	}
	if(strNew.find_first_of("\"")!= string::npos)
	{
		unsigned first=strNew.find_first_of("\"");
		unsigned last=strNew.find_last_of("\"");
		strNew = strNew.substr(first+1,last-first-1);
	}	
	cout <<"\x1b[33;1mFileName : \x1b[37;1m"<<strNew<<endl;
	ret = httpcGetDownloadSizeState(&context, NULL, &contentsize);
	if (ret != 0)
	{
		cout<<"\x1b[31;1mError 0x"<<hex<<setw(8)<<ret<<endl;
		return 1;
	}
	cout<<"\x1b[33;1msize(may be wrong) : \x1b[37;1m"<<contentsize<<endl;
	gfxFlushBuffers();
	gfxSwapBuffers();
	
	//We'll now check for invalid characters in filename(unlikely to occur)
	if(strNew.find("/")!=string::npos||strNew.find("#")!=string::npos||strNew.find("\\")!=string::npos||strNew.find(":")!=string::npos||strNew.find("*")!=string::npos
	||strNew.find("?")!=string::npos||strNew.find("\"")!=string::npos||strNew.find("<")!=string::npos||strNew.find(">")!=string::npos||strNew.find("|")!=string::npos
	||strNew.find("%")!=string::npos||strNew.empty())
	{
		strNew = tl(1,"Please enter the filename along with the extension.Eg:-multi.zip,k.3dsx,file.pdf,etc.","OK");
		cout<<strNew<<endl;
	}
	string location=loca + strNew;
	consoleSelect(&top);
	cout<<"\r\x1b[33;1mFile saved as : \x1b[37;1m"<<location<<endl;
	out.openfile(location);
	bar.length_set(contentsize);
	consoleSelect(&bottom);
	bar.print();
	do
	{
		u8 *buf = (u8*)linearAlloc(size);
		memset(buf, 0, size);
		ret = httpcDownloadData(&context, buf, 0x1000, &readsize);
		size += readsize;
		if(contentsize!=0){
		bar.update(readsize);
		bar.print();
		}
		else{
			;
		}
        out.writefile((const char*)buf,(size_t)readsize);
		linearFree(buf);
    }while(ret == (s32)HTTPC_RESULTCODE_DOWNLOADPENDING);
	out.closefile();
	consoleSelect(&top);
	if(ret!=0){
		httpcCloseContext(&context);
		return 1;
	}

	if((location.find(".zip")!=string::npos)||(location.find(".rar")!=string::npos))
	{	cout<<"Zip file found"<<endl;
		extract(location);
	}
	httpcCloseContext(&context);
	return 0;
}
