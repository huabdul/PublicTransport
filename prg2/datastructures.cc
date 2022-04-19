// Datastructures.cc

#include "datastructures.hh"

#include <random>
#include <cmath>
#include <stdexcept>
#include <unordered_map>
#include <list>
#include <algorithm>
#include <stack>
#include <queue>
#include <QDebug>
#include <set>

std::minstd_rand rand_engine; // Reasonably quick pseudo-random generator

template <typename Type>
Type random_in_range(Type start, Type end)
{
    auto range = end-start;
    ++range;

    auto num = std::uniform_int_distribution<unsigned long int>(0, range-1)(rand_engine);

    return static_cast<Type>(start+num);
}


Datastructures::Datastructures()
{
    // Replace this comment with your implementation
}

Datastructures::~Datastructures()
{
    clear_all();
}

int Datastructures::stop_count()
{
    return stops.size();
}

std::vector<StopID> Datastructures::all_stops()
{
    std::vector<StopID> s;

    for (auto it=stops.begin(); it!=stops.end(); it++) {
        s.push_back(it->first);
    }

    return s;
}

bool Datastructures::add_stop(StopID id, const Name& name, Coord xy)
{
    if (stops.find(id) != stops.end()) {return false;}

    Stop new_stop;
    new_stop.id = id;
    new_stop.name = name;
    new_stop.location = xy;
    new_stop.parent = NO_REGION;

    stops.insert(std::pair<StopID, Stop>(id, new_stop));

    unsorted_alph.push_back(id);
    unsorted_coord.push_back(id);

    return true;
}

Name Datastructures::get_stop_name(StopID id)
{
    if (stops.find(id) == stops.end()) {return NO_NAME;}
    return stops[id].name;
}

Coord Datastructures::get_stop_coord(StopID id)
{
    if (stops.find(id) == stops.end()) {return NO_COORD;}
    return stops[id].location;
}

std::vector<StopID> Datastructures::stops_alphabetically()
{
    auto compare_alph = [&](StopID i, StopID j)-> bool {
        return (stops[i].name < stops[j].name);
    };

    if (flag_alph == true || sorted_alph.size() == 0) {
        //Sort from scratch
        std::vector<StopID> temp;

        for (auto it=stops.begin(); it!=stops.end(); it++) { //O(n)
            temp.push_back(it->first);
        }

        std::sort(temp.begin(), temp.end(), compare_alph); //O(nlog(n))

        sorted_alph = temp;
        unsorted_alph.clear();
        flag_alph = false;

        return sorted_alph;
    }

    if (unsorted_alph.size() > 0) {
        //Sort the unsorted part and merge it with the sorted one.
        std::vector<StopID> sorted(sorted_alph.size()+unsorted_alph.size());

        //That will be mlog(m) where m is the unsorted vector size.
        std::sort(unsorted_alph.begin(), unsorted_alph.end(), compare_alph);

        //Merging operation is O(1+n-m) where n is the sorted vector size and m is the unsorted vector size.
        std::merge(sorted_alph.begin(), sorted_alph.end(), unsorted_alph.begin(), unsorted_alph.end(), sorted.begin(), compare_alph);
        sorted_alph = sorted;
        unsorted_alph.clear();
    }

    //Otherwise, if everything has been sorted previously, just return the sorted_alph vector
    return sorted_alph;
}


