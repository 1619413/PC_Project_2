#include<iostream>
#include<vector>
#include<sstream>
#include<limits.h>
#include<algorithm>
#include<set>
#include<fstream>
#include<chrono>
#include<random>
#include<omp.h>

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

int main(){
    int numNodes;
    vector<vector<int>>adjMatrix;
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

    int numVertices=numNodes;
    auto Compare = [](Node a, Node b) { return a.dist <b.dist ;};
    int sourceNode=0,destination=numVertices-1;


    // //Initialize the adj matrix
    // for(int i=0;i<numVertices;i++){
    //     getline(cin,line);
    //     stringstream s(line);

    //     s>>tmp;
    //     for(int j=0;j<numVertices;j++){
    //         adjMatrix[i].push_back((int)(tmp[j])-48);
    //     }


    // }

    //Print the adj matrix
    // for(int i=0;i<numVertices;i++){
    //     for(int k=0;k<numVertices;k++){
    //         cout<<adjMatrix[i][k]<<" ";
    //     }
    //     cout<<endl;
    // }
    genRandomGraph(480);
    double serialStarttime, serialEndtime, parallelStarttime, parallelEndtime;
    int serialShortestDist;
    int myId;
    int myFirstVertexIndex;
    int myLastVertexIndex;
    int numThreads;
    vector<bool>explored(numVertices,false);
    vector<int>prev(numVertices,-1);
    vector<Node>myMinVec;
    vector<int>dist(numVertices,INT_MAX);
    int globalMinimumDistance;
    int globalMinimumVertex;
    int k;
    int currVertexWeight;
    bool foundMinUnexpl;

    dist[sourceNode]=0;

    #pragma omp parallel private(myId, myFirstVertexIndex, myLastVertexIndex, myMinVec, k, foundMinUnexpl,currVertexWeight) shared(prev,dist,sourceNode, destination, explored, numThreads, globalMinimumDistance,globalMinimumVertex,serialStarttime, serialEndtime, parallelStarttime, parallelEndtime)
    {
        // omp_set_num_threads(7);
        myId=omp_get_thread_num();
        numThreads=omp_get_num_threads();

        #pragma omp single
        {
            globalMinimumDistance=INT_MAX;
            globalMinimumVertex=0;
            parallelStarttime=omp_get_wtime();
        }

        myFirstVertexIndex=(myId*numVertices)/numThreads;
        myLastVertexIndex=((myId+1)*numVertices)/numThreads-1;

        //Copy locally assigned vertices to local vector
        for(int i=myFirstVertexIndex;i<=myLastVertexIndex;i++){
            myMinVec.push_back(Node(i,dist[i]));
        }
        //sort the local vector
        sort(myMinVec.begin(),myMinVec.end(),Compare);

        while(!explored[destination]){//!explored[destination]

            #pragma omp single
            {
                //Reset global minimum distance
                globalMinimumDistance=INT_MAX;
            }
            //find minimum unexplored node
            for(k=0;k<myMinVec.size();k++){
                Node v=myMinVec[k];

                if(explored[v.id]==false){

                    if(v.dist<globalMinimumDistance){

                        #pragma omp critical
                        {
                            globalMinimumDistance=v.dist;
                            globalMinimumVertex=v.id;
                        }
                    }
                }

            }

            #pragma omp barrier

            #pragma omp single
            {
                if(globalMinimumVertex!=-1){
                    explored[globalMinimumVertex]=true;
                }
            }

            //Check the local vectors in myMinVec
            for(int vertexIndex=0;vertexIndex<myMinVec.size();vertexIndex++){
                currVertexWeight=adjMatrix[globalMinimumVertex][myMinVec[vertexIndex].id];

                if(currVertexWeight>0){
                    // if dist[v]+len(v,w)<dist[w]:
                    if(dist[globalMinimumVertex]+currVertexWeight<dist[myMinVec[vertexIndex].id]){
                        // dist[w]=dist[v]+len(v,w)
                        #pragma omp critical
                        {
                            //Update weight globally
                            dist[myMinVec[vertexIndex].id]=dist[globalMinimumVertex]+currVertexWeight;
                            //Update global path
                            // prev[myMinVec[vertexIndex].id]=globalMinimumVertex;
                        }
                        //Update weight locally
                        myMinVec[vertexIndex].dist=dist[globalMinimumVertex]+currVertexWeight;
                        //Sort the local vector
                        sort(myMinVec.begin(),myMinVec.end(),Compare);


                    }
                }
            }

            #pragma omp barrier
        }
        #pragma omp single
        {
            //Stop the timer for parallel
            parallelEndtime=omp_get_wtime();

            //Start timer for serial
            serialStarttime=omp_get_wtime();
            //Run the serial implementation
            serialShortestDist=serialDijkstra(adjMatrix,numVertices,sourceNode,destination);
            //Stop the timer for serial
            serialEndtime=omp_get_wtime();
        }

    }

    //Compare the distances
    cout<<"PARALLEL: Took "<<parallelEndtime-parallelStarttime<<" seconds to complete"<<endl;
    cout<<"SERIAL: Took "<<serialEndtime-serialStarttime<<" seconds to complete"<<endl;
    cout<<"SPEEDUP: "<<(serialEndtime-serialStarttime)/(parallelEndtime-parallelStarttime)<<endl;

    verify(globalMinimumDistance,serialShortestDist);

    // cout<<"The shortest path from: "<<sourceNode<<" to node: "<<destination<<" is "<<dist[destination]<<endl;
    // cout<<"The path to the destination to the source node is: ";
    // int currNode=destination;
    // while(currNode!=-1){
    //     cout<<currNode<<" ";
    //     currNode=prev[currNode];
    // }
    // cout<<endl;

    return 0;
}