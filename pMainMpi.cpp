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

int main(int argc,char* argv[]){
    //MPI PARALLEL APPROACH
    MPI_Init(&argc,&argv);
    int mpiSize,mpiRank;
    int mpiRoot=0;
    MPI_Comm_size(MPI_COMM_WORLD,&mpiSize);
    MPI_Comm_rank(MPI_COMM_WORLD,&mpiRank);

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

            globalMinimumDistance=INT_MAX;
            globalMinimumVertex=0;
            numVertices=numNodes;
            sourceNode=0;
            destination=numVertices-1;
            //Resize the adjacency matrix based on the number of vertices
            dist.resize(numVertices,INT_MAX);
            dist[sourceNode]=0;

        // //Initialize the adj matrix
        // for(int i=0;i<numVertices;i++){
        //     getline(cin,line);
        //     stringstream s(line);

        //     s>>tmp;
        //     for(int j=0;j<numVertices;j++){
        //         adjMatrix[i].push_back((int)(tmp[j])-48);//TODO: Fix this so weights can be more than one char
        //     }


        // }

        //Print the adj matrix
        for(int i=0;i<numVertices;i++){
            for(int k=0;k<numVertices;k++){
                cout<<adjMatrix[i][k]<<" ";
            }
            cout<<endl;
        }
    }

    auto Compare = [](Node a, Node b) { return a.dist <b.dist ;};

    //Broadcast the main int variables
    MPI_Bcast(&globalMinimumDistance,1,MPI_INT,mpiRoot,MPI_COMM_WORLD);
    MPI_Bcast(&globalMinimumVertex,1,MPI_INT,mpiRoot,MPI_COMM_WORLD);
    MPI_Bcast(&numVertices,1,MPI_INT,mpiRoot,MPI_COMM_WORLD);
    MPI_Bcast(&sourceNode,1,MPI_INT,mpiRoot,MPI_COMM_WORLD);
    MPI_Bcast(&destination,1,MPI_INT,mpiRoot,MPI_COMM_WORLD);

    // cout<<"DIST SIZE IS: "<<dist.size()<<" FOR RANK: "<<mpiRank<<endl;
    if(mpiRank!=mpiRoot){
        dist.resize(numVertices,INT_MAX);//local
        dist[sourceNode]=0;
        // cout<<"DIST SIZE IS: "<<dist.size()<<" FOR RANK: "<<mpiRank<<endl;
    }

        //   cout<<"THE ELEMENTS IN DIST RANK:"<<mpiRank<<"ARE: ";
        // for(int i=0;i<numVertices;i++){
        //     cout<<dist[i]<<" ";
        // }
        // cout<<endl;
    vector<int>globalPrev(numVertices,-1);
    vector<int>localPrev(numVertices,-1);
    vector<Node>myMinVec;//local
    if(mpiRank!=mpiRoot){
        adjMatrix.resize(numVertices,vector<int>(numVertices,0));//local
    }
    vector<int>explored(numVertices,0);//local
    int localMinimumPair[2];
    int globalMinimumPair[2];

    //Broadcast vectors
    // MPI_Bcast(&dist[0],numVertices,MPI_INT,mpiRoot,MPI_COMM_WORLD);
    // MPI_Bcast(&explored,numVertices,MPI_INT,mpiRoot,MPI_COMM_WORLD);

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

    // cout<<"ADJ MATRIX FOR RANK: "<<mpiRank<<endl;
    // for(int i=0;i<numVertices;i++){
    //     for(int j=0;j<numVertices;j++){
    //         cout<<adjMatrix[i][j]<<" ";
    //     }
    //     cout<<endl;
    // }

    int myFirstVertexIndex=(mpiRank*numVertices)/mpiSize;
    int myLastVertexIndex=((mpiRank+1)*numVertices)/mpiSize-1;

    //Copy locally assigned vertices to local vector
    for(int i=myFirstVertexIndex;i<=myLastVertexIndex;i++){
        myMinVec.push_back(Node(i,dist[i]));
        // cout<<"PUSHED BACK FOR RANK: "<<mpiRank<<endl;
    }
    // cout<<"MY MINVEC SIZE IS: "<<myMinVec.size()<<" FOR RANK: "<<mpiRank<<" WITH VERTEX ID OF "<<myMinVec[0].id<<endl;
    //sort the local vector
    sort(myMinVec.begin(),myMinVec.end(),Compare);

    while(!explored[destination]){
        // cout<<"RANK: "<<mpiRank<<" ENTERED"<<endl;
        if(mpiRank==mpiRoot){
            globalMinimumDistance=INT_MAX;
        }
        MPI_Bcast(&globalMinimumDistance,1,MPI_INT,mpiRoot,MPI_COMM_WORLD);
        localMinimumPair[0]=INT_MAX;
        localMinimumPair[1]=-1;
        for(int k=0;k<myMinVec.size();k++){
            Node v=myMinVec[k];
            // cout<<"HELLO"<<endl;

            if(explored[v.id]==0){
                // cout<<"RANK: "<<mpiRank<<" ENTERED"<<endl;
                if(v.dist<globalMinimumDistance){
                    //Send to the root node

                    localMinimumPair[0]=v.dist;
                    localMinimumPair[1]=v.id;
                    //Exit out the loop
                    k=myMinVec.size();
                }
            }
        }
        // cout<<"GLOBAL MINIMUM VERTEX IS: "<<globalMinimumPair[1]<<" AND THE GLOBAL MINIMUM DIST IS: "<<globalMinimumPair[0]<<endl;
        // cout<<"LOCAL MINIMUM VERTEX IS: "<<localMinimumPair[1]<<" AND THE LOCAL MINIMUM DIST IS: "<<localMinimumPair[0]<<endl;
        MPI_Allreduce(&localMinimumPair,&globalMinimumPair,1,MPI_2INT,MPI_MINLOC,MPI_COMM_WORLD);

        // cout<<"GLOBAL MINIMUM VERTEX IS: "<<globalMinimumPair[1]<<" AND THE GLOBAL MINIMUM DIST IS: "<<globalMinimumPair[0]<<endl;

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
                    localPrev[myMinVec[vertexIndex].id]=globalMinimumVertex;
                }
            }
        }
    }
    if(mpiRank!=mpiRoot){
        MPI_Send(&localPrev[0],numVertices,MPI_INT,mpiRoot,0,MPI_COMM_WORLD);
    }else{
        for(int otherProcs=1;otherProcs<mpiSize;otherProcs++){
            MPI_Recv(&localPrev[0],numVertices,MPI_INT,otherProcs,0,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
            for(int i=0;i<numVertices;i++){
                if(localPrev[i]!=-1){
                    globalPrev[i]=localPrev[i];
                }
            }
        }
    }


    if(mpiRank==mpiRoot){
        cout<<"The shortest path from: "<<sourceNode<<" to node: "<<destination<<" is "<<globalMinimumDistance<<endl;
        cout<<"The path to the destination to the source node is: ";
        int currNode=destination;
        while(currNode!=-1){
            cout<<currNode<<" ";
            currNode=globalPrev[currNode];
        }
        cout<<endl;
    }

    MPI_Finalize();
    return 0;


}