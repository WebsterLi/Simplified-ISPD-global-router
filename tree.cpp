#include <stdio.h>
#include <stdlib.h>
#include <iomanip>
#include "lib/mst.h"
#include "routingdb.h"
#include "tree.h"
#include <cmath>

using namespace std;

extern RoutingDB db;


void RoutingTree::MinimumSpanningTreeConstruction()
{
    cout << "..Minimum Spanning Tree (MST) Algorithm" << endl;
    db.ReserveSubNetNo(db.GetNetNo()*3);

    for (int i = 0; i < db.GetNetNo(); i++)
    {
	Net& n = db.GetNetByPosition(i);
	MinimumSpanningTreeConstruction(n);
    }
    cout << endl;
}


void RoutingTree::MinimumSpanningTreeConstruction(Net& n)
{
    MST m;
    /* input */
    m.SetPointNo(n.GetPinNo());

    /* set distance */
    for (int i = 0; i < n.GetPinNo()-1; i++)
    {
	for (int j = i+1; j < n.GetPinNo(); j++)
	{
	    Pin& pin1 = n.GetPin(i);
	    Pin& pin2 = n.GetPin(j);
	    m.SetDistance(i, j, ManhattanDistance(pin1, pin2));
	}
    }


    m.CalMinimumSpanningTree();

    /* result */
    for (int i = 0; i < m.GetResultSubNetNo(); i++)
    {
	int pin1_uid = n.GetPinUid(m.GetResultSubNet(i).GetPointId1());
	int pin2_uid = n.GetPinUid(m.GetResultSubNet(i).GetPointId2());
	Pin& pin1 = db.GetPin(pin1_uid);
	Pin& pin2 = db.GetPin(pin2_uid);
	db.AddSubNet(n, pin1, pin2);
	n.AddSubNetUid(db.GetSubNetNo()-1);
    }

    m.ClearDistanceM();
}




int RoutingTree::ManhattanDistance(Pin& p1, Pin& p2)
{
    int manh_len = 
	abs(p1.GetGx()-p2.GetGx()) + abs(p1.GetGy()-p2.GetGy()); 
    return manh_len; 
}


void RoutingTree::ShowInfo()
{
    cout << "RoutingTree Info:" << endl;
    cout << "..# of net: " << db.GetNetNo() << endl;
    cout << "..# of decomposed 2-pin subnet: " << db.GetSubNetNo() << endl;
    cout << endl;
}


