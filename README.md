# MesosphereChallenge
Software Engineering Intern Coding Challenge

Design and implement an elevator control system. What data structures, interfaces and algorithms will you need? Your elevator control system should be able to handle a few elevators — up to 16.
You can use the language of your choice to implement an elevator control system. In the end, your control system should provide an interface for:

1. Querying the state of the elevators (what floor are they on and where they are going),
2. receiving an update about the status of an elevator,
3. receiving a pickup request,
4. time-stepping the simulation.

## My Design
### Files
* Elevator.h - This file contains the definition of the Elevator class
* Elevator.cpp - This file contains the implementation of the member functions in the Elevetor class
* main.cpp - This file contains the scheduling of the elevators when an elevator is requested on the floor and it does the assignment of the floor to the corresponding elevator
* Makefile - Script for running make

### Build Instructions
To build, just type make in the console. To run the subsequent executable produced, type ./elevator
In the console, you can do 1 of 3 things

1. You can enter a floor number between 0 - 9 to request an elevator to pick you up from that floor. Once you are done entering the floor number for the floor you are currently on, you will then be prompted to enter the floor number for the floor that you want to be dropped off at. To account for the fact that multiple people might get on the same floor but they might all want to get off at different floors, you are able to enter multiple destination floors. There will be a subsequent output to the terminal that shows which elevator was assigned to service the request. There might be 2 elevators assigned to the same floor if there are multiple people waiting on the floor who wish to go in opposite directions. You can then type -1 after you are done entering all the destination floors. Here is an example:
  ```
  ./elevator 
  Enter floor (0 - 9) to pick people up from (-1 to exit or 10 to simulate for 5 seconds): 8
  Enter a floor to drop people off at (-1 to exit): 5
  Enter a floor to drop people off at (-1 to exit): 4
  Enter a floor to drop people off at (-1 to exit): -1
  >>>Elevator to bring people up: 0
  ```

2. The next thing that you can do is to run a simulation for 5 seconds. This simulation shows you how all of the 16 elevators are moving every second for 5 seconds. You will be able to see which floor every single elevator is on at any given second. To run the simulation, just type 10.
  ```
  At 12 seconds
  Elevator: 0 is at floor 2
  Elevator: 1 is at floor 2
  Elevator: 2 is at floor 0
  Elevator: 3 is at floor 0
  Elevator: 4 is at floor 0
  Elevator: 5 is at floor 0
  Elevator: 6 is at floor 0
  Elevator: 7 is at floor 0
  Elevator: 8 is at floor 0
  Elevator: 9 is at floor 0
  Elevator: 10 is at floor 0
  Elevator: 11 is at floor 0
  Elevator: 12 is at floor 0
  Elevator: 13 is at floor 0
  Elevator: 14 is at floor 0
  Elevator: 15 is at floor 0
  ```

3. You can type -1 to exit the program.

### Algorithm
There were a few schedulers that I could have used to run the scheduling of the elevators. In my implementation, each elevator had an individual vector of requests that it had been designated to serve and this vector was arranged in a min-heap for elevators that were going up and max-heap for elevators that were going down to reduce insert time of requests into the vector. The first entry in every vector represented in the next floor that the elevator was going to. Each elevator could be in 1 of 3 states: UP, DOWN, STOP and these states represented the direction of motion for each elevator. For each request on a given floor, I calculated the time for each elevator to get to that floor and the elevator with the shortest time to get to that floor was assigned the request. However due to the time contraint of this challenge, I was not able to handle all the possible cases. There are essentially 2 ways to calculate the time taken to reach a pick-up floor and service the corresponding requests on that floor. 

1. The floor to be picked up from is along the way for an elevator and the floor to be dropped off at is in the current direction of motion of an elevator. This can be calculated by taken the time taken to reach the pick-up floor from the floor that the elevator is currently on. 
2. The floor to be picked up from is not in the current direction of motion of the elevator but the time to reach that given floor for such an elevator is calculated by adding the time taken for the elevator to service all its pending requests and the time taken to reach the pick-up floor from the last floor it just dropped off at.
3. If the corresponding request at a given floor is in the opposite direction of motion for an elevator, then that elevator does not service the request which is mimics real-life application.

Also my implentation is capable of assigning different elevators to the same pick-up floor if there are multiple people at that floor each of whom wish to go in opposite directions. To mimic the motion of the elevators, I spawned 16 worker threads, 1 for each elevator and the elevators take 3 seconds to move from 1 floor to another if they have a pending vector of requests. If they do not have pending requests, they remain stationary. The threads are constantly updating the floor of each elevator.

This implentation has several advantages over First Come, First Serve in that requests are always serviced in the shortest time possible and the concurrent movement of the elevators allow us to calculate the shortest time taken to service a request. Consider a situation where a person on the 8th floor presses the elevator and requests to go to the 10th floor. This request is put onto the queue and a couple of seconds later someone on the 6th floor presses the button and wishes to go the 8th floor. In First Come, First Serve, the request on the 6th floor is ignored till the request on the 8th floor is serviced since the request on the 6th floor came later even though the person on the 6th floor could be picked up along the way to the 8th floor. However in my implementation, this is accounted for and that same elevator would be able to service both requests in the most efficient way possible. One advantage that FCFS provides is that all requests will get serviced because each request is put onto the queue and the requests are eventually emptied out. There is never a scenario when a certain request is never serviced. However in my implentation, there could *starvation* when there are numerous requests around a small range of floors and if a request comes in from a floor that is far away, then that request will never be serviced till all the requests in the small range are serviced. This can be easily accounted for by having a timeout on each request say 2 minutes such that if a request is not serviced within 2 minutes, then a random elevator is chosen to service the request. 

###Improvements

1. The capacity of the elevator is never accounted for. This can be fixed by having a capactiy property for the Elevator class and if the elevator currently exceeds the max capacity it is not assigned to the new request.
2. Starvation. This can be fixed by having a timeout on a request. 
3. More efficient calculation of the shortest time to reach a floor.
