
#include <cstdlib>

//implements the ship class. ALSO IMPLEMENTS VECTOR AND ARRAY
#include "shipClass.h"
// basic include for interfacing with the console
#include <iostream>
// standard c library. needed for the random number generator, at the very least 
#include <stdlib.h>
// used so that multiple things can be output together on the same line
#include <sstream>
// part of the valueEnter input filter
#include <limits>
// used for generating a random seed for die rolling
#include <ctime>


static unsigned int g_seed;

inline void fast_srand( int seed ){
	g_seed = seed;
}

inline int fastrand() {
	g_seed = (214013 * g_seed + 2531011);
	return (g_seed >> 16) & 0x7FFF;
}

int rolld6(void) {
	int ran = fastrand() % 6 + 1;
	return ran;
}

// value enter function, returns user-entered integers. Verifies that entries are valid. 
int valueEnter(bool overLoad = false, bool seed = false) {
		//dummy start number
		int input = -5;
		int maxIn;
		if (overLoad){
			maxIn = 10000;
		}else{
			maxIn = 10;
		}

		// if it's the first loop we won't show the error message 
		bool firstLoop = true;
		bool accepted = false;

		while(!accepted) {
			if (firstLoop){
				firstLoop = false;
			}else{
				std::cout << "That isn't an acceptable entry." << std::endl;
				std::cout << "Try again: ";
			}

			// try getting an input from the user. 
			try {
				std::cin >> input;
				if (!std::cin) {
					// bad input! Clear out cin and we'll try again. 
					std::cin.clear();
					std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
				}else{
					// force natural numbers (incl 0) in a reasonable range
					if (!seed){
						if (input>-1 && input <maxIn){
							accepted = true;
							return(input);
						}
					}else{
						if (input>-1){
							accepted = true;
							return(input);
						}
					}
				}
			}catch(int e){
				std::cout<<"Exception Number: " << e << std::endl;
			}
		}// end while loop
		
		return(input);
}


std::vector<Ship*> mergeShips(std::vector<Ship*> left, std::vector<Ship*> right, bool threat) {
	std::vector<Ship*> result;
	int rCount = 0;
	int lCount = 0;

	while (lCount<left.size() || rCount<right.size()) {
		if (lCount == left.size()) {
			result.push_back(right[rCount]);
			rCount++;
		}
		else if (rCount == right.size()) {
			result.push_back(left[lCount]);
			lCount++;
		}
		else {
			// if the desired sorting is by threat do this.
			// this puts the highest first. Lowest next
			if (threat) {
				if (left[lCount]->getThreat() > right[rCount]->getThreat()) {
					result.push_back(left[lCount]);
					lCount++;
				}
				else {
					result.push_back(right[rCount]);
					rCount++;
				}
			}
			else { // otherwise sort by initiative
				if (left[lCount]->getInitiative() < right[rCount]->getInitiative()) {
					result.push_back(left[lCount]);
					lCount++;
				}
				else {
					result.push_back(right[rCount]);
					rCount++;
				}
			}
		}
	}
	return(result);
}

// you can send this function a list of references to your ships, and then it sorts the references based on the ships' threat levels. 
// basic recursive merge sort, occupies the following two functions. 
std::vector<Ship*> sortShips(std::vector<Ship*> shipList, bool threat){
	if (shipList.size()==1){
		return shipList;
	}

	std::vector<Ship*> left;
	std::vector<Ship*> right;

	for (int i = 0; i<shipList.size(); i++){
		if (i<(shipList.size()/2)){
			left.push_back(shipList[i]);
		}else{
			right.push_back(shipList[i]);
		}
	}
	left = sortShips(left, threat);
	right = sortShips(right, threat);

	return(mergeShips(left, right, threat) );
}



// resets a buncha ships hulls
void resetHull(std::vector<Ship*> shipList){
	for (int shipCounter = 0; shipCounter < shipList.size(); shipCounter++) {
		shipList[shipCounter]->setHullNow( shipList[shipCounter]->getHullTot() );
	}
}

//check function. Requires a list of pointers to ships, tells you whether or not they are all dead.
// if all are dead return true
// ONLY CALL THIS ON ATTACKING ---OR--- DEFENDING SHIPS
//	otherwise the sim will get stuck in a loop, unless somehow all the ships died...
bool are_all_dead(std::vector<Ship*> shipList) {
	int counter = 0;
	bool allDead = true;
	
	while (counter < shipList.size()){
		if (shipList[counter]->getHullNow() >= 0){
			// ship is alive, return false (one or more alive).
			allDead = false;
			return false;
		}
		counter++;
	}

	//made it through the loop without finding a living ship. 
	return true;
}

