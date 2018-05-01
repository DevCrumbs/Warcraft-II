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
#include "j1Movement.h"
#include "Goal.h"
#include "j1Printer.h"
#include "j1PathManager.h"

#include "Entity.h"
#include "DynamicEntity.h"
#include "StaticEntity.h"

#include "Brofiler\Brofiler.h"

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
	crittersTexName = spritesheets.child("critters").attribute("name").as_string();
	khadgarTexName = spritesheets.child("khadgarAnimations").attribute("name").as_string();
	alleriaTexName = spritesheets.child("alleriaAnimations").attribute("name").as_string();
	turalyonTexName = spritesheets.child("turalyonAnimations").attribute("name").as_string();
	elvenArcherTexName = spritesheets.child("elvenArcherAnimations").attribute("name").as_string();
	trollAxethrowerTexName = spritesheets.child("trollAxethrowerAnimations").attribute("name").as_string();
	gryphonRiderTexName = spritesheets.child("gryphonRiderAnimations").attribute("name").as_string();
	dragonTexName = spritesheets.child("dragonAnimations").attribute("name").as_string();

	//Debug Textures Properties
	previewTilesopacity = config.child("previewTexturesProperties").attribute("tileBuildingPlaceOpacity").as_uint();
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

	//Scout Tower attributes
	scoutTowerInfo.sightRadius = { humanBuildings.child("scoutTower").child("attack").attribute("sightRadius").as_uint() };
	scoutTowerInfo.damage = { humanBuildings.child("scoutTower").child("attack").attribute("damage").as_uint() };
	scoutTowerInfo.attackWaitTime = { humanBuildings.child("scoutTower").child("attack").attribute("attackWaitTime").as_uint() };
	scoutTowerInfo.arrowSpeed = { humanBuildings.child("scoutTower").child("attack").attribute("arrowSpeed").as_uint() };
	scoutTowerInfo.maxLife = { humanBuildings.child("scoutTower").child("maxLife").attribute("value").as_int() };

	//Scout Tower animations
	aux = humanBuildings.child("scoutTower").child("sprites");
	scoutTowerInfo.completeTexArea = { aux.child("complete").attribute("x").as_int(), aux.child("complete").attribute("y").as_int(), aux.child("complete").attribute("w").as_int(), aux.child("complete").attribute("h").as_int() };
	scoutTowerInfo.inProgressTexArea = { aux.child("inProgress").attribute("x").as_int(), aux.child("inProgress").attribute("y").as_int(), aux.child("inProgress").attribute("w").as_int(), aux.child("inProgress").attribute("h").as_int() };

	//Player Guard Tower attributes
	playerGuardTowerInfo.sightRadius = { humanBuildings.child("guardTower").child("attack").attribute("sightRadius").as_uint() };
	playerGuardTowerInfo.damage = { humanBuildings.child("guardTower").child("attack").attribute("damage").as_uint() };
	playerGuardTowerInfo.attackWaitTime = { humanBuildings.child("guardTower").child("attack").attribute("attackWaitTime").as_uint() };
	playerGuardTowerInfo.arrowSpeed = { humanBuildings.child("guardTower").child("attack").attribute("arrowSpeed").as_uint() };
	playerGuardTowerInfo.maxLife = { humanBuildings.child("guardTower").child("maxLife").attribute("value").as_int() };

	//Player Guard Tower animations
	aux = humanBuildings.child("guardTower").child("sprites");
	playerGuardTowerInfo.completeTexArea = { aux.child("complete").attribute("x").as_int(), aux.child("complete").attribute("y").as_int(), aux.child("complete").attribute("w").as_int(), aux.child("complete").attribute("h").as_int() };
	playerGuardTowerInfo.inProgressTexArea = { aux.child("inProgress").attribute("x").as_int(), aux.child("inProgress").attribute("y").as_int(), aux.child("inProgress").attribute("w").as_int(), aux.child("inProgress").attribute("h").as_int() };

	//Player Cannon Tower attributes
	playerCannonTowerInfo.sightRadius = { humanBuildings.child("canonTower").child("attack").attribute("sightRadius").as_uint() };
	playerCannonTowerInfo.damage = { humanBuildings.child("canonTower").child("attack").attribute("damage").as_uint() };
	playerCannonTowerInfo.attackWaitTime = { humanBuildings.child("canonTower").child("attack").attribute("attackWaitTime").as_uint() };
	playerCannonTowerInfo.arrowSpeed = { humanBuildings.child("canonTower").child("attack").attribute("arrowSpeed").as_uint() };
	playerCannonTowerInfo.maxLife = { humanBuildings.child("canonTower").child("maxLife").attribute("value").as_int() };


	//Player Cannon Tower animations
	aux = humanBuildings.child("canonTower").child("sprites");
	playerCannonTowerInfo.completeTexArea = { aux.child("complete").attribute("x").as_int(), aux.child("complete").attribute("y").as_int(), aux.child("complete").attribute("w").as_int(), aux.child("complete").attribute("h").as_int() };
	playerCannonTowerInfo.inProgressTexArea = { aux.child("inProgress").attribute("x").as_int(), aux.child("inProgress").attribute("y").as_int(), aux.child("inProgress").attribute("w").as_int(), aux.child("inProgress").attribute("h").as_int() };

	barracksInfo.barracks1MaxLife = { humanBuildings.child("barracks").child("maxLife").attribute("value").as_int() };
	aux = humanBuildings.child("barracks").child("sprites");
	barracksInfo.inProgressTexArea = { aux.child("complete").attribute("x").as_int(), aux.child("complete").attribute("y").as_int(), aux.child("complete").attribute("w").as_int(), aux.child("complete").attribute("h").as_int() };

	barracksInfo.barracks2MaxLife = { humanBuildings.child("barracks2").child("maxLife").attribute("value").as_int() };
	aux = humanBuildings.child("barracks2").child("sprites");
	barracksInfo.completeTexArea = { aux.child("complete").attribute("x").as_int(), aux.child("complete").attribute("y").as_int(), aux.child("complete").attribute("w").as_int(), aux.child("complete").attribute("h").as_int() };

	elvenLumberMillInfo.maxLife = { humanBuildings.child("elvenLumberMill").child("maxLife").attribute("value").as_int() };
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


	//Neutral buildings
	pugi::xml_node neutralBuildings = staticEntities.child("neutralBuildings");
	aux = neutralBuildings.child("goldMine").child("sprites");
	goldMineInfo.completeTexArea = { aux.child("active").attribute("x").as_int(), aux.child("active").attribute("y").as_int(), aux.child("active").attribute("w").as_int(), aux.child("active").attribute("h").as_int() };
	goldMineInfo.inProgressTexArea = { aux.child("inactive").attribute("x").as_int(), aux.child("inactive").attribute("y").as_int(), aux.child("inactive").attribute("w").as_int(), aux.child("inactive").attribute("h").as_int() };

	aux = neutralBuildings.child("runeStone").child("sprites");
	runestoneInfo.completeTexArea = { aux.child("active").attribute("x").as_int(), aux.child("active").attribute("y").as_int(), aux.child("active").attribute("w").as_int(), aux.child("active").attribute("h").as_int() };
	runestoneInfo.inProgressTexArea = { aux.child("inactive").attribute("x").as_int(), aux.child("inactive").attribute("y").as_int(), aux.child("inactive").attribute("w").as_int(), aux.child("inactive").attribute("h").as_int() };

	//Enemy buildings
	pugi::xml_node orcishBuildings = staticEntities.child("orcishBuildings");

	greatHallInfo.life = { orcishBuildings.child("greatHall").child("maxLife").attribute("value").as_uint() };
	greatHallInfo.completeTexArea = { orcishBuildings.child("greatHall").child("sprite").attribute("x").as_int(), orcishBuildings.child("greatHall").child("sprite").attribute("y").as_int(), orcishBuildings.child("greatHall").child("sprite").attribute("w").as_int(), orcishBuildings.child("greatHall").child("sprite").attribute("h").as_int() };

	strongholdInfo.life = { orcishBuildings.child("stronghold").child("maxLife").attribute("value").as_uint() };
	strongholdInfo.completeTexArea = { orcishBuildings.child("stronghold").child("sprite").attribute("x").as_int(), orcishBuildings.child("stronghold").child("sprite").attribute("y").as_int(), orcishBuildings.child("stronghold").child("sprite").attribute("w").as_int(), orcishBuildings.child("stronghold").child("sprite").attribute("h").as_int() };

	fortressInfo.life = { orcishBuildings.child("fortress").child("maxLife").attribute("value").as_uint() };
	fortressInfo.completeTexArea = { orcishBuildings.child("fortress").child("sprite").attribute("x").as_int(), orcishBuildings.child("fortress").child("sprite").attribute("y").as_int(), orcishBuildings.child("fortress").child("sprite").attribute("w").as_int(), orcishBuildings.child("fortress").child("sprite").attribute("h").as_int() };

	enemyBarracksInfo.life = { orcishBuildings.child("barracks").child("maxLife").attribute("value").as_uint() };
	enemyBarracksInfo.completeTexArea = { orcishBuildings.child("barracks").child("sprite").attribute("x").as_int(), orcishBuildings.child("barracks").child("sprite").attribute("y").as_int(), orcishBuildings.child("barracks").child("sprite").attribute("w").as_int(), orcishBuildings.child("barracks").child("sprite").attribute("h").as_int() };

	pigFarmInfo.life = { orcishBuildings.child("pigFarm").child("maxLife").attribute("value").as_uint() };
	pigFarmInfo.completeTexArea = { orcishBuildings.child("pigFarm").child("sprite").attribute("x").as_int(), orcishBuildings.child("pigFarm").child("sprite").attribute("y").as_int(), orcishBuildings.child("pigFarm").child("sprite").attribute("w").as_int(), orcishBuildings.child("pigFarm").child("sprite").attribute("h").as_int() };

	trollLumberMillInfo.life = { orcishBuildings.child("trollLumberMill").child("maxLife").attribute("value").as_uint() };
	trollLumberMillInfo.completeTexArea = { orcishBuildings.child("trollLumberMill").child("sprite").attribute("x").as_int(), orcishBuildings.child("trollLumberMill").child("sprite").attribute("y").as_int(), orcishBuildings.child("trollLumberMill").child("sprite").attribute("w").as_int(), orcishBuildings.child("trollLumberMill").child("sprite").attribute("h").as_int() };

	altarOfStormsInfo.life = { orcishBuildings.child("altarOfStorms").child("maxLife").attribute("value").as_uint() };
	altarOfStormsInfo.completeTexArea = { orcishBuildings.child("altarOfStorms").child("sprite").attribute("x").as_int(), orcishBuildings.child("altarOfStorms").child("sprite").attribute("y").as_int(), orcishBuildings.child("altarOfStorms").child("sprite").attribute("w").as_int(), orcishBuildings.child("altarOfStorms").child("sprite").attribute("h").as_int() };

	dragonRoostInfo.life = { orcishBuildings.child("dragonRoost").child("maxLife").attribute("value").as_uint() };
	dragonRoostInfo.completeTexArea = { orcishBuildings.child("dragonRoost").child("sprite").attribute("x").as_int(), orcishBuildings.child("dragonRoost").child("sprite").attribute("y").as_int(), orcishBuildings.child("dragonRoost").child("sprite").attribute("w").as_int(), orcishBuildings.child("dragonRoost").child("sprite").attribute("h").as_int() };

	templeOfTheDamnedInfo.life = { orcishBuildings.child("templeOfTheDamned").child("maxLife").attribute("value").as_uint() };
	templeOfTheDamnedInfo.completeTexArea = { orcishBuildings.child("templeOfTheDamned").child("sprite").attribute("x").as_int(), orcishBuildings.child("templeOfTheDamned").child("sprite").attribute("y").as_int(), orcishBuildings.child("templeOfTheDamned").child("sprite").attribute("w").as_int(), orcishBuildings.child("templeOfTheDamned").child("sprite").attribute("h").as_int() };

	ogreMoundInfo.life = { orcishBuildings.child("ogreMound").child("maxLife").attribute("value").as_uint() };
	ogreMoundInfo.completeTexArea = { orcishBuildings.child("ogreMound").child("sprite").attribute("x").as_int(), orcishBuildings.child("ogreMound").child("sprite").attribute("y").as_int(), orcishBuildings.child("ogreMound").child("sprite").attribute("w").as_int(), orcishBuildings.child("ogreMound").child("sprite").attribute("h").as_int() };

	enemyBlacksmithInfo.life = { orcishBuildings.child("blacksmith").child("maxLife").attribute("value").as_uint() };
	enemyBlacksmithInfo.completeTexArea = { orcishBuildings.child("blacksmith").child("sprite").attribute("x").as_int(), orcishBuildings.child("blacksmith").child("sprite").attribute("y").as_int(), orcishBuildings.child("blacksmith").child("sprite").attribute("w").as_int(), orcishBuildings.child("blacksmith").child("sprite").attribute("h").as_int() };

	watchTowerInfo.life = { orcishBuildings.child("watchTower").child("maxLife").attribute("value").as_uint() };
	watchTowerInfo.completeTexArea = { orcishBuildings.child("watchTower").child("sprite").attribute("x").as_int(), orcishBuildings.child("watchTower").child("sprite").attribute("y").as_int(), orcishBuildings.child("watchTower").child("sprite").attribute("w").as_int(), orcishBuildings.child("watchTower").child("sprite").attribute("h").as_int() };
	watchTowerInfo.sightRadius = { orcishBuildings.child("watchTower").child("attack").attribute("sightRadius").as_uint() };
	watchTowerInfo.damage = { orcishBuildings.child("watchTower").child("attack").attribute("damage").as_uint() };
	watchTowerInfo.attackWaitTime = { orcishBuildings.child("watchTower").child("attack").attribute("attackWaitTime").as_uint() };
	watchTowerInfo.arrowSpeed = { orcishBuildings.child("watchTower").child("attack").attribute("arrowSpeed").as_uint() };

	enemyGuardTowerInfo.life = { orcishBuildings.child("guardTower").child("maxLife").attribute("value").as_uint() };
	enemyGuardTowerInfo.completeTexArea = { orcishBuildings.child("guardTower").child("sprite").attribute("x").as_int(), orcishBuildings.child("guardTower").child("sprite").attribute("y").as_int(), orcishBuildings.child("guardTower").child("sprite").attribute("w").as_int(), orcishBuildings.child("guardTower").child("sprite").attribute("h").as_int() };
	enemyGuardTowerInfo.sightRadius = { orcishBuildings.child("guardTower").child("attack").attribute("sightRadius").as_uint() };
	enemyGuardTowerInfo.damage = { orcishBuildings.child("guardTower").child("attack").attribute("damage").as_uint() };
	enemyGuardTowerInfo.attackWaitTime = { orcishBuildings.child("guardTower").child("attack").attribute("attackWaitTime").as_uint() };
	enemyGuardTowerInfo.arrowSpeed = { orcishBuildings.child("guardTower").child("attack").attribute("arrowSpeed").as_uint() };

	enemyCannonTowerInfo.life = { orcishBuildings.child("cannonTower").child("maxLife").attribute("value").as_uint() };
	enemyCannonTowerInfo.completeTexArea = { orcishBuildings.child("cannonTower").child("sprite").attribute("x").as_int(), orcishBuildings.child("cannonTower").child("sprite").attribute("y").as_int(), orcishBuildings.child("cannonTower").child("sprite").attribute("w").as_int(), orcishBuildings.child("cannonTower").child("sprite").attribute("h").as_int() };
	enemyCannonTowerInfo.sightRadius = { orcishBuildings.child("cannonTower").child("attack").attribute("sightRadius").as_uint() };
	enemyCannonTowerInfo.damage = { orcishBuildings.child("cannonTower").child("attack").attribute("damage").as_uint() };
	enemyCannonTowerInfo.attackWaitTime = { orcishBuildings.child("cannonTower").child("attack").attribute("attackWaitTime").as_uint() };
	enemyCannonTowerInfo.arrowSpeed = { orcishBuildings.child("cannonTower").child("attack").attribute("arrowSpeed").as_uint() };

	// DYNAMIC ENTITIES
	// Humans
	pugi::xml_node humanEntities = config.child("dynamicEntities").child("humans");

	// Footman animations
	pugi::xml_node footmanAnimations = humanEntities.child("footman").child("animations");
	pugi::xml_node currentAnimation;

	// Up animation Footman
	currentAnimation = footmanAnimations.child("up");
	footmanInfo.up.speed = currentAnimation.attribute("speed").as_float();
	footmanInfo.up.loop = currentAnimation.attribute("loop").as_bool();
	for (currentAnimation = currentAnimation.child("frame"); currentAnimation; currentAnimation = currentAnimation.next_sibling("frame")) {
		footmanInfo.up.PushBack({ currentAnimation.attribute("x").as_int(), currentAnimation.attribute("y").as_int(), currentAnimation.attribute("w").as_int(), currentAnimation.attribute("h").as_int() });
	}
	// Down animation Footman
	currentAnimation = footmanAnimations.child("down");
	footmanInfo.down.speed = currentAnimation.attribute("speed").as_float();
	footmanInfo.down.loop = currentAnimation.attribute("loop").as_bool();
	for (currentAnimation = currentAnimation.child("frame"); currentAnimation; currentAnimation = currentAnimation.next_sibling("frame")) {
		footmanInfo.down.PushBack({ currentAnimation.attribute("x").as_int(), currentAnimation.attribute("y").as_int(), currentAnimation.attribute("w").as_int(), currentAnimation.attribute("h").as_int() });
	}
	// Right animation Footman
	currentAnimation = footmanAnimations.child("right");
	footmanInfo.right.speed = currentAnimation.attribute("speed").as_float();
	footmanInfo.right.loop = currentAnimation.attribute("loop").as_bool();
	for (currentAnimation = currentAnimation.child("frame"); currentAnimation; currentAnimation = currentAnimation.next_sibling("frame")) {
		footmanInfo.right.PushBack({ currentAnimation.attribute("x").as_int(), currentAnimation.attribute("y").as_int(), currentAnimation.attribute("w").as_int(), currentAnimation.attribute("h").as_int() });
	}
	// Left animation Footman
	currentAnimation = footmanAnimations.child("left");
	footmanInfo.left.speed = currentAnimation.attribute("speed").as_float();
	footmanInfo.left.loop = currentAnimation.attribute("loop").as_bool();
	for (currentAnimation = currentAnimation.child("frame"); currentAnimation; currentAnimation = currentAnimation.next_sibling("frame")) {
		footmanInfo.left.PushBack({ currentAnimation.attribute("x").as_int(), currentAnimation.attribute("y").as_int(), currentAnimation.attribute("w").as_int(), currentAnimation.attribute("h").as_int() });
	}
	// Up Right animation Footman
	currentAnimation = footmanAnimations.child("upRight");
	footmanInfo.upRight.speed = currentAnimation.attribute("speed").as_float();
	footmanInfo.upRight.loop = currentAnimation.attribute("loop").as_bool();
	for (currentAnimation = currentAnimation.child("frame"); currentAnimation; currentAnimation = currentAnimation.next_sibling("frame")) {
		footmanInfo.upRight.PushBack({ currentAnimation.attribute("x").as_int(), currentAnimation.attribute("y").as_int(), currentAnimation.attribute("w").as_int(), currentAnimation.attribute("h").as_int() });
	}
	// Up Left animation Footman
	currentAnimation = footmanAnimations.child("upLeft");
	footmanInfo.upLeft.speed = currentAnimation.attribute("speed").as_float();
	footmanInfo.upLeft.loop = currentAnimation.attribute("loop").as_bool();
	for (currentAnimation = currentAnimation.child("frame"); currentAnimation; currentAnimation = currentAnimation.next_sibling("frame")) {
		footmanInfo.upLeft.PushBack({ currentAnimation.attribute("x").as_int(), currentAnimation.attribute("y").as_int(), currentAnimation.attribute("w").as_int(), currentAnimation.attribute("h").as_int() });
	}
	// Down Right animation Footman
	currentAnimation = footmanAnimations.child("downRight");
	footmanInfo.downRight.speed = currentAnimation.attribute("speed").as_float();
	footmanInfo.downRight.loop = currentAnimation.attribute("loop").as_bool();
	for (currentAnimation = currentAnimation.child("frame"); currentAnimation; currentAnimation = currentAnimation.next_sibling("frame")) {
		footmanInfo.downRight.PushBack({ currentAnimation.attribute("x").as_int(), currentAnimation.attribute("y").as_int(), currentAnimation.attribute("w").as_int(), currentAnimation.attribute("h").as_int() });
	}
	// Down Left animation Footman
	currentAnimation = footmanAnimations.child("downLeft");
	footmanInfo.downLeft.speed = currentAnimation.attribute("speed").as_float();
	footmanInfo.downLeft.loop = currentAnimation.attribute("loop").as_bool();
	for (currentAnimation = currentAnimation.child("frame"); currentAnimation; currentAnimation = currentAnimation.next_sibling("frame")) {
		footmanInfo.downLeft.PushBack({ currentAnimation.attribute("x").as_int(), currentAnimation.attribute("y").as_int(), currentAnimation.attribute("w").as_int(), currentAnimation.attribute("h").as_int() });
	}
	// Attack Up animation Footman
	currentAnimation = footmanAnimations.child("attackUp");
	footmanInfo.attackUp.speed = currentAnimation.attribute("speed").as_float();
	footmanInfo.attackUp.loop = currentAnimation.attribute("loop").as_bool();
	for (currentAnimation = currentAnimation.child("frame"); currentAnimation; currentAnimation = currentAnimation.next_sibling("frame")) {
		footmanInfo.attackUp.PushBack({ currentAnimation.attribute("x").as_int(), currentAnimation.attribute("y").as_int(), currentAnimation.attribute("w").as_int(), currentAnimation.attribute("h").as_int() });
	}
	// Attack Down animation Footman
	currentAnimation = footmanAnimations.child("attackDown");
	footmanInfo.attackDown.speed = currentAnimation.attribute("speed").as_float();
	footmanInfo.attackDown.loop = currentAnimation.attribute("loop").as_bool();
	for (currentAnimation = currentAnimation.child("frame"); currentAnimation; currentAnimation = currentAnimation.next_sibling("frame")) {
		footmanInfo.attackDown.PushBack({ currentAnimation.attribute("x").as_int(), currentAnimation.attribute("y").as_int(), currentAnimation.attribute("w").as_int(), currentAnimation.attribute("h").as_int() });
	}
	// Attack Left animation Footman
	currentAnimation = footmanAnimations.child("attackLeft");
	footmanInfo.attackLeft.speed = currentAnimation.attribute("speed").as_float();
	footmanInfo.attackLeft.loop = currentAnimation.attribute("loop").as_bool();
	for (currentAnimation = currentAnimation.child("frame"); currentAnimation; currentAnimation = currentAnimation.next_sibling("frame")) {
		footmanInfo.attackLeft.PushBack({ currentAnimation.attribute("x").as_int(), currentAnimation.attribute("y").as_int(), currentAnimation.attribute("w").as_int(), currentAnimation.attribute("h").as_int() });
	}
	// Attack Right animation Footman
	currentAnimation = footmanAnimations.child("attackRight");
	footmanInfo.attackRight.speed = currentAnimation.attribute("speed").as_float();
	footmanInfo.attackRight.loop = currentAnimation.attribute("loop").as_bool();
	for (currentAnimation = currentAnimation.child("frame"); currentAnimation; currentAnimation = currentAnimation.next_sibling("frame")) {
		footmanInfo.attackRight.PushBack({ currentAnimation.attribute("x").as_int(), currentAnimation.attribute("y").as_int(), currentAnimation.attribute("w").as_int(), currentAnimation.attribute("h").as_int() });
	}
	// Attack Up Left animation Footman
	currentAnimation = footmanAnimations.child("attackUpLeft");
	footmanInfo.attackUpLeft.speed = currentAnimation.attribute("speed").as_float();
	footmanInfo.attackUpLeft.loop = currentAnimation.attribute("loop").as_bool();
	for (currentAnimation = currentAnimation.child("frame"); currentAnimation; currentAnimation = currentAnimation.next_sibling("frame")) {
		footmanInfo.attackUpLeft.PushBack({ currentAnimation.attribute("x").as_int(), currentAnimation.attribute("y").as_int(), currentAnimation.attribute("w").as_int(), currentAnimation.attribute("h").as_int() });
	}
	// Attack Up Right animation Footman
	currentAnimation = footmanAnimations.child("attackUpRight");
	footmanInfo.attackUpRight.speed = currentAnimation.attribute("speed").as_float();
	footmanInfo.attackUpRight.loop = currentAnimation.attribute("loop").as_bool();
	for (currentAnimation = currentAnimation.child("frame"); currentAnimation; currentAnimation = currentAnimation.next_sibling("frame")) {
		footmanInfo.attackUpRight.PushBack({ currentAnimation.attribute("x").as_int(), currentAnimation.attribute("y").as_int(), currentAnimation.attribute("w").as_int(), currentAnimation.attribute("h").as_int() });
	}
	// Attack Down Left animation Footman
	currentAnimation = footmanAnimations.child("attackDownLeft");
	footmanInfo.attackDownLeft.speed = currentAnimation.attribute("speed").as_float();
	footmanInfo.attackDownLeft.loop = currentAnimation.attribute("loop").as_bool();
	for (currentAnimation = currentAnimation.child("frame"); currentAnimation; currentAnimation = currentAnimation.next_sibling("frame")) {
		footmanInfo.attackDownLeft.PushBack({ currentAnimation.attribute("x").as_int(), currentAnimation.attribute("y").as_int(), currentAnimation.attribute("w").as_int(), currentAnimation.attribute("h").as_int() });
	}
	// Attack Down Right animation Footman
	currentAnimation = footmanAnimations.child("attackDownRight");
	footmanInfo.attackDownRight.speed = currentAnimation.attribute("speed").as_float();
	footmanInfo.attackDownRight.loop = currentAnimation.attribute("loop").as_bool();
	for (currentAnimation = currentAnimation.child("frame"); currentAnimation; currentAnimation = currentAnimation.next_sibling("frame")) {
		footmanInfo.attackDownRight.PushBack({ currentAnimation.attribute("x").as_int(), currentAnimation.attribute("y").as_int(), currentAnimation.attribute("w").as_int(), currentAnimation.attribute("h").as_int() });
	}
	// Death Up animation Footman
	currentAnimation = footmanAnimations.child("deathUp");
	footmanInfo.deathUp.speed = currentAnimation.attribute("speed").as_float();
	footmanInfo.deathUp.loop = currentAnimation.attribute("loop").as_bool();
	for (currentAnimation = currentAnimation.child("frame"); currentAnimation; currentAnimation = currentAnimation.next_sibling("frame")) {
		footmanInfo.deathUp.PushBack({ currentAnimation.attribute("x").as_int(), currentAnimation.attribute("y").as_int(), currentAnimation.attribute("w").as_int(), currentAnimation.attribute("h").as_int() });
	}
	// Death Down animation Footman
	currentAnimation = footmanAnimations.child("deathDown");
	footmanInfo.deathDown.speed = currentAnimation.attribute("speed").as_float();
	footmanInfo.deathDown.loop = currentAnimation.attribute("loop").as_bool();
	for (currentAnimation = currentAnimation.child("frame"); currentAnimation; currentAnimation = currentAnimation.next_sibling("frame")) {
		footmanInfo.deathDown.PushBack({ currentAnimation.attribute("x").as_int(), currentAnimation.attribute("y").as_int(), currentAnimation.attribute("w").as_int(), currentAnimation.attribute("h").as_int() });
	}

	// Elven Archer animations
	pugi::xml_node elvenArcherAnimations = humanEntities.child("elvenArcher").child("animations");

	// Up animation Elven Archer
	currentAnimation = elvenArcherAnimations.child("up");
	elvenArcherInfo.up.speed = currentAnimation.attribute("speed").as_float();
	elvenArcherInfo.up.loop = currentAnimation.attribute("loop").as_bool();
	for (currentAnimation = currentAnimation.child("frame"); currentAnimation; currentAnimation = currentAnimation.next_sibling("frame")) {
		elvenArcherInfo.up.PushBack({ currentAnimation.attribute("x").as_int(), currentAnimation.attribute("y").as_int(), currentAnimation.attribute("w").as_int(), currentAnimation.attribute("h").as_int() });
	}
	// Down animation Elven Archer
	currentAnimation = elvenArcherAnimations.child("down");
	elvenArcherInfo.down.speed = currentAnimation.attribute("speed").as_float();
	elvenArcherInfo.down.loop = currentAnimation.attribute("loop").as_bool();
	for (currentAnimation = currentAnimation.child("frame"); currentAnimation; currentAnimation = currentAnimation.next_sibling("frame")) {
		elvenArcherInfo.down.PushBack({ currentAnimation.attribute("x").as_int(), currentAnimation.attribute("y").as_int(), currentAnimation.attribute("w").as_int(), currentAnimation.attribute("h").as_int() });
	}
	// Left animation Elven Archer
	currentAnimation = elvenArcherAnimations.child("left");
	elvenArcherInfo.left.speed = currentAnimation.attribute("speed").as_float();
	elvenArcherInfo.left.loop = currentAnimation.attribute("loop").as_bool();
	for (currentAnimation = currentAnimation.child("frame"); currentAnimation; currentAnimation = currentAnimation.next_sibling("frame")) {
		elvenArcherInfo.left.PushBack({ currentAnimation.attribute("x").as_int(), currentAnimation.attribute("y").as_int(), currentAnimation.attribute("w").as_int(), currentAnimation.attribute("h").as_int() });
	}
	// Right animation Elven Archer
	currentAnimation = elvenArcherAnimations.child("right");
	elvenArcherInfo.right.speed = currentAnimation.attribute("speed").as_float();
	elvenArcherInfo.right.loop = currentAnimation.attribute("loop").as_bool();
	for (currentAnimation = currentAnimation.child("frame"); currentAnimation; currentAnimation = currentAnimation.next_sibling("frame")) {
		elvenArcherInfo.right.PushBack({ currentAnimation.attribute("x").as_int(), currentAnimation.attribute("y").as_int(), currentAnimation.attribute("w").as_int(), currentAnimation.attribute("h").as_int() });
	}
	// Up Left animation Elven Archer
	currentAnimation = elvenArcherAnimations.child("upLeft");
	elvenArcherInfo.upLeft.speed = currentAnimation.attribute("speed").as_float();
	elvenArcherInfo.upLeft.loop = currentAnimation.attribute("loop").as_bool();
	for (currentAnimation = currentAnimation.child("frame"); currentAnimation; currentAnimation = currentAnimation.next_sibling("frame")) {
		elvenArcherInfo.upLeft.PushBack({ currentAnimation.attribute("x").as_int(), currentAnimation.attribute("y").as_int(), currentAnimation.attribute("w").as_int(), currentAnimation.attribute("h").as_int() });
	}
	// Up Right animation Elven Archer
	currentAnimation = elvenArcherAnimations.child("upRight");
	elvenArcherInfo.upRight.speed = currentAnimation.attribute("speed").as_float();
	elvenArcherInfo.upRight.loop = currentAnimation.attribute("loop").as_bool();
	for (currentAnimation = currentAnimation.child("frame"); currentAnimation; currentAnimation = currentAnimation.next_sibling("frame")) {
		elvenArcherInfo.upRight.PushBack({ currentAnimation.attribute("x").as_int(), currentAnimation.attribute("y").as_int(), currentAnimation.attribute("w").as_int(), currentAnimation.attribute("h").as_int() });
	}
	// Down Left animation Elven Archer
	currentAnimation = elvenArcherAnimations.child("downLeft");
	elvenArcherInfo.downLeft.speed = currentAnimation.attribute("speed").as_float();
	elvenArcherInfo.downLeft.loop = currentAnimation.attribute("loop").as_bool();
	for (currentAnimation = currentAnimation.child("frame"); currentAnimation; currentAnimation = currentAnimation.next_sibling("frame")) {
		elvenArcherInfo.downLeft.PushBack({ currentAnimation.attribute("x").as_int(), currentAnimation.attribute("y").as_int(), currentAnimation.attribute("w").as_int(), currentAnimation.attribute("h").as_int() });
	}
	// Down Right animation Elven Archer
	currentAnimation = elvenArcherAnimations.child("downRight");
	elvenArcherInfo.downRight.speed = currentAnimation.attribute("speed").as_float();
	elvenArcherInfo.downRight.loop = currentAnimation.attribute("loop").as_bool();
	for (currentAnimation = currentAnimation.child("frame"); currentAnimation; currentAnimation = currentAnimation.next_sibling("frame")) {
		elvenArcherInfo.downRight.PushBack({ currentAnimation.attribute("x").as_int(), currentAnimation.attribute("y").as_int(), currentAnimation.attribute("w").as_int(), currentAnimation.attribute("h").as_int() });
	}
	// Attack Up animation Elven Archer
	currentAnimation = elvenArcherAnimations.child("attackUp");
	elvenArcherInfo.attackUp.speed = currentAnimation.attribute("speed").as_float();
	elvenArcherInfo.attackUp.loop = currentAnimation.attribute("loop").as_bool();
	for (currentAnimation = currentAnimation.child("frame"); currentAnimation; currentAnimation = currentAnimation.next_sibling("frame")) {
		elvenArcherInfo.attackUp.PushBack({ currentAnimation.attribute("x").as_int(), currentAnimation.attribute("y").as_int(), currentAnimation.attribute("w").as_int(), currentAnimation.attribute("h").as_int() });
	}
	// Attack Down animation Elven Archer
	currentAnimation = elvenArcherAnimations.child("attackDown");
	elvenArcherInfo.attackDown.speed = currentAnimation.attribute("speed").as_float();
	elvenArcherInfo.attackDown.loop = currentAnimation.attribute("loop").as_bool();
	for (currentAnimation = currentAnimation.child("frame"); currentAnimation; currentAnimation = currentAnimation.next_sibling("frame")) {
		elvenArcherInfo.attackDown.PushBack({ currentAnimation.attribute("x").as_int(), currentAnimation.attribute("y").as_int(), currentAnimation.attribute("w").as_int(), currentAnimation.attribute("h").as_int() });
	}
	// Attack Left animation Elven Archer
	currentAnimation = elvenArcherAnimations.child("attackLeft");
	elvenArcherInfo.attackLeft.speed = currentAnimation.attribute("speed").as_float();
	elvenArcherInfo.attackLeft.loop = currentAnimation.attribute("loop").as_bool();
	for (currentAnimation = currentAnimation.child("frame"); currentAnimation; currentAnimation = currentAnimation.next_sibling("frame")) {
		elvenArcherInfo.attackLeft.PushBack({ currentAnimation.attribute("x").as_int(), currentAnimation.attribute("y").as_int(), currentAnimation.attribute("w").as_int(), currentAnimation.attribute("h").as_int() });
	}
	// Attack Right animation Elven Archer
	currentAnimation = elvenArcherAnimations.child("attackRight");
	elvenArcherInfo.attackRight.speed = currentAnimation.attribute("speed").as_float();
	elvenArcherInfo.attackRight.loop = currentAnimation.attribute("loop").as_bool();
	for (currentAnimation = currentAnimation.child("frame"); currentAnimation; currentAnimation = currentAnimation.next_sibling("frame")) {
		elvenArcherInfo.attackRight.PushBack({ currentAnimation.attribute("x").as_int(), currentAnimation.attribute("y").as_int(), currentAnimation.attribute("w").as_int(), currentAnimation.attribute("h").as_int() });
	}
	// Attack Up Left animation Elven Archer
	currentAnimation = elvenArcherAnimations.child("attackUpLeft");
	elvenArcherInfo.attackUpLeft.speed = currentAnimation.attribute("speed").as_float();
	elvenArcherInfo.attackUpLeft.loop = currentAnimation.attribute("loop").as_bool();
	for (currentAnimation = currentAnimation.child("frame"); currentAnimation; currentAnimation = currentAnimation.next_sibling("frame")) {
		elvenArcherInfo.attackUpLeft.PushBack({ currentAnimation.attribute("x").as_int(), currentAnimation.attribute("y").as_int(), currentAnimation.attribute("w").as_int(), currentAnimation.attribute("h").as_int() });
	}
	// Attack Up Right animation Elven Archer
	currentAnimation = elvenArcherAnimations.child("attackUpRight");
	elvenArcherInfo.attackUpRight.speed = currentAnimation.attribute("speed").as_float();
	elvenArcherInfo.attackUpRight.loop = currentAnimation.attribute("loop").as_bool();
	for (currentAnimation = currentAnimation.child("frame"); currentAnimation; currentAnimation = currentAnimation.next_sibling("frame")) {
		elvenArcherInfo.attackUpRight.PushBack({ currentAnimation.attribute("x").as_int(), currentAnimation.attribute("y").as_int(), currentAnimation.attribute("w").as_int(), currentAnimation.attribute("h").as_int() });
	}
	// Attack Down Left animation Elven Archer
	currentAnimation = elvenArcherAnimations.child("attackDownLeft");
	elvenArcherInfo.attackDownLeft.speed = currentAnimation.attribute("speed").as_float();
	elvenArcherInfo.attackDownLeft.loop = currentAnimation.attribute("loop").as_bool();
	for (currentAnimation = currentAnimation.child("frame"); currentAnimation; currentAnimation = currentAnimation.next_sibling("frame")) {
		elvenArcherInfo.attackDownLeft.PushBack({ currentAnimation.attribute("x").as_int(), currentAnimation.attribute("y").as_int(), currentAnimation.attribute("w").as_int(), currentAnimation.attribute("h").as_int() });
	}
	// Attack Down Right animation Elven Archer
	currentAnimation = elvenArcherAnimations.child("attackDownRight");
	elvenArcherInfo.attackDownRight.speed = currentAnimation.attribute("speed").as_float();
	elvenArcherInfo.attackDownRight.loop = currentAnimation.attribute("loop").as_bool();
	for (currentAnimation = currentAnimation.child("frame"); currentAnimation; currentAnimation = currentAnimation.next_sibling("frame")) {
		elvenArcherInfo.attackDownRight.PushBack({ currentAnimation.attribute("x").as_int(), currentAnimation.attribute("y").as_int(), currentAnimation.attribute("w").as_int(), currentAnimation.attribute("h").as_int() });
	}
	// Attack Death Up animation Elven Archer
	currentAnimation = elvenArcherAnimations.child("deathUp");
	elvenArcherInfo.deathUp.speed = currentAnimation.attribute("speed").as_float();
	elvenArcherInfo.deathUp.loop = currentAnimation.attribute("loop").as_bool();
	for (currentAnimation = currentAnimation.child("frame"); currentAnimation; currentAnimation = currentAnimation.next_sibling("frame")) {
		elvenArcherInfo.deathUp.PushBack({ currentAnimation.attribute("x").as_int(), currentAnimation.attribute("y").as_int(), currentAnimation.attribute("w").as_int(), currentAnimation.attribute("h").as_int() });
	}
	// Attack Death Down animation Elven Archer
	currentAnimation = elvenArcherAnimations.child("deathDown");
	elvenArcherInfo.deathDown.speed = currentAnimation.attribute("speed").as_float();
	elvenArcherInfo.deathDown.loop = currentAnimation.attribute("loop").as_bool();
	for (currentAnimation = currentAnimation.child("frame"); currentAnimation; currentAnimation = currentAnimation.next_sibling("frame")) {
		elvenArcherInfo.deathDown.PushBack({ currentAnimation.attribute("x").as_int(), currentAnimation.attribute("y").as_int(), currentAnimation.attribute("w").as_int(), currentAnimation.attribute("h").as_int() });
	}

	// Gryphon Rider animations
	pugi::xml_node gryphonRiderAnimations = humanEntities.child("gryphonRider").child("animations");

	// Up animation Gryphon Rider
	currentAnimation = gryphonRiderAnimations.child("up");
	gryphonRiderInfo.up.speed = currentAnimation.attribute("speed").as_float();
	gryphonRiderInfo.up.loop = currentAnimation.attribute("loop").as_bool();
	for (currentAnimation = currentAnimation.child("frame"); currentAnimation; currentAnimation = currentAnimation.next_sibling("frame")) {
		gryphonRiderInfo.up.PushBack({ currentAnimation.attribute("x").as_int(), currentAnimation.attribute("y").as_int(), currentAnimation.attribute("w").as_int(), currentAnimation.attribute("h").as_int() });
	}
	// Down animation Gryphon Rider
	currentAnimation = gryphonRiderAnimations.child("down");
	gryphonRiderInfo.down.speed = currentAnimation.attribute("speed").as_float();
	gryphonRiderInfo.down.loop = currentAnimation.attribute("loop").as_bool();
	for (currentAnimation = currentAnimation.child("frame"); currentAnimation; currentAnimation = currentAnimation.next_sibling("frame")) {
		gryphonRiderInfo.down.PushBack({ currentAnimation.attribute("x").as_int(), currentAnimation.attribute("y").as_int(), currentAnimation.attribute("w").as_int(), currentAnimation.attribute("h").as_int() });
	}
	// Left animation Gryphon Rider
	currentAnimation = gryphonRiderAnimations.child("left");
	gryphonRiderInfo.left.speed = currentAnimation.attribute("speed").as_float();
	gryphonRiderInfo.left.loop = currentAnimation.attribute("loop").as_bool();
	for (currentAnimation = currentAnimation.child("frame"); currentAnimation; currentAnimation = currentAnimation.next_sibling("frame")) {
		gryphonRiderInfo.left.PushBack({ currentAnimation.attribute("x").as_int(), currentAnimation.attribute("y").as_int(), currentAnimation.attribute("w").as_int(), currentAnimation.attribute("h").as_int() });
	}
	// Right animation Gryphon Rider
	currentAnimation = gryphonRiderAnimations.child("right");
	gryphonRiderInfo.right.speed = currentAnimation.attribute("speed").as_float();
	gryphonRiderInfo.right.loop = currentAnimation.attribute("loop").as_bool();
	for (currentAnimation = currentAnimation.child("frame"); currentAnimation; currentAnimation = currentAnimation.next_sibling("frame")) {
		gryphonRiderInfo.right.PushBack({ currentAnimation.attribute("x").as_int(), currentAnimation.attribute("y").as_int(), currentAnimation.attribute("w").as_int(), currentAnimation.attribute("h").as_int() });
	}
	// Up Left animation Gryphon Rider
	currentAnimation = gryphonRiderAnimations.child("upLeft");
	gryphonRiderInfo.upLeft.speed = currentAnimation.attribute("speed").as_float();
	gryphonRiderInfo.upLeft.loop = currentAnimation.attribute("loop").as_bool();
	for (currentAnimation = currentAnimation.child("frame"); currentAnimation; currentAnimation = currentAnimation.next_sibling("frame")) {
		gryphonRiderInfo.upLeft.PushBack({ currentAnimation.attribute("x").as_int(), currentAnimation.attribute("y").as_int(), currentAnimation.attribute("w").as_int(), currentAnimation.attribute("h").as_int() });
	}
	// Up Right animation Gryphon Rider
	currentAnimation = gryphonRiderAnimations.child("upRight");
	gryphonRiderInfo.upRight.speed = currentAnimation.attribute("speed").as_float();
	gryphonRiderInfo.upRight.loop = currentAnimation.attribute("loop").as_bool();
	for (currentAnimation = currentAnimation.child("frame"); currentAnimation; currentAnimation = currentAnimation.next_sibling("frame")) {
		gryphonRiderInfo.upRight.PushBack({ currentAnimation.attribute("x").as_int(), currentAnimation.attribute("y").as_int(), currentAnimation.attribute("w").as_int(), currentAnimation.attribute("h").as_int() });
	}
	// Down Left animation Gryphon Rider
	currentAnimation = gryphonRiderAnimations.child("downLeft");
	gryphonRiderInfo.downLeft.speed = currentAnimation.attribute("speed").as_float();
	gryphonRiderInfo.downLeft.loop = currentAnimation.attribute("loop").as_bool();
	for (currentAnimation = currentAnimation.child("frame"); currentAnimation; currentAnimation = currentAnimation.next_sibling("frame")) {
		gryphonRiderInfo.downLeft.PushBack({ currentAnimation.attribute("x").as_int(), currentAnimation.attribute("y").as_int(), currentAnimation.attribute("w").as_int(), currentAnimation.attribute("h").as_int() });
	}
	// Down Right animation Gryphon Rider
	currentAnimation = gryphonRiderAnimations.child("downRight");
	gryphonRiderInfo.downRight.speed = currentAnimation.attribute("speed").as_float();
	gryphonRiderInfo.downRight.loop = currentAnimation.attribute("loop").as_bool();
	for (currentAnimation = currentAnimation.child("frame"); currentAnimation; currentAnimation = currentAnimation.next_sibling("frame")) {
		gryphonRiderInfo.downRight.PushBack({ currentAnimation.attribute("x").as_int(), currentAnimation.attribute("y").as_int(), currentAnimation.attribute("w").as_int(), currentAnimation.attribute("h").as_int() });
	}
	// Attack Up animation Gryphon Rider
	currentAnimation = gryphonRiderAnimations.child("attackUp");
	gryphonRiderInfo.attackUp.speed = currentAnimation.attribute("speed").as_float();
	gryphonRiderInfo.attackUp.loop = currentAnimation.attribute("loop").as_bool();
	for (currentAnimation = currentAnimation.child("frame"); currentAnimation; currentAnimation = currentAnimation.next_sibling("frame")) {
		gryphonRiderInfo.attackUp.PushBack({ currentAnimation.attribute("x").as_int(), currentAnimation.attribute("y").as_int(), currentAnimation.attribute("w").as_int(), currentAnimation.attribute("h").as_int() });
	}
	// Attack Down animation Gryphon Rider
	currentAnimation = gryphonRiderAnimations.child("attackDown");
	gryphonRiderInfo.attackDown.speed = currentAnimation.attribute("speed").as_float();
	gryphonRiderInfo.attackDown.loop = currentAnimation.attribute("loop").as_bool();
	for (currentAnimation = currentAnimation.child("frame"); currentAnimation; currentAnimation = currentAnimation.next_sibling("frame")) {
		gryphonRiderInfo.attackDown.PushBack({ currentAnimation.attribute("x").as_int(), currentAnimation.attribute("y").as_int(), currentAnimation.attribute("w").as_int(), currentAnimation.attribute("h").as_int() });
	}
	// Attack Left animation Gryphon Rider
	currentAnimation = gryphonRiderAnimations.child("attackLeft");
	gryphonRiderInfo.attackLeft.speed = currentAnimation.attribute("speed").as_float();
	gryphonRiderInfo.attackLeft.loop = currentAnimation.attribute("loop").as_bool();
	for (currentAnimation = currentAnimation.child("frame"); currentAnimation; currentAnimation = currentAnimation.next_sibling("frame")) {
		gryphonRiderInfo.attackLeft.PushBack({ currentAnimation.attribute("x").as_int(), currentAnimation.attribute("y").as_int(), currentAnimation.attribute("w").as_int(), currentAnimation.attribute("h").as_int() });
	}
	// Attack Right animation Gryphon Rider
	currentAnimation = gryphonRiderAnimations.child("attackRight");
	gryphonRiderInfo.attackRight.speed = currentAnimation.attribute("speed").as_float();
	gryphonRiderInfo.attackRight.loop = currentAnimation.attribute("loop").as_bool();
	for (currentAnimation = currentAnimation.child("frame"); currentAnimation; currentAnimation = currentAnimation.next_sibling("frame")) {
		gryphonRiderInfo.attackRight.PushBack({ currentAnimation.attribute("x").as_int(), currentAnimation.attribute("y").as_int(), currentAnimation.attribute("w").as_int(), currentAnimation.attribute("h").as_int() });
	}
	// Attack Up Left animation Gryphon Rider
	currentAnimation = gryphonRiderAnimations.child("attackUpLeft");
	gryphonRiderInfo.attackUpLeft.speed = currentAnimation.attribute("speed").as_float();
	gryphonRiderInfo.attackUpLeft.loop = currentAnimation.attribute("loop").as_bool();
	for (currentAnimation = currentAnimation.child("frame"); currentAnimation; currentAnimation = currentAnimation.next_sibling("frame")) {
		gryphonRiderInfo.attackUpLeft.PushBack({ currentAnimation.attribute("x").as_int(), currentAnimation.attribute("y").as_int(), currentAnimation.attribute("w").as_int(), currentAnimation.attribute("h").as_int() });
	}
	// Attack Up Right animation Gryphon Rider
	currentAnimation = gryphonRiderAnimations.child("attackUpRight");
	gryphonRiderInfo.attackUpRight.speed = currentAnimation.attribute("speed").as_float();
	gryphonRiderInfo.attackUpRight.loop = currentAnimation.attribute("loop").as_bool();
	for (currentAnimation = currentAnimation.child("frame"); currentAnimation; currentAnimation = currentAnimation.next_sibling("frame")) {
		gryphonRiderInfo.attackUpRight.PushBack({ currentAnimation.attribute("x").as_int(), currentAnimation.attribute("y").as_int(), currentAnimation.attribute("w").as_int(), currentAnimation.attribute("h").as_int() });
	}
	// Attack Down Left animation Gryphon Rider
	currentAnimation = gryphonRiderAnimations.child("attackDownLeft");
	gryphonRiderInfo.attackDownLeft.speed = currentAnimation.attribute("speed").as_float();
	gryphonRiderInfo.attackDownLeft.loop = currentAnimation.attribute("loop").as_bool();
	for (currentAnimation = currentAnimation.child("frame"); currentAnimation; currentAnimation = currentAnimation.next_sibling("frame")) {
		gryphonRiderInfo.attackDownLeft.PushBack({ currentAnimation.attribute("x").as_int(), currentAnimation.attribute("y").as_int(), currentAnimation.attribute("w").as_int(), currentAnimation.attribute("h").as_int() });
	}
	// Attack Down Right animation Gryphon Rider
	currentAnimation = gryphonRiderAnimations.child("attackDownRight");
	gryphonRiderInfo.attackDownRight.speed = currentAnimation.attribute("speed").as_float();
	gryphonRiderInfo.attackDownRight.loop = currentAnimation.attribute("loop").as_bool();
	for (currentAnimation = currentAnimation.child("frame"); currentAnimation; currentAnimation = currentAnimation.next_sibling("frame")) {
		gryphonRiderInfo.attackDownRight.PushBack({ currentAnimation.attribute("x").as_int(), currentAnimation.attribute("y").as_int(), currentAnimation.attribute("w").as_int(), currentAnimation.attribute("h").as_int() });
	}
	// Attack Death Up animation Gryphon Rider
	currentAnimation = gryphonRiderAnimations.child("deathUp");
	gryphonRiderInfo.deathUp.speed = currentAnimation.attribute("speed").as_float();
	gryphonRiderInfo.deathUp.loop = currentAnimation.attribute("loop").as_bool();
	for (currentAnimation = currentAnimation.child("frame"); currentAnimation; currentAnimation = currentAnimation.next_sibling("frame")) {
		gryphonRiderInfo.deathUp.PushBack({ currentAnimation.attribute("x").as_int(), currentAnimation.attribute("y").as_int(), currentAnimation.attribute("w").as_int(), currentAnimation.attribute("h").as_int() });
	}
	// Attack Death Down animation Gryphon Rider
	currentAnimation = gryphonRiderAnimations.child("deathDown");
	gryphonRiderInfo.deathDown.speed = currentAnimation.attribute("speed").as_float();
	gryphonRiderInfo.deathDown.loop = currentAnimation.attribute("loop").as_bool();
	for (currentAnimation = currentAnimation.child("frame"); currentAnimation; currentAnimation = currentAnimation.next_sibling("frame")) {
		gryphonRiderInfo.deathDown.PushBack({ currentAnimation.attribute("x").as_int(), currentAnimation.attribute("y").as_int(), currentAnimation.attribute("w").as_int(), currentAnimation.attribute("h").as_int() });
	}

	// Orcs
	pugi::xml_node orcEntities = config.child("dynamicEntities").child("orcs");

	// Grunt animations
	pugi::xml_node gruntAnimations = orcEntities.child("grunt").child("animations");

	// Up animation Grunt
	currentAnimation = gruntAnimations.child("up");
	gruntInfo.up.speed = currentAnimation.attribute("speed").as_float();
	gruntInfo.up.loop = currentAnimation.attribute("loop").as_bool();
	for (currentAnimation = currentAnimation.child("frame"); currentAnimation; currentAnimation = currentAnimation.next_sibling("frame")) {
		gruntInfo.up.PushBack({ currentAnimation.attribute("x").as_int(), currentAnimation.attribute("y").as_int(), currentAnimation.attribute("w").as_int(), currentAnimation.attribute("h").as_int() });
	}
	// Down animation Grunt
	currentAnimation = gruntAnimations.child("down");
	gruntInfo.down.speed = currentAnimation.attribute("speed").as_float();
	gruntInfo.down.loop = currentAnimation.attribute("loop").as_bool();
	for (currentAnimation = currentAnimation.child("frame"); currentAnimation; currentAnimation = currentAnimation.next_sibling("frame")) {
		gruntInfo.down.PushBack({ currentAnimation.attribute("x").as_int(), currentAnimation.attribute("y").as_int(), currentAnimation.attribute("w").as_int(), currentAnimation.attribute("h").as_int() });
	}
	// Left animation Grunt
	currentAnimation = gruntAnimations.child("left");
	gruntInfo.left.speed = currentAnimation.attribute("speed").as_float();
	gruntInfo.left.loop = currentAnimation.attribute("loop").as_bool();
	for (currentAnimation = currentAnimation.child("frame"); currentAnimation; currentAnimation = currentAnimation.next_sibling("frame")) {
		gruntInfo.left.PushBack({ currentAnimation.attribute("x").as_int(), currentAnimation.attribute("y").as_int(), currentAnimation.attribute("w").as_int(), currentAnimation.attribute("h").as_int() });
	}
	// Right animation Grunt
	currentAnimation = gruntAnimations.child("right");
	gruntInfo.right.speed = currentAnimation.attribute("speed").as_float();
	gruntInfo.right.loop = currentAnimation.attribute("loop").as_bool();
	for (currentAnimation = currentAnimation.child("frame"); currentAnimation; currentAnimation = currentAnimation.next_sibling("frame")) {
		gruntInfo.right.PushBack({ currentAnimation.attribute("x").as_int(), currentAnimation.attribute("y").as_int(), currentAnimation.attribute("w").as_int(), currentAnimation.attribute("h").as_int() });
	}
	// Up Left animation Grunt
	currentAnimation = gruntAnimations.child("upLeft");
	gruntInfo.upLeft.speed = currentAnimation.attribute("speed").as_float();
	gruntInfo.upLeft.loop = currentAnimation.attribute("loop").as_bool();
	for (currentAnimation = currentAnimation.child("frame"); currentAnimation; currentAnimation = currentAnimation.next_sibling("frame")) {
		gruntInfo.upLeft.PushBack({ currentAnimation.attribute("x").as_int(), currentAnimation.attribute("y").as_int(), currentAnimation.attribute("w").as_int(), currentAnimation.attribute("h").as_int() });
	}
	// Up Right animation Grunt
	currentAnimation = gruntAnimations.child("upRight");
	gruntInfo.upRight.speed = currentAnimation.attribute("speed").as_float();
	gruntInfo.upRight.loop = currentAnimation.attribute("loop").as_bool();
	for (currentAnimation = currentAnimation.child("frame"); currentAnimation; currentAnimation = currentAnimation.next_sibling("frame")) {
		gruntInfo.upRight.PushBack({ currentAnimation.attribute("x").as_int(), currentAnimation.attribute("y").as_int(), currentAnimation.attribute("w").as_int(), currentAnimation.attribute("h").as_int() });
	}
	// Down Left animation Grunt
	currentAnimation = gruntAnimations.child("downLeft");
	gruntInfo.downLeft.speed = currentAnimation.attribute("speed").as_float();
	gruntInfo.downLeft.loop = currentAnimation.attribute("loop").as_bool();
	for (currentAnimation = currentAnimation.child("frame"); currentAnimation; currentAnimation = currentAnimation.next_sibling("frame")) {
		gruntInfo.downLeft.PushBack({ currentAnimation.attribute("x").as_int(), currentAnimation.attribute("y").as_int(), currentAnimation.attribute("w").as_int(), currentAnimation.attribute("h").as_int() });
	}
	// Down Right animation Grunt
	currentAnimation = gruntAnimations.child("downRight");
	gruntInfo.downRight.speed = currentAnimation.attribute("speed").as_float();
	gruntInfo.downRight.loop = currentAnimation.attribute("loop").as_bool();
	for (currentAnimation = currentAnimation.child("frame"); currentAnimation; currentAnimation = currentAnimation.next_sibling("frame")) {
		gruntInfo.downRight.PushBack({ currentAnimation.attribute("x").as_int(), currentAnimation.attribute("y").as_int(), currentAnimation.attribute("w").as_int(), currentAnimation.attribute("h").as_int() });
	}
	// Attack Up animation Grunt
	currentAnimation = gruntAnimations.child("attackUp");
	gruntInfo.attackUp.speed = currentAnimation.attribute("speed").as_float();
	gruntInfo.attackUp.loop = currentAnimation.attribute("loop").as_bool();
	for (currentAnimation = currentAnimation.child("frame"); currentAnimation; currentAnimation = currentAnimation.next_sibling("frame")) {
		gruntInfo.attackUp.PushBack({ currentAnimation.attribute("x").as_int(), currentAnimation.attribute("y").as_int(), currentAnimation.attribute("w").as_int(), currentAnimation.attribute("h").as_int() });
	}
	// Attack Down animation Grunt
	currentAnimation = gruntAnimations.child("attackDown");
	gruntInfo.attackDown.speed = currentAnimation.attribute("speed").as_float();
	gruntInfo.attackDown.loop = currentAnimation.attribute("loop").as_bool();
	for (currentAnimation = currentAnimation.child("frame"); currentAnimation; currentAnimation = currentAnimation.next_sibling("frame")) {
		gruntInfo.attackDown.PushBack({ currentAnimation.attribute("x").as_int(), currentAnimation.attribute("y").as_int(), currentAnimation.attribute("w").as_int(), currentAnimation.attribute("h").as_int() });
	}
	// Attack Left animation Grunt
	currentAnimation = gruntAnimations.child("attackLeft");
	gruntInfo.attackLeft.speed = currentAnimation.attribute("speed").as_float();
	gruntInfo.attackLeft.loop = currentAnimation.attribute("loop").as_bool();
	for (currentAnimation = currentAnimation.child("frame"); currentAnimation; currentAnimation = currentAnimation.next_sibling("frame")) {
		gruntInfo.attackLeft.PushBack({ currentAnimation.attribute("x").as_int(), currentAnimation.attribute("y").as_int(), currentAnimation.attribute("w").as_int(), currentAnimation.attribute("h").as_int() });
	}
	// Attack Right animation Grunt
	currentAnimation = gruntAnimations.child("attackRight");
	gruntInfo.attackRight.speed = currentAnimation.attribute("speed").as_float();
	gruntInfo.attackRight.loop = currentAnimation.attribute("loop").as_bool();
	for (currentAnimation = currentAnimation.child("frame"); currentAnimation; currentAnimation = currentAnimation.next_sibling("frame")) {
		gruntInfo.attackRight.PushBack({ currentAnimation.attribute("x").as_int(), currentAnimation.attribute("y").as_int(), currentAnimation.attribute("w").as_int(), currentAnimation.attribute("h").as_int() });
	}
	// Attack Up Left animation Grunt
	currentAnimation = gruntAnimations.child("attackUpLeft");
	gruntInfo.attackUpLeft.speed = currentAnimation.attribute("speed").as_float();
	gruntInfo.attackUpLeft.loop = currentAnimation.attribute("loop").as_bool();
	for (currentAnimation = currentAnimation.child("frame"); currentAnimation; currentAnimation = currentAnimation.next_sibling("frame")) {
		gruntInfo.attackUpLeft.PushBack({ currentAnimation.attribute("x").as_int(), currentAnimation.attribute("y").as_int(), currentAnimation.attribute("w").as_int(), currentAnimation.attribute("h").as_int() });
	}
	// Attack Up Right animation Grunt
	currentAnimation = gruntAnimations.child("attackUpRight");
	gruntInfo.attackUpRight.speed = currentAnimation.attribute("speed").as_float();
	gruntInfo.attackUpRight.loop = currentAnimation.attribute("loop").as_bool();
	for (currentAnimation = currentAnimation.child("frame"); currentAnimation; currentAnimation = currentAnimation.next_sibling("frame")) {
		gruntInfo.attackUpRight.PushBack({ currentAnimation.attribute("x").as_int(), currentAnimation.attribute("y").as_int(), currentAnimation.attribute("w").as_int(), currentAnimation.attribute("h").as_int() });
	}
	// Attack Down Left animation Grunt
	currentAnimation = gruntAnimations.child("attackDownLeft");
	gruntInfo.attackDownLeft.speed = currentAnimation.attribute("speed").as_float();
	gruntInfo.attackDownLeft.loop = currentAnimation.attribute("loop").as_bool();
	for (currentAnimation = currentAnimation.child("frame"); currentAnimation; currentAnimation = currentAnimation.next_sibling("frame")) {
		gruntInfo.attackDownLeft.PushBack({ currentAnimation.attribute("x").as_int(), currentAnimation.attribute("y").as_int(), currentAnimation.attribute("w").as_int(), currentAnimation.attribute("h").as_int() });
	}
	// Attack Down Right animation Grunt
	currentAnimation = gruntAnimations.child("attackDownRight");
	gruntInfo.attackDownRight.speed = currentAnimation.attribute("speed").as_float();
	gruntInfo.attackDownRight.loop = currentAnimation.attribute("loop").as_bool();
	for (currentAnimation = currentAnimation.child("frame"); currentAnimation; currentAnimation = currentAnimation.next_sibling("frame")) {
		gruntInfo.attackDownRight.PushBack({ currentAnimation.attribute("x").as_int(), currentAnimation.attribute("y").as_int(), currentAnimation.attribute("w").as_int(), currentAnimation.attribute("h").as_int() });
	}
	// Attack Death Up animation Grunt
	currentAnimation = gruntAnimations.child("deathUp");
	gruntInfo.deathUp.speed = currentAnimation.attribute("speed").as_float();
	gruntInfo.deathUp.loop = currentAnimation.attribute("loop").as_bool();
	for (currentAnimation = currentAnimation.child("frame"); currentAnimation; currentAnimation = currentAnimation.next_sibling("frame")) {
		gruntInfo.deathUp.PushBack({ currentAnimation.attribute("x").as_int(), currentAnimation.attribute("y").as_int(), currentAnimation.attribute("w").as_int(), currentAnimation.attribute("h").as_int() });
	}
	// Attack Death Down animation Grunt
	currentAnimation = gruntAnimations.child("deathDown");
	gruntInfo.deathDown.speed = currentAnimation.attribute("speed").as_float();
	gruntInfo.deathDown.loop = currentAnimation.attribute("loop").as_bool();
	for (currentAnimation = currentAnimation.child("frame"); currentAnimation; currentAnimation = currentAnimation.next_sibling("frame")) {
		gruntInfo.deathDown.PushBack({ currentAnimation.attribute("x").as_int(), currentAnimation.attribute("y").as_int(), currentAnimation.attribute("w").as_int(), currentAnimation.attribute("h").as_int() });
	}

	// Troll Axethrower animations
	pugi::xml_node trollAxethrowerAnimations = orcEntities.child("trollAxethrower").child("animations");

	// Up animation Troll Axethrower
	currentAnimation = trollAxethrowerAnimations.child("up");
	trollAxethrowerInfo.up.speed = currentAnimation.attribute("speed").as_float();
	trollAxethrowerInfo.up.loop = currentAnimation.attribute("loop").as_bool();
	for (currentAnimation = currentAnimation.child("frame"); currentAnimation; currentAnimation = currentAnimation.next_sibling("frame")) {
		trollAxethrowerInfo.up.PushBack({ currentAnimation.attribute("x").as_int(), currentAnimation.attribute("y").as_int(), currentAnimation.attribute("w").as_int(), currentAnimation.attribute("h").as_int() });
	}
	// Down animation Troll Axethrower
	currentAnimation = trollAxethrowerAnimations.child("down");
	trollAxethrowerInfo.down.speed = currentAnimation.attribute("speed").as_float();
	trollAxethrowerInfo.down.loop = currentAnimation.attribute("loop").as_bool();
	for (currentAnimation = currentAnimation.child("frame"); currentAnimation; currentAnimation = currentAnimation.next_sibling("frame")) {
		trollAxethrowerInfo.down.PushBack({ currentAnimation.attribute("x").as_int(), currentAnimation.attribute("y").as_int(), currentAnimation.attribute("w").as_int(), currentAnimation.attribute("h").as_int() });
	}
	// Left animation Troll Axethrower
	currentAnimation = trollAxethrowerAnimations.child("left");
	trollAxethrowerInfo.left.speed = currentAnimation.attribute("speed").as_float();
	trollAxethrowerInfo.left.loop = currentAnimation.attribute("loop").as_bool();
	for (currentAnimation = currentAnimation.child("frame"); currentAnimation; currentAnimation = currentAnimation.next_sibling("frame")) {
		trollAxethrowerInfo.left.PushBack({ currentAnimation.attribute("x").as_int(), currentAnimation.attribute("y").as_int(), currentAnimation.attribute("w").as_int(), currentAnimation.attribute("h").as_int() });
	}
	// Right animation Troll Axethrower
	currentAnimation = trollAxethrowerAnimations.child("right");
	trollAxethrowerInfo.right.speed = currentAnimation.attribute("speed").as_float();
	trollAxethrowerInfo.right.loop = currentAnimation.attribute("loop").as_bool();
	for (currentAnimation = currentAnimation.child("frame"); currentAnimation; currentAnimation = currentAnimation.next_sibling("frame")) {
		trollAxethrowerInfo.right.PushBack({ currentAnimation.attribute("x").as_int(), currentAnimation.attribute("y").as_int(), currentAnimation.attribute("w").as_int(), currentAnimation.attribute("h").as_int() });
	}
	// Up Left animation Troll Axethrower
	currentAnimation = trollAxethrowerAnimations.child("upLeft");
	trollAxethrowerInfo.upLeft.speed = currentAnimation.attribute("speed").as_float();
	trollAxethrowerInfo.upLeft.loop = currentAnimation.attribute("loop").as_bool();
	for (currentAnimation = currentAnimation.child("frame"); currentAnimation; currentAnimation = currentAnimation.next_sibling("frame")) {
		trollAxethrowerInfo.upLeft.PushBack({ currentAnimation.attribute("x").as_int(), currentAnimation.attribute("y").as_int(), currentAnimation.attribute("w").as_int(), currentAnimation.attribute("h").as_int() });
	}
	// Up Right animation Troll Axethrower
	currentAnimation = trollAxethrowerAnimations.child("upRight");
	trollAxethrowerInfo.upRight.speed = currentAnimation.attribute("speed").as_float();
	trollAxethrowerInfo.upRight.loop = currentAnimation.attribute("loop").as_bool();
	for (currentAnimation = currentAnimation.child("frame"); currentAnimation; currentAnimation = currentAnimation.next_sibling("frame")) {
		trollAxethrowerInfo.upRight.PushBack({ currentAnimation.attribute("x").as_int(), currentAnimation.attribute("y").as_int(), currentAnimation.attribute("w").as_int(), currentAnimation.attribute("h").as_int() });
	}
	// Down Left animation Troll Axethrower
	currentAnimation = trollAxethrowerAnimations.child("downLeft");
	trollAxethrowerInfo.downLeft.speed = currentAnimation.attribute("speed").as_float();
	trollAxethrowerInfo.downLeft.loop = currentAnimation.attribute("loop").as_bool();
	for (currentAnimation = currentAnimation.child("frame"); currentAnimation; currentAnimation = currentAnimation.next_sibling("frame")) {
		trollAxethrowerInfo.downLeft.PushBack({ currentAnimation.attribute("x").as_int(), currentAnimation.attribute("y").as_int(), currentAnimation.attribute("w").as_int(), currentAnimation.attribute("h").as_int() });
	}
	// Down Right animation Troll Axethrower
	currentAnimation = trollAxethrowerAnimations.child("downRight");
	trollAxethrowerInfo.downRight.speed = currentAnimation.attribute("speed").as_float();
	trollAxethrowerInfo.downRight.loop = currentAnimation.attribute("loop").as_bool();
	for (currentAnimation = currentAnimation.child("frame"); currentAnimation; currentAnimation = currentAnimation.next_sibling("frame")) {
		trollAxethrowerInfo.downRight.PushBack({ currentAnimation.attribute("x").as_int(), currentAnimation.attribute("y").as_int(), currentAnimation.attribute("w").as_int(), currentAnimation.attribute("h").as_int() });
	}
	// Attack Up animation Troll Axethrower
	currentAnimation = trollAxethrowerAnimations.child("attackUp");
	trollAxethrowerInfo.attackUp.speed = currentAnimation.attribute("speed").as_float();
	trollAxethrowerInfo.attackUp.loop = currentAnimation.attribute("loop").as_bool();
	for (currentAnimation = currentAnimation.child("frame"); currentAnimation; currentAnimation = currentAnimation.next_sibling("frame")) {
		trollAxethrowerInfo.attackUp.PushBack({ currentAnimation.attribute("x").as_int(), currentAnimation.attribute("y").as_int(), currentAnimation.attribute("w").as_int(), currentAnimation.attribute("h").as_int() });
	}
	// Attack Down animation Troll Axethrower
	currentAnimation = trollAxethrowerAnimations.child("attackDown");
	trollAxethrowerInfo.attackDown.speed = currentAnimation.attribute("speed").as_float();
	trollAxethrowerInfo.attackDown.loop = currentAnimation.attribute("loop").as_bool();
	for (currentAnimation = currentAnimation.child("frame"); currentAnimation; currentAnimation = currentAnimation.next_sibling("frame")) {
		trollAxethrowerInfo.attackDown.PushBack({ currentAnimation.attribute("x").as_int(), currentAnimation.attribute("y").as_int(), currentAnimation.attribute("w").as_int(), currentAnimation.attribute("h").as_int() });
	}
	// Attack Left animation Troll Axethrower
	currentAnimation = trollAxethrowerAnimations.child("attackLeft");
	trollAxethrowerInfo.attackLeft.speed = currentAnimation.attribute("speed").as_float();
	trollAxethrowerInfo.attackLeft.loop = currentAnimation.attribute("loop").as_bool();
	for (currentAnimation = currentAnimation.child("frame"); currentAnimation; currentAnimation = currentAnimation.next_sibling("frame")) {
		trollAxethrowerInfo.attackLeft.PushBack({ currentAnimation.attribute("x").as_int(), currentAnimation.attribute("y").as_int(), currentAnimation.attribute("w").as_int(), currentAnimation.attribute("h").as_int() });
	}
	// Attack Right animation Troll Axethrower
	currentAnimation = trollAxethrowerAnimations.child("attackRight");
	trollAxethrowerInfo.attackRight.speed = currentAnimation.attribute("speed").as_float();
	trollAxethrowerInfo.attackRight.loop = currentAnimation.attribute("loop").as_bool();
	for (currentAnimation = currentAnimation.child("frame"); currentAnimation; currentAnimation = currentAnimation.next_sibling("frame")) {
		trollAxethrowerInfo.attackRight.PushBack({ currentAnimation.attribute("x").as_int(), currentAnimation.attribute("y").as_int(), currentAnimation.attribute("w").as_int(), currentAnimation.attribute("h").as_int() });
	}
	// Attack Up Left animation Troll Axethrower
	currentAnimation = trollAxethrowerAnimations.child("attackUpLeft");
	trollAxethrowerInfo.attackUpLeft.speed = currentAnimation.attribute("speed").as_float();
	trollAxethrowerInfo.attackUpLeft.loop = currentAnimation.attribute("loop").as_bool();
	for (currentAnimation = currentAnimation.child("frame"); currentAnimation; currentAnimation = currentAnimation.next_sibling("frame")) {
		trollAxethrowerInfo.attackUpLeft.PushBack({ currentAnimation.attribute("x").as_int(), currentAnimation.attribute("y").as_int(), currentAnimation.attribute("w").as_int(), currentAnimation.attribute("h").as_int() });
	}
	// Attack Up Right animation Troll Axethrower
	currentAnimation = trollAxethrowerAnimations.child("attackUpRight");
	trollAxethrowerInfo.attackUpRight.speed = currentAnimation.attribute("speed").as_float();
	trollAxethrowerInfo.attackUpRight.loop = currentAnimation.attribute("loop").as_bool();
	for (currentAnimation = currentAnimation.child("frame"); currentAnimation; currentAnimation = currentAnimation.next_sibling("frame")) {
		trollAxethrowerInfo.attackUpRight.PushBack({ currentAnimation.attribute("x").as_int(), currentAnimation.attribute("y").as_int(), currentAnimation.attribute("w").as_int(), currentAnimation.attribute("h").as_int() });
	}
	// Attack Down Left animation Troll Axethrower
	currentAnimation = trollAxethrowerAnimations.child("attackDownLeft");
	trollAxethrowerInfo.attackDownLeft.speed = currentAnimation.attribute("speed").as_float();
	trollAxethrowerInfo.attackDownLeft.loop = currentAnimation.attribute("loop").as_bool();
	for (currentAnimation = currentAnimation.child("frame"); currentAnimation; currentAnimation = currentAnimation.next_sibling("frame")) {
		trollAxethrowerInfo.attackDownLeft.PushBack({ currentAnimation.attribute("x").as_int(), currentAnimation.attribute("y").as_int(), currentAnimation.attribute("w").as_int(), currentAnimation.attribute("h").as_int() });
	}
	// Attack Down Right animation Troll Axethrower
	currentAnimation = trollAxethrowerAnimations.child("attackDownRight");
	trollAxethrowerInfo.attackDownRight.speed = currentAnimation.attribute("speed").as_float();
	trollAxethrowerInfo.attackDownRight.loop = currentAnimation.attribute("loop").as_bool();
	for (currentAnimation = currentAnimation.child("frame"); currentAnimation; currentAnimation = currentAnimation.next_sibling("frame")) {
		trollAxethrowerInfo.attackDownRight.PushBack({ currentAnimation.attribute("x").as_int(), currentAnimation.attribute("y").as_int(), currentAnimation.attribute("w").as_int(), currentAnimation.attribute("h").as_int() });
	}
	// Attack Death Up animation Troll Axethrower
	currentAnimation = trollAxethrowerAnimations.child("deathUp");
	trollAxethrowerInfo.deathUp.speed = currentAnimation.attribute("speed").as_float();
	trollAxethrowerInfo.deathUp.loop = currentAnimation.attribute("loop").as_bool();
	for (currentAnimation = currentAnimation.child("frame"); currentAnimation; currentAnimation = currentAnimation.next_sibling("frame")) {
		trollAxethrowerInfo.deathUp.PushBack({ currentAnimation.attribute("x").as_int(), currentAnimation.attribute("y").as_int(), currentAnimation.attribute("w").as_int(), currentAnimation.attribute("h").as_int() });
	}
	// Attack Death Down animation Troll Axethrower
	currentAnimation = trollAxethrowerAnimations.child("deathDown");
	trollAxethrowerInfo.deathDown.speed = currentAnimation.attribute("speed").as_float();
	trollAxethrowerInfo.deathDown.loop = currentAnimation.attribute("loop").as_bool();
	for (currentAnimation = currentAnimation.child("frame"); currentAnimation; currentAnimation = currentAnimation.next_sibling("frame")) {
		trollAxethrowerInfo.deathDown.PushBack({ currentAnimation.attribute("x").as_int(), currentAnimation.attribute("y").as_int(), currentAnimation.attribute("w").as_int(), currentAnimation.attribute("h").as_int() });
	}

	// Dragon animations
	pugi::xml_node dragonAnimations = orcEntities.child("dragon").child("animations");

	// Up animation Dragon
	currentAnimation = dragonAnimations.child("up");
	dragonInfo.up.speed = currentAnimation.attribute("speed").as_float();
	dragonInfo.up.loop = currentAnimation.attribute("loop").as_bool();
	for (currentAnimation = currentAnimation.child("frame"); currentAnimation; currentAnimation = currentAnimation.next_sibling("frame")) {
		dragonInfo.up.PushBack({ currentAnimation.attribute("x").as_int(), currentAnimation.attribute("y").as_int(), currentAnimation.attribute("w").as_int(), currentAnimation.attribute("h").as_int() });
	}
	// Down animation Dragon
	currentAnimation = dragonAnimations.child("down");
	dragonInfo.down.speed = currentAnimation.attribute("speed").as_float();
	dragonInfo.down.loop = currentAnimation.attribute("loop").as_bool();
	for (currentAnimation = currentAnimation.child("frame"); currentAnimation; currentAnimation = currentAnimation.next_sibling("frame")) {
		dragonInfo.down.PushBack({ currentAnimation.attribute("x").as_int(), currentAnimation.attribute("y").as_int(), currentAnimation.attribute("w").as_int(), currentAnimation.attribute("h").as_int() });
	}
	// Left animation Dragon
	currentAnimation = dragonAnimations.child("left");
	dragonInfo.left.speed = currentAnimation.attribute("speed").as_float();
	dragonInfo.left.loop = currentAnimation.attribute("loop").as_bool();
	for (currentAnimation = currentAnimation.child("frame"); currentAnimation; currentAnimation = currentAnimation.next_sibling("frame")) {
		dragonInfo.left.PushBack({ currentAnimation.attribute("x").as_int(), currentAnimation.attribute("y").as_int(), currentAnimation.attribute("w").as_int(), currentAnimation.attribute("h").as_int() });
	}
	// Right animation Dragon
	currentAnimation = dragonAnimations.child("right");
	dragonInfo.right.speed = currentAnimation.attribute("speed").as_float();
	dragonInfo.right.loop = currentAnimation.attribute("loop").as_bool();
	for (currentAnimation = currentAnimation.child("frame"); currentAnimation; currentAnimation = currentAnimation.next_sibling("frame")) {
		dragonInfo.right.PushBack({ currentAnimation.attribute("x").as_int(), currentAnimation.attribute("y").as_int(), currentAnimation.attribute("w").as_int(), currentAnimation.attribute("h").as_int() });
	}
	// Up Left animation Dragon
	currentAnimation = dragonAnimations.child("upLeft");
	dragonInfo.upLeft.speed = currentAnimation.attribute("speed").as_float();
	dragonInfo.upLeft.loop = currentAnimation.attribute("loop").as_bool();
	for (currentAnimation = currentAnimation.child("frame"); currentAnimation; currentAnimation = currentAnimation.next_sibling("frame")) {
		dragonInfo.upLeft.PushBack({ currentAnimation.attribute("x").as_int(), currentAnimation.attribute("y").as_int(), currentAnimation.attribute("w").as_int(), currentAnimation.attribute("h").as_int() });
	}
	// Up Right animation Dragon
	currentAnimation = dragonAnimations.child("upRight");
	dragonInfo.upRight.speed = currentAnimation.attribute("speed").as_float();
	dragonInfo.upRight.loop = currentAnimation.attribute("loop").as_bool();
	for (currentAnimation = currentAnimation.child("frame"); currentAnimation; currentAnimation = currentAnimation.next_sibling("frame")) {
		dragonInfo.upRight.PushBack({ currentAnimation.attribute("x").as_int(), currentAnimation.attribute("y").as_int(), currentAnimation.attribute("w").as_int(), currentAnimation.attribute("h").as_int() });
	}
	// Down Left animation Dragon
	currentAnimation = dragonAnimations.child("downLeft");
	dragonInfo.downLeft.speed = currentAnimation.attribute("speed").as_float();
	dragonInfo.downLeft.loop = currentAnimation.attribute("loop").as_bool();
	for (currentAnimation = currentAnimation.child("frame"); currentAnimation; currentAnimation = currentAnimation.next_sibling("frame")) {
		dragonInfo.downLeft.PushBack({ currentAnimation.attribute("x").as_int(), currentAnimation.attribute("y").as_int(), currentAnimation.attribute("w").as_int(), currentAnimation.attribute("h").as_int() });
	}
	// Down Right animation Dragon
	currentAnimation = dragonAnimations.child("downRight");
	dragonInfo.downRight.speed = currentAnimation.attribute("speed").as_float();
	dragonInfo.downRight.loop = currentAnimation.attribute("loop").as_bool();
	for (currentAnimation = currentAnimation.child("frame"); currentAnimation; currentAnimation = currentAnimation.next_sibling("frame")) {
		dragonInfo.downRight.PushBack({ currentAnimation.attribute("x").as_int(), currentAnimation.attribute("y").as_int(), currentAnimation.attribute("w").as_int(), currentAnimation.attribute("h").as_int() });
	}
	// Attack Up animation Dragon
	currentAnimation = dragonAnimations.child("attackUp");
	dragonInfo.attackUp.speed = currentAnimation.attribute("speed").as_float();
	dragonInfo.attackUp.loop = currentAnimation.attribute("loop").as_bool();
	for (currentAnimation = currentAnimation.child("frame"); currentAnimation; currentAnimation = currentAnimation.next_sibling("frame")) {
		dragonInfo.attackUp.PushBack({ currentAnimation.attribute("x").as_int(), currentAnimation.attribute("y").as_int(), currentAnimation.attribute("w").as_int(), currentAnimation.attribute("h").as_int() });
	}
	// Attack Down animation Dragon
	currentAnimation = dragonAnimations.child("attackDown");
	dragonInfo.attackDown.speed = currentAnimation.attribute("speed").as_float();
	dragonInfo.attackDown.loop = currentAnimation.attribute("loop").as_bool();
	for (currentAnimation = currentAnimation.child("frame"); currentAnimation; currentAnimation = currentAnimation.next_sibling("frame")) {
		dragonInfo.attackDown.PushBack({ currentAnimation.attribute("x").as_int(), currentAnimation.attribute("y").as_int(), currentAnimation.attribute("w").as_int(), currentAnimation.attribute("h").as_int() });
	}
	// Attack Left animation Dragon
	currentAnimation = dragonAnimations.child("attackLeft");
	dragonInfo.attackLeft.speed = currentAnimation.attribute("speed").as_float();
	dragonInfo.attackLeft.loop = currentAnimation.attribute("loop").as_bool();
	for (currentAnimation = currentAnimation.child("frame"); currentAnimation; currentAnimation = currentAnimation.next_sibling("frame")) {
		dragonInfo.attackLeft.PushBack({ currentAnimation.attribute("x").as_int(), currentAnimation.attribute("y").as_int(), currentAnimation.attribute("w").as_int(), currentAnimation.attribute("h").as_int() });
	}
	// Attack Right animation Dragon
	currentAnimation = dragonAnimations.child("attackRight");
	dragonInfo.attackRight.speed = currentAnimation.attribute("speed").as_float();
	dragonInfo.attackRight.loop = currentAnimation.attribute("loop").as_bool();
	for (currentAnimation = currentAnimation.child("frame"); currentAnimation; currentAnimation = currentAnimation.next_sibling("frame")) {
		dragonInfo.attackRight.PushBack({ currentAnimation.attribute("x").as_int(), currentAnimation.attribute("y").as_int(), currentAnimation.attribute("w").as_int(), currentAnimation.attribute("h").as_int() });
	}
	// Attack Up Left animation Dragon
	currentAnimation = dragonAnimations.child("attackUpLeft");
	dragonInfo.attackUpLeft.speed = currentAnimation.attribute("speed").as_float();
	dragonInfo.attackUpLeft.loop = currentAnimation.attribute("loop").as_bool();
	for (currentAnimation = currentAnimation.child("frame"); currentAnimation; currentAnimation = currentAnimation.next_sibling("frame")) {
		dragonInfo.attackUpLeft.PushBack({ currentAnimation.attribute("x").as_int(), currentAnimation.attribute("y").as_int(), currentAnimation.attribute("w").as_int(), currentAnimation.attribute("h").as_int() });
	}
	// Attack Up Right animation Dragon
	currentAnimation = dragonAnimations.child("attackUpRight");
	dragonInfo.attackUpRight.speed = currentAnimation.attribute("speed").as_float();
	dragonInfo.attackUpRight.loop = currentAnimation.attribute("loop").as_bool();
	for (currentAnimation = currentAnimation.child("frame"); currentAnimation; currentAnimation = currentAnimation.next_sibling("frame")) {
		dragonInfo.attackUpRight.PushBack({ currentAnimation.attribute("x").as_int(), currentAnimation.attribute("y").as_int(), currentAnimation.attribute("w").as_int(), currentAnimation.attribute("h").as_int() });
	}
	// Attack Down Left animation Dragon
	currentAnimation = dragonAnimations.child("attackDownLeft");
	dragonInfo.attackDownLeft.speed = currentAnimation.attribute("speed").as_float();
	dragonInfo.attackDownLeft.loop = currentAnimation.attribute("loop").as_bool();
	for (currentAnimation = currentAnimation.child("frame"); currentAnimation; currentAnimation = currentAnimation.next_sibling("frame")) {
		dragonInfo.attackDownLeft.PushBack({ currentAnimation.attribute("x").as_int(), currentAnimation.attribute("y").as_int(), currentAnimation.attribute("w").as_int(), currentAnimation.attribute("h").as_int() });
	}
	// Attack Down Right animation Dragon
	currentAnimation = dragonAnimations.child("attackDownRight");
	dragonInfo.attackDownRight.speed = currentAnimation.attribute("speed").as_float();
	dragonInfo.attackDownRight.loop = currentAnimation.attribute("loop").as_bool();
	for (currentAnimation = currentAnimation.child("frame"); currentAnimation; currentAnimation = currentAnimation.next_sibling("frame")) {
		dragonInfo.attackDownRight.PushBack({ currentAnimation.attribute("x").as_int(), currentAnimation.attribute("y").as_int(), currentAnimation.attribute("w").as_int(), currentAnimation.attribute("h").as_int() });
	}
	// Attack Death Up animation Dragon
	currentAnimation = dragonAnimations.child("deathUp");
	dragonInfo.deathUp.speed = currentAnimation.attribute("speed").as_float();
	dragonInfo.deathUp.loop = currentAnimation.attribute("loop").as_bool();
	for (currentAnimation = currentAnimation.child("frame"); currentAnimation; currentAnimation = currentAnimation.next_sibling("frame")) {
		dragonInfo.deathUp.PushBack({ currentAnimation.attribute("x").as_int(), currentAnimation.attribute("y").as_int(), currentAnimation.attribute("w").as_int(), currentAnimation.attribute("h").as_int() });
	}
	// Attack Death Down animation Dragon
	currentAnimation = dragonAnimations.child("deathDown");
	dragonInfo.deathDown.speed = currentAnimation.attribute("speed").as_float();
	dragonInfo.deathDown.loop = currentAnimation.attribute("loop").as_bool();
	for (currentAnimation = currentAnimation.child("frame"); currentAnimation; currentAnimation = currentAnimation.next_sibling("frame")) {
		dragonInfo.deathDown.PushBack({ currentAnimation.attribute("x").as_int(), currentAnimation.attribute("y").as_int(), currentAnimation.attribute("w").as_int(), currentAnimation.attribute("h").as_int() });
	}

	// Prisoners
	pugi::xml_node prisionerEntities = config.child("dynamicEntities").child("prisoners");

	// Alleria
	pugi::xml_node alleriaAnimations = prisionerEntities.child("alleria").child("animations");
	currentAnimation = alleriaAnimations.child("idle");
	alleriaInfo.idle.speed = currentAnimation.attribute("speed").as_float();
	alleriaInfo.idle.loop = currentAnimation.attribute("loop").as_bool();
	for (currentAnimation = currentAnimation.child("frame"); currentAnimation; currentAnimation = currentAnimation.next_sibling("frame")) {
		alleriaInfo.idle.PushBack({ currentAnimation.attribute("x").as_int(), currentAnimation.attribute("y").as_int(), currentAnimation.attribute("w").as_int(), currentAnimation.attribute("h").as_int() });
	}

	//Turalyon
	pugi::xml_node turalyonAnimations = prisionerEntities.child("turalyon").child("animations");
	currentAnimation = turalyonAnimations.child("idle");
	turalyonInfo.idle.speed = currentAnimation.attribute("speed").as_float();
	turalyonInfo.idle.loop = currentAnimation.attribute("loop").as_bool();
	for (currentAnimation = currentAnimation.child("frame"); currentAnimation; currentAnimation = currentAnimation.next_sibling("frame")) {
		turalyonInfo.idle.PushBack({ currentAnimation.attribute("x").as_int(), currentAnimation.attribute("y").as_int(), currentAnimation.attribute("w").as_int(), currentAnimation.attribute("h").as_int() });
	}

	// Critters
	// Sheep
	pugi::xml_node sheepAnimations = config.child("dynamicEntities").child("critters").child("sheep").child("animations");

	// up
	currentAnimation = sheepAnimations.child("up");
	critterSheepInfo.up.speed = currentAnimation.attribute("speed").as_float();
	critterSheepInfo.up.loop = currentAnimation.attribute("loop").as_bool();
	for (currentAnimation = currentAnimation.child("frame"); currentAnimation; currentAnimation = currentAnimation.next_sibling("frame")) {
		critterSheepInfo.up.PushBack({ currentAnimation.attribute("x").as_int(), currentAnimation.attribute("y").as_int(), currentAnimation.attribute("w").as_int(), currentAnimation.attribute("h").as_int() });
	}
	// down
	currentAnimation = sheepAnimations.child("down");
	critterSheepInfo.down.speed = currentAnimation.attribute("speed").as_float();
	critterSheepInfo.down.loop = currentAnimation.attribute("loop").as_bool();
	for (currentAnimation = currentAnimation.child("frame"); currentAnimation; currentAnimation = currentAnimation.next_sibling("frame")) {
		critterSheepInfo.down.PushBack({ currentAnimation.attribute("x").as_int(), currentAnimation.attribute("y").as_int(), currentAnimation.attribute("w").as_int(), currentAnimation.attribute("h").as_int() });
	}
	// left
	currentAnimation = sheepAnimations.child("left");
	critterSheepInfo.left.speed = currentAnimation.attribute("speed").as_float();
	critterSheepInfo.left.loop = currentAnimation.attribute("loop").as_bool();
	for (currentAnimation = currentAnimation.child("frame"); currentAnimation; currentAnimation = currentAnimation.next_sibling("frame")) {
		critterSheepInfo.left.PushBack({ currentAnimation.attribute("x").as_int(), currentAnimation.attribute("y").as_int(), currentAnimation.attribute("w").as_int(), currentAnimation.attribute("h").as_int() });
	}
	// right
	currentAnimation = sheepAnimations.child("right");
	critterSheepInfo.right.speed = currentAnimation.attribute("speed").as_float();
	critterSheepInfo.right.loop = currentAnimation.attribute("loop").as_bool();
	for (currentAnimation = currentAnimation.child("frame"); currentAnimation; currentAnimation = currentAnimation.next_sibling("frame")) {
		critterSheepInfo.right.PushBack({ currentAnimation.attribute("x").as_int(), currentAnimation.attribute("y").as_int(), currentAnimation.attribute("w").as_int(), currentAnimation.attribute("h").as_int() });
	}
	// up-left
	currentAnimation = sheepAnimations.child("upLeft");
	critterSheepInfo.upLeft.speed = currentAnimation.attribute("speed").as_float();
	critterSheepInfo.upLeft.loop = currentAnimation.attribute("loop").as_bool();
	for (currentAnimation = currentAnimation.child("frame"); currentAnimation; currentAnimation = currentAnimation.next_sibling("frame")) {
		critterSheepInfo.upLeft.PushBack({ currentAnimation.attribute("x").as_int(), currentAnimation.attribute("y").as_int(), currentAnimation.attribute("w").as_int(), currentAnimation.attribute("h").as_int() });
	}
	// up-right
	currentAnimation = sheepAnimations.child("upRight");
	critterSheepInfo.upRight.speed = currentAnimation.attribute("speed").as_float();
	critterSheepInfo.upRight.loop = currentAnimation.attribute("loop").as_bool();
	for (currentAnimation = currentAnimation.child("frame"); currentAnimation; currentAnimation = currentAnimation.next_sibling("frame")) {
		critterSheepInfo.upRight.PushBack({ currentAnimation.attribute("x").as_int(), currentAnimation.attribute("y").as_int(), currentAnimation.attribute("w").as_int(), currentAnimation.attribute("h").as_int() });
	}
	// down-left
	currentAnimation = sheepAnimations.child("downLeft");
	critterSheepInfo.downLeft.speed = currentAnimation.attribute("speed").as_float();
	critterSheepInfo.downLeft.loop = currentAnimation.attribute("loop").as_bool();
	for (currentAnimation = currentAnimation.child("frame"); currentAnimation; currentAnimation = currentAnimation.next_sibling("frame")) {
		critterSheepInfo.downLeft.PushBack({ currentAnimation.attribute("x").as_int(), currentAnimation.attribute("y").as_int(), currentAnimation.attribute("w").as_int(), currentAnimation.attribute("h").as_int() });
	}
	// down-right
	currentAnimation = sheepAnimations.child("downRight");
	critterSheepInfo.downRight.speed = currentAnimation.attribute("speed").as_float();
	critterSheepInfo.downRight.loop = currentAnimation.attribute("loop").as_bool();
	for (currentAnimation = currentAnimation.child("frame"); currentAnimation; currentAnimation = currentAnimation.next_sibling("frame")) {
		critterSheepInfo.downRight.PushBack({ currentAnimation.attribute("x").as_int(), currentAnimation.attribute("y").as_int(), currentAnimation.attribute("w").as_int(), currentAnimation.attribute("h").as_int() });
	}
	// death up-left
	currentAnimation = sheepAnimations.child("deathUpLeft");
	critterSheepInfo.deathUpLeft.speed = currentAnimation.attribute("speed").as_float();
	critterSheepInfo.deathUpLeft.loop = currentAnimation.attribute("loop").as_bool();
	for (currentAnimation = currentAnimation.child("frame"); currentAnimation; currentAnimation = currentAnimation.next_sibling("frame")) {
		critterSheepInfo.deathUpLeft.PushBack({ currentAnimation.attribute("x").as_int(), currentAnimation.attribute("y").as_int(), currentAnimation.attribute("w").as_int(), currentAnimation.attribute("h").as_int() });
	}
	// death up-right
	currentAnimation = sheepAnimations.child("deathUpRight");
	critterSheepInfo.deathUpRight.speed = currentAnimation.attribute("speed").as_float();
	critterSheepInfo.deathUpRight.loop = currentAnimation.attribute("loop").as_bool();
	for (currentAnimation = currentAnimation.child("frame"); currentAnimation; currentAnimation = currentAnimation.next_sibling("frame")) {
		critterSheepInfo.deathUpRight.PushBack({ currentAnimation.attribute("x").as_int(), currentAnimation.attribute("y").as_int(), currentAnimation.attribute("w").as_int(), currentAnimation.attribute("h").as_int() });
	}
	// death down-left
	currentAnimation = sheepAnimations.child("deathDownLeft");
	critterSheepInfo.deathDownLeft.speed = currentAnimation.attribute("speed").as_float();
	critterSheepInfo.deathDownLeft.loop = currentAnimation.attribute("loop").as_bool();
	for (currentAnimation = currentAnimation.child("frame"); currentAnimation; currentAnimation = currentAnimation.next_sibling("frame")) {
		critterSheepInfo.deathDownLeft.PushBack({ currentAnimation.attribute("x").as_int(), currentAnimation.attribute("y").as_int(), currentAnimation.attribute("w").as_int(), currentAnimation.attribute("h").as_int() });
	}
	// death down-right
	currentAnimation = sheepAnimations.child("deathDownRight");
	critterSheepInfo.deathDownRight.speed = currentAnimation.attribute("speed").as_float();
	critterSheepInfo.deathDownRight.loop = currentAnimation.attribute("loop").as_bool();
	for (currentAnimation = currentAnimation.child("frame"); currentAnimation; currentAnimation = currentAnimation.next_sibling("frame")) {
		critterSheepInfo.deathDownRight.PushBack({ currentAnimation.attribute("x").as_int(), currentAnimation.attribute("y").as_int(), currentAnimation.attribute("w").as_int(), currentAnimation.attribute("h").as_int() });
	}

	// Boar
	pugi::xml_node boarAnimations = config.child("dynamicEntities").child("critters").child("boar").child("animations");

	// up
	currentAnimation = boarAnimations.child("up");
	critterBoarInfo.up.speed = currentAnimation.attribute("speed").as_float();
	critterBoarInfo.up.loop = currentAnimation.attribute("loop").as_bool();
	for (currentAnimation = currentAnimation.child("frame"); currentAnimation; currentAnimation = currentAnimation.next_sibling("frame")) {
		critterBoarInfo.up.PushBack({ currentAnimation.attribute("x").as_int(), currentAnimation.attribute("y").as_int(), currentAnimation.attribute("w").as_int(), currentAnimation.attribute("h").as_int() });
	}
	// down
	currentAnimation = boarAnimations.child("down");
	critterBoarInfo.down.speed = currentAnimation.attribute("speed").as_float();
	critterBoarInfo.down.loop = currentAnimation.attribute("loop").as_bool();
	for (currentAnimation = currentAnimation.child("frame"); currentAnimation; currentAnimation = currentAnimation.next_sibling("frame")) {
		critterBoarInfo.down.PushBack({ currentAnimation.attribute("x").as_int(), currentAnimation.attribute("y").as_int(), currentAnimation.attribute("w").as_int(), currentAnimation.attribute("h").as_int() });
	}
	// left
	currentAnimation = boarAnimations.child("left");
	critterBoarInfo.left.speed = currentAnimation.attribute("speed").as_float();
	critterBoarInfo.left.loop = currentAnimation.attribute("loop").as_bool();
	for (currentAnimation = currentAnimation.child("frame"); currentAnimation; currentAnimation = currentAnimation.next_sibling("frame")) {
		critterBoarInfo.left.PushBack({ currentAnimation.attribute("x").as_int(), currentAnimation.attribute("y").as_int(), currentAnimation.attribute("w").as_int(), currentAnimation.attribute("h").as_int() });
	}
	// right
	currentAnimation = boarAnimations.child("right");
	critterBoarInfo.right.speed = currentAnimation.attribute("speed").as_float();
	critterBoarInfo.right.loop = currentAnimation.attribute("loop").as_bool();
	for (currentAnimation = currentAnimation.child("frame"); currentAnimation; currentAnimation = currentAnimation.next_sibling("frame")) {
		critterBoarInfo.right.PushBack({ currentAnimation.attribute("x").as_int(), currentAnimation.attribute("y").as_int(), currentAnimation.attribute("w").as_int(), currentAnimation.attribute("h").as_int() });
	}
	// up-left
	currentAnimation = boarAnimations.child("upLeft");
	critterBoarInfo.upLeft.speed = currentAnimation.attribute("speed").as_float();
	critterBoarInfo.upLeft.loop = currentAnimation.attribute("loop").as_bool();
	for (currentAnimation = currentAnimation.child("frame"); currentAnimation; currentAnimation = currentAnimation.next_sibling("frame")) {
		critterBoarInfo.upLeft.PushBack({ currentAnimation.attribute("x").as_int(), currentAnimation.attribute("y").as_int(), currentAnimation.attribute("w").as_int(), currentAnimation.attribute("h").as_int() });
	}
	// up-right
	currentAnimation = boarAnimations.child("upRight");
	critterBoarInfo.upRight.speed = currentAnimation.attribute("speed").as_float();
	critterBoarInfo.upRight.loop = currentAnimation.attribute("loop").as_bool();
	for (currentAnimation = currentAnimation.child("frame"); currentAnimation; currentAnimation = currentAnimation.next_sibling("frame")) {
		critterBoarInfo.upRight.PushBack({ currentAnimation.attribute("x").as_int(), currentAnimation.attribute("y").as_int(), currentAnimation.attribute("w").as_int(), currentAnimation.attribute("h").as_int() });
	}
	// down-left
	currentAnimation = boarAnimations.child("downLeft");
	critterBoarInfo.downLeft.speed = currentAnimation.attribute("speed").as_float();
	critterBoarInfo.downLeft.loop = currentAnimation.attribute("loop").as_bool();
	for (currentAnimation = currentAnimation.child("frame"); currentAnimation; currentAnimation = currentAnimation.next_sibling("frame")) {
		critterBoarInfo.downLeft.PushBack({ currentAnimation.attribute("x").as_int(), currentAnimation.attribute("y").as_int(), currentAnimation.attribute("w").as_int(), currentAnimation.attribute("h").as_int() });
	}
	// down-right
	currentAnimation = boarAnimations.child("downRight");
	critterBoarInfo.downRight.speed = currentAnimation.attribute("speed").as_float();
	critterBoarInfo.downRight.loop = currentAnimation.attribute("loop").as_bool();
	for (currentAnimation = currentAnimation.child("frame"); currentAnimation; currentAnimation = currentAnimation.next_sibling("frame")) {
		critterBoarInfo.downRight.PushBack({ currentAnimation.attribute("x").as_int(), currentAnimation.attribute("y").as_int(), currentAnimation.attribute("w").as_int(), currentAnimation.attribute("h").as_int() });
	}
	// death up-left
	currentAnimation = boarAnimations.child("deathUpLeft");
	critterBoarInfo.deathUpLeft.speed = currentAnimation.attribute("speed").as_float();
	critterBoarInfo.deathUpLeft.loop = currentAnimation.attribute("loop").as_bool();
	for (currentAnimation = currentAnimation.child("frame"); currentAnimation; currentAnimation = currentAnimation.next_sibling("frame")) {
		critterBoarInfo.deathUpLeft.PushBack({ currentAnimation.attribute("x").as_int(), currentAnimation.attribute("y").as_int(), currentAnimation.attribute("w").as_int(), currentAnimation.attribute("h").as_int() });
	}
	// death up-right
	currentAnimation = boarAnimations.child("deathUpRight");
	critterBoarInfo.deathUpRight.speed = currentAnimation.attribute("speed").as_float();
	critterBoarInfo.deathUpRight.loop = currentAnimation.attribute("loop").as_bool();
	for (currentAnimation = currentAnimation.child("frame"); currentAnimation; currentAnimation = currentAnimation.next_sibling("frame")) {
		critterBoarInfo.deathUpRight.PushBack({ currentAnimation.attribute("x").as_int(), currentAnimation.attribute("y").as_int(), currentAnimation.attribute("w").as_int(), currentAnimation.attribute("h").as_int() });
	}
	// death down-left
	currentAnimation = boarAnimations.child("deathDownLeft");
	critterBoarInfo.deathDownLeft.speed = currentAnimation.attribute("speed").as_float();
	critterBoarInfo.deathDownLeft.loop = currentAnimation.attribute("loop").as_bool();
	for (currentAnimation = currentAnimation.child("frame"); currentAnimation; currentAnimation = currentAnimation.next_sibling("frame")) {
		critterBoarInfo.deathDownLeft.PushBack({ currentAnimation.attribute("x").as_int(), currentAnimation.attribute("y").as_int(), currentAnimation.attribute("w").as_int(), currentAnimation.attribute("h").as_int() });
	}
	// death down-right
	currentAnimation = boarAnimations.child("deathDownRight");
	critterBoarInfo.deathDownRight.speed = currentAnimation.attribute("speed").as_float();
	critterBoarInfo.deathDownRight.loop = currentAnimation.attribute("loop").as_bool();
	for (currentAnimation = currentAnimation.child("frame"); currentAnimation; currentAnimation = currentAnimation.next_sibling("frame")) {
		critterBoarInfo.deathDownRight.PushBack({ currentAnimation.attribute("x").as_int(), currentAnimation.attribute("y").as_int(), currentAnimation.attribute("w").as_int(), currentAnimation.attribute("h").as_int() });
	}

	return ret;
}

