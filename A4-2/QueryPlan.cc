#include <cstring>
#include <climits>
#include <string>
#include <algorithm>

#include "Defs.h"
#include "QueryPlan.h"
#include "Pipe.h"
#include "RelOp.h"


using std::endl;
using std::string;

extern char* catalog_path;
extern char* dbfileile_dir;
extern char* tpch_dir;

extern FuncOperator* finalFunction;
extern TableList* tables;
extern AndList* boolean;
extern NameList* groupingAtts;
extern NameList* attsToSelect;
extern int distinctAtts;
extern int distinctFunc;

QueryPlan::QueryPlan(Statistics* statistics): root(NULL), resultName("STDOUT"), statistics(statistics), used(NULL) {}
QueryPlan::~QueryPlan() { if (root) delete root; }

void QueryPlan::PlanQuery() {
	CreateLeafNode();
	CreateJoinNode();
	SumNodes();
	PRojectNodeCreation();
	FindDistinctNode();
	CreateWriteResultNodes();

	swap(boolean, used);
	if(used){
		cout<<"ERROR IN WHERE CNF"<<endl;
		exit(EXIT_FAILURE);
	}
}

void QueryPlan::PrintQuery(std::ostream& os) const {
	root->Print(os);
}

void QueryPlan::SetOutput(char* output) {
	resultName = output;
}

void QueryPlan::ExecuteQuery() {
	resultFile = (resultName == "STDOUT" ? stdout : resultName == "NONE" ? NULL : fopen(resultName.c_str(), "a")); 
	if (resultFile) {
		int nodeCount = root->pipeId;
		Pipe** pipes = new Pipe*[nodeCount];
		RelationalOp** localrelationalOp = new RelationalOp*[nodeCount];
		root->Execute(pipes, localrelationalOp);
		for (int i=0; i<nodeCount; ++i){
			localrelationalOp[i] -> WaitUntilDone();
		}
		for (int i=0; i<nodeCount; ++i) {
			delete pipes[i];
			delete localrelationalOp[i];
		}
		delete[] pipes;
		delete[] localrelationalOp;
		if (resultFile!=stdout){
			fclose(resultFile);
		}
	}
	root->pipeId = 0;
	nodes.clear();
}



void QueryPlan::CreateLeafNode() {
	for (TableList* tableList = tables; tableList; tableList = tableList->next) {
		statistics->CopyRel(tableList->tableName, tableList->aliasAs); 
		AndList* andListpushedAndList;
		LeafNode* newLeaf = new LeafNode(boolean, andListpushedAndList, tableList->tableName, tableList->aliasAs, statistics);
		ConcatenateAndList(used, andListpushedAndList);
		nodes.push_back(newLeaf); 
	}
}


void QueryPlan::PRojectNodeCreation() {
	if (attsToSelect && !finalFunction && !groupingAtts) {
		root = new ProjectNode(attsToSelect, root); 
	}
}


void QueryPlan::CreateJoinNode() {
	FindLeastCostJoin();
	while (nodes.size()>1) {

		Node* lNode = nodes.back(); 
		nodes.pop_back();
		Node* rNode = nodes.back(); 
		nodes.pop_back();


		AndList* andpushedAndListList;
		JoinNode* newJoinNode = new JoinNode(boolean, andpushedAndListList, lNode, rNode, statistics); 
		ConcatenateAndList(used, andpushedAndListList);

		nodes.push_back(newJoinNode);
	}
	root = nodes.front();
}

void QueryPlan::FindLeastCostJoin() {
	std::vector<Node*> operands(nodes);
	sort(operands.begin(), operands.end());
	int leastCost = INT_MAX, actualCost;
	do {           
	  	if ((actualCost=EstimateJoinPermutationCost(operands, *statistics, leastCost))<leastCost && actualCost>0) { 
	  		leastCost = actualCost; nodes = operands; 
	  	}
  	} while (next_permutation(operands.begin(), operands.end()));
}