int getTarget(Ship* self, std::vector<Ship*> targets){
	bool hasTarget = false;
	int target = 0;
	while (!hasTarget) { // start with no target
		if (target == targets.size()) {
			// if we reached the end of the list with no viable target
			return -1;
		}

		//if this target is alive
		if (targets[target]->getHullNow() >= 0) { // hull can be zero, this means there is no hull. It's OK
			return(target);
			
		}else {
			target++;
		}
	}
	return target;
}

int attack(Ship* self, std::vector<Ship*> targets){
	if (self->getHullNow() >= 0){

	int attackRoll = 0;
	int target = 0; // index in targets, which of the available enemies??

	// find a target for ion cannons! 
	for (int ion = 0; ion < self->getIonCannons(); ion++) { //attacking with each ion cannon
		target = getTarget(self, targets); // get target
		if (target == -1){return -1;}; // if there is no target found, return the exit condition

		attackRoll = rolld6();

		if (attackRoll == 6 ){
			targets[target]->setHullNow(targets[target]->getHullNow() - 1);
		}else if (attackRoll + self->getComputers() - targets[target]->getShields() >=6 && attackRoll!=1){
			targets[target]->setHullNow(targets[target]->getHullNow() - 1);
		}
		
	}

	for (int ion = 0; ion < self->getPlasmaCannons(); ion++) { //attacking with each plasma cannon
		target = getTarget(self, targets); // get target
		if (target == -1) { return -1; }; // if there is no target found, return the exit condition

		attackRoll = rolld6();

		if (attackRoll == 6) {
			targets[target]->setHullNow(targets[target]->getHullNow() - 2);
		}
		else if (attackRoll + self->getComputers() - targets[target]->getShields() >= 6 && attackRoll != 1) {
			targets[target]->setHullNow(targets[target]->getHullNow() - 2);
		}

	}

	for (int ion = 0; ion < self->getAntiMatterCannons(); ion++) { //attacking with each antimatter
		target = getTarget(self, targets); // get target
		if (target == -1) { return -1; }; // if there is no target found, return the exit condition

		attackRoll = rolld6();

		if (attackRoll == 6) {
			targets[target]->setHullNow(targets[target]->getHullNow() - 4);
		}
		else if (attackRoll + self->getComputers() - targets[target]->getShields() >= 6 && attackRoll != 1) {
			targets[target]->setHullNow(targets[target]->getHullNow() - 4);
		}

	}


	// the value returned represents the outcome of the attack

	// 0 == made it to end without running out of targets. 
	// 1 == all targets dead, battle over. 
	return 0;

	}else{return 0;}
}

int missileVolley(Ship* self, std::vector<Ship*> targets){
	int attackRoll = 0;
	int target = 0; // index in targets, which of the available enemies??

					// find a target for missiles! 
	for (int ion = 0; ion < self->getMissiles(); ion++) { //attacking once with each missile
		target = getTarget(self, targets); // get target
		if (target == -1) { return -1; }; // if there is no target found, return the exit condition

		attackRoll = rolld6();

		if (attackRoll == 6) {
			targets[target]->setHullNow(targets[target]->getHullNow() - 2);
		}
		else if ((attackRoll + self->getComputers() - targets[target]->getShields() >= 6) && (attackRoll != 1)) {
			targets[target]->setHullNow(targets[target]->getHullNow() - 2);
		}

	}
	return 0;
}

int getWinner(std::vector<Ship*> attackers, std::vector<Ship*> defenders){
	bool attackDead = are_all_dead(attackers);
	bool defendDead = are_all_dead(defenders);

	if (attackDead && defendDead){
		return 0;
	}else if(attackDead){
		return 1;
	}else if(defendDead){
		return -1;
	}else{
		return 2;
	}
}


