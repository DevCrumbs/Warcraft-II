#include "Defs.h"
#include "p2Log.h"

#include "j1App.h"
#include "j1PathFinding.h"
#include "j1EntityFactory.h"
#include "j1Map.h"
#include "j1PathManager.h"
#include "j1Scene.h"

#include "Brofiler\Brofiler.h"

j1PathFinding::j1PathFinding() : j1Module()
{
	name.assign("pathfinding");
}

// Destructor
j1PathFinding::~j1PathFinding()
{
	trigger = nullptr;
}

// Called before quitting
bool j1PathFinding::CleanUp()
{
	distanceHeuristic = DistanceHeuristic_DistanceManhattan;

	open.pathNodeList.clear();
	close.pathNodeList.clear();
	last_path.clear();

	last_tile = { -1,-1 };
	goal = { -1,-1 };

	trigger = nullptr;
	isPathRequested = false;

	return true;
}

// Utility: return true if pos is inside the map boundaries
bool j1PathFinding::CheckBoundaries(const iPoint& pos) const
{
	return (pos.x >= 0 && pos.x <= (int)(App->scene->w - 1) &&
		pos.y >= 0 && pos.y <= (int)(App->scene->h - 1));
}

// Utility: returns true if the tile is walkable
bool j1PathFinding::IsWalkable(const iPoint& pos) const
{
	int t = GetTileAt(pos);
	return INVALID_WALK_CODE && t > 0;
}

// Utility: return the walkability value of a tile
int j1PathFinding::GetTileAt(const iPoint& pos) const
{
	if (CheckBoundaries(pos))
		// Scene map
		return App->scene->data[(pos.y*App->scene->w) + pos.x];

	return INVALID_WALK_CODE;
}

bool j1PathFinding::IsOnBase(const iPoint& pos)
{
	bool ret = false; 

	SDL_Rect entityPos{ pos.x * 32,pos.y* 32, 32,32 };
	SDL_Rect result{ 0,0,0,0 };

	if (SDL_IntersectRect(&entityPos, &App->map->playerBase, &result))
		ret = true;
	

	return ret;
}

// To request all tiles involved in the last generated path
const vector<iPoint>* j1PathFinding::GetLastPath() const
{
	return &last_path;
}

// To request the last tile checked by the search algorithm
iPoint j1PathFinding::GetLastTile() const
{
	return last_tile;
}

// PathNode -------------------------------------------------------------------------
// Convenient constructors
// ----------------------------------------------------------------------------------
PathNode::PathNode() : g(-1), h(-1), pos(-1, -1), parent(NULL)
{}

PathNode::PathNode(float g, float h, const iPoint& pos, const PathNode* parent, const bool diagonal = false) : g(g), h(h), pos(pos), parent(parent), diagonal(diagonal)
{}

PathNode::PathNode(const PathNode& node) : g(node.g), h(node.h), pos(node.pos), parent(node.parent)
{}

