#include "routingdb.h"
#include <string>
#include <cmath>
#include <iomanip>
#include <cassert>

using namespace std;

extern RoutingDB db;


/* ======================== Pin ====================================== */
void Pin::ShowInfo(){
	cout << "Pin[" << GetUid() << "]: "
		<< "(" << GetGx() << "," << GetGy() << ") layer " << GetLayer() << endl;
}



/* ======================== SubNet ===================================== */
int SubNet::GetType(){//calculate line # of outputfile in L-Route.
	if (GetSourcePin().GetGy() == GetTargetPin().GetGy()){
		return 1;
	}
	else if(GetSourcePin().GetGx() == GetTargetPin().GetGx()){
		return 3;//V + via up & down.
	}
	else {
		return 4;//V + H + via*2.
	}
}

void SubNet::ShowInfo(){
	cout << "subnet[" << GetUid() << "] of net[" << GetNetUid() << "]" << endl;
	cout << "....source pin: "; GetSourcePin().ShowInfo();
	cout << "....target pin: "; GetTargetPin().ShowInfo();
}


/* ======================== Net ====================================== */
void Net::AddPinUidTileMap(int pin_id, short gx, short gy, short layer){
	//assert (layer == 1);
	int key = gx*1000+gy;
	tilePinUidM[key] = pin_id;
}

Pin& Net::GetPin(int id){
	int pin_uid = GetPinUid(id);
	return db.GetPin(pin_uid);
}


SubNet& Net::GetSubNet(int id){
	int subnet_uid = GetSubNetUid(id);
	return db.GetSubNet(subnet_uid);
}


int Net::GetPinUidByTile(short gx, short gy, short layer){
	//assert (layer == 1);
	map <int, int>::iterator ans;
	int key = gx*1000+gy;

	ans = tilePinUidM.find(key);
	if (ans == tilePinUidM.end()){
		/* not find */
		return -1;
	}
	else{
		/* find */
		return tilePinUidM[key];
	}
}



void Net::ShowInfo(){
	cout << "..net[" << GetUid() << "] " << GetName() << endl;
	cout << "...number of pin: " << GetPinNo() << endl;
	for (int i = 0; i < GetPinNo(); i++){
		Pin& p = GetPin(i);
		cout << "...."; 
		p.ShowInfo();
	}

	if (GetSubNetNo() > 0){
		cout << "...subnets: " << GetSubNetNo() << endl;
		for (int i = 0; i < GetSubNetNo(); i++)
		{
			SubNet& e = GetSubNet(i);
			cout << "...."; 
			e.ShowInfo();
		}
	}
	cout << endl;
}


void Net::ShowSubNetInfo(){
	cout << "..net[" << GetUid() << "] " << GetName() << endl;

	cout << "...subnets: " << GetSubNetNo() << endl;
	for (int i = 0; i < GetSubNetNo(); i++){
		SubNet& e = GetSubNet(i);
		cout << "...."; 
		e.ShowInfo();
	}
	cout << endl;
}




/* ======================== RoutingDB ================================ */

	
void RoutingDB::AddCapapcityAdjust(short gx1, short gy1, short layer1, 
		short gx2, short gy2, short layer2, short reduce_capacity){
	capacity_adjustV.push_back(CapacityAdjust(gx1, gy1, layer1, 
				gx2, gy2, layer2, reduce_capacity));
}

void RoutingDB::AddSubNet(Net& n, Pin& source_pin, Pin& target_pin){
	subnetV.push_back(SubNet(GetSubNetNo(), n, source_pin, target_pin));
}


void RoutingDB::AddNetUidPositionMap(int uid, int pos){ netUidPosM[uid] = pos; }

void RoutingDB::AddPin(short gx, short gy, short layer){ 
	//assert (layer == 1);
	pinV.push_back(Pin(GetPinNo(), gx, gy)); 
}


int RoutingDB::CalCenterX(short gx){
	int x = (int) (GetLowerLeftX() + (gx+0.5)*GetTileWidth());
	return x;
}

int RoutingDB::CalCenterY(short gy){
	int y = (int) (GetLowerLeftY() + (gy+0.5)*GetTileHeight());
	return y;
}


short RoutingDB::CalGx(int x){
	short gx = (x-GetLowerLeftX())/GetTileWidth();
	return gx;
}


short RoutingDB::CalGy(int y){
	short gy = (y-db.GetLowerLeftY())/db.GetTileHeight();
	return gy;
}

CapacityAdjust& RoutingDB::GetCapacityAdjust(int id)
{ return capacity_adjustV[id]; }

int RoutingDB::GetChipHeight()                  
{ return GetVertiGlobalTileNo()*GetTileHeight(); }

int RoutingDB::GetChipWidth()                  
{ return GetHoriGlobalTileNo()*GetTileWidth(); }


void RoutingDB::ShowInfo(){
	cout << "RoutingDB Info: " << endl;

	cout << "..# of horizontal global tiles: " 
		<< GetHoriGlobalTileNo() << endl;
	cout << "..# of vertical global tiles: " 
		<< GetVertiGlobalTileNo() << endl;
	cout << "..# of layers: " << GetLayerNo() << endl;

	cout << "..vertical capacity: "; 
	for (int i = 0; i < GetLayerNo(); i++)
	{ cout << GetLayerVertiCapacity(i) << " "; }
	cout << endl;

	cout << "..horizontal capacity: "; 
	for (int i = 0; i < GetLayerNo(); i++)
	{ cout << GetLayerHoriCapacity(i) << " "; }
	cout << endl;

	cout << "..minimum width: "; 
	for (int i = 0; i < GetLayerNo(); i++)
	{ cout << GetLayerMinWidth(i) << " "; }
	cout << endl;

	cout << "..minimum spacing: "; 
	for (int i = 0; i < GetLayerNo(); i++)
	{ cout << GetLayerMinSpacing(i) << " "; }
	cout << endl;

	cout << "..via spacing: "; 
	for (int i = 0; i < GetLayerNo(); i++)
	{ cout << GetLayerViaSpacing(i) << " "; }
	cout << endl;

	cout << "..chip lower left x: " << GetLowerLeftX() << endl; 
	cout << "..chip lower left y: " << GetLowerLeftY() << endl; 
	cout << "..global tile width: " << GetTileWidth() << endl; 
	cout << "..global tile height: " << GetTileHeight() << endl; 

	cout << "..# of capacity adjustment: " << GetCapacityAdjustNo() << endl;
	cout << endl;

	cout << "..# of net: " << GetNetNo() << endl;

	for (int i = 0; i < GetNetNo(); i++)
	{
		Net& n = GetNetByPosition(i);
		n.ShowInfo();
	}

	cout << endl;
}


void RoutingDB::ShowNetInfo(){
	cout << "Net/SubNet Info: " << endl;

	for (int i = 0; i < GetNetNo(); i++)
	{
		Net& n = GetNetByPosition(i);
		n.ShowSubNetInfo();
	}

	cout << endl;
}


