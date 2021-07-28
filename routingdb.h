#ifndef __ROUTINGDB_H__
#define __ROUTINGDB_H__

#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <set>

using namespace std;

class CapacityAdjust;
class Net;
class Pin;
class RoutingDB;
class SubNet;


class Net
{
	public:
		Net(string _name, int _uid) 
		{
			subnetUidV.clear();
			name = _name;
			pinUidV.clear();
			tilePinUidM.clear();
			uid = _uid;
		}

		~Net() { ; }

		void AddSubNetUid(int id)	{ subnetUidV.push_back(id); }
		void AddPinUid(int id)	{ pinUidV.push_back(id); }
		void AddPinUidTileMap(int pin_id, short gx, short gy);
		void AddPinUidTileMap(int pin_id, short gx, short gy, short layer);
		void ClearPinUidTileMap()	{ tilePinUidM.clear(); }
		void ClearPinUidV()	{ pinUidV.clear(); }
		SubNet& GetSubNet(int id);                 
		int GetSubNetNo()	{ return subnetUidV.size(); }
		int GetSubNetUid(int id)	{ return subnetUidV[id]; }
		string GetName()	{ return name; }
		Pin& GetPin(int id);                 
		int GetPinNo()	{ return pinUidV.size(); }
		int GetPinUid(int id)	{ return pinUidV[id]; }
		int GetPinUidByTile(short gx, short gy);            
		int GetPinUidByTile(short gx, short gy, short layer);
		int GetUid()	{ return uid; }
		void ReservePinNo(int no)	{ pinUidV.reserve(no); }
		void ShowInfo();
		void ShowSubNetInfo();

	private:
		string name;
		int uid;
		vector <int> pinUidV;
		vector <int> subnetUidV;

		map <int, int> tilePinUidM;  /* parser: query pin_uid by tile coordinate */
};

class CapacityAdjust
{
	public:
		CapacityAdjust(short _gx1, short _gy1, short _layer1,
				short _gx2, short _gy2, short _layer2, 
				short _reduce_capacity)
		{
			gx1 = _gx1;
			gy1 = _gy1;
			layer1 = _layer1;
			gx2 = _gx2;
			gy2 = _gy2;
			layer2 = _layer2;
			reduce_capacity = _reduce_capacity;
		}
		~CapacityAdjust()  {;}

		short GetGx1()             { return gx1; }
		short GetGx2()             { return gx2; }
		short GetGy1()             { return gy1; }
		short GetGy2()             { return gy2; }
		short GetLayer1()          { return layer1; }
		short GetLayer2()          { return layer2; }
		short GetReduceCapacity()  { return reduce_capacity; }
		void ShowInfo();

	private:
		short gx1;
		short gy1;
		short layer1;
		short gx2;
		short gy2;
		short layer2;
		short reduce_capacity;
};

class Pin
{
	public:
		Pin(int _uid, short _gx, short _gy) 
		{
			gx = _gx; 
			gy = _gy;
			uid = _uid;
		}

		~Pin() {;}

		short GetGx()                      { return gx; }
		short GetGy()                      { return gy; }
		short GetLayer()                   { return 1; }  /* pins all lie in layer 1 */
		int GetUid()                       { return uid; }
		void ShowInfo();

	private:
		short gx;                    /* global tile x coordinate */
		short gy;                    /* global tile y coordinate */
		int uid;
};

class SubNet
{
	public:
		SubNet(int _uid, Net& _n, Pin& _s_pin, Pin& _t_pin)
		{
			n = &_n;
			source_pin = &_s_pin;
			target_pin = &_t_pin;
			uid = _uid;
		}

		SubNet() { ; }
		~SubNet() { ; }

		int GetType();
		int GetNetUid()                { return GetNet().GetUid(); }
		Net& GetNet()                  { return *n; }
		Pin& GetSourcePin()            { return *source_pin; }
		short GetSourcePinGx()         { return GetSourcePin().GetGx(); }
		short GetSourcePinGy()         { return GetSourcePin().GetGy(); }
		short GetSourcePinLayer()      { return GetSourcePin().GetLayer(); }
		int GetSourcePinUid()          { return GetSourcePin().GetUid(); }
		Pin& GetTargetPin()            { return *target_pin; }
		short GetTargetPinGx()         { return GetTargetPin().GetGx(); }
		short GetTargetPinGy()         { return GetTargetPin().GetGy(); }
		short GetTargetPinLayer()      { return GetTargetPin().GetLayer(); }
		int GetTargetPinUid()          { return GetTargetPin().GetUid(); }
		int GetUid()                   { return uid; }
		void ShowInfo();

	private:
		Net* n;
		Pin* source_pin;
		Pin* target_pin;
		int uid;
};