// PathNode -------------------------------------------------------------------------
// Fills a list (PathList) of all valid adjacent pathnodes
// ----------------------------------------------------------------------------------
uint PathNode::FindWalkableAdjacents(PathList& list_to_fill, bool isWalkabilityChecked) const
{
	iPoint cell;
	uint before = list_to_fill.pathNodeList.size();

	cell.create(pos.x, pos.y + 1);
	if (isWalkabilityChecked) {
		if (App->pathfinding->IsWalkable(cell))
			list_to_fill.pathNodeList.push_back(PathNode(-1, -1, cell, this));
	}
	else
		list_to_fill.pathNodeList.push_back(PathNode(-1, -1, cell, this));

	// south
	cell.create(pos.x, pos.y - 1);
	if (isWalkabilityChecked) {
		if (App->pathfinding->IsWalkable(cell))
			list_to_fill.pathNodeList.push_back(PathNode(-1, -1, cell, this));
	}
	else
		list_to_fill.pathNodeList.push_back(PathNode(-1, -1, cell, this));

	// east
	cell.create(pos.x + 1, pos.y);
	if (isWalkabilityChecked) {
		if (App->pathfinding->IsWalkable(cell))
			list_to_fill.pathNodeList.push_back(PathNode(-1, -1, cell, this));
	}
	else
		list_to_fill.pathNodeList.push_back(PathNode(-1, -1, cell, this));

	// west
	cell.create(pos.x - 1, pos.y);
	if (isWalkabilityChecked) {
		if (App->pathfinding->IsWalkable(cell))
			list_to_fill.pathNodeList.push_back(PathNode(-1, -1, cell, this));
	}
	else
		list_to_fill.pathNodeList.push_back(PathNode(-1, -1, cell, this));

	// north-west
	cell.create(pos.x + 1, pos.y - 1);
	if (isWalkabilityChecked) {
		if (App->pathfinding->IsWalkable(cell))
			list_to_fill.pathNodeList.push_back(PathNode(-1, -1, cell, this, true));
	}
	else
		list_to_fill.pathNodeList.push_back(PathNode(-1, -1, cell, this, true));

	// south-west
	cell.create(pos.x - 1, pos.y - 1);
	if (isWalkabilityChecked) {
		if (App->pathfinding->IsWalkable(cell))
			list_to_fill.pathNodeList.push_back(PathNode(-1, -1, cell, this, true));
	}
	else
		list_to_fill.pathNodeList.push_back(PathNode(-1, -1, cell, this, true));

	// north-west
	cell.create(pos.x + 1, pos.y + 1);
	if (isWalkabilityChecked) {
		if (App->pathfinding->IsWalkable(cell))
			list_to_fill.pathNodeList.push_back(PathNode(-1, -1, cell, this, true));
	}
	else
		list_to_fill.pathNodeList.push_back(PathNode(-1, -1, cell, this, true));

	// south-est
	cell.create(pos.x - 1, pos.y + 1);
	if (isWalkabilityChecked) {
		if (App->pathfinding->IsWalkable(cell))
			list_to_fill.pathNodeList.push_back(PathNode(-1, -1, cell, this, true));
	}
	else
		list_to_fill.pathNodeList.push_back(PathNode(-1, -1, cell, this, true));

	return list_to_fill.pathNodeList.size();
}

// PathNode -------------------------------------------------------------------------
// Calculates this tile score
// ----------------------------------------------------------------------------------
float PathNode::Score() const
{
	return g + h;
}

// PathNode -------------------------------------------------------------------------
// Calculate the F for a specific destination tile
// ----------------------------------------------------------------------------------
float PathNode::CalculateF(bool h, const iPoint& destination, DistanceHeuristic distanceHeuristic)
{
	if (diagonal)
		g = parent->g + 1.7f;
	else
		g = parent->g + 1.0f;

	if (h)
		this->h = CalculateDistance(pos, destination, distanceHeuristic);

	return g + this->h;
}

// PathList ------------------------------------------------------------------------
// Looks for a node in this list and returns it's list node or NULL
// ---------------------------------------------------------------------------------
const PathNode* PathList::Find(const iPoint& point) const
{
	list<PathNode>::const_iterator item = pathNodeList.begin();

	while (item != pathNodeList.end())
	{
		if ((*item).pos == point)
			return &(*item);
		item++;
	}

	return NULL;
}

// PathList ------------------------------------------------------------------------
// Returns the Pathnode with lowest score in this list or NULL if empty
// ---------------------------------------------------------------------------------
const PathNode* PathList::GetNodeLowestScore() const
{
	const PathNode* ret = NULL;
	float min = INT_MAX;

	list<PathNode>::const_reverse_iterator item = pathNodeList.rbegin();

	while (item != pathNodeList.rend())
	{
		if ((*item).Score() < min)
		{
			min = (*item).Score();
			ret = &(*item);
		}
		item++;
	}

	return ret;
}

// ---------------------------------------------------------------------------------

int CalculateDistance(iPoint origin, iPoint destination, DistanceHeuristic distanceHeuristic)
{
	int distance = 0;

	switch (distanceHeuristic) {
	case DistanceHeuristic_DistanceTo:
		distance = origin.DistanceTo(destination);
		break;
	case DistanceHeuristic_DistanceNoSqrt:
		distance = origin.DistanceNoSqrt(destination);
		break;
	case DistanceHeuristic_DistanceManhattan:
		distance = origin.DistanceManhattan(destination);
		break;
	}

	return distance;
}

