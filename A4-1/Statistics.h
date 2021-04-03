#ifndef STATISTICS_H_
#define STATISTICS_H_
#include "ParseTree.h"
#include <vector>
#include <string>
#include <unordered_map>
#include <fstream>
#include <iostream>
#include <unordered_set>
#include <sstream>
using namespace std;
struct Rel{
	int numAttr;
	unordered_map<string,double> attr;
	Rel(){};//default constructor
	Rel(int numAttr):numAttr(numAttr){}//this is default attribute
	void AddRel(char *attrName,int numDistincts)
	{
		attr[string(attrName)]=numDistincts;
	}
	void operator= (const Rel& copy){
		this->numAttr=copy.numAttr;
		this->attr=copy.attr;
	}
	void AddAtt(string Name,int val){
		attr[Name]=val;
	}
	friend std::ostream& operator<<(std::ostream& os, const Rel& relation);
  	friend std::istream& operator>>(std::istream& is, Rel& Rel);
};

class Statistics
{
	unordered_map<string,Rel> relation;
	unordered_map<string,double> estimate;
	
	/*
		we will split the string for finding purose
		1. We have kept unordered_map for that we don't insert same values in the unordered_map again
	*/
	std::unordered_set<string> split(string s){
		stringstream ss(s);
		string line;
		unordered_set<string> res;
		while(getline(ss,line,'#')){
			res.insert(line);
		}
		return res;
	}
	bool checkRel(char *relName[],int numJoin){
		unordered_set<string> st;
		for(auto i=0;i<numJoin;i++)
			st.insert(string(relName[i]));
		for(auto i=0;i<numJoin;i++){
			string rel(relName[i]);
			for(auto iter:estimate){
				auto check=split(iter.first);
				if(check.count(rel)){
					for(auto i:check){
						if(!st.count(i)){
							return false;
						}
					}
				}
			}
		}
		return true;
	}
	bool checkAtt(struct Operand *left, double &t, char **relNames, int numToJoin){
	string operand(left->value);
	if(left->code == 4){
			bool found = false;
			for(int i=0; i<numToJoin; i++){
				string relname(relNames[i]);				
				if(relation[relname].attr.count(operand)){
						found = true;
						t = relation[relname].attr[operand]!=-1?relation[relname].attr[operand]:t;
						return true;
				}
			}
			if(!found){
						cout<<operand<<" not found!!"<<endl;
						return false;
			}
	}
	return true;
}
public:
	Statistics();
	Statistics(Statistics &copyMe);	 // Performs deep copy
	~Statistics();

	int ParseRelation(string name, string &rel);

	void LoadAllStatistics();
	void AddRel(char *relName, int numTuples);
	void AddAtt(char *relName, char *attName, int numDistincts);
	void CopyRel(char *oldName, char *newName);
	
	void Read(char *fromWhere);
	void Write(char *fromWhere);

	void  Apply(struct AndList *parseTree, char *relNames[], int numToJoin);
	double Estimate(struct AndList *parseTree, char **relNames, int numToJoin);
	friend std::ostream& operator<<(std::ostream& os, const Statistics& stat);
  	friend std::istream& operator>>(std::istream& is, Statistics& stat);
};

#endif