//Exactly the same as the pervious function, but with different comparison criteria.
std::vector<StopID> Datastructures::stops_coord_order()
{
    auto compare_coord = [&](StopID i, StopID j)-> bool {
        Stop s1 = stops[i]; Stop s2 = stops[j];
        long int dist1 = pow(s1.location.x, 2) + pow(s1.location.y, 2);
        long int dist2 = pow(s2.location.x, 2) + pow(s2.location.y, 2);
        //Handle the case when distances are the same.
        if (dist1 == dist2) {return s1.location.y < s2.location.y;}
        return (dist1 < dist2);
    };

    if (flag_coord == true || sorted_coord.size() == 0) {
        //Sort from scratch
        std::vector<StopID> temp;

        for (auto it=stops.begin(); it!=stops.end(); it++) { //O(n)
            temp.push_back(it->first);
        }

        std::sort(temp.begin(), temp.end(), compare_coord); //O(nlog(n))

        sorted_coord = temp;
        unsorted_coord.clear();
        flag_coord = false;

        return sorted_coord;
    }

    if (unsorted_coord.size() > 0) {
        //Merge
        std::vector<StopID> sorted(sorted_coord.size()+unsorted_coord.size());

        std::sort(unsorted_coord.begin(), unsorted_coord.end(), compare_coord);

        std::merge(sorted_coord.begin(), sorted_coord.end(), unsorted_coord.begin(), unsorted_coord.end(), sorted.begin(), compare_coord);
        sorted_coord = sorted;
        unsorted_coord.clear();
    }

    //Otherwise, just return the sorted_coord vector
    return sorted_coord;
}

StopID Datastructures::min_coord()
{
    if (stop_count() == 0) {return NO_STOP;}

    int long min_dist = std::numeric_limits<int>::max();
    int long min_id = -1;

    for (auto it=stops.begin(); it!=stops.end(); it++) {
        int long dist = pow(it->second.location.x, 2) + pow(it->second.location.y, 2);
        if (dist < min_dist) {min_dist = dist; min_id = it->first;}

        //Handle the case when distances are the same
        if (dist == min_dist) {
            if (it->second.location.y < stops[min_id].location.y) {
                min_dist = dist; min_id = it->first;
            }
        }
    }
    return min_id;
}

StopID Datastructures::max_coord()
{
    if (stop_count() == 0) {return NO_STOP;}

    int long max_dist = -1;
    int long max_id = -1;

    for (auto it=stops.begin(); it!=stops.end(); it++) {
        int long dist = pow(it->second.location.x, 2) + pow(it->second.location.y, 2);
        if (dist > max_dist) {max_dist = dist; max_id = it->first;}

        //Handle the case when distances are the same
        if (dist == max_dist) {
            if (it->second.location.y > stops[max_id].location.y) {
                max_dist = dist; max_id = it->first;
            }
        }
    }
    return max_id;
}

std::vector<StopID> Datastructures::find_stops(Name const& name)
{
    //Basic linear search O(n)
    std::vector<StopID> matches;
    for (auto it=stops.begin(); it!=stops.end(); it++) {
        if (it->second.name == name) {matches.push_back(it->first);}
    }
    return matches;
}

bool Datastructures::change_stop_name(StopID id, const Name& newname)
{
    if (stops.find(id) == stops.end()) {return false;}
    stops[id].name = newname;
    flag_alph = true;
    return true;
}

bool Datastructures::change_stop_coord(StopID id, Coord newcoord)
{
    if (stops.find(id) == stops.end()) {return false;}
    stops[id].location = newcoord;
    flag_coord = true;
    return true;
}

bool Datastructures::add_region(RegionID id, const Name &name)
{
    Region new_region;
    new_region.id = id;
    new_region.name = name;
    new_region.parent = NO_REGION;

    auto ret = regions.insert(std::pair<RegionID, Region>(id, new_region));
    return ret.second;
}

Name Datastructures::get_region_name(RegionID id)
{
    if (regions.find(id) == regions.end()) {return NO_NAME;}
    return regions[id].name;
}

std::vector<RegionID> Datastructures::all_regions()
{
    std::vector<RegionID> r;

    for (auto it=regions.begin(); it!=regions.end(); it++) {
        r.push_back(it->first);
    }

    return r;
}

bool Datastructures::add_stop_to_region(StopID id, RegionID parentid)
{
    if (stops.find(id) == stops.end()) {return false;}
    if (regions.find(parentid) == regions.end()) {return false;}
    if (stops[id].parent != NO_REGION) {return false;}

    stops[id].parent = parentid; //O(1) average
    regions[parentid].region_stops.push_back(id); //O(1) amortized
    return true;
}

