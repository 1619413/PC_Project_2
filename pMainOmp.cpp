#include<iostream>
#include<omp.h>
#include<limits.h>
#include<vector>
#include<sstream>
#include<algorithm>

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

int main(){
    auto Compare = [](Node a, Node b) { return a.dist <b.dist ;};
    int sourceNode=0,destination=3;
    int numVertices=4;
    vector<vector<int>>adjMatrix(numVertices);
    string line,tmp;

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

    int myId;
    int myFirstVertexIndex;
    int myLastVertexIndex;
    int numThreads;
    vector<bool>explored(numVertices,false);
    vector<Node>path;
    vector<Node>myMinVec;
    int dist[numVertices];
    int prev[numVertices];
    int globalMinimumDistance;
    int globalMinimumVertex;
    int k;
    int currVertexWeight;
    bool foundMinUnexpl;
    // For each vertex v:
    for(int v=0;v<numVertices;v++){
        dist[v]=INT_MAX;
        prev[v]=-1;
    }
    dist[sourceNode]=0;
    path.push_back(Node(sourceNode,dist[sourceNode]));

    #pragma omp parallel private(myId, myFirstVertexIndex, myLastVertexIndex, myMinVec, k, foundMinUnexpl) shared(sourceNode, destination, explored, numThreads, globalMinimumDistance,globalMinimumVertex, path)
    {
        omp_set_num_threads(4);
        myId=omp_get_thread_num();
        numThreads=omp_get_num_threads();

        myFirstVertexIndex=(myId*numVertices)/numThreads;
        myLastVertexIndex=((myId+1)*numVertices)/numThreads-1;

        //Copy locally assigned vertices to local vector
        for(int i=myFirstVertexIndex;i<=myLastVertexIndex;i++){
            myMinVec.push_back(Node(i,dist[i]));
        }
        //sort the local vector
        sort(myMinVec.begin(),myMinVec.end(),Compare);

        while(!explored[destination]){
            #pragma omp single
            {
                globalMinimumDistance=INT_MAX;
                globalMinimumVertex=-1;
            }
            foundMinUnexpl=false;
            //find minimum unexplored node
            k=0;
            while(!foundMinUnexpl&&k<myMinVec.size()){
                Node v=myMinVec[k];

                if(explored[v.id]==false){
                    cout<<v.dist<<endl;
                    if(v.dist<globalMinimumDistance){
                        #pragma omp critical
                        {
                            globalMinimumDistance=v.dist;
                            globalMinimumVertex=v.id;
                        }
                        foundMinUnexpl=true;
                    }
                    k++;
                }else{
                    k++;
                }

            }

            #pragma omp barrier

            #pragma omp single
            {
                if(globalMinimumVertex!=-1){
                    explored[globalMinimumVertex]=true;
                }
            }


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
                            path.push_back(Node(globalMinimumVertex,globalMinimumDistance));
                        }
                        //Update weight locally
                        myMinVec[vertexIndex].dist=myMinVec[vertexIndex].dist+currVertexWeight;
                        //Sort the local vector
                        sort(myMinVec.begin(),myMinVec.end(),Compare);
                    }
                }
            }

            #pragma omp barrier
        }

    }

    //TODO: cout the result
    int pathLength=0;
    cout<<"The path to: "<<destination<<" from: "<<sourceNode<<" is: ";
    for(int hops=0;hops<path.size();hops++){
        cout<<path[hops].id<<" ";
        pathLength+=path[hops].dist;
    }
    cout<<endl;
    cout<<"The total distance was: "<<pathLength<<endl;

    return 0;
}