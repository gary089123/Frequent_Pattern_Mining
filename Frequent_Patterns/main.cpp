//
//  main.cpp
//  Frequent_Patterns
//
//  Created by Gary on 2018/3/27.
//  Copyright © 2018年 Gary. All rights reserved.
//

#include <iostream>
#include <cstdio>
#include <sstream>
#include <vector>
#include <stack>
#include <thread>
#include <algorithm>
#include <iomanip>
#include <fstream>
#include <ctime>

using namespace std;
float min_sup =0;
class node{
public:
    int item;
    int value;
    node *next = NULL;
    node *parent = NULL;
    vector<node*> childs;
    node(int item){
        this->item = item;
        this->value = 1;
    }
};

class tree{
    
public:
    node root = node(-1);
    vector<pair<pair<int,int>,pair<node*,node*>>> HeaderTable;
    tree(vector<pair<int, int>>HeaderTable){
        for (vector<pair<int, int>>::iterator it = HeaderTable.begin(); it != HeaderTable.end(); it++) {
            if ( it->second>=min_sup) {
                this->HeaderTable.push_back(pair<pair<int,int>,pair<node*,node*>>(*it,pair<node*,node*>(NULL,NULL)));
            }
        }
    }
    void growth(node *parent,vector<int>transaction,int i){
        if (i!=transaction.size()) {
            bool has_child = false;
            for (int j = 0; j<parent->childs.size(); j++) {
                if (parent->childs[j]->item==transaction[i]) {
                    parent->childs[j]->value++;
                    growth(parent->childs[j],transaction, i+1);
                    has_child = true;
                    break;
                }
            }
            if (!has_child) {
                node* newchild = new node(transaction[i]);
                (parent->childs).push_back(newchild);
                newchild->parent = parent;
                for (auto it =  HeaderTable.begin(); it != HeaderTable.end(); it++) {
                    if ((*it).first.first == transaction[i]){
                        if ((*it).second.first == NULL){
                            (*it).second.first = newchild;
                        }
                        else{
                            (*it).second.second->next = newchild;
                        }
                        (*it).second.second = newchild;
                        break;
                    }
                }
                
                growth(newchild,transaction, i+1);
            }
        }
    }
    void printHeaderTable(){
        for (auto it = this->HeaderTable.begin(); it!=this->HeaderTable.end(); it++) {
            if ((*it).second.first != NULL) {
                cout << (*it).first.first << ":" << (*it).first.second << " "<< (*it).second.first->item << "->"<<(*it).second.second->item <<endl;
            }
            
            
        }
    }
    void printTree(node* root,int i){
        cout << i <<" "<<root->item <<" "<< root->value<<endl;
        for (auto it = root->childs.begin(); it != root ->childs.end(); it++) {
            printTree(*it,i+1);
        }
    }
};


int mining(tree fptree,vector<int>pattern);
vector<pair<vector<int>,int>> output;

int main(void)
{
    time_t t1 = clock();
    
    int HTable[1000];
    int max=0;
    memset(HTable,0,sizeof(HTable));
    FILE *fPtr;
    fPtr = freopen("sample2.in","r", stdin);
    int trans=0;
    vector<vector<int>> transactions;
    string line;
    while(!getline(cin, line).eof()){
        trans++;
        vector<int> arr;
        istringstream ssline(line);
        string number;
        while(getline(ssline, number, ',')){
            int data = atoi(number.c_str());
            arr.push_back(data);
            HTable[data]++;
            int temp = HTable[data];
            if ( temp > max){
                max = temp ;
            }
        }
        transactions.push_back(arr);
    }
    vector<pair<int,vector<int>>> sort(max+1);
    for (int i=0; i<1000; i++) {
        
        if (HTable[i]!=0) {
            sort[HTable[i]].first++;
            sort[HTable[i]].second.push_back(i);
        }
    }
    vector<pair<int, int>> HeaderTable;
    HeaderTable.reserve(100);
    for(int i=max;i>=0;i--){
        if (i < min_sup) {
            break;
        }
        for(int j=0;j<sort[i].first;j++){
            HeaderTable.push_back(pair<int,int>(sort[i].second[j],i));
        }
    }
    for (auto it = transactions.begin(); it != transactions.end(); it++) {
        vector<int> temp;
        for(int i=0;i<HeaderTable.size();i++){
            for (auto itt = it->begin(); itt != it->end(); itt++) {
                if (HeaderTable[i].first== *itt) {
                    temp.push_back(*itt);
                    break;
                }
            }
        }
        *it = temp;
    }
    for (auto it = transactions.begin(); it != transactions.end(); it++) {
         for (auto itt = it->begin(); itt != it->end(); itt++) {
         }
    }
    min_sup=trans*0.1;
    tree fptree(HeaderTable);
    
    
    for ( auto it = transactions.begin(); it!= transactions.end(); it++) {
        fptree.growth(&fptree.root, *it, 0);
    }
    vector<int>pattern;
    mining(fptree,pattern);
    
    
    std::sort(output.begin(), output.end(), [](const pair<vector<int>,int>& a, const pair<vector<int>,int>& b)
    {
        if (a.first.size() < b.first.size()) {
            return true;
        }
        else if(a.first.size() == b.first.size()){
            int size = (int)a.first.size();
            for (int i=0;i<size;i++){
                if(a.first[i] < b.first[i]){
                    return true;
                }
                else if (a.first[i] > b.first[i]){
                    return  false;
                }
            }
            return false;
            
        }
        else{
            return false;
        }
    } );
    
    ofstream myfile ("example.txt");
    if (myfile.is_open())
    {
        for (auto it = output.begin(); it != output.end(); it++) {
            for (auto itt = (*it).first.begin(); itt != (*it).first.end()-1; itt++) {
                myfile << (*itt) <<",";
            }
            myfile << (*it).first.back() << ":"<<fixed  << setprecision(4) <<(float)(*it).second/(float)trans<<endl;
        }
    }
    else cout << "Unable to open file";
    /*
    cout << "Fequent :"<<output.size() <<endl;
    for (auto it = output.begin(); it != output.end(); it++) {
        for (auto itt = (*it).first.begin(); itt != (*it).first.end()-1; itt++) {
            cout << (*itt) <<",";
        }
        printf("%d:%.4f\n",(*it).first.back(),(float)(*it).second/(float)trans);
    }*/
    time_t t2 = clock();
    cout << "time : "<<(t2-t1)/(double)(CLOCKS_PER_SEC)<<endl;
    return 0;
}