bool Datastructures::add_subregion_to_region(RegionID id, RegionID parentid)
{
    if (regions.find(id) == regions.end()) {return false;}
    if (regions.find(parentid) == regions.end()) {return false;}
    if (regions[id].parent != NO_REGION) {return false;}

    regions[id].parent = parentid; //O(1) average
    regions[parentid].children.push_back(id); //O(1) amortized
    return true;
}

std::vector<RegionID> Datastructures::stop_regions(StopID id)
{
    if (stops.find(id) == stops.end()) {return {NO_REGION};}

    std::vector<RegionID> r;
    RegionID current_region = stops[id].parent;

    while (true) {
        r.push_back(current_region); //O(1) amortized
        current_region = regions[current_region].parent; //O(1) average
        if (current_region == NO_REGION) {break;}
    }
    return r;
}

void Datastructures::creation_finished()
{
}

void Datastructures::get_region_subregions(RegionID id, std::vector<RegionID>& v) { //O(k)
    //Get all the subregions within a region.
    std::vector<RegionID> sub = regions[id].children;

    for (auto it=sub.begin(); it!=sub.end(); it++) { //O(k), k is number of regions
       v.push_back(*it);
       get_region_subregions(*it, v);
    }
}

std::list<StopID> Datastructures::get_all_region_stops(RegionID id) { //O(k*m) ~= O(n)
    //Get all the stops contained within a region directly or indirectly (i.e. within its subregions).
    std::vector<RegionID> sub = {id};
    get_region_subregions(id, sub); //O(k)

    std::list<StopID> all_region_stops;

    for (auto it=sub.begin(); it!=sub.end(); it++) { //O(k)
        std::vector<StopID> current_stops = regions[*it].region_stops;
        //O(m) where m is the average number of stops per region
        all_region_stops.insert(all_region_stops.end(), current_stops.begin(), current_stops.end());
    }

    return all_region_stops;
}

std::pair<Coord,Coord> Datastructures::region_bounding_box(RegionID id)
{
    if (regions.find(id) == regions.end()) {return {NO_COORD, NO_COORD};}

    std::list<StopID> all_region_stops = get_all_region_stops(id); //O(n)
    if (all_region_stops.size() == 0) {return {NO_COORD, NO_COORD};}

    long int minx = std::numeric_limits<int>::max();
    long int miny = std::numeric_limits<int>::max();
    long int maxx = std::numeric_limits<int>::min();
    long int maxy = std::numeric_limits<int>::min();
    Coord max_coord, min_coord;

    for (auto it=all_region_stops.begin(); it!=all_region_stops.end(); it++) { //O(n)
        long int x = stops[*it].location.x; long int y = stops[*it].location.y;
        if (x < minx) {minx=x;}
        if (y < miny) {miny=y;}
        if (x > maxx) {maxx=x;}
        if (y > maxy) {maxy=y;}
    }
    min_coord.x = minx; min_coord.y = miny;
    max_coord.x = maxx; max_coord.y = maxy;
    return {min_coord, max_coord};
}

std::vector<StopID> Datastructures::stops_closest_to(StopID id)
{
    //If no stop with the given ID exists, return nothing.
    if (stops.find(id) == stops.end()) {return {NO_STOP};}

    //Initialize the return vector.
    std::vector<StopID> s;

    //Define the to-be-sorted vector (temp) and the stop coordinates.
    std::vector<Stop> temp;
    long int X = stops[id].location.x;
    long int Y = stops[id].location.y;

    //Add all Stop objects (except the given stop) from the map to the temp vector.
    for (auto it=stops.begin(); it!=stops.end(); it++) { //O(n)
        if (it->first != id) {temp.push_back(it->second);}
    }

    //Define the criteria for sorting (i.e. proximity the given stop).
    auto comp = [&](Stop i, Stop j)-> bool {
        long int dist1 = pow(i.location.x - X, 2) + pow(i.location.y - Y, 2);
        long int dist2 = pow(j.location.x - X, 2) + pow(j.location.y - Y, 2);
        return (dist1 < dist2);
    };

    //Sort and take only the first 5 (or less) elements.
    std::sort(temp.begin(), temp.end(), comp); //O(nlog(n))
    if (temp.size() > 5) {temp.resize(5);}

    //Extract the IDs from the storted stops and return them.
    for (auto it=temp.begin(); it!=temp.end(); it++) { //O(n)
        s.push_back(it->id);
    }

    return s;
}

