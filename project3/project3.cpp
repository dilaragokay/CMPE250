/*
Student Name: DİLARA GOKAY
Student Number: 2014400102
Project Number: 3
Operating System: XUBUNTU
Compile Status: SUCCESS
Program Status: SUCCESS
*/
#include <iostream>
#include <queue>
#include <algorithm>
#include <fstream>
#include <vector>
#include<thread>
#include <iomanip>
#include <sstream>

using namespace std;

// Processes are vertices and dependencies are edges
// Topological sort algoritm is implemented to solve the problem

struct Vertex{
    int id;                                 //ID of the vertex
    double duration;
    Vertex(int id_, double duration_){
        id = id_;
        duration = duration_;
    }
    Vertex(){
        id = 0;
        duration = 0;
    }
};

// topological sort algorithm w/ adjacency list
void top_sort_using_adj_list(char* argv[]) {
    int N,M;                                //# of processes and dependencies respectively
    fstream fin(argv[1]);
    fin >> N >> M;
    int indegree[N+1];                      //index: ID of vertex, value: indegree
    double timePassed[N+1];
    vector<Vertex> vertices;
    double duration;
    //time needed to complete each process
    for(int i = 0;i<N;i++){
        fin >> duration;
        vertices.push_back(Vertex(i, duration));
    }
    vector<vector<int>> graph(N+1);
    int from, to;
    for(int i=0; i<=N; i++){
        indegree[i] = 0;
        timePassed[i] = 0;
    }
    for(int i=0; i<M; i++) {
        fin >> from >> to;
        graph[from].push_back(to);
        indegree[to]++;
    }
    fin.close();
    queue<Vertex> zero_list;
    for(int i=0; i<N; i++){
        if(indegree[i]==0){
            zero_list.push(vertices[i]);
        }
    }
    int cnt = 0;
    double result = 0;
    while(!zero_list.empty()){
        Vertex node = zero_list.front();
        zero_list.pop();
        cnt++;
        result = timePassed[node.id] + node.duration;
        for(int i=0; i<graph[node.id].size(); i++){
            indegree[graph[node.id][i]]--;
            if(timePassed[graph[node.id][i]] < timePassed[node.id] + node.duration){
                timePassed[graph[node.id][i]] = timePassed[node.id] + node.duration;
            }
            if(indegree[graph[node.id][i]]==0){
                zero_list.push(vertices[graph[node.id][i]]);
            }
        }
    }
    ofstream outfile;
    outfile.open(argv[2]);
    if(cnt<N){
        outfile << fixed << setprecision(6) << -1.;
    }
    else{
        outfile << fixed << setprecision(6) << result;
    }
}

int main(int argc, char* argv[]){
    top_sort_using_adj_list(argv);
    return 0;
}
