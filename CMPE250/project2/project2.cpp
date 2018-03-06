/*
Student Name: DİLARA GÖKAY
Student Number: 2014400102
Project Number: 2
Operating System: XUBUNTU
Compile Status: SUCCESS
Program Status: SUCCESS
*/
#include <iostream>
#include <sstream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <queue>
#include<thread>
#include <iomanip>

using namespace std;

struct Event{
    int index;
    double arrTime;         //arrival time of event
    double cpuWork;
    double IOwork;
    double exitTime;
    double departure;       //departure time of event from PQ
    int location;           //0=added, 1=cpu, 2=io
    bool quantum;
    double waitTime;
    vector<double> queue;   //the queue which holds the times when events enter & exit queues
    Event(double arrTime_, double cpuWork_, double IOwork_, int index_){
            index = index_;
            arrTime = arrTime_;
            cpuWork = cpuWork_;
            IOwork = IOwork_;
            departure = arrTime_;
            exitTime = -1;
            location = 0;
            quantum = false;
            waitTime = 0;
    }
    Event() {
        arrTime = 0;
        cpuWork = 0;
        IOwork = 0;
        departure = 0;
        exitTime = -1;
        location = 0;
        quantum = false;
        index = -1;
        waitTime = 0;
    }
    bool isSame(Event obj){
        bool a = arrTime == obj.arrTime && cpuWork == obj.cpuWork && IOwork == obj.IOwork && departure == obj.departure;
        return exitTime == obj.exitTime && quantum == obj.quantum && location == obj.location && index == obj.index && a;
    }

};

struct CPU{
    bool empty;
    double frequency;
    double activity;
    int indexe;
    int index;
    CPU(double frequency_, double activity_, int index_){
        indexe = 0;
        empty = true;
        frequency = frequency_;
        activity = activity_;
        index = index_;
    }
    CPU(){
        indexe = 0;
        empty = true;
        frequency = 0;
        activity = 0;
        index = 0;
    }
};

struct IO{
    bool empty;
    double quantum;
    double activity;
    int index;
    int indexe;
    IO(double quantum_, double activity_, int index_){
        indexe = 0;
        empty = true;
        quantum = quantum_;
        activity = activity_;
        index = index_;
    }
    IO(){
        indexe = 0;
        empty = true;
        quantum = 0;
        activity =0;
        index = 0;
    }
};
//compares cpu works
struct CompareCPU {
    bool operator()(Event& e1, Event& e2)
    {
        return e1.cpuWork > e2.cpuWork;
    }
};
//compares departure times
struct CompareDep {
    bool operator()(Event& e1, Event& e2)
    {
        return e1.departure > e2.departure;
    }
};
//Heap where events wait for cpu
struct CPUHeap{
    int max_length;
    int length;
    std::priority_queue<Event, std::vector<Event>, CompareCPU > minHeap;
    CPUHeap(){
        max_length = 0;
        length = 0;
    }
    void push(Event e){
        minHeap.push(e);
        length++;
        if(length>max_length){
            max_length = length;
        }
    }
    const Event& top() const{
        minHeap.top();
    }
    void pop(){
        minHeap.pop();
        length--;
    }
    bool empty(){
        return minHeap.empty();
    }
};
//heap where events wait for io
struct IOHeap{
    int max_length;
    int length;
    std::queue<Event> minHeap;
    IOHeap(){
        max_length = 0;
        length = 0;
    }
    void push(Event e){
        minHeap.push(e);
        length++;
        if(length>max_length){
            max_length = length;
        }
    }
    const Event& top() const{
        minHeap.front();
    }
    void pop(){
        minHeap.pop();
        length--;
    }
    bool maxWillChange(){
        return length+1>max_length;
    }
    bool empty(){
        return minHeap.empty();
    }
};