int QueryPlan::EstimateJoinPermutationCost(std::vector<Node*> operands, Statistics statistics, int bfound) { 
  	std::vector<JoinNode*> tmpNodesToJoin;  
  	AndList* andRecycler = NULL;     
  	while (operands.size()>1) {     
	  	
	  	Node* left = operands.back();
		operands.pop_back();
		Node* right = operands.back(); 
		operands.pop_back();

	  	AndList* pushedAndList;
		JoinNode* newJoinNode = new JoinNode(boolean, pushedAndList, left, right, &statistics);
		ConcatenateAndList(andRecycler, pushedAndList);

	  	operands.push_back(newJoinNode);
	  	tmpNodesToJoin.push_back(newJoinNode); 

		if (newJoinNode->estimatedCost<=0 || newJoinNode->actualCost > bfound) {
			break; 
		}
	}
	int actualCost = operands.back()->actualCost;

	for (int i = 0; i < tmpNodesToJoin.size(); ++i) { 
		--tmpNodesToJoin[i]->pipeId;
		free(tmpNodesToJoin[i]); 
	}

	ConcatenateAndList(boolean, andRecycler);
  	return operands.back()->estimatedCost<0 ? -1 : actualCost;
}


void QueryPlan::SumNodes() {
	if (groupingAtts) {
		if (distinctFunc) {
			root = new DistinctNode(root); 
		}
		root = new GroupByNode(groupingAtts, finalFunction, root);
	} else if (finalFunction) {
		root = new SumNode(finalFunction, root); 
	}
}


void QueryPlan::FindDistinctNode() {
	if (distinctAtts) root = new DistinctNode(root);
}


void QueryPlan::CreateWriteResultNodes() {
	root = new WriteResultNode(resultFile, root);
}

void QueryPlan::ConcatenateAndList(AndList*& andListLeft, AndList*& andListright) {
	if (!andListLeft) { swap(andListLeft, andListright); return; }
	AndList *pre = andListLeft, *cur = andListLeft->rightAnd;
	for (; cur; pre = cur, cur = cur->rightAnd);
		pre->rightAnd = andListright;
	andListright = NULL;
}


int Node::pipeId = 0; 

Node::Node(const std::string& op, Schema* out, Statistics* statistics): operation(op), resultantSchema(out), relationCount(0), estimatedCost(0), actualCost(0), statistics(statistics), resultPipeId(pipeId++) {}

Node::Node(const std::string& op, Schema* out, char* rName, Statistics* statistics): operation(op), resultantSchema(out), relationCount(0), estimatedCost(0), actualCost(0), statistics(statistics), resultPipeId(pipeId++) {
	if (rName) {
		relNames[relationCount++] = strdup(rName);
	}
}

Node::Node(const std::string& op, Schema* out, char* rNames[], int num, Statistics* statistics): operation(op), resultantSchema(out), relationCount(0), estimatedCost(0), actualCost(0), statistics(statistics), resultPipeId(pipeId++) {
	for (; relationCount<num; ++relationCount){
		relNames[relationCount] = strdup(rNames[relationCount]);
	}
}

Node::~Node() {
	
	for (int i=0; i<relationCount; ++i){
		delete[] relNames[i];
	}
}


AndList* Node::MoveSelectionDown(AndList*& alist, Schema* targetSchemaSchema) {
	AndList andListhead; andListhead.rightAnd = alist;  

	AndList *andListcur = alist, *andListPre = &andListhead, *andListResult = NULL;
	for (; andListcur; andListcur = andListPre->rightAnd)
	if (CheckSchemaOr(andListcur->left, targetSchemaSchema)) { 
		andListPre->rightAnd = andListcur->rightAnd;
		andListcur->rightAnd = andListResult;       
		andListResult = andListcur;      
	} else {
		andListPre = andListcur;
	}
	alist = andListhead.rightAnd; 
	return andListResult;
}

