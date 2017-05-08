#pragma once
#include "structure.h"

Table::Table()
{
	tuples.reserve(10000);
	attr.reserve(11);
}

void Table::write_data()
{
	string file_name = table_name + ".txt";
	FILE *p = fopen(file_name.c_str(), "w");
	if (!p) {
		cout << "Cannot open file.(writing)\n";
		return;
	}
	int tuple_size = tuples.size();
	int attr_size = attr.size();
	int i, j;
	fprintf(p, "%d\n", tuple_size);
	fprintf(p, "%d\n", attr_size);
	for (i = 0; i < tuple_size; i++)
	{
		for (j = 0; j < attr_size; j++)
		{
			fprintf(p, "%s\n", tuples[i].values[j].val.c_str());
			//fwrite(tuples[i].values[j].val.c_str(), sizeof(tuples[i].values[j].val.c_str()),1, p);
			//fprintf(p, "\n");
		}
	}
	fclose(p);
}

void Table::load_data()
{
	string file_name = table_name + ".txt";
	FILE *p = fopen(file_name.c_str(), "r");
	if (!p) {
		cout << "Cannot open file.(reading)\n";
		return;
	}
	int tuple_size = 0, attr_size = 0;
	char t[10], a[10];
	fgets(t, 10, p);
	tuple_size = atoi(t);
	//cout << tuple_size << endl;
	fgets(a, 10, p);
	attr_size = atoi(a);
	//cout << attr_size << endl;
	int i, j;
	for (i = 0; i < tuple_size; i++)
	{
		Tuple t;
		for (j = 0; j < attr_size; j++)
		{
			char temp[50];
			fgets(temp, 45, p);
			string str = temp;
			Value v(attr[j].type, str.substr(0,str.length() -1 ));
			t.values.push_back(v);
			
		}
		tuples.push_back(t);
	}
	fclose(p);
	//print_table();
}