void printConditionalPatternBases(vector<pair<pair<int,vector<pair<vector<int>, int>>>,vector<pair<int,int>>>>ConditionalPatternBases){
    for (auto it = ConditionalPatternBases.begin(); it!= ConditionalPatternBases.end(); it++) {
        cout << (*it).first.first << " | ";
        
        for (auto itt = (*it).first.second.begin(); itt!= (*it).first.second.end(); itt++) {
            cout << "<";
            for (auto ittt = (*itt).first.begin(); ittt!= (*itt).first.end(); ittt++) {
                cout << *ittt<<" ";
            }
            cout <<":"<< (*itt).second;
            cout << "> ";
        }
        cout << "  ";
        for (auto itt = (*it).second.begin() ; itt != (*it).second.end() ; itt++){
            cout << (*itt).first <<":"<<(*itt).second<<" ";
        }
        cout << endl;
    }
}
int c=0;
int mining(tree fptree, vector<int>pattern){
    /*
    cout <<"---------------------------start mining--------------------------"<<endl;
    cout << "pattern : " ;
    for (auto it = pattern.begin(); it != pattern.end(); it++) {
        cout << *it << " ";
    }
    cout <<endl;
    cout << "----->> tree <<-----"<<endl;
    fptree.printTree(&fptree.root, 1);
    cout << "----->> Headertable <<-----"<<endl;
    fptree.printHeaderTable();
    */
    vector<pair<pair<int,vector<pair<vector<int>, int>>>,vector<pair<int,int>>>> ConditionalPatternBases;
    
    for (auto it = fptree.HeaderTable.begin(); it != fptree.HeaderTable.end(); it++) {
        vector<pair<int,int>> list;
        node* leaves = (*it).second.first;
        vector<pair<vector<int>, int>> paths;
        node* node = leaves->parent;
       
        while (node != NULL) {
            
            vector<int> path;
            while (node != NULL){
                path.push_back(node->item);
                
                if (node ->item != -1){
                    for ( auto itt = list.begin(); ; itt++) {
                        if (itt == list.end()) {
                            list.push_back(make_pair(node->item,leaves->value));
                            break;
                        }
                        if(node->item == (*itt).first ) {
                            itt->second+=leaves->value;
                            break;
                        }
                    }
                }
                node = node ->parent;
            }
            paths.push_back(pair<vector<int>, int>(path,leaves->value));
            leaves = leaves->next;
            if ( leaves == NULL) {
                break;
            }
            else{
                node = leaves->parent;
            }
        }
        std::sort(list.begin(), list.end(), [](const pair<int,int>& a, const pair<int,int>& b){ if (a.second > b.second)return true;else return false;});
        ConditionalPatternBases.push_back(make_pair(make_pair((*it).first.first, paths), list));
        
        vector<int> pat;
        pat.push_back((*it).first.first);
        for (auto itt = pattern.begin(); itt != pattern.end(); itt++) {
            pat.push_back(*itt);
        }
        sort(pat.begin(), pat.end());
        
        output.push_back(make_pair(pat, (*it).first.second));
    }
    //cout << "----->> ConPatBase <<-----"<<endl;
    //printConditionalPatternBases(ConditionalPatternBases);
    for (auto it = ConditionalPatternBases.rbegin();  it != ConditionalPatternBases.rend(); it++) {
        
        int frequence = 0;
        for (auto itt = (*it).first.second.begin(); itt!= (*it).first.second.end(); itt++) {
            frequence = frequence + (*itt).second;
        }
            if (!(*it).first.second.empty()) {
                tree cfptree((*it).second);
                
                for (auto itt = (*it).first.second.begin(); itt!= (*it).first.second.end(); itt++) {
                    (*itt).first.pop_back();
                    
                    vector<int> sorted_transaction;
                    for (auto ittt = (*it).second.begin(); ittt!= it->second.end(); ittt++) {
                        for (auto itttt = itt->first.begin();  itttt!= itt->first.end(); itttt++) {
                            if ( ittt->first == *itttt) {
                                sorted_transaction.push_back(ittt->first);
                                break;
                            }
                        }
                    }
                    
                    for ( int i =0;  i<itt->second; i++) {
                        cfptree.growth(&cfptree.root, sorted_transaction, 0);
                    }
                }
                
                vector<int> pa = pattern;
                pa.push_back((*it).first.first);
                //cout <<"-----------------next level mining---------------"<<endl;
                mining(cfptree, pa);
                
            }

    }
     //cout <<"---------------------------end  mining--------------------------"<<endl;
    
    return 0;
}


