#include "switches.h"
#include "util.h"
#include "physics.h"

// Door lock mechanics, lifts, and cabinet verification without structures

bool canUseLift1(int fromFloor, int toFloor)
{
    return (fromFloor == 1 && toFloor == 2) || (fromFloor == 2 && toFloor == 1);
}

bool canUseLift2(int fromFloor, int toFloor, bool lift2Operational, int emergencyTripsLeft)
{
    return (fromFloor == 2 || fromFloor == 4 || fromFloor == 5) && (toFloor == 2 || toFloor == 4 || toFloor == 5) && (lift2Operational || emergencyTripsLeft > 0);
}

bool unlockFloor3Door(const char inventory[MAX_INVENTORY_CAPACITY], int inventoryCount)
{
    return hasItem(inventory, inventoryCount, ENT_MASTER_KEY);
}

bool installFuseControlPanel(char inventory[MAX_INVENTORY_CAPACITY], int& inventoryCount, bool objectives[NUM_OBJECTIVES], int& score)
{
    if (!hasItem(inventory, inventoryCount, ENT_FUSE))
    {
        return false;
    }
    removeItem(inventory, inventoryCount, ENT_FUSE);
    objectives[OBJ_FUSE_INSTALLED] = true;
    applyScoreDelta(score, SCORE_INSTALL_FUSE);
    return true;
}

bool verifyCabinetCode(const char* enteredCode, const bool objectives[NUM_OBJECTIVES])
{
    return objectives[OBJ_CLUE_4] && objectives[OBJ_CLUE_2] && objectives[OBJ_CLUE_7] && stringEquals(enteredCode, "427");
}