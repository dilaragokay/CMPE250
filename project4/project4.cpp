/*
Student Name: DİLARA GOKAY
Student Number: 2014400102
Project Number: 4
Operating System: XUBUNTU
Compile Status: SUCCESS
Program Status: SUCCESS
*/

#include <iostream>
#include <string>
#include <algorithm>
#include <vector>
#include <fstream>
#include <stdio.h>
#include <stdlib.h>
#include <queue>

using namespace std;

struct Edge;
int N, M;
vector<Edge> edges;
vector<Edge> inMST;
int* contains;
int totalTime;

class DisjointSet{
public:
    int SIZE;
    int* arr;
    DisjointSet(int size){
        SIZE = size;
        arr = new int[SIZE];
        // Make each node a root.
        for (int i = 0; i < SIZE; ++i) {
            arr[i] = -1;
        }
    }
    int find(int set){
        if(arr[set] < 0){
            // Means this is a root
            return set;
        }else{
            // Call find with its root.
            return find(arr[set]);
        }
    }
    void Union(int set1,int set2){
        set1 = find(set1);
        set2 = find(set2);
        if(arr[set1] > arr[set2]){
            // First one is less deep, so make its root second.
            arr[set1] = set2;
        }else if(arr[set1] < arr[set2]){
            // Vice versa
            arr[set2] = set1;
        }else{
            // They are equal in depth. Set first one as second one's root. (Arbitrarily chosen)
            // And increase the depth.
            arr[set1]--;
            arr[set2] = set1;
        }
        if(contains[set1] == 1){
            contains[set2] = 1;
        }
        if(contains[set2] == 1){
            contains[set1] = 1;
        }
    }
};

struct Edge {
    int n1;
    int n2;
    double w;
    Edge(int n1_, int n2_, double w_) {
        n1 = n1_;
        n2 = n2_;
        w = w_;
    }
};

priority_queue<Edge> edgee;

struct Vertex{
    int value;
    bool added;
    Vertex(int value_){
        this->value = value_;
        added = false;
    }
};

vector<Vertex> verticestoDelete;

bool operator<(const Edge& e1, const Edge& e2) {
    return e1.w < e2.w;
}

ostream& operator<<(ostream& os, const Edge& e) {
    os << e.n1 << "---" << e.n2 << "\tweight=" << e.w;
    return os;
}

void read_input(char* argv[]) {

    fstream fin(argv[1]);
    fin >> N >> M;
    int n1, n2, w;
    contains = new int[N];
    for(int i=0; i<N-1; i++) {
        fin >> n1 >> n2 >> w;
        edgee.push(Edge(n1, n2, w));
    }
    for(int i = 0; i<M;i++){
        fin >> n1;
        verticestoDelete.push_back(Vertex(n1));
        contains[n1] = 1;
    }
    fin.close();
}

void write_input(char* argv[]){
    ofstream outfile;
    outfile.open(argv[2]);
    outfile << totalTime;
}

void kruskal() {
    DisjointSet ds(N);
    int numofCuts = 0;
    vector<int> seenVertices;
    while (numofCuts<M-1) {             //until it reaches the number of edges to cut
        int root1 = ds.find(edgee.top().n1);
        int root2 = ds.find(edgee.top().n2);
        if(contains[root1] == 1 && contains[root2] == 1){
            numofCuts++;
            totalTime += edgee.top().w;
        }
        else if (root1 != root2){
            ds.Union(root1,root2);
        }
        edgee.pop();
    }
}

int main(int argc, char *argv[]) {
    read_input(argv);
    kruskal();
    write_input(argv);
    return 0;
}
