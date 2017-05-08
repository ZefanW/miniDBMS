#pragma once
#include "command.h"

class DBMS {
public:
	vector<Table> tables;

	//int get_table_by_name(string s, Table &t);

	DBMS();
	int get_table_index(string str);
	int get_attr_index(string str, int index);
	void Insert(Insert_Command insert);
	void Create(Create_Command create, Table &t);
	void Select(Select_Command select);
	void select_range( Expression exp, vector<int> &index);
	void select_equal(Expression exp, vector<int> &index);
	void save_data();
	void load_data();
	void create_index();
	//vector的交集，并集操作
	void intersect(vector<int>& exp1, vector<int> &exp2, vector<int> &index);
	void Union(vector<int> &exp1, vector<int>& exp2, vector<int>& index);
};