bool Datastructures::remove_stop(StopID id)
{
    //If no stop with the given ID exists, return false.
    if (stops.find(id) == stops.end()) {return false;}
    //Otherwise, erase it from its region's children vector.
    Stop s = stops[id];
    std::vector<StopID> ch = regions[s.parent].region_stops;

    for (auto it=ch.begin(); it!=ch.end(); it++) { //O(m)
        if (*it == id) {ch.erase(it); break;}
    }
    regions[s.parent].region_stops = ch; //O(n)

    //Then erase it from the sorted and unsorted vectors O(n).
    for (auto it=sorted_alph.begin(); it!=sorted_alph.end(); it++) {
        if (*it == id) {sorted_alph.erase(it); break;}
    }
    for (auto it=sorted_coord.begin(); it!=sorted_coord.end(); it++) {
        if (*it == id) {sorted_coord.erase(it); break;}
    }
    for (auto it=unsorted_alph.begin(); it!=unsorted_alph.end(); it++) {
        if (*it == id) {unsorted_alph.erase(it); break;}
    }
    for (auto it=unsorted_coord.begin(); it!=unsorted_coord.end(); it++) {
        if (*it == id) {unsorted_coord.erase(it); break;}
    }

    //Finally, erase the stop from the stops map.
    stops.erase(id); //O(n)

    return true;
}

void Datastructures::get_region_superregions(RegionID id, std::vector<RegionID>& v) { //O(k)
    //Get all the direct and indirect parents of a given region.
    RegionID parent = regions[id].parent;
    if (parent != NO_REGION) {
        v.push_back(parent);
        get_region_superregions(parent, v);
    }
}

RegionID Datastructures::stops_common_region(StopID id1, StopID id2)
{
    if (stops.find(id1) == stops.end()) {return NO_REGION;}
    if (stops.find(id2) == stops.end()) {return NO_REGION;}

    //Climb up the hierarchy to find all regions to which each stop belongs.
    std::vector<RegionID> r1 = {stops[id1].parent};
    std::vector<RegionID> r2 = {stops[id2].parent};

    get_region_superregions(r1.front(), r1); //O(k)
    get_region_superregions(r2.front(), r2); //O(k)

    //Sort the regions of the second stop.
    std::sort(r2.begin(), r2.end()); //O(klog(k))

    //For each element in r1 (in order, from lowest to highest in heirarchy), do a binary search to see if a match exists in r2.
    for (auto it=r1.begin();it!=r1.end();it++) { //O(klog(k))
        if (std::binary_search(r2.begin(), r2.end(), *it)) {
            return *it;
            break;
        }
    }

    //If all else failed, there is no common region.
    return NO_REGION;
}

//PHASE (2) IMPLEMENTATION

void Datastructures::clear_all()
{
    stops.clear();
    regions.clear();
    sorted_alph.clear();
    unsorted_alph.clear();
    sorted_coord.clear();
    unsorted_coord.clear();
    flag_alph = false;
    flag_coord = false;

    routes.clear();
    trips.clear();
}

std::vector<RouteID> Datastructures::all_routes()
{
    std::vector<RouteID> r;

    for (auto it=routes.begin(); it!=routes.end(); it++) {
        r.push_back(it->first);
    }

    return r;
}

