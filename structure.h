
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <string>
#include <iomanip> 
using namespace std;

typedef enum VAR_TYPE {
	_INT, VARCHAR, _NULL
}TYPE;

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

	Attribute() {};
	Attribute(string name, TYPE t, int l, bool pri, bool n) {
		attr_name = name;
		type = t;
		len = l;
		is_primary = pri;
		not_null = n;
	}
};

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
};


