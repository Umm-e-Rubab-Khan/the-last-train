#include "logger.h"
#include "util.h"
#include <fstream>
#include <iostream>

using namespace std;

static const char* objectiveNames[NUM_OBJECTIVES] = { "Master Key collected", "Clue digit 4 found", "Clue digit 2 found", "Clue digit 7 found", "Cabinet unlocked", "Train Key acquired", "Fuse collected", "Fuse installed", "Lift Pass collected", "Lift 2 active", "Passenger rescued", "Cat Carrier collected", "Cat secured", "Train prepared", "Mission won", "Mission failed" };

void writeGameSummaryReport(const char* filename, int score, int remainingEnergy, int remainingTime, const bool objectives[NUM_OBJECTIVES], bool victory)
{
    ofstream file(filename);

    if (!file)
    {
        cout << "Error: Could not open summary report file." << endl;
        return;
    }

    file << "GAME SUMMARY REPORT\n";
    file << "Result: " << (victory ? "VICTORY" : "DEFEAT") << endl;
    file << "Final Score: " << score << endl;
    file << "Remaining Energy: " << remainingEnergy << endl;
    file << "Remaining Time: " << remainingTime << endl;

    int completedObjectives = 0;

    file << "\nOBJECTIVES:\n";

    for (int i = 0; i < NUM_OBJECTIVES; i++)
    {
        file << objectiveNames[i] << ": " << (objectives[i] ? "DONE" : "NOT DONE") << endl;

        if (objectives[i])
        {
            completedObjectives++;
        }
    }

    file << "\nObjectives completed: " << completedObjectives << " of " << NUM_OBJECTIVES << endl;
}

bool saveGameStateToFile(const char* filename, int playerFloor, int playerRoom, int energy, int time, int score, const char inventory[MAX_INVENTORY_CAPACITY], int inventoryCount, const bool objectives[NUM_OBJECTIVES], int emergencyTripsLeft, const char roomEntities[NUM_FLOORS][ROOMS_PER_FLOOR][MAX_ENTITIES_PER_ROOM], const int entityCounts[NUM_FLOORS][ROOMS_PER_FLOOR])
{
    ofstream file(filename);

    if (!file)
    {
        cout << "Error: Could not open save game file."<< endl;
        return false;
    }

    file << playerFloor << ' ' << playerRoom << ' ' << energy << ' ' << time << ' ' << score << ' ' << emergencyTripsLeft << endl;

    file << inventoryCount;

    for (int i = 0; i < inventoryCount; i++)
    {
        file << ' ' << inventory[i];
    }

    file << endl;

    for (int i = 0; i < NUM_OBJECTIVES; i++)
    {
        file << (objectives[i] ? 1 : 0);

        if (i < NUM_OBJECTIVES - 1)
        {
            file << ' ';
        }
    }

    file << endl;

    for (int f = 0; f < NUM_FLOORS; f++)
    {
        for (int r = 0; r < ROOMS_PER_FLOOR; r++)
        {
            file << entityCounts[f][r];

            for (int i = 0; i < entityCounts[f][r]; i++)
            {
                file << ' ' << roomEntities[f][r][i];
            }

            file << endl;
        }
    }

    return true;
}

bool loadGameStateFromFile(const char* filename, int& playerFloor, int& playerRoom, int& energy, int& time, int& score, char inventory[MAX_INVENTORY_CAPACITY], int& inventoryCount, bool objectives[NUM_OBJECTIVES], int& emergencyTripsLeft, char roomEntities[NUM_FLOORS][ROOMS_PER_FLOOR][MAX_ENTITIES_PER_ROOM], int entityCounts[NUM_FLOORS][ROOMS_PER_FLOOR])
{
    ifstream file(filename);

    if (!file)
    {
        cout << "Error: Could not open saved game file." << endl;
        return false;
    }

    int tFloor = 0;
    int tRoom = 0;
    int tEnergy = 0;
    int tTime = 0;
    int tScore = 0;
    int tTrips = 0;
    int tInventoryCount = 0;
    char tInventory[MAX_INVENTORY_CAPACITY];
    bool tObjectives[NUM_OBJECTIVES];
    int tCounts[NUM_FLOORS][ROOMS_PER_FLOOR];
    char tEntities[NUM_FLOORS][ROOMS_PER_FLOOR][MAX_ENTITIES_PER_ROOM];

     file >> tFloor >> tRoom >> tEnergy >> tTime >> tScore >> tTrips;

    if (!file)
    {
        return false;
    }

    file >> tInventoryCount;

    if (!file || tInventoryCount < 0 || tInventoryCount > MAX_INVENTORY_CAPACITY)
    {
        return false;
    }

    for (int i = 0; i < tInventoryCount; i++)
    {
        file >> tInventory[i];

        if (!file)
        {
            return false;
        }
    }

    for (int i = 0; i < NUM_OBJECTIVES; i++)
    {
        int value = 0;
        file >> value;

        if (!file)
        {
            return false;
        }

        tObjectives[i] = (value == 1);
    }

    for (int f = 0; f < NUM_FLOORS; f++)
    {
        for (int r = 0; r < ROOMS_PER_FLOOR; r++)
        {
            file >> tCounts[f][r];

            if (!file || tCounts[f][r] < 0 || tCounts[f][r] > MAX_ENTITIES_PER_ROOM)
            {
                return false;
            }

            for (int i = 0; i < tCounts[f][r]; i++)
            {
                file >> tEntities[f][r][i];

                if (!file)
                {
                    return false;
                }
            }

            for (int i = tCounts[f][r]; i < MAX_ENTITIES_PER_ROOM; i++)
            {
                tEntities[f][r][i] = ENT_NONE;
            }
        }
    }

    if (tFloor < 1 || tFloor > NUM_FLOORS || tRoom < 1 || tRoom > ROOMS_PER_FLOOR)
    {
        return false;
    }

    playerFloor = tFloor;
    playerRoom = tRoom;
    energy = tEnergy;
    time = tTime;
    score = tScore;
    emergencyTripsLeft = tTrips;

    inventoryCount = tInventoryCount;
    for (int i = 0; i < tInventoryCount; i++)
    {
        inventory[i] = tInventory[i];
    }

    for (int i = 0; i < NUM_OBJECTIVES; i++)
    {
        objectives[i] = tObjectives[i];
    }

    for (int f = 0; f < NUM_FLOORS; f++)
    {
        for (int r = 0; r < ROOMS_PER_FLOOR; r++)
        {
            entityCounts[f][r] = tCounts[f][r];
            for (int i = 0; i < MAX_ENTITIES_PER_ROOM; i++)
            {
                roomEntities[f][r][i] = tEntities[f][r][i];
            }
        }
    }

    return true;
}