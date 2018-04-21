#include "Defs.h"
#include "p2Log.h"

#include "j1App.h"
#include "j1PathFinding.h"
#include "j1PathManager.h"
#include "j1Map.h"
#include "j1Movement.h"
#include "j1Scene.h"

#include "Entity.h"

#include "Brofiler\Brofiler.h"

j1PathManager::j1PathManager(double msSearchPerUpdate) : j1Module(), msSearchPerUpdate(msSearchPerUpdate)
{
	name.assign("pathmanager");
}

// Destructor
j1PathManager::~j1PathManager()
{
}

// Called before quitting
bool j1PathManager::CleanUp()
{
	bool ret = true;

	list<PathPlanner*>::const_iterator it = searchRequests.begin();

	while (it != searchRequests.end()) {

		delete *it;
		it++;
	}
	searchRequests.clear();

	return ret;
}

bool j1PathManager::Update(float dt)
{
	bool ret = true;

	if (searchRequests.size() > 0)
		UpdateSearches();

	return ret;
}

void j1PathManager::UpdateSearches()
{
	BROFILER_CATEGORY(__FUNCTION__, Profiler::Color::Orchid);

	timer.Start();

	list<PathPlanner*>::const_iterator currSearch = searchRequests.begin();

	while (timer.ReadMs() < msSearchPerUpdate && searchRequests.size() > 0) {

		// Make one search cycle of this path request
		PathfindingStatus result = (*currSearch)->CycleOnce();

		// If the search has terminated, remove it from the list
		if (result == PathfindingStatus_PathFound || result == PathfindingStatus_PathNotFound) {
			currSearch = searchRequests.erase(currSearch);
		}
		else {
			// Move on to the next search
			currSearch++;
		}

		// The iterator may now be pointing to the end of the list.
		// If so, it must be reset to the beginning
		if (currSearch == searchRequests.end())
			currSearch = searchRequests.begin();
	}
}

void j1PathManager::Register(PathPlanner* pathPlanner)
{
	list<PathPlanner*>::const_iterator it = find(searchRequests.begin(), searchRequests.end(), pathPlanner);

	if (it == searchRequests.end())
		searchRequests.push_back(pathPlanner);
}

void j1PathManager::UnRegister(PathPlanner* pathPlanner)
{
	pathPlanner->SetSearchRequested(false);

	searchRequests.remove(pathPlanner);
}

// ---------------------------------------------------------------------
// PATH PLANNER
// ---------------------------------------------------------------------

PathPlanner::PathPlanner(Entity* owner) :entity(owner) {}

PathPlanner::~PathPlanner()
{
	entity = nullptr;

	// Remove Current Search
	App->pathmanager->UnRegister(this);

	if (currentSearch != nullptr)
		delete currentSearch;
	currentSearch = nullptr;

	// Remove Trigger
	if (trigger != nullptr)
		delete trigger;
	trigger = nullptr;
}

bool PathPlanner::RequestAStar(iPoint origin, iPoint destination)
{
	bool ret = false;

	if (isSearchRequested)
		return false;

	App->pathmanager->UnRegister(this);
	GetReadyForNewSearch();

	pathfindingAlgorithmType = PathfindingAlgorithmType_AStar;

	// Current search
	if (currentSearch == nullptr)
		currentSearch = new j1PathFinding();

	// Invalidate if origin or destination are non-walkable
	ret = currentSearch->InitializeAStar(origin, destination);

	if (ret)
		App->pathmanager->Register(this);

	return ret;
}

bool PathPlanner::RequestDijkstra(iPoint origin, FindActiveTrigger::ActiveTriggerType activeTriggerType, bool isPathRequested)
{
	bool ret = true;

	if (isSearchRequested)
		return false;

	App->pathmanager->UnRegister(this);
	GetReadyForNewSearch();

	pathfindingAlgorithmType = PathfindingAlgorithmType_Dijkstra;

	// Current search
	if (currentSearch == nullptr)
		currentSearch = new j1PathFinding();

	// Trigger
	switch (activeTriggerType) {

	case FindActiveTrigger::ActiveTriggerType_Goal:

		if (trigger == nullptr)
			trigger = new FindActiveTrigger(activeTriggerType, entity);
		else {
			trigger->SetActiveTriggerType(activeTriggerType);
			trigger->SetEntity(entity);
		}

		break;

	case FindActiveTrigger::ActiveTriggerType_Object:
		
		if (trigger == nullptr)
			trigger = new FindActiveTrigger(activeTriggerType, entity->entityType);
		else {
			trigger->SetActiveTriggerType(activeTriggerType);
			trigger->SetEntityType(entity->entityType);
		}

		break;

	case FindActiveTrigger::ActiveTriggerType_NoType:
	default:

		trigger = nullptr;

		break;
	}

	// Invalidate if origin is non-walkable
	this->isPathRequested = isPathRequested;

	ret = currentSearch->InitializeDijkstra(origin, trigger, isPathRequested);

	if (ret)
		App->pathmanager->Register(this);

	return ret;
}

