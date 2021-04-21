#ifndef QUERY_PLAN_H_
#define QUERY_PLAN_H_

#include <iostream>
#include <string>
#include <vector>

#include "DBFile.h"
#include "Schema.h"
#include "Function.h"
#include "ParseTree.h"
#include "Statistics.h"
#include "Comparison.h"

#define MAX_RELATIONS 12
#define MAX_RELATION_NAME 50
#define MAX_ATTRIBUTE_COUNT 100

class Node;
class QueryPlan {
public:
	QueryPlan(Statistics* st);
	~QueryPlan();

	void PlanQuery();
	void PrintQuery(std::ostream& os = std::cout) const;
	void SetOutput(char* out);
	void ExecuteQuery();
	int test() {for (int i = 0 ; i < 100000; i++); return 0;}
	int testCreateLeafNode() { return test();}
	int testEstimateJoinPermutationCost() { return test();}
	int testFindLeastCostJoin() { return test();}


private:
	Node* root;
	std::vector<Node*> nodes;
	std::string resultName;
	FILE* resultFile;

	Statistics* statistics;
	AndList* used;  
	void CreateLeafNode();
	void CreateJoinNode();
	void FindLeastCostJoin();
	void SumNodes();
	void PRojectNodeCreation();
	void FindDistinctNode();
	void CreateWriteResultNodes();
  	int EstimateJoinPermutationCost(std::vector<Node*> operands, Statistics st, int bestFound); // intentional copy

	static void ConcatenateAndList(AndList*& left, AndList*& right);

	QueryPlan(const QueryPlan&);
	QueryPlan& operator=(const QueryPlan&);
};

class Pipe; class RelationalOp;
class Node {
	friend class QueryPlan;
	friend class OnePipeNode;
	friend class TwoPipeNode;   
	friend class ProjectNode;
	friend class DistinctNode;
	friend class JoinNode;
	friend class SumNode;
	friend class GroupByNode;
	friend class WriteResultNode;

public:
	virtual ~Node();

protected:
	std::string operation;
	Schema* resultantSchema;
	char* relNames[MAX_RELATIONS];
	int relationCount;
	int estimatedCost, actualCost;  
	Statistics* statistics;
	int resultPipeId;
	static int pipeId;

	Node(const std::string& operation, Schema* out, Statistics* statistics);
	Node(const std::string& operation, Schema* out, char* relName, Statistics* statistics);
	Node(const std::string& operation, Schema* out, char* relNames[], int num, Statistics* statistics);

	virtual void Print(std::ostream& os = std::cout, int level = 0) const;
	virtual void PrintOperator(std::ostream& os = std::cout, int level = 0) const;
	virtual void PrintSchema(std::ostream& os = std::cout, int level = 0) const;
	virtual void PrintOperatorInfo(std::ostream& os = std::cout, int level = 0) const = 0;
	virtual void PrintPipe(std::ostream& os, int level = 0) const = 0;
	virtual void PrintChildren(std::ostream& os, int level = 0) const = 0;

	virtual void Execute(Pipe** pipes, RelationalOp** relationalOp) = 0;

	static AndList* MoveSelectionDown(AndList*& andList, Schema* targetSchema);
	static bool CheckSchemaOr(OrList* orlist, Schema* targetSchema);
	static bool CheckSchemaComparisonOp(ComparisonOp* comparisonOp, Schema* targetSchema);
};

class LeafNode: private Node { 
	friend class QueryPlan;

	DBFile dbfile;
	bool dbFileOpen;
	CNF selectionOp;
	Record literal;

	LeafNode (AndList*& boolean, AndList*& pushed, char* relName, char* alias, Statistics* statistics);
	~LeafNode() {
		if (dbFileOpen) dbfile.Close();
	}

	void PrintOperator(std::ostream& os = std::cout, int level = 0) const;
	void PrintOperatorInfo(std::ostream& os = std::cout, int level = 0) const;
	void PrintPipe(std::ostream& os, int level) const;
	void PrintChildren(std::ostream& os, int level) const {}

	void Execute(Pipe** pipes, RelationalOp** relationalOp);
};

class OnePipeNode: protected Node {
	friend class QueryPlan;

protected:
	Node* child;
	int inputPipeId;

	OnePipeNode(const std::string& operation, Schema* out, Node* node, Statistics* st);
	virtual ~OnePipeNode() { delete child; }
	void PrintPipe(std::ostream& os, int level) const;
	void PrintChildren(std::ostream& os, int level) const { child->Print(os, level+1); }
};

class TwoPipeNode: protected Node { 
	friend class QueryPlan;

protected:
	Node* childLeft;
	Node* childRight;
  	int leftInputPipeId, rightInputPipeId;

	TwoPipeNode(const std::string& operation, Node* l, Node* r, Statistics* st);
	virtual ~TwoPipeNode() {
		delete childLeft; delete childRight;
	}
	
	void PrintPipe(std::ostream& os, int level) const;
	void PrintChildren(std::ostream& os, int level) const{
		childLeft->Print(os, level+1);
		childRight->Print(os, level+1);
	}
};

class ProjectNode: private OnePipeNode {
	friend class QueryPlan;

	int storeThis[MAX_ATTRIBUTE_COUNT];
	int inputAttrCount, outputAttrCount;

	ProjectNode(NameList* attributes, Node* node);
	void PrintOperatorInfo(std::ostream& os = std::cout, int level = 0) const;
	void Execute(Pipe** pipes, RelationalOp** relationalOp);
};

class DistinctNode: private OnePipeNode {
	friend class QueryPlan;

	OrderMaker orderMakerDistinct;

	DistinctNode(Node* node);
	void PrintOperatorInfo(std::ostream& os = std::cout, int level = 0) const {}
	void Execute(Pipe** pipes, RelationalOp** relationalOp);
};

class SumNode: private OnePipeNode {
	friend class QueryPlan;

	Function function;

	SumNode(FuncOperator* parseTree, Node* node);
	Schema* resultSchema(FuncOperator* parseTree, Node* node);
	void PrintOperatorInfo(std::ostream& os = std::cout, int level = 0) const;
	void Execute(Pipe** pipes, RelationalOp** relationalOp);
};

class GroupByNode: private OnePipeNode {
	friend class QueryPlan;

	OrderMaker grpByOM;
	Function function;

	GroupByNode(NameList* groupUs, FuncOperator* parseTree, Node* node);
	Schema* resultSchema(NameList* groupUs, FuncOperator* parseTree, Node* node);
	void PrintOperatorInfo(std::ostream& os = std::cout, int level = 0) const;
	void Execute(Pipe** pipes, RelationalOp** relationalOp);
};

class JoinNode: private TwoPipeNode {
	friend class QueryPlan;

	CNF selectionOp;
	Record literal;

	JoinNode(AndList*& boolean, AndList*& pushed, Node* l, Node* r, Statistics* statistics);
	void PrintOperatorInfo(std::ostream& os = std::cout, int level = 0) const;
	void Execute(Pipe** pipes, RelationalOp** relationalOp);
};

class WriteResultNode: private OnePipeNode {
	friend class QueryPlan;

	FILE*& resultFile;

	WriteResultNode(FILE*& out, Node* node);
	void PrintOperatorInfo(std::ostream& os = std::cout, int level = 0) const;
	void Execute(Pipe** pipes, RelationalOp** relationalOp);
};

#endif