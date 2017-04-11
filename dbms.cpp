#pragma once
#include "dbms.h"


DBMS::DBMS()
{
	tables.reserve(10);
}

int DBMS::get_table_index(string str)
{
	int i = 0; bool found = false;
	for (i = 0; i < tables.size(); i++) {
		if (str == tables[i].table_name) {
			found = true;
			break;
		}
	}
	if (!found) return -1;
	return i;
}

int DBMS::get_attr_index(string str, int index)
{
	int i = 0; bool found = false;
	for (i = 0; i < tables[index].attr.size(); i++) {
		if (str == tables[index].attr[i].attr_name) {
			found = true;
			break;
		}
	}
	if (!found) return -1;
	return i;
}



void DBMS::Insert(Insert_Command insert)
{
	Tuple in_tuple;
	bool found = false; int k = 0;
	for (k = 0; k < tables.size(); k++) {
		if (tables[k].table_name == insert.tablename) {
			found = true;
			break;
		}
	}
	if (!found) {
		cout << "Cannot find table by name: " << insert.tablename << endl;
		return;
	}

	//cout << "Current tuple num: " << t.tuples.size() << endl;
	// check the insertion 
	if (tables[k].attr.size() != insert.insert_values.size()) {
		cout << "Insertion attributes size mismatch." << endl;
		return;
	}
	int size = tables[k].attr.size();
	if (insert.attr_name.size() == 0) { // default sequence
		for (int i = 0; i < size; i++) {
			if (insert.insert_values[i].type == _NULL) {
				if (tables[k].attr[i].is_primary == true || tables[k].attr[i].not_null == true) {
					cout << "Attribute " << tables[k].attr[i].attr_name << " cannot be NULL." << endl;
					return;
				}
			}
			else {
				if (insert.insert_values[i].type != tables[k].attr[i].type) {
					cout << "Attribute: " << tables[k].attr[i].attr_name << " type " << tables[k].attr[i].type << " mismatch with " << insert.insert_values[i].type << endl;
					return;
				}
				if (insert.insert_values[i].type == VARCHAR) {
					if (insert.insert_values[i].val.length() > tables[k].attr[i].len) {
						cout << "Varchar length overflow." << endl;
						return;
					}
				}
			}
			if (tables[k].attr[i].is_primary == true) {
				string pri_name = insert.insert_values[i].val;
				for (Tuple temp : tables[k].tuples) {
					if (temp.values[i].val == pri_name) {
						cout << "Primary key duplication." << endl;
						return;
					}
				}
			}
		}
		in_tuple = Tuple(insert.insert_values);
		tables[k].tuples.push_back(in_tuple);
	}
	else { // specified order of attributes
		for (int i = 0; i < size; i++) {	//check each value in the order of its name
			bool found_attr = false; int j = 0;
			for (j = 0; j < size; j++) {
				if (insert.attr_name[i] == tables[k].attr[j].attr_name) {
					found_attr = true;
					break;
				}

			}
			if (found_attr == false) {
				cout << "Invalid attribute: " << insert.attr_name[i] << endl;
				return;
			}
			if (insert.insert_values[i].type == _NULL) {
				if (tables[k].attr[j].is_primary == true || tables[k].attr[j].not_null == true) {
					cout << "Attribute " << tables[k].attr[j].attr_name << " cannot be NULL." << endl;
					return;
				}
			}
			else {
				if (insert.insert_values[i].type != tables[k].attr[j].type) {
					cout << "Attribute: " << tables[k].attr[j].attr_name << " type " << tables[k].attr[j].type << " mismatch with " << insert.insert_values[i].type << endl;
					return;
				}
				if (insert.insert_values[i].type == VARCHAR) {
					if (insert.insert_values[i].val.length() > tables[k].attr[j].len) {
						cout << "Varchar length overflow." << endl;
						return;
					}
				}
			}
			if (tables[k].attr[j].is_primary == true) {
				string pri_name = insert.insert_values[i].val;
				for (Tuple temp : tables[k].tuples) {
					if (temp.values[j].val == pri_name) {
						cout << "Primary key duplication." << endl;
						return;
					}
				}
			}
			// for now, value is checked
			string str; Value empty(_NULL, str);
			for (int n = 0; n < size; n++) in_tuple.values.push_back(empty);
			in_tuple.values[j] = insert.insert_values[i];

		}
		tables[k].tuples.push_back(in_tuple);
	}
	//cout << "Current tuple num: " << t.tuples.size() << endl;
	//t.print_table();
}

