#include "Defs.h"
#include "p2Log.h"

#include "j1Module.h"
#include "j1App.h"

#include "j1EntityFactory.h"
#include "j1Render.h"
#include "j1Textures.h"
#include "j1Scene.h"
#include "j1Map.h"
#include "j1Pathfinding.h"
#include "j1Collision.h"
#include "j1Input.h"
#include "j1Player.h"

#include "Entity.h"
#include "DynamicEntity.h"
#include "StaticEntity.h"

j1EntityFactory::j1EntityFactory()
{
	name.assign("entities");
}

// Destructor
j1EntityFactory::~j1EntityFactory()
{
}

bool j1EntityFactory::Awake(pugi::xml_node& config) {

	bool ret = true;

	// Spritesheets
	pugi::xml_node spritesheets = config.child("spritesheets");
	humanBuildingsTexName = spritesheets.child("humanBuildings").attribute("name").as_string();
	neutralBuildingsTexName = spritesheets.child("neutralBuildings").attribute("name").as_string();
	orcishBuildingsTexName = spritesheets.child("orcishBuildings").attribute("name").as_string();
	footmanTexName = spritesheets.child("footmanAnimations").attribute("name").as_string();
	gruntTexName = spritesheets.child("gruntAnimations").attribute("name").as_string();

	//Debug Textures Properties
	buildingPreviewTiles.opacity = config.child("previewTexturesProperties").attribute("tileBuildingPlaceOpacity").as_uint();
	previewBuildingOpacity = config.child("previewTexturesProperties").attribute("buildingPlaceOpacity").as_uint();

	// Static entities
	pugi::xml_node staticEntities = config.child("staticEntities");

	//Human buildings
	pugi::xml_node humanBuildings = staticEntities.child("humanBuildings");

	townHallInfo.townHallMaxLife = { humanBuildings.child("townHall").child("maxLife").attribute("value").as_int() };
	pugi::xml_node aux = humanBuildings.child("townHall").child("sprites");
	townHallInfo.townHallCompleteTexArea = { aux.child("complete").attribute("x").as_int(), aux.child("complete").attribute("y").as_int(), aux.child("complete").attribute("w").as_int(), aux.child("complete").attribute("h").as_int() };
	townHallInfo.townHallInProgressTexArea = { aux.child("inProgress").attribute("x").as_int(), aux.child("inProgress").attribute("y").as_int(), aux.child("inProgress").attribute("w").as_int(), aux.child("inProgress").attribute("h").as_int() };

	townHallInfo.keepMaxLife = { humanBuildings.child("keep").child("maxLife").attribute("value").as_int() };
	aux = humanBuildings.child("keep").child("sprites");
	townHallInfo.keepCompleteTexArea = { aux.child("complete").attribute("x").as_int(), aux.child("complete").attribute("y").as_int(), aux.child("complete").attribute("w").as_int(), aux.child("complete").attribute("h").as_int() };
	townHallInfo.keepInProgressTexArea = { aux.child("inProgress").attribute("x").as_int(), aux.child("inProgress").attribute("y").as_int(), aux.child("inProgress").attribute("w").as_int(), aux.child("inProgress").attribute("h").as_int() };

	townHallInfo.castleMaxLife = { humanBuildings.child("castle").child("maxLife").attribute("value").as_int() };
	aux = humanBuildings.child("castle").child("sprites");
	townHallInfo.castleCompleteTexArea = { aux.child("complete").attribute("x").as_int(), aux.child("complete").attribute("y").as_int(), aux.child("complete").attribute("w").as_int(), aux.child("complete").attribute("h").as_int() };
	townHallInfo.castleInProgressTexArea = { aux.child("inProgress").attribute("x").as_int(), aux.child("inProgress").attribute("y").as_int(), aux.child("inProgress").attribute("w").as_int(), aux.child("inProgress").attribute("h").as_int() };

	chickenFarmInfo.maxLife = { humanBuildings.child("chickenFarm").child("maxLife").attribute("value").as_int() };
	aux = humanBuildings.child("chickenFarm").child("sprites");
	chickenFarmInfo.completeTexArea = { aux.child("complete").attribute("x").as_int(), aux.child("complete").attribute("y").as_int(), aux.child("complete").attribute("w").as_int(), aux.child("complete").attribute("h").as_int() };
	chickenFarmInfo.inProgressTexArea = { aux.child("inProgress").attribute("x").as_int(), aux.child("inProgress").attribute("y").as_int(), aux.child("inProgress").attribute("w").as_int(), aux.child("inProgress").attribute("h").as_int() };

	scoutTowerInfo.maxLife = { humanBuildings.child("scoutTower").child("maxLife").attribute("value").as_int() };
	aux = humanBuildings.child("scoutTower").child("sprites");
	scoutTowerInfo.completeTexArea = { aux.child("complete").attribute("x").as_int(), aux.child("complete").attribute("y").as_int(), aux.child("complete").attribute("w").as_int(), aux.child("complete").attribute("h").as_int() };
	scoutTowerInfo.inProgressTexArea = { aux.child("inProgress").attribute("x").as_int(), aux.child("inProgress").attribute("y").as_int(), aux.child("inProgress").attribute("w").as_int(), aux.child("inProgress").attribute("h").as_int() };

	playerGuardTowerInfo.maxLife = { humanBuildings.child("guardTower").child("maxLife").attribute("value").as_int() };
	aux = humanBuildings.child("guardTower").child("sprites");
	playerGuardTowerInfo.completeTexArea = { aux.child("complete").attribute("x").as_int(), aux.child("complete").attribute("y").as_int(), aux.child("complete").attribute("w").as_int(), aux.child("complete").attribute("h").as_int() };
	playerGuardTowerInfo.inProgressTexArea = { aux.child("inProgress").attribute("x").as_int(), aux.child("inProgress").attribute("y").as_int(), aux.child("inProgress").attribute("w").as_int(), aux.child("inProgress").attribute("h").as_int() };

	playerCannonTowerInfo.maxLife = { humanBuildings.child("canonTower").child("maxLife").attribute("value").as_int() };
	aux = humanBuildings.child("canonTower").child("sprites");
	playerCannonTowerInfo.completeTexArea = { aux.child("complete").attribute("x").as_int(), aux.child("complete").attribute("y").as_int(), aux.child("complete").attribute("w").as_int(), aux.child("complete").attribute("h").as_int() };
	playerCannonTowerInfo.inProgressTexArea = { aux.child("inProgress").attribute("x").as_int(), aux.child("inProgress").attribute("y").as_int(), aux.child("inProgress").attribute("w").as_int(), aux.child("inProgress").attribute("h").as_int() };

	barracksInfo.barracks1MaxLife = { humanBuildings.child("barracks").child("maxLife").attribute("value").as_int() };
	aux = humanBuildings.child("barracks").child("sprites");
	barracksInfo.barracksCompleteTexArea = { aux.child("complete").attribute("x").as_int(), aux.child("complete").attribute("y").as_int(), aux.child("complete").attribute("w").as_int(), aux.child("complete").attribute("h").as_int() };

	barracksInfo.barracks2MaxLife = { humanBuildings.child("barracks2").child("maxLife").attribute("value").as_int() };
	aux = humanBuildings.child("barracks2").child("sprites");
	barracksInfo.barracks2CompleteTexArea = { aux.child("complete").attribute("x").as_int(), aux.child("complete").attribute("y").as_int(), aux.child("complete").attribute("w").as_int(), aux.child("complete").attribute("h").as_int() };

	elvenLumberMillInfo.maxLife= { humanBuildings.child("elvenLumberMill").child("maxLife").attribute("value").as_int() };
	aux = humanBuildings.child("elvenLumberMill").child("sprites");
	elvenLumberMillInfo.completeTexArea = { aux.child("complete").attribute("x").as_int(), aux.child("complete").attribute("y").as_int(), aux.child("complete").attribute("w").as_int(), aux.child("complete").attribute("h").as_int() };
	elvenLumberMillInfo.inProgressTexArea = { aux.child("inProgress").attribute("x").as_int(), aux.child("inProgress").attribute("y").as_int(), aux.child("inProgress").attribute("w").as_int(), aux.child("inProgress").attribute("h").as_int() };

	mageTowerInfo.maxLife = { humanBuildings.child("mageTower").child("maxLife").attribute("value").as_int() };
	aux = humanBuildings.child("mageTower").child("sprites");
	mageTowerInfo.completeTexArea = { aux.child("complete").attribute("x").as_int(), aux.child("complete").attribute("y").as_int(), aux.child("complete").attribute("w").as_int(), aux.child("complete").attribute("h").as_int() };
	mageTowerInfo.inProgressTexArea = { aux.child("inProgress").attribute("x").as_int(), aux.child("inProgress").attribute("y").as_int(), aux.child("inProgress").attribute("w").as_int(), aux.child("inProgress").attribute("h").as_int() };

	gryphonAviaryInfo.maxLife = { humanBuildings.child("gryphonAviary").child("maxLife").attribute("value").as_int() };
	aux = humanBuildings.child("gryphonAviary").child("sprites");
	gryphonAviaryInfo.completeTexArea = { aux.child("complete").attribute("x").as_int(), aux.child("complete").attribute("y").as_int(), aux.child("complete").attribute("w").as_int(), aux.child("complete").attribute("h").as_int() };
	gryphonAviaryInfo.inProgressTexArea = { aux.child("inProgress").attribute("x").as_int(), aux.child("inProgress").attribute("y").as_int(), aux.child("inProgress").attribute("w").as_int(), aux.child("inProgress").attribute("h").as_int() };

	stablesInfo.maxLife = { humanBuildings.child("stables").child("maxLife").attribute("value").as_int() };
	aux = humanBuildings.child("stables").child("sprites");
	stablesInfo.completeTexArea = { aux.child("complete").attribute("x").as_int(), aux.child("complete").attribute("y").as_int(), aux.child("complete").attribute("w").as_int(), aux.child("complete").attribute("h").as_int() };
	stablesInfo.inProgressTexArea = { aux.child("inProgress").attribute("x").as_int(), aux.child("inProgress").attribute("y").as_int(), aux.child("inProgress").attribute("w").as_int(), aux.child("inProgress").attribute("h").as_int() };

	//Construction planks for the human buildings

	pugi::xml_node smallConstructionPlanks = humanBuildings.child("constructionPlanks").child("smallTilePlanks");
	aux = smallConstructionPlanks.child("first");
	chickenFarmInfo.constructionPlanks1 = scoutTowerInfo.constructionPlanks1 = playerGuardTowerInfo.constructionPlanks1 = playerCannonTowerInfo.constructionPlanks1 = { aux.attribute("x").as_int(), aux.attribute("y").as_int(), aux.attribute("w").as_int(), aux.attribute("h").as_int() };
	aux = smallConstructionPlanks.child("second");
	chickenFarmInfo.constructionPlanks2 = scoutTowerInfo.constructionPlanks2 = playerGuardTowerInfo.constructionPlanks2 = playerCannonTowerInfo.constructionPlanks2 = { aux.attribute("x").as_int(), aux.attribute("y").as_int(), aux.attribute("w").as_int(), aux.attribute("h").as_int() };

	pugi::xml_node mediumConstructionPlanks = humanBuildings.child("constructionPlanks").child("mediumTilePlanks");
	aux = mediumConstructionPlanks.child("first");
	barracksInfo.constructionPlanks1 = elvenLumberMillInfo.constructionPlanks1 = { aux.attribute("x").as_int(), aux.attribute("y").as_int(), aux.attribute("w").as_int(), aux.attribute("h").as_int() };
	mageTowerInfo.constructionPlanks1 = gryphonAviaryInfo.constructionPlanks1 = stablesInfo.constructionPlanks1 = { aux.attribute("x").as_int(), aux.attribute("y").as_int(), aux.attribute("w").as_int(), aux.attribute("h").as_int() };
	aux = mediumConstructionPlanks.child("second");
	barracksInfo.constructionPlanks2 = elvenLumberMillInfo.constructionPlanks2 = { aux.attribute("x").as_int(), aux.attribute("y").as_int(), aux.attribute("w").as_int(), aux.attribute("h").as_int() };
	mageTowerInfo.constructionPlanks2 = gryphonAviaryInfo.constructionPlanks2 = stablesInfo.constructionPlanks2 = { aux.attribute("x").as_int(), aux.attribute("y").as_int(), aux.attribute("w").as_int(), aux.attribute("h").as_int() };


	//Building preview tiles
	pugi::xml_node previewTiles = humanBuildings.child("previewTiles");
	buildingPreviewTiles.greenTile = { previewTiles.child("green").attribute("x").as_int(), previewTiles.child("green").attribute("y").as_int(), previewTiles.child("green").attribute("w").as_int(), previewTiles.child("green").attribute("h").as_int() };
	buildingPreviewTiles.redTile = { previewTiles.child("red").attribute("x").as_int(), previewTiles.child("red").attribute("y").as_int(), previewTiles.child("red").attribute("w").as_int(), previewTiles.child("red").attribute("h").as_int() };

	//Neutral buildings
	pugi::xml_node neutralBuildings = staticEntities.child("neutralBuildings");

	goldMineInfo.maxLife = { neutralBuildings.child("goldMine").child("maxLife").attribute("value").as_int() };
	aux = neutralBuildings.child("goldMine").child("sprites");
	goldMineInfo.completeTexArea = { aux.child("active").attribute("x").as_int(), aux.child("active").attribute("y").as_int(), aux.child("active").attribute("w").as_int(), aux.child("active").attribute("h").as_int() };
	goldMineInfo.inProgressTexArea = { aux.child("inactive").attribute("x").as_int(), aux.child("inactive").attribute("y").as_int(), aux.child("inactive").attribute("w").as_int(), aux.child("inactive").attribute("h").as_int() };

	aux = neutralBuildings.child("runeStone").child("sprites");
	runestoneInfo.completeTexArea = { aux.attribute("x").as_int(), aux.attribute("y").as_int(), aux.attribute("w").as_int(), aux.attribute("h").as_int() };

	//Dynamic entities
	//Humans
	pugi::xml_node humanEntities = config.child("dynamicEntities").child("humans");

	//Footman animations
	pugi::xml_node footmanAnimations = humanEntities.child("footman").child("animations");
	pugi::xml_node currentAnimation;

	//Up animation Footman
	currentAnimation = footmanAnimations.child("up");
	footmanInfo.up.speed = currentAnimation.attribute("speed").as_float();
	footmanInfo.up.loop = currentAnimation.attribute("loop").as_bool();
	for (currentAnimation = currentAnimation.child("frame"); currentAnimation; currentAnimation = currentAnimation.next_sibling("frame")) {
		footmanInfo.up.PushBack({ currentAnimation.attribute("x").as_int(), currentAnimation.attribute("y").as_int(), currentAnimation.attribute("w").as_int(), currentAnimation.attribute("h").as_int() });
	}
	//Down animation Footman
	currentAnimation = footmanAnimations.child("down");
	footmanInfo.down.speed = currentAnimation.attribute("speed").as_float();
	footmanInfo.down.loop = currentAnimation.attribute("loop").as_bool();
	for (currentAnimation = currentAnimation.child("frame"); currentAnimation; currentAnimation = currentAnimation.next_sibling("frame")) {
		footmanInfo.down.PushBack({ currentAnimation.attribute("x").as_int(), currentAnimation.attribute("y").as_int(), currentAnimation.attribute("w").as_int(), currentAnimation.attribute("h").as_int() });
	}
	//Right animation Footman
	currentAnimation = footmanAnimations.child("right");
	footmanInfo.right.speed = currentAnimation.attribute("speed").as_float();
	footmanInfo.right.loop = currentAnimation.attribute("loop").as_bool();
	for (currentAnimation = currentAnimation.child("frame"); currentAnimation; currentAnimation = currentAnimation.next_sibling("frame")) {
		footmanInfo.right.PushBack({ currentAnimation.attribute("x").as_int(), currentAnimation.attribute("y").as_int(), currentAnimation.attribute("w").as_int(), currentAnimation.attribute("h").as_int() });
	}
	//Left animation Footman
	currentAnimation = footmanAnimations.child("left");
	footmanInfo.left.speed = currentAnimation.attribute("speed").as_float();
	footmanInfo.left.loop = currentAnimation.attribute("loop").as_bool();
	for (currentAnimation = currentAnimation.child("frame"); currentAnimation; currentAnimation = currentAnimation.next_sibling("frame")) {
		footmanInfo.left.PushBack({ currentAnimation.attribute("x").as_int(), currentAnimation.attribute("y").as_int(), currentAnimation.attribute("w").as_int(), currentAnimation.attribute("h").as_int() });
	}
	//Up Right animation Footman
	currentAnimation = footmanAnimations.child("upRight");
	footmanInfo.upRight.speed = currentAnimation.attribute("speed").as_float();
	footmanInfo.upRight.loop = currentAnimation.attribute("loop").as_bool();
	for (currentAnimation = currentAnimation.child("frame"); currentAnimation; currentAnimation = currentAnimation.next_sibling("frame")) {
		footmanInfo.upRight.PushBack({ currentAnimation.attribute("x").as_int(), currentAnimation.attribute("y").as_int(), currentAnimation.attribute("w").as_int(), currentAnimation.attribute("h").as_int() });
	}
	//Up Left animation Footman
	currentAnimation = footmanAnimations.child("upLeft");
	footmanInfo.upLeft.speed = currentAnimation.attribute("speed").as_float();
	footmanInfo.upLeft.loop = currentAnimation.attribute("loop").as_bool();
	for (currentAnimation = currentAnimation.child("frame"); currentAnimation; currentAnimation = currentAnimation.next_sibling("frame")) {
		footmanInfo.upLeft.PushBack({ currentAnimation.attribute("x").as_int(), currentAnimation.attribute("y").as_int(), currentAnimation.attribute("w").as_int(), currentAnimation.attribute("h").as_int() });
	}
	//Down Right animation Footman
	currentAnimation = footmanAnimations.child("downRight");
	footmanInfo.downRight.speed = currentAnimation.attribute("speed").as_float();
	footmanInfo.downRight.loop = currentAnimation.attribute("loop").as_bool();
	for (currentAnimation = currentAnimation.child("frame"); currentAnimation; currentAnimation = currentAnimation.next_sibling("frame")) {
		footmanInfo.downRight.PushBack({ currentAnimation.attribute("x").as_int(), currentAnimation.attribute("y").as_int(), currentAnimation.attribute("w").as_int(), currentAnimation.attribute("h").as_int() });
	}
	//Down Left animation Footman
	currentAnimation = footmanAnimations.child("downLeft");
	footmanInfo.downLeft.speed = currentAnimation.attribute("speed").as_float();
	footmanInfo.downLeft.loop = currentAnimation.attribute("loop").as_bool();
	for (currentAnimation = currentAnimation.child("frame"); currentAnimation; currentAnimation = currentAnimation.next_sibling("frame")) {
		footmanInfo.downLeft.PushBack({ currentAnimation.attribute("x").as_int(), currentAnimation.attribute("y").as_int(), currentAnimation.attribute("w").as_int(), currentAnimation.attribute("h").as_int() });
	}
	//Attack Up animation Footman
	currentAnimation = footmanAnimations.child("attackUp");
	footmanInfo.attackUp.speed = currentAnimation.attribute("speed").as_float();
	footmanInfo.attackUp.loop = currentAnimation.attribute("loop").as_bool();
	for (currentAnimation = currentAnimation.child("frame"); currentAnimation; currentAnimation = currentAnimation.next_sibling("frame")) {
		footmanInfo.attackUp.PushBack({ currentAnimation.attribute("x").as_int(), currentAnimation.attribute("y").as_int(), currentAnimation.attribute("w").as_int(), currentAnimation.attribute("h").as_int() });
	}
	//Attack Down animation Footman
	currentAnimation = footmanAnimations.child("attackDown");
	footmanInfo.attackDown.speed = currentAnimation.attribute("speed").as_float();
	footmanInfo.attackDown.loop = currentAnimation.attribute("loop").as_bool();
	for (currentAnimation = currentAnimation.child("frame"); currentAnimation; currentAnimation = currentAnimation.next_sibling("frame")) {
		footmanInfo.attackDown.PushBack({ currentAnimation.attribute("x").as_int(), currentAnimation.attribute("y").as_int(), currentAnimation.attribute("w").as_int(), currentAnimation.attribute("h").as_int() });
	}
	//Attack Left animation Footman
	currentAnimation = footmanAnimations.child("attackLeft");
	footmanInfo.attackLeft.speed = currentAnimation.attribute("speed").as_float();
	footmanInfo.attackLeft.loop = currentAnimation.attribute("loop").as_bool();
	for (currentAnimation = currentAnimation.child("frame"); currentAnimation; currentAnimation = currentAnimation.next_sibling("frame")) {
		footmanInfo.attackLeft.PushBack({ currentAnimation.attribute("x").as_int(), currentAnimation.attribute("y").as_int(), currentAnimation.attribute("w").as_int(), currentAnimation.attribute("h").as_int() });
	}
	//Attack Right animation Footman
	currentAnimation = footmanAnimations.child("attackRight");
	footmanInfo.attackRight.speed = currentAnimation.attribute("speed").as_float();
	footmanInfo.attackRight.loop = currentAnimation.attribute("loop").as_bool();
	for (currentAnimation = currentAnimation.child("frame"); currentAnimation; currentAnimation = currentAnimation.next_sibling("frame")) {
		footmanInfo.attackRight.PushBack({ currentAnimation.attribute("x").as_int(), currentAnimation.attribute("y").as_int(), currentAnimation.attribute("w").as_int(), currentAnimation.attribute("h").as_int() });
	}
	//Attack Up Left animation Footman
	currentAnimation = footmanAnimations.child("attackUpLeft");
	footmanInfo.attackUpLeft.speed = currentAnimation.attribute("speed").as_float();
	footmanInfo.attackUpLeft.loop = currentAnimation.attribute("loop").as_bool();
	for (currentAnimation = currentAnimation.child("frame"); currentAnimation; currentAnimation = currentAnimation.next_sibling("frame")) {
		footmanInfo.attackUpLeft.PushBack({ currentAnimation.attribute("x").as_int(), currentAnimation.attribute("y").as_int(), currentAnimation.attribute("w").as_int(), currentAnimation.attribute("h").as_int() });
	}
	//Attack Up Right animation Footman
	currentAnimation = footmanAnimations.child("attackUpRight");
	footmanInfo.attackUpRight.speed = currentAnimation.attribute("speed").as_float();
	footmanInfo.attackUpRight.loop = currentAnimation.attribute("loop").as_bool();
	for (currentAnimation = currentAnimation.child("frame"); currentAnimation; currentAnimation = currentAnimation.next_sibling("frame")) {
		footmanInfo.attackUpRight.PushBack({ currentAnimation.attribute("x").as_int(), currentAnimation.attribute("y").as_int(), currentAnimation.attribute("w").as_int(), currentAnimation.attribute("h").as_int() });
	}
	//Attack Down Left animation Footman
	currentAnimation = footmanAnimations.child("attackDownLeft");
	footmanInfo.attackDownLeft.speed = currentAnimation.attribute("speed").as_float();
	footmanInfo.attackDownLeft.loop = currentAnimation.attribute("loop").as_bool();
	for (currentAnimation = currentAnimation.child("frame"); currentAnimation; currentAnimation = currentAnimation.next_sibling("frame")) {
		footmanInfo.attackDownLeft.PushBack({ currentAnimation.attribute("x").as_int(), currentAnimation.attribute("y").as_int(), currentAnimation.attribute("w").as_int(), currentAnimation.attribute("h").as_int() });
	}
	//Attack Down Right animation Footman
	currentAnimation = footmanAnimations.child("attackDownRight");
	footmanInfo.attackDownRight.speed = currentAnimation.attribute("speed").as_float();
	footmanInfo.attackDownRight.loop = currentAnimation.attribute("loop").as_bool();
	for (currentAnimation = currentAnimation.child("frame"); currentAnimation; currentAnimation = currentAnimation.next_sibling("frame")) {
		footmanInfo.attackDownRight.PushBack({ currentAnimation.attribute("x").as_int(), currentAnimation.attribute("y").as_int(), currentAnimation.attribute("w").as_int(), currentAnimation.attribute("h").as_int() });
	}
	//Death Up animation Footman
	currentAnimation = footmanAnimations.child("deathUp");
	footmanInfo.deathUp.speed = currentAnimation.attribute("speed").as_float();
	footmanInfo.deathUp.loop = currentAnimation.attribute("loop").as_bool();
	for (currentAnimation = currentAnimation.child("frame"); currentAnimation; currentAnimation = currentAnimation.next_sibling("frame")) {
		footmanInfo.deathUp.PushBack({ currentAnimation.attribute("x").as_int(), currentAnimation.attribute("y").as_int(), currentAnimation.attribute("w").as_int(), currentAnimation.attribute("h").as_int() });
	}
	//Death Down animation Footman
	currentAnimation = footmanAnimations.child("deathDown");
	footmanInfo.deathDown.speed = currentAnimation.attribute("speed").as_float();
	footmanInfo.deathDown.loop = currentAnimation.attribute("loop").as_bool();
	for (currentAnimation = currentAnimation.child("frame"); currentAnimation; currentAnimation = currentAnimation.next_sibling("frame")) {
		footmanInfo.deathDown.PushBack({ currentAnimation.attribute("x").as_int(), currentAnimation.attribute("y").as_int(), currentAnimation.attribute("w").as_int(), currentAnimation.attribute("h").as_int() });
	}

	//Orcs
	pugi::xml_node orcEntities = config.child("dynamicEntities").child("orcs");

	//Grunt animations
	pugi::xml_node gruntAnimations = orcEntities.child("grunt").child("animations");
	
	//Up animation Grunt
	currentAnimation = gruntAnimations.child("up");
	gruntInfo.up.speed = currentAnimation.attribute("speed").as_float();
	gruntInfo.up.loop = currentAnimation.attribute("loop").as_bool();
	for (currentAnimation = currentAnimation.child("frame"); currentAnimation; currentAnimation = currentAnimation.next_sibling("frame")) {
		gruntInfo.up.PushBack({ currentAnimation.attribute("x").as_int(), currentAnimation.attribute("y").as_int(), currentAnimation.attribute("w").as_int(), currentAnimation.attribute("h").as_int() });
	}
	//Down animation Grunt
	currentAnimation = gruntAnimations.child("down");
	gruntInfo.down.speed = currentAnimation.attribute("speed").as_float();
	gruntInfo.down.loop = currentAnimation.attribute("loop").as_bool();
	for (currentAnimation = currentAnimation.child("frame"); currentAnimation; currentAnimation = currentAnimation.next_sibling("frame")) {
		gruntInfo.down.PushBack({ currentAnimation.attribute("x").as_int(), currentAnimation.attribute("y").as_int(), currentAnimation.attribute("w").as_int(), currentAnimation.attribute("h").as_int() });
	}
	//Left animation Grunt
	currentAnimation = gruntAnimations.child("left");
	gruntInfo.left.speed = currentAnimation.attribute("speed").as_float();
	gruntInfo.left.loop = currentAnimation.attribute("loop").as_bool();
	for (currentAnimation = currentAnimation.child("frame"); currentAnimation; currentAnimation = currentAnimation.next_sibling("frame")) {
		gruntInfo.left.PushBack({ currentAnimation.attribute("x").as_int(), currentAnimation.attribute("y").as_int(), currentAnimation.attribute("w").as_int(), currentAnimation.attribute("h").as_int() });
	}
	//Right animation Grunt
	currentAnimation = gruntAnimations.child("right");
	gruntInfo.right.speed = currentAnimation.attribute("speed").as_float();
	gruntInfo.right.loop = currentAnimation.attribute("loop").as_bool();
	for (currentAnimation = currentAnimation.child("frame"); currentAnimation; currentAnimation = currentAnimation.next_sibling("frame")) {
		gruntInfo.right.PushBack({ currentAnimation.attribute("x").as_int(), currentAnimation.attribute("y").as_int(), currentAnimation.attribute("w").as_int(), currentAnimation.attribute("h").as_int() });
	}
	//Up Left animation Grunt
	currentAnimation = gruntAnimations.child("upLeft");
	gruntInfo.upLeft.speed = currentAnimation.attribute("speed").as_float();
	gruntInfo.upLeft.loop = currentAnimation.attribute("loop").as_bool();
	for (currentAnimation = currentAnimation.child("frame"); currentAnimation; currentAnimation = currentAnimation.next_sibling("frame")) {
		gruntInfo.upLeft.PushBack({ currentAnimation.attribute("x").as_int(), currentAnimation.attribute("y").as_int(), currentAnimation.attribute("w").as_int(), currentAnimation.attribute("h").as_int() });
	}
	//Up Right animation Grunt
	currentAnimation = gruntAnimations.child("upRight");
	gruntInfo.upRight.speed = currentAnimation.attribute("speed").as_float();
	gruntInfo.upRight.loop = currentAnimation.attribute("loop").as_bool();
	for (currentAnimation = currentAnimation.child("frame"); currentAnimation; currentAnimation = currentAnimation.next_sibling("frame")) {
		gruntInfo.upRight.PushBack({ currentAnimation.attribute("x").as_int(), currentAnimation.attribute("y").as_int(), currentAnimation.attribute("w").as_int(), currentAnimation.attribute("h").as_int() });
	}
	//Down Left animation Grunt
	currentAnimation = gruntAnimations.child("downLeft");
	gruntInfo.downLeft.speed = currentAnimation.attribute("speed").as_float();
	gruntInfo.downLeft.loop = currentAnimation.attribute("loop").as_bool();
	for (currentAnimation = currentAnimation.child("frame"); currentAnimation; currentAnimation = currentAnimation.next_sibling("frame")) {
		gruntInfo.downLeft.PushBack({ currentAnimation.attribute("x").as_int(), currentAnimation.attribute("y").as_int(), currentAnimation.attribute("w").as_int(), currentAnimation.attribute("h").as_int() });
	}
	//Down Right animation Grunt
	currentAnimation = gruntAnimations.child("downRight");
	gruntInfo.downRight.speed = currentAnimation.attribute("speed").as_float();
	gruntInfo.downRight.loop = currentAnimation.attribute("loop").as_bool();
	for (currentAnimation = currentAnimation.child("frame"); currentAnimation; currentAnimation = currentAnimation.next_sibling("frame")) {
		gruntInfo.downRight.PushBack({ currentAnimation.attribute("x").as_int(), currentAnimation.attribute("y").as_int(), currentAnimation.attribute("w").as_int(), currentAnimation.attribute("h").as_int() });
	}
	//Attack Up animation Grunt
	currentAnimation = gruntAnimations.child("attackUp");
	gruntInfo.attackUp.speed = currentAnimation.attribute("speed").as_float();
	gruntInfo.attackUp.loop = currentAnimation.attribute("loop").as_bool();
	for (currentAnimation = currentAnimation.child("frame"); currentAnimation; currentAnimation = currentAnimation.next_sibling("frame")) {
		gruntInfo.attackUp.PushBack({ currentAnimation.attribute("x").as_int(), currentAnimation.attribute("y").as_int(), currentAnimation.attribute("w").as_int(), currentAnimation.attribute("h").as_int() });
	}
	//Attack Down animation Grunt
	currentAnimation = gruntAnimations.child("attackDown");
	gruntInfo.attackDown.speed = currentAnimation.attribute("speed").as_float();
	gruntInfo.attackDown.loop = currentAnimation.attribute("loop").as_bool();
	for (currentAnimation = currentAnimation.child("frame"); currentAnimation; currentAnimation = currentAnimation.next_sibling("frame")) {
		gruntInfo.attackDown.PushBack({ currentAnimation.attribute("x").as_int(), currentAnimation.attribute("y").as_int(), currentAnimation.attribute("w").as_int(), currentAnimation.attribute("h").as_int() });
	}
	//Attack Left animation Grunt
	currentAnimation = gruntAnimations.child("attackLeft");
	gruntInfo.attackLeft.speed = currentAnimation.attribute("speed").as_float();
	gruntInfo.attackLeft.loop = currentAnimation.attribute("loop").as_bool();
	for (currentAnimation = currentAnimation.child("frame"); currentAnimation; currentAnimation = currentAnimation.next_sibling("frame")) {
		gruntInfo.attackLeft.PushBack({ currentAnimation.attribute("x").as_int(), currentAnimation.attribute("y").as_int(), currentAnimation.attribute("w").as_int(), currentAnimation.attribute("h").as_int() });
	}
	//Attack Right animation Grunt
	currentAnimation = gruntAnimations.child("attackRight");
	gruntInfo.attackRight.speed = currentAnimation.attribute("speed").as_float();
	gruntInfo.attackRight.loop = currentAnimation.attribute("loop").as_bool();
	for (currentAnimation = currentAnimation.child("frame"); currentAnimation; currentAnimation = currentAnimation.next_sibling("frame")) {
		gruntInfo.attackRight.PushBack({ currentAnimation.attribute("x").as_int(), currentAnimation.attribute("y").as_int(), currentAnimation.attribute("w").as_int(), currentAnimation.attribute("h").as_int() });
	}
	//Attack Up Left animation Grunt
	currentAnimation = gruntAnimations.child("attackUpLeft");
	gruntInfo.attackUpLeft.speed = currentAnimation.attribute("speed").as_float();
	gruntInfo.attackUpLeft.loop = currentAnimation.attribute("loop").as_bool();
	for (currentAnimation = currentAnimation.child("frame"); currentAnimation; currentAnimation = currentAnimation.next_sibling("frame")) {
		gruntInfo.attackUpLeft.PushBack({ currentAnimation.attribute("x").as_int(), currentAnimation.attribute("y").as_int(), currentAnimation.attribute("w").as_int(), currentAnimation.attribute("h").as_int() });
	}
	//Attack Up Right animation Grunt
	currentAnimation = gruntAnimations.child("attackUpRight");
	gruntInfo.attackUpRight.speed = currentAnimation.attribute("speed").as_float();
	gruntInfo.attackUpRight.loop = currentAnimation.attribute("loop").as_bool();
	for (currentAnimation = currentAnimation.child("frame"); currentAnimation; currentAnimation = currentAnimation.next_sibling("frame")) {
		gruntInfo.attackUpRight.PushBack({ currentAnimation.attribute("x").as_int(), currentAnimation.attribute("y").as_int(), currentAnimation.attribute("w").as_int(), currentAnimation.attribute("h").as_int() });
	}
	//Attack Down Left animation Grunt
	currentAnimation = gruntAnimations.child("attackDownLeft");
	gruntInfo.attackDownLeft.speed = currentAnimation.attribute("speed").as_float();
	gruntInfo.attackDownLeft.loop = currentAnimation.attribute("loop").as_bool();
	for (currentAnimation = currentAnimation.child("frame"); currentAnimation; currentAnimation = currentAnimation.next_sibling("frame")) {
		gruntInfo.attackDownLeft.PushBack({ currentAnimation.attribute("x").as_int(), currentAnimation.attribute("y").as_int(), currentAnimation.attribute("w").as_int(), currentAnimation.attribute("h").as_int() });
	}
	//Attack Down Right animation Grunt
	currentAnimation = gruntAnimations.child("attackDownRight");
	gruntInfo.attackDownRight.speed = currentAnimation.attribute("speed").as_float();
	gruntInfo.attackDownRight.loop = currentAnimation.attribute("loop").as_bool();
	for (currentAnimation = currentAnimation.child("frame"); currentAnimation; currentAnimation = currentAnimation.next_sibling("frame")) {
		gruntInfo.attackDownRight.PushBack({ currentAnimation.attribute("x").as_int(), currentAnimation.attribute("y").as_int(), currentAnimation.attribute("w").as_int(), currentAnimation.attribute("h").as_int() });
	}
	//Attack Death Up animation Grunt
	currentAnimation = gruntAnimations.child("deathUp");
	gruntInfo.deathUp.speed = currentAnimation.attribute("speed").as_float();
	gruntInfo.deathUp.loop = currentAnimation.attribute("loop").as_bool();
	for (currentAnimation = currentAnimation.child("frame"); currentAnimation; currentAnimation = currentAnimation.next_sibling("frame")) {
		gruntInfo.deathUp.PushBack({ currentAnimation.attribute("x").as_int(), currentAnimation.attribute("y").as_int(), currentAnimation.attribute("w").as_int(), currentAnimation.attribute("h").as_int() });
	}
	//Attack Death Down animation Grunt
	currentAnimation = gruntAnimations.child("deathDown");
	gruntInfo.deathDown.speed = currentAnimation.attribute("speed").as_float();
	gruntInfo.deathDown.loop = currentAnimation.attribute("loop").as_bool();
	for (currentAnimation = currentAnimation.child("frame"); currentAnimation; currentAnimation = currentAnimation.next_sibling("frame")) {
		gruntInfo.deathDown.PushBack({ currentAnimation.attribute("x").as_int(), currentAnimation.attribute("y").as_int(), currentAnimation.attribute("w").as_int(), currentAnimation.attribute("h").as_int() });
	}
	

	return ret;
}

