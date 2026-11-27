#include "physics.h"
#include "util.h"

// Movement validation and resource adjustments without structures

void applyResourceDelta(int& energy, int& time, int deltaEnergy, int deltaTime)
{
    energy += deltaEnergy;
    time += deltaTime;
    energy = clampInt(energy, 0, MAX_ENERGY);
    time = clampInt(time, 0, MAX_TIME);
}

void applyScoreDelta(int& score, int deltaScore)
{
    score += deltaScore;
}

bool hasItem(const char inventory[MAX_INVENTORY_CAPACITY], int inventoryCount, char itemSymbol)
{   
    int i=0;
    while(i<inventoryCount)
    {
        if(inventory[i]==itemSymbol)
        {
            return true;
        }
        i++;
    }
    return false;
}

bool addItem(char inventory[MAX_INVENTORY_CAPACITY], int& inventoryCount, char itemSymbol)
{
    if (inventoryCount >= MAX_INVENTORY_CAPACITY)
    {
        return false;
    }
    inventory[inventoryCount] = itemSymbol;
    inventoryCount++;
    return true;
}

bool removeItem(char inventory[MAX_INVENTORY_CAPACITY], int& inventoryCount, char itemSymbol)
{
    for (int i = 0; i < inventoryCount; i++)
    {
        if (inventory[i] == itemSymbol)
        {
            for (int j = i; j < inventoryCount - 1; j++)
            {
                inventory[j] = inventory[j + 1];
            }
            inventoryCount--;
            return true;
        }
    }
    return false;
}

bool canTraverse(int targetFloor, int targetRoom, char connType, const char inventory[MAX_INVENTORY_CAPACITY], int inventoryCount, int emergencyTripsLeft, bool lift2Operational)
{
    if (targetFloor < 1 || targetFloor > NUM_FLOORS)
    {
        return false;
    }
    if (targetRoom < 1 || targetRoom > ROOMS_PER_FLOOR)
    {
        return false;
    }

    switch (connType)
    {
        case CONN_DOOR:
        case CONN_WINDOW:
        case CONN_STAIRS:
        case CONN_LIFT1:
            return true;
        case CONN_LOCKED_DOOR:
            return hasItem(inventory, inventoryCount, ENT_MASTER_KEY);
        case CONN_LIFT2:
        {
            bool validLift2Floor = (targetFloor == 2 || targetFloor == 4 || targetFloor == 5);
            bool liftHasPower = (lift2Operational || emergencyTripsLeft > 0);
            return validLift2Floor && liftHasPower;
        }
        default:
            return false;
    }
}

bool isDirectlyConnected(int fromF, int fromR, int toF, int toR, const char horizontalConn[NUM_FLOORS][ROOMS_PER_FLOOR - 1], char& outConnType)
{
    outConnType = CONN_NONE;

    bool fromValid = (fromF >= 1 && fromF <= NUM_FLOORS && fromR >= 1 && fromR <= ROOMS_PER_FLOOR);
    bool toValid = (toF >= 1 && toF <= NUM_FLOORS && toR >= 1 && toR <= ROOMS_PER_FLOOR);
    if (!fromValid || !toValid)
    {
        return false;
    }

    if (fromF == toF)
    {
        if (toR - fromR != 1 && toR - fromR != -1)
        {
            return false;
        }

        int column = (fromR < toR ? fromR : toR) - 1;
        outConnType = horizontalConn[fromF - 1][column];

        if (outConnType == CONN_NONE || outConnType == CONN_SOLID_WALL)
        {
            return false;
        }
        return true;
    }

    // Stairs
    bool adjacentFloors = (toF - fromF == 1 || fromF - toF == 1);
    bool brokenLink = (fromF == 3 && toF == 4) || (fromF == 4 && toF == 3);
    if (fromR == 1 && toR == 1 && adjacentFloors && !brokenLink)
    {
        outConnType = CONN_STAIRS;
        return true;
    }

    // Lift 1
    bool bothOnLift1Floors = (fromF == 1 || fromF == 2) && (toF == 1 || toF == 2);
    bool bothInLift1Rooms = (fromR == 2 || fromR == 3) && (toR == 2 || toR == 3);
    if (bothOnLift1Floors && bothInLift1Rooms)
    {
        outConnType = CONN_LIFT1;
        return true;
    }
    // Lift 2
    bool bothOnLift2Floors = (fromF == 2 || fromF == 4 || fromF == 5) && (toF == 2 || toF == 4 || toF == 5);
    if (bothOnLift2Floors && fromR == 4 && toR == 4)
    {
        outConnType = CONN_LIFT2;
        return true;
    }

    return false;
}