// ----------------------------------------------------------------------------------
// Actual A* algorithm: return number of steps in the creation of the path or -1 ----
// ----------------------------------------------------------------------------------
int j1PathFinding::CreatePath(const iPoint& origin, const iPoint& destination, DistanceHeuristic distanceHeuristic)
{
	BROFILER_CATEGORY(__FUNCTION__, Profiler::Color::Orchid);

	last_path.clear();
	int ret = 0;

	// If origin or destination are not walkable, return -1
	if (!IsWalkable(origin) || !IsWalkable(destination))
		ret = -1;
	else {

		// Create two lists: open, close

		// Add the origin tile to open
		PathNode originNode(0, CalculateDistance(origin, destination, distanceHeuristic), origin, nullptr);
		open.pathNodeList.push_back(originNode);

		// Iterate while we have tile in the open list
		while (open.pathNodeList.size() > 0) {

			// Move the lowest score cell from open list to the closed list
			PathNode* curr = (PathNode*)open.GetNodeLowestScore();
			close.pathNodeList.push_back(*curr);

			// Erase element from list -----
			list<PathNode>::iterator it = open.pathNodeList.begin();
			while (it != open.pathNodeList.end()) {

				if (&(*it) == &(*curr))
					break;
				it++;
			}
			open.pathNodeList.erase(it);
			// Erase element from list -----

			// If we just added the destination, we are done!
			// Backtrack to create the final path
			if (close.pathNodeList.back().pos == destination) {

				for (PathNode iterator = close.pathNodeList.back(); iterator.parent != nullptr;
					iterator = *close.Find(iterator.parent->pos)) {

					last_path.push_back(iterator.pos);
				}

				last_path.push_back(close.pathNodeList.front().pos);

				// Flip() the path when you are finish
				reverse(last_path.begin(), last_path.end());

				ret = last_path.size();

				return ret;

			}
			else {
				// Fill a list of all adjancent nodes
				PathList neighbors;
				close.pathNodeList.back().FindWalkableAdjacents(neighbors);

				// Iterate adjancent nodes:
				list<PathNode>::iterator iterator = neighbors.pathNodeList.begin();

				while (iterator != neighbors.pathNodeList.end()) {
					// ignore nodes in the closed list
					if (close.Find((*iterator).pos) != NULL) {
						iterator++;
						continue;
					}

					(*iterator).CalculateF(true, destination, distanceHeuristic);
					// If it is already in the open list, check if it is a better path (compare G)
					if (open.Find((*iterator).pos) != NULL) {

						// If it is a better path, Update the parent
						PathNode open_node = *open.Find((*iterator).pos);
						if ((*iterator).g < open_node.g)
							open_node.parent = (*iterator).parent;
					}
					else {
						// If it is NOT found, calculate its F and add it to the open list
						open.pathNodeList.push_back(*iterator);
					}
					iterator++;
				}
				neighbors.pathNodeList.clear();
			}
		}
	}

	return ret;
}

int j1PathFinding::BacktrackToCreatePath()
{
	last_path.clear();

	// Backtrack to create the final path
	if (close.pathNodeList.size() > 1) {

		for (PathNode iterator = close.pathNodeList.back(); iterator.parent != nullptr;
			iterator = *close.Find(iterator.parent->pos)) {

			last_path.push_back(iterator.pos);
		}
	}

	last_path.push_back(close.pathNodeList.front().pos);

	// Flip the path
	reverse(last_path.begin(), last_path.end());

	return last_path.size();
}

bool j1PathFinding::InitializeAStar(const iPoint& origin, const iPoint& destination, DistanceHeuristic distanceHeuristic, bool isWalkabilityChecked)
{
	this->isWalkabilityChecked = isWalkabilityChecked;

	// If origin or destination are not walkable, return false
	if (this->isWalkabilityChecked) {

		if (!IsWalkable(origin) || !IsWalkable(destination))
			return false;
	}

	goal = destination;
	this->distanceHeuristic = distanceHeuristic;

	// Add the origin tile to open
	PathNode originNode(0, CalculateDistance(origin, destination, distanceHeuristic), origin, nullptr);
	open.pathNodeList.push_back(originNode);

	return true;
}

