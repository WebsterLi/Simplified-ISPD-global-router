#ifndef __TREE_H__
#define __TREE_H__

#include <iostream>
#include <vector>
#include "routingdb.h"

using namespace std;

class RoutingTree
{
    public:
	RoutingTree() {;}
	~RoutingTree() {;}

	int ManhattanDistance(Pin& p1, Pin& p2);
	void MinimumSpanningTreeConstruction();
	void ShowInfo();

    private:
	void MinimumSpanningTreeConstruction(Net& n);
};

#endif
