#ifndef UI_BUILDINGINFO_H
#define UI_BUILDINGINFO_H

#include "graphics/tooltip.h"
#include "input/mouse.h"
#include "sound/speech.h"
#include "window/building/common.h"

void UI_BuildingInfo_drawFigureImagesLocal(BuildingInfoContext *c);
void UI_BuildingInfo_drawFigureList(BuildingInfoContext *c);
void UI_BuildingInfo_playFigurePhrase(BuildingInfoContext *c);
void UI_BuildingInfo_handleMouseFigureList(const mouse *m, BuildingInfoContext *c);

void UI_BuildingInfo_drawHouse(BuildingInfoContext *c);

void UI_BuildingInfo_drawWheatFarm(BuildingInfoContext *c);
void UI_BuildingInfo_drawVegetableFarm(BuildingInfoContext *c);
void UI_BuildingInfo_drawFruitFarm(BuildingInfoContext *c);
void UI_BuildingInfo_drawOliveFarm(BuildingInfoContext *c);
void UI_BuildingInfo_drawVinesFarm(BuildingInfoContext *c);
void UI_BuildingInfo_drawPigFarm(BuildingInfoContext *c);

void UI_BuildingInfo_drawMarbleQuarry(BuildingInfoContext *c);
void UI_BuildingInfo_drawIronMine(BuildingInfoContext *c);
void UI_BuildingInfo_drawTimberYard(BuildingInfoContext *c);
void UI_BuildingInfo_drawClayPit(BuildingInfoContext *c);

void UI_BuildingInfo_drawWineWorkshop(BuildingInfoContext *c);
void UI_BuildingInfo_drawOilWorkshop(BuildingInfoContext *c);
void UI_BuildingInfo_drawWeaponsWorkshop(BuildingInfoContext *c);
void UI_BuildingInfo_drawFurnitureWorkshop(BuildingInfoContext *c);
void UI_BuildingInfo_drawPotteryWorkshop(BuildingInfoContext *c);

void UI_BuildingInfo_drawMarket(BuildingInfoContext *c);
void UI_BuildingInfo_drawGranary(BuildingInfoContext *c);
void UI_BuildingInfo_drawGranaryOrders(BuildingInfoContext *c);
void UI_BuildingInfo_drawWarehouse(BuildingInfoContext *c);
void UI_BuildingInfo_drawWarehouseOrders(BuildingInfoContext *c);

void UI_BuildingInfo_drawGranaryForeground(BuildingInfoContext *c);
void UI_BuildingInfo_drawGranaryOrdersForeground(BuildingInfoContext *c);
void UI_BuildingInfo_drawWarehouseForeground(BuildingInfoContext *c);
void UI_BuildingInfo_drawWarehouseOrdersForeground(BuildingInfoContext *c);

void UI_BuildingInfo_handleMouseGranary(const mouse *m, BuildingInfoContext *c);
void UI_BuildingInfo_handleMouseGranaryOrders(const mouse *m, BuildingInfoContext *c);
void UI_BuildingInfo_handleMouseWarehouse(const mouse *m, BuildingInfoContext *c);
void UI_BuildingInfo_handleMouseWarehouseOrders(const mouse *m, BuildingInfoContext *c);

void UI_BuildingInfo_drawEngineersPost(BuildingInfoContext *c);
void UI_BuildingInfo_drawShipyard(BuildingInfoContext *c);
void UI_BuildingInfo_drawDock(BuildingInfoContext *c);
void UI_BuildingInfo_drawWharf(BuildingInfoContext *c);

void UI_BuildingInfo_drawBurningRuin(BuildingInfoContext *c);
void UI_BuildingInfo_drawRubble(BuildingInfoContext *c);

void UI_BuildingInfo_drawAqueduct(BuildingInfoContext *c);
void UI_BuildingInfo_drawReservoir(BuildingInfoContext *c);
void UI_BuildingInfo_drawFountain(BuildingInfoContext *c);
void UI_BuildingInfo_drawWell(BuildingInfoContext *c);

void UI_BuildingInfo_drawWall(BuildingInfoContext *c);
void UI_BuildingInfo_drawPrefect(BuildingInfoContext *c);
void UI_BuildingInfo_drawFort(BuildingInfoContext *c);
void UI_BuildingInfo_drawGatehouse(BuildingInfoContext *c);
void UI_BuildingInfo_drawTower(BuildingInfoContext *c);
void UI_BuildingInfo_drawMilitaryAcademy(BuildingInfoContext *c);
void UI_BuildingInfo_drawBarracks(BuildingInfoContext *c);
void UI_BuildingInfo_drawLegionInfo(BuildingInfoContext *c);
void UI_BuildingInfo_drawLegionInfoForeground(BuildingInfoContext *c);
void UI_BuildingInfo_handleMouseLegionInfo(const mouse *m, BuildingInfoContext *c);
int UI_BuildingInfo_getTooltipLegionInfo(BuildingInfoContext *c);

void UI_BuildingInfo_drawNativeHut(BuildingInfoContext *c);
void UI_BuildingInfo_drawNativeMeeting(BuildingInfoContext *c);
void UI_BuildingInfo_drawNativeCrops(BuildingInfoContext *c);
void UI_BuildingInfo_drawMissionPost(BuildingInfoContext *c);

void UI_BuildingInfo_drawTerrain(BuildingInfoContext *c);
void UI_BuildingInfo_drawNoPeople(BuildingInfoContext *c);

#endif
