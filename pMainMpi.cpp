#include<iostream>
#include<limits.h>
#include<sstream>
#include<vector
#include<algorithm>
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

int main(int argc,char* argv[]){
    auto Compare = [](Node a, Node b) { return a.dist <b.dist ;};

    int numVertices;
    string line,tmp;

    vector<int>dist(numVertices,INT_MAX);//local
    dist[sourceNode]=0;
    vector<Node>myMinVec;//local
    vector<vector<int>>adjMatrix(numVertices);//local
    vector<bool>explored(numVertices,false);//local
    //Initialize the adj matrix
    for(int i=0;i<numVertices;i++){
        getline(cin,line);
        stringstream s(line);

        s>>tmp;
        for(int j=0;j<numVertices;j++){
            adjMatrix[i].push_back((int)(tmp[j])-48);
        }


    }

    //Print the adj matrix
    for(int i=0;i<numVertices;i++){
        for(int k=0;k<numVertices;k++){
            cout<<adjMatrix[i][k]<<" ";
        }
        cout<<endl;
    }

    //MPI PARALLEL APPROACH
    MPI_Init(&argc,&argv);
    int mpiSize,mpiRank;
    int mpiRoot=0;
    MPI_Comm_size(MPI_COMM_WORLD,&mpiSize);
    MPI_Comm_rank(MPI_COMM_WORLD,&mpiRank);

    int globalMinimumDistance;
    int globalMinimumVertex;

    if(mpiRank==mpiRoot){
        globalMinimumDistance=INT_MAX;
        globalMinimumVertex=0;
        numVertices=4;
    }

    //Broadcast the main int variables
    MPI_Bcast(&globalMinimumDistance,1,MPI_INT,mpiRoot,MPI_COMM_WORLD);
    MPI_Bcast(&globalMinimumVertex,1,MPI_INT,mpiRoot,MPI_COMM_WORLD);
    MPI_Bcast(&numVertices,1,MPI_INT,mpiRoot,MPI_COMM_WORLD);
    //Broadcast vectors
    MPI_Bcast(&dist,numVertices,MPI_INT,mpiRoot,MPI_COMM_WORLD);
    MPI_Bcast(&explored,numVertices,MPI_INT,mpiRoot,MPI_COMM_WORLD);
    for(int i=0;i<numVertices;i++){
        MPI_Bcast(&adjMatrix[i],numVertices,MPI_INT,mpiRoot,MPI_COMM_WORLD);
    }


    int myFirstVertexIndex=(mpiRank*numVertices)/mpiSize;
    int myLastVertexIndex=((mpiRank+1)*numVertices)/mpiSize-1;

    //Copy locally assigned vertices to local vector
    for(int i=myFirstVertexIndex;i<=myLastVertexIndex;i++){
        myMinVec.push_back(Node(i,dist[i]));
    }

    //sort the local vector
    sort(myMinVec.begin(),myMinVec.end(),Compare);

    while(!explored[destination]){
        for(int k=0;k<myMinVec.size();k++){
            Node v=myMinVec[k];
        }
    }

    MPI_Finalize();
    return 0;


}