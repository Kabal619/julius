#include "Event.h"

#include "Building.h"
#include "Calc.h"
#include "Formation.h"
#include "PlayerMessage.h"
#include "Random.h"
#include "Walker.h"

#include "Data/CityInfo.h"
#include "Data/Constants.h"
#include "Data/Empire.h"
#include "Data/Event.h"
#include "Data/Formation.h"
#include "Data/Grid.h"
#include "Data/Invasion.h"
#include "Data/Model.h"
#include "Data/Random.h"
#include "Data/Scenario.h"
#include "Data/Settings.h"
#include "Data/Walker.h"

#include <string.h>

static int getEmpireObjectForInvasion(int pathId, int year);
static int startInvasion(int enemyType, int amount, int invasionPoint, int attackType, int invasionId);

static const int enemyIdToEnemyType[20] = {
	0, 7, 7, 10, 8, 8, 9, 5, 6, 3, 3, 3, 2, 2, 4, 4, 4, 6, 1, 6
};

static const int localUprisingNumEnemies[20] = {
	0, 0, 0, 0, 0, 3, 3, 3, 0, 6, 6, 6, 6, 6, 9, 9, 9, 9, 9, 9
};

static const struct {
	int pctType1;
	int pctType2;
	int pctType3;
	int walkerTypes[3];
	int formationLayout;
} enemyProperties[12] = {
	{100, 0, 0, 49, 0, 0, 8},
	{40, 60, 0, 49, 51, 0, 8},
	{50, 50, 0, 50, 53, 0, 8},
	{80, 20, 0, 50, 48, 0, 8},
	{50, 50, 0, 49, 52, 0, 8},
	{30, 70, 0, 44, 43, 0, 0},
	{50, 50, 0, 44, 43, 0, 10},
	{50, 50, 0, 45, 43, 0, 10},
	{80, 20, 0, 45, 43, 0, 10},
	{80, 20, 0, 44, 46, 0, 11},
	{90, 10, 0, 45, 47, 0, 11},
	{100, 0, 0, 57, 0, 0, 0}
};

void Event_initInvasions()
{
	memset(Data_InvasionWarnings, 0, sizeof(Data_InvasionWarnings));
	int pathCurrent = 1;
	int pathMax = 0;
	for (int i = 0; i < MAX_EMPIRE_OBJECTS; i++) {
		if (Data_Empire_Objects[i].inUse && Data_Empire_Objects[i].type == EmpireObject_BattleIcon) {
			if (Data_Empire_Objects[i].invasionPathId > pathMax) {
				pathMax = Data_Empire_Objects[i].invasionPathId;
			}
		}
	}
	if (pathMax == 0) {
		return;
	}
	struct Data_InvasionWarning *warning = &Data_InvasionWarnings[0];
	for (int i = 0; i < 20; i++) {
		Random_generateNext();
		if (!Data_Scenario.invasions.type[i]) {
			continue;
		}
		Data_Scenario.invasions_month[i] = 2 + (Data_Random.random1_7bit & 7);
		if (Data_Scenario.invasions.type[i] == InvasionType_LocalUprising ||
			Data_Scenario.invasions.type[i] == InvasionType_DistantBattle) {
			continue;
		}
		for (int year = 1; year < 8; year++) {
			int objectId = getEmpireObjectForInvasion(pathCurrent, year);
			if (!objectId) {
				continue;
			}
			struct Data_Empire_Object *obj = &Data_Empire_Objects[objectId];
			warning->inUse = 1;
			warning->empireInvasionPathId = obj->invasionPathId;
			warning->warningYears = obj->invasionYears;
			warning->empireX = obj->x;
			warning->empireY = obj->y;
			warning->empireGraphicId = obj->graphicId;
			warning->invasionId = i;
			warning->empireObjectId = objectId;
			warning->gameMonthNotified = 0;
			warning->gameYearNotified = 0;
			warning->monthsToGo = 12 * Data_Scenario.invasions.year[i];
			warning->monthsToGo += Data_Scenario.invasions_month[i];
			warning->monthsToGo -= 12 * year;
			++warning;
		}
		pathCurrent++;
		if (pathCurrent > pathMax) {
			pathCurrent = 1;
		}
	}
}

