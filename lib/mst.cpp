#include <iostream>
#include <map>
#include <vector>
#include <algorithm>
#include <climits>
#include "mst.h"

using namespace std;


void MST::CalMinimumSpanningTree()
{
    SetTreeMode(MIN_SPANNING_TREE);
    CalSpanningTree();
}

void MST::CalSpanningTree()
{
    UpdateAllSubNetDistance();

    int number_of_points = GetPointNo();

    switch (number_of_points)
    {
	case 0:
	case 1:
	    { 
		//cout << "pin no == " << number_of_points << endl; 
		//exit(1);
	    }
	    break;
	case 2:
	    /* {{{ Trivial algorithm for 2-pin nets */
	    {
		AddResultSubNet(0, 1);
	    }
	    /* }}} */
	    break;

	case 3:
	    /* {{{ Trivial algorithm for 3-pin nets */
	    {
		int dist01 = GetDistance(0, 1);
		int dist02 = GetDistance(0, 2);
		int dist12 = GetDistance(1, 2);

		if (GetTreeMode() == MIN_SPANNING_TREE)
		{
		    if (dist01 < dist02)
		    {
			AddResultSubNet(0, 1);
			if (dist02 < dist12)
			{ AddResultSubNet(0, 2); }
			else
			{ AddResultSubNet(1, 2); }
		    }
		    else //dist02 < dist01
		    {
			AddResultSubNet(0, 2);
			if (dist01 < dist12)
			{ AddResultSubNet(0, 1); }
			else 
			{ AddResultSubNet(1, 2); }
		    }
		}
		else // GetTreeMode() == MAX_SPANNING_TREE
		{
		    if (dist01 > dist02)
		    {
			AddResultSubNet(0, 1);
			if (dist02 > dist12)
			{ AddResultSubNet(0, 2); }
			else
			{ AddResultSubNet(1, 2); }
		    }
		    else //dist02 < dist01
		    {
			AddResultSubNet(0, 2);
			if (dist01 > dist12)
			{ AddResultSubNet(0, 1); }
			else 
			{ AddResultSubNet(1, 2); }
		    }
		}
	    }
	    /* }}} */
	    break;

	default:
	    /* {{{ Kruskal algorithm for multi-pin nets */


	    vector <MST_Set*> SetStartV;
	    vector <MST_Set*> SetEndV;
	    vector <int> SetNumV;
	    vector <bool> connectedV;

	    SetStartV.clear();
	    SetEndV.clear();
	    SetNumV.clear();
	    connectedV.clear();

	    SetStartV.resize(number_of_points);
	    SetEndV.resize(number_of_points);
	    SetNumV.resize(number_of_points, 1);
	    connectedV.resize(number_of_points, false);

	    MST_Set* ms;
	    for (int i = 0; i < GetPointNo(); i++)
	    {
		ms = new MST_Set(i, i);

		SetStartV[i] = ms;
		SetEndV[i] = ms;
	    }

	    /* Sort by non-decreasing order */
	    if (GetTreeMode() == MIN_SPANNING_TREE)
	    { 
		sort(inputSubNetV.begin(), inputSubNetV.end(), 
			MST_SubNet_ShortToLong_Sort); 
	    }
	    else // GetTreeMode() == MAX_SPANNING_TREE
	    { 
		sort(inputSubNetV.begin(), inputSubNetV.end(), 
			MST_SubNet_LongToShort_Sort); 
	    }


	    for (int i = 0; i < GetInputSubNetNo(); i++)
	    {
		int p1_set_id = 
		    SetStartV[inputSubNetV[i].GetPointId1()]->GetRepresentative();
		int p2_set_id = 
		    SetStartV[inputSubNetV[i].GetPointId2()]->GetRepresentative();

		//cout << "subnet[" << i << "] (" << inputSubNetV[i].GetPointId1()
		//    << "," << inputSubNetV[i].GetPointId2() << ") represent: (" 
		//    << p1_set_id << "," << p2_set_id << ")" << endl;

		if (p1_set_id != p2_set_id)
		{
		    int s_pin_uid =
			SetStartV[inputSubNetV[i].GetPointId1()]->GetPinUid();
		    int t_pin_uid =
			SetStartV[inputSubNetV[i].GetPointId2()]->GetPinUid();

		    AddResultSubNet(s_pin_uid, t_pin_uid);

		    connectedV[s_pin_uid] = true;
		    connectedV[t_pin_uid] = true;

		    /* Union big eat small */
		    if (SetNumV[p1_set_id] > SetNumV[p2_set_id])
		    {
			for (MST_Set* ms = SetStartV[p2_set_id]; ms != NULL; 
				ms = ms->GetLink())
			{ ms->SetRepresentative(p1_set_id); }
			SetEndV[p1_set_id]->SetLink(SetStartV[p2_set_id]);
			SetEndV[p1_set_id] = SetEndV[p2_set_id];
			SetNumV[p1_set_id] += SetNumV[p2_set_id];
			SetNumV[p2_set_id] = 0;
		    }
		    else
		    {
			for (MST_Set* ms = SetStartV[p1_set_id]; ms != NULL; 
				ms = ms->GetLink())
			{ ms->SetRepresentative(p2_set_id); }
			SetEndV[p2_set_id]->SetLink(SetStartV[p1_set_id]);
			SetEndV[p2_set_id] = SetEndV[p1_set_id];
			SetNumV[p2_set_id] += SetNumV[p2_set_id];
			SetNumV[p1_set_id] = 0;
		    }
		}
	    }
	    //cout << "total MST length: " << total_net_length << endl;


	    /* check loss-connected point */
	    for (int i = 0; i < GetPointNo(); i++)
	    {
		if (connectedV[i] == false)
		{
		    if (GetResultSubNetNo() > 0)
		    {
			/* random assign subnet (i,0) */
			cout << "..random assign subnet(" << i << ",0)" << endl;
			AddResultSubNet(i, GetResultSubNet(0).GetPointId1());
		    }
		    else
		    {
			/* all point do not connected */
			for (int j = 0; j < GetPointNo()-1; j++)
			{
			    cout << "..random assign subnet(" << j << "," << j+1 
				<< ")" << endl;
			    AddResultSubNet(j, j+1);
			}
			break;
		    }
		}
	    }


	    //MST_SubNetV.clear();
	    for (unsigned int i = 0; i < SetStartV.size(); i++)
	    { delete SetStartV[i]; }
	    SetStartV.clear();
	    SetEndV.clear();
	    SetNumV.clear();
	    /* }}} */
	    break;
    }


}



