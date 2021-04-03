#include "Statistics.h"
#include "Helper.h"
Statistics::Statistics()
{
}
Statistics::Statistics(Statistics &copyMe)
{
}
Statistics::~Statistics()
{
}

void Statistics::AddRel(char *relName, int numTuples)
{
    relation[string(relName)].numAttr=numTuples;
    estimate[string(relName)]=numTuples;
}
void Statistics::AddAtt(char *relName, char *attName, int numDistincts)
{
    relation[string(relName)].AddAtt(string(attName),numDistincts);
}
void Statistics::CopyRel(char *oldName, char *newName)
{
    string newRelation=string(newName),oldRelation=string(oldName);
    relation[string(newName)].numAttr=relation[oldRelation].numAttr;
    for(auto iter:relation[oldRelation].attr)
        relation[newRelation].attr[newRelation+"."+iter.first]=iter.second;
    estimate[newRelation]=relation[newRelation].numAttr;
}
	
void Statistics::Read(char *fromWhere)
{
    ifstream in(fromWhere);
    in>>*this;
    in.close();
}
void Statistics::Write(char *fromWhere)
{
    ofstream out(fromWhere);
    out<<*this;
	out << endl;
    out.close();
}

void  Statistics::Apply(struct AndList *parseTree, char *relNames[], int numToJoin)
{
    if(!checkRel( relNames, numToJoin))
		return ;	
	struct AndList *pA = parseTree;
    double fact = 1.0,orFact = 0,orFactDe = 0,orFactPartial = 0;
    while(pA!= NULL){
		struct OrList *pOr = pA->left;
		orFact = 0;
		orFactDe = 1;
		unordered_map<string, int> diff;
		string previous;
		while(pOr!=NULL){
				struct ComparisonOp *pComp = pOr->left;
				double tLeft = 1.0,tRight = 1.0;
				if(!checkAtt(pComp->left, tLeft, relNames, numToJoin) || !checkAtt(pComp->right, tRight, relNames, numToJoin)){
					return ;
				}			
				string oper1(pComp->left->value);

				if (pComp->code == 1 || pComp->code == 2){
						orFact += 1.0/3;
						orFactDe *= 1.0/3;
                        for(auto & iter:relation)
							if(iter.second.attr.count(oper1)!=0)
								iter.second.attr[oper1] /= 3;
						
						pOr = pOr->rightOr;
						continue;
                }
				if(tRight==1.0|| tLeft>tRight || tLeft<tRight){
					if(diff.count(oper1)){
						diff[oper1]++;
					}
					else{
						diff[oper1] = min(tLeft,tRight);
					}
						
                }
				if(oper1.compare(previous)!=0 && orFactPartial != 0){
						orFact += orFactPartial;
						orFactDe *= orFactPartial;
						orFactPartial = 0;
						
					}
					orFactPartial += 1.0/max(tLeft, tRight);
					previous = oper1;

				pOr = pOr->rightOr;
        }
        
       if(orFactPartial){
				orFact += orFactPartial;
				orFactDe *= orFactPartial;
				orFactPartial = 0;				
		}
        
		if(orFact!=orFactDe){
			fact *= (orFact - orFactDe);
		}
		else{
			fact *=  orFact;
		}
        for(auto &ch:diff)
            for(auto &rel:relation)
				if(rel.second.attr.count(ch.first)){
					rel.second.attr[ch.first] = ch.second;
				}
        pA = pA->rightAnd;
    }
  
    long double maxTup = 1;
    bool reltable[numToJoin];
    for(int i=0; i<numToJoin; i++){
		reltable[i] = true;
	}
	
    for(int i=0; i<numToJoin; i++){
		if(!reltable[i]){
			continue;
		}
		string relname(relNames[i]);
		for(auto iter = estimate.begin(); iter!=estimate.end(); iter++){
            auto check=split(iter->first);
			if(check.count(relNames[i])!=0){				
				reltable[i] = false;
				maxTup *= iter->second;
				for(int j=i+1; j<numToJoin; j++){
					if(check.count(relNames[j])!=0)
						reltable[j] = false;
				}
                break;
            }
        }
	 }
	 
	double result = fact * maxTup;

    string newSet;
    for(int i=0; i<numToJoin; i++){
		string relname(relNames[i]);
		newSet+=relname+"#";
		for(auto iter =estimate.begin(); iter!=estimate.end(); iter++){
            auto check=split(iter->first);
			if(check.count(relname)!=0){
				estimate.erase(iter);
				break;
			}
		}
    }
    estimate.insert({newSet,result});
}
double Statistics::Estimate(struct AndList *parseTree, char **relNames, int numToJoin)
{
    if(!checkRel(relNames,numToJoin)){
        return 0;
    }
    struct AndList *pA = parseTree;
    double fact = 1.0;
    double orFact = 0;
    double orFactDe = 0;
    double orFactPartial = 0;
    while(pA!= NULL){
		struct OrList *pOr = pA->left;
		orFact = 0;
		orFactDe = 1;
		string previous;
		while(pOr!=NULL){
				struct ComparisonOp *pComp = pOr->left;
				double tLeft = 1.0,tRight = 1.0;
				if(!checkAtt(pComp->left, tLeft, relNames, numToJoin) || !checkAtt(pComp->right, tRight, relNames, numToJoin))
					return 0;
				if (pComp->code == 1 || pComp->code == 2){
					orFact += 1.0/3;
					orFactDe *= 1.0/3;
					pOr = pOr->rightOr;
						continue;
				}

				string oper1(pComp->left->value);
				if(oper1.compare(previous)!=0 && orFactPartial != 0){
						orFact += orFactPartial;
						orFactDe *= orFactPartial;
						orFactPartial = 0;						
				}
				orFactPartial += 1.0/max(tLeft, tRight);
				previous = oper1;
				pOr = pOr->rightOr;
        }
        if(orFactPartial != 0){
				orFact += orFactPartial;
				orFactDe *= orFactPartial;
				orFactPartial = 0;				
		}
		if(orFact!=orFactDe){
			fact *= (orFact - orFactDe);
		}
		else{
			fact *=  orFact;
		}
        pA = pA->rightAnd;
    }
  
    long double maxTup = 1;
    vector<bool> reltable(numToJoin,true);
    for(int i=0; i<numToJoin; i++){
		if(!reltable[i]){
			continue;
		}
			
		string relname(relNames[i]);
		
		for(auto iter = estimate.begin(); iter!=estimate.end(); iter++){
            auto check=split(iter->first);
			if(check.count(relNames[i])){				
				reltable[i] = false;
				maxTup *= iter->second;
				
				for(int j=0; j<numToJoin; j++){
					if(check.count(relNames[j])){
						reltable[j] = false;
					}
						
				}
                break;
            }
        }
	 }
    return fact * maxTup;
}