//returns 0 on draw, 1 on defense victory, -1 on attack vicory, and 2 on error
int simulateBattle(std::vector<Ship*> attackingShips, std::vector<Ship*> defendingShips, std::vector<Ship*> initiative) {
	// we are given pointers to two vectors containing each of the ships. 
	// first, set all ships to max health
	resetHull(attackingShips);
	resetHull(defendingShips);
	int missileResult = 0;

	int thisShip = 0;
	while (thisShip < initiative.size()){
		if (initiative[thisShip]->isAttacker()){
			missileResult += missileVolley(initiative[thisShip], defendingShips);
		}else{
			missileResult += missileVolley(initiative[thisShip], attackingShips);
		}
		thisShip++;
	}
	// for each ship, the missile result can only be 0 (for successfull attacks) or -1 (for all ships destroyed)
	// therefore the sum will be 0 for all successfull attacks or <0 for all ships destroyed
	if (missileResult < 0) {
		return getWinner(attackingShips, defendingShips);
	}

	bool stillAlive = true;
	int result = 0;

	thisShip = 0;
	while (stillAlive){
		thisShip = thisShip % initiative.size();
		if (initiative[thisShip]->isAttacker()){
			result = attack(initiative[thisShip], defendingShips);
		}else{
			result = attack(initiative[thisShip], attackingShips);
		}

		if (result==-1){
			return getWinner(attackingShips, defendingShips);
			stillAlive = false;
		}
		thisShip++;
	}

	
		
	// fire missiles ();
	//		damage done --> check state ();
	// while (ships alive) --> round of combat();
	//		damage done --> check state ();

	// return winner;

}


