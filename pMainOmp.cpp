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
    int count=0;
    auto Compare = [](Node a, Node b) { return a.dist <b.dist ;};
    int sourceNode=0,destination=6;
    int numVertices=7;
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
    vector<int>prev(numVertices,-1);
    vector<Node>myMinVec;
    vector<int>dist(numVertices,INT_MAX);
    int globalMinimumDistance;
    int globalMinimumVertex;
    int k;
    int currVertexWeight;
    bool foundMinUnexpl;

    dist[sourceNode]=0;

    #pragma omp parallel private(myId, myFirstVertexIndex, myLastVertexIndex, myMinVec, k, foundMinUnexpl,currVertexWeight) shared(prev,dist,sourceNode, destination, explored, numThreads, globalMinimumDistance,globalMinimumVertex)
    {
        omp_set_num_threads(7);
        myId=omp_get_thread_num();
        numThreads=omp_get_num_threads();

        #pragma omp single
        {
            globalMinimumDistance=INT_MAX;
            globalMinimumVertex=0;
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

            // foundMinUnexpl=false;
            //find minimum unexplored node

            for(k=0;k<myMinVec.size();k++){
                Node v=myMinVec[k];
                // #pragma omp critical
                // {
                //     cout<<"VERTEX ID IS: "<<v.id<<endl;
                // }

                if(explored[v.id]==false){
                    // cout<<"FOUND UNEXPLORED NODE"<<endl;
                    //  #pragma omp single
                    // {
                    //     // cout<<"VERTEX ID IS: "<<myId<<endl;
                    //     cout<<"EXPLORED VECTOR IS: ";
                    //     for(auto mock=0;mock<explored.size();mock++){
                    //         cout<<explored[mock]<<" ";
                    //     }
                    //     cout<<endl;
                    // }
                    // #pragma omp critical
                    // {
                    //     cout<<"ID: "<<v.id<<" DIST: "<<v.dist<<" GLOB DIST: "<<globalMinimumDistance<<" for count: "<<count<<endl;
                    // }
                    if(v.dist<globalMinimumDistance){
                        // cout<<"FOUND SMALLER DISTANCE WITH ID: "<<v.id<<" AND EXPLORED STATUS OF: "<<explored[v.id]<<endl;
                        #pragma omp critical
                        {
                            globalMinimumDistance=v.dist;
                            // cout<<"GLOBAL MINIMUM DISTANCE IS: "<<globalMinimumDistance<<endl;
                            globalMinimumVertex=v.id;
                        }
                        // foundMinUnexpl=true;
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

            // #pragma omp single
            // {
            //     for(auto mock=0;mock<explored.size();mock++){
            //         cout<<explored[mock]<<" ";
            //     }
            //     cout<<endl;
            // }

            //Check the local vectors in myMinVec
            for(int vertexIndex=0;vertexIndex<myMinVec.size();vertexIndex++){
                currVertexWeight=adjMatrix[globalMinimumVertex][myMinVec[vertexIndex].id];

                // #pragma omp critical
                // {
                //     cout<<"My current vertex weight is: "<<currVertexWeight<<" for node: "<<myMinVec[vertexIndex].id<<endl;
                // }
                // cout<<currVertexWeight<<endl;
                if(currVertexWeight>0){
                // if dist[v]+len(v,w)<dist[w]:

                    // cout<<"THE DISTANCE FOR GLOBAL MIN VERTEX IS "<<dist[globalMinimumVertex]<<endl;
                    if(dist[globalMinimumVertex]+currVertexWeight<dist[myMinVec[vertexIndex].id]){
                        // cout<<"FOUND LOWER DISTANCE"<<endl;
                        // dist[w]=dist[v]+len(v,w)
                        #pragma omp critical
                        {
                            //Update weight globally
                            dist[myMinVec[vertexIndex].id]=dist[globalMinimumVertex]+currVertexWeight;
                            // cout<<"UPDATED DIST IS: "<<dist[myMinVec[vertexIndex].id]<<endl;
                            //Update global path
                            prev[myMinVec[vertexIndex].id]=globalMinimumVertex;
                        }
                        //Update weight locally
                        myMinVec[vertexIndex].dist=dist[globalMinimumVertex]+currVertexWeight;
                        // cout<<"LOCALLY UPDATED DISTANCE: "<<myMinVec[vertexIndex].dist<<" FOR VERTEX ID: "<<myMinVec[vertexIndex].id<<" for thread id: "<<myId<<endl;
                        //Sort the local vector
                        sort(myMinVec.begin(),myMinVec.end(),Compare);


                    }
                }
            }

            #pragma omp barrier

            #pragma omp single
            {
                //Reset global minimum distance
                globalMinimumDistance=INT_MAX;
            }
            count++;
            // #pragma omp single
            // {
            //     for(auto mock=0;mock<explored.size();mock++){
            //         cout<<explored[mock]<<" ";
            //     }
            //     cout<<endl;
            // }
        }

    }

    //TODO: cout the result
    cout<<"The shortest path from: "<<sourceNode<<" to node: "<<destination<<" is "<<dist[destination]<<endl;
    cout<<"The path to the destination to the source node is: ";
    int currNode=destination;
    while(currNode!=-1){
        cout<<currNode<<" ";
        currNode=prev[currNode];
    }
    cout<<endl;

    return 0;
}