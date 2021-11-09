#include<iostream>
#include<vector>
#include<sstream>
#include<limits.h>

using namespace std;

void genRandomGraph(int numNodes){
    int maxEdges=numNodes*(numNodes-1),currEdges=0;
    float randomNum,p=0.3, currDensity=0, goalDensity=0.35;
    bool exitLoop=false;
    srand(time(0));

    //Setup adjacency matrix
    vector<vector<int>>adjMatrix(numNodes,vector<int>(numNodes,0));
    //Maximum number of edges in our graph n(n-1)/2 -----> use this for density calculation
    for(int row=0;row<numNodes;row++){
        // if(exitLoop){
        //     break;
        // }
        for(int col=0;col<numNodes;col++){
            randomNum=(float)rand()/RAND_MAX;
            if(randomNum<=p){
                // currDensity=(float)currEdges/maxEdges;
                // cout<<currDensity<<endl;
                if(row!=col){//currDensity<goalDensity&&
                    adjMatrix[row][col]=1;
                    adjMatrix[col][row]=1;
                    currEdges+=1;
                }
                // else{
                //     exitLoop=true;
                //     break;
                // }

            }
        }
    }

    //Random walk

    for(int row=0;row<numNodes;row++){
        for(int col=0;col<numNodes;col++){
            cout<<adjMatrix[row][col]<<" ";
        }
        cout<<endl;
    }
}

int minDistance(int dist[], bool sptSet[], int numNodes)
{
    // Initialize min value
    int min = INT_MAX, min_index;

    for (int v = 0; v < V; v++)
        if (sptSet[v] == false && dist[v] <= min)
            min = dist[v], min_index = v;

    return min_index;
}
int main(){

    int numNodes=3;
    vector<vector<int>>adjMatrix(numNodes);
    string line,tmp;

    //Initialize the adj matrix
    for(int i=0;i<numNodes;i++){
        getline(cin,line);
        stringstream s(line);

        s>>tmp;
        for(int j=0;j<numNodes;j++){
            adjMatrix[i].push_back((int)(tmp[j])-48);
        }


    }

    //Print the adj matrix
    for(int i=0;i<numNodes;i++){
        for(int k=0;k<numNodes;k++){
            cout<<adjMatrix[i][k]<<" ";
        }
        cout<<endl;
    }



    // cout<<maxEdges<<endl;


    //SERIAL APPROACH
    vector<int>visited(1,0);//initialize with node 0
    vector<int>weights(numNodes,0);
    // cout<<visited[0]<<endl;
    int nodes=numNodes;

    //For everything we haven't visited
    for(int node=0;node<numNodes;node++){
        //Check if it is connected to the node 0
        if(adjMatrix[0][node]==1){
            weights[node]=1;
        }else{
            weights[node]=1000000;
        }
    }

    while(visited.size()!=nodes){

    }



    return 0;
}
