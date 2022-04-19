// Datastructures.hh

#ifndef DATASTRUCTURES_HH
#define DATASTRUCTURES_HH

#include <string>
#include <vector>
#include <tuple>
#include <utility>
#include <limits>
#include <unordered_map>
#include <list>
#include <algorithm>

// Types for IDs
using StopID = long int;
using RegionID = std::string;
using RouteID = std::string;
using Name = std::string;

// Return values for cases where required thing was not found
RouteID const NO_ROUTE = "!!NO_ROUTE!!";
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

// Return value for cases where coordinates were not found
Coord const NO_COORD = {NO_VALUE, NO_VALUE};

// Type for time of day in minutes from midnight (i.e., 60*hours + minutes)
using Time = int;

// Return value for cases where color was not found
Time const NO_TIME = std::numeric_limits<Time>::min();

// Type for a duration of time (in minutes)
using Duration = int;

// Return value for cases where Duration is unknown
Duration const NO_DURATION = NO_VALUE;

// Type for a distance (in metres)
using Distance = int;

// Return value for cases where Duration is unknown
Distance const NO_DISTANCE = NO_VALUE;

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
    std::unordered_map<RouteID, StopID> routes_out; //Routes going out from this stop and the next stop on that route.
};


// This is the class you are supposed to implement

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

    std::vector<RouteID> all_routes();
    bool add_route(RouteID id, std::vector<StopID> route_stops);
    std::vector<std::pair<RouteID, StopID>> routes_from(StopID stopid);
    std::vector<StopID> route_stops(RouteID id);
    void clear_routes();


    std::vector<std::tuple<StopID, RouteID, Distance>> journey_any(StopID fromstop, StopID tostop);
    std::vector<std::tuple<StopID, RouteID, Distance>> journey_least_stops(StopID fromstop, StopID tostop);
    std::vector<std::tuple<StopID, RouteID, Distance>> journey_with_cycle(StopID fromstop);
    std::vector<std::tuple<StopID, RouteID, Distance>> journey_shortest_distance(StopID fromstop, StopID tostop);

    bool add_trip(RouteID routeid, const std::vector<Time> &stop_times);
    std::vector<std::pair<Time, Duration> > route_times_from(RouteID routeid, StopID stopid);
    std::vector<std::tuple<StopID, RouteID, Time>> journey_earliest_arrival(StopID fromstop, StopID tostop, Time starttime);

    void add_walking_connections();

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

    std::unordered_map<RouteID, std::vector<StopID>> routes;
    std::unordered_map<RouteID, std::vector<std::vector<Time>>> trips;
    Distance calc_dist(StopID a, StopID b);
};


#endif // DATASTRUCTURES_HH
