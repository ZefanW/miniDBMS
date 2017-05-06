#pragma once
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <string>
#include <iomanip> 
#include<map>
#include<unordered_map>
#include<algorithm>
using namespace std;

vector < unordered_map<string, int> > StringIndexList;
vector<multimap<int, int> > IntIndexList;
vector<vector<int> > HashTargetList;

typedef enum VAR_TYPE {
	_INT, VARCHAR, _NULL
}TYPE;

typedef enum FUNC_MODE {
	SUM, COUNT, NOR
}MODE;

typedef enum CALCULATOR {
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

class Value {
public:
	TYPE type;
	string val;

	Value() {};
	Value(TYPE t , string v) {
		type = t;
		val = v;
	}
};

class Tuple {
public:
	vector<Value> values;

	Tuple() {
		values.reserve(11);
	}

	Tuple(vector<Value> val) {
		values = val;
	}
};

class Attribute {
public:
	string attr_name;
	TYPE type;
	int len;
	bool is_primary;
	bool not_null;
	int indexpos;

	Attribute() {};
	Attribute(string name, TYPE t, int l, bool pri, bool n) {
		attr_name = name;
		type = t;
		len = l;
		is_primary = pri;
		not_null = n;
	}
};

void insert_index(Attribute& target, int index, Value& element)
{
	if (target.type == VARCHAR)
	{
		if (StringIndexList[target.indexpos].count(element.val))HashTargetList[StringIndexList[target.indexpos][element.val]].push_back(index);
		else
		{
			int vectorindex = HashTargetList.size();
			StringIndexList[target.indexpos].insert(make_pair(element.val, vectorindex));
			HashTargetList[vectorindex].push_back(index);
		}
	}
	else if (target.type == _INT)
	{
		int intform = atoi(element.val.c_str());
		IntIndexList[target.indexpos].insert(make_pair(intform, index));
	}
}

void select_from_index(Attribute& target, Value& Comparison, OP op, vector<int>& returnvalue)
{
	if (target.type == VARCHAR)
	{
		//Assuming op can only be EQUAL or NEQUAL
		int result;
		if (StringIndexList[target.indexpos].count(Comparison.val))
		{
			result= StringIndexList[target.indexpos][Comparison.val];
		}
		else result = -1;
		if (op == EQU)
		{
			if (result == -1)
			{
				returnvalue.resize(0);
				return;
			}
			else
			{
				returnvalue = HashTargetList[result];
				return;
			}
		}
		else if (op == NEQ)
		{
			for (unordered_map<string,int>::iterator it = StringIndexList[target.indexpos].begin(); it != StringIndexList[target.indexpos].end(); it++)
			{
				if (result != (*it).second)
				{
					int Hashindex = (*it).second;
					for (vector<int>::iterator it2 = HashTargetList[Hashindex].begin(); it2 != HashTargetList[Hashindex].end(); it2++)
					{
						returnvalue.push_back(*it2);
					}
				}
			}
			sort(returnvalue.begin(), returnvalue.end());
			return;
		}
	}
	else if (target.type == _INT)
	{
		int intform = atoi(Comparison.val.c_str());
		pair< multimap<int, int>::iterator, multimap<int, int>::iterator > indexrange;
		switch(op)
		{
			case GRE:
			{
				indexrange.first = IntIndexList[target.indexpos].upper_bound(intform);
				indexrange.second = IntIndexList[target.indexpos].end();
				for (multimap<int, int>::iterator it = indexrange.first; it != indexrange.second; it++)
				{
					returnvalue.push_back(it->second);
				}
				break;
			}
			case LESS:
			{
				indexrange.second = IntIndexList[target.indexpos].lower_bound(intform);
				indexrange.second--;
				indexrange.first = IntIndexList[target.indexpos].begin();
				for (multimap<int, int>::iterator it = indexrange.first; it != indexrange.second; it++)
				{
					returnvalue.push_back(it->second);
				}
				break;
			}
			case EQU:
			{
				indexrange = IntIndexList[target.indexpos].equal_range(intform);
				for (multimap<int, int>::iterator it = indexrange.first; it != indexrange.second; it++)
				{
					returnvalue.push_back(it->second);
				}
				break;
			}
			case NEQ:
			{
				for (multimap<int, int>::iterator it = indexrange.first; it != indexrange.second; it++)
				{
					if (it->first != intform)returnvalue.push_back(it->second);
				}
				break;
			}
			default:break;
		}
		sort(returnvalue.begin(), returnvalue.end());
	}
}

class Table {
public:
	string table_name;
	vector<Tuple> tuples;
	vector<Attribute> attr;
	Table();
	Table(string name, vector<Attribute> a, vector<Tuple> t) {
		table_name = name;
		attr = a;
		tuples = t;
	}
	void print_table() {
		cout << "----------------- Print Table: " << table_name << " -------------------" << endl;
		for (int i = 0;i<attr.size();i++) {
			if (attr[i].is_primary == true) cout << "Primary: ";
			cout << attr[i].attr_name << endl;
		}
		if (!tuples.size()) { cout << "----------------- Table is empty.       -------------------" << endl; return; }
		for (Tuple temp_t : tuples) {
			for (Value temp_v : temp_t.values) {
				//cout << temp_v.val << "    ";
				cout << setw(15) << temp_v.val;
			}
			cout << endl;
		}
		cout << "----------------- End of Table: " << table_name << " -------------------" << endl << endl;
	}
	void write_data();
	void load_data();
};

class Element {
public:
	bool is_imme;
	// imme 有可能是数字，也有可能是字符串, 用string来表示，如果是整数类型，换成数字来做
	string imme;
	// 不是立即数则是一个Attr， 而且这个Attr不一定出现在select后面，要从表中找
	select_attr attr;
	TYPE imme_type;
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