bool j1EntityFactory::Start()
{
	bool ret = true;

	LOG("Loading entities textures");

	humanBuildingsTex = App->tex->Load(humanBuildingsTexName.data());
	neutralBuildingsTex = App->tex->Load(neutralBuildingsTexName.data());
	orcishBuildingsTex = App->tex->Load(orcishBuildingsTexName.data());
	footmanTex = App->tex->Load(footmanTexName.data());
	gruntTex = App->tex->Load(gruntTexName.data());

	return ret;
}

bool j1EntityFactory::PreUpdate()
{
	bool ret = true;

	// Spawn entities
	list<Entity*>::const_iterator it = toSpawnEntities.begin();

	while (it != toSpawnEntities.end()) {

		fPoint pos = (*it)->GetPosition();
		int x = pos.x * App->scene->scale;
		int y = pos.y * App->scene->scale;
		App->map->WorldToMap(x, y);

		// Move the entity from the waiting list to the active list
		if ((*it)->entityType == EntityType_DynamicEntity) {

			activeDynamicEntities.push_back((DynamicEntity*)(*it));
			LOG("Spawning dynamic entity at tile %d,%d", x, y);
		}
		else if ((*it)->entityType == EntityType_StaticEntity) {

			activeStaticEntities.push_back((StaticEntity*)(*it));
			LOG("Spawning static entity at tile %d,%d", x, y);
		}

		it++;
	}
	toSpawnEntities.clear();

	return ret;
}

