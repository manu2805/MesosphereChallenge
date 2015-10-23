#include "elevator.h"

// Constructor to set elevator to be at the floor 0 and stationary
Elevator::Elevator() {
	this->currFloor = 0;
	this->direction = STOP;
	for (int i = 0; i < MAX_FLOOR; i++)
		peoplePerFloor.push_back(0);
}

//Return a status of the elevator which contains
//Current floor, number of people inside, direction it is going and queue
//of destination floors.
tuple<int, int, int, vector<int>> Elevator::status() {
	return make_tuple(currFloor, capacity, direction, destFloors);
}

// Pickup a passenger.
void Elevator::pickup(vector<int> passengers, int presentFloor, int direction) {
	// Let everyone alight from the lift at the start.
	// So people for current floor is equal to 0.
	int count = destFloors.size();
	if (destFloors[0] == presentFloor && count != 0) {
			peoplePerFloor[presentFloor] = 0;
			destFloors.erase(destFloors.begin());
	}

	// Let people waiting the on floor to get in
	if (direction == UP) {
		for (auto &it : passengers) {
			auto exists = find(destFloors.begin(), destFloors.end(), it);
			if (exists != destFloors.end()) {
				peoplePerFloor[it] += 1;
			} else {
				destFloors.push_back(it);
				make_heap(destFloors.begin(), destFloors.end());
				peoplePerFloor[it] = 1;
			}
		}
		sort_heap(this->destFloors.begin(), this->destFloors.end());
	} else if (direction == DOWN) {
		for (auto &it : passengers) {
			auto exists = find(destFloors.begin(), destFloors.end(), it);
			if (exists != destFloors.end()) {
				peoplePerFloor[it] += 1;
			} else {
				destFloors.push_back(it);
				make_heap(destFloors.begin(), destFloors.end());
				peoplePerFloor[it] = 1;
			}
		}
		sort_heap(destFloors.begin(), destFloors.end());
		reverse(destFloors.begin(), destFloors.end());
	} else {
		for (auto &it : passengers) {
			destFloors.push_back(1);
			peoplePerFloor[it] = 1;
		}
		make_heap(destFloors.begin(), destFloors.end());
		this->direction = direction;
		if (direction == DOWN) reverse(destFloors.begin(), destFloors.end());
	}
	for (auto &it : this->destFloors)
		cout << it << endl;
}

// Stop elevator at the floor that is in front of the destqueue
void Elevator::dropoff(int currFloor) {
	peoplePerFloor[currFloor] = 0;
	destFloors.erase(destFloors.begin());
}