bool Datastructures::add_route(RouteID id, std::vector<StopID> route_stops)
{
    //If there already exists a route with the same ID.
    if (routes.find(id) != routes.end()) {return false;}

    //Or if some stop's ID is not found (a stop in the route doesn't exist).
    for (auto it=route_stops.begin(); it!=route_stops.end(); it++) {
        if (stops.find(*it) == stops.end()) {return false; break;}
    }

    //Or if there is given only one stop.
    if (route_stops.size() <= 1) {return false;}

    //Otherwise, add the new route.
    routes.insert(std::pair<RouteID, std::vector<StopID>>(id, route_stops));

    for (auto it=route_stops.begin(); it!=std::prev(route_stops.end()); it++) {
        stops[*it].routes_out.insert(std::pair<RouteID, StopID>(id, *std::next(it)));
    }

    return true;
}

std::vector<std::pair<RouteID, StopID>> Datastructures::routes_from(StopID stopid)
{
    if (stops.find(stopid) == stops.end()) {return {{NO_ROUTE, NO_STOP}};}

    std::vector<std::pair<RouteID, StopID>> required_routes;

    for (auto &pair : stops[stopid].routes_out) {
        required_routes.push_back(pair);
    }

    return required_routes;
}

std::vector<StopID> Datastructures::route_stops(RouteID id)
{
    if (routes.find(id) == routes.end()) {return {NO_STOP};}

    return routes[id];
}

void Datastructures::clear_routes()
{
    for (auto it=stops.begin(); it!=stops.end(); it++) {
        it->second.routes_out.clear();
    }

    routes.clear();
}

Distance Datastructures::calc_dist(StopID a, StopID b) {
    return sqrt(pow(stops[a].location.x-stops[b].location.x, 2) + pow(stops[a].location.y-stops[b].location.y, 2));
}

std::vector<std::tuple<StopID, RouteID, Distance>> Datastructures::journey_any(StopID fromstop, StopID tostop)
{
    //If either of the stops isn't found.
    if (stops.find(fromstop) == stops.end() || stops.find(tostop) == stops.end()) {return {{NO_STOP, NO_ROUTE, NO_DISTANCE}};}
    //If the starting stop is the same as the destination stop.
    if (fromstop == tostop) {return {};}


    //The stack which will hold the fringe stops and the paths leading to them.
    std::stack<std::pair<StopID, std::vector<std::tuple<StopID, RouteID, Distance>>>> thestack;
    //A boolean map that holds the visited stops.
    std::unordered_map<StopID, bool> visited;
    //Initialize the stack with the starting stop.
    thestack.push(std::pair<StopID, std::vector<std::tuple<StopID, RouteID, Distance>>>(fromstop, {}));

    while (thestack.size() != 0) {

        auto popped = thestack.top();
        thestack.pop();

        StopID crntstop = popped.first;
        auto crntpath = popped.second;


        if (crntstop == tostop) {
            StopID prevstop = std::get<0>(crntpath.back());
            Distance nextdist = calc_dist(prevstop, tostop);
            Distance prevdist = std::get<2>(crntpath.back());
            crntpath.push_back(std::tuple<StopID, RouteID, Distance>(crntstop, NO_ROUTE, nextdist+prevdist));
            return crntpath;
        }

        visited[crntstop] = true;

        auto successors = routes_from(crntstop);

        for (auto it=successors.begin(); it!=successors.end(); it++) {
            if (visited[it->second] == true) {continue;}

            RouteID nextroute = it->first;
            StopID nextstop = it->second;
            Distance prevdist, nextdist;

            if (crntpath.size()==0) {prevdist=0; nextdist=0;}
            else {
                prevdist = std::get<2>(crntpath.back());
                StopID prevstop = std::get<0>(crntpath.back());
                nextdist = calc_dist(prevstop, crntstop);
            }

            auto newpath = crntpath;

            newpath.push_back(std::tuple<StopID, RouteID, Distance>(crntstop, nextroute, prevdist+nextdist));
            thestack.push(std::pair<StopID, std::vector<std::tuple<StopID, RouteID, Distance>>>(nextstop, newpath));
        }

    }

    return {};
}