bool Node::CheckSchemaOr(OrList* orlist, Schema* targetSchema) {
	for (; orlist; orlist=orlist->rightOr){
		if (!CheckSchemaComparisonOp(orlist->left, targetSchema)) {
			return false;
		}
	}
	return true;
}


bool Node::CheckSchemaComparisonOp(ComparisonOp* comparisonOp, Schema* targetSchema) {
	Operand *operandLeft = comparisonOp->left, *operandRight = comparisonOp->right;
	bool result = targetSchema->Find(operandRight->value);
	return (operandLeft->code!=NAME || targetSchema->Find(operandLeft->value)!=-1) &&
	(operandRight->code!=NAME || targetSchema->Find(operandRight->value)!=-1);
}


LeafNode::LeafNode(AndList*& boolean, AndList*& pushedAndList, char* relName, char* alias, Statistics* statistics): Node("SELECT FILE", new Schema(catalog_path, relName, alias), relName, statistics), dbFileOpen(false) {
	pushedAndList = MoveSelectionDown(boolean, resultantSchema);
	estimatedCost = statistics->Estimate(pushedAndList, relNames, relationCount);
	statistics->Apply(pushedAndList, relNames, relationCount);
	selectionOp.GrowFromParseTree(pushedAndList, resultantSchema, literal);
}


OnePipeNode::OnePipeNode(const std::string& operation, Schema* out, Node* node, Statistics* st): Node (operation, out, node->relNames, node->relationCount, st), child(node), inputPipeId(node->resultPipeId) {}

TwoPipeNode::TwoPipeNode(const std::string& operation, Node* l, Node* r, Statistics* st): Node (operation, new Schema(*l->resultantSchema, *r->resultantSchema), st),
childLeft(l), childRight(r), leftInputPipeId(childLeft->resultPipeId), rightInputPipeId(childRight->resultPipeId) {
	for (int i=0; i<l->relationCount;){
		relNames[relationCount++] = strdup(l->relNames[i++]);
	}
	for (int j=0; j<r->relationCount;){
		relNames[relationCount++] = strdup(r->relNames[j++]);
	}
}


ProjectNode::ProjectNode(NameList* nameList, Node* node): OnePipeNode("PROJECT", NULL, node, NULL), inputAttrCount(node->resultantSchema->GetNumAtts()), outputAttrCount(0) {
	Schema* schema = node->resultantSchema;
	Attribute resultAtts[MAX_ATTRIBUTE_COUNT];
	for (; nameList; nameList=nameList->next, outputAttrCount++) {

		resultAtts[outputAttrCount].name = nameList->name; 
		resultAtts[outputAttrCount].myType = schema->FindType(nameList->name);
	}
	resultantSchema = new Schema ("", outputAttrCount, resultAtts); 
}

///******************
DistinctNode::DistinctNode(Node* node): OnePipeNode("DISTINCT", new Schema(*node->resultantSchema), node, NULL), orderMakerDistinct(node->resultantSchema) {}


JoinNode::JoinNode(AndList*& boolean, AndList*& pushedAndList, Node* l, Node* r, Statistics* st): TwoPipeNode("JOIN", l, r, st) {
	pushedAndList = MoveSelectionDown(boolean, resultantSchema);
	estimatedCost = statistics->Estimate(pushedAndList, relNames, relationCount);
	statistics->Apply(pushedAndList, relNames, relationCount);
	actualCost = l->actualCost + estimatedCost + r->actualCost; 
	selectionOp.GrowFromParseTree(pushedAndList, l->resultantSchema, r->resultantSchema, literal);
}


SumNode::SumNode(FuncOperator* parseTree, Node* node): OnePipeNode("SUM", resultSchema(parseTree, node), node, NULL) {
	function.GrowFromParseTree (parseTree, *node->resultantSchema);
}

Schema* SumNode::resultSchema(FuncOperator* parseTree, Node* node) {
	Function function;
	Attribute attributes[2][1] = {{{"sum", Int}}, {{"sum", Double}}};
	function.GrowFromParseTree (parseTree, *node->resultantSchema);
	return new Schema ("", 1, attributes[function.GetReturnsIntType()]); 
}

