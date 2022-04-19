// Datastructures.hh

#ifndef DATASTRUCTURES_HH
#define DATASTRUCTURES_HH

#include <string>
#include <vector>
#include <list>
#include <utility>
#include <limits>
#include <unordered_map>
#include <map>

// Types for IDs
using StopID = long int;
using RegionID = std::string;
using Name = std::string;

// Return values for cases where required thing was not found
StopID const NO_STOP = -1;
RegionID const NO_REGION = "!!NO_REGION!!";

// Return value for cases where integer values were not found
int const NO_VALUE = std::numeric_limits<int>::min();

// Return value for cases where name values were not found
Name const NO_NAME = "!!NO_NAME!!";

// Type for a coordinate (x, y)
struct Coord
{
    int x = NO_VALUE;
    int y = NO_VALUE;
};

// Return value for cases where coordinates were not found
Coord const NO_COORD = {NO_VALUE, NO_VALUE};

// Example: Defining == and hash function for Coord so that it can be used
// as key for std::unordered_map/set, if needed
inline bool operator==(Coord c1, Coord c2) { return c1.x == c2.x && c1.y == c2.y; }
inline bool operator!=(Coord c1, Coord c2) { return !(c1==c2); } // Not strictly necessary

// Example: Defining < for Coord so that it can be used
// as key for std::map/set
inline bool operator<(Coord c1, Coord c2)
{
    if (c1.y < c2.y) { return true; }
    else if (c2.y < c1.y) { return false; }
    else { return c1.x < c2.x; }
}


struct Region {
    RegionID id;
    Name name;
    RegionID parent; //Parent region.
    std::vector<RegionID> children; //The subregions belonging directly to that region.
    std::vector<StopID> region_stops; //The stops belonging directly to that region.
};

struct Stop {
    StopID id;
    Name name;
    Coord location;
    RegionID parent; //Parent region.
};



class Datastructures
{
public:
    Datastructures();
    ~Datastructures();

    int stop_count();

    void clear_all();

    std::vector<StopID> all_stops();

    bool add_stop(StopID id, Name const& name, Coord xy);

    Name get_stop_name(StopID id);

    Coord get_stop_coord(StopID id);

    std::vector<StopID> stops_alphabetically();
    std::vector<StopID> stops_coord_order();

    StopID min_coord();
    StopID max_coord();

    std::vector<StopID> find_stops(Name const& name);

    bool change_stop_name(StopID id, Name const& newname);
    bool change_stop_coord(StopID id, Coord newcoord);

    bool add_region(RegionID id, Name const& name);
    Name get_region_name(RegionID id);
    std::vector<RegionID> all_regions();

    bool add_stop_to_region(StopID id, RegionID parentid);
    bool add_subregion_to_region(RegionID id, RegionID parentid);
    std::vector<RegionID> stop_regions(StopID id);
    
    void creation_finished();
    std::pair<Coord, Coord> region_bounding_box(RegionID id);
    std::vector<StopID> stops_closest_to(StopID id);
    bool remove_stop(StopID id);
    RegionID stops_common_region(StopID id1, StopID id2);

private:

    std::unordered_map<StopID, Stop> stops;
    std::unordered_map<RegionID, Region> regions;

    std::vector<StopID> sorted_alph;
    std::vector<StopID> unsorted_alph;

    std::vector<StopID> sorted_coord;
    std::vector<StopID> unsorted_coord;

    bool flag_alph=false;
    bool flag_coord=false;

    void get_region_subregions(RegionID id, std::vector<RegionID>& v);
    std::list<StopID> get_all_region_stops(RegionID id);

    void get_region_superregions(RegionID id, std::vector<RegionID>& v);

};

#endif // DATASTRUCTURES_HH