// Called before render is available
bool j1EntityFactory::Update(float dt)
{
	bool ret = true;
	// Update active dynamic entities
	list<StaticEntity*>::const_iterator it = activeStaticEntities.begin();

	while (it != activeStaticEntities.end()) {
		(*it)->Move(dt);
		it++;
	}

	return ret;
}

void j1EntityFactory::Draw()
{
	// Blit active entities

	// Dynamic entities (one texture per dynamic entity)
	list<DynamicEntity*>::const_iterator dynEnt = activeDynamicEntities.begin();

	while (dynEnt != activeDynamicEntities.end()) {

		switch ((*dynEnt)->dynamicEntityType) {

			// Player
		case DynamicEntityType_Footman:
			(*dynEnt)->Draw(footmanTex);
			break;
		case DynamicEntityType_ElvenArcher:
			(*dynEnt)->Draw(elvenArcherTex);
			break;
		case DynamicEntityType_GryphonRider:
			(*dynEnt)->Draw(gryphonRiderTex);
			break;
		case DynamicEntityType_Mage:
			(*dynEnt)->Draw(mageTex);
			break;
		case DynamicEntityType_Paladin:
			(*dynEnt)->Draw(paladinTex);
			break;

		case DynamicEntityType_Turalyon:
			(*dynEnt)->Draw(turalyonTex);
			break;
		case DynamicEntityType_Khadgar:
			(*dynEnt)->Draw(khadgarTex);
			break;
		case DynamicEntityType_Alleria:
			(*dynEnt)->Draw(alleriaTex);
			break;

			// Enemy
		case DynamicEntityType_Grunt:
			(*dynEnt)->Draw(gruntTex);
			break;
		case DynamicEntityType_TrollAxethrower:
			(*dynEnt)->Draw(trollAxethrowerTex);
			break;
		case DynamicEntityType_Dragon:
			(*dynEnt)->Draw(dragonTex);
			break;

		default:
			break;
		}

		dynEnt++;
	}

	// Static entities (altas textures for every type of static entity)
	list<StaticEntity*>::const_iterator statEnt = activeStaticEntities.begin();

	while (statEnt != activeStaticEntities.end()) {

		switch ((*statEnt)->staticEntityCategory) {

		case StaticEntityCategory_HumanBuilding:
			(*statEnt)->Draw(humanBuildingsTex);
			break;
		case StaticEntityCategory_NeutralBuilding:
			(*statEnt)->Draw(neutralBuildingsTex);
			break;
		case StaticEntityCategory_OrcishBuilding:
			(*statEnt)->Draw(orcishBuildingsTex);
			break;

		default:
			break;
		}

		statEnt++;
	}

	DrawStaticEntityPreview(App->scene->GetAlphaBuilding(), App->player->GetMousePos());
}

