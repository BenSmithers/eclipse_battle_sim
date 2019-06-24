#pragma once

// these are needed by EclipseBattleSim.cpp in addition to this header
#include <array>
#include <vector>

// Ship class or eclipse battle si

class Ship {
	
public:
	Ship();
	Ship(std::array<int, 8> templateParts);
	Ship(std::array<int, 8> templateParts, bool Defender);
	
	// return the various properties
	int getID(void);
	bool isAttacker(void);
	int getHullTot(void);
	int getHullNow(void);
	int getShields(void);
	int getComputers(void);
	float getInitiative(void);
	
	int getIonCannons(void);
	int getPlasmaCannons(void);
	int getAntiMatterCannons(void);
	int getMissiles(void);


	float getThreat(void);

	void setHullNow(int change);
	

private:
	int id;
	bool attacker;

	int hullTot;
	int hullNow;
	int shields;
	int computers;

	int ionCannons;
	int plasmaCannons;
	int antiMatterCannons;
	int missiles;

	float initiative;

	float threat;
};

Ship::Ship() {
	id = 0;
	hullTot = 0;
	hullNow = 0;
	shields = 0;
	initiative = 0.0;
	computers = 0;
	ionCannons = 1;
	plasmaCannons = 0;
	antiMatterCannons = 0;
	missiles = 0;
	threat = 1.0;
}

Ship::Ship(std::array<int, 8> templateParts) {
	// build the ship
	id = 0;
	float constant1 = 1;
	float constant2 = 1;

	hullTot = templateParts[0];
	hullNow = templateParts[0];
	shields = templateParts[1];
	computers = templateParts[2];
	ionCannons = templateParts[3];
	plasmaCannons = templateParts[4];
	antiMatterCannons = templateParts[5];
	missiles = templateParts[6];
	threat = constant1 * (hullTot + shields) + constant2 * (computers + 1)*(ionCannons + (2 * plasmaCannons) + (4 * antiMatterCannons) + (4 * missiles));
	initiative = float(templateParts[7]);
	attacker = true;
}

Ship::Ship(std::array<int, 8> templateParts, bool Defender) {
	// build the ship
	id = 0;
	float constant1 = 1.0;
	float constant2 = 1.0;

	hullTot = templateParts[0];
	hullNow = templateParts[0];
	shields = templateParts[1];
	computers = templateParts[2];
	ionCannons = templateParts[3];
	plasmaCannons = templateParts[4];
	antiMatterCannons = templateParts[5];
	missiles = templateParts[6];
	threat = constant1 * (hullTot + shields) + constant2 * (computers + 1)*(ionCannons + (2 * plasmaCannons) + (4 * antiMatterCannons) + (4 * missiles));
	if (Defender) {
		initiative = float(templateParts[7]) + 0.5;
		attacker = false;
	}else{
		initiative = float(templateParts[7]);
		attacker = true;
	}
}

bool Ship::isAttacker(void){
	return attacker;
}

int Ship::getID(void) {
	return id;
}

int Ship::getHullTot(void){
	return hullTot;
}

int Ship::getHullNow(void){
	return hullNow;
}

void Ship::setHullNow(int change){
	hullNow = change;
}

int Ship::getShields(void){
	return shields;
}

int Ship::getComputers(void){
	return computers;
}

int Ship::getIonCannons(void){
	return ionCannons;
}

int Ship::getPlasmaCannons(void){
	return plasmaCannons;
}

int Ship::getAntiMatterCannons(void){
	return antiMatterCannons;
}

int Ship::getMissiles(void){
	return missiles;
}

float Ship::getInitiative(void){
	return initiative;
}

float Ship::getThreat(void){
	return threat;
}