class RoutingDB 
{
	public:
		RoutingDB() 
		{
			capacity_adjustV.clear();
			subnetV.clear();
			global_tile_width = -1;
			global_tile_height = -1;
			hori_capacityV.clear();
			hori_global_tile_no = -1;
			layer_no = -1;
			lower_left_x = -1;
			lower_left_y = -1;
			min_widthV.clear();
			min_spacingV.clear();
			netV.clear();
			pinV.clear();
			verti_capacityV.clear();
			verti_global_tile_no = -1;
			via_spacingV.clear();
		}
		~RoutingDB() { ; }

		void AddCapapcityAdjust(short gx1, short gy1, short layer1,
				short gx2, short gy2, short layer2, short value);
		void AddSubNet(Net& n, Pin& s_pin, Pin& t_pin);
		void AddHoriCapacity(short v)        { hori_capacityV.push_back(v); }
		void AddMinSpacing(short v)          { min_spacingV.push_back(v); }
		void AddMinWidth(short v)            { min_widthV.push_back(v); }
		void AddNet(string n, int id)        { netV.push_back(Net(n, id)); }
		void AddNetUidPositionMap(int uid, int pos);
		void AddPin(short gx, short gy, short layer); 
		void AddVertiCapacity(short v)       { verti_capacityV.push_back(v); }
		void AddViaSpacing(short v)          { via_spacingV.push_back(v); }
		int CalCenterX(short gx);
		int CalCenterY(short gy);
		short CalGx(int x);
		short CalGy(int x);
		void ClearCapacityAdjust()           { capacity_adjustV.clear(); }
		CapacityAdjust& GetCapacityAdjust(int id);
		int GetCapacityAdjustNo()            { return capacity_adjustV.size(); }
		int GetChipHeight();
		int GetChipWidth();
		SubNet& GetSubNet(int id)                { return subnetV[id]; }
		int GetSubNetNo()                      { return subnetV.size(); }
		short GetHoriGlobalTileNo()          { return hori_global_tile_no; }
		int GetLayerHoriCapacity(short L)    { return hori_capacityV[L]; }
		int GetLayerMinSpacing(short L)      { return min_spacingV[L]; }
		int GetLayerMinWidth(short L)        { return min_widthV[L]; }
		short GetLayerNo()                   { return layer_no; }
		int GetLayerVertiCapacity(short L)   { return verti_capacityV[L]; }
		int GetLayerViaSpacing(short L)      { return via_spacingV[L]; }
		int GetLowerLeftX()                  { return lower_left_x; }
		int GetLowerLeftY()                  { return lower_left_y; }
		Net& GetNetByPosition(int pos)       { return netV[pos]; }
		Net& GetNetByUid(int uid)            { return netV[GetNetPosByUid(uid)]; }
		int GetNetPosByUid(int uid)          { return netUidPosM[uid]; }
		int GetNetNo()                       { return netV.size(); }
		Pin& GetPin(int id)                  { return pinV[id]; }
		int GetPinNo()                       { return pinV.size(); }
		int GetTileHeight()                  { return global_tile_height; }
		int GetTileWidth()                   { return global_tile_width; }
		short GetVertiGlobalTileNo()         { return verti_global_tile_no; }
		void RemoveLastNet()                 { netV.erase(netV.end()-1); }
		void RemoveLastPin()                 { pinV.erase(pinV.end()-1); }
		void ReserveCapacityAdjustNo(int no) { capacity_adjustV.reserve(no); }
		void ReserveSubNetNo(int no)           { subnetV.reserve(no); }
		void ReserveNetNo(int no)            { netV.reserve(no); }
		void ResizeSubNetNo(int no)            { subnetV.resize(no); }
		void SetHoriGlobalTileNo(short v)    { hori_global_tile_no = v; }
		void SetLayerNo(short v)             { layer_no = v; }
		void SetLowerLeftX(int v)            { lower_left_x = v; }
		void SetLowerLeftY(int v)            { lower_left_y = v; }
		void SetTileWidth(int v)             { global_tile_width = v; }
		void SetTileHeight(int v)            { global_tile_height = v; }
		void SetVertiGlobalTileNo(short v)   { verti_global_tile_no = v; }
		void ShowInfo();
		void ShowNetInfo();

	private:
		short hori_global_tile_no;
		short verti_global_tile_no;
		short layer_no;
		int global_tile_width;
		int global_tile_height;
		int lower_left_x;
		int lower_left_y;

		vector <short> verti_capacityV;
		vector <short> hori_capacityV;
		vector <short> min_widthV;
		vector <short> min_spacingV;
		vector <short> via_spacingV;

		vector <CapacityAdjust> capacity_adjustV;
		vector <SubNet> subnetV;
		vector <Net> netV;
		vector <Pin> pinV;

		map <int, int> netUidPosM; //hash of net's uid <-> position in vector
};



#endif