void j1EntityFactory::DrawStaticEntityPreview(StaticEntityType staticEntityType, iPoint mousePos)
{
	switch (staticEntityType) {

	case StaticEntityType_ChickenFarm:
		SDL_SetTextureAlphaMod(humanBuildingsTex, previewBuildingOpacity);
		App->render->Blit(humanBuildingsTex, mousePos.x, mousePos.y, &chickenFarmInfo.completeTexArea);
		break;
	case StaticEntityType_ElvenLumberMill:
		SDL_SetTextureAlphaMod(humanBuildingsTex, previewBuildingOpacity);
		App->render->Blit(humanBuildingsTex, mousePos.x, mousePos.y, &elvenLumberMillInfo.completeTexArea);
		break;
	case StaticEntityType_MageTower:
		SDL_SetTextureAlphaMod(humanBuildingsTex, previewBuildingOpacity);
		App->render->Blit(humanBuildingsTex, mousePos.x, mousePos.y, &mageTowerInfo.completeTexArea);
		break;
	case StaticEntityType_GryphonAviary:
		SDL_SetTextureAlphaMod(humanBuildingsTex, previewBuildingOpacity);
		App->render->Blit(humanBuildingsTex, mousePos.x, mousePos.y, &gryphonAviaryInfo.completeTexArea);
		break;
	case StaticEntityType_Stables:
		SDL_SetTextureAlphaMod(humanBuildingsTex, previewBuildingOpacity);
		App->render->Blit(humanBuildingsTex, mousePos.x, mousePos.y, &stablesInfo.completeTexArea);
		break;
	case StaticEntityType_ScoutTower:
		SDL_SetTextureAlphaMod(humanBuildingsTex, previewBuildingOpacity);
		App->render->Blit(humanBuildingsTex, mousePos.x, mousePos.y, &scoutTowerInfo.completeTexArea);
		break;
	case StaticEntityType_PlayerGuardTower:
		SDL_SetTextureAlphaMod(humanBuildingsTex, previewBuildingOpacity);
		App->render->Blit(humanBuildingsTex, mousePos.x, mousePos.y, &playerGuardTowerInfo.completeTexArea);
		break;
	case StaticEntityType_PlayerCannonTower:
		SDL_SetTextureAlphaMod(humanBuildingsTex, previewBuildingOpacity);
		App->render->Blit(humanBuildingsTex, mousePos.x, mousePos.y, &playerCannonTowerInfo.completeTexArea);
		break;
	case StaticEntityType_NoType:
		break;
	default:
		break;
	}
	
	HandleStaticEntityPreviewTiles(staticEntityType, mousePos);

}

