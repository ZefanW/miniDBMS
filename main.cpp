#include "dbms.h"
#include <Windows.h>
#include <fstream>
#include <sstream>
DBMS dbms;
#define YY_NO_UNISTD_H 1
#include<io.h>
#define isatty _isatty
#define fileno _fileno
#include "lex.yy.c"
using namespace std;
/*
bool checkValid(string &s)
{
	if (s.find("Parser error") != string::npos)return false;
	else return true;
}

DBMS dbms;
bool ReadProcess()
{
	string ordername;
	while (ordername.length() == 0)
	{
		if (cin.eof())return false;
		cin >> ordername;
	}
	if(ordername == "CREATE")
	{
		Create_Command toCreate;
		Table t;
		string s;
		while (s.length() == 0)getline(cin,s);
		toCreate.Set_Name(s);
		while (true)
		{
			do { getline(cin, s); } while (s[0]==' '||s[0]=='\0');
			if (s == "//")break;
			if (!checkValid(s))
			{
				printf("Parser error!\n");
				return false;
			}
			toCreate.Add_Attribute(s);
		}
		if(toCreate.tablename.length())dbms.Create(toCreate, t);
		//for (int i = 0; i < dbms.tables.size(); i++) {
			//dbms.tables[i].print_table();
		//}
	}
	else if(ordername == "INSERT")
	{
		Insert_Command toInsert;
		toInsert.attr_name.clear();
		string s;
		do { cin>>s; } while (s.length() == 0);
		toInsert.Set_Name(s);
		while (true)
		{
			do { getline(cin, s); } while (s.length() == 0);
			if (s == "CONTENTS")break;
			if (!checkValid(s))
			{
				printf("Parser error!\n");
				return false;
			}
			toInsert.Add_Attribute(s);
		}
		while (true)
		{
			do { getline(cin, s); } while (s.length() == 0);
			if (s == "//")break;
			if (!checkValid(s))
			{
				printf("Parser error!\n");
				return false;
			}
			toInsert.Add_Value(s);
		}
		dbms.Insert(toInsert);
	}
	else
	{
		cout << "Invalid command." << endl;
		return false;
	}
	return true;
}

int main()
{
	if (!freopen("scannedcode.txt", "r",stdin))
	{
		cout << "Cannot open File" << endl;
		return 0;
	}
	while (ReadProcess());
	for (int i = 0; i < dbms.tables.size(); i++) {
	dbms.tables[i].print_table();
	}
	fclose(stdin);
	return 0;
}

*/

int main()
{
	cout << "Stage2 of Project1" << endl;
	cout << "Input command lines below to utilize the DBMS" << endl;
	cout << "Notice: input files is recommended because it will be faster." << endl;
	cout << "To input a (.sql)file:	input [filename]" << endl;
	cout << "To input sql lines:	raw\n			[sql codes]\n			end [ctrl+Z]\n" << endl;
	cout << "To terminate the program:	exit" << endl;
	string command;
	char type[256] = {};
	char inputfile[256] = {};
	FILE *p;
	/*p = freopen("tables.txt", "r", stdin);
	lex();
	fclose(p);
	p = freopen("tweets.sql", "r", stdin);
	lex();
	fclose(p);
	p = freopen("user1.sql", "r", stdin);
	lex();
	fclose(p);*/
	//p = freopen("select2.sql", "r", stdin);
	//lex();
	//fclose(p);
	freopen("con", "r", stdin);
	//dbms.tables[0].print_table();
	//dbms.tables[1].print_table();
	while (true)
	{
		
		//do { getline(cin, command); } while (command.size() == 0 || command[0] == '\n' || command[0] == '\r');
		getline(cin, command);
		//cout << command.length() << endl;
		//cout << command.c_str() << endl;
		sscanf(command.c_str(), "%s", type);
		string types = type;
		if (types.compare("input") == 0)
		{
			sscanf(command.c_str(), "%s %s", type, inputfile);
			strcpy(CurFile, inputfile);
			filenum += 1;
			//streambuf *cinbuf = cin.rdbuf();
			if (!(p = freopen(inputfile, "r", stdin)))
			{
				cout << "Cannot open current file. " << endl;
				freopen("con", "r", stdin);
				memset(type, 0, sizeof(type));
				memset(inputfile, 0, sizeof(inputfile));
				continue;
			}
			lex();
			fclose(p);
			//cin.clear();
			//cin.rdbuf(cinbuf);
			freopen("con", "r", stdin);
			memset(type, 0, sizeof(type));
			memset(inputfile, 0, sizeof(inputfile));
		}
		else if (types.compare("exit") == 0)
		{
			//dbms.tables[0].print_table();
			//dbms.tables[1].print_table();
			return 0;
		}
		else if(types.compare("raw")==0)
		{
			//stringstream ss(command);
			//streambuf *cinbuf = cin.rdbuf();
			//cin.rdbuf(ss.rdbuf());
			lex();
			//cin.rdbuf(cinbuf);
		}
		else if (types.compare("save") == 0)
		{
			dbms.save_data();
		}
		else if (types.compare("load") == 0)
		{
			dbms.load_data();
		}
	}
}
