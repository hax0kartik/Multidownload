#include "fs.hpp"
#include <iostream>
#include <string>
#include <sstream>
#include <fstream>

using namespace std;
void wf(string a,string b)
{
	ofstream out(a);
	out << b;
	out.close();
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