void DBMS::Create(Create_Command create, Table &t)
{
	// we assume that table name is always leagal
	t.table_name = create.tablename;
	t.tuples.clear();
	t.attr = create.attributes;
	tables.push_back(t);
	// add table t into dbms
	//t.print_table();
}

void DBMS::Select(Select_Command select)
{
	/*cout << "TABLE:\n";
	for (int i = 0; i < select.tablename.size(); i++)
	{
		cout << select.tablename[i] << endl;
	}
	printf("%d %d", select.is_all, select.func_mode);
	cout << "ATTR:\n";
	for (int i = 0; i < select.attr.size(); i++)
	{
		cout << select.attr[i].attr_name << " " << select.attr[i].alias << " " << select.attr[i].tablename << endl;
	}
	cout << "CONDT:\n";
	cout << select.condt.exp_num << endl;
	cout << select.condt.logic << endl;
	cout << select.condt.exp1.op << endl;
	cout << select.condt.exp2.op << endl;
	cout << select.condt.exp1.elem1.attr.attr_name << " " << select.condt.exp1.elem1.attr.alias << endl;
	cout << select.condt.exp1.elem2.attr.attr_name << " " << select.condt.exp1.elem2.attr.alias << endl;
	cout << select.condt.exp2.elem1.attr.attr_name << " " << select.condt.exp2.elem1.attr.alias << endl;
	//test code1
	/*if (select.condt.exp_num == 0)cout << "No Condition" << endl;
	else
	{
	cout << (select.condt.exp1.elem1.is_imme ? (select.condt.exp1.elem1.imme) : (select.condt.exp1.elem1.attr.attr_name));
	cout << select.condt.exp1.op;
	cout << (select.condt.exp1.elem2.is_imme ? (select.condt.exp1.elem2.imme) : (select.condt.exp1.elem2.attr.attr_name));
	if (select.condt.exp_num == 2)
	{
	cout << (select.condt.exp2.elem1.is_imme ? (select.condt.exp2.elem1.imme) : (select.condt.exp2.elem1.attr.attr_name));
	cout << select.condt.exp2.op;
	cout << (select.condt.exp2.elem2.is_imme ? (select.condt.exp2.elem2.imme) : (select.condt.exp2.elem2.attr.attr_name));
	}
	}*/
	cout << endl;
	//test code2
	int index1 = -1, index2 = -1;
	index1 = get_table_index(select.tablename[0]);
	if (index1 < 0) { cout << "Cannot find table by name : " << select.tablename[0]; return; }
	vector<Tuple> valid_tuple; valid_tuple.reserve(1000);
	if (select.tablename.size() == 1) {
		int attr_size = select.attr.size();
		for (int i = 0; i < attr_size; i++) {
			select.attr[i].attr_index = get_attr_index(select.attr[i].attr_name, index1);
			select.attr[i].table_index = index1;
		}
		// construct valid_tuple
		if (select.condt.exp_num == 0)	valid_tuple = tables[index1].tuples;
		else {
			if (!select.condt.exp1.elem1.is_imme) { //第一个元素不是立即数,得到attindex
				select.condt.exp1.elem1.attr.attr_index = get_attr_index(select.condt.exp1.elem1.attr.attr_name, index1);
			}
			if (!select.condt.exp2.elem2.is_imme) {//第二个元素不是立即数,得到attindex
				select.condt.exp1.elem2.attr.attr_index = get_attr_index(select.condt.exp1.elem2.attr.attr_name, index1);
			}
			if (select.condt.exp_num == 2) {
				if (!select.condt.exp2.elem1.is_imme) { //第一个元素不是立即数,得到attrindex
					select.condt.exp2.elem1.attr.attr_index = get_attr_index(select.condt.exp2.elem1.attr.attr_name, index1);
				}
				if (!select.condt.exp2.elem2.is_imme) { //第二个元素不是立即数，得到attrindex
					select.condt.exp2.elem2.attr.attr_index = get_attr_index(select.condt.exp2.elem2.attr.attr_name, index1);
				}
			}
			for (Tuple temp : tables[index1].tuples) {
				bool check1 = false;
				// 每个表达式的第一个元素应该为attr， 第二个元素才可以是立即数
				if (!select.condt.exp1.elem2.is_imme) { //第二个元素不是立即数,得到attindex，开始比较
					int elem2_index = select.condt.exp1.elem2.attr.attr_index;
					int elem1_index = select.condt.exp1.elem1.attr.attr_index;
					//检查两个attr的type是否一致
					if (temp.values[elem1_index].type != temp.values[elem2_index].type) {
						cout << "Type mismatch, cannot compare.\n";
						return;
					}
					if (temp.values[elem2_index].type == _INT) {
						int elem1_val, elem2_val;
						elem1_val = atoi(temp.values[elem1_index].val.c_str());
						elem2_val = atoi(temp.values[elem2_index].val.c_str());
						switch (select.condt.exp1.op)
						{
						case GRE: {
							if (elem1_val > elem2_val) check1 = true;
							else check1 = false;
							break;
						}
						case LESS: {
							if (elem1_val < elem2_val) check1 = true;
							else check1 = false;
							break;
						}
						case EQU: {
							if (elem1_val == elem2_val) check1 = true;
							else check1 = false;
							break;
						}
						case NEQ: {
							if (elem1_val != elem2_val) check1 = true;
							else check1 = false;
							break;
						}
						}
					}
					else {
						string elem1_val = temp.values[select.condt.exp1.elem1.attr.attr_index].val;
						string elem2_val = temp.values[elem2_index].val;
						switch (select.condt.exp1.op)
						{
						case EQU: {
							if (elem1_val == elem2_val) check1 = true;
							else check1 = false;
							break;
						}
						case NEQ: {
							if (elem1_val != elem2_val) check1 = true;
							else check1 = false;
							break;
						}
						}
					}
				}
				else { // 第二个元素是立即数
					int elem1_index = select.condt.exp1.elem1.attr.attr_index;
					//检查立即数类型和attr类型是否一致
					if (temp.values[elem1_index].type != select.condt.exp1.elem2.imme_type) {
						cout << "Type mismatch, cannot compare.\n";
						return;
					}
					if (temp.values[elem1_index].type == _INT) {
						int elem1_val = atoi(temp.values[elem1_index].val.c_str());
						int elem2_val = atoi(select.condt.exp1.elem2.imme.c_str());
						switch (select.condt.exp1.op)
						{
						case GRE: {
							if (elem1_val > elem2_val) check1 = true;
							else check1 = false;
							break;
						}
						case LESS: {
							if (elem1_val < elem2_val) check1 = true;
							else check1 = false;
							break;
						}
						case EQU: {
							if (elem1_val == elem2_val) check1 = true;
							else check1 = false;
							break;
						}
						case NEQ: {
							if (elem1_val != elem2_val) check1 = true;
							else check1 = false;
							break;
						}
						}
					}
					else {
						string elem1_val = temp.values[elem1_index].val;
						string elem2_val = select.condt.exp1.elem2.imme;
						switch (select.condt.exp1.op)
						{
						case EQU: {
							if (elem1_val == elem2_val) check1 = true;
							else check1 = false;
							break;
						}
						case NEQ: {
							if (elem1_val != elem2_val) check1 = true;
							else check1 = false;
							break;
						}
						}
					}
				}
				if (select.condt.exp_num == 1) {
					if (check1) valid_tuple.push_back(temp);
				}
				else if (select.condt.exp_num == 2) {
					bool check2 = false;
					if (!select.condt.exp2.elem2.is_imme) { //第二个元素不是立即数，开始比较
						int elem1_index = select.condt.exp2.elem1.attr.attr_index;
						int elem2_index = select.condt.exp2.elem2.attr.attr_index;
						//检查两个attr的type是否一致
						if (temp.values[elem1_index].type != temp.values[elem2_index].type) {
							cout << "Type mismatch, cannot compare.\n";
							return;
						}
						if (temp.values[elem2_index].type == _INT) {
							int elem1_val, elem2_val;
							elem1_val = atoi(temp.values[elem1_index].val.c_str());
							elem2_val = atoi(temp.values[elem2_index].val.c_str());
							switch (select.condt.exp2.op)
							{
							case GRE: {
								if (elem1_val > elem2_val) check2 = true;
								else check2 = false;
								break;
							}
							case LESS: {
								if (elem1_val < elem2_val) check2 = true;
								else check2 = false;
								break;
							}
							case EQU: {
								if (elem1_val == elem2_val) check2 = true;
								else check2 = false;
								break;
							}
							case NEQ: {
								if (elem1_val != elem2_val) check2 = true;
								else check2 = false;
								break;
							}
							}
						}
						else {
							string elem1_val = temp.values[select.condt.exp2.elem1.attr.attr_index].val;
							string elem2_val = temp.values[elem2_index].val;
							switch (select.condt.exp2.op)
							{
							case EQU: {
								if (elem1_val == elem2_val) check2 = true;
								else check2 = false;
								break;
							}
							case NEQ: {
								if (elem1_val != elem2_val) check2 = true;
								else check2 = false;
								break;
							}
							}
						}
					}
					else { // 第二个元素是立即数
						int elem1_index = select.condt.exp2.elem1.attr.attr_index;
						if (temp.values[elem1_index].type != select.condt.exp2.elem2.imme_type) {
							cout << "Type mismatch, cannot compare.\n";
							return;
						}
						if (temp.values[elem1_index].type == _INT) {
							int elem1_val = atoi(temp.values[elem1_index].val.c_str());
							int elem2_val = atoi(select.condt.exp2.elem2.imme.c_str());
							switch (select.condt.exp2.op)
							{
							case GRE: {
								if (elem1_val > elem2_val) check2 = true;
								else check2 = false;
								break;
							}
							case LESS: {
								if (elem1_val < elem2_val) check2 = true;
								else check2 = false;
								break;
							}
							case EQU: {
								if (elem1_val == elem2_val) check2 = true;
								else check2 = false;
								break;
							}
							case NEQ: {
								if (elem1_val != elem2_val) check2 = true;
								else check2 = false;
								break;
							}
							}
						}
						else {
							string elem1_val = temp.values[elem1_index].val;
							string elem2_val = select.condt.exp2.elem2.imme;
							switch (select.condt.exp2.op)
							{
							case EQU: {
								if (elem1_val == elem2_val) check2 = true;
								else check2 = false;
								break;
							}
							case NEQ: {
								if (elem1_val != elem2_val) check2 = true;
								else check2 = false;
								break;
							}
							}
						}
					}
					switch (select.condt.logic)
					{
					case _AND: {
						if (check1 && check2) valid_tuple.push_back(temp);
						break;
					}
					case _OR: {
						if (check1 || check2) valid_tuple.push_back(temp);
						break;
					}
					}
				}
			}
		}

		// 只有一个表的时候，把所有的attr都放到valid_tuple中了
		int tuple_num = valid_tuple.size();
		if (select.func_mode == NOR) {
			if (select.is_all) {
				int all_attr = tables[index1].attr.size();
				for (int i = 0; i < all_attr; i++)
					cout << setw(25) << tables[index1].attr[i].attr_name;
				cout << endl;
				for (int i = 0; i < tuple_num; i++) {
					for (int j = 0; j < all_attr; j++) {
						cout << setw(25) << valid_tuple[i].values[j].val;
					}
					cout << endl;
				}
			}
			else {
				for (int i = 0; i < attr_size; i++)
					cout << setw(25) << select.attr[i].attr_name;
				cout << endl;
				for (int i = 0; i < tuple_num; i++) {
					for (int j = 0; j < attr_size; j++) {
						cout << setw(25) << valid_tuple[i].values[select.attr[j].attr_index].val;
					}
					cout << endl;
				}
			}
		}
		else if (select.func_mode == COUNT) {
			//still need to check NON_NULL
			cout << valid_tuple.size() << endl;
		}
		else if (select.func_mode == SUM) {
			if (attr_size != 1) { cout << "Attributes to sum > 1.\n"; return; }
			int count_index = select.attr[0].attr_index;
			if (valid_tuple[0].values[count_index].type != _INT) { cout << "NON_INT type cannot SUM.\n" << endl; return; }
			int sum = 0;
			for (int i = 0; i < tuple_num; i++) {
				sum += atoi(valid_tuple[i].values[count_index].val.c_str());
			}
			cout << sum << endl;
		}
	}
	else if (select.tablename.size() == 2) {
		index2 = get_table_index(select.tablename[1]);
		if (index2 < 0) { cout << "Cannot find table by name : " << select.tablename[1]; return; }
		// COUNT(*)出现的时候，attr从where中取得，在构建类的时候实现比较方便！！ is_all 也不用置true
		// b.*这样的东西出现时，is_all 不用置为true，在构建类的时候完成attr就行
		// 也就是说，两个表时不检查is_all, 这个设计有点失败
		int attr_size = select.attr.size();
		// 构建好每个attr的属性
		for (int i = 0; i < attr_size; i++) {
			//没有别名的情况
			if (!select.attr[i].alias) {
				// 假设在table1中
				int index = get_attr_index(select.attr[i].attr_name, index1);
				int index_2 = get_attr_index(select.attr[i].attr_name, index2);
				// 如果在两个表中都找到了这个attr
				if (index >= 0 && index_2 >= 0) {
					cout << "Ambigious attribute.\n";
					return;
				}
				if (index < 0) {
					select.attr[i].table_index = index2; select.attr[i].attr_index = index_2;
				}
				else {
					select.attr[i].table_index = index1; select.attr[i].attr_index = index;
				}
			}
			//有别名的情况
			else {
				select.attr[i].table_index = get_table_index(select.attr[i].tablename);
				select.attr[i].attr_index = get_attr_index(select.attr[i].attr_name, select.attr[i].table_index);
			}
		}
		// 构建condition中的attr的属性
		if (select.condt.exp_num) {
			//构建第一个表达式的attr的属性
			if (!select.condt.exp1.elem1.is_imme) {
				if (!select.condt.exp1.elem1.attr.alias) {
					int index = get_attr_index(select.condt.exp1.elem1.attr.attr_name, index1);
					int index_2 = get_attr_index(select.condt.exp1.elem1.attr.attr_name, index2);
					if (index >= 0 && index_2 >= 0) {
						cout << "Ambigious attribute.\n";
						return;
					}
					if (index < 0) {
						select.condt.exp1.elem1.attr.table_index = index2; select.condt.exp1.elem1.attr.attr_index = index_2;
					}
					else {
						select.condt.exp1.elem1.attr.table_index = index1; select.condt.exp1.elem1.attr.attr_index = index;
					}
				}
				else {
					select.condt.exp1.elem1.attr.table_index = get_table_index(select.condt.exp1.elem1.attr.tablename);
					select.condt.exp1.elem1.attr.attr_index = get_attr_index(select.condt.exp1.elem1.attr.attr_name, select.condt.exp1.elem1.attr.table_index);
				}
			}
			//第一个表达式的第二个元素
			if (!select.condt.exp1.elem2.is_imme) {
				if (!select.condt.exp1.elem2.attr.alias) {
					int index = get_attr_index(select.condt.exp1.elem2.attr.attr_name, index1);
					int index_2 = get_attr_index(select.condt.exp1.elem2.attr.attr_name, index2);
					if (index >= 0 && index_2 >= 0) {
						cout << "Ambigious attribute.\n";
						return;
					}
					if (index < 0) {
						select.condt.exp1.elem2.attr.table_index = index2; select.condt.exp1.elem2.attr.attr_index = index_2;
					}
					else {
						select.condt.exp1.elem2.attr.table_index = index1; select.condt.exp1.elem2.attr.attr_index = index;
					}
				}
				else {
					select.condt.exp1.elem2.attr.table_index = get_table_index(select.condt.exp1.elem2.attr.tablename);
					select.condt.exp1.elem2.attr.attr_index = get_attr_index(select.condt.exp1.elem2.attr.attr_name, select.condt.exp1.elem2.attr.table_index);
				}
			}
			if (select.condt.exp_num == 2) {
				//构建第二个表达式的attr的属性
				if (!select.condt.exp2.elem1.is_imme) {
					if (!select.condt.exp2.elem1.attr.alias) {
						int index = get_attr_index(select.condt.exp2.elem1.attr.attr_name, index1);
						int index_2 = get_attr_index(select.condt.exp2.elem1.attr.attr_name, index2);
						if (index >= 0 && index_2 >= 0) {
							cout << "Ambigious attribute.\n";
							return;
						}
						if (index < 0) {
							select.condt.exp2.elem1.attr.table_index = index2; select.condt.exp2.elem1.attr.attr_index = index_2;
						}
						else {
							select.condt.exp2.elem1.attr.table_index = index1; select.condt.exp2.elem1.attr.attr_index = index;
						}
					}
					else {
						select.condt.exp2.elem1.attr.table_index = get_table_index(select.condt.exp2.elem1.attr.tablename);
						select.condt.exp2.elem1.attr.attr_index = get_attr_index(select.condt.exp2.elem1.attr.attr_name, select.condt.exp2.elem1.attr.table_index);
					}
				}
				//第一个表达式的第二个元素
				if (!select.condt.exp2.elem2.is_imme) {
					if (!select.condt.exp2.elem2.attr.alias) {
						int index = get_attr_index(select.condt.exp2.elem2.attr.attr_name, index1);
						int index_2 = get_attr_index(select.condt.exp2.elem2.attr.attr_name, index2);
						if (index >= 0 && index_2 >= 0) {
							cout << "Ambigious attribute.\n";
							return;
						}
						if (index < 0) {
							select.condt.exp2.elem2.attr.table_index = index2; select.condt.exp2.elem2.attr.attr_index = index_2;
						}
						else {
							select.condt.exp2.elem2.attr.table_index = index1; select.condt.exp2.elem2.attr.attr_index = index;
						}
					}
					else {
						select.condt.exp2.elem2.attr.table_index = get_table_index(select.condt.exp2.elem2.attr.tablename);
						select.condt.exp2.elem2.attr.attr_index = get_attr_index(select.condt.exp2.elem2.attr.attr_name, select.condt.exp2.elem2.attr.table_index);
					}
				}
			}
		}
		int t1, t2;
		int table1_size = tables[index1].tuples.size();
		int table2_size = tables[index2].tuples.size();
		for (t1 = 0; t1 < table1_size; t1++) {
			for (t2 = 0; t2 < table2_size; t2++) {
				if (select.condt.exp_num == 0) { // 将temp1， temp2中需要的attr放入valid_tuple.values[k]
					Tuple t;
					for (int i = 0; i < attr_size; i++) {
						if (select.attr[i].table_index == index1)
							t.values.push_back(tables[index1].tuples[t1].values[select.attr[i].attr_index]);
						else
							t.values.push_back(tables[index2].tuples[t2].values[select.attr[i].attr_index]);
					}
					valid_tuple.push_back(t);
				}
				else {
					bool check1 = false;
					if (!select.condt.exp1.elem2.is_imme) { //第二个元素不是立即数,开始比较
						int elem1_index = select.condt.exp1.elem1.attr.attr_index;
						int elem2_index = select.condt.exp1.elem2.attr.attr_index;
						int elem1_table = select.condt.exp1.elem1.attr.table_index;
						int elem2_table = select.condt.exp1.elem2.attr.table_index;
						if (tables[elem1_table].attr[elem1_index].type != tables[elem2_table].attr[elem2_index].type) {
							cout << "Type mismatch, cannot compare.\n";
							return;
						}
						if (tables[elem1_table].attr[elem1_index].type == _INT) {
							int elem1_val, elem2_val;
							if(elem1_table == index1) 
								elem1_val = atoi(tables[elem1_table].tuples[t1].values[elem1_index].val.c_str());
							else
								elem1_val = atoi(tables[elem1_table].tuples[t2].values[elem1_index].val.c_str());
							if(elem2_table == index1)
								elem2_val = atoi(tables[elem2_table].tuples[t1].values[elem2_index].val.c_str());
							else 
								elem2_val = atoi(tables[elem2_table].tuples[t2].values[elem2_index].val.c_str());
							switch (select.condt.exp1.op)
							{
							case GRE: {
								if (elem1_val > elem2_val) check1 = true;
								else check1 = false;
								break;
							}
							case LESS: {
								if (elem1_val < elem2_val) check1 = true;
								else check1 = false;
								break;
							}
							case EQU: {
								if (elem1_val == elem2_val) check1 = true;
								else check1 = false;
								break;
							}
							case NEQ: {
								if (elem1_val != elem2_val) check1 = true;
								else check1 = false;
								break;
							}
							}
						}
						else {
							string elem1_val, elem2_val;
							if(elem1_table == index1)
								elem1_val = tables[elem1_table].tuples[t1].values[elem1_index].val;
							else
								elem1_val = tables[elem1_table].tuples[t2].values[elem1_index].val;
							if (elem2_table == index1)
								elem2_val = tables[elem2_table].tuples[t1].values[elem2_index].val;
							else
								elem2_val = tables[elem2_table].tuples[t2].values[elem2_index].val;
							switch (select.condt.exp1.op)
							{
							case EQU: {
								if (elem1_val == elem2_val) check1 = true;
								else check1 = false;
								break;
							}
							case NEQ: {
								if (elem1_val != elem2_val) check1 = true;
								else check1 = false;
								break;
							}
							}
						}
					}
					else { // 第二个元素是立即数，开始比较
						int elem1_index = select.condt.exp1.elem1.attr.attr_index;
						int elem1_table = select.condt.exp1.elem1.attr.table_index;
						if (tables[elem1_table].attr[elem1_index].type != select.condt.exp1.elem2.imme_type) {
							cout << "Type mismatch, cannot compare.\n";
							return;
						}
						if (tables[elem1_table].attr[elem1_index].type == _INT) {
							int elem1_val;
							if(elem1_table == index1)
								elem1_val = atoi(tables[elem1_table].tuples[t1].values[elem1_index].val.c_str());
							else
								elem1_val = atoi(tables[elem1_table].tuples[t2].values[elem1_index].val.c_str());
							int elem2_val = atoi(select.condt.exp1.elem2.imme.c_str());
							switch (select.condt.exp1.op)
							{
							case GRE: {
								if (elem1_val > elem2_val) check1 = true;
								else check1 = false;
								break;
							}
							case LESS: {
								if (elem1_val < elem2_val) check1 = true;
								else check1 = false;
								break;
							}
							case EQU: {
								if (elem1_val == elem2_val) check1 = true;
								else check1 = false;
								break;
							}
							case NEQ: {
								if (elem1_val != elem2_val) check1 = true;
								else check1 = false;
								break;
							}
							}
						}
						else {
							string elem1_val;
							if(elem1_table == index1)
								elem1_val = tables[elem1_table].tuples[t1].values[elem1_index].val;
							else
								elem1_val = tables[elem1_table].tuples[t2].values[elem1_index].val;
							string elem2_val = select.condt.exp1.elem2.imme;
							switch (select.condt.exp1.op)
							{
							case EQU: {
								if (elem1_val == elem2_val) check1 = true;
								else check1 = false;
								break;
							}
							case NEQ: {
								if (elem1_val != elem2_val) check1 = true;
								else check1 = false;
								break;
							}
							}
						}
					}
					if (select.condt.exp_num == 1) {
						if (check1) { // 加入需要的attr组成tuple，加入valid_tuple中
							Tuple t;
							for (int i = 0; i < attr_size; i++) {
								if (select.attr[i].table_index == index1)
									t.values.push_back(tables[index1].tuples[t1].values[select.attr[i].attr_index]);
								else
									t.values.push_back(tables[index2].tuples[t2].values[select.attr[i].attr_index]);
							}
							valid_tuple.push_back(t);
						}
					}
					else if (select.condt.exp_num == 2) {
						bool check2 = false; bool is_good = false;
						if (!select.condt.exp2.elem2.is_imme) { //第二个元素不是立即数,开始比较
							int elem1_index = select.condt.exp2.elem1.attr.attr_index;
							int elem2_index = select.condt.exp2.elem2.attr.attr_index;
							int elem1_table = select.condt.exp2.elem1.attr.table_index;
							int elem2_table = select.condt.exp2.elem2.attr.table_index;
							if (tables[elem1_table].attr[elem1_index].type != tables[elem2_table].attr[elem2_index].type) {
								cout << "Type mismatch, cannot compare.\n";
								return;
							}
							if (tables[elem1_table].attr[elem1_index].type == _INT) {
								int elem1_val, elem2_val;
								if (elem1_table == index1)
									elem1_val = atoi(tables[elem1_table].tuples[t1].values[elem1_index].val.c_str());
								else
									elem1_val = atoi(tables[elem1_table].tuples[t2].values[elem1_index].val.c_str());
								if (elem2_table == index1)
									elem2_val = atoi(tables[elem2_table].tuples[t1].values[elem2_index].val.c_str());
								else
									elem2_val = atoi(tables[elem2_table].tuples[t2].values[elem2_index].val.c_str());
								switch (select.condt.exp2.op)
								{
								case GRE: {
									if (elem1_val > elem2_val) check2 = true;
									else check2 = false;
									break;
								}
								case LESS: {
									if (elem1_val < elem2_val) check2 = true;
									else check2 = false;
									break;
								}
								case EQU: {
									if (elem1_val == elem2_val) check2 = true;
									else check2 = false;
									break;
								}
								case NEQ: {
									if (elem1_val != elem2_val) check2 = true;
									else check2 = false;
									break;
								}
								}
							}
							else {
								string elem1_val, elem2_val;
								if (elem1_table == index1)
									elem1_val = tables[elem1_table].tuples[t1].values[elem1_index].val;
								else
									elem1_val = tables[elem1_table].tuples[t2].values[elem1_index].val;
								if (elem2_table == index1)
									elem2_val = tables[elem2_table].tuples[t1].values[elem2_index].val;
								else
									elem2_val = tables[elem2_table].tuples[t2].values[elem2_index].val;
								switch (select.condt.exp2.op)
								{
								case EQU: {
									if (elem1_val == elem2_val) check2 = true;
									else check2 = false;
									break;
								}
								case NEQ: {
									if (elem1_val != elem2_val) check2 = true;
									else check2 = false;
									break;
								}
								}
							}
						}
						else { // 第二个元素是立即数，开始比较
							int elem1_index = select.condt.exp2.elem1.attr.attr_index;
							int elem1_table = select.condt.exp2.elem1.attr.table_index;
							if (tables[elem1_table].attr[elem1_index].type != select.condt.exp2.elem2.imme_type) {
								cout << "Type mismatch, cannot compare.\n";
								return;
							}
							if (tables[elem1_table].attr[elem1_index].type == _INT) {
								int elem1_val;
								if(elem1_table == index1)
									elem1_val = atoi(tables[elem1_table].tuples[t1].values[elem1_index].val.c_str());
								else
									elem1_val = atoi(tables[elem1_table].tuples[t2].values[elem1_index].val.c_str());
								int elem2_val = atoi(select.condt.exp2.elem2.imme.c_str());
								switch (select.condt.exp2.op)
								{
								case GRE: {
									if (elem1_val > elem2_val) check2 = true;
									else check2 = false;
									break;
								}
								case LESS: {
									if (elem1_val < elem2_val) check2 = true;
									else check2 = false;
									break;
								}
								case EQU: {
									if (elem1_val == elem2_val) check2 = true;
									else check2 = false;
									break;
								}
								case NEQ: {
									if (elem1_val != elem2_val) check2 = true;
									else check2 = false;
									break;
								}
								}
							}
							else {
								string elem1_val;
								if (elem1_table == index1)
									elem1_val = tables[elem1_table].tuples[t1].values[elem1_index].val;
								else
									elem1_val = tables[elem1_table].tuples[t2].values[elem1_index].val;
								string elem2_val = select.condt.exp2.elem2.imme;
								switch (select.condt.exp2.op)
								{
								case EQU: {
									if (elem1_val == elem2_val) check2 = true;
									else check2 = false;
									break;
								}
								case NEQ: {
									if (elem1_val != elem2_val) check2 = true;
									else check2 = false;
									break;
								}
								}
							}
						}
						switch (select.condt.logic)
						{
						case _AND: {
							if (check1 && check2) is_good = true;
							break;
						}
						case _OR: {
							if (check1 || check2) is_good = true;
							break;
						}
						}
						if (is_good) {
							Tuple t;
							for (int i = 0; i < attr_size; i++) {
								if (select.attr[i].table_index == index1)
									t.values.push_back(tables[index1].tuples[t1].values[select.attr[i].attr_index]);
								else
									t.values.push_back(tables[index2].tuples[t2].values[select.attr[i].attr_index]);
							}
							valid_tuple.push_back(t);
						}
					}
				}
			}
		}
		int tuple_size = valid_tuple.size();
		if (select.func_mode == COUNT) {
			cout << tuple_size << endl;
		}
		else if (select.func_mode == SUM) {
			if (attr_size != 1) { cout << "Attributes to sum > 1.\n"; return; }
			if (valid_tuple[0].values[0].type != _INT) { cout << "NON_INT type cannot SUM.\n" << endl; return; }
			int sum = 0;
			for (int i = 0; i < tuple_size; i++) {
				sum += atoi(valid_tuple[i].values[0].val.c_str());
			}
			cout << sum << endl;
		}
		else if (select.func_mode == NOR) {
			for (int i = 0; i < attr_size; i++)
				cout << setw(25) << select.attr[i].attr_name;
			cout << endl;
			for (int i = 0; i < tuple_size; i++) {
				for (int j = 0; j < attr_size; j++)
					cout << setw(25) << valid_tuple[i].values[j].val;
				cout << endl;
			}
		}
	}
}