//Handles when to return green or red tiles 
void j1EntityFactory::HandleStaticEntityPreviewTiles(StaticEntityType staticEntityType, iPoint mousePos)
{
	SDL_SetTextureAlphaMod(neutralBuildingsTex, buildingPreviewTiles.opacity);


	switch (staticEntityType) {

	case StaticEntityType_ChickenFarm:
	case StaticEntityType_ScoutTower:
		DrawStaticEntityPreviewTiles(true, Small, mousePos);
		if (isEntityOnTile(App->player->GetMouseTilePos(), Small)) {
			DrawStaticEntityPreviewTiles(false, Small, mousePos);
		}
		break;
	case StaticEntityType_ElvenLumberMill:
	case StaticEntityType_MageTower:
	case StaticEntityType_GryphonAviary:
	case StaticEntityType_Stables:
		DrawStaticEntityPreviewTiles(true, Medium, mousePos);
		if (isEntityOnTile(App->player->GetMouseTilePos(), Medium)) {
			DrawStaticEntityPreviewTiles(false, Medium, mousePos);
		}
		break;
	default:
		break;
	}
}

//Returns green or red tiles depending on the size of the building
void j1EntityFactory::DrawStaticEntityPreviewTiles(bool isPlaceable, StaticEntitySize buildingSize, iPoint mousePos)
{
	switch (buildingSize) {
	case Small:
		if (isPlaceable) {
			App->render->Blit(neutralBuildingsTex, mousePos.x, mousePos.y, &buildingPreviewTiles.greenTile);
			App->render->Blit(neutralBuildingsTex, mousePos.x + 32, mousePos.y, &buildingPreviewTiles.greenTile);
			App->render->Blit(neutralBuildingsTex, mousePos.x, mousePos.y + 32, &buildingPreviewTiles.greenTile);
			App->render->Blit(neutralBuildingsTex, mousePos.x + 32, mousePos.y + 32, &buildingPreviewTiles.greenTile);
		}
		else if (!isPlaceable) {
			App->render->Blit(neutralBuildingsTex, mousePos.x, mousePos.y, &buildingPreviewTiles.redTile);
			App->render->Blit(neutralBuildingsTex, mousePos.x + 32, mousePos.y, &buildingPreviewTiles.redTile);
			App->render->Blit(neutralBuildingsTex, mousePos.x, mousePos.y + 32, &buildingPreviewTiles.redTile);
			App->render->Blit(neutralBuildingsTex, mousePos.x + 32, mousePos.y + 32, &buildingPreviewTiles.redTile);
		}
		break;
	case Medium:
		if (isPlaceable) {
			App->render->Blit(neutralBuildingsTex, mousePos.x, mousePos.y, &buildingPreviewTiles.greenTile);
			App->render->Blit(neutralBuildingsTex, mousePos.x + 32, mousePos.y, &buildingPreviewTiles.greenTile);
			App->render->Blit(neutralBuildingsTex, mousePos.x, mousePos.y + 32, &buildingPreviewTiles.greenTile);
			App->render->Blit(neutralBuildingsTex, mousePos.x + 32, mousePos.y + 32, &buildingPreviewTiles.greenTile);
			App->render->Blit(neutralBuildingsTex, mousePos.x + 32, mousePos.y + 64, &buildingPreviewTiles.greenTile);
			App->render->Blit(neutralBuildingsTex, mousePos.x + 64, mousePos.y + 32, &buildingPreviewTiles.greenTile);
			App->render->Blit(neutralBuildingsTex, mousePos.x + 64, mousePos.y, &buildingPreviewTiles.greenTile);
			App->render->Blit(neutralBuildingsTex, mousePos.x, mousePos.y + 64, &buildingPreviewTiles.greenTile);
			App->render->Blit(neutralBuildingsTex, mousePos.x + 64, mousePos.y + 64, &buildingPreviewTiles.greenTile);
		}
		else if (!isPlaceable) {
			App->render->Blit(neutralBuildingsTex, mousePos.x, mousePos.y, &buildingPreviewTiles.redTile);
			App->render->Blit(neutralBuildingsTex, mousePos.x + 32, mousePos.y, &buildingPreviewTiles.redTile);
			App->render->Blit(neutralBuildingsTex, mousePos.x, mousePos.y + 32, &buildingPreviewTiles.redTile);
			App->render->Blit(neutralBuildingsTex, mousePos.x + 32, mousePos.y + 32, &buildingPreviewTiles.redTile);
			App->render->Blit(neutralBuildingsTex, mousePos.x + 32, mousePos.y + 64, &buildingPreviewTiles.redTile);
			App->render->Blit(neutralBuildingsTex, mousePos.x + 64, mousePos.y + 32, &buildingPreviewTiles.redTile);
			App->render->Blit(neutralBuildingsTex, mousePos.x + 64, mousePos.y, &buildingPreviewTiles.redTile);
			App->render->Blit(neutralBuildingsTex, mousePos.x, mousePos.y + 64, &buildingPreviewTiles.redTile);
			App->render->Blit(neutralBuildingsTex, mousePos.x + 64, mousePos.y + 64, &buildingPreviewTiles.redTile);
		}
		break;
	case Big:
		if (isPlaceable) {
			App->render->Blit(humanBuildingsTex, mousePos.x, mousePos.y, &buildingPreviewTiles.greenTile);
			App->render->Blit(humanBuildingsTex, mousePos.x + 32, mousePos.y, &buildingPreviewTiles.greenTile);
			App->render->Blit(humanBuildingsTex, mousePos.x, mousePos.y + 32, &buildingPreviewTiles.greenTile);
			App->render->Blit(humanBuildingsTex, mousePos.x + 32, mousePos.y + 32, &buildingPreviewTiles.greenTile);
			App->render->Blit(humanBuildingsTex, mousePos.x + 32, mousePos.y + 64, &buildingPreviewTiles.greenTile);
			App->render->Blit(humanBuildingsTex, mousePos.x + 64, mousePos.y + 32, &buildingPreviewTiles.greenTile);
			App->render->Blit(humanBuildingsTex, mousePos.x + 64, mousePos.y, &buildingPreviewTiles.greenTile);
			App->render->Blit(humanBuildingsTex, mousePos.x, mousePos.y + 64, &buildingPreviewTiles.greenTile);
			App->render->Blit(humanBuildingsTex, mousePos.x + 64, mousePos.y + 64, &buildingPreviewTiles.greenTile);
			App->render->Blit(humanBuildingsTex, mousePos.x + 32, mousePos.y + 96, &buildingPreviewTiles.greenTile);
			App->render->Blit(humanBuildingsTex, mousePos.x + 64, mousePos.y + 96, &buildingPreviewTiles.greenTile);
			App->render->Blit(humanBuildingsTex, mousePos.x + 96, mousePos.y + 32, &buildingPreviewTiles.greenTile);
			App->render->Blit(humanBuildingsTex, mousePos.x + 96, mousePos.y + 64, &buildingPreviewTiles.greenTile);
			App->render->Blit(humanBuildingsTex, mousePos.x + 96, mousePos.y, &buildingPreviewTiles.greenTile);
			App->render->Blit(humanBuildingsTex, mousePos.x, mousePos.y + 96, &buildingPreviewTiles.greenTile);
			App->render->Blit(humanBuildingsTex, mousePos.x + 96, mousePos.y + 96, &buildingPreviewTiles.greenTile);
		}
		else if (!isPlaceable) {
			App->render->Blit(humanBuildingsTex, mousePos.x, mousePos.y, &buildingPreviewTiles.redTile);
			App->render->Blit(humanBuildingsTex, mousePos.x + 32, mousePos.y, &buildingPreviewTiles.redTile);
			App->render->Blit(humanBuildingsTex, mousePos.x, mousePos.y + 32, &buildingPreviewTiles.redTile);
			App->render->Blit(humanBuildingsTex, mousePos.x + 32, mousePos.y + 32, &buildingPreviewTiles.redTile);
			App->render->Blit(humanBuildingsTex, mousePos.x + 32, mousePos.y + 64, &buildingPreviewTiles.redTile);
			App->render->Blit(humanBuildingsTex, mousePos.x + 64, mousePos.y + 32, &buildingPreviewTiles.redTile);
			App->render->Blit(humanBuildingsTex, mousePos.x + 64, mousePos.y, &buildingPreviewTiles.redTile);
			App->render->Blit(humanBuildingsTex, mousePos.x, mousePos.y + 64, &buildingPreviewTiles.redTile);
			App->render->Blit(humanBuildingsTex, mousePos.x + 64, mousePos.y + 64, &buildingPreviewTiles.redTile);
			App->render->Blit(humanBuildingsTex, mousePos.x + 32, mousePos.y + 96, &buildingPreviewTiles.redTile);
			App->render->Blit(humanBuildingsTex, mousePos.x + 64, mousePos.y + 96, &buildingPreviewTiles.redTile);
			App->render->Blit(humanBuildingsTex, mousePos.x + 96, mousePos.y + 32, &buildingPreviewTiles.redTile);
			App->render->Blit(humanBuildingsTex, mousePos.x + 96, mousePos.y + 64, &buildingPreviewTiles.redTile);
			App->render->Blit(humanBuildingsTex, mousePos.x + 96, mousePos.y, &buildingPreviewTiles.redTile);
			App->render->Blit(humanBuildingsTex, mousePos.x, mousePos.y + 96, &buildingPreviewTiles.redTile);
			App->render->Blit(humanBuildingsTex, mousePos.x + 96, mousePos.y + 96, &buildingPreviewTiles.redTile);
		}
		break;
	case None:
		break;
	default:
		break;
	}

}