int Statistics::ParseRelation(string name, string &rel) {
	int prefixPos = name.find(".");
	if(prefixPos != string::npos) {
		rel = name.substr(0,prefixPos);
	}
	return 1;
}
	// } else {
	// 	//suppkey
	// 	unordered_map <string, vector<string>*>::iterator crIt =
	// 			this->m_att_to_rel.find(name);
	// 	if(crIt == this->m_att_to_rel.end() || crIt->second->size()<1) {
	// 		cerr <<"Atts: " <<name << " can not find its relations."<<endl;
	// 		return 0;
	// 	}
	// 	vector<string> *rels = crIt->second;
	// 	if(rels->size() <=0) {
	// 		cerr <<"Atts: "<<name <<" does not belong to any relations!"<<endl;
	// 		return 0;
	// 	}
	// 	if(rels->size()==1) { //this att belong to only one relation
	// 		rel = rels->at(0);
	// 	} else { //this att belong to more than one relations, like s.id, l.id, not happen in TPCH
	// 		cerr <<"Atts: "<<name<<" is ambiguous!"<<endl;
	// 		return 0;
	// 	}
	// }
// 	return 1;
// }

std::ostream& operator<<(std::ostream& os, const Rel& relation){
    os<<relation.numAttr<<" "<<relation.attr.size()<<" ";
    for(auto i:relation.attr){
        os<<i.first<<" "<<i.second<<" ";
    }
    return os;
}

std::istream& operator>>(std::istream& is, Rel& Rel){
    int size;
    is>>Rel.numAttr>>size;
    for(int i=0;i<size;i++){
        string temp;
        is>>temp;
        is>>Rel.attr[temp];
    }
    return is;
}

std::ostream& operator<<(std::ostream& os, const Statistics& stat){
    os<<stat.relation.size()<<" ";
    for(auto iter:stat.relation)
        os<<iter.first<<" "<<iter.second;
    os<<save_two_values<unordered_map<string,double>>(stat.estimate);
    return os;
}

std::istream& operator>>(std::istream& is, Statistics& stat){
    int size;
    is>>size;
    for(int i=0;i<size;i++){
        string temp;
        struct Rel tempRel;
        is>>temp>>tempRel;
        stat.relation[temp]=tempRel;
        //stat.estimate[temp]=tempRel.numAttr;
    }
    is>>size;
    for(int i=0;i<size;i++){
        string temp;
        double dbl;
        is>>temp>>dbl;
        stat.estimate[temp]=dbl;
    }
    return is;
}