static int getEmpireObjectForInvasion(int pathId, int year)
{
	for (int i = 0; i < MAX_EMPIRE_OBJECTS; i++) {
		struct Data_Empire_Object *obj = &Data_Empire_Objects[i];
		if (obj->inUse && obj->type == EmpireObject_BattleIcon &&
			obj->invasionPathId == pathId && obj->invasionYears == year) {
			return i;
		}
	}
	return 0;
}

void Event_handleInvasions()
{
	for (int i = 0; i < MAX_INVASION_WARNINGS; i++) {
		if (!Data_InvasionWarnings[i].inUse) {
			continue;
		}
		// update warnings
		struct Data_InvasionWarning *warning = &Data_InvasionWarnings[i];
		warning->monthsToGo--;
		if (warning->monthsToGo <= 0) {
			if (warning->handled != 1) {
				warning->handled = 1;
				warning->gameYearNotified = Data_CityInfo_Extra.gameTimeYear;
				warning->gameMonthNotified = Data_CityInfo_Extra.gameTimeMonth;
				if (warning->warningYears > 2) {
					PlayerMessage_post(0, 25, 0, 0);
				} else if (warning->warningYears > 1) {
					PlayerMessage_post(0, 26, 0, 0);
				} else {
					PlayerMessage_post(0, 27, 0, 0);
				}
			}
		}
		if (Data_CityInfo_Extra.gameTimeYear >= Data_Scenario.startYear + Data_Scenario.invasions.year[warning->invasionId] &&
			Data_CityInfo_Extra.gameTimeMonth >= Data_Scenario.invasions_month[warning->invasionId]) {
			// invasion attack time has passed
			warning->inUse = 0;
			if (warning->warningYears > 1) {
				continue;
			}
			// enemy invasions
			if (Data_Scenario.invasions.type[warning->invasionId] == InvasionType_EnemyArmy) {
				int gridOffset = startInvasion(
					enemyIdToEnemyType[Data_Scenario.enemyId],
					Data_Scenario.invasions.amount[warning->invasionId],
					Data_Scenario.invasions.from[warning->invasionId],
					Data_Scenario.invasions.attackType[warning->invasionId],
					warning->invasionId);
				if (gridOffset > 0) {
					if (enemyIdToEnemyType[Data_Scenario.enemyId] > 4) {
						PlayerMessage_post(1, 114, Data_Event.lastInternalInvasionId, gridOffset);
					} else {
						PlayerMessage_post(1, 23, Data_Event.lastInternalInvasionId, gridOffset);
					}
				}
			}
			if (Data_Scenario.invasions.type[warning->invasionId] == InvasionType_Caesar) {
				int gridOffset = startInvasion(
					EnemyType_Caesar,
					Data_Scenario.invasions.amount[warning->invasionId],
					Data_Scenario.invasions.from[warning->invasionId],
					Data_Scenario.invasions.attackType[warning->invasionId],
					warning->invasionId);
				if (gridOffset > 0) {
					PlayerMessage_post(1, 24, Data_Event.lastInternalInvasionId, gridOffset);
				}
			}
		}
	}
	// local uprisings
	for (int i = 0; i < 20; i++) {
		if (Data_Scenario.invasions.type[i] == InvasionType_LocalUprising) {
			if (Data_CityInfo_Extra.gameTimeYear == Data_Scenario.startYear + Data_Scenario.invasions.year[i] &&
				Data_CityInfo_Extra.gameTimeMonth >= Data_Scenario.invasions_month[i]) {
				int gridOffset = startInvasion(
					EnemyType_Barbarian,
					Data_Scenario.invasions.amount[i],
					Data_Scenario.invasions.from[i],
					Data_Scenario.invasions.attackType[i],
					i);
				if (gridOffset > 0) {
					PlayerMessage_post(1, 22, Data_Event.lastInternalInvasionId, gridOffset);
				}
			}
		}
	}
}