bool j1EntityFactory::Start()
{
	bool ret = true;

	LOG("Loading entities textures");

	/// TODO Joan (balancing)
	// ENTITIES INFO
	/// ALLIANCE
	/// Dynamic Entities
	alleriaInfo.unitInfo.size = { 64,64 };
	alleriaInfo.unitInfo.offsetSize = { -16,-16 };
	turalyonInfo.unitInfo.size = { 64,64 };
	turalyonInfo.unitInfo.offsetSize = { -16,-16 };

	// Footman
	footmanInfo.unitInfo.priority = 2;

	/// Radius
	footmanInfo.unitInfo.attackRadius = 2;
	footmanInfo.unitInfo.sightRadius = 4;

	/// Damage
	footmanInfo.unitInfo.heavyDamage = 6;
	footmanInfo.unitInfo.lightDamage = 8;
	footmanInfo.unitInfo.airDamage = 0;
	footmanInfo.unitInfo.towerDamage = 6;

	/// Speed
	footmanInfo.unitInfo.maxSpeed = 70.0f;
	footmanInfo.unitInfo.currSpeed = footmanInfo.unitInfo.maxSpeed;

	/// Life
	footmanInfo.unitInfo.maxLife = 60;
	footmanInfo.unitInfo.currLife = footmanInfo.unitInfo.maxLife;

	///
	footmanInfo.unitInfo.size = { 32,32 };
	footmanInfo.unitInfo.offsetSize = { 0,0 };
	//_Footman

	// Elven Archer
	elvenArcherInfo.unitInfo.priority = 2;

	/// Radius
	elvenArcherInfo.unitInfo.attackRadius = 5;
	elvenArcherInfo.unitInfo.sightRadius = 8;

	/// Damage
	elvenArcherInfo.unitInfo.heavyDamage = 4;
	elvenArcherInfo.unitInfo.lightDamage = 7;
	elvenArcherInfo.unitInfo.airDamage = 10;
	elvenArcherInfo.unitInfo.towerDamage = 11;

	/// Speed
	elvenArcherInfo.unitInfo.maxSpeed = 90.0f;
	elvenArcherInfo.unitInfo.currSpeed = elvenArcherInfo.unitInfo.maxSpeed;

	/// Life	
	elvenArcherInfo.unitInfo.maxLife = 50;
	elvenArcherInfo.unitInfo.currLife = elvenArcherInfo.unitInfo.maxLife;

	///
	elvenArcherInfo.arrowSpeed = 170.0f;
	elvenArcherInfo.unitInfo.size = { 32,32 };
	elvenArcherInfo.unitInfo.offsetSize = { 0,0 };
	//_Elven_Archer

	// Gryphon Rider
	gryphonRiderInfo.unitInfo.priority = 4;

	/// Radius
	gryphonRiderInfo.unitInfo.attackRadius = 7;
	gryphonRiderInfo.unitInfo.sightRadius = 12;

	/// Damage
	gryphonRiderInfo.unitInfo.heavyDamage = 7;
	gryphonRiderInfo.unitInfo.lightDamage = 5;
	gryphonRiderInfo.unitInfo.airDamage = 7;
	gryphonRiderInfo.unitInfo.towerDamage = 8;

	/// Speed
	gryphonRiderInfo.unitInfo.maxSpeed = 160.0f;
	gryphonRiderInfo.unitInfo.currSpeed = gryphonRiderInfo.unitInfo.maxSpeed;

	/// Life
	gryphonRiderInfo.unitInfo.maxLife = 100;
	gryphonRiderInfo.unitInfo.currLife = gryphonRiderInfo.unitInfo.maxLife;

	///
	gryphonRiderInfo.fireSpeed = 120.0f;
	gryphonRiderInfo.unitInfo.size = { 64,64 };
	gryphonRiderInfo.unitInfo.offsetSize = { -16,-16 };
	//_Gryphon_Rider

	/// Static Entities
	// Towers
	scoutTowerInfo.life = 150;
	scoutTowerInfo.damage = 7;

	playerGuardTowerInfo.life = 175;
	playerGuardTowerInfo.damage = 13;

	playerCannonTowerInfo.life = 200;
	playerCannonTowerInfo.damage = 17;
	//_Towers

	/// HORDE
	/// Dynamic Entities
	// Grunt
	gruntInfo.unitInfo.priority = 3;

	/// Radius
	gruntInfo.unitInfo.attackRadius = 2;
	gruntInfo.unitInfo.sightRadius = 4;

	/// Damage
	gruntInfo.unitInfo.heavyDamage = 6;
	gruntInfo.unitInfo.lightDamage = 8;
	gruntInfo.unitInfo.airDamage = 0;
	gruntInfo.unitInfo.towerDamage = 6;

	/// Speed
	gruntInfo.unitInfo.maxSpeed = 70.0f;
	gruntInfo.unitInfo.currSpeed = gruntInfo.unitInfo.maxSpeed;

	/// Life
	gruntInfo.unitInfo.maxLife = 60;
	gruntInfo.unitInfo.currLife = gruntInfo.unitInfo.maxLife;

	///
	gruntInfo.droppedGold = 10;
	gruntInfo.unitInfo.size = { 32,32 };
	gruntInfo.unitInfo.offsetSize = { 0,0 };
	//_Grunt

	// Troll Axethrower
	trollAxethrowerInfo.unitInfo.priority = 3;

	/// Radius
	trollAxethrowerInfo.unitInfo.attackRadius = 5;
	trollAxethrowerInfo.unitInfo.sightRadius = 8;

	/// Damage
	trollAxethrowerInfo.unitInfo.heavyDamage = 4;
	trollAxethrowerInfo.unitInfo.lightDamage = 7;
	trollAxethrowerInfo.unitInfo.airDamage = 10;
	trollAxethrowerInfo.unitInfo.towerDamage = 11;

	/// Speed
	trollAxethrowerInfo.unitInfo.maxSpeed = 90.0f;
	trollAxethrowerInfo.unitInfo.currSpeed = trollAxethrowerInfo.unitInfo.maxSpeed;

	/// Life
	trollAxethrowerInfo.unitInfo.maxLife = 40;
	trollAxethrowerInfo.unitInfo.currLife = trollAxethrowerInfo.unitInfo.maxLife;

	///
	trollAxethrowerInfo.axeSpeed = 160.0f;
	trollAxethrowerInfo.droppedGold = 10;
	trollAxethrowerInfo.unitInfo.size = { 32,32 };
	trollAxethrowerInfo.unitInfo.offsetSize = { 0,0 };
	//_Troll_Axethrower

	// Dragon
	dragonInfo.unitInfo.priority = 5;

	/// Radius
	dragonInfo.unitInfo.attackRadius = 7;
	dragonInfo.unitInfo.sightRadius = 12;

	/// Damage
	dragonInfo.unitInfo.heavyDamage = 7;
	dragonInfo.unitInfo.lightDamage = 5;
	dragonInfo.unitInfo.airDamage = 7;
	dragonInfo.unitInfo.towerDamage = 8;

	/// Speed
	dragonInfo.unitInfo.maxSpeed = 170.0f;
	dragonInfo.unitInfo.currSpeed = dragonInfo.unitInfo.maxSpeed;

	/// Life
	dragonInfo.unitInfo.maxLife = 50;
	dragonInfo.unitInfo.currLife = dragonInfo.unitInfo.maxLife;

	///
	dragonInfo.fireSpeed = 200.0f;
	dragonInfo.droppedGold = 10;
	dragonInfo.unitInfo.size = { 64,64 };
	dragonInfo.unitInfo.offsetSize = { -16,-16 };
	//_Dragon

	/// Static Entities
	// Towers
	watchTowerInfo.life = 150;
	watchTowerInfo.damage = 7;

	enemyGuardTowerInfo.life = 175;
	enemyGuardTowerInfo.damage = 13;

	enemyCannonTowerInfo.life = 200;
	enemyCannonTowerInfo.damage = 17;
	//_Towers

	// CRITTERS
	// Sheep
	critterSheepInfo.unitInfo.priority = 1;

	/// Speed
	critterSheepInfo.unitInfo.maxSpeed = 30.0f;
	critterSheepInfo.unitInfo.currSpeed = footmanInfo.unitInfo.maxSpeed;

	/// Life
	critterSheepInfo.unitInfo.currLife = 100;
	critterSheepInfo.unitInfo.maxLife = critterSheepInfo.unitInfo.currLife;

	///
	critterSheepInfo.restoredHealth = 10;
	critterSheepInfo.unitInfo.size = { 32,32 };
	critterSheepInfo.unitInfo.offsetSize = { 0,0 };
	//_Sheep

	// Boar
	critterBoarInfo.unitInfo.priority = 1;

	/// Speed
	critterBoarInfo.unitInfo.maxSpeed = 30.0f;
	critterBoarInfo.unitInfo.currSpeed = footmanInfo.unitInfo.maxSpeed;

	/// Life
	critterBoarInfo.unitInfo.currLife = 20;
	critterBoarInfo.unitInfo.maxLife = critterBoarInfo.unitInfo.currLife;

	///
	critterBoarInfo.restoredHealth = 20;
	critterBoarInfo.unitInfo.size = { 32,32 };
	critterBoarInfo.unitInfo.offsetSize = { 0,0 };
	//_Boar

	// NEUTRAL BUILDINGS
	// Runestone
	runestoneInfo.sightRadius = 7;
	//_Runestone

	// Load textures
	humanBuildingsTex = App->tex->Load(humanBuildingsTexName.data());
	neutralBuildingsTex = App->tex->Load(neutralBuildingsTexName.data());
	orcishBuildingsTex = App->tex->Load(orcishBuildingsTexName.data());

	elvenArcherTex = App->tex->Load(elvenArcherTexName.data());
	trollAxethrowerTex = App->tex->Load(trollAxethrowerTexName.data());
	footmanTex = App->tex->Load(footmanTexName.data());
	gruntTex = App->tex->Load(gruntTexName.data());
	gryphonRiderTex = App->tex->Load(gryphonRiderTexName.data());
	dragonTex = App->tex->Load(dragonTexName.data());

	crittersTex = App->tex->Load(crittersTexName.data());

	khadgarTex = App->tex->Load(khadgarTexName.data());
	alleriaTex = App->tex->Load(alleriaTexName.data());
	turalyonTex = App->tex->Load(turalyonTexName.data());

	//Already built buildings info
	builtChickenFarmInfo = chickenFarmInfo;
	builtChickenFarmInfo.isBuilt = true;

	builtBarracksInfo = barracksInfo;
	builtBarracksInfo.isBuilt = true;

	return ret;
}

