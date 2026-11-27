#include "level.h"
#include "util.h"
#include <fstream>

using namespace std;

// Modular implementation using primitive arrays for map parsing and verification

const char* getEntityName(char entitySymbol)
{
    switch (entitySymbol)
    {
        case ENT_MASTER_KEY:
            return "Master Key";
        case ENT_ENERGY_PACK:
            return "Energy Pack";
        case ENT_EMERGENCY_TOOLKIT:
            return "Emergency Toolkit";
        case ENT_CLUE_4:
            return "Clue Digit 4";
        case ENT_FUSE:
            return "Fuse";
        case ENT_CAT:
            return "Cat";
        case ENT_CONTROL_PANEL:
            return "Control Panel";
        case ENT_LIFT_PASS:
            return "Lift Pass";
        case ENT_PASSENGER:
            return "Injured Passenger";
        case ENT_MEDICAL_KIT:
            return "Medical Kit";
        case ENT_EMERGENCY_CARD:
            return "Emergency Access Card";
        case ENT_CLUE_2:
            return "Clue Digit 2";
        case ENT_CLUE_7:
            return "Clue Digit 7";
        case ENT_CABINET_TRAIN_KEY:
            return "Cabinet / Train Key";
        case ENT_CAT_CARRIER:
            return "Cat Carrier";
        case ENT_OBSERVATION_TOKEN:
            return "Observation Token";
        default:
            return "Unknown";
    }
}

bool validateStartingRoom(int startFloor, int startRoom, const char horizontalConn[NUM_FLOORS][ROOMS_PER_FLOOR - 1])
{
    (void)horizontalConn;  
    if (startFloor < 1 || startFloor > NUM_FLOORS || startRoom < 1 || startRoom > ROOMS_PER_FLOOR) { return false; }
    // rooms the player could never leave
    if (startFloor == 3 && startRoom >= 2) { return false; }   // behind the locked door on floor 3
    if (startFloor == 4 && startRoom >= 3) { return false; }   // cut off by the wall on floor 4

    return true;
}
