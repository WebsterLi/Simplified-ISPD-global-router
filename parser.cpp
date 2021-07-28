#include "parser.h"
#include <iomanip>
#include <cassert>
#include <cstdlib>

using namespace std;
extern RoutingDB db;


void Parser::ReadISPD(ifstream& file)
{
	enum Reading {none, verti_capacity, hori_capacity, 
		min_width, min_spacing, via_spacing,
		tile_info, net_info, capacity_adjust};


	string pattern;
	int int_pattern;
	int net_no;

	Reading read_object;
	read_object = none;


	while (file >> pattern)
	{
		if (pattern == "grid") //grid
		{ 
			file >> int_pattern; 
			db.SetHoriGlobalTileNo(int_pattern); 

			file >> int_pattern; 
			db.SetVertiGlobalTileNo(int_pattern); 

			file >> int_pattern; 
			db.SetLayerNo(int_pattern); 
		}
		else if (pattern == "vertical") //vertical capacity
		{ 
			file >> pattern;
			read_object = verti_capacity; 
		}
		else if (pattern == "horizontal") //horizontal capacity
		{ 
			file >> pattern;
			read_object = hori_capacity; 
		}
		else if (pattern == "minimum")
		{
			file >> pattern;
			if (pattern == "width") //minimum width
			{
				read_object = min_width; 
			}
			else if (pattern == "spacing") //minimum spacing
			{
				read_object = min_spacing; 
			}
		}
		else if (pattern == "via") //via spacing
		{
			file >> pattern;
			read_object = via_spacing; 
		}
		else if (pattern == "num") //num net
		{
			file >> pattern;
			file >> net_no;
			db.ReserveNetNo(net_no);
			read_object = net_info; 
		}

		else if (read_object == verti_capacity)
		{
			db.AddVertiCapacity(atoi(pattern.c_str()));
			for (int i = 1; i < db.GetLayerNo(); i++)
			{
				file >> int_pattern; 
				db.AddVertiCapacity(int_pattern);
			}
		}
		else if (read_object == hori_capacity)
		{
			db.AddHoriCapacity(atoi(pattern.c_str()));
			for (int i = 1; i < db.GetLayerNo(); i++)
			{
				file >> int_pattern; 
				db.AddHoriCapacity(int_pattern);
			}
		}
		else if (read_object == min_width)
		{
			db.AddMinWidth(atoi(pattern.c_str()));
			for (int i = 1; i < db.GetLayerNo(); i++)
			{
				file >> int_pattern; 
				db.AddMinWidth(int_pattern);
			}
		}
		else if (read_object == min_spacing)
		{
			db.AddMinSpacing(atoi(pattern.c_str()));
			for (int i = 1; i < db.GetLayerNo(); i++)
			{
				file >> int_pattern; 
				db.AddMinSpacing(int_pattern);
			}
		}
		else if (read_object == via_spacing)
		{
			db.AddViaSpacing(atoi(pattern.c_str()));
			for (int i = 1; i < db.GetLayerNo(); i++)
			{
				file >> int_pattern; 
				db.AddViaSpacing(int_pattern);
			}
			read_object = tile_info;
		}
		else if (read_object == tile_info)
		{
			db.SetLowerLeftX(atoi(pattern.c_str()));
			file >> int_pattern; 
			db.SetLowerLeftY(int_pattern);
			file >> int_pattern; 
			db.SetTileWidth(int_pattern);
			file >> int_pattern; 
			db.SetTileHeight(int_pattern);
		}
		else if (read_object == net_info)
		{
			for (int i = 0; i < net_no; i++)
			{
				string name;
				if (i != 0)
				{ file >> name; } //name
				else
				{ name = pattern; }

				int id; 
				file >> id;
				int pin_no; 
				file >> pin_no;
				int min_width; 
				file >> min_width;

				assert (min_width == 1);

				if (pin_no > 1000)
				{
					cout << "..ignore net " << name << " id: " << id <<
						" pin no: " << pin_no << " (> 1000)" << endl;
					for (int j = 0; j < pin_no; j++)
					{
						int x; 
						file >> x;
						int y; 
						file >> y;
						int layer; 
						file >> layer;
					}
				}
				else
				{
					db.AddNet(name, id);


					Net& n = db.GetNetByPosition(db.GetNetNo()-1);

					n.ReservePinNo(pin_no);

					int non_redundant_pin_no = 0;
					for (int j = 0; j < pin_no; j++)
					{
						int x; 
						file >> x;
						int y; 
						file >> y;

						int layer; 
						file >> layer;

						assert (layer == 1);

						int gx = db.CalGx(x);
						int gy = db.CalGy(y);

						/* filter out redundancy pin */
						if (n.GetPinUidByTile(gx, gy, layer) == -1)
						{
							db.AddPin(gx, gy, layer);
							n.AddPinUid(db.GetPinNo()-1);
							n.AddPinUidTileMap(db.GetPinNo()-1, gx, gy, layer);
							non_redundant_pin_no++;
						}
					}

					if (non_redundant_pin_no == 1)
					{
						db.RemoveLastPin();
						n.ClearPinUidV();
						db.RemoveLastNet();
					}
					else
					{ 
						/* net input id and uid may be different */
						db.AddNetUidPositionMap(id, db.GetNetNo()-1);
						/* clear information for redundant pin */
						n.ClearPinUidTileMap(); 
					}
				}
			}
			read_object = capacity_adjust; 
		}
		else if (read_object == capacity_adjust) //capacity adjustment
		{
			int adjust_no = atoi(pattern.c_str());
			db.ReserveCapacityAdjustNo(adjust_no);
			for (int i = 0; i < adjust_no; i++)
			{
				short gx1;
				short gy1;
				short layer1;
				short gx2;
				short gy2;
				short layer2;
				int reduce_capacity;
				file >> gx1 >> gy1 >> layer1 
					>> gx2 >> gy2 >> layer2 >> reduce_capacity;
				/* no via capacity adjustment */
				assert (layer1 == layer2);

				db.AddCapapcityAdjust(gx1, gy1, layer1, 
						gx2, gy2, layer2, reduce_capacity);
			}
			read_object = none;
		}
	}
	cout << endl;
}

