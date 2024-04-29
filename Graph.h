//
//  Graph.h
//  Best Flights Finder
//
//  Created by David Holguin on 4/29/24.
//

#ifndef Graph_h
#define Graph_h

#include <sstream>
#include <iomanip>
#include <fstream>
#include <string>

#include <climits>
#include <vector>
#include <map>
#include <queue>

using namespace std;


struct Edge;

struct Vertex {
	string code;
	int bestArrival;
	Edge* parentEdge;
	vector<Edge*> adjlist;
	Vertex(const string &newcode)
	{ code = newcode; bestArrival = INT_MAX; parentEdge = nullptr; }
};


struct Edge {
	Vertex* origin;
	Vertex* dest;
	string airline, flno;
	int departure, arrival;

	Edge(const string& airline, const string& flno,
		Vertex* origin, Vertex* dest, int departure, int arrival):
		airline(airline),
		flno(flno),
		origin(origin),
		dest(dest),
		departure(departure),
		arrival(arrival) {}
};



/**
 Compares vertices based on their dvalues.
 It returns vertex with smallest bestArrival
 */
struct CompareVertices {
    bool operator()(const Vertex* v1, const Vertex* v2) {
	   return v1->bestArrival > v2->bestArrival; // Min-heap based on bestArrival
    }
};


class Graph {
private:
	priority_queue<Vertex*, vector<Vertex*>, CompareVertices> heap;
	unordered_map<string, Vertex*> vmap;
	vector<Edge*> elist;
	
	///A recursive function to traverse from final destination to source and then print edge info during unfolding.
	void printHelper (Edge* e) {
		if (!e)
			return;
		printHelper(e->origin->parentEdge);
		cout<<setw(3)<<left<<e->airline;
		cout<<setw(4)<<right<<e->flno;
		cout<<setw(4)<<right<<e->origin->code;
		cout<<setw(4)<<right<<e->dest->code;
		cout<<setw(5)<<right<<e->departure;
		cout<<setw(5)<<right<<e->arrival<<endl;
	}
	
public:
	///scans a txt file and imports vertices into graph
	void readAirports(const string filename) {
		ifstream file(filename);
		if (!file.is_open()) {  cout<<"could not open airports file"<<endl;
			exit(EXIT_FAILURE); }
		
		string line;
		getline(file, line);

		while (getline(file, line)) {
			line.erase(0, line.find_first_not_of(" \t\r\n"));
			line.erase(line.find_last_not_of(" \t\r\n") + 1);
			
			if (line.empty())
				continue;
			Vertex* v = new Vertex(line);
			vmap[line] = v;
		}
		
		file.close();
	}
	///scans a txt file and imports edges into graph
	void readFlights(const string filename) {
		ifstream file(filename);
		if (!file.is_open()) {
			cout<<"could not open flights file"<<endl;
			exit(EXIT_FAILURE);
		}
		
		string line;
		getline(file, line);
		getline(file, line);
		
		while (getline(file, line)) {
			line.erase(0, line.find_first_not_of(" \t\r\n")); //erase leading whitespaces
			line.erase(line.find_last_not_of(" \t\r\n") + 1); //erase trailing whitespaces
			
			if (line.empty())
				continue;
			
			istringstream iss(line);
			string airline, flno, originCode, destCode;
			int depttime, arrtime;
			
			if (!(iss >> airline >> flno >> originCode >> destCode >> depttime >> arrtime))
				continue;
			
			if (vmap.find(originCode) == vmap.end() || vmap.find(destCode) == vmap.end())
				continue;
			
			Vertex* origin = vmap[originCode];
			Vertex* dest = vmap[destCode];
			Edge* e = new Edge(airline, flno, origin, dest, depttime, arrtime);
			elist.push_back(e);
			origin->adjlist.push_back(e);
		}
		
		file.close();
	}
	
	
	/**
	 Performs Dijkstra's algorithm on a directed graph
	 @discussion it will start with an entry node and a base startTime. It will then enter a loop that "relaxes" all outgoing viable edges. A viable edge is an edge whose departure time is no earlier than the earliest arrival to the current vertex. Relaxing an edge means to update its destination vertex with a better arrival if possible. Every time an edge is relaxed, its destination will be added to the queue. The "cloud" represents all vertices that have been fully checked and require no more visiting. The top of the queue will be the next vertex to visit and add to the cloud. Repeat the process until the final destination node is visited. Once visited it means the best edge towards it was taken, so no more searching is required.
	 @param srcCode a unique string identifying a starting  airport
	 @param sinkCode a unique string identifying a final destination airport
	 @param startTime the earliest time in which the first flight can depart
	 @var bestArrival: the dvalue() to be updated once a faster path from source to a given vertex has been found.
	 @note this algorithm doesn't work with negative edge weights or islanded vertices.
	 */
	int dijkstra(const string& srcCode, const string& sinkCode, int startTime) {
		Vertex* v = vmap[srcCode];
		Vertex* destination = vmap[sinkCode];
		if (v == destination)
			return -1;
		v->bestArrival = startTime;
		while (v != destination)
		{
			for (Edge* e : v->adjlist)
			{
				if (e->departure < v->bestArrival)  //check for invalid departures
					continue;
				relax(e);		//relax only valid departures
			}
			if (heap.empty())   return -1; //cannot find viable edge towards destination
			v = heap.top();
			heap.pop();
		}
		return destination->bestArrival;
	}
	
	
	///Updates the bestArrival time  from source to vertex v if possible. It also adds the best new value to the queue
	void relax (Edge* e) {
		Vertex* v = e->dest;
		if (e->arrival < v->bestArrival )
		{	//update dvalue() in vertex v
			v->bestArrival = e->arrival; //same as adding d(u) + weight(e)
			v->parentEdge = e;
			heap.push(v);   //add vertex with new dvalue() to queue
		}
	}

	
	///Prints the itinierary for the shortest path found
	///@param code The unique string identifying the final destination airport.
	void printPath(const string& code) {

		Edge* e = vmap[code]->parentEdge;
		printHelper(e);
	}
};

#endif /* Graph_h */
