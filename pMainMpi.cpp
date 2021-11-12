#include<iostream>
#include<vector>
#include<sstream>
#include<limits.h>
#include<algorithm>
#include<set>
#include<fstream>
#include<chrono>
#include<random>
#include<mpi.h>


using namespace std;

class Node{
    public:
        int id;
        int dist;

    Node(int vertexId, int distance){
        id=vertexId;
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
    // int prev[numNodes];
    // For each vertex v:
    for(int v=0;v<numNodes;v++){
        dist[v]=INT_MAX;
        // prev[v]=-1;
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
                            // Update prev
                            // prev[i]=vId;
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

void verify(int i1,int i2){
    cout<<endl;
    if(i1==i2){
        cout<<"SERIAL AND PARALLEL IMPLEMENTATION OUTPUTS ARE THE SAME"<<endl;
    }else{
        cout<<"SERIAL AND PARALLEL IMPLEMENTATION OUTPUTS ARE NOT THE SAME"<<endl;
    }
}

int main(int argc,char* argv[]){
    //MPI PARALLEL APPROACH
    MPI_Init(&argc,&argv);
    int mpiSize,mpiRank;
    int mpiRoot=0;
    MPI_Comm_size(MPI_COMM_WORLD,&mpiSize);
    MPI_Comm_rank(MPI_COMM_WORLD,&mpiRank);

    double parallelStarttime,parallelEndtime;
    vector<vector<int>>adjMatrix;
    vector<int>dist;
    int numVertices;
    int sourceNode;
    int destination;
    int globalMinimumDistance;
    int globalMinimumVertex;
    int currentVertex;
    int currVertexWeight;


    if(mpiRank==mpiRoot){
        int numNodes;
        string line,tmp;
        genRandomGraph(1200);

            //Taking input from a file
            ifstream myReadFile;
            myReadFile.open("input.txt");
            int rowsInput=0;
            if (myReadFile.is_open()) {
                getline(myReadFile,line);
                stringstream s(line);
                s>>numNodes;
                adjMatrix.resize(numNodes,vector<int>(numNodes,0));

                for(int rowsInput=0;rowsInput<numNodes;rowsInput++){
                    getline(myReadFile,line);
                    stringstream s(line);

                    s>>tmp;
                    for(int colsInput=0;colsInput<numNodes;colsInput++){
                        adjMatrix[rowsInput][colsInput]=(int)(tmp[colsInput])-48;
                    }

                }
            }
            myReadFile.close();

            globalMinimumDistance=INT_MAX;
            globalMinimumVertex=0;
            numVertices=numNodes;
            sourceNode=0;
            destination=numVertices-1;
            //Resize the adjacency matrix based on the number of vertices
            dist.resize(numVertices,INT_MAX);
            dist[sourceNode]=0;

        //Print the adj matrix
        // for(int i=0;i<numVertices;i++){
        //     for(int k=0;k<numVertices;k++){
        //         cout<<adjMatrix[i][k]<<" ";
        //     }
        //     cout<<endl;
        // }

        parallelStarttime=MPI_Wtime();
    }

    auto Compare = [](Node a, Node b) { return a.dist <b.dist ;};

    //Broadcast the main int variables
    MPI_Bcast(&globalMinimumDistance,1,MPI_INT,mpiRoot,MPI_COMM_WORLD);
    MPI_Bcast(&globalMinimumVertex,1,MPI_INT,mpiRoot,MPI_COMM_WORLD);
    MPI_Bcast(&numVertices,1,MPI_INT,mpiRoot,MPI_COMM_WORLD);
    MPI_Bcast(&sourceNode,1,MPI_INT,mpiRoot,MPI_COMM_WORLD);
    MPI_Bcast(&destination,1,MPI_INT,mpiRoot,MPI_COMM_WORLD);

    if(mpiRank!=mpiRoot){
        dist.resize(numVertices,INT_MAX);//local
        dist[sourceNode]=0;
    }

    // vector<int>globalPrev(numVertices,-1);
    // vector<int>localPrev(numVertices,-1);
    vector<Node>myMinVec;//local
    if(mpiRank!=mpiRoot){
        adjMatrix.resize(numVertices,vector<int>(numVertices,0));//local
    }
    vector<int>explored(numVertices,0);//local
    int localMinimumPair[2];
    int globalMinimumPair[2];

    //Send each row to the other nodes
    if(mpiRank==mpiRoot){
        globalMinimumPair[0]=INT_MAX;
        globalMinimumPair[1]=0;
    }
    for(int i=0;i<numVertices;i++){
        MPI_Bcast(&adjMatrix[i][0],numVertices,MPI_INT,mpiRoot,MPI_COMM_WORLD);
    }
    MPI_Bcast(&globalMinimumPair[0],1,MPI_INT,mpiRoot,MPI_COMM_WORLD);
    MPI_Bcast(&globalMinimumPair[1],1,MPI_INT,mpiRoot,MPI_COMM_WORLD);

    int myFirstVertexIndex=(mpiRank*numVertices)/mpiSize;
    int myLastVertexIndex=((mpiRank+1)*numVertices)/mpiSize-1;

    //Copy locally assigned vertices to local vector
    for(int i=myFirstVertexIndex;i<=myLastVertexIndex;i++){
        myMinVec.push_back(Node(i,dist[i]));
    }
    //sort the local vector
    sort(myMinVec.begin(),myMinVec.end(),Compare);

    while(!explored[destination]){

        if(mpiRank==mpiRoot){
            globalMinimumDistance=INT_MAX;
        }
        MPI_Bcast(&globalMinimumDistance,1,MPI_INT,mpiRoot,MPI_COMM_WORLD);
        localMinimumPair[0]=INT_MAX;
        localMinimumPair[1]=-1;
        for(int k=0;k<myMinVec.size();k++){
            Node v=myMinVec[k];

            if(explored[v.id]==0){

                if(v.dist<globalMinimumDistance){
                    //Send to the root node

                    localMinimumPair[0]=v.dist;
                    localMinimumPair[1]=v.id;
                    //Exit out the loop
                    k=myMinVec.size();
                }
            }
        }

        MPI_Allreduce(&localMinimumPair,&globalMinimumPair,1,MPI_2INT,MPI_MINLOC,MPI_COMM_WORLD);

        if(mpiRank==mpiRoot){
            dist[globalMinimumPair[1]]=globalMinimumPair[0];
            explored[globalMinimumPair[1]]=1;
        }
        MPI_Bcast(&dist[0],numVertices,MPI_INT,mpiRoot,MPI_COMM_WORLD);
        MPI_Bcast(&explored[0],numVertices,MPI_INT,mpiRoot,MPI_COMM_WORLD);

        globalMinimumVertex=globalMinimumPair[1];
        globalMinimumDistance=globalMinimumPair[0];
        //Check the local vectors in myMinVec
        for(int vertexIndex=0;vertexIndex<myMinVec.size();vertexIndex++){
            currVertexWeight=adjMatrix[globalMinimumVertex][myMinVec[vertexIndex].id];

            if(currVertexWeight>0){
                // if dist[v]+len(v,w)<dist[w]:
                if(dist[globalMinimumVertex]+currVertexWeight<dist[myMinVec[vertexIndex].id]){
                    // dist[w]=dist[v]+len(v,w)
                    //Update weight locally
                    myMinVec[vertexIndex].dist=dist[globalMinimumVertex]+currVertexWeight;
                    //Sort the local vector
                    sort(myMinVec.begin(),myMinVec.end(),Compare);
                    //update prev
                    // localPrev[myMinVec[vertexIndex].id]=globalMinimumVertex;
                }
            }
        }
    }
    // if(mpiRank!=mpiRoot){
    //     MPI_Send(&localPrev[0],numVertices,MPI_INT,mpiRoot,0,MPI_COMM_WORLD);
    // }else{
    //     for(int otherProcs=1;otherProcs<mpiSize;otherProcs++){
    //         MPI_Recv(&localPrev[0],numVertices,MPI_INT,otherProcs,0,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
    //         for(int i=0;i<numVertices;i++){
    //             if(localPrev[i]!=-1){
    //                 globalPrev[i]=localPrev[i];
    //             }
    //         }
    //     }
    // }


    if(mpiRank==mpiRoot){
        parallelEndtime=MPI_Wtime();
        cout<<endl;
        // cout<<"PARALLEL: The shortest path from: "<<sourceNode<<" to node: "<<destination<<" is "<<globalMinimumDistance<<endl;
        cout<<"PARALLEL: Took "<<parallelEndtime-parallelStarttime<<" seconds to complete"<<endl;

        double serialStarttime, serialEndtime;
        int serialShortestDist;

        serialStarttime=MPI_Wtime();
        serialShortestDist=serialDijkstra(adjMatrix,numVertices,sourceNode,destination);
        serialEndtime=MPI_Wtime();

        // cout<<"SERIAL: The shortest path from: "<<sourceNode<<" to node: "<<destination<<" is "<<serialShortestDist<<endl;
        cout<<"SERIAL: Took "<<serialEndtime-serialStarttime<<" seconds to complete"<<endl;

        cout<<"SPEEDUP: "<<(serialEndtime-serialStarttime)/(parallelEndtime-parallelStarttime)<<endl;

        verify(globalMinimumDistance,serialShortestDist);
    }

    MPI_Finalize();
    return 0;


}