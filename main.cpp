#include <algorithm>
#include <iostream>
#include <sstream>
#include <cstdlib>
#include <pthread.h>
#include <unistd.h>

#include "elevator.h"

using namespace std;

#define NUM_THREADS 17
#define TRAVEL_TIME 3 // In Seconds
#define SIMULATION_TIME 5 // Time to run simulation for

//Story struct to hold 2 lists
// 1 list for people going up
// 1 list for people going down
struct Story {
	int current;
	vector<int> goingUp;
	vector<int> goingDown;
};

// Global variables.
// 1) The state of all elevators at any time
// 2) People waiting at each floor at any time
// 3) Mutex to acqure when printing to terminal
// 4) Duration of program so far.
Elevator allElevators[MAX_ELEVATORS];
Story allStorys[MAX_FLOOR];
static pthread_mutex_t printf_mutex;
int runtime = 0;

// Move elevators in the correct directions till their respective queues are empty
void *moveElevators(void *threadid) {
	long tid;
	int nextFloor;
	int difference;
	tid = (long)threadid;
	//cout << tid << endl;
	while(1) {
		if (allElevators[tid].destFloors.size() != 0) {
			nextFloor = allElevators[tid].destFloors.front();
			difference = abs(allElevators[tid].currFloor - nextFloor);

			if ((nextFloor - allElevators[tid].currFloor) >= 0)
				allElevators[tid].direction = UP;
			else
				allElevators[tid].direction = DOWN;

			for (int i = 0; i < difference; i++) {
				sleep(TRAVEL_TIME);
				(allElevators[tid].direction == DOWN) ? allElevators[tid].currFloor -= 1 : allElevators[tid].currFloor += 1;
			}
			allElevators[tid].dropoff(nextFloor);
			allElevators[tid].currFloor = nextFloor;
		} else {
			//cout << allElevators[tid].currFloor << endl;
			allElevators[tid].direction = STOP;
		}
	} 
	pthread_mutex_lock(&printf_mutex);
	cout << "Thread Created, " << tid << endl;
	pthread_mutex_unlock(&printf_mutex);
	pthread_exit(NULL);
}

// Calculate duration of the program so far
void *duration(void *) {
	while(1) {
		sleep(1);
		++runtime;
	}
	pthread_exit(NULL);
}

//Elevator Scheduler - Assign the elevator based on the shortest time taken to get from
//the elevator's current floor to the start floor if the queue is empty. If the elevators
//queue is not empty, then calculate the distance from the point at which the elevetor's queu
//is emptied after dropping off its last passenger.
int assignElevator(int startFloor, int movingDir) {
	int closestElevator = 11;
	int floorDiff;
	int elevatorChosen = -1;
	int last, lastFloor;
	int i = 0;
	for (i = 0; i < MAX_ELEVATORS; i++) {
		// Picking elevators for passengers who want to go down
		if (movingDir == DOWN) {
			if (allElevators[i].destFloors.size() != 0) {
				last = allElevators[i].destFloors.size();
				lastFloor = allElevators[i].destFloors[last - 1];
			}
			if (allElevators[i].direction == DOWN && allElevators[i].currFloor > startFloor) {
				floorDiff = allElevators[i].currFloor - startFloor;
			} else if (allElevators[i].direction == UP && allElevators[i].currFloor < startFloor && lastFloor < startFloor) {
				floorDiff = abs(allElevators[i].currFloor - startFloor);
			}
			if (floorDiff < closestElevator) {
				closestElevator = floorDiff;
				elevatorChosen = i;
			}
		}
		// Picking elevators for passengers who want to go up
		if (movingDir == UP) {
			if (allElevators[i].destFloors.size() != 0) {
				last = allElevators[i].destFloors.size();
				lastFloor = allElevators[i].destFloors[last - 1];
			}
			if (allElevators[i].direction == UP && allElevators[i].currFloor < startFloor) {
				floorDiff = abs(allElevators[i].currFloor - startFloor);
			} else if (allElevators[i].direction == DOWN && allElevators[i].currFloor > startFloor && lastFloor > startFloor) {
				floorDiff = abs(allElevators[i].currFloor - startFloor);
			}
			if (floorDiff < closestElevator) {
				closestElevator = floorDiff;
				elevatorChosen = i;
			}
		}

		// Check closest stationary elevator
		if (allElevators[i].direction == STOP) {
			floorDiff = abs(allElevators[i].currFloor - startFloor);
			if (floorDiff < closestElevator) {
				closestElevator = floorDiff;
				elevatorChosen = i;
			}
		}
	}

	// If chosen elevator is currently stationary, change the direction immediately
	// so that the same elevator isnt chosen again.
	if (allElevators[elevatorChosen].direction == STOP) {
		if (startFloor > allElevators[elevatorChosen].currFloor)
			allElevators[elevatorChosen].direction = UP;
		else
			allElevators[elevatorChosen].direction = DOWN;
	}
	
	return elevatorChosen; 
}

