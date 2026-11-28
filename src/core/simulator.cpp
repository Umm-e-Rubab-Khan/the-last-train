#include "simulator.h"
#include "physics.h"
#include "switches.h"
#include "logger.h"
#include "viewer_iface.h"
#include "level.h"
#include "util.h"
#include <iostream>

using namespace std;

// Modular simulation loop using primitive variables

bool checkEndConditions(int energy, int time, const bool objectives[NUM_OBJECTIVES])
{
    return energy <= 0 || time <= 0 || objectives[OBJ_MISSION_WON] || objectives[OBJ_MISSION_FAILED];
}

bool handleMoveCommand(int targetFloor, int targetRoom, int& currentFloor, int& currentRoom, int& energy, int& time, const char horizontalConn[NUM_FLOORS][ROOMS_PER_FLOOR - 1], const char inventory[MAX_INVENTORY_CAPACITY], int inventoryCount, int& emergencyTripsLeft, bool lift2Operational)
{
    char connType = CONN_NONE;

    if (!isDirectlyConnected(currentFloor, currentRoom, targetFloor, targetRoom, horizontalConn, connType))
    {
        if (connType == CONN_SOLID_WALL)
        {
            cout << "Cannot move there: a solid wall blocks the way.\n";
        }
        else
        {
            cout << "Cannot move there: no direct path between those rooms.\n";
        }
        return false;
    }

    if (!canTraverse(targetFloor, targetRoom, connType, inventory, inventoryCount, emergencyTripsLeft, lift2Operational))
    {
        if (connType == CONN_LIFT1 || connType == CONN_LIFT2)
        {
            cout << "Cannot use the lift right now: it is not operational.\n";
        }
        else
        {
            cout << "Cannot pass through: the door is locked.\n";
        }
        return false;
    }

    int costEnergy = COST_MOVE_ORDINARY_ENERGY;
    int costTime = COST_MOVE_ORDINARY_TIME;

    if (connType == CONN_WINDOW)
    {
        costEnergy = COST_MOVE_WINDOW_ENERGY;
        costTime = COST_MOVE_WINDOW_TIME;
    }
    else if (connType == CONN_LIFT1 || connType == CONN_LIFT2)
    {
        costEnergy = COST_MOVE_LIFT_ENERGY;
        costTime = COST_MOVE_LIFT_TIME;
    }

    if (connType == CONN_LIFT2 && !lift2Operational)
    {
        emergencyTripsLeft--;
    }

    applyResourceDelta(energy, time, costEnergy, costTime);

    currentFloor = targetFloor;
    currentRoom = targetRoom;
    cout << "Moved to F" << currentFloor << "-R" << currentRoom << ".\n";
    return true;
}

bool handleInspectCommand(int currentFloor, int currentRoom, int& energy, int& time, const char roomEntities[NUM_FLOORS][ROOMS_PER_FLOOR][MAX_ENTITIES_PER_ROOM], int entityCount, bool objectives[NUM_OBJECTIVES], int& score)
{
    if (currentFloor < 1 || currentFloor > NUM_FLOORS || currentRoom < 1 || currentRoom > ROOMS_PER_FLOOR)
    {
        return false;
    }

    applyResourceDelta(energy, time, COST_INSPECT_ENERGY, COST_INSPECT_TIME);

    int f = currentFloor - 1;
    int r = currentRoom - 1;

    for (int i = 0; i < entityCount; i++)
    {
        char symbol = roomEntities[f][r][i];
        cout << "[" << symbol << "] " << getEntityName(symbol) << "\n";

        if (symbol == ENT_CLUE_4 && !objectives[OBJ_CLUE_4])
        {
            objectives[OBJ_CLUE_4] = true;
            applyScoreDelta(score, SCORE_CLUE_FOUND);
        }
        else if (symbol == ENT_CLUE_2 && !objectives[OBJ_CLUE_2])
        {
            objectives[OBJ_CLUE_2] = true;
            applyScoreDelta(score, SCORE_CLUE_FOUND);
        }
        else if (symbol == ENT_CLUE_7 && !objectives[OBJ_CLUE_7])
        {
            objectives[OBJ_CLUE_7] = true;
            applyScoreDelta(score, SCORE_CLUE_FOUND);
        }
    }

    if (entityCount == 0)
    {
        cout << "nothing here\n";
    }

    return true;
}

bool handleCollectCommand(char itemSymbol, int currentFloor, int currentRoom, int& energy, int& time, int& score, char roomEntities[NUM_FLOORS][ROOMS_PER_FLOOR][MAX_ENTITIES_PER_ROOM], int& entityCount, char inventory[MAX_INVENTORY_CAPACITY], int& inventoryCount, bool objectives[NUM_OBJECTIVES])
{
    if (currentFloor < 1 || currentFloor > NUM_FLOORS || currentRoom < 1 || currentRoom > ROOMS_PER_FLOOR)
    {
        return false;
    }

    int f = currentFloor - 1;
    int r = currentRoom - 1;

    int idx = -1;
    for (int i = 0; i < entityCount; i++)
    {
        if (roomEntities[f][r][i] == itemSymbol)
        {
            idx = i;
            break;
        }
    }

    if (idx == -1)
    {
        cout << "Item not found in this room.\n";
        return false;
    }

    if (itemSymbol == ENT_CLUE_4 || itemSymbol == ENT_CLUE_2 || itemSymbol == ENT_CLUE_7 || itemSymbol == ENT_CAT || itemSymbol == ENT_PASSENGER || itemSymbol == ENT_CONTROL_PANEL || itemSymbol == ENT_CABINET_TRAIN_KEY)
    {
        cout << "You cannot collect " << getEntityName(itemSymbol) << ".\n";
        return false;
    }

    if (itemSymbol == ENT_ENERGY_PACK)
    {
        applyResourceDelta(energy, time, BONUS_ENERGY_PACK, 0);
    }
    else if (itemSymbol == ENT_EMERGENCY_TOOLKIT)
    {
        applyResourceDelta(energy, time, BONUS_EMERGENCY_TOOLKIT, 0);
    }
    else if (itemSymbol == ENT_OBSERVATION_TOKEN)
    {
        applyResourceDelta(energy, time, 0, BONUS_OBSERVATION_TOKEN);
    }
    else
    {
        if (inventoryCount >= MAX_INVENTORY_CAPACITY)
        {
            cout << "Inventory is full.\n";
            return false;
        }

        applyResourceDelta(energy, time, COST_ITEM_PICKUP_ENERGY, COST_ITEM_PICKUP_TIME);
        inventory[inventoryCount++] = itemSymbol;

        if (itemSymbol == ENT_MASTER_KEY)
        {
            objectives[OBJ_MASTER_KEY] = true;
            applyScoreDelta(score, SCORE_KEY_COLLECT);
        }
        else if (itemSymbol == ENT_FUSE)
        {
            objectives[OBJ_FUSE_COLLECTED] = true;
        }
        else if (itemSymbol == ENT_LIFT_PASS)
        {
            objectives[OBJ_LIFT_PASS] = true;
            applyScoreDelta(score, SCORE_LIFT_PASS);
        }
        else if (itemSymbol == ENT_CAT_CARRIER)
        {
            objectives[OBJ_CAT_CARRIER] = true;
        }
    }

    for (int i = idx; i < entityCount - 1; i++)
    {
        roomEntities[f][r][i] = roomEntities[f][r][i + 1];
    }
    entityCount--;
    roomEntities[f][r][entityCount] = ENT_NONE;

    return true;
}