const EntityInfo& j1EntityFactory::GetBuildingInfo(StaticEntityType staticEntityType) 
{
	switch (staticEntityType) {
	case StaticEntityType_ChickenFarm:
		return (const EntityInfo&)chickenFarmInfo;
		break;
	case StaticEntityType_Barracks:
		return (const EntityInfo&)barracksInfo;
		break;
	case StaticEntityType_ElvenLumberMill:
		return (const EntityInfo&)elvenLumberMillInfo;
		break;
	case StaticEntityType_MageTower:
		return(const EntityInfo&)mageTowerInfo;
		break;
	case StaticEntityType_GryphonAviary:
		return(const EntityInfo&)gryphonAviaryInfo;
		break;
	case StaticEntityType_Stables:
		return(const EntityInfo&)stablesInfo;
		break;
	case StaticEntityType_ScoutTower:
		return(const EntityInfo&)scoutTowerInfo;
		break;
	case StaticEntityType_PlayerGuardTower:
		return(const EntityInfo&)playerGuardTowerInfo;
		break;
	case StaticEntityType_PlayerCannonTower:
		return(const EntityInfo&)playerCannonTowerInfo;
		break;
	default:
		return (const EntityInfo&)chickenFarmInfo;
		break;
	}

	//return ret;
}

SDL_Texture* j1EntityFactory::GetHumanBuildingTexture() {

	return humanBuildingsTex;
}

SDL_Texture* j1EntityFactory::GetNeutralBuildingTexture() {

	return neutralBuildingsTex;
}

