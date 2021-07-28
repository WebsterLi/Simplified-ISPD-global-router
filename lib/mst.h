#ifndef __MST_H__
#define __MST_H__

//#include <iostream>
#include <map>
#include <vector>

using namespace std;

enum {MIN_SPANNING_TREE, MAX_SPANNING_TREE};

class MST_Point
{
    public:
	MST_Point(int _x, int _y)
	{
	    x = _x;
	    y = _y;
	}

	~MST_Point()              { ; }

	int GetX()               { return x; }
	int GetY()               { return y; }

    private:
	int x;
	int y;
};


class MST_SubNet
{
    public:
	MST_SubNet(int _point_id_1, int _point_id_2, int _distance)
	{
	   point_id_1 = _point_id_1;
	   point_id_2 = _point_id_2;
	   distance = _distance;
	}

	MST_SubNet(int _point_id_1, int _point_id_2)
	{
	   point_id_1 = _point_id_1;
	   point_id_2 = _point_id_2;
	   distance = -1;
	}

	~MST_SubNet() {;}

	int GetPointId1() const              { return point_id_1; }
	int GetPointId2() const              { return point_id_2; }
	int GetDistance() const              { return distance; }
	void SetDistance(int v)               { distance = v; }

    private:
	int point_id_1;
	int point_id_2;
	int distance;
};


struct MST_SubNet_Compare
{
    bool operator()(MST_SubNet m1, MST_SubNet m2) const
    {
	if (m1.GetPointId1() < m2.GetPointId1())
	{ return true; }
	else if (m1.GetPointId1() == m2.GetPointId1())
	{
	    if (m1.GetPointId2() < m2.GetPointId2())
	    { return true; }
	}
	return false;
    }
};


class MST_Set
{
    public:
	MST_Set(int pin_uid, int rep)
	{ 
	    pinUid = pin_uid; 
	    representative = rep; 
	    link = NULL; 
	}
	~MST_Set()                         {;}

	int GetPinUid() const              {return pinUid;}
	int GetRepresentative() const      {return representative;}
	MST_Set* GetLink() const           {return link;}
	void SetLink(MST_Set* ms)          {link = ms;}
	void SetRepresentative(int value)  {representative = value;}

    private:
	int pinUid;
	int representative;
	MST_Set* link;
};


class MST
{
    public: 
	MST() 
	{ 
	    tree_mode = MIN_SPANNING_TREE;
	    number_of_point = 0;
	    distanceM.clear(); 
	    resultSubNetV.clear();
	    inputSubNetV.clear();
	}
	~MST() {;}

	void SetPointNo(int v)             { number_of_point = v; }
	void CalMinimumSpanningTree();
	void ClearDistanceM()              { distanceM.clear(); }
	int GetResultSubNetNo()              { return resultSubNetV.size(); }
	int GetInputSubNetNo()               { return inputSubNetV.size(); }
	MST_SubNet& GetResultSubNet(int id)    { return resultSubNetV[id]; }
	MST_SubNet& GetInputSubNet(int id)     { return inputSubNetV[id]; }
	void SetDistance(int point_id_1, int point_id_2, int distance);
	void IncDistance(int point_id_1, int point_id_2, int distance);

    private:

	void AddResultSubNet(int point_id_1, int point_id_2);
	void CalSpanningTree();
	int GetDistance(int point_id_1, int point_id_2);
	int GetPointNo()                   { return number_of_point; }
	int GetTreeMode()                  { return tree_mode; }
	void SetTreeMode(int v)            { tree_mode = v; }
	void UpdateAllSubNetDistance();   //for incremental distance input

	static bool MST_SubNet_ShortToLong_Sort(const MST_SubNet& m1, 
		const MST_SubNet& m2);
	static bool MST_SubNet_LongToShort_Sort(const MST_SubNet& m1, 
		const MST_SubNet& m2);

	int tree_mode; //MIN_SPANNING_TREE, MAX_SPANNING_TREE
	int number_of_point;
	map <MST_SubNet, int, MST_SubNet_Compare> distanceM;
	vector <MST_SubNet> resultSubNetV;
	vector <MST_SubNet> inputSubNetV;
};




#endif
