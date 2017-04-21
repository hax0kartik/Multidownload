#include "fs.hpp"
#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include <cstdio>
#include <iomanip>
using namespace std;
void wf(string a,uint64_t* b, size_t size)
{
	cout<<"Writing the file please wait..."<<endl;
	FILE *fp;
	fp = fopen(a.c_str() , "w+b" );
	fwrite(b, 1 , size, fp);
	fclose(fp);
}	
string rf(string a)
{
	ifstream input_file(a);
	if (!input_file.is_open()) 
	{ 
      input_file.close();// check for successful opening
      return "error";
	}
    string buffer;
    input_file >> buffer;
	input_file.close();
	return buffer;
}