int Event_startInvasionLocalUprisingFromMars()
{
	if (Data_Settings.saveGameMissionId < 0 || Data_Settings.saveGameMissionId > 19) {
		return 0;
	}
	int amount = localUprisingNumEnemies[Data_Settings.saveGameMissionId];
	if (amount <= 0) {
		return 0;
	}
	int gridOffset = startInvasion(EnemyType_Barbarian, amount, 8, 0, 23);
	if (gridOffset) {
		PlayerMessage_post(1, 121, Data_Event.lastInternalInvasionId, gridOffset);
	}
	return 1;
}

void Event_startInvasionFromCheat()
{
	int gridOffset = startInvasion(enemyIdToEnemyType[Data_Scenario.enemyId], 150, 8, 0, 23);
	if (gridOffset) {
		if (enemyIdToEnemyType[Data_Scenario.enemyId] > 4) {
			PlayerMessage_post(1, 114, Data_Event.lastInternalInvasionId, gridOffset);
		} else {
			PlayerMessage_post(1, 23, Data_Event.lastInternalInvasionId, gridOffset);
		}
	}
}

int Event_existsUpcomingInvasion()
{
	for (int i = 0; i < 101; i++) {
		if (Data_InvasionWarnings[i].inUse && Data_InvasionWarnings[i].handled) {
			return 1;
		}
	}
	return 0;
}