void PathPlanner::GetReadyForNewSearch()
{
	pathfindingAlgorithmType = PathfindingAlgorithmType_NoType;
	isSearchCompleted = false;
	isSearchRequested = true;

	// Delete any active search
	if (currentSearch != nullptr)
		currentSearch->CleanUp();

	if (trigger != nullptr)
		trigger->CleanUp();
}

PathfindingStatus PathPlanner::CycleOnce()
{
	PathfindingStatus result;

	switch (pathfindingAlgorithmType) {

	case PathfindingAlgorithmType_AStar:

		result = currentSearch->CycleOnceAStar();

		// Let the bot know of the failure to find a path
		if (result == PathfindingStatus_PathNotFound) {
			// ERROR!
		}
		// Let the bot know a path has been found
		else if (result == PathfindingStatus_PathFound)

			isSearchCompleted = true;

		break;

	case PathfindingAlgorithmType_Dijkstra:

		result = currentSearch->CycleOnceDijkstra();

		// Let the bot know of the failure to find a path/tile
		if (result == PathfindingStatus_PathNotFound) {
			// ERROR!
		}
		// Let the bot know a path/tile has been found
		else if (result == PathfindingStatus_PathFound)

			isSearchCompleted = true;

		break;
	}

	return result;
}

vector<iPoint> PathPlanner::GetPath() const
{
	if (isSearchCompleted)

		if (pathfindingAlgorithmType == PathfindingAlgorithmType_AStar || (pathfindingAlgorithmType == PathfindingAlgorithmType_Dijkstra && isPathRequested))

			return *currentSearch->GetLastPath();
}

iPoint PathPlanner::GetTile() const
{
	if (isSearchCompleted)

		return currentSearch->GetLastTile();
}

bool PathPlanner::IsSearchCompleted() const
{
	return isSearchCompleted;
}

bool PathPlanner::IsSearchRequested() const
{
	return isSearchRequested;
}

void PathPlanner::SetSearchRequested(bool isSearchRequested)
{
	this->isSearchRequested = isSearchRequested;
}

void PathPlanner::SetCheckingCurrTile(bool isCheckingCurrTile)
{
	if (trigger != nullptr)
		trigger->isCheckingCurrTile = isCheckingCurrTile;
}

void PathPlanner::SetCheckingNextTile(bool isCheckingNextTile)
{
	if (trigger != nullptr)
		trigger->isCheckingNextTile = isCheckingNextTile;
}

void PathPlanner::SetCheckingGoalTile(bool isCheckingGoalTile)
{
	if (trigger != nullptr)
		trigger->isCheckingGoalTile = isCheckingGoalTile;
}

j1PathFinding* PathPlanner::GetCurrentSearch() const
{
	return currentSearch;
}

// FindActiveTrigger class ---------------------------------------------------------------------------------

FindActiveTrigger::FindActiveTrigger(ActiveTriggerType activeTriggerType, Entity* entity) :activeTriggerType(activeTriggerType), entity(entity) {}

FindActiveTrigger::FindActiveTrigger(ActiveTriggerType activeTriggerType, ENTITY_CATEGORY entityType) : activeTriggerType(activeTriggerType), entityType(entityType) {}

bool FindActiveTrigger::CleanUp()
{
	activeTriggerType = ActiveTriggerType_NoType;
	entity = nullptr;

	entityType = EntityCategory_NONE;

	isCheckingCurrTile = false;
	isCheckingNextTile = false;
	isCheckingGoalTile = true;

	return true;
}

void FindActiveTrigger::SetActiveTriggerType(ActiveTriggerType activeTriggerType) 
{
	this->activeTriggerType = activeTriggerType;
}

void FindActiveTrigger::SetEntity(Entity* entity) 
{
	this->entity = entity;
}

void FindActiveTrigger::SetEntityType(ENTITY_CATEGORY entityType) 
{
	this->entityType = entityType;
}

bool FindActiveTrigger::isSatisfied(iPoint tile) const
{
	bool isSatisfied = false;

	DynamicEntity* dynamicEntity = (DynamicEntity*)entity;

	switch (activeTriggerType) {

	case ActiveTriggerType_Goal:

		if (App->movement->IsValidTile(dynamicEntity->GetSingleUnit(), tile, isCheckingCurrTile, isCheckingNextTile, isCheckingGoalTile))
			isSatisfied = true;

		break;

	case ActiveTriggerType_Object:

		isSatisfied = true;

		break;

	case ActiveTriggerType_NoType:
	default:

		break;
	}

	return isSatisfied;
}