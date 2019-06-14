#include <iostream>
#include "network_manager.h"
#include "gplot.h"
#include "path.h"

#include <algorithm>
#include <unistd.h>
#include <queue>
#include <map>

using namespace std;

// create NetworkManager first
NetworkManager *nm = new NetworkManager();

map<string, vector<Edge *>> adv;
vector<Edge *> dfs_paths;
vector<Edge *> existed_edges;
vector<Edge *> dfs_paths_no;

string double_connect_head;  
string double_connect_tail;
string initial_point;

void dfs(string src);

int main(int argc, char** argv){
	
	cout<<"initial_point: "<<endl;							//enter the initial point
	cin>>initial_point;
	
    /* start your program */
	
	nm->interpret("topo.txt");                              //read "topo.txt"

    Edge *elist_org = nm->elist;
	
	Gplot *gp_in = new Gplot();								//generate inpit graph
    gp_in->gp_add(elist_org);
    gp_in->gp_dump(true);
    gp_in->gp_export("graph_in");
	
	nm->print_all_v();										//print vertices
	nm->print_all_e();										//print edges
	
	int num_node = 0;
	
	Vertex **vlist = nm->vlist;
	
	while(vlist[num_node] != NULL){							//count vertex number
		num_node = num_node + 1;
	}
	cout<<"num_node: "<< num_node <<endl;
		
	
	int num_vertice[num_node];

	
	for (int i=0;i<num_node;i++){
		num_vertice[i] = 0;
	}		
		
	while(elist_org!=NULL){									//count each of edge->head->name number 
	    cout << elist_org->head->name << endl;
		for (int i=0;i<num_node;i++){
			if(elist_org->head->name == vlist[i]->name){
				num_vertice[i] = num_vertice[i] + 1;
			}
		}
			elist_org=elist_org->next;
	}
	
	for (int i=0;i<num_node;i++){							
		cout << vlist[i]->name <<"出現次數: " << num_vertice[i] << endl;
	}	
	
	for (int i=0;i<num_node;i++){							//judge even and odd verticies && connect odd verticies
		for (int j=i+1;j<num_node;j++){
			if(((num_vertice[i] % 2 == 1) && (num_vertice[j] % 2 == 1)) && (vlist[i]->name != vlist[j]->name)){
		       nm->connect_r(vlist[i]->name, vlist[j]->name);
		       double_connect_head = vlist[i]->name;
		       double_connect_tail = vlist[j]->name;
			}
		}
	}
	
	cout<<"double_connect"<<endl;
	cout<<"("<<double_connect_head<<","<<double_connect_tail<<")"<<endl<<endl;
	
	Edge *elist_re = nm->elist;
	
	/*Gplot *gp_re = new Gplot();
    gp_re->gp_add(elist_re);
    gp_re->gp_dump(true);
    gp_re->gp_export("graph_re");*/
	
	 // build the edges belong to vertex 
	while(elist_re!=NULL){									
        adv[elist_re->head->name].push_back(elist_re);
        elist_re=elist_re->next;
    }
	
	elist_re = nm->elist;
	
	while(elist_re!=NULL){									//cout adv
		for(int i=0;i<adv[elist_re->head->name].size();i++){
			cout<<"("<<adv[elist_re->head->name].at(i)->head->name<<","<<adv[elist_re->head->name].at(i)->tail->name<<")";
		}
			
        cout<<endl;
        elist_re=elist_re->next;
    }	
	
	cout<<endl;
	
	//first_point = "b";
	
    dfs(initial_point);

    Edge *dfs_elist=NULL, *tmp= dfs_elist;

    cout << "DFS: " << endl;
    for(int i=0;i<dfs_paths.size();i++){
        cout << dfs_paths.at(i)->head->name << "->" << dfs_paths.at(i)->tail->name << endl;
        if(tmp==NULL){
            dfs_elist = new Edge(dfs_paths.at(i));
            tmp = dfs_elist;
        } else {
            tmp->next = new Edge(dfs_paths.at(i));
            tmp = tmp->next;
        }
    }
	
    Gplot *gp = new Gplot();
    gp->gp_add(dfs_elist);
    gp->gp_dump(true);
    gp->gp_export("graph_out");
	
	
    ofstream out("main.txt");  								//output "main.txt"
    for(int i=0;i<dfs_paths.size();i++){
        out << dfs_paths.at(i)->head->name << "->" << dfs_paths.at(i)->tail->name << endl;
	}
    out.close(); 

    return 0;
}

int num = 0; 
int num_count = 2;

void dfs(string src){
	
    for(int i=0;i<adv[src].size();i++){
		num = num + 1;
		cout<< endl << "num: " << num  <<endl;
		
		if(find(dfs_paths_no.begin(), dfs_paths_no.end(), adv[src].at(i))==dfs_paths_no.end()){		//if existing reverse of dfs_paths, and it won't run.
		
        if(find(dfs_paths.begin(), dfs_paths.end(), adv[src].at(i))==dfs_paths.end()){
				
				if(dfs_paths.size()>1){																//pop the wrong paths
					if(dfs_paths.back()->head->name == adv[src].at(i)->head->name){
					dfs_paths.pop_back();
					dfs_paths_no.pop_back();
					}
				}
				if(dfs_paths.size()>1){
					if(dfs_paths.back()->tail->name == adv[src].at(i)->head->name){
						num_count = num_count + 1;
						cout<<"count: "<< num_count <<endl<<endl;
					}
					else{
						num_count = 0;
						cout<<"count: "<< num_count <<endl<<endl;
					}
				}
				
				if(dfs_paths.size()>1){																//pop the wrong paths
					if(num_count == 0){
						while(dfs_paths.back()->tail->name != adv[src].at(i)->head->name){
					    dfs_paths.pop_back();
					    dfs_paths_no.pop_back();
					    }
					}	
				}
																									//if it is odd vertex ,pushing same direction of edge back to dfs_paths_no																							
				if(((adv[src].at(i)->head->name == double_connect_head) && (adv[src].at(i)->tail->name == double_connect_tail))  ||  ((adv[src].at(i)->head->name == double_connect_tail) && (adv[src].at(i)->tail->name == double_connect_head))){
			
				Edge *elist_re = nm->elist;
				while(elist_re!=NULL){
					if((elist_re->head->name==adv[src].at(i)->head->name) && (elist_re->tail->name==adv[src].at(i)->tail->name)){
						dfs_paths_no.push_back(elist_re);
					}
						
					elist_re = elist_re->next;
				}	
				}																					
				else{																				//else if it is even vertex ,pushing reverse of edge back to dfs_paths_no
			
				Edge *elist_re = nm->elist;
				while(elist_re!=NULL){
					if((elist_re->head->name==adv[src].at(i)->tail->name) && (elist_re->tail->name==adv[src].at(i)->head->name)){
						dfs_paths_no.push_back(elist_re);
					}
						
					elist_re = elist_re->next;
				}	
				}
				
				cout<<"dfs_paths_no: "<<endl;
				for(int i=0;i<dfs_paths.size();i++){
					cout<<"("<<dfs_paths_no[i]->head->name<<","<<dfs_paths_no[i]->tail->name<<")"<<endl;
				}
				
				dfs_paths.push_back(adv[src].at(i));
				
				cout<<"dfs_paths: "<<endl;
					for(int i=0;i<dfs_paths.size();i++){
						cout<<"("<<dfs_paths[i]->head->name<<","<<dfs_paths[i]->tail->name<<")"<<endl;
				}

                dfs(adv[src].at(i)->tail->name);
			}
		}
    }
}
