// c-style includes
#include <stdio.h>
#include <stdlib.h>


// c++ style includes
#include <string>
#include <iostream>
#include <sstream>
#include <algorithm>
#include <vector>
#include <tuple>

using namespace std;

using std::cerr;
using std::endl;
using std::string;
using std::stringstream;

#define MAX_CAPACITY 10
#define MAX_ELEVATORS 16
#define MAX_FLOOR 10
#define UP 1
#define DOWN -1
#define STOP 0

//Elevator Class
class Elevator {
	public:
		int currFloor; // current floor elevator is on
		int direction; // 1 for up, -1 for down, 0 for stop
		vector<int> destFloors; //min/max heap of all the floors to stop at
		vector<int> peoplePerFloor;
		int capacity; // current number of people in the lift

		Elevator();
		tuple<int, int, int, vector<int>> status();
		void update();
		void pickup(vector<int> peoplePerFloor, int presentFloor, int direction);
		void dropoff(int);
};


