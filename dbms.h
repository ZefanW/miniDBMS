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
};