GroupByNode::GroupByNode(NameList* groupUs, FuncOperator* parseTree, Node* node): OnePipeNode("GROUPBY", resultSchema(groupUs, parseTree, node), node, NULL) {
	grpByOM.growFromParseTree(groupUs, node->resultantSchema);
	function.GrowFromParseTree (parseTree, *node->resultantSchema);
}

Schema* GroupByNode::resultSchema(NameList* groupUs, FuncOperator* parseTree, Node* node) {
	Function fun;
	Attribute atts[2][1] = {{{"sum", Int}}, {{"sum", Double}}};
	Schema* schema = node->resultantSchema;
	fun.GrowFromParseTree (parseTree, *schema);
	Attribute resultAtts[MAX_ATTRIBUTE_COUNT];

	resultAtts[0].name = "sum";
	resultAtts[0].myType = fun.GetReturnsIntType();
	int outputAttrCount = 1;
	for (; groupUs; groupUs=groupUs->next, outputAttrCount++) {
		resultAtts[outputAttrCount].name = groupUs->name; 
		resultAtts[outputAttrCount].myType = schema->FindType(groupUs->name); 
	}
	return new Schema ("", outputAttrCount, resultAtts);
}


WriteResultNode::WriteResultNode(FILE*& out, Node* node): OnePipeNode("WRITEOUT(PROJECT)", new Schema(*node->resultantSchema), node, NULL), resultFile(out) {}


void LeafNode::Execute(Pipe** pipes, RelationalOp** localrelationalOp) {
	std::string dbName = std::string(relNames[0]) + ".bin";
	dbfile.Open((char*)dbName.c_str()); dbFileOpen = true;
	SelectFile* selectFile = new SelectFile();

	pipes[resultPipeId] = new Pipe(PIPE_SIZE);
	localrelationalOp[resultPipeId] = selectFile;
	selectFile -> Run(dbfile, *pipes[resultPipeId], selectionOp, literal);
}


void ProjectNode::Execute(Pipe** pipes, RelationalOp** localrelationalOp) {
	child -> Execute(pipes, localrelationalOp);
	Project* project = new Project();

	pipes[resultPipeId] = new Pipe(PIPE_SIZE);
	localrelationalOp[resultPipeId] = project;
	project -> Run(*pipes[inputPipeId], *pipes[resultPipeId], storeThis, inputAttrCount, outputAttrCount);
}


void DistinctNode::Execute(Pipe** pipes, RelationalOp** localrelationalOp) {
	child -> Execute(pipes, localrelationalOp);
	DuplicateRemoval* removeUs = new DuplicateRemoval();
	pipes[resultPipeId] = new Pipe(PIPE_SIZE);
	localrelationalOp[resultPipeId] = removeUs;
	removeUs -> Run(*pipes[inputPipeId], *pipes[resultPipeId], *resultantSchema);
}


void SumNode::Execute(Pipe** pipes, RelationalOp** localrelationalOp) {
	child -> Execute(pipes, localrelationalOp);
	Sum* sum = new Sum();
	pipes[resultPipeId] = new Pipe(PIPE_SIZE);
	localrelationalOp[resultPipeId] = sum;
	sum -> Run(*pipes[inputPipeId], *pipes[resultPipeId], function);
}


void GroupByNode::Execute(Pipe** pipes, RelationalOp** localrelationalOp) {
	child -> Execute(pipes, localrelationalOp);
	GroupBy* grp = new GroupBy();
	pipes[resultPipeId] = new Pipe(PIPE_SIZE);
	localrelationalOp[resultPipeId] = grp;
	grp -> Run(*pipes[inputPipeId], *pipes[resultPipeId], grpByOM, function);
}


