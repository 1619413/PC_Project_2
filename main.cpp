#include<iostream>
#include<vector>
#include<sstream>
#include<limits.h>
#include<algorithm>
#include<set>
#include<fstream>
#include<chrono>
#include<random>

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
    int row=0;
    int col=0;
    int maxEdges=numNodes*(numNodes-1),currEdges=0;
    float randomNum,p=0.00001, currDensity=0, goalDensity=0.35;
    // bool exitLoop=false;
    unsigned seed = chrono::system_clock::now().time_since_epoch().count();
    default_random_engine e(seed);

    //Setup adjacency matrix
    vector<vector<int>>adjMatrix(numNodes,vector<int>(numNodes,0));
    //Maximum number of edges in our graph n(n-1)/2 -----> use this for density calculation
    for(row=0;row<numNodes;row++){
        // if(exitLoop){
        //     break;
        // }
        for(col=0;col<numNodes;col++){
            randomNum=(float)rand()/RAND_MAX;
            if(randomNum<=p){
                currDensity=(float)currEdges/maxEdges;
                // cout<<currDensity<<endl;
                if(row!=col){//
                    if(currDensity<goalDensity){//Only go through one side of the main diagonal
                        adjMatrix[row][col]=1;
                        adjMatrix[col][row]=1;
                        currEdges+=2;
                    }
                    else{
                        row=numNodes;
                        break;
                    }


                }

            }

        }
        row+=1;
        col+=1;
    }
    //Random walk
    vector<int>randomWalk;
    vector<bool>vistitedRandomWalk(numNodes,false);
    int currentNodeIndex=0;
    for(int randomWalkIndex=0; randomWalkIndex<numNodes;randomWalkIndex++){
        randomWalk.push_back(randomWalkIndex);
    }

    shuffle(randomWalk.begin(),randomWalk.end(),e);

    for(int randomWalkIndex=0;randomWalkIndex<randomWalk.size();randomWalkIndex++){
        if(randomWalk[randomWalkIndex]!=0){
            adjMatrix[currentNodeIndex][randomWalk[randomWalkIndex]]=1;
            adjMatrix[randomWalk[randomWalkIndex]][currentNodeIndex]=1;
            currentNodeIndex=randomWalk[randomWalkIndex];
        }
    }

    // cout<<endl;
    // cout<<"RANDOM GENERATED GRAPH: "<<endl;
    // for(int row=0;row<numNodes;row++){
    //     for(int col=0;col<numNodes;col++){
    //         cout<<adjMatrix[row][col]<<" ";
    //     }
    //     cout<<endl;
    // }

    ofstream myWriteFile;
    myWriteFile.open("input.txt",ofstream::trunc);
    if (myWriteFile.is_open()) {
        myWriteFile<<numNodes<<"\n";

        string line="";

        for(int outRow=0;outRow<numNodes;outRow++){
            for(int outCol=0;outCol<numNodes;outCol++){
                line+=to_string(adjMatrix[outRow][outCol]);
            }
            myWriteFile<<line<<"\n";
            line="";
        }
    }
}

int serialDijkstra(vector<vector<int>>adjacencyMatrix, int numNodes, int sourceVert, int destVert){
    auto Compare = [](Node a, Node b) { return a.dist <b.dist ;};

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
    dist[sourceVert]=0;
    //Add all the nodes to the priority queue
    for(int i=0;i<numNodes;i++){
        minVec.push_back(Node(i,dist[i]));
    }

    sort(minVec.begin(),minVec.end(),Compare);

    // while desination is not explored:
    while(!explored[destVert]){
        // v = least-value unexplored vertex
        for(int k=0;k<numNodes;k++){
            Node v=minVec[k];
            //make sure it is unexplored aswell
            if(explored[v.id]==false){
                minVec.erase(minVec.begin());
                // set v to explored
                int vId=v.id;
                explored[vId]=true;
                // for each edge (v,w):
                for(int i=0;i<numNodes;i++){
                    if(adjacencyMatrix[vId][i]==1){
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
                break;
            }
        }



    }
    // cout<<"The path to the destination to the source node is: ";
    // int currNode=destVert;
    // while(currNode!=-1){
    //     cout<<currNode<<" ";
    //     currNode=prev[currNode];
    // }
    // cout<<endl;

    return dist[destVert];
}

int main(){

    int numNodes;
    vector<vector<int>>adjMatrix;
    string line,tmp;
    // genRandomGraph(50);

    //Taking input from a file
    ifstream myReadFile;
    myReadFile.open("input.txt");
    int rowsInput=0;
    if (myReadFile.is_open()) {
        getline(myReadFile,line);
        stringstream s(line);
        s>>numNodes;
        adjMatrix.resize(numNodes,vector<int>(numNodes,0));
        // cout<<line<<endl;
        // cout<<numNodes<<endl;
        for(int rowsInput=0;rowsInput<numNodes;rowsInput++){
            getline(myReadFile,line);
            stringstream s(line);

            s>>tmp;
            for(int colsInput=0;colsInput<numNodes;colsInput++){
                adjMatrix[rowsInput][colsInput]=(int)(tmp[colsInput])-48;//TODO: NEED TO FIX WHEN WEIGHTS CONSIST OF 2 DIGITS
            }

        }
    }
    myReadFile.close();

    //Print the adj matrix
    // cout<<"The adjacency matrix is: "<<endl;
    // for(int i=0;i<numNodes;i++){
    //     for(int k=0;k<numNodes;k++){
    //         cout<<adjMatrix[i][k]<<" ";
    //     }
    //     cout<<endl;
    // }

    int sourceNode=0,destination=numNodes-1, serialShortestDist;

    serialShortestDist=serialDijkstra(adjMatrix,numNodes,sourceNode,destination);
    cout<<"The shortest path from: "<<sourceNode<<" to node: "<<destination<<" is "<<serialShortestDist<<endl;

    return 0;
}