std::vector<std::tuple<StopID, RouteID, Distance>> Datastructures::journey_least_stops(StopID fromstop, StopID tostop)
{
    //If either of the stops isn't found.
    if (stops.find(fromstop) == stops.end() || stops.find(tostop) == stops.end()) {return {{NO_STOP, NO_ROUTE, NO_DISTANCE}};}
    //If the starting stop is the same as the destination stop.
    if (fromstop == tostop) {return {};}

    std::queue<std::pair<StopID, std::vector<std::tuple<StopID, RouteID, Distance>>>> thequeue;

    std::unordered_map<StopID, bool> visited;

    thequeue.push(std::pair<StopID, std::vector<std::tuple<StopID, RouteID, Distance>>>(fromstop, {}));

    while (thequeue.size() != 0) {

        auto popped = thequeue.front();
        thequeue.pop();

        StopID crntstop = popped.first;
        auto crntpath = popped.second;


        if (crntstop == tostop) {
            StopID prevstop = std::get<0>(crntpath.back());
            Distance nextdist = calc_dist(prevstop, tostop);
            Distance prevdist = std::get<2>(crntpath.back());
            crntpath.push_back(std::tuple<StopID, RouteID, Distance>(crntstop, NO_ROUTE, nextdist+prevdist));
            return crntpath;
        }

        visited[crntstop] = true;

        auto successors = routes_from(crntstop);

        for (auto it=successors.begin(); it!=successors.end(); it++) {
            if (visited[it->second] == true) {continue;}

            RouteID nextroute = it->first;
            StopID nextstop = it->second;
            Distance prevdist, nextdist;

            if (crntpath.size()==0) {prevdist=0; nextdist=0;}
            else {
                prevdist = std::get<2>(crntpath.back());
                StopID prevstop = std::get<0>(crntpath.back());
                nextdist = calc_dist(prevstop, crntstop);
            }

            auto newpath = crntpath;

            newpath.push_back(std::tuple<StopID, RouteID, Distance>(crntstop, nextroute, prevdist+nextdist));
            thequeue.push(std::pair<StopID, std::vector<std::tuple<StopID, RouteID, Distance>>>(nextstop, newpath));
        }

    }

    return {};
}

std::vector<std::tuple<StopID, RouteID, Distance>> Datastructures::journey_with_cycle(StopID fromstop)
{
    //If the stop isn't found.
    if (stops.find(fromstop) == stops.end()) {return {{NO_STOP, NO_ROUTE, NO_DISTANCE}};}

    //The stack which will hold the fringe stops and the paths leading to them.
    std::stack<std::pair<StopID, std::vector<std::tuple<StopID, RouteID, Distance>>>> thestack;

    //Initialize the stack with the starting stop.
    thestack.push(std::pair<StopID, std::vector<std::tuple<StopID, RouteID, Distance>>>(fromstop, {}));

    while (thestack.size() != 0) {

        auto popped = thestack.top();
        thestack.pop();

        StopID crntstop = popped.first;
        auto crntpath = popped.second;

        for (auto &tuple : crntpath) {
            if (crntstop == std::get<0>(tuple)){
                StopID prevstop = std::get<0>(crntpath.back());
                Distance nextdist = calc_dist(prevstop, crntstop);
                Distance prevdist = std::get<2>(crntpath.back());
                crntpath.push_back(std::tuple<StopID, RouteID, Distance>(crntstop, NO_ROUTE, nextdist+prevdist));
                return crntpath;
            }
        }

        auto successors = routes_from(crntstop);

        for (auto it=successors.begin(); it!=successors.end(); it++) {

            RouteID nextroute = it->first;
            StopID nextstop = it->second;
            Distance prevdist, nextdist;

            if (crntpath.size()==0) {prevdist=0; nextdist=0;}
            else {
                prevdist = std::get<2>(crntpath.back());
                StopID prevstop = std::get<0>(crntpath.back());
                nextdist = calc_dist(prevstop, crntstop);
            }

            auto newpath = crntpath;

            newpath.push_back(std::tuple<StopID, RouteID, Distance>(crntstop, nextroute, prevdist+nextdist));
            thestack.push(std::pair<StopID, std::vector<std::tuple<StopID, RouteID, Distance>>>(nextstop, newpath));
        }

    }

    return {};
}