// Simulate the movement of all 16 elevators for each second. Change the duration for
// the test by changing the SIMULATION_TIME prepocessor directive
void simulate() {
	tuple<int, int, int, vector<int>> elevatorStatus;
	for (int i = 0; i < SIMULATION_TIME; i++) {
		cout << "At " << runtime << " seconds" << endl;
		for (int j = 0; j < MAX_ELEVATORS; j++) {
			elevatorStatus = allElevators[j].status();
			pthread_mutex_lock(&printf_mutex);
			cout << "Elevator: " << j << " is at floor " << get<0>(elevatorStatus) << endl;
			pthread_mutex_unlock(&printf_mutex);
		}
		cout << "" << endl;
		sleep(1);
	}
} 

int main() {
	//Vector for current floor and number of each people at floor
	vector<int> moveUp;
	vector<int> moveDown;
	int destFloor = 0;
	int passengerCount = 0;
	int startFloor = 0;
	int chosenElevator;
	int simulateFloor;
	int simulationTime;

	//Initiaize mutex 
	pthread_mutex_init(&printf_mutex, NULL);

	//Create thread to move the elevators. 1 thread for each elevator
	pthread_t threads[NUM_THREADS];
	long i = 0;
	int rc;
	for(i = 0; i < NUM_THREADS; i++){
		if (i == 16) 
			rc = pthread_create(&threads[i], NULL, duration, NULL);
		else
    		rc = pthread_create(&threads[i], NULL, moveElevators, (void *)i);

    	if (rc){
        	cout << "Error:unable to create thread," << rc << endl;
        	exit(-1);
      	}
   	}

	while(1) {
		moveUp.clear();
		moveDown.clear();
		pthread_mutex_lock(&printf_mutex);
		cout << "Enter floor (0 - 9) to pick people up from (-1 to exit or 10 to simulate for 5 seconds): ";
		pthread_mutex_unlock(&printf_mutex);
		cin >> startFloor;

		// Choose a floor to start picking people up from
		if (startFloor == -1)
			break;
		if (startFloor == 10) {
			pthread_mutex_lock(&printf_mutex);
			cout << "SIMULATION FOR " << SIMULATION_TIME << " seconds!" << endl;
			pthread_mutex_unlock(&printf_mutex);
			simulate();
			continue;
		}

		// Keep entering floors to drop people off at from the pick-up floor
		while (1) {
			pthread_mutex_lock(&printf_mutex);
			cout << "Enter a floor to drop people off at (-1 to exit): ";
			pthread_mutex_unlock(&printf_mutex);
			cin >> destFloor;
			if (destFloor == -1) {
				break;
			} else if (destFloor < 0 || destFloor > 11) {
				cout << "Invalid Input" << endl;
				continue;
			} else if (destFloor > startFloor) {
				moveUp.push_back(destFloor);
			} else {
				moveDown.push_back(destFloor);
			}
		}

		// Assign the floors above and below as queues for the respective floor
		if (moveUp.size() != 0) 
			allStorys[startFloor].goingUp = moveUp;
		if (moveDown.size() != 0) 
			allStorys[startFloor].goingDown = moveDown;

		// Choose an elevator to bring people up.
		if (moveUp.size() != 0) {
			chosenElevator = assignElevator(startFloor, UP);
			allElevators[chosenElevator].destFloors.push_back(startFloor);
			for (uint i = 0; i < moveUp.size(); i++)
				allElevators[chosenElevator].destFloors.push_back(moveUp[i]);
			sort(allElevators[chosenElevator].destFloors.begin(), allElevators[chosenElevator].destFloors.end()); 
			cout << "Elevator to bring people up: " << chosenElevator << endl;
		}
		
		// Choose an elevator to bring people down.
		if (moveDown.size() != 0) {
			chosenElevator = assignElevator(startFloor, DOWN);
			allElevators[chosenElevator].destFloors.push_back(startFloor);
			for (uint i = 0; i < moveDown.size(); i++)
				allElevators[chosenElevator].destFloors.push_back(moveDown[i]);
			sort(allElevators[chosenElevator].destFloors.begin(), allElevators[chosenElevator].destFloors.end());
			reverse(allElevators[chosenElevator].destFloors.begin(), allElevators[chosenElevator].destFloors.end());
			cout << "Elevator to bring people down: " << chosenElevator << endl;
		}
	} 
	
	return 0;
}