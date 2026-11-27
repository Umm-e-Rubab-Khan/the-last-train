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
bool loadInitialState(const char* filename, int& playerFloor, int& playerRoom, int& energy, int& time, int& score, char inventory[MAX_INVENTORY_CAPACITY], int& inventoryCount, bool objectives[NUM_OBJECTIVES], int& emergencyTripsLeft)
{
    ifstream file(filename);
    if (!file)
    {
        return false;
    }

    playerFloor = 1;
    playerRoom = 1;
    inventoryCount = 0;
    emergencyTripsLeft = 0;
    (void)inventory;   // the inventory starts empty, nothing to fill in

    char line[MAX_LINE_LEN];
    while (file.getline(line, MAX_LINE_LEN))
    {
        int colon = -1;
        for (int i = 0; line[i] != '\0'; i++)
        {
            if (line[i] == ':')
            {
                colon = i;
                break;
            }
        }
        if (colon == -1)
        {
            continue;
        }
        line[colon] = '\0';
        int v = colon + 1;
        while (line[v] == ' ')
        {
            v++;
        }
        const char* value = line + v;
        int number = 0;
        for (int k = 0; value[k] >= '0' && value[k] <= '9'; k++)
        {
            number = number * 10 + (value[k] - '0');
        }
        bool isYes = stringEquals(value, "YES");
        if (stringEquals(line, "INITIAL_ENERGY"))
        {
            energy = number;
        }
        else if (stringEquals(line, "INITIAL_TIME"))
        {
            time = number;
        }
        else if (stringEquals(line, "INITIAL_SCORE"))
        {
            score = number;
        }
        else if (stringEquals(line, "PASSENGER_RESCUED"))
        {
            objectives[OBJ_PASSENGER_RESCUED] = isYes;
        }
        else if (stringEquals(line, "CAT_RESCUED"))
        {
            objectives[OBJ_CAT_SECURED] = isYes;
        }
        else if (stringEquals(line, "CABINET_UNLOCKED"))
        {
            objectives[OBJ_CABINET_UNLOCKED] = isYes;
        }
        else if (stringEquals(line, "TRAIN_KEY_COLLECTED"))
        {
            objectives[OBJ_TRAIN_KEY] = isYes;
        }
        else if (stringEquals(line, "FUSE_INSTALLED"))
        {
            objectives[OBJ_FUSE_INSTALLED] = isYes;
        }
        else if (stringEquals(line, "LIFT_PASS_COLLECTED"))
        {
            objectives[OBJ_LIFT_PASS] = isYes;
        }
        else if (stringEquals(line, "TRAIN_PREPARED"))
        {
            objectives[OBJ_TRAIN_PREPARED] = isYes;
        }
    }

    energy = clampInt(energy, 0, MAX_ENERGY);
    time = clampInt(time, 0, MAX_TIME);
    return true;
}

bool loadBuildingMap(const char* filename, char horizontalConn[NUM_FLOORS][ROOMS_PER_FLOOR - 1], char roomEntities[NUM_FLOORS][ROOMS_PER_FLOOR][MAX_ENTITIES_PER_ROOM], int entityCounts[NUM_FLOORS][ROOMS_PER_FLOOR])
{
    ifstream file(filename);
    if (!file)
    {
        return false;
    }
    for (int f = 0; f < NUM_FLOORS; f++)
    {
        for (int c = 0; c < ROOMS_PER_FLOOR - 1; c++)
        {
            horizontalConn[f][c] = CONN_NONE;
        }
        for (int r = 0; r < ROOMS_PER_FLOOR; r++)
        {
            entityCounts[f][r] = 0;
        }
    }

    int floorsLoaded = 0;
    char line[MAX_LINE_LEN];
    char below[MAX_LINE_LEN];
    while (file.getline(line, MAX_LINE_LEN))
    {
        int labelCol[ROOMS_PER_FLOOR] = { 0 };
        int labelCount = 0;
        int floorNum = 0;
        for (int i = 0; line[i] != '\0'; i++)
        {
            if (line[i] == 'F' && line[i + 1] >= '1' && line[i + 1] <= '5' && line[i + 2] == '-' && line[i + 3] == 'R')
            {
                if (labelCount < ROOMS_PER_FLOOR)
                {
                    labelCol[labelCount] = i;
                }
                floorNum = line[i + 1] - '0';
                labelCount++;
            }
        }
        if (labelCount != ROOMS_PER_FLOOR)
        {
            continue;
        }
        if (!file.getline(below, MAX_LINE_LEN))
        {
            return false;
        }
        int f = floorNum - 1;
        for (int g = 0; g < ROOMS_PER_FLOOR - 1; g++)
        {
            for (int i = labelCol[g] + 5; i < labelCol[g + 1]; i++)
            {
                char c = line[i];
                if (c == 'D' || c == 'W' || c == '#' || (c == 'L' && line[i + 1] != '1'))
                {
                    horizontalConn[f][g] = c;
                }
            }
        }
        for (int x = 0; below[x] != '\0'; x++)
        {
            if (below[x] == '|' && x >= labelCol[ROOMS_PER_FLOOR - 1])
            {
                break;
            }
            if (below[x] == ' ' || below[x] == '|')
            {
                continue;
            }
            int r = 0;
            for (int j = 1; j < ROOMS_PER_FLOOR; j++)
            {
                if (x >= labelCol[j] - 1)
                {
                    r = j;
                }
            }
            if (entityCounts[f][r] < MAX_ENTITIES_PER_ROOM)
            {
                roomEntities[f][r][entityCounts[f][r]] = below[x];
                entityCounts[f][r]++;
            }
        }
        floorsLoaded++;
    }
    if (entityCounts[0][3] < MAX_ENTITIES_PER_ROOM)
    {
        roomEntities[0][3][entityCounts[0][3]] = ENT_ENERGY_PACK;
        entityCounts[0][3]++;
    }
    return floorsLoaded == NUM_FLOORS;
}