class PriorityQueueDistComp {
public:
    bool operator() (std::tuple<StopID, std::vector<std::tuple<StopID, RouteID, Distance>>, Distance> i, std::tuple<StopID, std::vector<std::tuple<StopID, RouteID, Distance>>, Distance> j) {
        return (std::get<2>(i) < std::get<2>(j));
    }
};

std::vector<std::tuple<StopID, RouteID, Distance>> Datastructures::journey_shortest_distance(StopID fromstop, StopID tostop)
{
    //If either of the stops isn't found.
    if (stops.find(fromstop) == stops.end() || stops.find(tostop) == stops.end()) {return {{NO_STOP, NO_ROUTE, NO_DISTANCE}};}
    //If the starting stop is the same as the destination stop.
    if (fromstop == tostop) {return {};}

    std::priority_queue<std::tuple<StopID, std::vector<std::tuple<StopID, RouteID, Distance>>, Distance>, std::vector<std::tuple<StopID, std::vector<std::tuple<StopID, RouteID, Distance>>, Distance>>, PriorityQueueDistComp> thequeue;

    std::unordered_map<StopID, bool> visited;

    thequeue.push(std::tuple<StopID, std::vector<std::tuple<StopID, RouteID, Distance>>, Distance>(fromstop, {}, 0));

    while (thequeue.size() != 0) {

        auto popped = thequeue.top();
        thequeue.pop();

        StopID crntstop = std::get<0>(popped);
        auto crntpath = std::get<1>(popped);

        if (crntstop == tostop) {
            StopID prevstop = std::get<0>(crntpath.back());
            Distance nextdist = calc_dist(prevstop, tostop);
            Distance prevdist = std::get<2>(crntpath.back());
            crntpath.push_back(std::tuple<StopID, RouteID, Distance>(crntstop, NO_ROUTE, nextdist+prevdist));
            return crntpath;
        }

        visited[crntstop] = true;

        auto successors = routes_from(crntstop);

        for (auto it=successors.begin(); it!=successors.end(); it++) {
            if (visited[it->second] == true) {continue;}

            RouteID nextroute = it->first;
            StopID nextstop = it->second;
            Distance prevdist, nextdist;

            if (crntpath.size()==0) {prevdist=0; nextdist=0;}
            else {
                prevdist = std::get<2>(crntpath.back());
                StopID prevstop = std::get<0>(crntpath.back());
                nextdist = calc_dist(prevstop, crntstop);
            }

            Distance cost = calc_dist(crntstop, nextstop) + nextdist + prevdist;

            auto newpath = crntpath;

            newpath.push_back(std::tuple<StopID, RouteID, Distance>(crntstop, nextroute, prevdist+nextdist));
            thequeue.push(std::tuple<StopID, std::vector<std::tuple<StopID, RouteID, Distance>>, Distance>(nextstop, newpath, cost));
        }
    }

    return {};
}

bool Datastructures::add_trip(RouteID routeid, std::vector<Time> const& stop_times)
{
    if (routes.find(routeid) == routes.end()) {return false;}

    trips[routeid].push_back(stop_times);
    return true;
}

std::vector<std::pair<Time, Duration>> Datastructures::route_times_from(RouteID routeid, StopID stopid)
{
    if (routes.find(routeid) == routes.end()) {return {{NO_TIME, NO_DURATION}};}
    if (stops.find(stopid) == stops.end()) {return {{NO_TIME, NO_DURATION}};}
    if (trips.find(routeid) == trips.end()) {return {{NO_TIME, NO_DURATION}};}


    std::vector<StopID> routestops = routes[routeid];
    auto it = std::find(routestops.begin(), --routestops.end(), stopid);

    //If the stop is not on the route or it's the last stop.
    if (it == routestops.end()) {return {{NO_TIME, NO_DURATION}};}

    int idx = std::distance(routestops.begin(), it);

    std::vector<std::pair<Time, Duration>> return_vector;

    for (auto &times : trips[routeid]) {
        Time departure = times[idx];
        Duration duration = times[idx+1] - times[idx];
        return_vector.push_back(std::pair<Time, Duration>(departure, duration));
    }

    return return_vector;
}