int main(int argc, char* argv[]) {
    //takes all inputs from file and puts them in a vector
    double m, n, p;
    double r;
    ifstream in(argv[1]);
    vector<CPU> CPUs;
    if(in >> m){                //# of CPUs
        for(int i = 0;i<m;i++){
            in >> n;            //frequency of CPU
            CPUs.push_back(CPU(n, 0, i));
        }
    }
    vector<IO> IOs;
    if(in >> m){                //# of IOs
        for(int i = 0;i<m;i++){
            in >> n;            //quantum of IO
            IOs.push_back(IO(n, 0, i));
        }
    }
    vector<Event> events;
    if(in >> m){                //# of events
        for(int i = 0; i<m; i++){
            in >> n >> p >> r;
            events.push_back(Event(n,p,r,i));
        }
    }
    //priority queue for the events that are waiting for their departure
    priority_queue<Event, vector<Event>, CompareDep> depHeap;
    for(int i = 0;i<events.size();i++) {
        depHeap.push(events[i]);
    }
    IOHeap ioHeap;
    CPUHeap cpuHeap;
    double t = 0;       //time
    //add events according to their departure time in depHeap
    while (!depHeap.empty()) {
        //there will be at most 3 events at the same time
        vector<Event> process;
        Event current;
        Event e1;
        current = events[depHeap.top().index];        //subtract root
        depHeap.pop();
        t = current.departure;
        //if there is any other event occurring at the same time
        process.push_back(current);
        if(depHeap.top().arrTime == t){
            current = events[depHeap.top().index];
            depHeap.pop();
            process.push_back(current);
        }
        if(depHeap.top().arrTime == t){
            current = events[depHeap.top().index];
            depHeap.pop();
            process.push_back(current);
        }
        //to process all elements occur at t
        //look for each element of vector
        //io>cpu>added
        for(int i = 0;i<process.size(); i++){
            e1 = events[process[i].index];
            if(e1.location == 2) {
                int indexevent = process[i].index;
                int indexIO = -1;
                double quan = -1;
                //a method for finding index of io
                for(int j = 0;j<IOs.size();j++){
                    if(IOs[j].indexe == indexevent){
                        indexIO = j;
                        quan = IOs[j].quantum;
                    }
                }
                events[indexevent].quantum = events[indexevent].IOwork > quan;
                //if ioheap is empty and next process is more than quantum
                if(events[indexevent].IOwork == 0){
                    events[indexevent].exitTime = t;
                    IOs[indexIO].empty = true;
                    if(!ioHeap.empty()) {
                        Event e2 = ioHeap.top();    //new event came from heap
                        ioHeap.pop();
                        //finding index of event
                        int indexevent3 = 0;
                        for (int k = 0; k < events.size(); k++) {
                            if (events[k].isSame(e2)) {
                                indexevent3 = k;
                            }
                        }
                        events[indexevent3].location = 2;
                        events[indexevent3].queue.push_back(t);
                        if (events[indexevent3].IOwork > quan) {
                            events[indexevent3].departure = t + quan;
                            events[indexevent3].IOwork = events[indexevent3].IOwork - quan;
                            IOs[indexIO].activity += quan;
                            IOs[indexIO].indexe = indexevent3;
                            IOs[indexIO].empty = false;
                            depHeap.push(events[indexevent3]);
                        } else {
                            events[indexevent3].departure = t + events[indexevent3].IOwork;
                            IOs[indexIO].activity += events[indexevent3].IOwork;
                            events[indexevent3].IOwork = 0;
                            IOs[indexIO].indexe = indexevent3;
                            IOs[indexIO].empty = false;
                            depHeap.push(events[indexevent3]);
                        }
                    }
                }
                else if(events[indexevent].quantum){
                        if(ioHeap.empty()){
                            events[indexevent].departure = t + quan;
                            events[indexevent].IOwork = events[indexevent].IOwork - quan;
                            IOs[indexIO].activity += quan;
                            IOs[indexIO].indexe = indexevent;
                            depHeap.push(events[indexevent]);
                        }
                        else{
                            bool a = ioHeap.maxWillChange();
                            if(a){
                                ioHeap.length--;
                            }
                            events[indexevent].queue.push_back(t);      //old event sent to heap
                            ioHeap.push(events[indexevent]);
                            IOs[indexIO].empty = true;  //io emptied
                            Event e2 = ioHeap.top();    //new event came from heap
                            ioHeap.pop();
                            if(a) {
                                ioHeap.length++;
                            }
                            //finding index of event
                            int indexevent2 = 0;
                            for(int k = 0;k<events.size();k++){
                                if(events[k].isSame(e2)){
                                    indexevent2 = k;
                                }
                            }
                            events[indexevent2].location = 2;
                            events[indexevent2].queue.push_back(t);
                            if (events[indexevent2].IOwork > quan) {
                                events[indexevent2].departure = t + quan;
                                events[indexevent2].IOwork = events[indexevent2].IOwork - quan;
                                IOs[indexIO].activity += quan;
                                IOs[indexIO].indexe = indexevent2;
                                IOs[indexIO].empty = false;
                                depHeap.push(events[indexevent2]);
                            } else {
                                events[indexevent2].departure = t + events[indexevent2].IOwork;
                                IOs[indexIO].activity += events[indexevent2].IOwork;
                                events[indexevent2].IOwork = 0;
                                IOs[indexIO].indexe = indexevent2;
                                IOs[indexIO].empty = false;
                                depHeap.push(events[indexevent2]);
                            }
                        }
                }
                else{
                        if(ioHeap.empty()){
                            events[indexevent].departure = t + events[indexevent].IOwork;
                            events[indexevent].IOwork = 0;
                            IOs[indexIO].activity += events[indexevent].IOwork;
                            IOs[indexIO].indexe = indexevent; //because fields of e1 changed
                            depHeap.push(events[indexevent]);
                        }
                        else{
                            bool a = ioHeap.maxWillChange();
                            if(a) {
                                ioHeap.length--;
                            }
                            events[indexevent].queue.push_back(t);      //old event sent to heap
                            ioHeap.push(events[indexevent]);
                            IOs[indexIO].empty = true;  //io emptied
                            Event e2 = ioHeap.top();    //new event came from heap
                            ioHeap.pop();
                            if(a) {
                                ioHeap.length++;
                            }
                            //finding index of event
                            int indexevent2 = 0;
                            for(int k = 0;k<events.size();k++){
                                if(events[k].isSame(e2)){
                                    indexevent2 = k;
                                }
                            }
                            events[indexevent2].location = 2;
                            events[indexevent2].queue.push_back(t);
                            if (events[indexevent2].IOwork > quan) {
                                events[indexevent2].departure = t + quan;
                                events[indexevent2].IOwork = events[indexevent2].IOwork - quan;
                                IOs[indexIO].activity += quan;
                                IOs[indexIO].indexe = indexevent2;
                                IOs[indexIO].empty = false;
                                depHeap.push(events[indexevent2]);
                            } else {
                                events[indexevent2].departure = t + events[indexevent2].IOwork;
                                IOs[indexIO].activity += events[indexevent2].IOwork;
                                events[indexevent2].IOwork = 0;
                                IOs[indexIO].indexe = indexevent2;
                                IOs[indexIO].empty = false;
                                depHeap.push(events[indexevent2]);
                            }
                        }
                }
            }
        }

        for(int i = 0;i<process.size(); i++) { //or maybe i better say !process.empty()
            e1 = events[process[i].index];
            if(e1.location == 1){
                int indexevent = process[i].index;
                bool inIO = false;
                int indexcpu = -1;
                //a method for finding index of cpu
                for(int c = 0;c<CPUs.size();c++){
                    if(CPUs[c].indexe == indexevent){
                        indexcpu = c;
                    }
                }
                for(int j = 0;j<IOs.size()&&!inIO;j++){
                    //putting in io
                    if(IOs[j].empty){
                        IOs[j].empty = false;
                        events[indexevent].location = 2;
                        if(events[indexevent].IOwork>IOs[j].quantum) {
                            events[indexevent].quantum = true;
                            events[indexevent].departure = t + IOs[j].quantum;
                            IOs[j].activity += IOs[j].quantum;
                            events[indexevent].IOwork -= IOs[j].quantum;
                        }
                        else {
                            events[indexevent].quantum = false;
                            events[indexevent].departure = t + events[indexevent].IOwork;
                            IOs[j].activity += events[indexevent].IOwork;
                            events[indexevent].IOwork = 0;
                        }
                        CPUs[indexcpu].empty = true;
                        IOs[j].indexe = indexevent;
                        depHeap.push(events[indexevent]);
                        inIO = true;
                    }
                }
                //if all io's are full
                if(!inIO){
                    events[indexevent].queue.push_back(t);
                    CPUs[indexcpu].empty = true;
                    ioHeap.push(events[indexevent]);
                }
                //after sending to io or io heap, take a new event to emptied cpu
                if(!cpuHeap.empty()){
                    Event e2(cpuHeap.top());
                    cpuHeap.pop();
                    //finding index of event
                    int indexevent2 = 0;
                    for(int k = 0;k<events.size();k++){
                        if(events[k].isSame(e2)){
                            indexevent2 = k;
                        }
                    }
                    events[indexevent2].location = 1;
                    events[indexevent2].queue.push_back(t);
                    events[indexevent2].departure = t + events[indexevent2].cpuWork/CPUs[indexcpu].frequency;
                    CPUs[indexcpu].activity += events[indexevent2].cpuWork/CPUs[indexcpu].frequency;
                    CPUs[indexcpu].indexe = indexevent2;
                    CPUs[indexcpu].empty = false;
                    depHeap.push(events[indexevent2]);
                }
                else{
                    CPUs[indexcpu].empty = true;
                }
            }

        }

        for(int i = 0;i<process.size(); i++) {
            e1 = events[process[i].index];
            if(e1.location == 0){
                int indexevent = process[i].index;
                bool inCPU = false;
                for(int j = 0;j<CPUs.size() && !inCPU;j++){
                    if(CPUs[j].empty){
                        CPUs[j].empty = false;
                        events[indexevent].location = 1;
                        events[indexevent].departure = t + events[indexevent].cpuWork / CPUs[j].frequency;
                        CPUs[j].activity += events[indexevent].cpuWork / CPUs[j].frequency;
                        CPUs[j].indexe = indexevent;
                        depHeap.push(events[indexevent]);
                        inCPU = true;
                    }
                }
                if(!inCPU){
                    events[indexevent].queue.push_back(t);
                    cpuHeap.push(events[indexevent]);
                }
            }
        }
    }

    //writing to file
    ofstream outfile;
    outfile.open (argv[2]);
    outfile << cpuHeap.max_length << "\n";        //max length of cpu heap
    outfile << ioHeap.max_length << "\n";         //max length of io heap
    int max = 0;
    int index = 0;
    for(int i = 0;i<CPUs.size();i++){
        if(CPUs[i].activity>=max) {
            max = CPUs[i].activity;
            index = i;
        }
    }
    outfile << index+1 << "\n";                     //ID of CPU which is most active
    max = 0;
    index = 0;
    for(int i = 0;i<CPUs.size();i++){
        if(IOs[i].activity>=max) {
            max = IOs[i].activity;
            index = i;
        }
    }
    outfile << index+1 << "\n";                     //ID of IO device which is most active
    //calculate wait time of all tasks
    for(int i = 0;i<events.size();i++){
        for(int j=0;j<events[i].queue.size();j+=2){
            events[i].waitTime = events[i].waitTime + events[i].queue[j+1] - events[i].queue[j];
        }
    }
    //calculate total wait time of all events
    double totalWait = 0;
    for(int i = 0;i<events.size();i++){
        totalWait += events[i].waitTime;
    }
    //calculate average wait time
    double average = totalWait / events.size();
    // outfile average wait time of all tasks
    outfile << fixed << setprecision(6) << average << "\n";
    double longestWait = 0;
    for(int i = 0;i<events.size();i++){
        if(events[i].waitTime>longestWait){
            longestWait = events[i].waitTime;
        }
    }
    outfile << fixed << setprecision(6) <<longestWait << endl;
    double totalTime = 0;
    for(int i = 0; i<events.size();i++){
        totalTime = totalTime + events[i].exitTime- events[i].arrTime;
    }
    totalTime = totalTime/events.size();
    outfile << fixed << setprecision(6) << totalTime;
    outfile.close();
        return 0;
}