PathfindingStatus j1PathFinding::CycleOnceAStar()
{
	// If the open list is empty, the path has not been found
	if (open.pathNodeList.size() == 0)
		return PathfindingStatus_PathNotFound;
	else if (close.pathNodeList.size() == DEFAULT_PATH_LENGTH)
		return PathfindingStatus_PathNotFound;

	// Move the lowest score cell from open list to the closed list
	PathNode* curr = (PathNode*)open.GetNodeLowestScore();

	/// Push_back the lowest score cell to the closed list
	close.pathNodeList.push_back(*curr);

	// If the current node is the goal, the path has been found
	if (curr->pos == goal) {

		BacktrackToCreatePath();

		return PathfindingStatus_PathFound;
	}

	/// Erase the lowest score cell from the open list
	list<PathNode>::iterator it = open.pathNodeList.begin();
	while (it != open.pathNodeList.end()) {

		if (&(*it) == &(*curr))
			break;
		it++;
	}
	open.pathNodeList.erase(it);

	// Fill a list of all adjancent nodes
	PathList neighbors;
	close.pathNodeList.back().FindWalkableAdjacents(neighbors, isWalkabilityChecked);

	list<PathNode>::iterator iterator = neighbors.pathNodeList.begin();

	while (iterator != neighbors.pathNodeList.end()) {

		// Ignore nodes in the closed list
		if (close.Find((*iterator).pos) != NULL) {
			iterator++;
			continue;
		}

		(*iterator).CalculateF(true, goal, distanceHeuristic);

		// If it is already in the open list, check if it is a better path (compare G)
		if (open.Find((*iterator).pos) != NULL) {

			// If it is a better path, Update the parent
			PathNode open_node = *open.Find((*iterator).pos);
			if ((*iterator).g < open_node.g)
				open_node.parent = (*iterator).parent;
		}
		else {

			// If it is NOT found, calculate its F and add it to the open list
			open.pathNodeList.push_back(*iterator);
		}

		iterator++;
	}
	neighbors.pathNodeList.clear();

	// There are still nodes to explore
	return PathfindingStatus_SearchIncomplete;
}

// Initialize CycleOnceDijkstra
bool j1PathFinding::InitializeDijkstra(const iPoint& origin, FindActiveTrigger* trigger, bool isPathRequested, bool isWalkabilityChecked)
{
	this->isWalkabilityChecked = isWalkabilityChecked;

	// If origin is not walkable, return false
	if (this->isWalkabilityChecked) {
		if (!IsWalkable(origin))
			return false;
	}

	this->trigger = trigger;
	this->isPathRequested = isPathRequested;

	// Add the origin tile to the priorityQueue
	PathNode originNode(0, 0, origin, nullptr);
	open.pathNodeList.push_back(originNode);

	return true;
}

// CycleOnce Dijkstra
PathfindingStatus j1PathFinding::CycleOnceDijkstra()
{
	// If the open list is empty, the path has not been found
	if (open.pathNodeList.size() == 0)
		return PathfindingStatus_PathNotFound;

	// Move the lowest score cell from open list to the closed list
	PathNode* curr = (PathNode*)open.GetNodeLowestScore();

	/// Push_back the lowest score cell to the closed list
	close.pathNodeList.push_back(*curr);

	// If the current node is the goal, the path has been found
	if (trigger != nullptr) {

		if (trigger->isSatisfied(curr->pos)) {

			last_tile = curr->pos;

			if (isPathRequested)
				BacktrackToCreatePath();

			return PathfindingStatus_PathFound;
		}
	}

	/// Erase the lowest score cell from the open list
	list<PathNode>::iterator it = open.pathNodeList.begin();
	while (it != open.pathNodeList.end()) {

		if (&(*it) == &(*curr))
			break;
		it++;
	}
	open.pathNodeList.erase(it);

	// Fill a list of all adjancent nodes
	PathList neighbors;
	close.pathNodeList.back().FindWalkableAdjacents(neighbors, isWalkabilityChecked);

	list<PathNode>::iterator iterator = neighbors.pathNodeList.begin();

	while (iterator != neighbors.pathNodeList.end()) {

		// Ignore nodes in the closed list
		if (close.Find((*iterator).pos) != NULL) {
			iterator++;
			continue;
		}

		(*iterator).CalculateF();

		// If it is already in the open list, check if it is a better path (compare G)
		if (open.Find((*iterator).pos) != NULL) {

			// If it is a better path, Update the parent
			PathNode open_node = *open.Find((*iterator).pos);
			if ((*iterator).g < open_node.g)
				open_node.parent = (*iterator).parent;
		}
		else {

			// If it is NOT found, calculate its F and add it to the open list
			open.pathNodeList.push_back(*iterator);
		}

		iterator++;
	}
	neighbors.pathNodeList.clear();

	// There are still nodes to explore
	return PathfindingStatus_SearchIncomplete;
}