void MST::AddResultSubNet(int point_id_1, int point_id_2)
{
    int min_point_id = min(point_id_1, point_id_2);
    int max_point_id = max(point_id_1, point_id_2);
    resultSubNetV.push_back(MST_SubNet(min_point_id, max_point_id, 
		GetDistance(point_id_1, point_id_2)));
}


void MST::IncDistance(int point_id_1, int point_id_2, int inc_distance)
{
    int min_point_id = min(point_id_1, point_id_2);
    int max_point_id = max(point_id_1, point_id_2);

    MST_SubNet me(min_point_id, max_point_id);
    inputSubNetV.push_back(me);

    map < MST_SubNet, int, MST_SubNet_Compare >::iterator ans;

    ans = distanceM.find(me);
    if (ans == distanceM.end())
    {
	/* not find */
	distanceM[me] = inc_distance;
    }
    else
    {
	/* find */
	int ori_distance = distanceM[me];
	distanceM[me] = ori_distance+inc_distance;
    }

}


void MST::UpdateAllSubNetDistance()
{
    for (int i = 0; i < GetInputSubNetNo(); i++)
    {
	MST_SubNet& me = GetInputSubNet(i);
	int distance = GetDistance(me.GetPointId1(), me.GetPointId2());
	me.SetDistance(distance);
    }
}


void MST::SetDistance(int point_id_1, int point_id_2, int distance)
{
    int min_point_id = min(point_id_1, point_id_2);
    int max_point_id = max(point_id_1, point_id_2);

    MST_SubNet me(min_point_id, max_point_id);
    inputSubNetV.push_back(me);

    MST_SubNet key(min_point_id, max_point_id);
    distanceM[key] = distance;
}




int MST::GetDistance(int point_id_1, int point_id_2)
{
    int min_point_id = min(point_id_1, point_id_2);
    int max_point_id = max(point_id_1, point_id_2);

    map < MST_SubNet, int, MST_SubNet_Compare >::iterator ans;

    MST_SubNet key(min_point_id, max_point_id);

    ans = distanceM.find(key);
    if (ans == distanceM.end())
    {
	/* not find */
	if (GetTreeMode() == MIN_SPANNING_TREE)
	{ return INT_MAX; }
	else // GetTreeMode() == MAX_SPANNING_TREE
	{ return INT_MIN; }
    }
    else
    {
	/* find */
	return distanceM[key];
    }
}


bool MST::MST_SubNet_ShortToLong_Sort(const MST_SubNet& m1, const MST_SubNet& m2)
{ 
    return m1.GetDistance() < m2.GetDistance();
}


bool MST::MST_SubNet_LongToShort_Sort(const MST_SubNet& m1, const MST_SubNet& m2)
{ 
    return m1.GetDistance() > m2.GetDistance();
}