// Returns true if there is an entity on the tile
bool j1EntityFactory::isEntityOnTile(iPoint tile, StaticEntitySize buildingSize) const
{
	//Dynamic entities
	list<DynamicEntity*>::const_iterator activeDyn = activeDynamicEntities.begin();

	while (activeDyn != activeDynamicEntities.end()) {
	
		iPoint entityTile = App->map->WorldToMap((*activeDyn)->GetPosition().x, (*activeDyn)->GetPosition().y);
		if (tile.x == entityTile.x && tile.y == entityTile.y) 
		return true;

		activeDyn++;
	}

	//Static entities
	list<StaticEntity*>::const_iterator activeStatic = activeStaticEntities.begin();

	while (activeStatic != activeStaticEntities.end()) {

			iPoint entityTile = App->map->WorldToMap((*activeStatic)->GetPosition().x, (*activeStatic)->GetPosition().y);
			
			//This checks all of the tiles that conform of the static entity and their sorrounding tiles
			if ((*activeStatic)->GetSize().x == 64 && (*activeStatic)->GetSize().y == 64) {
				switch (buildingSize)
				{
				case Small:
					for (int i = -1; i < 2; i++) {
						for (int j = -1; j < 2; j++) {
							if (tile.x == entityTile.x + i && tile.y == entityTile.y + j)
								return true;
							}
						}
					break;
				case Medium:
					for (int i = -2; i < 2; i++) {
						for (int j = -2; j < 2; j++) {
							if (tile.x == entityTile.x + i && tile.y == entityTile.y + j)
								return true;
						}
					}
					break;
				case Big:
					for (int i = -3; i < 2; i++) {
						for (int j = -3; j < 2; j++) {
							if (tile.x == entityTile.x + i && tile.y == entityTile.y + j)
								return true;
						}
					}
					break;
				default:
					break;
				}
			}
			else if ((*activeStatic)->GetSize().x == 96 && (*activeStatic)->GetSize().y == 96) {
				switch (buildingSize)
				{
				case Small:
					for (int i = -1; i < 3; i++) {
						for (int j = -1; j < 3; j++) {
							if (tile.x == entityTile.x + i && tile.y == entityTile.y + j)
								return true;
						}
					}
					break;
				case Medium:
					for (int i = -2; i < 3; i++) {
						for (int j = -2; j < 3; j++) {
							if (tile.x == entityTile.x + i && tile.y == entityTile.y + j)
								return true;
						}
					}
					break;
				case Big:
					for (int i = -3; i < 3; i++) {
						for (int j = -3; j < 3; j++) {
							if (tile.x == entityTile.x + i && tile.y == entityTile.y + j)
								return true;
						}
					}
					break;
				default:
					break;
				}
			}
			else if ((*activeStatic)->GetSize().x == 128 && (*activeStatic)->GetSize().y == 128) {
				switch (buildingSize)
				{
				case Small:
					for (int i = -1; i < 4; i++) {
						for (int j = -1; j < 4; j++) {
							if (tile.x == entityTile.x + i && tile.y == entityTile.y + j)
								return true;
						}
					}
					break;
				case Medium:
					for (int i = -2; i < 4; i++) {
						for (int j = -2; j < 4; j++) {
							if (tile.x == entityTile.x + i && tile.y == entityTile.y + j)
								return true;
						}
					}
					break;
				case Big:
					for (int i = -3; i < 4; i++) {
						for (int j = -3; j < 4; j++) {
							if (tile.x == entityTile.x + i && tile.y == entityTile.y + j)
								return true;
						}
					}
					break;
				default:
					break;
				}
			}

			activeStatic++;
	}

	// We do also need to check the toSpawn list (just in case)
	list<Entity*>::const_iterator toSpawn = toSpawnEntities.begin();

	while (toSpawn != toSpawnEntities.end()) {

			iPoint entityTile = App->map->WorldToMap((*toSpawn)->GetPosition().x, (*toSpawn)->GetPosition().y);

			if (tile.x == entityTile.x && tile.y == entityTile.y)
				return true;

		toSpawn++;
	}

	return false;
}

bool j1EntityFactory::PostUpdate()
{
	bool ret = true;

	// Remove active entities
	/// Remove dynamic entities
	list<DynamicEntity*>::const_iterator dynEnt = activeDynamicEntities.begin();

	while (dynEnt != activeDynamicEntities.end()) {
		if ((*dynEnt)->remove) {
			delete *dynEnt;
			activeDynamicEntities.remove(*dynEnt);
		}

		dynEnt++;
	}

	/// Remove static entities
	list<StaticEntity*>::const_iterator statEnt = activeStaticEntities.begin();

	while (statEnt != activeStaticEntities.end()) {
		if ((*statEnt)->remove) {
			delete *statEnt;
			activeStaticEntities.remove(*statEnt);
		}

		statEnt++;
	}

	return ret;
}

// Called before quitting
bool j1EntityFactory::CleanUp()
{
	bool ret = true;

	LOG("Freeing all entities");

	// Remove active entities
	/// Remove dynamic entities
	list<DynamicEntity*>::const_iterator dynEnt = activeDynamicEntities.begin();

	while (dynEnt != activeDynamicEntities.end()) {
		if ((*dynEnt)->remove) {
			delete *dynEnt;
			activeDynamicEntities.remove(*dynEnt);
		}

		dynEnt++;
	}
	activeDynamicEntities.clear();

	/// Remove static entities
	list<StaticEntity*>::const_iterator statEnt = activeStaticEntities.begin();

	while (statEnt != activeStaticEntities.end()) {
		if ((*statEnt)->remove) {
			delete *statEnt;
			activeStaticEntities.remove(*statEnt);
		}

		statEnt++;
	}
	activeStaticEntities.clear();

	// Remove to spawn entities
	list<Entity*>::const_iterator it = toSpawnEntities.begin();

	while (it != toSpawnEntities.end()) {
		delete *it;
		it++;
	}
	toSpawnEntities.clear();

	// Free all textures
	App->tex->UnLoad(footmanTex);

	return ret;
}
StaticEntity* j1EntityFactory::AddStaticEntity(StaticEntityType staticEntityType, fPoint pos, const EntityInfo& entityInfo, j1Module* listener)
{
	switch (staticEntityType) {

	case StaticEntityType_TownHall:
	{
		TownHall* townHall = new TownHall(pos, { 128,128 }, townHallInfo.townHallMaxLife, (const TownHallInfo&)entityInfo, listener);
		townHall->entityType = EntityType_StaticEntity;
		townHall->staticEntityCategory = StaticEntityCategory_HumanBuilding;
		townHall->staticEntityType = StaticEntityType_TownHall;

		toSpawnEntities.push_back((Entity*)townHall);
		return (StaticEntity*)townHall;
	}
	break;

	case StaticEntityType_ChickenFarm:
	{
		ChickenFarm* chickenFarm = new ChickenFarm(pos, { 64,64 }, chickenFarmInfo.maxLife, (const ChickenFarmInfo&)entityInfo, listener);
		chickenFarm->entityType = EntityType_StaticEntity;
		chickenFarm->staticEntityCategory = StaticEntityCategory_HumanBuilding;
		chickenFarm->staticEntityType = StaticEntityType_ChickenFarm;


		toSpawnEntities.push_back((Entity*)chickenFarm);
		return (StaticEntity*)chickenFarm;
	}
	break;

	case StaticEntityType_Barracks:
	{
		Barracks* barracks = new Barracks(pos, { 96,96 }, barracksInfo.barracks1MaxLife, (const BarracksInfo&)entityInfo, listener);
		barracks->entityType = EntityType_StaticEntity;
		barracks->staticEntityCategory = StaticEntityCategory_HumanBuilding;
		barracks->staticEntityType = StaticEntityType_Barracks;

		toSpawnEntities.push_back((Entity*)barracks);
		return (StaticEntity*)barracks;
	}
	break;

	case StaticEntityType_ElvenLumberMill:
	{
		ElvenLumberMill* elvenLumberMill = new ElvenLumberMill(pos, { 96,96 }, elvenLumberMillInfo.maxLife, (const ElvenLumberMillInfo&)entityInfo, listener);
		elvenLumberMill->entityType = EntityType_StaticEntity;
		elvenLumberMill->staticEntityCategory = StaticEntityCategory_HumanBuilding;
		elvenLumberMill->staticEntityType = StaticEntityType_Barracks;

		toSpawnEntities.push_back((Entity*)elvenLumberMill);
		return (StaticEntity*)elvenLumberMill;
	}
	break;

	case StaticEntityType_MageTower:
	{
		MageTower* mageTower = new MageTower(pos, { 96,96 }, mageTowerInfo.maxLife, (const MageTowerInfo&)entityInfo, listener);
		mageTower->entityType = EntityType_StaticEntity;
		mageTower->staticEntityCategory = StaticEntityCategory_HumanBuilding;
		mageTower->staticEntityType = StaticEntityType_MageTower;

		toSpawnEntities.push_back((Entity*)mageTower);
		return (StaticEntity*)mageTower;
	}
	break;

	case StaticEntityType_GryphonAviary:
	{
		GryphonAviary* gryphonAviary = new GryphonAviary(pos, { 96,96 }, gryphonAviaryInfo.maxLife, (const GryphonAviaryInfo&)entityInfo, listener);
		gryphonAviary->entityType = EntityType_StaticEntity;
		gryphonAviary->staticEntityCategory = StaticEntityCategory_HumanBuilding;
		gryphonAviary->staticEntityType = StaticEntityType_GryphonAviary;

		toSpawnEntities.push_back((Entity*)gryphonAviary);
		return (StaticEntity*)gryphonAviary;
	}
	break;

	case StaticEntityType_Stables:
	{
		Stables* stables = new Stables(pos, { 96,96 }, stablesInfo.maxLife, (const StablesInfo&)entityInfo, listener);
		stables->entityType = EntityType_StaticEntity;
		stables->staticEntityCategory = StaticEntityCategory_HumanBuilding;
		stables->staticEntityType = StaticEntityType_Stables;

		toSpawnEntities.push_back((Entity*)stables);
		return (StaticEntity*)stables;
	}
	break;

	case StaticEntityType_ScoutTower:
	{
		ScoutTower* scoutTower = new ScoutTower(pos, { 64,64 }, scoutTowerInfo.maxLife, (const ScoutTowerInfo&)entityInfo, listener);
		scoutTower->entityType = EntityType_StaticEntity;
		scoutTower->staticEntityCategory = StaticEntityCategory_HumanBuilding;
		scoutTower->staticEntityType = StaticEntityType_ScoutTower;

		toSpawnEntities.push_back((Entity*)scoutTower);
		return (StaticEntity*)scoutTower;
	}
	break;

	case StaticEntityType_PlayerGuardTower:
	{
		PlayerGuardTower* playerGuardTower = new PlayerGuardTower(pos, { 64,64 }, 12, (const PlayerGuardTowerInfo&)entityInfo, listener);
		playerGuardTower->entityType = EntityType_StaticEntity;
		playerGuardTower->staticEntityCategory = StaticEntityCategory_HumanBuilding;
		playerGuardTower->staticEntityType = StaticEntityType_PlayerGuardTower;

		toSpawnEntities.push_back((Entity*)playerGuardTower);
		return (StaticEntity*)playerGuardTower;
	}
	break;

	case StaticEntityType_PlayerCannonTower:
	{
		PlayerCannonTower* playerCannonTower = new PlayerCannonTower(pos, { 64,64 }, 12, (const PlayerCannonTowerInfo&)entityInfo, listener);
		playerCannonTower->entityType = EntityType_StaticEntity;
		playerCannonTower->staticEntityCategory = StaticEntityCategory_HumanBuilding;
		playerCannonTower->staticEntityType = StaticEntityType_PlayerCannonTower;

		toSpawnEntities.push_back((Entity*)playerCannonTower);
		return (StaticEntity*)playerCannonTower;
	}
	break;

	case StaticEntityType_GoldMine:
	{
		GoldMine* goldMine = new GoldMine(pos, { 128,128 }, goldMineInfo.maxLife, (const GoldMineInfo&)entityInfo, listener);
		goldMine->entityType = EntityType_StaticEntity;
		goldMine->staticEntityCategory = StaticEntityCategory_NeutralBuilding;
		goldMine->staticEntityType = StaticEntityType_GoldMine;

		toSpawnEntities.push_back((Entity*)goldMine);
		return (StaticEntity*)goldMine;
	}
	break;

	case StaticEntityType_Runestone:
	{
		Runestone* runestone = new Runestone(pos, { 64,64 }, 0, (const RunestoneInfo&)entityInfo, listener);
		runestone->entityType = EntityType_StaticEntity;
		runestone->staticEntityCategory = StaticEntityCategory_NeutralBuilding;
		runestone->staticEntityType = StaticEntityType_Runestone;

		toSpawnEntities.push_back((Entity*)runestone);
		return (StaticEntity*)runestone;
	}
	break;

	case StaticEntityType_WatchTower:
	{
		WatchTower* watchTower = new WatchTower(pos, { 64,64 }, 12, (const WatchTowerInfo&)entityInfo, listener);
		watchTower->entityType = EntityType_StaticEntity;
		watchTower->staticEntityCategory = StaticEntityCategory_OrcishBuilding;
		watchTower->staticEntityType = StaticEntityType_WatchTower;

		toSpawnEntities.push_back((Entity*)watchTower);
		return (StaticEntity*)watchTower;
	}
	break;

	case StaticEntityType_EnemyGuardTower:
	{
		EnemyGuardTower* enemyGuardTower = new EnemyGuardTower(pos, { 64,64 }, 12, (const EnemyGuardTowerInfo&)entityInfo, listener);
		enemyGuardTower->entityType = EntityType_StaticEntity;
		enemyGuardTower->staticEntityCategory = StaticEntityCategory_OrcishBuilding;
		enemyGuardTower->staticEntityType = StaticEntityType_EnemyGuardTower;

		toSpawnEntities.push_back((Entity*)enemyGuardTower);
		return (StaticEntity*)enemyGuardTower;
	}
	break;

	case StaticEntityType_EnemyCannonTower:
	{
		EnemyCannonTower* enemyCannonTower = new EnemyCannonTower(pos, { 64,64 }, 12, (const EnemyCannonTowerInfo&)entityInfo, listener);
		enemyCannonTower->entityType = EntityType_StaticEntity;
		enemyCannonTower->staticEntityCategory = StaticEntityCategory_OrcishBuilding;
		enemyCannonTower->staticEntityType = StaticEntityType_EnemyCannonTower;

		toSpawnEntities.push_back((Entity*)enemyCannonTower);
		return (StaticEntity*)enemyCannonTower;
	}
	break;

	default:

		return nullptr;
	}
}

