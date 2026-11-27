#include "util.h"
#include <iostream>
using namespace std;

// Primitive helper utilities, clamping, and screen control
int clampInt(int val, int minVal, int maxVal) 
{
    if (val < minVal) 
    {
        return minVal;
    }
    if (val > maxVal) 
    {
        return maxVal;
    }
    return val;
}
bool stringEquals(const char* a, const char* b)
{
    int i = 0;
    while (a[i] != '\0' && a[i] == b[i])
    {
        i++;
    }
    return a[i] == b[i];
}
void printHeader(const char* title)
{
    for (int i = 0; i < 25; i++) 
    {
    cout << "=";
    }
    cout << endl;
    cout << title << endl;
    for (int i = 0; i < 25; i++) 
    {
    cout << "=";
    }
    cout << endl;
}

void clearScreen()
{
    cout << "\033[2J\033[H";
}