void Statistics::LoadAllStatistics() {
		char *relName[] = {(char*)"supplier",(char*)"partsupp", (char*)"lineitem",
					(char*)"orders",(char*)"customer",(char*)"nation", (char*)"part", (char*)"region"};
		AddRel(relName[0],10000);     //supplier
	    AddRel(relName[1],800000);    //partsupp
	    AddRel(relName[2],6001215);   //lineitem
	    AddRel(relName[3],1500000);   //orders
	    AddRel(relName[4],150000);    //customer
	    AddRel(relName[5],25);        //nation
	    AddRel(relName[6], 200000);   //part
	    AddRel(relName[7], 5);        //region
//	    AddRel(relName[8], 3);        //mal_test


	    AddAtt(relName[0], (char*)"s_suppkey",10000);
	    AddAtt(relName[0], (char*)"s_nationkey",25);
		AddAtt(relName[0], (char*)"s_acctbal",9955);
		AddAtt(relName[0], (char*)"s_name",100000);
		AddAtt(relName[0], (char*)"s_address",100000);
		AddAtt(relName[0], (char*)"s_phone",100000);
		AddAtt(relName[0], (char*)"s_comment",10000);


	    AddAtt(relName[1], (char*)"ps_suppkey", 10000);
	    AddAtt(relName[1], (char*)"ps_partkey", 200000);
	    AddAtt(relName[1], (char*)"ps_availqty", 9999);
	    AddAtt(relName[1], (char*)"ps_supplycost", 99865);
	    AddAtt(relName[1], (char*)"ps_comment", 799123);


	    AddAtt(relName[2], (char*) "l_returnflag",3);
	    AddAtt(relName[2], (char*)"l_discount",11);
	    AddAtt(relName[2], (char*)"l_shipmode",7);
	    AddAtt(relName[2], (char*)"l_orderkey",1500000);
	    AddAtt(relName[2], (char*)"l_receiptdate",0);
	    AddAtt(relName[2], (char*)"l_partkey",200000);
	    AddAtt(relName[2], (char*)"l_suppkey",10000);
	    AddAtt(relName[2], (char*)"l_linenumbe",7);
	    AddAtt(relName[2], (char*)"l_quantity",50);
	    AddAtt(relName[2], (char*)"l_extendedprice",933900);
	    AddAtt(relName[2], (char*)"l_tax",9);
	    AddAtt(relName[2], (char*)"l_linestatus",2);
	    AddAtt(relName[2], (char*)"l_shipdate",2526);
	    AddAtt(relName[2], (char*)"l_commitdate",2466);
	    AddAtt(relName[2], (char*)"l_shipinstruct",4);
	    AddAtt(relName[2], (char*)"l_comment",4501941);


	    AddAtt(relName[3], (char*)"o_custkey",150000);
	    AddAtt(relName[3], (char*)"o_orderkey",1500000);
	    AddAtt(relName[3], (char*)"o_orderdate",2406);
	    AddAtt(relName[3], (char*)"o_totalprice",1464556);
	    AddAtt(relName[3], (char*)"o_orderstatus", 3);
	    AddAtt(relName[3], (char*)"o_orderpriority", 5);
	    AddAtt(relName[3], (char*)"o_clerk", 1000);
	    AddAtt(relName[3], (char*)"o_shippriority", 1);
	    AddAtt(relName[3], (char*)"o_comment", 1478684);


	    AddAtt(relName[4], (char*)"c_custkey",150000);
	    AddAtt(relName[4], (char*)"c_nationkey",25);
	    AddAtt(relName[4], (char*)"c_mktsegment",5);
	    AddAtt(relName[4], (char*)"c_name", 150000);
	    AddAtt(relName[4], (char*)"c_address", 150000);
	    AddAtt(relName[4], (char*)"c_phone", 150000);
	    AddAtt(relName[4], (char*)"c_acctbal", 140187);
	    AddAtt(relName[4], (char*)"c_comment", 149965);

	    AddAtt(relName[5], (char*)"n_nationkey",25);
	    AddAtt(relName[5], (char*)"n_regionkey",5);
	    AddAtt(relName[5], (char*)"n_name",25);
	    AddAtt(relName[5], (char*)"n_comment",25);

	    AddAtt(relName[6], (char*)"p_partkey",200000);
	    AddAtt(relName[6], (char*)"p_size",50);
	    AddAtt(relName[6], (char*)"p_container",40);
	    AddAtt(relName[6], (char*)"p_name", 199996);
		AddAtt(relName[6], (char*)"p_mfgr", 5);
		AddAtt(relName[6], (char*)"p_brand", 25);
		AddAtt(relName[6], (char*)"p_type", 150);
		AddAtt(relName[6], (char*)"p_retailprice", 20899);
		AddAtt(relName[6], (char*)"p_comment", 127459);


	    AddAtt(relName[7], (char*)"r_regionkey",5);
	    AddAtt(relName[7], (char*)"r_name",5);
	    AddAtt(relName[7], (char*)"r_comment",5);

}