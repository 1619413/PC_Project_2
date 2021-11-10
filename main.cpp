#include<iostream>
#include<vector>
#include<sstream>
#include<limits.h>
#include<algorithm>
#include<set>

using namespace std;

class Node{
    public:
    int id;
    int dist;

    Node(int indentity,int distance){
        id=indentity;
        dist=distance;
    }
};

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

int main(){

    int numNodes=7;
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
    //TODO: ensure the algorithm works with higher weights than 1 for each edge
    auto Compare = [](Node a, Node b) { return a.dist <b.dist ;};
    int sourceNode=0,destination=6;
    vector<Node>minVec;
    // set all vertices to unexplored
    vector<bool>explored(numNodes,false);
    int dist[numNodes];
    int prev[numNodes];
    // For each vertex v:
    for(int v=0;v<numNodes;v++){
        dist[v]=INT_MAX;
        prev[v]=-1;
    }
    dist[sourceNode]=0;
    //Add all the nodes to the priority queue
    for(int i=0;i<numNodes;i++){
        minVec.push_back(Node(i,dist[i]));
    }
    //TODO: sort the vector
    sort(minVec.begin(),minVec.end(),Compare);

    // while desination is not explored:
    while(!explored[destination]){
        // v = least-value unexplored vertex
        Node v=minVec[0];
        minVec.erase(minVec.begin());
        // set v to explored
        int vId=v.id;
        explored[vId]=true;
        // for each edge (v,w):
        for(int i=0;i<numNodes;i++){
            if(adjMatrix[vId][i]==1){
            // if dist[v]+len(v,w)<dist[w]:
                if(dist[vId]+1<dist[i]){
                    // dist[w]=dist[v]+len(v,w)
                    dist[i]=dist[vId]+1;
                    //Find the updated node in the vector
                    for(int j=0;j<numNodes;j++){
                        if(minVec[j].id==i){
                            minVec[j].dist=dist[vId]+1;
                        }
                    }
                    // prev[w]=visited
                    prev[i]=vId;
                    //Sort the vector
                    sort(minVec.begin(),minVec.end(),Compare);
                }
            }
        }

    }
    cout<<"The shortest path from: "<<sourceNode<<" to node: "<<destination<<" is "<<dist[destination]<<endl;
    cout<<"The path to the destination to the source node is: ";
    int currNode=destination;
    while(currNode!=-1){
        cout<<currNode<<" ";
        currNode=prev[currNode];
    }



    return 0;
}
