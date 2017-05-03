%option noyywrap				
	/* 	To solve the imcompatability on Win10. 
	*	If on other environment, you can try comment this line.	*/
%Start CREATE INSERT SELECT
%{
#define MAX_NAME_LENGTH 100
#define MAX_ID_LENGTH	256
#define PARSERERROR	{printf("\nParser error at FILE %d: %s, line %d: %s",filenum,CurFile,linenum,yytext);error=true;}
#define LL(x) {strcat(str,x);}

char CurFile[MAX_NAME_LENGTH];
int filenum=0;
int linenum=0;
char str[MAX_ID_LENGTH];
bool termination=false;
bool error=false;
bool Eof=false;
enum QUETYPE
{
	_CREATE,_INSERT,_SELECT
}CurQue;
enum STAGE
{
	_TABLENAME,_ATTRIBUTENAME,_COMMON,_VALUES,_CONDITION,_FUNC
}CurSta;
void NewCreate();
void NewInsert();
void NewSelect();
void catchCondition();
Create_Command createorder;
Insert_Command insertorder;
Select_Command selectorder;
Attribute curattr;
Value Curval;
select_attr CurSA;
Expression Curexp;
Element Curele;
vector<string> Oritablename;
vector<string> ALs;

%}

id			[a-zA-Z_][a-zA-Z_0-9]*
space		[ \t\r]
recour		\n
blank		[ \n\r\t]
interger	-?[1-9][0-9]*|0
varchartype	varchar{blank}*\({blank}*{interger}{blank}*\)
create		[Cc][Rr][Ee][Aa][Tt][Ee]{blank}+[Tt][Aa][Bb][Ll][Ee]
insert		[Ii][Nn][Ss][Ee][Rr][Tt]{blank}+[Ii][Nn][Tt][Oo]
values		[Vv][Aa][Ll][Uu][Ee][Ss]
quotestring	[\"\'][^,;]*[\"\']
null		[Nn][Uu][Ll][Ll]
select		[Ss][Ee][Ll][Ee][Cc][Tt]
from		[Ff][Rr][Oo][Mm]
where		[Ww][Hh][Ee][Rr][Ee]
abnormal	[,\(\)]{2}|;\)
sum			[Ss][Uu][Mm]\(
count		[Cc][Oo][Uu][Nn][Tt]\(
as			[Aa][Ss]
alias		{id}"."{id}|{id}".*"
buildalias	{id}{blank}+{as}{blank}+{id}
or			[Oo][Rr]
and			[Aa][Nn][Dd]
operator	[<>=]|"<>"|"!="|"^="
expre		[{id}{interger}]{space}*{operator}{space}*[{id}{interger}]
end			[Ee][Nn][Dd]
	/*	Declarations	*/
%%
{recour}			linenum+=1;
{space}				;
{end}				return 0;
<INITIAL>{create}	{BEGIN CREATE;NewCreate();}
<INITIAL>{insert}	{BEGIN INSERT;NewInsert();}
<INITIAL>{select}	{BEGIN SELECT;NewSelect();}
<CREATE>{id}		{
						switch(CurSta)
						{
							case _TABLENAME:
							{
								string temp=yytext;
								createorder.Set_Name(temp);
								break;
							}
							case _ATTRIBUTENAME:
							{
								LL(yytext);
								LL(" ");
							}
						}
					}
<CREATE>{varchartype}	LL(yytext);
<CREATE>,			{
						if(CurSta==_ATTRIBUTENAME)
						{
							string temp=str;
							createorder.Add_Attribute(temp);
							memset(str,0,sizeof(str));
						}
					}
<CREATE>\(			CurSta=_ATTRIBUTENAME;
<CREATE>\)			{
						CurSta=_COMMON;
						string temp=str;
						createorder.Add_Attribute(temp);
						memset(str,0,sizeof(str));	
					}
<CREATE>;			{
						if(error==true)
						{
							error=false;
							BEGIN INITIAL;
						}
						if(CurSta==_COMMON)
						{
							termination=true;
							BEGIN INITIAL;
							return 0;
						}
						else PARSERERROR;
					}
	/*	 CRREATE 	*/
<INSERT>{null}		{
						if(CurSta==_VALUES)
						{
							Curval.type=_NULL;
							insertorder.insert_values.push_back(Curval);
						}
					}
<INSERT>{id}		{
						string temp=yytext;
						if(CurSta==_TABLENAME)insertorder.tablename=temp;
						if(CurSta==_ATTRIBUTENAME)insertorder.attr_name.push_back(temp);
						if(CurSta==_VALUES)PARSERERROR;
					}
<INSERT>{interger}	{
						if(CurSta==_VALUES)
						{
							Curval.type=_INT;
							Curval.val=yytext;
							insertorder.insert_values.push_back(Curval);
						}
						else PARSERERROR;
					}
<INSERT>{quotestring}	{
							if(CurSta==_VALUES)
							{
								string temp=yytext;
								Curval.type=VARCHAR;
								Curval.val=temp.substr(1,temp.length()-2);
								insertorder.insert_values.push_back(Curval);
							}
							else PARSERERROR;
						}
<INSERT>{values}{blank}*\(	CurSta=_VALUES;
<INSERT>\(			{
						if(CurSta==_TABLENAME)CurSta=_ATTRIBUTENAME;
						//else if(CurSta==_COMMON)CurSta=_VALUES;
						else PARSERERROR;
					}
<INSERT>\)			{
						CurSta=_COMMON;
					}
<INSERT>\,			;
<INSERT>;			{
						if(error==true)
						{
							error=false;
							BEGIN INITIAL;
						}
						BEGIN INITIAL;
						termination=true;
						return 0;
					}

	/*	INSERT	*/
<SELECT>{from}		{
						//printf("get a from");
						if(CurSta==_ATTRIBUTENAME)CurSta=_TABLENAME;
						else PARSERERROR;
					}
<SELECT>{where}		{
						//printf("get a where");
						CurSta=_CONDITION;
					}
<SELECT>{or}		{
						selectorder.condt.exp_num=2;
						selectorder.condt.logic=_OR;
						selectorder.condt.exp1=Curexp;
						selectorder.condt.exp1.elem2=Curele;
					}
<SELECT>{and}		{
						selectorder.condt.exp_num=2;
						selectorder.condt.logic=_AND;
						selectorder.condt.exp1=Curexp;
						selectorder.condt.exp1.elem2=Curele;
					}
<SELECT>"*"			{
						if(CurSta==_ATTRIBUTENAME||CurSta==_FUNC)selectorder.is_all=true;
						else PARSERERROR;
						if(CurSta==_FUNC)
							//无法预知from中的table有多少个。因此只能一视同仁地将*转化为全部attr加入selectorder.attr。
							//建议以table数量作为是否转化的标准而不是函数是否存在。
						{
							/*for(int i=0;i<dbms.tables.size();i++)
							{
								for(int j=0;j<dbms.tables[i].attr.size();j++)
								{
									CurSA.attr_name=dbms.tables[i].attr[j].attr_name;
									CurSA.tablename=dbms.tables[i].table_name;
									CurSA.alias=true;//Suppose it's true
									selectorder.attr.push_back(CurSA);
								}
							}*/
						}
						
					}
<SELECT>{alias}		{
						string temp=yytext;
						int dotpos=temp.find('.');
						CurSA.attr_name=temp.substr(dotpos+1,temp.length()-dotpos);
						//printf("%s",CurSA.attr_name.c_str());
						CurSA.tablename=temp.substr(0,dotpos);
						//printf("%s",CurSA.tablename.c_str());
						CurSA.alias=true;
						if(CurSta==_ATTRIBUTENAME||CurSta==_FUNC)
						{
							selectorder.attr.push_back(CurSA);
						}
						if(CurSta==_CONDITION)
						{
							for(int i=0;i<Oritablename.size();i++)
							{
								if(CurSA.tablename.compare(ALs[i])==0)
								{
									CurSA.tablename=Oritablename[i];
									break;
								}
							}
							Curele.attr=CurSA;
							Curele.is_imme=false;
						}
					}
<SELECT>{buildalias}	{
							char table[256]={};
							char AL[256]={};
							sscanf(yytext,"%s %*s %s",table,AL);
							string tablename=table;
							string Alias=AL;
							Oritablename.push_back(tablename);
							ALs.push_back(AL);
							selectorder.tablename.push_back(tablename);
							for(int i=0;i<selectorder.attr.size();i++)
							{
								if(selectorder.attr[i].tablename==AL)
								{
									int tableindex=dbms.get_table_index(tablename);
									if(selectorder.attr[i].attr_name[0]=='*')
									{
										selectorder.attr[i].attr_name=dbms.tables[tableindex].attr[0].attr_name;
										for(int j=1;j<dbms.tables[tableindex].attr.size();j++)
										{
											CurSA.attr_name=dbms.tables[tableindex].attr[j].attr_name;
											CurSA.tablename=tablename;
											CurSA.alias=true;
											selectorder.attr.push_back(CurSA);
										}
									}
									selectorder.attr[i].tablename=tablename;
									selectorder.attr[i].alias=true;
								}
							}
						}
<SELECT>{count}		{
						if(CurSta==_ATTRIBUTENAME)
						{
							selectorder.func_mode=COUNT;
							CurSta=_FUNC;
						}
					}
<SELECT>{sum}		{
						if(CurSta==_ATTRIBUTENAME)
						{
							selectorder.func_mode=SUM;
							CurSta=_FUNC;
						}
					}
<SELECT>{id}		{
						if(CurSta==_ATTRIBUTENAME||CurSta==_FUNC)
						{
							string temp=yytext;
							CurSA.attr_name=temp;
							CurSA.alias=false;
							CurSA.tablename.resize(0);
							selectorder.attr.push_back(CurSA);
						}
						else if(CurSta==_TABLENAME)
						{
							string temp=yytext;
							selectorder.tablename.push_back(temp);
							if(selectorder.tablename.size()>=2)
							{
								for(int i=0;i<selectorder.attr.size();i++)
								{
									if(selectorder.attr[i].attr_name.compare("*")==0)
									{
										for(int j=0;j<selectorder.tablename.size();j++)
										{
											if(selectorder.attr[i].tablename==selectorder.tablename[j])
											{
												int index=dbms.get_table_index(selectorder.tablename[j]);
												selectorder.attr[i].attr_name=dbms.tables[index].attr[0].attr_name;
												for(int k=1;k<dbms.tables[index].attr.size();k++)
												{
													CurSA.tablename=selectorder.tablename[j];
													CurSA.attr_name=dbms.tables[index].attr[k].attr_name;
													CurSA.alias=true;
													selectorder.attr.push_back(CurSA);
												}
											}
										}
									}
								}
							}
						}
						else if(CurSta==_CONDITION)
						{
							Curele.is_imme=false;
							CurSA.attr_name=yytext;
							CurSA.alias=false;
							Curele.attr=CurSA;
						}
					}
<SELECT>{quotestring}	{
							if(CurSta==_CONDITION)
							{
								string temp=yytext;
								Curele.is_imme=true;
								Curele.imme_type=VARCHAR;
								Curele.imme=temp.substr(1,temp.length()-2);
							}
						}				
<SELECT>{interger}	{
						if(CurSta==_CONDITION)
						{
							Curele.is_imme=true;
							Curele.imme_type=_INT;
							Curele.imme=yytext;//how to express a immi?
						}
					}
<SELECT>{operator}	{
						Curexp.elem1=Curele;
						if(strcmp(yytext,">")==0)Curexp.op=GRE;
						if(strcmp(yytext,"<")==0)Curexp.op=LESS;
						if(strcmp(yytext,"=")==0)Curexp.op=EQU;
						if(strcmp(yytext,"!=")==0)Curexp.op=NEQ;
						if(strcmp(yytext,"<>")==0)Curexp.op=NEQ;
						if(strcmp(yytext,"^=")==0)Curexp.op=NEQ;
					}
<SELECT>\)			{
						if(CurSta==_FUNC)CurSta=_ATTRIBUTENAME;
						else PARSERERROR;
					}
<SELECT>,			;
<SELECT>;			{
						//printf("get a semicolon");
						if(error==true)
						{
							error=false;
							BEGIN INITIAL;
						}
						else
						{
							termination=true;
							if(CurSta==_CONDITION)
							{
								if(selectorder.condt.exp_num==2)
								{
									Curexp.elem2=Curele;
									selectorder.condt.exp2=Curexp;
								}
								if(selectorder.condt.exp_num==0)
								{
									Curexp.elem2=Curele;
									selectorder.condt.exp1=Curexp;
									selectorder.condt.exp_num=1;
								}
							}
							if(selectorder.func_mode!=NOR&&selectorder.is_all==true)
							{
								for(int i=0;i<selectorder.tablename.size();i++)
								{
									int index=dbms.get_table_index(selectorder.tablename[i]);
									for(int j=0;j<dbms.tables[index].attr.size();j++)
									{
										CurSA.attr_name=dbms.tables[index].attr[j].attr_name;
										CurSA.alias=true;
										CurSA.tablename=selectorder.tablename[i];
										selectorder.attr.push_back(CurSA);
									}
								}
							}
							BEGIN INITIAL;
							Oritablename.clear();
							ALs.clear();
							return 0;
						}
					}
.	{PARSERERROR;}
%%

void catchCondition()
{
	
}

void yyparse()
{
	error=true;
	return;
}

void NewCreate()
{
	createorder.attributes.clear();
	CurQue=_CREATE;
	CurSta=_TABLENAME;
	return;
}

void NewInsert()
{
	insertorder.attr_name.clear();
	insertorder.insert_values.clear();
	CurQue=_INSERT;
	CurSta=_TABLENAME;
	return;
}

void NewSelect()
{
	//printf("get a select");
	selectorder.Collapse();
	CurQue=_SELECT;
	CurSta=_ATTRIBUTENAME;
	return;
}

void lex()
{
	termination=false;
	error=false;
	Eof=false;
	linenum=1;
	while(true)//It should continue only terminated
	{
		yylex();
		if(termination)
		{
			switch(CurQue)
			{
				case _CREATE:
				{
					Table t;
					dbms.Create(createorder,t);
					break;
				}
				case _INSERT:
				{
					dbms.Insert(insertorder);
					break;
				}
				case _SELECT:
				{
					dbms.Select(selectorder);
					break;
				}
			}
			termination=false;
			continue;
		}
		break;
	}
	return;
}
