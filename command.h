#include "structure.h"
#define MAX_ID_LENGTH 256

typedef enum FUNC_MODE {
	SUM, COUNT, NOR
}MODE;

typedef enum CAL {
	GRE, LESS, EQU, NEQ
}OP;

typedef enum LOGIC_RELATION {
	_AND, _OR
}LOGIC;

class select_attr {
public:
	string attr_name;
	bool alias;
	string tablename;

	int table_index;
	int attr_index;
};

class Insert_Command {
public:
	string tablename;
	vector<string> attr_name;
	vector<Value> insert_values;
	Insert_Command() {
		attr_name.reserve(11);
		insert_values.reserve(11);
	}
	void Set_Name(string &s)
	{
		//if (s.find(" ")!=s.length()-1)
		//{
		//	printf("Parser error");
		//	exit(-1);
		//}
		tablename = s;
		return;
	}
	void Add_Attribute(string &s)
	{
		attr_name.push_back(s);
		return;
	}
	void Add_Value(string &s)
	{
		char type[20];
		TYPE t = _NULL;
		string v;
		sscanf(s.c_str(), "%s", type);
		if (strcmp(type, "int") == 0)
		{
			t = _INT;
		}
		if (strcmp(type, "str") == 0)
		{
			t = VARCHAR;
		}
		v = s.substr(4);
		Value temp(t, v);
		insert_values.push_back(temp);
		return;
	}
};

class Create_Command {
public:
	string tablename;
	vector<Attribute> attributes;
	Create_Command() {
		attributes.reserve(11);
	}
	void Set_Name(string &s)
	{
		if (s.find(" ") != s.length() - 1)
		{
			printf("Parser error\n");
			return;
		}
		char temp[MAX_ID_LENGTH];
		sscanf(s.c_str(),"%s", temp);
		tablename = temp;
		return;
	}
	void Add_Attribute(string &s)
	{
		string names;
		TYPE t=_NULL;
		int l = 0;
		bool pri = false;
		bool n = false;
		char namec[MAX_ID_LENGTH];
		char type[20];
		sscanf(s.c_str(), "%s %s", namec, type);
		//cout << type << endl;
		names = namec;
		if (strcmp(type, "int") == 0)t = _INT;
		if (strstr(type, "varchar")) {
			t = VARCHAR;
			sscanf(type, "varchar(%d)", &l);
		}
		if (strstr(s.c_str(), "PRIMARY KEY"))
		{
			pri = n = true;
		}
		if (t == _NULL)
		{
			printf("Invalid type!");
			return;
		}
		Attribute temp(names, t, l, pri, n);
		attributes.push_back(temp);
		return;
	}
};

class Select_Command {
public:
	vector<string> tablename;
	bool is_all;
	// is_all 就是 * ，代表选取所有的attr
	MODE func_mode;
	vector<select_attr> attr;

	Condition condt;

	Select_Command() {
		tablename.reserve(2);
		attr.reserve(11);
	}
};

class Element {
public:
	bool is_imme;
	// imme 有可能是数字，也有可能是字符串, 用string来表示，如果是整数类型，换成数字来做
	string imme;
	// 不是立即数则是一个Attr， 而且这个Attr不一定出现在select后面，要从表中找
	select_attr attr;

	Element() {};
};

class Expression {
public:
	Element elem1;
	Element elem2;
	OP op;
	Expression() {};
};

class Condition {
public:
	int exp_num;
	Expression exp1;
	Expression exp2;
	LOGIC logic;

	Condition() {};
};