DynamicEntity* j1EntityFactory::AddDynamicEntity(DynamicEntityType dynamicEntityType, fPoint pos, iPoint size, uint life, float speed, const EntityInfo& entityInfo, j1Module* listener)
{
	switch (dynamicEntityType) {

	case DynamicEntityType_Footman:
	{
		Footman* footman = new Footman(pos, size, life, speed, (const FootmanInfo&)entityInfo, listener);
		footman->entityType = EntityType_DynamicEntity;
		footman->dynamicEntityType = DynamicEntityType_Footman;

		toSpawnEntities.push_back((Entity*)footman);
		return (DynamicEntity*)footman;
	}
	break;

	case DynamicEntityType_ElvenArcher:
	{
		ElvenArcher* elvenArcher = new ElvenArcher(pos, size, life, speed, (const ElvenArcherInfo&)entityInfo, listener);
		elvenArcher->entityType = EntityType_DynamicEntity;
		elvenArcher->dynamicEntityType = DynamicEntityType_ElvenArcher;

		toSpawnEntities.push_back((Entity*)elvenArcher);
		return (DynamicEntity*)elvenArcher;
	}
	break;

	case DynamicEntityType_GryphonRider:
	{
		GryphonRider* gryphonRider = new GryphonRider(pos, size, life, speed, (const GryphonRiderInfo&)entityInfo, listener);
		gryphonRider->entityType = EntityType_DynamicEntity;
		gryphonRider->dynamicEntityType = DynamicEntityType_GryphonRider;

		toSpawnEntities.push_back((Entity*)gryphonRider);
		return (DynamicEntity*)gryphonRider;
	}
	break;

	case DynamicEntityType_Mage:
	{
		Mage* mage = new Mage(pos, size, life, speed, (const MageInfo&)entityInfo, listener);
		mage->entityType = EntityType_DynamicEntity;
		mage->dynamicEntityType = DynamicEntityType_Mage;

		toSpawnEntities.push_back((Entity*)mage);
		return (DynamicEntity*)mage;
	}
	break;

	case DynamicEntityType_Paladin:
	{
		Paladin* paladin = new Paladin(pos, size, life, speed, (const PaladinInfo&)entityInfo, listener);
		paladin->entityType = EntityType_DynamicEntity;
		paladin->dynamicEntityType = DynamicEntityType_Paladin;

		toSpawnEntities.push_back((Entity*)paladin);
		return (DynamicEntity*)paladin;
	}
	break;

	case DynamicEntityType_Turalyon:
	{
		Turalyon* turalyon = new Turalyon(pos, size, life, speed, (const TuralyonInfo&)entityInfo, listener);
		turalyon->entityType = EntityType_DynamicEntity;
		turalyon->dynamicEntityType = DynamicEntityType_Turalyon;

		toSpawnEntities.push_back((Entity*)turalyon);
		return (DynamicEntity*)turalyon;
	}
	break;

	case DynamicEntityType_Khadgar:
	{
		Khadgar* khadgar = new Khadgar(pos, size, life, speed, (const KhadgarInfo&)entityInfo, listener);
		khadgar->entityType = EntityType_DynamicEntity;
		khadgar->dynamicEntityType = DynamicEntityType_Khadgar;

		toSpawnEntities.push_back((Entity*)khadgar);
		return (DynamicEntity*)khadgar;
	}
	break;

	case DynamicEntityType_Alleria:
	{
		Alleria* alleria = new Alleria(pos, size, life, speed, (const AlleriaInfo&)entityInfo, listener);
		alleria->entityType = EntityType_DynamicEntity;
		alleria->dynamicEntityType = DynamicEntityType_Alleria;

		toSpawnEntities.push_back((Entity*)alleria);
		return (DynamicEntity*)alleria;
	}
	break;

	case DynamicEntityType_Grunt:
	{
		Grunt* grunt = new Grunt(pos, size, life, speed, (const GruntInfo&)entityInfo, listener);
		grunt->entityType = EntityType_DynamicEntity;
		grunt->dynamicEntityType = DynamicEntityType_Grunt;

		toSpawnEntities.push_back((Entity*)grunt);
		return (DynamicEntity*)grunt;
	}
	break;

	case DynamicEntityType_TrollAxethrower:
	{
		TrollAxethrower* trollAxethrower = new TrollAxethrower(pos, size, life, speed, (const TrollAxethrowerInfo&)entityInfo, listener);
		trollAxethrower->entityType = EntityType_DynamicEntity;
		trollAxethrower->dynamicEntityType = DynamicEntityType_TrollAxethrower;

		toSpawnEntities.push_back((Entity*)trollAxethrower);
		return (DynamicEntity*)trollAxethrower;
	}
	break;

	case DynamicEntityType_Dragon:
	{
		Dragon* dragon = new Dragon(pos, size, life, speed, (const DragonInfo&)entityInfo, listener);
		dragon->entityType = EntityType_DynamicEntity;
		dragon->dynamicEntityType = DynamicEntityType_Dragon;

		toSpawnEntities.push_back((Entity*)dragon);
		return (DynamicEntity*)dragon;
	}
	break;

	default:

		return nullptr;
	}
}

// -------------------------------------------------------------
// -------------------------------------------------------------

bool j1EntityFactory::Load(pugi::xml_node& save)
{
	bool ret = true;

	pugi::xml_node node;

	list<DynamicEntity*>::const_iterator dynEnt = activeDynamicEntities.begin();

	while (dynEnt != activeDynamicEntities.end()) {
		// MYTODO: Add some code here
		dynEnt++;
	}

	list<StaticEntity*>::const_iterator statEnt = activeStaticEntities.begin();

	while (statEnt != activeStaticEntities.end()) {
		// MYTODO: Add some code here
		statEnt++;
	}

	return ret;
}

bool j1EntityFactory::Save(pugi::xml_node& save) const
{
	bool ret = true;

	pugi::xml_node node;

	list<DynamicEntity*>::const_iterator dynEnt = activeDynamicEntities.begin();

	while (dynEnt != activeDynamicEntities.end()) {
		// MYTODO: Add some code here
		dynEnt++;
	}

	list<StaticEntity*>::const_iterator statEnt = activeStaticEntities.begin();

	while (statEnt != activeStaticEntities.end()) {
		// MYTODO: Add some code here
		statEnt++;
	}

	return ret;
}