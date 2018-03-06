/*
Student Name: DİLARA GÖKAY
Student Number: 2014400102
Project Number: 5
Operating System: XUBUNTU
Compile Status: SUCCESS
Program Status: SUCCESS
*/

#include <iostream>
#include <vector>
#include <queue>
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <fstream>
#include <algorithm>

using namespace std;

int V, E, source, dest;         //# of vertices, # of edges, ID of source and ID of destination
int* heuristic;                 //heuristic values
int* dist_so_far;               //shortest distance discovered so far from the beginning vertex
int* came_from;                 //ID of vertex came from
bool* visited;
int result;

struct Vertex{
    int id;
    int distance;
    Vertex(int id_){
        id = id_;
        distance = INT_MAX;     //at first, distance to all unvisited vertices are considered as infinite
    }
    Vertex(int id_, int distance_){
        id = id_;
        distance = distance_;
    }
};

struct Edge {
    int to;
    int w;                      //weight(or distance) of edge
    Edge(int to_, int w_) {
        to = to_;
        w = w_;
    }
};

vector<vector<Edge>> graph;

struct compareDistance {
    bool operator()(Vertex& e1, Vertex& e2)
    {
        return e1.distance > e2.distance;
    }
};

bool myComparison (Vertex e1, Vertex e2) {
    return e1.distance < e2.distance;
}

std::priority_queue<Vertex, std::vector<Vertex>, compareDistance> myPQ;

void read_input(char* argv[]){
    FILE *fid;
    fid = fopen(argv[1],"r");
    fscanf(fid,"%d%d",&V,&E);
    int dist, n1, n2, w;
    heuristic = new int[V];
    dist_so_far = new int[V];
    came_from = new int[V];
    visited = new bool[V];
    graph.resize(V);
    for(int i=0; i<V; i++) {
        fscanf(fid,"%d",&dist);
        heuristic[i] = dist;
        dist_so_far[i] = INT_MAX;
        visited[i] = false;
    }
    for(int i = 0; i<E;i++){
        fscanf(fid, "%d%d%d", &n1, &n2, &w);
        graph[n1].push_back(Edge(n2, w));
        graph[n2].push_back(Edge(n1, w));
    }
    fscanf(fid, "%d%d", &source, &dest);
    fclose(fid);
}

void write_input(char* argv[]){
    ofstream outfile;
    outfile.open(argv[2]);
    outfile << result;
}

// below, A* Algorithm is implemented
void aStar(){
    dist_so_far[source] = 0;            //source to source distance is 0
    came_from[source] = -1;             //means source vertex comes from no other vertex
    myPQ.push(Vertex(source, 0));
    int current;
    while(true){
        // below condition is true if shortest path known by that time does not lead to the destination
        if(myPQ.empty()){
            // then we trace back to find the shortest path that will lead to the destination
            int vertexCameFrom = came_from[current];
            myPQ.push(Vertex(vertexCameFrom, dist_so_far[vertexCameFrom]+heuristic[vertexCameFrom]));
        }
        current = myPQ.top().id;
        myPQ.pop();
        visited[current] = true;
        if(current == dest){            // if we find our destination
            result = dist_so_far[current];
            return;                     // where while loop breaks
        }
        Vertex min = Vertex(-1, INT_MAX);                  //minimum of neighbours
        vector<Vertex> minimums;
        // in below for loop, new shortest distances of the neighbours of the current vertex are calculated
        for(int i = 0; i < graph[current].size(); i++){
            int idofneighbour = graph[current][i].to;
            int new_cost = dist_so_far[current] + graph[current][i].w;
            if(!visited[idofneighbour]){
                // if new path is shorter than the known one
                if(new_cost < dist_so_far[idofneighbour]){
                    // then new path is the shortest one
                    dist_so_far[idofneighbour] = new_cost;
                    came_from[idofneighbour] = current;
                }
                // if cost(or length of path) of that neighbour is the shortest one
                if(new_cost + heuristic[idofneighbour] <= min.distance){
                    // then it is the "min" among neighbour of the current vertex
                    min = Vertex(idofneighbour, new_cost + heuristic[idofneighbour]);
                    minimums.push_back(min);
                }
            }
        }
        // below, shortest path and vertex among neighbours is found
        sort(minimums.begin(), minimums.end(), myComparison);
        for(int i = 0; i < minimums.size() && minimums[i].distance == min.distance; i++){
            min = minimums[0];
            myPQ.push(minimums[i]);
        }
    }
}
int main(int argc, char *argv[]){
    read_input(argv);
    aStar();
    write_input(argv);
    return 0;
};