void JoinNode::Execute(Pipe** pipes, RelationalOp** localrelationalOp) {
	childLeft -> Execute(pipes, localrelationalOp); childRight -> Execute(pipes, localrelationalOp);
	Join* joinOp = new Join();
	pipes[resultPipeId] = new Pipe(PIPE_SIZE);
	localrelationalOp[resultPipeId] = joinOp;
	joinOp -> Run(*pipes[leftInputPipeId], *pipes[rightInputPipeId], *pipes[resultPipeId], selectionOp, literal);
}


void WriteResultNode::Execute(Pipe** pipes, RelationalOp** localrelationalOp) {
	child -> Execute(pipes, localrelationalOp);
	WriteOut* writeOut = new WriteOut();
	pipes[resultPipeId] = new Pipe(PIPE_SIZE);
	localrelationalOp[resultPipeId] = writeOut;
	writeOut -> Run(*pipes[inputPipeId], resultFile, *resultantSchema);

}


string Indent(int level, string s){
	return string(3*(level+1), ' ') + s;
}

string IndentOperator(int level){
	return Indent(level, "---> ");
}

string IndentOperatorInfo(int level){
	return Indent(level, "+ ");
}

void Node::Print(std::ostream& os, int level) const {
	PrintOperator(os, level);
	PrintOperatorInfo(os, level);
	PrintSchema(os, level);
	PrintPipe(os, level);
	PrintChildren(os, level);
}


void Node::PrintOperator(std::ostream& os, int level) const {
	os << IndentOperator(level) << operation << ": ";
}

void Node::PrintSchema(std::ostream& os, int level) const {
	os << IndentOperatorInfo(level) << "Output schema:" << endl;
	resultantSchema->Print(os);
}

void LeafNode::PrintPipe(std::ostream& os, int level) const {
	os << IndentOperatorInfo(level) << "Output pipe: " << resultPipeId << endl;
}

void OnePipeNode::PrintPipe(std::ostream& os, int level) const {
	os << IndentOperatorInfo(level) << "Output pipe: " << resultPipeId << endl;
	os << IndentOperatorInfo(level) << "Input pipe: " << inputPipeId << endl;
}

void TwoPipeNode::PrintPipe(std::ostream& os, int level) const {
	os << IndentOperatorInfo(level) << "Output pipe: " << resultPipeId << endl;
	os << IndentOperatorInfo(level) << "Input pipe: " << leftInputPipeId << ", " << rightInputPipeId << endl;
}

void LeafNode::PrintOperator(std::ostream& os, int level) const {
	os << IndentOperator(level) << "Select from " << relNames[0] << ": ";
}


void LeafNode::PrintOperatorInfo(std::ostream& os, int level) const {
	selectionOp.Print();
}

void ProjectNode::PrintOperatorInfo(std::ostream& os, int level) const {
	os << storeThis[0];
	for (int i=1; i<outputAttrCount; ++i) os << ',' << storeThis[i];
		os << endl;
	os << IndentOperatorInfo(level) << inputAttrCount << " input attributes; " << outputAttrCount << " output attributes" << endl;
}

void JoinNode::PrintOperatorInfo(std::ostream& os, int level) const {
	selectionOp.Print();
	os << IndentOperatorInfo(level) << "Estimate = " << estimatedCost << ", Cost = " << actualCost << endl;
}

void SumNode::PrintOperatorInfo(std::ostream& os, int level) const {
	os << IndentOperatorInfo(level) << "Function: "; (const_cast<Function*>(&function))->Print();
}

void GroupByNode::PrintOperatorInfo(std::ostream& os, int level) const {
	os << IndentOperatorInfo(level) << "OrderMaker: "; (const_cast<OrderMaker*>(&grpByOM))->Print();
	os << IndentOperatorInfo(level) << "Function: "; (const_cast<Function*>(&function))->Print();
}

void WriteResultNode::PrintOperatorInfo(std::ostream& os, int level) const {
	os << IndentOperatorInfo(level) << "Output to " << resultFile << endl;
}