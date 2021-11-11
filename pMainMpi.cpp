#include<iostream>
#include<limits.h>
#include<sstream>
#include<vector>

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
    int numVertices=4;
    string line,tmp;

    vector<vector<int>>adjMatrix(numVertices);
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
    return 0;


}