static int startInvasion(int enemyType, int amount, int invasionPoint, int attackType, int invasionId)
{
	if (amount <= 0) {
		return -1;
	}
	int formationsPerType[3];
	int soldiersPerFormation[3][4];
	int x, y;
	int orientation;

	amount = Calc_adjustWithPercentage(amount,
		Data_Model_Difficulty.enemyPercentage[Data_Settings.difficulty]);
	if (amount >= 150) {
		amount = 150;
	}
	Data_Event.lastInternalInvasionId++;
	if (Data_Event.lastInternalInvasionId > 32000) {
		Data_Event.lastInternalInvasionId = 1;
	}
	// calculate soldiers per type
	int numType1 = Calc_adjustWithPercentage(amount, enemyProperties[enemyType].pctType1);
	int numType2 = Calc_adjustWithPercentage(amount, enemyProperties[enemyType].pctType2);
	int numType3 = Calc_adjustWithPercentage(amount, enemyProperties[enemyType].pctType3);
	numType1 = amount - (numType1 + numType2 + numType3) + numType1; // assign leftovers to type1

	for (int t = 0; t < 3; t++) {
		formationsPerType[t] = 0;
		for (int f = 0; f < 4; f++) {
			soldiersPerFormation[t][f] = 0;
		}
	}

	// calculate number of formations
	if (numType1 > 0) {
		if (numType1 <= 16) {
			formationsPerType[0] = 1;
			soldiersPerFormation[0][0] = numType1;
		} else if (numType1 <= 32) {
			formationsPerType[0] = 2;
			soldiersPerFormation[0][1] = numType1 / 2;
			soldiersPerFormation[0][0] = numType1 - numType1 / 2;
		} else {
			formationsPerType[0] = 3;
			soldiersPerFormation[0][2] = numType1 / 3;
			soldiersPerFormation[0][1] = numType1 / 3;
			soldiersPerFormation[0][0] = numType1 - 2 * (numType1 / 3);
		}
	}
	if (numType2 > 0) {
		if (numType2 <= 16) {
			formationsPerType[1] = 1;
			soldiersPerFormation[1][0] = numType2;
		} else if (numType2 <= 32) {
			formationsPerType[1] = 2;
			soldiersPerFormation[1][1] = numType2 / 2;
			soldiersPerFormation[1][0] = numType2 - numType2 / 2;
		} else {
			formationsPerType[1] = 3;
			soldiersPerFormation[1][2] = numType2 / 3;
			soldiersPerFormation[1][1] = numType2 / 3;
			soldiersPerFormation[1][0] = numType2 - 2 * (numType2 / 3);
		}
	}
	if (numType3 > 0) {
		if (numType3 <= 16) {
			formationsPerType[2] = 1;
			soldiersPerFormation[2][0] = numType3;
		} else if (numType3 <= 32) {
			formationsPerType[2] = 2;
			soldiersPerFormation[2][1] = numType3 / 2;
			soldiersPerFormation[2][0] = numType3 - numType3 / 2;
		} else {
			formationsPerType[2] = 3;
			soldiersPerFormation[2][2] = numType3 / 3;
			soldiersPerFormation[2][1] = numType3 / 3;
			soldiersPerFormation[2][0] = numType3 - 2 * (numType3 / 3);
		}
	}
	// determine invasion point
	if (enemyType == EnemyType_Caesar) {
		x = Data_Scenario.entryPoint.x;
		y = Data_Scenario.entryPoint.y;
	} else {
		int numPoints = 0;
		for (int i = 0; i < 8; i++) {
			if (Data_Scenario.invasionPoints.x[i] != -1) {
				numPoints++;
			}
		}
		if (invasionPoint == 8) {
			if (numPoints <= 2) {
				invasionPoint = Data_Random.random1_7bit & 1;
			} else if (numPoints <= 4) {
				invasionPoint = Data_Random.random1_7bit & 3;
			} else {
				invasionPoint = Data_Random.random1_7bit & 7;
			}
		}
		if (numPoints > 0) {
			while (Data_Scenario.invasionPoints.x[invasionPoint] == -1) {
				invasionPoint++;
				if (invasionPoint >= 8) {
					invasionPoint = 0;
				}
			}
		}
		x = Data_Scenario.invasionPoints.x[invasionPoint];
		y = Data_Scenario.invasionPoints.y[invasionPoint];
	}
	if (x == -1 || y == -1) {
		x = Data_Scenario.exitPoint.x;
		y = Data_Scenario.exitPoint.y;
	}
	// determine orientation
	if (!y) {
		orientation = Direction_Bottom;
	} else if (y >= Data_Scenario.mapSizeY - 1) {
		orientation = Direction_Top;
	} else if (!x) {
		orientation = Direction_Right;
	} else if (x >= Data_Scenario.mapSizeX - 1) {
		orientation = Direction_Left;
	} else {
		orientation = Direction_Bottom;
	}
	// check terrain
	int gridOffset = GridOffset(x, y);
	int terrain = Data_Grid_terrain[gridOffset];
	if (terrain & (Terrain_Elevation | Terrain_Rock | Terrain_Tree)) {
		return -1;
	}
	if (terrain & Terrain_Water) {
		if (!(terrain & Terrain_Road)) { // bridge
			return -1;
		}
	} else if (terrain & (Terrain_Building | Terrain_Aqueduct | Terrain_Gatehouse | Terrain_Wall)) {
		Building_destroyByEnemy(x, y, gridOffset);
	}
	// spawn the lot!
	int seq = 0;
	for (int type = 0; type < 3; type++) {
		if (formationsPerType[type] <= 0) {
			continue;
		}
		int walkerType = enemyProperties[enemyType].walkerTypes[type];
		for (int i = 0; i < formationsPerType[type]; i++) {
			int formationId = Formation_create(
				walkerType, enemyProperties[enemyType].formationLayout,
				orientation, x, y);
			if (formationId <= 0) {
				continue;
			}
			struct Data_Formation *f = &Data_Formations[formationId];
			f->attackType = attackType;
			f->orientation = orientation;
			f->enemyType = enemyType;
			f->invasionId = invasionId;
			f->invasionSeq = Data_Event.lastInternalInvasionId;
			for (int w = 0; w < soldiersPerFormation[type][i]; w++) {
				int walkerId = Walker_create(walkerType, x, y, orientation);
				Data_Walkers[walkerId].isFriendly = 0;
				Data_Walkers[walkerId].actionState = WalkerActionState_151_EnemyInitial;
				Data_Walkers[walkerId].waitTicks = 200 * seq + 10 * w + 10;
				Data_Walkers[walkerId].formationId = formationId;
				WalkerName_set(walkerId);
				Data_Walkers[walkerId].isGhost = 1;
			}
			seq++;
		}
	}
	return gridOffset;
}