class PriorityQueueDurComp {
public:
    bool operator() (std::pair<StopID, std::vector<std::tuple<StopID, RouteID, Time>>> i, std::pair<StopID, std::vector<std::tuple<StopID, RouteID, Time>>> j) {
        return std::get<2>(i.second.back()) < std::get<2>(j.second.back());
    }
};

std::vector<std::tuple<StopID, RouteID, Time>> Datastructures::journey_earliest_arrival(StopID fromstop, StopID tostop, Time starttime)
{
    //If either of the stops isn't found.
    if (stops.find(fromstop) == stops.end() || stops.find(tostop) == stops.end()) {return {{NO_STOP, NO_ROUTE, NO_DISTANCE}};}
    //If the starting stop is the same as the destination stop.
    if (fromstop == tostop) {return {};}
    //Flag to mark the first stop
    bool flag = false;

    std::priority_queue<std::pair<StopID, std::vector<std::tuple<StopID, RouteID, Time>>>, std::vector<std::pair<StopID, std::vector<std::tuple<StopID, RouteID, Distance>>>>, PriorityQueueDurComp> thequeue;

    std::unordered_map<StopID, bool> visited;

    thequeue.push(std::pair<StopID, std::vector<std::tuple<StopID, RouteID, Time>>>(fromstop, {}));

    while (thequeue.size() != 0) {

        auto popped = thequeue.top();
        thequeue.pop();

        StopID crntstop = popped.first;
        auto crntpath = popped.second;

        if (crntstop == tostop) {
            StopID prevstop = std::get<0>(crntpath.back());
            RouteID prevroute = std::get<1>(crntpath.back());
            std::vector<StopID> routestops = routes[prevroute];

            auto it = std::find(routestops.begin(), routestops.end(), prevstop);
            int idx = std::distance(routestops.begin(), it);
            Time t = route_times_from(prevroute, prevstop)[idx].first + route_times_from(prevroute, prevstop)[idx].second;

            crntpath.push_back(std::tuple<StopID, RouteID, Distance>(crntstop, NO_ROUTE, t));
            return crntpath;
        }

        visited[crntstop] = true;

        auto successor_routes = routes_from(crntstop);
        std::vector<std::tuple<Time, Duration, RouteID, StopID>> successor_trips;

        for (auto &rs : successor_routes) {
            auto route_trips = route_times_from(rs.first, crntstop);
            if (route_trips == std::vector<std::pair<Time, Duration>>({{NO_TIME, NO_DURATION}})) {continue;}

            for (auto &td : route_trips) {
                if ((td.first >= starttime && td.first <= starttime + 20*60) || flag==true) {
                    successor_trips.push_back(std::tuple<Time, Duration, RouteID, StopID>(td.first, td.second, rs.first, rs.second));
                }
            }
        }

        flag = true;

        for (auto it=successor_trips.begin(); it!=successor_trips.end(); it++) {
            if (visited[std::get<3>(*it)] == true) {continue;}
            //Prevent journeys spanning midnight.
            if (crntpath.size()!=0 && std::get<0>(*it) <= std::get<2>(crntpath.back())) {continue;}

            RouteID nextroute = std::get<2>(*it);
            StopID nextstop = std::get<3>(*it);

            auto newpath = crntpath;

            newpath.push_back(std::tuple<StopID, RouteID, Time>(crntstop, nextroute, std::get<0>(*it)));
            thequeue.push(std::pair<StopID, std::vector<std::tuple<StopID, RouteID, Time>>>(nextstop, newpath));
        }
    }

    return {};
}

void Datastructures::add_walking_connections()
{
}