int main()
{
	int seed;
	std::cout << "Would you like to enter a seed (0=no)?" <<std::endl;
	seed = valueEnter(true,seed = true);
	if (seed == 0){
		seed = 10847011912;
	}

	fast_srand(seed);

	std::cout << "Running Eclipse Battle Simulator" <<std::endl;
	
	std::cout << std::endl;
	std::cout << "How many rounds of combat? " << std::endl;
	std::cout << "A higher number guarantees more accurate projections. " <<std::endl;
	int number_of_rounds = valueEnter(true);

	// Use these arrays to store the number of each ship type.
	std::array<int, 3> attacking_ships_of_type{{0, 0, 0 }};
	std::array<int, 4> defending_ships_of_type{{0,0,0,0}};
	// types of ships
	std::array<std::string, 4> shipnames{ { "Interceptors", "Cruisers", "Dreadnoughts", "Starbases" } };
	
	std::cout << "Enter the number of... " << std::endl;

	for(int typeCount=0 ; typeCount < attacking_ships_of_type.size(); typeCount++){
		std::cout << "attacking " << shipnames[typeCount] << ": ";
		attacking_ships_of_type[typeCount] = valueEnter();
	}

	std::cout << std::endl;

	for (int typeCount = 0; typeCount < defending_ships_of_type.size(); typeCount++) {
		std::cout << "defending " << shipnames[typeCount] << ": ";
		defending_ships_of_type[typeCount] = valueEnter();
	}

	std::cout << std::endl;

	//just the number of ships on each side
	const int allAttack = attacking_ships_of_type[0] + attacking_ships_of_type[1] + attacking_ships_of_type[2];
	const int allDefend = defending_ships_of_type[0] + defending_ships_of_type[1] + defending_ships_of_type[2] + defending_ships_of_type[3];

	// constructing the 
	std::vector<Ship> attackingShips(allAttack);
	std::vector<Ship> defendingShips(allDefend);

	// the templateParts array holds the number of each ship component. The user fills this in, and the array is passed to the Ship constructor.
	//		that ship constructor makes a Ship template, which is used to fill each Ship vector with an appropriate number of ships
	std::array<int, 8> templateParts{ { 0, 0, 0, 0, 0, 0, 0,0} };
	std::array<std::string, 8> templateNames{{"hull", "shields", "computers","ion cannons", "plasma cannons", "anti-matter cannons", "missiles" , "initiative"}};
	int partCounter; // used to cycle through the different part types, allowing an user to input each type of part for their ships

	std::cout << "Programing the attacking ships." << std::endl;

	//this for loop goes over the three types of attacking ships. Has built in functionality to support more types (with modification)
	for (int typeCount = 0; typeCount < attacking_ships_of_type.size(); typeCount++) {
		//Are there any ships of that type at all??
		if (attacking_ships_of_type[typeCount]!= 0){
			// So we're working with type of ship TYPECOUNT, have the user enter in the characteristics of that ship
			partCounter = 0;
			while(partCounter < templateParts.size()){
				std::cout<< "Enter the attacking "<< shipnames[typeCount]<<"' "<< templateNames[partCounter]<<" value: ";
				templateParts[partCounter] = valueEnter();
				partCounter++;
			}
			// the part template is ready, so apply that to a ship

			Ship shipTemplate(templateParts);

			// going from zero to the number 
			for (int i=0; i<attacking_ships_of_type[typeCount]; i++){
				if(typeCount == 0){
					attackingShips[i] = shipTemplate; // sets the first attacking_ships_of_type[0] of the attacking ships using the template
				}else if(typeCount ==1) {
					attackingShips[i + attacking_ships_of_type[0]] = shipTemplate; // now we do the same, but we're shifting over a bit to not overwrite the first group
				}else if(typeCount == 2) {
					attackingShips[i + attacking_ships_of_type[0] + attacking_ships_of_type[1]] = shipTemplate; //same, but shifting over even more! 
				}else{
					std::cout << "An error has occured in the swtich statement." << std::endl;
				}//end ifififif
			
			}// end for loop for applying the template
		}
	}// end the attack ship construction loop! 
	// creates "attackingShips", a vector of Ships


	std::cout << "Programing the defending ships." <<std::endl;
	

	//work with user to build the defending ships
	for (int typeCount = 0; typeCount < defending_ships_of_type.size(); typeCount++) {
		if (defending_ships_of_type[typeCount]!=0) {
			partCounter = 0;
			while (partCounter < templateParts.size()) {
				std::cout << "Enter the defending " << shipnames[typeCount] << "' " << templateNames[partCounter] << " value: ";
				templateParts[partCounter] = valueEnter();
				partCounter++;
			}// end of while. This loop creates the parts template to be applied to all of the ships of this class. 
	
			// the extra argument forces a different constructor to be used 
			Ship shipTemplate(templateParts, true); 
	
			for (int i = 0; i<defending_ships_of_type[typeCount]; i++) {
				if (typeCount == 0) {
					defendingShips[i] = shipTemplate; // sets the first attacking_ships_of_type[0] of the attacking ships using the template
				}else if(typeCount ==1) {
					defendingShips[i + defending_ships_of_type[0]] = shipTemplate; // now we do the same, but we're shifting over a bit to not overwrite the first group
				}else if(typeCount ==2) {
					defendingShips[i + defending_ships_of_type[0] + defending_ships_of_type[1]] = shipTemplate; //same, but shifting over even more! 
				}else if(typeCount ==3) {
					defendingShips[i + defending_ships_of_type[0] + defending_ships_of_type[1] + defending_ships_of_type[2]] = shipTemplate;
				}else{
					std::cout << "An error has occured in the swtich statement." << std::endl;
				}//end switch
	
			}// end for loop for applying the template
		}// end the attack ship construction loop! 
	}
	// creates "defendingShips", a vector of Ships

	//by this point we have generated two vectors, each one containing a list of ships

	// generate initiative list.
	std::vector<Ship*> list_of_all_ships;
	std::vector<Ship*> list_of_attacking_ships;
	std::vector<Ship*> list_of_defending_ships;

	
	// create lists of pointers to the actual ships
	for (int i=0; i<(allAttack); i++){
		list_of_all_ships.push_back(&attackingShips[i]);
		list_of_attacking_ships.push_back(&attackingShips[i]);
	}
	for (int i=0; i<(allDefend); i++){
		list_of_all_ships.push_back( &defendingShips[i] );
		list_of_defending_ships.push_back( &defendingShips[i] );
	}

	std::vector<Ship*> initiativeSort = sortShips(list_of_all_ships, false);	
	std::vector<Ship*> threatSortAttack = sortShips(list_of_attacking_ships, true);
	std::vector<Ship*> threatSortDefend = sortShips(list_of_defending_ships, true);


	int attackWins = 0;
	int defendWins = 0;
	int draws = 0;
	//errors can only happen with missile volleys
	int errors = 0;
	int winner = 0;

	// the actual battle simulation. Easiest part of all the code 
	int battleCount = 0;
	while (battleCount < number_of_rounds){
		winner = simulateBattle(threatSortAttack, threatSortDefend, initiativeSort);
		if (winner == 0){
			draws++;
		}else if(winner == 1){
			defendWins++;
		}else if(winner == -1){
			attackWins++;
		}else{
			errors++;
		}

		battleCount++;
	}


	std::cout << "Attackers won: " << attackWins << std::endl;
	std::cout << "Defenders won: " << defendWins << std::endl;
	std::cout << "Draws: " << draws << std::endl;
	std::cout << "Errors: " << errors << std::endl;

	float atkRatio = 100* attackWins/ number_of_rounds;
	float defRatio = 100* defendWins / number_of_rounds;

	std::cout<<std::endl;
	std::cout<< "Odds of attacker victory: " << atkRatio <<"%."<<std::endl;
	std::cout << "Odds of defender victory: " << defRatio << "%." << std::endl;
	std::cout << "Odds of draw: " << (100*draws/number_of_rounds) << "%." <<std::endl;


    return 0;
}
