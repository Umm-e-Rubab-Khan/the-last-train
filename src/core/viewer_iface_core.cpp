#include "viewer_iface.h"
#include "level.h"
#include "util.h"
#include <iostream>
#include <iomanip>

using namespace std;
// Terminal UI and dashboard rendering using primitive parameters

static const char* roomNames[NUM_FLOORS][ROOMS_PER_FLOOR] = { { "Entrance Lobby", "Reception", "Waiting Area", "Utility Room" },{ "Lounge", "Archive", "Lab", "Meeting Room" },{ "Stairwell Annex", "Security Vestibule", "Animal Room", "Control Room" },{ "First Aid Station", "Medical Room", "Maintenance Corridor", "Office" },{ "Security Room", "Train Boarding Platform", "Storage Room", "Observation Deck" }};
static const char* objectiveNames[NUM_OBJECTIVES] = { "Master Key collected", "Clue digit 4 found", "Clue digit 2 found", "Clue digit 7 found", "Cabinet unlocked", "Train Key acquired", "Fuse collected", "Fuse installed", "Lift Pass collected", "Lift 2 active", "Passenger rescued", "Cat Carrier collected", "Cat secured", "Train prepared", "Mission won", "Mission failed" };

void renderDashboard(int playerFloor, int playerRoom, int energy, int time, int score, const char inventory[MAX_INVENTORY_CAPACITY], int inventoryCount, int emergencyTripsLeft)
{
    printHeader("STATUS");
    cout << "Location: F" << playerFloor << "-R" << playerRoom << " " << roomNames[playerFloor - 1][playerRoom - 1] << endl;
    cout << "Energy: " << setw(6) << energy << endl << "Time:   " << setw(6) << time << endl << "Score:  " << setw(6) << score << endl << "Emergency trips left: " << setw(6) << emergencyTripsLeft << endl << "Inventory: ";
    if (inventoryCount == 0)
    {
        cout << "empty" << endl;
    }
    else
    {
    cout << endl;
    for (int i = 0; i < inventoryCount; i++)
        {
            cout << inventory[i] << " - " << getEntityName(inventory[i]) << endl;
        }
    }
}
void renderRoomDetails(int floor, int room, const char roomEntities[NUM_FLOORS][ROOMS_PER_FLOOR][MAX_ENTITIES_PER_ROOM], int entityCount)
{
    cout << "Room: " << roomNames[floor - 1][room - 1] << endl;
    cout << "Things here: " << entityCount << endl;
    if (entityCount > 0)
    {
        cout << "Entities: ";
        for (int i = 0; i < entityCount; i++)
        {
            cout << roomEntities[floor - 1][room - 1][i] << " ";
        }
        cout << endl;
    }
    cout << "Something might be hidden in this room. Inspect to look for it." << endl;
}
void renderAsciiBuilding(int playerFloor, int playerRoom)
{   cout << " 1  2  3  4 " << endl;
    for (int floor = NUM_FLOORS; floor >= 1; floor--)
    {
        for (int room = 1; room <= ROOMS_PER_FLOOR; room++)
        {
            if (floor == playerFloor && room == playerRoom)
            {
                cout << "[P]";
            }
            else
            {
                cout << "[ ]";
            }
        }
        cout << " F" << floor << endl;
    }
}
void renderObjectivesChecklist(const bool objectives[NUM_OBJECTIVES])
{
    printHeader("OBJECTIVES");
    int done = 0;
    for (int i = 0; i < NUM_OBJECTIVES; i++)
    {
        if (objectives[i])
        {
            cout << "[X] ";
            done++;
        }
        else
        {
            cout << "[ ] ";
        }
        cout << objectiveNames[i] << endl;
    }
    cout << "Completed : " << done << " / " << NUM_OBJECTIVES << endl;
}