bool j1EntityFactory::PreUpdate()
{
	bool ret = true;

	// Spawn entities
	list<Entity*>::const_iterator it = toSpawnEntities.begin();

	while (it != toSpawnEntities.end()) {

		fPoint pos = (*it)->GetPos();
		int x = pos.x * App->scene->scale;
		int y = pos.y * App->scene->scale;
		App->map->WorldToMap(x, y);

		// Move the entity from the waiting list to the active list
		if ((*it)->entityType == EntityCategory_DYNAMIC_ENTITY) {

			activeDynamicEntities.push_back((DynamicEntity*)(*it));
			if ((*it)->entitySide == EntitySide_Player)
				App->player->unitProduce++;
			LOG("Spawning dynamic entity at tile %d,%d", x, y);
		}
		else if ((*it)->entityType == EntityCategory_STATIC_ENTITY) {

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

	// Update active static entities
	list<StaticEntity*>::const_iterator statEnt = activeStaticEntities.begin();

	while (statEnt != activeStaticEntities.end()) {
		(*statEnt)->Move(dt);
		statEnt++;
	}

	// Update active dynamic entities
	list<DynamicEntity*>::const_iterator dynEnt = activeDynamicEntities.begin();

	while (dynEnt != activeDynamicEntities.end()) {
		(*dynEnt)->Move(dt);
		dynEnt++;
	}

	return ret;
}

void j1EntityFactory::OnCollision(ColliderGroup* c1, ColliderGroup* c2, CollisionState collisionState)
{
	// Check for collisions
	list<DynamicEntity*>::const_iterator dynEntity = activeDynamicEntities.begin();

	while (dynEntity != activeDynamicEntities.end()) {

		// - SightRadiusCollider and AttackRadiusCollider call their owner as the c1 Collider
		if ((*dynEntity)->GetSightRadiusCollider() == c1 || (*dynEntity)->GetAttackRadiusCollider() == c1) {

			(*dynEntity)->OnCollision(c1, c2, collisionState);
			break;
		}

		dynEntity++;
	}

	list<StaticEntity*>::const_iterator statEntity = activeStaticEntities.begin();

	while (statEntity != activeStaticEntities.end()) {

		// - SightRadiusCollider calls its owner as the c1 Collider
		if ((*statEntity)->GetSightRadiusCollider() == c1) {

			(*statEntity)->OnCollision(c1, c2, collisionState);
			break;
		}

		statEntity++;
	}
}

void j1EntityFactory::DrawStaticEntityPreview(ENTITY_TYPE staticEntityType, iPoint mousePos)
{
	switch (staticEntityType) {

	case EntityType_CHICKEN_FARM:
		SDL_SetTextureAlphaMod(humanBuildingsTex, previewBuildingOpacity);
		App->printer->PrintSprite(mousePos, humanBuildingsTex, chickenFarmInfo.completeTexArea);
		break;
	case EntityType_ELVEN_LUMBER_MILL:
		SDL_SetTextureAlphaMod(humanBuildingsTex, previewBuildingOpacity);
		App->printer->PrintSprite(mousePos, humanBuildingsTex, elvenLumberMillInfo.completeTexArea);
		break;
	case EntityType_MAGE_TOWER:
		SDL_SetTextureAlphaMod(humanBuildingsTex, previewBuildingOpacity);
		App->printer->PrintSprite(mousePos, humanBuildingsTex, mageTowerInfo.completeTexArea);
		break;
	case EntityType_GRYPHON_AVIARY:
		SDL_SetTextureAlphaMod(humanBuildingsTex, previewBuildingOpacity);
		App->printer->PrintSprite(mousePos, humanBuildingsTex, gryphonAviaryInfo.completeTexArea);
		break;
	case EntityType_STABLES:
		SDL_SetTextureAlphaMod(humanBuildingsTex, previewBuildingOpacity);
		App->printer->PrintSprite(mousePos, humanBuildingsTex, stablesInfo.completeTexArea);
		break;
	case EntityType_SCOUT_TOWER:
		SDL_SetTextureAlphaMod(humanBuildingsTex, previewBuildingOpacity);
		App->printer->PrintSprite(mousePos, humanBuildingsTex, scoutTowerInfo.completeTexArea);
		break;
	case EntityType_PLAYER_GUARD_TOWER:
		SDL_SetTextureAlphaMod(humanBuildingsTex, previewBuildingOpacity);
		App->printer->PrintSprite(mousePos, humanBuildingsTex, playerGuardTowerInfo.completeTexArea);
		break;
	case EntityType_PLAYER_CANNON_TOWER:
		SDL_SetTextureAlphaMod(humanBuildingsTex, previewBuildingOpacity);
		App->printer->PrintSprite(mousePos, humanBuildingsTex, playerCannonTowerInfo.completeTexArea);
		break;
	case EntityType_BARRACKS:
		SDL_SetTextureAlphaMod(humanBuildingsTex, previewBuildingOpacity);
		App->printer->PrintSprite(mousePos, humanBuildingsTex, barracksInfo.completeTexArea);
		break;
	default:
		break;
	}
	
	HandleStaticEntityPreviewTiles(staticEntityType, mousePos);
}

// Handles when to return green or red tiles 
void j1EntityFactory::HandleStaticEntityPreviewTiles(ENTITY_TYPE staticEntityType, iPoint mousePos)
{
	switch (staticEntityType) {

	case EntityType_CHICKEN_FARM:
	case EntityType_SCOUT_TOWER:
	case EntityType_PLAYER_GUARD_TOWER:
	case EntityType_PLAYER_CANNON_TOWER:
		DrawStaticEntityPreviewTiles(true, StaticEntitySize_Small, mousePos);

		if (IsPreviewBuildingOnEntity(App->player->GetMouseTilePos(), StaticEntitySize_Small))
			DrawStaticEntityPreviewTiles(false, StaticEntitySize_Small, mousePos);

		break;
	case EntityType_ELVEN_LUMBER_MILL:
	case EntityType_MAGE_TOWER:
	case EntityType_GRYPHON_AVIARY:
	case EntityType_STABLES:

		DrawStaticEntityPreviewTiles(true, StaticEntitySize_Medium, mousePos);


		if (IsPreviewBuildingOnEntity(App->player->GetMouseTilePos(), StaticEntitySize_Medium))
			DrawStaticEntityPreviewTiles(false, StaticEntitySize_Medium, mousePos);

		break;

	default:
		break;
	}
}

//Draws green or red tiles on the preview depending on if there's a building on the ground and the preview building size
void j1EntityFactory::DrawStaticEntityPreviewTiles(bool isPlaceable, StaticEntitySize buildingSize, iPoint mousePos)
{

	iPoint mouseTilePos = App->player->GetMouseTilePos();
	SDL_Color green = { 0,255,0, (Uint8)previewTilesopacity };
	SDL_Color red = { 255,0,0,(Uint8)previewTilesopacity };

	if (isPlaceable) { //Small
		if (!IsEntityOnTileBySize(mouseTilePos) && App->pathfinding->IsWalkable(mouseTilePos))
			App->printer->PrintQuad({ mousePos.x, mousePos.y, 32, 32 }, green, true);

		if (!IsEntityOnTileBySize({ mouseTilePos.x + 1, mouseTilePos.y }) && App->pathfinding->IsWalkable({ mouseTilePos.x + 1, mouseTilePos.y }))
			App->printer->PrintQuad({ mousePos.x + 32, mousePos.y, 32, 32 }, green, true);

		if (!IsEntityOnTileBySize({ mouseTilePos.x, mouseTilePos.y + 1 }) && App->pathfinding->IsWalkable({ mouseTilePos.x, mouseTilePos.y + 1 }))
			App->printer->PrintQuad({ mousePos.x, mousePos.y + 32, 32, 32 }, green, true);

		if (!IsEntityOnTileBySize({ mouseTilePos.x + 1, mouseTilePos.y + 1 }) && App->pathfinding->IsWalkable({ mouseTilePos.x + 1, mouseTilePos.y + 1 }))
			App->printer->PrintQuad({ mousePos.x + 32, mousePos.y + 32, 32, 32 }, green, true);

		if (buildingSize == StaticEntitySize_Medium || buildingSize == StaticEntitySize_Big) { //Medium

			if (!IsEntityOnTileBySize({ mouseTilePos.x + 1, mouseTilePos.y + 2 }) && App->pathfinding->IsWalkable({ mouseTilePos.x + 1, mouseTilePos.y + 2 }))
				App->printer->PrintQuad({ mousePos.x + 32, mousePos.y + 64, 32, 32 }, green, true);

			if (!IsEntityOnTileBySize({ mouseTilePos.x + 2, mouseTilePos.y + 1 }) && App->pathfinding->IsWalkable({ mouseTilePos.x + 2, mouseTilePos.y + 1 }))
				App->printer->PrintQuad({ mousePos.x + 64, mousePos.y + 32, 32, 32 }, green, true);

			if (!IsEntityOnTileBySize({ mouseTilePos.x + 2, mouseTilePos.y }) && App->pathfinding->IsWalkable({ mouseTilePos.x + 2, mouseTilePos.y }))
				App->printer->PrintQuad({ mousePos.x + 64, mousePos.y, 32, 32 }, green, true);

			if (!IsEntityOnTileBySize({ mouseTilePos.x, mouseTilePos.y + 2 }) && App->pathfinding->IsWalkable({ mouseTilePos.x , mouseTilePos.y + 2 }))
				App->printer->PrintQuad({ mousePos.x, mousePos.y + 64, 32, 32 }, green, true);

			if (!IsEntityOnTileBySize({ mouseTilePos.x + 2, mouseTilePos.y + 2 }) && App->pathfinding->IsWalkable({ mouseTilePos.x + 2, mouseTilePos.y + 2 }))
				App->printer->PrintQuad({ mousePos.x + 64, mousePos.y + 64, 32, 32 }, green, true);

		}

	}
	else if (!isPlaceable)
	{
		if (IsEntityOnTileBySize(mouseTilePos) || !App->pathfinding->IsWalkable(mouseTilePos))
			App->printer->PrintQuad({ mousePos.x, mousePos.y, 32, 32 }, red, true);

		if (IsEntityOnTileBySize({ mouseTilePos.x + 1, mouseTilePos.y }) || !App->pathfinding->IsWalkable({ mouseTilePos.x + 1, mouseTilePos.y }))
			App->printer->PrintQuad({ mousePos.x + 32, mousePos.y, 32, 32 }, red, true);

		if (IsEntityOnTileBySize({ mouseTilePos.x, mouseTilePos.y + 1 }) || !App->pathfinding->IsWalkable({ mouseTilePos.x, mouseTilePos.y + 1 }))
			App->printer->PrintQuad({ mousePos.x, mousePos.y + 32, 32, 32 }, red, true);

		if (IsEntityOnTileBySize({ mouseTilePos.x + 1, mouseTilePos.y + 1 }) || !App->pathfinding->IsWalkable({ mouseTilePos.x + 1, mouseTilePos.y + 1 }))
			App->printer->PrintQuad({ mousePos.x + 32, mousePos.y + 32, 32, 32 }, red, true);

		if (buildingSize == StaticEntitySize_Medium || buildingSize == StaticEntitySize_Big) { //Medium

			if (IsEntityOnTileBySize({ mouseTilePos.x + 1, mouseTilePos.y + 2 }) || !App->pathfinding->IsWalkable({ mouseTilePos.x + 1, mouseTilePos.y + 2 }))
				App->printer->PrintQuad({ mousePos.x + 32, mousePos.y + 64, 32, 32 }, red, true);

			if (IsEntityOnTileBySize({ mouseTilePos.x + 2, mouseTilePos.y + 1 }) || !App->pathfinding->IsWalkable({ mouseTilePos.x + 2, mouseTilePos.y + 1 }))
				App->printer->PrintQuad({ mousePos.x + 64, mousePos.y + 32, 32, 32 }, red, true);

			if (IsEntityOnTileBySize({ mouseTilePos.x + 2, mouseTilePos.y }) || !App->pathfinding->IsWalkable({ mouseTilePos.x + 2, mouseTilePos.y }))
				App->printer->PrintQuad({ mousePos.x + 64, mousePos.y , 32, 32 }, red, true);

			if (IsEntityOnTileBySize({ mouseTilePos.x , mouseTilePos.y + 2 }) || !App->pathfinding->IsWalkable({ mouseTilePos.x, mouseTilePos.y + 2 }))
				App->printer->PrintQuad({ mousePos.x, mousePos.y + 64, 32, 32 }, red, true);

			if (IsEntityOnTileBySize({ mouseTilePos.x + 2, mouseTilePos.y + 2 }) || !App->pathfinding->IsWalkable({ mouseTilePos.x + 2, mouseTilePos.y + 2 }))
				App->printer->PrintQuad({ mousePos.x + 64, mousePos.y + 64, 32, 32 }, red, true);

		}
	}
}

const EntityInfo& j1EntityFactory::GetBuildingInfo(ENTITY_TYPE staticEntityType)
{
	switch (staticEntityType) {
		//Human buildings
	case EntityType_TOWN_HALL: 
		return (const EntityInfo&)townHallInfo;
		break;
	case EntityType_CHICKEN_FARM:
		return (const EntityInfo&)chickenFarmInfo;
		break;
	case EntityType_BARRACKS:
		return (const EntityInfo&)barracksInfo;
		break;
	case EntityType_ELVEN_LUMBER_MILL:
		return (const EntityInfo&)elvenLumberMillInfo;
		break;
	case EntityType_MAGE_TOWER:
		return(const EntityInfo&)mageTowerInfo;
		break;
	case EntityType_GRYPHON_AVIARY:
		return(const EntityInfo&)gryphonAviaryInfo;
		break;
	case EntityType_STABLES:
		return(const EntityInfo&)stablesInfo;
		break;
	case EntityType_SCOUT_TOWER:
		return(const EntityInfo&)scoutTowerInfo;
		break;
	case EntityType_PLAYER_GUARD_TOWER:
		return(const EntityInfo&)playerGuardTowerInfo;
		break;
	case EntityType_PLAYER_CANNON_TOWER:
		return(const EntityInfo&)playerCannonTowerInfo;
		break;
		//Neutral buildings
	case EntityType_GOLD_MINE: 
		return (const EntityInfo&)goldMineInfo;
		break;
	case EntityType_RUNESTONE:
		return (const EntityInfo&)runestoneInfo;
		break;
		//Enemy buildings
	case EntityType_GREAT_HALL: 
		return (const EntityInfo&)greatHallInfo;
		break;
	case EntityType_STRONGHOLD:
		return (const EntityInfo&)strongholdInfo;
		break;
	case EntityType_FORTRESS:
		return (const EntityInfo&)fortressInfo;
		break;
	case EntityType_ENEMY_BARRACKS:
		return (const EntityInfo&)enemyBarracksInfo;
		break;
	case EntityType_PIG_FARM:
		return (const EntityInfo&)pigFarmInfo;
		break;
	case EntityType_TROLL_LUMBER_MILL:
		return (const EntityInfo&)trollLumberMillInfo;
		break;
	case EntityType_ALTAR_OF_STORMS:
		return (const EntityInfo&)altarOfStormsInfo;
		break;
	case EntityType_DRAGON_ROOST:
		return (const EntityInfo&)dragonRoostInfo;
		break;
	case EntityType_TEMPLE_OF_THE_DAMNED:
		return (const EntityInfo&)templeOfTheDamnedInfo;
		break;
	case EntityType_OGRE_MOUND:
		return (const EntityInfo&)ogreMoundInfo;
		break;
	case EntityType_ENEMY_BLACKSMITH:
		return (const EntityInfo&)enemyBlacksmithInfo;
		break;
	case EntityType_WATCH_TOWER: 
		return (const EntityInfo&)watchTowerInfo;
		break;
	case EntityType_ENEMY_GUARD_TOWER:
		return (const EntityInfo&)enemyGuardTowerInfo;
		break;
	case EntityType_ENEMY_CANNON_TOWER:
		return (const EntityInfo&)enemyCannonTowerInfo;
		break;
	default:
		return (const EntityInfo&)chickenFarmInfo;
		break;
	}
}

const EntityInfo& j1EntityFactory::GetUnitInfo(ENTITY_TYPE dynamicEntityType)
{
	switch (dynamicEntityType) {
	case EntityType_FOOTMAN:
		return (const EntityInfo&)footmanInfo;
		break;
	case EntityType_GRUNT:
		return (const EntityInfo&)gruntInfo;
		break;
	case EntityType_ELVEN_ARCHER:
		return (const EntityInfo&)elvenArcherInfo;
		break;
	case EntityType_TROLL_AXETHROWER:
		return (const EntityInfo&)trollAxethrowerInfo;
		break;
	case EntityType_GRYPHON_RIDER:
		return (const EntityInfo&)gryphonRiderInfo;
		break;
	case EntityType_DRAGON:
		return (const EntityInfo&)dragonInfo;
		break;
	case EntityType_SHEEP:
		return (EntityInfo&)critterSheepInfo;
		break;
	case EntityType_BOAR:
		return (EntityInfo&)critterBoarInfo;
		break;
	case EntityType_ALLERIA:
		return (EntityInfo&)alleriaInfo;
		break;
	case EntityType_TURALYON:
		return (EntityInfo&)turalyonInfo;
		break;
	default:
		return (const EntityInfo&)footmanInfo;
		break;
	}
}

const EntityInfo& j1EntityFactory::GetBuiltBuilding(ENTITY_TYPE staticEntityType)
{
	switch (staticEntityType) {
	case EntityType_BARRACKS:
		return (const EntityInfo&)builtBarracksInfo;
		break;

	case EntityType_CHICKEN_FARM:
		return (const EntityInfo&)builtChickenFarmInfo;
		break;

	default:
		return (const EntityInfo&)builtChickenFarmInfo;
		break;
	}
}

SDL_Texture* j1EntityFactory::GetHumanBuildingTexture() {

	return humanBuildingsTex;
}

SDL_Texture* j1EntityFactory::GetNeutralBuildingTexture() {

	return neutralBuildingsTex;
}

// It returns true if there's an entity in the tile (it considers the size of the entity)
bool j1EntityFactory::IsEntityOnTileBySize(iPoint tile) const
{
	// Dynamic entities
	list<DynamicEntity*>::const_iterator activeDyn = activeDynamicEntities.begin();

	while (activeDyn != activeDynamicEntities.end()) {

		iPoint entityTile = App->map->WorldToMap((*activeDyn)->GetPos().x, (*activeDyn)->GetPos().y);
		if (tile.x == entityTile.x && tile.y == entityTile.y)
			return true;

		activeDyn++;
	}

	// Static entities
	list<StaticEntity*>::const_iterator activeStatic = activeStaticEntities.begin();

	while (activeStatic != activeStaticEntities.end()) {

		iPoint entityTile = App->map->WorldToMap((*activeStatic)->GetPos().x, (*activeStatic)->GetPos().y);

		if ((*activeStatic)->GetSize().x == 64 && (*activeStatic)->GetSize().y == 64) { //Small
			for (int i = 0; i < 2; i++) {
				for (int j = 0; j < 2; j++) {
					if (tile.x == entityTile.x + i && tile.y == entityTile.y + j)
						return true;
				}
			}
		}
		else if ((*activeStatic)->GetSize().x == 96 && (*activeStatic)->GetSize().y == 96) { //Medium
			for (int i = 0; i < 3; i++) {
				for (int j = 0; j < 3; j++) {
					if (tile.x == entityTile.x + i && tile.y == entityTile.y + j)
						return true;
				}
			}
		}
		else if ((*activeStatic)->GetSize().x == 128 && (*activeStatic)->GetSize().y == 128) { //Big

			for (int i = 0; i < 4; i++) {
				for (int j = 0; j < 4; j++) {
					if (tile.x == entityTile.x + i && tile.y == entityTile.y + j)
						return true;
				}
			}
		}
		activeStatic++;
	}
	// We do also need to check the toSpawn list (just in case)
	list<Entity*>::const_iterator toSpawn = toSpawnEntities.begin();
	while (toSpawn != toSpawnEntities.end()) {

		iPoint entityTile = App->map->WorldToMap((*toSpawn)->GetPos().x, (*toSpawn)->GetPos().y);

		if (tile.x == entityTile.x && tile.y == entityTile.y)
			return true;

		toSpawn++;
	}

	if (!App->pathfinding->IsOnBase({ App->player->GetMouseTilePos().x, App->player->GetMouseTilePos().y }))
	{
		LOG("OUT");
		return true;
	}
	else
		LOG("IN");

	return false;
}

// Returns true if a building can NOT be built in that spot
bool j1EntityFactory::IsPreviewBuildingOnEntity(iPoint tile, StaticEntitySize buildingSize) const
{
	// Dynamic entities
	list<DynamicEntity*>::const_iterator activeDyn = activeDynamicEntities.begin();

	while (activeDyn != activeDynamicEntities.end()) {
	
		iPoint entityTile = App->map->WorldToMap((*activeDyn)->GetPos().x, (*activeDyn)->GetPos().y);
		iPoint entityNextTile = (*activeDyn)->GetSingleUnit()->nextTile;

		// This checks the tile of the dynamic entity and its surroundings
		switch (buildingSize)
		{
		case StaticEntitySize_Small:
			for (int i = -1; i < 1; i++) {
				for (int j = -1; j < 1; j++) {
					if ((tile.x == entityTile.x + i && tile.y == entityTile.y + j)
						|| (tile.x == entityNextTile.x + i && tile.y == entityNextTile.y + j))
						return true;
				}
			}
			break;
		case StaticEntitySize_Medium:
			for (int i = -2; i < 1; i++) {
				for (int j = -2; j < 1; j++) {
					if ((tile.x == entityTile.x + i && tile.y == entityTile.y + j)
						|| (tile.x == entityNextTile.x + i && tile.y == entityNextTile.y + j))
						return true;
				}
			}
			break;
		case StaticEntitySize_Big:
			for (int i = -3; i < 1; i++) {
				for (int j = -3; j < 1; j++) {
					if ((tile.x == entityTile.x + i && tile.y == entityTile.y + j)
						|| (tile.x == entityNextTile.x + i && tile.y == entityNextTile.y + j))
						return true;
				}
			}
			break;
		default:
			break;
		}

		activeDyn++;
	}

	// Static entities
	list<StaticEntity*>::const_iterator activeStatic = activeStaticEntities.begin();

	while (activeStatic != activeStaticEntities.end()) {

		iPoint entityTile = App->map->WorldToMap((*activeStatic)->GetPos().x, (*activeStatic)->GetPos().y);

		//This checks all of the tiles that conform of the static entity and their surrounding tiles
		if ((*activeStatic)->GetSize().x == 64 && (*activeStatic)->GetSize().y == 64) {
			switch (buildingSize)
			{
			case StaticEntitySize_Small:
				for (int i = -1; i < 2; i++) {
					for (int j = -1; j < 2; j++) {
						if (tile.x == entityTile.x + i && tile.y == entityTile.y + j)
							return true;
					}
				}
				break;
			case StaticEntitySize_Medium:
				for (int i = -2; i < 2; i++) {
					for (int j = -2; j < 2; j++) {
						if (tile.x == entityTile.x + i && tile.y == entityTile.y + j)
							return true;
					}
				}
				break;
			case StaticEntitySize_Big:
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
			case StaticEntitySize_Small:
				for (int i = -1; i < 3; i++) {
					for (int j = -1; j < 3; j++) {
						if (tile.x == entityTile.x + i && tile.y == entityTile.y + j)
							return true;
					}
				}
				break;
			case StaticEntitySize_Medium:
				for (int i = -2; i < 3; i++) {
					for (int j = -2; j < 3; j++) {
						if (tile.x == entityTile.x + i && tile.y == entityTile.y + j)
							return true;
					}
				}
				break;
			case StaticEntitySize_Big:
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
			case StaticEntitySize_Small:
				for (int i = -1; i < 4; i++) {
					for (int j = -1; j < 4; j++) {
						if (tile.x == entityTile.x + i && tile.y == entityTile.y + j)
							return true;
					}
				}
				break;
			case StaticEntitySize_Medium:
				for (int i = -2; i < 4; i++) {
					for (int j = -2; j < 4; j++) {
						if (tile.x == entityTile.x + i && tile.y == entityTile.y + j)
							return true;
					}
				}
				break;
			case StaticEntitySize_Big:
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

	// Check if a building can be edificated, depending on the map walkability
	switch (buildingSize)
	{
	case StaticEntitySize_Small:
		for (int i = 0; i < 2; i++) {
			for (int j = 0; j < 2; j++) {
				if (!App->pathfinding->IsWalkable({ App->player->GetMouseTilePos().x + i, App->player->GetMouseTilePos().y + j }))
					return true;
			}
		}
		break;
	case StaticEntitySize_Medium:
		for (int i = 0; i < 3; i++) {
			for (int j = 0; j < 3; j++) {
				if (!App->pathfinding->IsWalkable({ App->player->GetMouseTilePos().x + i, App->player->GetMouseTilePos().y + j }))
					return true;
			}
		}
		break;
	case StaticEntitySize_Big:

		for (int i = 0; i < 4; i++) {
			for (int j = 0; j < 4; j++) {
				if (!App->pathfinding->IsWalkable({ App->player->GetMouseTilePos().x + i, App->player->GetMouseTilePos().y + j }))
					return true;
			}
		}
		break;

	default:
		break;
	}

	switch (buildingSize)
	{
	case StaticEntitySize_Small:
		for (int i = 0; i < 2; i++) {
			for (int j = 0; j < 2; j++) {
				if (!App->pathfinding->IsOnBase({ App->player->GetMouseTilePos().x + i, App->player->GetMouseTilePos().y + j }))
					return true;
			}
		}
		break;
	case StaticEntitySize_Medium:
		for (int i = 0; i < 3; i++) {
			for (int j = 0; j < 3; j++) {
				if (!App->pathfinding->IsOnBase({ App->player->GetMouseTilePos().x + i, App->player->GetMouseTilePos().y + j }))
					return true;
			}
		}
		break;
	case StaticEntitySize_Big:

		for (int i = 0; i < 4; i++) {
			for (int j = 0; j < 4; j++) {
				if (!App->pathfinding->IsOnBase({ App->player->GetMouseTilePos().x + i, App->player->GetMouseTilePos().y + j }))
					return true;
			}
		}
		break;

	default:
		break;
	}

	return false;
}

bool j1EntityFactory::PostUpdate()
{
	bool ret = true;

	// Remove active entities
	/// Remove dynamic entities
	list<DynamicEntity*>::iterator dynEnt = activeDynamicEntities.begin();

	while (dynEnt != activeDynamicEntities.end()) {

		if ((*dynEnt)->isRemove) {

			delete *dynEnt;
			activeDynamicEntities.remove(*dynEnt);

			dynEnt = activeDynamicEntities.begin();
			continue;
		}

		dynEnt++;
	}

	/// Remove static entities
	list<StaticEntity*>::iterator statEnt = activeStaticEntities.begin();

	while (statEnt != activeStaticEntities.end()) {

		if ((*statEnt)->isRemove) {

			delete *statEnt;
			activeStaticEntities.remove(*statEnt);

			statEnt = activeStaticEntities.begin();
			continue;
		}

		statEnt++;
	}

	// Send active entities to blit
	/// Blit dynamic entities
	dynEnt = activeDynamicEntities.begin();

	while (dynEnt != activeDynamicEntities.end()) {

		if (!(*dynEnt)->GetBlitState()) {

			dynEnt++;
			continue;
		}

		switch ((*dynEnt)->dynamicEntityType) {
			// Player
		case EntityType_FOOTMAN:
			(*dynEnt)->Draw(footmanTex);
			break;
		case EntityType_ELVEN_ARCHER:
			(*dynEnt)->Draw(elvenArcherTex);
			break;
		case EntityType_GRYPHON_RIDER:
			(*dynEnt)->Draw(gryphonRiderTex);
			break;
		case EntityType_MAGE:
			(*dynEnt)->Draw(mageTex);
			break;
		case EntityType_PALADIN:
			(*dynEnt)->Draw(paladinTex);
			break;

		case EntityType_TURALYON:
			(*dynEnt)->Draw(turalyonTex);
			break;
		case EntityType_ALLERIA:
			(*dynEnt)->Draw(alleriaTex);
			break;

			// Enemy
		case EntityType_GRUNT:
			(*dynEnt)->Draw(gruntTex);
			break;
		case EntityType_TROLL_AXETHROWER:
			(*dynEnt)->Draw(trollAxethrowerTex);
			break;
		case EntityType_DRAGON:
			(*dynEnt)->Draw(dragonTex);
			break;

			// Neutral
		case EntityType_SHEEP:
		case EntityType_BOAR:
			(*dynEnt)->Draw(crittersTex);
			break;

		default:
			break;
		}

		dynEnt++;
	}

	/// Blit static entities
	statEnt = activeStaticEntities.begin();

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

	if (App->scene->GetAlphaBuilding() != EntityType_NONE) {
		DrawStaticEntityPreview(App->scene->GetAlphaBuilding(), App->player->GetMousePos());
		if (App->scene->GetAlphaBuilding() != EntityType_MAX)
			SDL_SetTextureAlphaMod(App->entities->GetHumanBuildingTexture(), 255);
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

		if ((*dynEnt) != nullptr) {
			delete *dynEnt;
			activeDynamicEntities.remove(*dynEnt++);
			continue;
		}

		dynEnt++;
	}
	activeDynamicEntities.clear();

	/// Remove static entities
	list<StaticEntity*>::const_iterator statEnt = activeStaticEntities.begin();

	while (statEnt != activeStaticEntities.end()) {

		if ((*statEnt) != nullptr) {
			delete *statEnt;
			activeStaticEntities.remove(*statEnt++);
			continue;
		}

		statEnt++;
	}
	activeStaticEntities.clear();

	// Remove to spawn entities
	list<Entity*>::const_iterator it = toSpawnEntities.begin();

	while (it != toSpawnEntities.end()) {

		if ((*it) != nullptr) {
			delete *it;
			toSpawnEntities.remove(*it++);
			continue;
		}

		it++;
	}
	toSpawnEntities.clear();

	// Remove units selected
	list<DynamicEntity*>::const_iterator unitSel = unitsSelected.begin();
	unitsSelected.clear();

	// Free all textures
	if (footmanTex != nullptr)
		App->tex->UnLoad(footmanTex);

	if (gruntTex != nullptr)
		App->tex->UnLoad(gruntTex);

	if (elvenArcherTex != nullptr)
		App->tex->UnLoad(elvenArcherTex);

	if (gryphonRiderTex != nullptr)
		App->tex->UnLoad(gryphonRiderTex);

	if (paladinTex != nullptr)
		App->tex->UnLoad(paladinTex);

	if (alleriaTex != nullptr)
		App->tex->UnLoad(alleriaTex);

	if (khadgarTex != nullptr)
		App->tex->UnLoad(khadgarTex);

	if (turalyonTex != nullptr)
		App->tex->UnLoad(turalyonTex);

	if (gruntTex != nullptr)
		App->tex->UnLoad(gruntTex);

	if (trollAxethrowerTex != nullptr)
		App->tex->UnLoad(trollAxethrowerTex);

	if (dragonTex != nullptr)
		App->tex->UnLoad(dragonTex);

	if (crittersTex != nullptr)
		App->tex->UnLoad(crittersTex);

	if (humanBuildingsTex != nullptr)
		App->tex->UnLoad(humanBuildingsTex);

	if (orcishBuildingsTex != nullptr)
		App->tex->UnLoad(orcishBuildingsTex);

	if (neutralBuildingsTex != nullptr)
		App->tex->UnLoad(neutralBuildingsTex);

	return ret;
}

Entity* j1EntityFactory::AddEntity(ENTITY_TYPE entityType, fPoint pos, const EntityInfo& entityInfo, const UnitInfo& unitInfo, j1Module* listener)
{
	switch (entityType) {

		// STATIC ENTITIES
		// Player static entities
	case EntityType_TOWN_HALL:
	{
		uint maxLife = DetermineBuildingMaxLife(EntityType_NONE, StaticEntitySize_Big);

		TownHall* townHall = new TownHall(pos, { 128,128 }, maxLife, maxLife, (const TownHallInfo&)entityInfo, listener);
		townHall->entityType = EntityCategory_STATIC_ENTITY;
		townHall->staticEntityCategory = StaticEntityCategory_HumanBuilding;
		townHall->staticEntityType = EntityType_TOWN_HALL;
		townHall->entitySide = EntitySide_Player;
		townHall->buildingSize = StaticEntitySize_Big;
		townHall->SetStringLife(townHall->GetCurrLife(), townHall->GetMaxLife());

		toSpawnEntities.push_back((Entity*)townHall);
		return (StaticEntity*)townHall;
	}
	break;

	case EntityType_CHICKEN_FARM:
	{
		uint maxLife = DetermineBuildingMaxLife(EntityType_NONE, StaticEntitySize_Small);

		ChickenFarm* chickenFarm = new ChickenFarm(pos, { 64,64 }, maxLife, maxLife, (const ChickenFarmInfo&)entityInfo, listener);
		chickenFarm->entityType = EntityCategory_STATIC_ENTITY;
		chickenFarm->staticEntityCategory = StaticEntityCategory_HumanBuilding;
		chickenFarm->staticEntityType = EntityType_CHICKEN_FARM;
		chickenFarm->entitySide = EntitySide_Player;
		chickenFarm->buildingSize = StaticEntitySize_Small;
		chickenFarm->SetStringLife(chickenFarm->GetCurrLife(), chickenFarm->GetMaxLife());

		toSpawnEntities.push_back((Entity*)chickenFarm);
		return (StaticEntity*)chickenFarm;
	}
	break;

	case EntityType_BARRACKS:
	{
		uint maxLife = DetermineBuildingMaxLife(EntityType_NONE, StaticEntitySize_Medium);

		Barracks* barracks = new Barracks(pos, { 96,96 }, maxLife, maxLife, (const BarracksInfo&)entityInfo, listener);
		barracks->entityType = EntityCategory_STATIC_ENTITY;
		barracks->staticEntityCategory = StaticEntityCategory_HumanBuilding;
		barracks->staticEntityType = EntityType_BARRACKS;
		barracks->entitySide = EntitySide_Player;
		barracks->buildingSize = StaticEntitySize_Medium;
		barracks->SetStringLife(barracks->GetCurrLife(), barracks->GetMaxLife());

		toSpawnEntities.push_back((Entity*)barracks);
		return (StaticEntity*)barracks;
	}
	break;

	case EntityType_ELVEN_LUMBER_MILL:
	{
		uint maxLife = DetermineBuildingMaxLife(EntityType_NONE, StaticEntitySize_Medium);

		ElvenLumberMill* elvenLumberMill = new ElvenLumberMill(pos, { 96,96 }, maxLife, maxLife, (const ElvenLumberMillInfo&)entityInfo, listener);
		elvenLumberMill->entityType = EntityCategory_STATIC_ENTITY;
		elvenLumberMill->staticEntityCategory = StaticEntityCategory_HumanBuilding;
		elvenLumberMill->staticEntityType = EntityType_BARRACKS;
		elvenLumberMill->entitySide = EntitySide_Player;
		elvenLumberMill->buildingSize = StaticEntitySize_Medium;
		elvenLumberMill->SetStringLife(elvenLumberMill->GetCurrLife(), elvenLumberMill->GetMaxLife());

		toSpawnEntities.push_back((Entity*)elvenLumberMill);
		return (StaticEntity*)elvenLumberMill;
	}
	break;

	case EntityType_MAGE_TOWER:
	{
		uint maxLife = DetermineBuildingMaxLife(EntityType_NONE, StaticEntitySize_Medium);

		MageTower* mageTower = new MageTower(pos, { 96,96 }, maxLife, maxLife, (const MageTowerInfo&)entityInfo, listener);
		mageTower->entityType = EntityCategory_STATIC_ENTITY;
		mageTower->staticEntityCategory = StaticEntityCategory_HumanBuilding;
		mageTower->staticEntityType = EntityType_MAGE_TOWER;
		mageTower->entitySide = EntitySide_Player;
		mageTower->buildingSize = StaticEntitySize_Medium;
		mageTower->SetStringLife(mageTower->GetCurrLife(), mageTower->GetMaxLife());

		toSpawnEntities.push_back((Entity*)mageTower);
		return (StaticEntity*)mageTower;
	}
	break;

	case EntityType_GRYPHON_AVIARY:
	{
		uint maxLife = DetermineBuildingMaxLife(EntityType_NONE, StaticEntitySize_Medium);

		GryphonAviary* gryphonAviary = new GryphonAviary(pos, { 96,96 }, maxLife, maxLife, (const GryphonAviaryInfo&)entityInfo, listener);
		gryphonAviary->entityType = EntityCategory_STATIC_ENTITY;
		gryphonAviary->staticEntityCategory = StaticEntityCategory_HumanBuilding;
		gryphonAviary->staticEntityType = EntityType_GRYPHON_AVIARY;
		gryphonAviary->entitySide = EntitySide_Player;
		gryphonAviary->buildingSize = StaticEntitySize_Medium;
		gryphonAviary->SetStringLife(gryphonAviary->GetCurrLife(), gryphonAviary->GetMaxLife());

		toSpawnEntities.push_back((Entity*)gryphonAviary);
		return (StaticEntity*)gryphonAviary;
	}
	break;

	case EntityType_STABLES:
	{
		uint maxLife = DetermineBuildingMaxLife(EntityType_NONE, StaticEntitySize_Medium);

		Stables* stables = new Stables(pos, { 96,96 }, maxLife, maxLife, (const StablesInfo&)entityInfo, listener);
		stables->entityType = EntityCategory_STATIC_ENTITY;
		stables->staticEntityCategory = StaticEntityCategory_HumanBuilding;
		stables->staticEntityType = EntityType_STABLES;
		stables->entitySide = EntitySide_Player;
		stables->buildingSize = StaticEntitySize_Medium;
		stables->SetStringLife(stables->GetCurrLife(), stables->GetMaxLife());

		toSpawnEntities.push_back((Entity*)stables);
		return (StaticEntity*)stables;
	}
	break;

	case EntityType_SCOUT_TOWER:
	{
		uint maxLife = DetermineBuildingMaxLife(EntityType_SCOUT_TOWER, StaticEntitySize_Small);

		ScoutTower* scoutTower = new ScoutTower(pos, { 64,64 }, maxLife, maxLife, (const ScoutTowerInfo&)entityInfo, listener);
		scoutTower->entityType = EntityCategory_STATIC_ENTITY;
		scoutTower->staticEntityCategory = StaticEntityCategory_HumanBuilding;
		scoutTower->staticEntityType = EntityType_SCOUT_TOWER;
		scoutTower->entitySide = EntitySide_Player;
		scoutTower->buildingSize = StaticEntitySize_Small;
		scoutTower->SetStringLife(scoutTower->GetCurrLife(), scoutTower->GetMaxLife());

		toSpawnEntities.push_back((Entity*)scoutTower);
		return (StaticEntity*)scoutTower;
	}
	break;

	case EntityType_PLAYER_GUARD_TOWER:
	{
		uint maxLife = DetermineBuildingMaxLife(EntityType_PLAYER_GUARD_TOWER, StaticEntitySize_Small);

		PlayerGuardTower* playerGuardTower = new PlayerGuardTower(pos, { 64,64 }, maxLife, maxLife, (const PlayerGuardTowerInfo&)entityInfo, listener);
		playerGuardTower->entityType = EntityCategory_STATIC_ENTITY;
		playerGuardTower->staticEntityCategory = StaticEntityCategory_HumanBuilding;
		playerGuardTower->staticEntityType = EntityType_PLAYER_GUARD_TOWER;
		playerGuardTower->entitySide = EntitySide_Player;
		playerGuardTower->buildingSize = StaticEntitySize_Small;
		playerGuardTower->SetStringLife(playerGuardTower->GetCurrLife(), playerGuardTower->GetMaxLife());

		toSpawnEntities.push_back((Entity*)playerGuardTower);
		return (StaticEntity*)playerGuardTower;
	}
	break;

	case EntityType_PLAYER_CANNON_TOWER:
	{
		uint maxLife = DetermineBuildingMaxLife(EntityType_PLAYER_CANNON_TOWER, StaticEntitySize_Small);

		PlayerCannonTower* playerCannonTower = new PlayerCannonTower(pos, { 64,64 }, maxLife, maxLife, (const PlayerCannonTowerInfo&)entityInfo, listener);
		playerCannonTower->entityType = EntityCategory_STATIC_ENTITY;
		playerCannonTower->staticEntityCategory = StaticEntityCategory_HumanBuilding;
		playerCannonTower->staticEntityType = EntityType_PLAYER_CANNON_TOWER;
		playerCannonTower->entitySide = EntitySide_Player;
		playerCannonTower->buildingSize = StaticEntitySize_Small;
		playerCannonTower->SetStringLife(playerCannonTower->GetCurrLife(), playerCannonTower->GetMaxLife());

		toSpawnEntities.push_back((Entity*)playerCannonTower);
		return (StaticEntity*)playerCannonTower;
	}
	break;

	case EntityType_GOLD_MINE:
	{
		GoldMine* goldMine = new GoldMine(pos, { 96,96 }, 0, 0, (const GoldMineInfo&)entityInfo, listener);
		goldMine->entityType = EntityCategory_STATIC_ENTITY;
		goldMine->staticEntityCategory = StaticEntityCategory_NeutralBuilding;
		goldMine->staticEntityType = EntityType_GOLD_MINE;
		goldMine->entitySide = EntitySide_Neutral;
		goldMine->buildingSize = StaticEntitySize_Medium;

		toSpawnEntities.push_back((Entity*)goldMine);
		return (StaticEntity*)goldMine;
	}
	break;

	case EntityType_RUNESTONE:
	{
		Runestone* runestone = new Runestone(pos, { 64,64 }, 0, 0, (const RunestoneInfo&)entityInfo, listener);
		runestone->entityType = EntityCategory_STATIC_ENTITY;
		runestone->staticEntityCategory = StaticEntityCategory_NeutralBuilding;
		runestone->staticEntityType = EntityType_RUNESTONE;
		runestone->entitySide = EntitySide_Neutral;
		runestone->buildingSize = StaticEntitySize_Small;

		toSpawnEntities.push_back((Entity*)runestone);
		return (StaticEntity*)runestone;
	}
	break;

		// Enemy static entities
	case EntityType_GREAT_HALL:
	{
		uint maxLife = DetermineBuildingMaxLife(EntityType_NONE, StaticEntitySize_Big);

		GreatHall* greatHall = new GreatHall(pos, { 128, 128 }, maxLife, maxLife, (const GreatHallInfo&)entityInfo, listener);
		greatHall->entityType = EntityCategory_STATIC_ENTITY;
		greatHall->staticEntityCategory = StaticEntityCategory_OrcishBuilding;
		greatHall->staticEntityType = EntityType_GREAT_HALL;
		greatHall->entitySide = EntitySide_Enemy;
		greatHall->buildingSize = StaticEntitySize_Big;

		toSpawnEntities.push_back((Entity*)greatHall);
		return (StaticEntity*)greatHall;
	}
	break;

	case EntityType_STRONGHOLD:
	{
		uint maxLife = DetermineBuildingMaxLife(EntityType_NONE, StaticEntitySize_Big);

		Stronghold* stronghold = new Stronghold(pos, { 128, 128 }, maxLife, maxLife, (const StrongholdInfo&)entityInfo, listener);
		stronghold->entityType = EntityCategory_STATIC_ENTITY;
		stronghold->staticEntityCategory = StaticEntityCategory_OrcishBuilding;
		stronghold->staticEntityType = EntityType_STRONGHOLD;
		stronghold->entitySide = EntitySide_Enemy;
		stronghold->buildingSize = StaticEntitySize_Big;

		toSpawnEntities.push_back((Entity*)stronghold);
		return (StaticEntity*)stronghold;
	}
	break;

	case EntityType_FORTRESS:
	{
		uint maxLife = DetermineBuildingMaxLife(EntityType_NONE, StaticEntitySize_Big);

		Fortress* fortress = new Fortress(pos, { 128, 128 }, maxLife, maxLife, (const FortressInfo&)entityInfo, listener);
		fortress->entityType = EntityCategory_STATIC_ENTITY;
		fortress->staticEntityCategory = StaticEntityCategory_OrcishBuilding;
		fortress->staticEntityType = EntityType_FORTRESS;
		fortress->entitySide = EntitySide_Enemy;
		fortress->buildingSize = StaticEntitySize_Big;

		toSpawnEntities.push_back((Entity*)fortress);
		return (StaticEntity*)fortress;
	}
	break;

	case EntityType_ENEMY_BARRACKS:
	{
		uint maxLife = DetermineBuildingMaxLife(EntityType_NONE, StaticEntitySize_Medium);

		EnemyBarracks* enemyBarracks = new EnemyBarracks(pos, { 96, 96 }, maxLife, maxLife, (const EnemyBarracksInfo&)entityInfo, listener);
		enemyBarracks->entityType = EntityCategory_STATIC_ENTITY;
		enemyBarracks->staticEntityCategory = StaticEntityCategory_OrcishBuilding;
		enemyBarracks->staticEntityType = EntityType_ENEMY_BARRACKS;
		enemyBarracks->entitySide = EntitySide_Enemy;
		enemyBarracks->buildingSize = StaticEntitySize_Medium;

		toSpawnEntities.push_back((Entity*)enemyBarracks);
		return (StaticEntity*)enemyBarracks;
	}
	break;

	case EntityType_PIG_FARM:
	{
		uint maxLife = DetermineBuildingMaxLife(EntityType_NONE, StaticEntitySize_Small);

		PigFarm* pigFarm = new PigFarm(pos, { 64, 64 }, maxLife, maxLife, (const PigFarmInfo&)entityInfo, listener);
		pigFarm->entityType = EntityCategory_STATIC_ENTITY;
		pigFarm->staticEntityCategory = StaticEntityCategory_OrcishBuilding;
		pigFarm->staticEntityType = EntityType_PIG_FARM;
		pigFarm->entitySide = EntitySide_Enemy;
		pigFarm->buildingSize = StaticEntitySize_Small;

		toSpawnEntities.push_back((Entity*)pigFarm);
		return (StaticEntity*)pigFarm;
	}
	break;

	case EntityType_TROLL_LUMBER_MILL:
	{
		uint maxLife = DetermineBuildingMaxLife(EntityType_NONE, StaticEntitySize_Medium);

		TrollLumberMill* trollLumberMill = new TrollLumberMill(pos, { 96, 96 }, maxLife, maxLife, (const TrollLumberMillInfo&)entityInfo, listener);
		trollLumberMill->entityType = EntityCategory_STATIC_ENTITY;
		trollLumberMill->staticEntityCategory = StaticEntityCategory_OrcishBuilding;
		trollLumberMill->staticEntityType = EntityType_TROLL_LUMBER_MILL;
		trollLumberMill->entitySide = EntitySide_Enemy;
		trollLumberMill->buildingSize = StaticEntitySize_Medium;

		toSpawnEntities.push_back((Entity*)trollLumberMill);
		return (StaticEntity*)trollLumberMill;
	}
	break;

	case EntityType_ALTAR_OF_STORMS:
	{
		uint maxLife = DetermineBuildingMaxLife(EntityType_NONE, StaticEntitySize_Medium);

		AltarOfStorms* altarOfStorms = new AltarOfStorms(pos, { 96, 96 }, maxLife, maxLife, (const AltarOfStormsInfo&)entityInfo, listener);
		altarOfStorms->entityType = EntityCategory_STATIC_ENTITY;
		altarOfStorms->staticEntityCategory = StaticEntityCategory_OrcishBuilding;
		altarOfStorms->staticEntityType = EntityType_ALTAR_OF_STORMS;
		altarOfStorms->entitySide = EntitySide_Enemy;
		altarOfStorms->buildingSize = StaticEntitySize_Medium;

		toSpawnEntities.push_back((Entity*)altarOfStorms);
		return (StaticEntity*)altarOfStorms;
	}
	break;

	case EntityType_DRAGON_ROOST:
	{
		uint maxLife = DetermineBuildingMaxLife(EntityType_NONE, StaticEntitySize_Medium);

		DragonRoost* dragonRoost = new DragonRoost(pos, { 96, 96 }, maxLife, maxLife, (const DragonRoostInfo&)entityInfo, listener);
		dragonRoost->entityType = EntityCategory_STATIC_ENTITY;
		dragonRoost->staticEntityCategory = StaticEntityCategory_OrcishBuilding;
		dragonRoost->staticEntityType = EntityType_DRAGON_ROOST;
		dragonRoost->entitySide = EntitySide_Enemy;
		dragonRoost->buildingSize = StaticEntitySize_Medium;

		toSpawnEntities.push_back((Entity*)dragonRoost);
		return (StaticEntity*)dragonRoost;
	}
	break;

	case EntityType_TEMPLE_OF_THE_DAMNED:
	{
		uint maxLife = DetermineBuildingMaxLife(EntityType_NONE, StaticEntitySize_Medium);

		TempleOfTheDamned* templeOfTheDamned = new TempleOfTheDamned(pos, { 96, 96 }, maxLife, maxLife, (const TempleOfTheDamnedInfo&)entityInfo, listener);
		templeOfTheDamned->entityType = EntityCategory_STATIC_ENTITY;
		templeOfTheDamned->staticEntityCategory = StaticEntityCategory_OrcishBuilding;
		templeOfTheDamned->staticEntityType = EntityType_TEMPLE_OF_THE_DAMNED;
		templeOfTheDamned->entitySide = EntitySide_Enemy;
		templeOfTheDamned->buildingSize = StaticEntitySize_Medium;

		toSpawnEntities.push_back((Entity*)templeOfTheDamned);
		return (StaticEntity*)templeOfTheDamned;
	}
	break;

	case EntityType_OGRE_MOUND:
	{
		uint maxLife = DetermineBuildingMaxLife(EntityType_NONE, StaticEntitySize_Medium);

		OgreMound* ogreMound = new OgreMound(pos, { 96, 96 }, maxLife, maxLife, (const OgreMoundInfo&)entityInfo, listener);
		ogreMound->entityType = EntityCategory_STATIC_ENTITY;
		ogreMound->staticEntityCategory = StaticEntityCategory_OrcishBuilding;
		ogreMound->staticEntityType = EntityType_OGRE_MOUND;
		ogreMound->entitySide = EntitySide_Enemy;
		ogreMound->buildingSize = StaticEntitySize_Medium;

		toSpawnEntities.push_back((Entity*)ogreMound);
		return (StaticEntity*)ogreMound;
	}
	break;

	case EntityType_ENEMY_BLACKSMITH:
	{
		uint maxLife = DetermineBuildingMaxLife(EntityType_NONE, StaticEntitySize_Medium);

		EnemyBlacksmith* enemyBlacksmith = new EnemyBlacksmith(pos, { 96, 96 }, maxLife, maxLife, (const EnemyBlacksmithInfo&)entityInfo, listener);
		enemyBlacksmith->entityType = EntityCategory_STATIC_ENTITY;
		enemyBlacksmith->staticEntityCategory = StaticEntityCategory_OrcishBuilding;
		enemyBlacksmith->staticEntityType = EntityType_ENEMY_BLACKSMITH;
		enemyBlacksmith->entitySide = EntitySide_Enemy;
		enemyBlacksmith->buildingSize = StaticEntitySize_Medium;

		toSpawnEntities.push_back((Entity*)enemyBlacksmith);
		return (StaticEntity*)enemyBlacksmith;
	}
	break;

	case EntityType_WATCH_TOWER:
	{
		uint maxLife = DetermineBuildingMaxLife(EntityType_WATCH_TOWER, StaticEntitySize_Small);

		WatchTower* watchTower = new WatchTower(pos, { 64,64 }, maxLife, maxLife, (const WatchTowerInfo&)entityInfo, listener);
		watchTower->entityType = EntityCategory_STATIC_ENTITY;
		watchTower->staticEntityCategory = StaticEntityCategory_OrcishBuilding;
		watchTower->staticEntityType = EntityType_WATCH_TOWER;
		watchTower->entitySide = EntitySide_Enemy;
		watchTower->buildingSize = StaticEntitySize_Small;

		toSpawnEntities.push_back((Entity*)watchTower);
		return (StaticEntity*)watchTower;
	}
	break;

	case EntityType_ENEMY_GUARD_TOWER:
	{
		uint maxLife = DetermineBuildingMaxLife(EntityType_ENEMY_GUARD_TOWER, StaticEntitySize_Small);

		EnemyGuardTower* enemyGuardTower = new EnemyGuardTower(pos, { 64,64 }, maxLife, maxLife, (const EnemyGuardTowerInfo&)entityInfo, listener);
		enemyGuardTower->entityType = EntityCategory_STATIC_ENTITY;
		enemyGuardTower->staticEntityCategory = StaticEntityCategory_OrcishBuilding;
		enemyGuardTower->staticEntityType = EntityType_ENEMY_GUARD_TOWER;
		enemyGuardTower->entitySide = EntitySide_Enemy;
		enemyGuardTower->buildingSize = StaticEntitySize_Small;

		toSpawnEntities.push_back((Entity*)enemyGuardTower);
		return (StaticEntity*)enemyGuardTower;
	}
	break;

	case EntityType_ENEMY_CANNON_TOWER:
	{
		uint maxLife = DetermineBuildingMaxLife(EntityType_ENEMY_CANNON_TOWER, StaticEntitySize_Small);

		EnemyCannonTower* enemyCannonTower = new EnemyCannonTower(pos, { 64,64 }, maxLife, maxLife, (const EnemyCannonTowerInfo&)entityInfo, listener);
		enemyCannonTower->entityType = EntityCategory_STATIC_ENTITY;
		enemyCannonTower->staticEntityCategory = StaticEntityCategory_OrcishBuilding;
		enemyCannonTower->staticEntityType = EntityType_ENEMY_CANNON_TOWER;
		enemyCannonTower->entitySide = EntitySide_Enemy;
		enemyCannonTower->buildingSize = StaticEntitySize_Small;

		toSpawnEntities.push_back((Entity*)enemyCannonTower);
		return (StaticEntity*)enemyCannonTower;
	}
	break;

		// DYNAMIC ENTITIES
		// Player dynamic entities
	case EntityType_FOOTMAN:
	{
		Footman* footman = new Footman(pos, footmanInfo.unitInfo.size, footmanInfo.unitInfo.currLife, footmanInfo.unitInfo.maxLife, unitInfo, (const FootmanInfo&)entityInfo, listener);
		footman->entityType = EntityCategory_DYNAMIC_ENTITY;
		footman->dynamicEntityType = EntityType_FOOTMAN;
		footman->entitySide = EntitySide_Player;
		footman->SetStringLife(footman->GetCurrLife(), footman->GetMaxLife());

		toSpawnEntities.push_back((Entity*)footman);
		return (DynamicEntity*)footman;
	}
	break;

	case EntityType_ELVEN_ARCHER:
	{
		ElvenArcher* elvenArcher = new ElvenArcher(pos, elvenArcherInfo.unitInfo.size, elvenArcherInfo.unitInfo.currLife, elvenArcherInfo.unitInfo.maxLife, unitInfo, (const ElvenArcherInfo&)entityInfo, listener);
		elvenArcher->entityType = EntityCategory_DYNAMIC_ENTITY;
		elvenArcher->dynamicEntityType = EntityType_ELVEN_ARCHER;
		elvenArcher->entitySide = EntitySide_Player;
		elvenArcher->SetStringLife(elvenArcher->GetCurrLife(), elvenArcher->GetMaxLife());

		toSpawnEntities.push_back((Entity*)elvenArcher);
		return (DynamicEntity*)elvenArcher;
	}
	break;

	case EntityType_GRYPHON_RIDER:
	{
		GryphonRider* gryphonRider = new GryphonRider(pos, gryphonRiderInfo.unitInfo.size, gryphonRiderInfo.unitInfo.currLife, gryphonRiderInfo.unitInfo.maxLife, unitInfo, (const GryphonRiderInfo&)entityInfo, listener);
		gryphonRider->entityType = EntityCategory_DYNAMIC_ENTITY;
		gryphonRider->dynamicEntityType = EntityType_GRYPHON_RIDER;
		gryphonRider->entitySide = EntitySide_Player;
		gryphonRider->SetStringLife(gryphonRider->GetCurrLife(), gryphonRider->GetMaxLife());

		toSpawnEntities.push_back((Entity*)gryphonRider);
		return (DynamicEntity*)gryphonRider;
	}
	break;

	case EntityType_MAGE:
	{

	}
	break;

	case EntityType_PALADIN:
	{

	}
	break;

	case EntityType_TURALYON:
	{
		Turalyon* turalyon = new Turalyon(pos, turalyonInfo.unitInfo.size, turalyonInfo.unitInfo.currLife, turalyonInfo.unitInfo.maxLife, unitInfo, (const TuralyonInfo&)entityInfo, listener);
		turalyon->entityType = EntityCategory_DYNAMIC_ENTITY;
		turalyon->dynamicEntityType = EntityType_TURALYON;
		turalyon->entitySide = EntitySide_NoSide;

		toSpawnEntities.push_back((Entity*)turalyon);
		return (DynamicEntity*)turalyon;
	}
	break;

	case EntityType_ALLERIA:
	{
		Alleria* alleria = new Alleria(pos, alleriaInfo.unitInfo.size, gruntInfo.unitInfo.currLife, gruntInfo.unitInfo.maxLife, unitInfo, (const AlleriaInfo&)entityInfo, listener);
		alleria->entityType = EntityCategory_DYNAMIC_ENTITY;
		alleria->dynamicEntityType = EntityType_ALLERIA;
		alleria->entitySide = EntitySide_NoSide;

		toSpawnEntities.push_back((Entity*)alleria);
		return (DynamicEntity*)alleria;

	}
	break;

		// Enemy dynamic entities
	case EntityType_GRUNT:
	{
		Grunt* grunt = new Grunt(pos, gruntInfo.unitInfo.size, gruntInfo.unitInfo.currLife, gruntInfo.unitInfo.maxLife, unitInfo, (const GruntInfo&)entityInfo, listener);
		grunt->entityType = EntityCategory_DYNAMIC_ENTITY;
		grunt->dynamicEntityType = EntityType_GRUNT;
		grunt->entitySide = EntitySide_Enemy;

		toSpawnEntities.push_back((Entity*)grunt);
		return (DynamicEntity*)grunt;
	}
	break;

	case EntityType_TROLL_AXETHROWER:
	{
		TrollAxethrower* trollAxethrower = new TrollAxethrower(pos, trollAxethrowerInfo.unitInfo.size, trollAxethrowerInfo.unitInfo.currLife, trollAxethrowerInfo.unitInfo.maxLife, unitInfo, (const TrollAxethrowerInfo&)entityInfo, listener);
		trollAxethrower->entityType = EntityCategory_DYNAMIC_ENTITY;
		trollAxethrower->dynamicEntityType = EntityType_TROLL_AXETHROWER;
		trollAxethrower->entitySide = EntitySide_Enemy;

		toSpawnEntities.push_back((Entity*)trollAxethrower);
		return (DynamicEntity*)trollAxethrower;
	}
	break;

	case EntityType_DRAGON:
	{
		Dragon* dragon = new Dragon(pos, dragonInfo.unitInfo.size, gruntInfo.unitInfo.currLife, gruntInfo.unitInfo.maxLife, unitInfo, (const DragonInfo&)entityInfo, listener);
		dragon->entityType = EntityCategory_DYNAMIC_ENTITY;
		dragon->dynamicEntityType = EntityType_DRAGON;
		dragon->entitySide = EntitySide_Enemy;

		toSpawnEntities.push_back((Entity*)dragon);
		return (DynamicEntity*)dragon;
	}
	break;

		// Neutral dynamic entities
	case EntityType_SHEEP:
	{
		CritterSheep* critterSheep = new CritterSheep(pos, critterSheepInfo.unitInfo.size, critterSheepInfo.unitInfo.currLife, critterSheepInfo.unitInfo.maxLife, unitInfo, (const CritterSheepInfo&)entityInfo, listener);
		critterSheep->entityType = EntityCategory_DYNAMIC_ENTITY;
		critterSheep->dynamicEntityType = EntityType_SHEEP;
		critterSheep->entitySide = EntitySide_Neutral;

		toSpawnEntities.push_back((Entity*)critterSheep);
		return (DynamicEntity*)critterSheep;
	}
	break;

	case EntityType_BOAR:
	{
		CritterBoar* critterBoar = new CritterBoar(pos, critterBoarInfo.unitInfo.size, critterBoarInfo.unitInfo.currLife, critterBoarInfo.unitInfo.maxLife, unitInfo, (const CritterBoarInfo&)entityInfo, listener);
		critterBoar->entityType = EntityCategory_DYNAMIC_ENTITY;
		critterBoar->dynamicEntityType = EntityType_BOAR;
		critterBoar->entitySide = EntitySide_Neutral;

		toSpawnEntities.push_back((Entity*)critterBoar);
		return (DynamicEntity*)critterBoar;
	}
	break;

	default:

		return nullptr;
	}
}

void j1EntityFactory::DestroyStaticEntity(StaticEntity* staticEntity)
{
	activeStaticEntities.remove(staticEntity);

	// TODO: if you want to destroy the entity, the remove method is not enough!
}

//Checks the number of active units on the game 
uint j1EntityFactory::CheckNumberOfEntities(ENTITY_TYPE entityType, ENTITY_CATEGORY entityCategory)
{
	uint numOfEntities = 0;

	if (entityCategory == EntityCategory_DYNAMIC_ENTITY) {

		list<DynamicEntity*>::const_iterator activeDynamic = activeDynamicEntities.begin();

		while (activeDynamic != activeDynamicEntities.end()) {
			if ((*activeDynamic)->dynamicEntityType == entityType)
				numOfEntities++;

			activeDynamic++;
		}
	}
	else if (entityCategory == EntityCategory_STATIC_ENTITY) {

		list<StaticEntity*>::const_iterator activeStatic = activeStaticEntities.begin();

		while (activeStatic != activeStaticEntities.end()) {
			if ((*activeStatic)->staticEntityType == entityType)
				numOfEntities++;

			activeStatic++;
		}
	}
	else
		numOfEntities = 0;

	return numOfEntities;
}

uint j1EntityFactory::GetNumberOfPlayerUnits() const
{
	uint ret = 0;

	list<DynamicEntity*>::const_iterator it = activeDynamicEntities.begin();

	while (it != activeDynamicEntities.end()) {

		if ((*it)->entitySide == EntitySide_Player)
			ret++;
		it++;
	}

	return ret;
}

/// SANDRA
// Returns a pointer to the Entity that is on the tile or nullptr
Entity* j1EntityFactory::IsEntityOnTile(iPoint tile, ENTITY_CATEGORY entityCategory, EntitySide entitySide) const
{
	if (entityCategory == EntityCategory_DYNAMIC_ENTITY || entityCategory == EntityCategory_NONE) {

		if (activeDynamicEntities.size() > 0) {

			list<DynamicEntity*>::const_iterator activeDyn = activeDynamicEntities.begin();

			while (activeDyn != activeDynamicEntities.end()) {

				// The unit cannot be dead
				if (!(*activeDyn)->isDead) {

					iPoint entityTile = App->map->WorldToMap((*activeDyn)->GetPos().x, (*activeDyn)->GetPos().y);

					switch (entitySide) {

					case EntitySide_Player:

						if ((*activeDyn)->entitySide == EntitySide_Player)
							if (tile.x == entityTile.x && tile.y == entityTile.y)
								return (Entity*)(*activeDyn);
						break;

					case EntitySide_Enemy:

						if ((*activeDyn)->entitySide == EntitySide_Enemy)
							if (tile.x == entityTile.x && tile.y == entityTile.y)
								return (Entity*)(*activeDyn);
						break;

					case EntitySide_Neutral:

						if ((*activeDyn)->entitySide == EntitySide_Neutral)
							if (tile.x == entityTile.x && tile.y == entityTile.y)
								return (Entity*)(*activeDyn);
						break;

					case EntitySide_NoSide:

						if (tile.x == entityTile.x && tile.y == entityTile.y)
							return (Entity*)(*activeDyn);
						break;
					}
				}

				activeDyn++;
			}
		}
	}

	if (entityCategory == EntityCategory_STATIC_ENTITY) {

		if (activeStaticEntities.size() > 0) {

			list<StaticEntity*>::const_iterator activeStat = activeStaticEntities.begin();

			while (activeStat != activeStaticEntities.end()) {

				iPoint entityTile = App->map->WorldToMap((*activeStat)->GetPos().x, (*activeStat)->GetPos().y);

				switch (entitySide) {

				case EntitySide_Player:

					if ((*activeStat)->entitySide == EntitySide_Player) {

						if ((*activeStat)->GetSize().x == 64 && (*activeStat)->GetSize().y == 64) { //Small
							for (int i = 0; i < 2; i++) {
								for (int j = 0; j < 2; j++) {
									if (tile.x == entityTile.x + i && tile.y == entityTile.y + j)
										return (Entity*)(*activeStat);
								}
							}
						}
						else if ((*activeStat)->GetSize().x == 96 && (*activeStat)->GetSize().y == 96) { //Medium
							for (int i = 0; i < 3; i++) {
								for (int j = 0; j < 3; j++) {
									if (tile.x == entityTile.x + i && tile.y == entityTile.y + j)
										return (Entity*)(*activeStat);
								}
							}
						}
						else if ((*activeStat)->GetSize().x == 128 && (*activeStat)->GetSize().y == 128) { //Big

							for (int i = 0; i < 4; i++) {
								for (int j = 0; j < 4; j++) {
									if (tile.x == entityTile.x + i && tile.y == entityTile.y + j)
										return (Entity*)(*activeStat);
								}
							}
						}

					}
					break;

				case EntitySide_Enemy:

					if ((*activeStat)->entitySide == EntitySide_Enemy) {

						if ((*activeStat)->GetSize().x == 64 && (*activeStat)->GetSize().y == 64) { //Small
							for (int i = 0; i < 2; i++) {
								for (int j = 0; j < 2; j++) {
									if (tile.x == entityTile.x + i && tile.y == entityTile.y + j)
										return (Entity*)(*activeStat);
								}
							}
						}
						else if ((*activeStat)->GetSize().x == 96 && (*activeStat)->GetSize().y == 96) { //Medium
							for (int i = 0; i < 3; i++) {
								for (int j = 0; j < 3; j++) {
									if (tile.x == entityTile.x + i && tile.y == entityTile.y + j)
										return (Entity*)(*activeStat);
								}
							}
						}
						else if ((*activeStat)->GetSize().x == 128 && (*activeStat)->GetSize().y == 128) { //Big

							for (int i = 0; i < 4; i++) {
								for (int j = 0; j < 4; j++) {
									if (tile.x == entityTile.x + i && tile.y == entityTile.y + j)
										return (Entity*)(*activeStat);
								}
							}
						}

					}
					break;

				case EntitySide_Neutral:

					if ((*activeStat)->entitySide == EntitySide_Neutral) {

						if ((*activeStat)->GetSize().x == 64 && (*activeStat)->GetSize().y == 64) { //Small
							for (int i = 0; i < 2; i++) {
								for (int j = 0; j < 2; j++) {
									if (tile.x == entityTile.x + i && tile.y == entityTile.y + j)
										return (Entity*)(*activeStat);
								}
							}
						}
						else if ((*activeStat)->GetSize().x == 96 && (*activeStat)->GetSize().y == 96) { //Medium
							for (int i = 0; i < 3; i++) {
								for (int j = 0; j < 3; j++) {
									if (tile.x == entityTile.x + i && tile.y == entityTile.y + j)
										return (Entity*)(*activeStat);
								}
							}
						}
						else if ((*activeStat)->GetSize().x == 128 && (*activeStat)->GetSize().y == 128) { //Big

							for (int i = 0; i < 4; i++) {
								for (int j = 0; j < 4; j++) {
									if (tile.x == entityTile.x + i && tile.y == entityTile.y + j)
										return (Entity*)(*activeStat);
								}
							}
						}

					}
					break;

				case EntitySide_NoSide:

					if ((*activeStat)->GetSize().x == 64 && (*activeStat)->GetSize().y == 64) { //Small
						for (int i = 0; i < 2; i++) {
							for (int j = 0; j < 2; j++) {
								if (tile.x == entityTile.x + i && tile.y == entityTile.y + j)
									return (Entity*)(*activeStat);
							}
						}
					}
					else if ((*activeStat)->GetSize().x == 96 && (*activeStat)->GetSize().y == 96) { //Medium
						for (int i = 0; i < 3; i++) {
							for (int j = 0; j < 3; j++) {
								if (tile.x == entityTile.x + i && tile.y == entityTile.y + j)
									return (Entity*)(*activeStat);
							}
						}
					}
					else if ((*activeStat)->GetSize().x == 128 && (*activeStat)->GetSize().y == 128) { //Big

						for (int i = 0; i < 4; i++) {
							for (int j = 0; j < 4; j++) {
								if (tile.x == entityTile.x + i && tile.y == entityTile.y + j)
									return (Entity*)(*activeStat);
							}
						}
					}
					break;
				}
				activeStat++;
			}
		}
	}

	// We do also need to check the toSpawn list (just in case)
	/*
	list<Entity*>::const_iterator toSpawn = toSpawnEntities.begin();

	while (toSpawn != toSpawnEntities.end()) {

		iPoint entityTile = App->map->WorldToMap((*toSpawn)->GetPos().x, (*toSpawn)->GetPos().y);

		switch (entitySide) {

		case EntitySide_Player:

			if ((*toSpawn)->entitySide == EntitySide_Player) {

				if (entityCategory == EntityCategory_NONE ||
					(entityCategory == EntityCategory_DYNAMIC_ENTITY && (*toSpawn)->entityType == EntityCategory_DYNAMIC_ENTITY)
					|| (entityCategory == EntityCategory_STATIC_ENTITY && (*toSpawn)->entityType == EntityCategory_STATIC_ENTITY))

					if ((*toSpawn)->entitySide == EntitySide_Player)
						if (tile.x == entityTile.x && tile.y == entityTile.y)
							return (Entity*)(*toSpawn);
			}
			break;

		case EntitySide_Enemy:

			if (entityCategory == EntityCategory_NONE ||
				(entityCategory == EntityCategory_DYNAMIC_ENTITY && (*toSpawn)->entityType == EntityCategory_DYNAMIC_ENTITY)
				|| (entityCategory == EntityCategory_STATIC_ENTITY && (*toSpawn)->entityType == EntityCategory_STATIC_ENTITY))
				
				if ((*toSpawn)->entitySide == EntitySide_Enemy)
					if (tile.x == entityTile.x && tile.y == entityTile.y)
						return (Entity*)(*toSpawn);
			break;

		case EntitySide_Neutral:

			if (entityCategory == EntityCategory_NONE ||
				(entityCategory == EntityCategory_DYNAMIC_ENTITY && (*toSpawn)->entityType == EntityCategory_DYNAMIC_ENTITY)
				|| (entityCategory == EntityCategory_STATIC_ENTITY && (*toSpawn)->entityType == EntityCategory_STATIC_ENTITY))
				
				if ((*toSpawn)->entitySide == EntitySide_Neutral)
					if (tile.x == entityTile.x && tile.y == entityTile.y)
						return (Entity*)(*toSpawn);
			break;

		case EntitySide_NoSide:

			if (entityCategory == EntityCategory_NONE ||
				(entityCategory == EntityCategory_DYNAMIC_ENTITY && (*toSpawn)->entityType == EntityCategory_DYNAMIC_ENTITY)
				|| (entityCategory == EntityCategory_STATIC_ENTITY && (*toSpawn)->entityType == EntityCategory_STATIC_ENTITY))
				
				if (tile.x == entityTile.x && tile.y == entityTile.y)
					return (Entity*)(*toSpawn);
			break;
		}

		toSpawn++;
	}
	*/

	return nullptr;
}

// Selects an Entity
bool j1EntityFactory::SelectEntity(Entity* entity)
{
	bool ret = false;

	if (entity == nullptr)
		return false;

	list<DynamicEntity*>::const_iterator it = activeDynamicEntities.begin();

	while (it != activeDynamicEntities.end()) {

		// Remove entities from the unitsSelected list
		if ((*it) != entity) {
			unitsSelected.remove(GetDynamicEntityByEntity(*it));
			(*it)->isSelected = false;
		}
		it++;
	}

	// If the unit isn't in the unitsSelected list, add it
	if (find(unitsSelected.begin(), unitsSelected.end(), entity) == unitsSelected.end()) {

		DynamicEntity* unit = GetDynamicEntityByEntity(entity);

		if (unit->GetIsValid()) {

			unitsSelected.push_back(unit);
			(entity)->isSelected = true;

			ret = true;
		}
	}

	// TODO: Add StaticEntities

	// Update the color of the selection of all entities (Dynamic and Static)
	//SetUnitsSelectedColor();

	return ret;
}

// Selects the entities within a rectangle
/// TODO:
/// - If units are selected, buildings cannot be selected
/// - If a building is selected, units cannot be selected
/// · Only 1 building can be selected at a time
void j1EntityFactory::SelectEntitiesWithinRectangle(SDL_Rect rectangleRect, ENTITY_CATEGORY entityCategory, EntitySide entitySide)
{
	list<DynamicEntity*>::const_iterator it = activeDynamicEntities.begin();

	while (it != activeDynamicEntities.end()) {

		// The unit cannot be dead
		if ((*it)->isDead) {

			it++;
			continue;
		}

		if (entitySide == EntitySide_NoSide
			|| (entitySide == EntitySide_Player && (*it)->entitySide == EntitySide_Player)
			|| (entitySide == EntitySide_Enemy && (*it)->entitySide == EntitySide_Enemy)) {

			if (entityCategory == EntityCategory_NONE
				|| (entityCategory == EntityCategory_DYNAMIC_ENTITY && (*it)->entityType == EntityCategory_DYNAMIC_ENTITY)) {

				SDL_Rect entityRect = { (*it)->GetPos().x, (*it)->GetPos().y, (*it)->GetSize().x, (*it)->GetSize().y };

				// If the unit is within the selection:
				if (SDL_HasIntersection(&entityRect, &rectangleRect)) {

					// It there are less units than MAX_UNITS_SELECTED selected:
					if (unitsSelected.size() < MAX_UNITS_SELECTED) {

						// If the unit isn't in the unitsSelected list, add it
						if (find(unitsSelected.begin(), unitsSelected.end(), *it) == unitsSelected.end()) {

							if ((*it)->GetIsValid()) {

								unitsSelected.push_back(GetDynamicEntityByEntity(*it));
								(*it)->isSelected = true;
							}
						}
					}
				}

				else {

					// If the unit is in the unitsSelected list, remove it
					if (find(unitsSelected.begin(), unitsSelected.end(), *it) != unitsSelected.end()) {
						unitsSelected.remove(GetDynamicEntityByEntity(*it));
						(*it)->isSelected = false;
					}
				}
			}
		}
		else {

			// If the unit is in the unitsSelected list, remove it
			if (find(unitsSelected.begin(), unitsSelected.end(), *it) != unitsSelected.end()) {
				unitsSelected.remove(GetDynamicEntityByEntity(*it));
				(*it)->isSelected = false;
			}
		}

		it++;
	}

	// TODO: Add StaticEntities

	// Update the color of the selection of all entities (Dynamic and Static)
	//SetUnitsSelectedColor();


}

// Unselects all entities
void j1EntityFactory::UnselectAllEntities()
{
	list<DynamicEntity*>::const_iterator it = activeDynamicEntities.begin();

	while (it != activeDynamicEntities.end()) {

		if ((*it)->isSelected) {

			(*it)->isSelected = false;

			// If the entity is in the unitsSelected list, remove it
			if (find(unitsSelected.begin(), unitsSelected.end(), *it) != unitsSelected.end())
				unitsSelected.remove(GetDynamicEntityByEntity(*it));
		}

		it++;
	}
}

// Returns a pointer to the DynamicEntity of an Entity
DynamicEntity* j1EntityFactory::GetDynamicEntityByEntity(Entity* entity) const
{
	if (entity->entityType == EntityCategory_DYNAMIC_ENTITY)
		return (DynamicEntity*)entity;

	return nullptr;
}

// Returns a list with the last units selected
list<DynamicEntity*> j1EntityFactory::GetLastUnitsSelected() const
{
	return unitsSelected;
}

bool j1EntityFactory::RemoveUnitFromUnitsSelected(Entity* entity)
{
	bool ret = false;

	list<DynamicEntity*>::const_iterator it = find(unitsSelected.begin(), unitsSelected.end(), entity);

	if (it != unitsSelected.end()) {

		(*it)->isSelected = false;

		unitsSelected.remove(*it);
		ret = true;
	}

	return ret;
}

// Updates the selection color of all entities
void j1EntityFactory::SetUnitsSelectedColor()
{
	//SDL_Color colors[10] = { ColorYellow, ColorDarkGreen, ColorBrightBlue, ColorOrange, ColorPink, ColorPurple, ColorGrey, ColorBlack, ColorOlive, ColorViolet };
	//string colorNames[10] = { "Yellow", "DarkGreen", "BrightBlue", "Orange", "Pink", "Purple", "Grey", "Black", "Olive", "Violet" };

	list<DynamicEntity*>::const_iterator it = activeDynamicEntities.begin();
	//uint i = 0;

	while (it != activeDynamicEntities.end())
	{
		// If the unit is selected, change its color
		if ((*it)->isSelected) {

			//GetDynamicEntityByEntity(*it)->SetColor(colors[i], colorNames[i]);
			//i++;
		}
		else {

			// If the unit is no longer selected, change its color to default white
			GetDynamicEntityByEntity(*it)->SetColor(ColorWhite, "White");
		}

		it++;
	}
}

bool j1EntityFactory::CommandToUnits(list<DynamicEntity*> units, UnitCommand unitCommand)
{
	bool ret = false;

	list<DynamicEntity*>::const_iterator it = units.begin();

	while (it != units.end()) {

		if ((*it)->GetIsValid()) {

			if ((*it)->SetUnitCommand(unitCommand))
				ret = true;
		}

		it++;
	}

	return ret;
}

bool j1EntityFactory::RemoveAllUnitsGoals(list<DynamicEntity*> units)
{
	bool ret = false;

	list<DynamicEntity*>::const_iterator it = units.begin();

	while (it != units.end()) {

		(*it)->GetBrain()->RemoveAllSubgoals();
		ret = true;

		it++;
	}

	return ret;
}

// Attack
bool j1EntityFactory::InvalidateTargetInfo(Entity* target)
{
	if (target == nullptr)
		return false;

	list<DynamicEntity*>::const_iterator dynEnt = activeDynamicEntities.begin();

	while (dynEnt != activeDynamicEntities.end()) {

		(*dynEnt)->SetIsRemovedTargetInfo(target);
		(*dynEnt)->RemoveAttackingUnit(target);

		dynEnt++;
	}

	list<StaticEntity*>::const_iterator statEnt = activeStaticEntities.begin();

	while (statEnt != activeStaticEntities.end()) {

		(*statEnt)->RemoveAttackingUnit(target);

		statEnt++;
	}

	return true;
}

// Movement
void j1EntityFactory::InvalidateMovementEntity(Entity* entity)
{
	list<DynamicEntity*>::const_iterator it = activeDynamicEntities.begin();

	while (it != activeDynamicEntities.end()) {

		if (!(*it)->isDead) {

			if ((*it)->GetSingleUnit()->waitUnit != nullptr) {

				// The dead entity was the waitUnit of another entity
				if ((*it)->GetSingleUnit()->waitUnit->unit == entity) {

					(*it)->GetSingleUnit()->ResetUnitCollisionParameters();
					App->pathmanager->UnRegister((*it)->GetSingleUnit()->unit->GetPathPlanner());
				}
			}
		}

		it++;
	}
}

// Entities
Entity* j1EntityFactory::IsEntityUnderMouse(iPoint mousePos, ENTITY_CATEGORY entityCategory, EntitySide entitySide) const
{
	// DYNAMIC ENTITIES
	if (entityCategory == EntityCategory_DYNAMIC_ENTITY || entityCategory == EntityCategory_NONE) {

		if (activeDynamicEntities.size() > 0) {

			list<DynamicEntity*>::const_iterator activeDyn = activeDynamicEntities.begin();

			while (activeDyn != activeDynamicEntities.end()) {

				// The unit cannot be dead
				if (!(*activeDyn)->isDead) {

					// An offset value is applied ONLY to the units selection
					iPoint offsetValue = { 20,20 };

					iPoint entityPos = { (int)(*activeDyn)->GetPos().x + (*activeDyn)->GetOffsetSize().x - offsetValue.x, (int)(*activeDyn)->GetPos().y + (*activeDyn)->GetOffsetSize().y - offsetValue.y };
					iPoint entitySize = { (*activeDyn)->GetSize().x + offsetValue.x, (*activeDyn)->GetSize().y + offsetValue.y };
					uint scale = App->win->GetScale();

					switch (entitySide) {

					case EntitySide_Player:

						if ((*activeDyn)->entitySide == EntitySide_Player)
							if (mousePos.x > entityPos.x / scale && mousePos.x < entityPos.x / scale + entitySize.x && mousePos.y > entityPos.y / scale && mousePos.y < entityPos.y / scale + entitySize.y)
								return (Entity*)(*activeDyn);
						break;

					case EntitySide_Enemy:

						if ((*activeDyn)->entitySide == EntitySide_Enemy)
							if (mousePos.x > entityPos.x / scale && mousePos.x < entityPos.x / scale + entitySize.x && mousePos.y > entityPos.y / scale && mousePos.y < entityPos.y / scale + entitySize.y)
								return (Entity*)(*activeDyn);
						break;

					case EntitySide_Neutral:

						if ((*activeDyn)->entitySide == EntitySide_Neutral)
							if (mousePos.x > entityPos.x / scale && mousePos.x < entityPos.x / scale + entitySize.x && mousePos.y > entityPos.y / scale && mousePos.y < entityPos.y / scale + entitySize.y)
								return (Entity*)(*activeDyn);
						break;

					case EntitySide_NoSide:

						if (mousePos.x > entityPos.x / scale && mousePos.x < entityPos.x / scale + entitySize.x && mousePos.y > entityPos.y / scale && mousePos.y < entityPos.y / scale + entitySize.y)
							return (Entity*)(*activeDyn);
						break;
					}
				}
				activeDyn++;
			}
		}
	}

	// STATIC ENTITIES
	if (entityCategory == EntityCategory_STATIC_ENTITY) {

		if (activeStaticEntities.size() > 0) {

			list<StaticEntity*>::const_iterator activeStat = activeStaticEntities.begin();

			while (activeStat != activeStaticEntities.end()) {

				iPoint entityPos = { (int)(*activeStat)->GetPos().x, (int)(*activeStat)->GetPos().y };
				iPoint entitySize = { (*activeStat)->GetSize().x, (*activeStat)->GetSize().y };
				uint scale = App->win->GetScale();

				switch (entitySide) {

				case EntitySide_Player:

					if ((*activeStat)->entitySide == EntitySide_Player)
						if (mousePos.x > entityPos.x / scale && mousePos.x < entityPos.x / scale + entitySize.x && mousePos.y > entityPos.y / scale && mousePos.y < entityPos.y / scale + entitySize.y)
							return (Entity*)(*activeStat);
					break;

				case EntitySide_Enemy:

					if ((*activeStat)->entitySide == EntitySide_Enemy)
						if (mousePos.x > entityPos.x / scale && mousePos.x < entityPos.x / scale + entitySize.x && mousePos.y > entityPos.y / scale && mousePos.y < entityPos.y / scale + entitySize.y)
							return (Entity*)(*activeStat);
					break;

				case EntitySide_Neutral:

					if ((*activeStat)->entitySide == EntitySide_Neutral)
						if (mousePos.x > entityPos.x / scale && mousePos.x < entityPos.x / scale + entitySize.x && mousePos.y > entityPos.y / scale && mousePos.y < entityPos.y / scale + entitySize.y)
							return (Entity*)(*activeStat);
					break;

				case EntitySide_NoSide:

					if (mousePos.x > entityPos.x / scale && mousePos.x < entityPos.x / scale + entitySize.x && mousePos.y > entityPos.y / scale && mousePos.y < entityPos.y / scale + entitySize.y)
						return (Entity*)(*activeStat);
					break;
				}
				activeStat++;
			}
		}
	}

	return nullptr;
}

void j1EntityFactory::SelectEntitiesOnScreen(ENTITY_TYPE entityType)
{
	UnselectAllEntities();

	list<DynamicEntity*>::const_iterator it = activeDynamicEntities.begin();

	while (it != activeDynamicEntities.end()) {

		if (App->render->IsInScreen((*it)->GetPos())) {

			if (entityType != EntityType_NONE) {

				if ((*it)->dynamicEntityType == entityType && unitsSelected.size() < MAX_UNITS_SELECTED) {

					(*it)->isSelected = true;
					unitsSelected.push_back(*it);
				}
			}
			else {

				if (unitsSelected.size() < MAX_UNITS_SELECTED) {

					(*it)->isSelected = true;
					unitsSelected.push_back(*it);
				}
			}
		}

		it++;
	}
}

// Dynamic Entities
bool j1EntityFactory::IsOnlyThisTypeOfUnits(list<DynamicEntity*> units, ENTITY_TYPE entityType)
{
	if (units.size() == 0)
		return false;

	list<DynamicEntity*>::const_iterator it = units.begin();

	while (it != units.end()) {

		if ((*it)->dynamicEntityType != entityType)
			return false;

		it++;
	}

	return true;
}

bool j1EntityFactory::AreAllUnitsDoingSomething(list<DynamicEntity*> units, UnitState unitState)
{
	if (units.size() == 0)
		return false;

	list<DynamicEntity*>::const_iterator it = units.begin();

	while (it != units.end()) {

		if ((*it)->GetUnitState() != unitState)
			return false;

		it++;
	}

	return true;
}

// Static Entities
bool j1EntityFactory::SelectBuilding(StaticEntity* staticEntity) 
{
	bool ret = false;

	list<StaticEntity*>::const_iterator it = activeStaticEntities.begin();

	while (it != activeStaticEntities.end()) {
	
		if (*it == staticEntity) {

			(*it)->isSelected = true;
			ret = true;
		}
		else if ((*it)->isSelected)

			(*it)->isSelected = false;
	
		it++;
	}

	return ret;
}

void j1EntityFactory::UnselectAllBuildings() 
{
	list<StaticEntity*>::const_iterator it = activeStaticEntities.begin();

	while (it != activeStaticEntities.end()) {

		(*it)->isSelected = false;

		it++;
	}
}

uint j1EntityFactory::DetermineBuildingMaxLife(ENTITY_TYPE buildingType, StaticEntitySize buildingSize)
{
	/// TODO Joan (balancing)
	uint maxLife = 0;

		// Towers
	if (buildingType == EntityType_SCOUT_TOWER || buildingType == EntityType_WATCH_TOWER) {

		maxLife = 200;
		return maxLife;
	}
	else if (buildingType == EntityType_PLAYER_GUARD_TOWER || buildingType == EntityType_ENEMY_GUARD_TOWER) {

		maxLife = 300;
		return maxLife;
	}
	else if (buildingType == EntityType_PLAYER_CANNON_TOWER || buildingType == EntityType_ENEMY_CANNON_TOWER) {

		maxLife = 500;
		return maxLife;
	}

	// The rest of buildings
	switch (buildingSize) {

	case StaticEntitySize_Small:

		maxLife = 200;
		break;

	case StaticEntitySize_Medium:

		maxLife = 400;
		break;

	case StaticEntitySize_Big:

		maxLife = 600;
		break;

	case StaticEntitySize_None:
	default:
		break;
	}

	return maxLife;
}

uint j1EntityFactory::DetermineBuildingGold(ENTITY_TYPE buildingType, StaticEntitySize buildingSize)
{
	/// TODO Joan (balancing)
	uint gold = 0;

	// Towers
	if (buildingType == EntityType_SCOUT_TOWER || buildingType == EntityType_WATCH_TOWER) {

		gold = 200;
		return gold;
	}
	else if (buildingType == EntityType_PLAYER_GUARD_TOWER || buildingType == EntityType_ENEMY_GUARD_TOWER) {

		gold = 250;
		return gold;
	}
	else if (buildingType == EntityType_PLAYER_CANNON_TOWER || buildingType == EntityType_ENEMY_CANNON_TOWER) {

		gold = 300;
		return gold;
	}

	// The rest of buildings
	switch (buildingSize) {

	case StaticEntitySize_Small:

		gold = 150;
		break;

	case StaticEntitySize_Medium:

		gold = 300;
		break;

	case StaticEntitySize_Big:

		gold = 600;
		break;

	case StaticEntitySize_None:
	default:
		break;
	}

	return gold;
}
///_SANDRA

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