//
// 	@author David Holguin
//	Created on 4/28/24.
//  	
//	Variation of shortest path search in a directed graph using Dijkstra's.
//

#include <iostream>
#include "Graph.h"
using namespace std;

int main(int argc, char* args[]) {
	if (argc != 4) { return EXIT_FAILURE; }
	
	Graph g;
	g.readAirports("airports.txt");
	g.readFlights("flights.txt");
	
	string origin = args[1], destination = args[2];
	int startTime = stoi(args[3]);
	int bestArrivalTime = g.dijkstra(origin, destination, startTime);
	cout<<bestArrivalTime<<endl;
	g.printPath(args[2]);
	
    return 0;
}
