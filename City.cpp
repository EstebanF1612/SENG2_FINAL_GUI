#include "City.h"
#include <string>
#include <iostream>
#include <vector>
using namespace std;

int City::numCities = 0;

City::City() {
    this->name = "";
    this->population = 0;
    this->avgDistEnemyBase = 0;
    this->avgDistFriendlyBase = 0;
    this->estEnemyInfantryPower = 0;
    this->estFriendlyInfantryPower = 0;
    this->riskFactor = 0;
    this->factors[0] = this->population;
    this->factors[1] = this->avgDistFriendlyBase;
    this->factors[2] = this->avgDistEnemyBase;
    this->factors[3] = this->estEnemyInfantryPower;
    this->factors[4] = this->estFriendlyInfantryPower;
    this->SerialNum = this->numCities;
    numCities++;
}

City::City(string name, int population, double avgDistEnemyBase, double avgDistFriendlyBase,
    int estEnemyInfantryPower, int estFriendlyInfantryPower) {
    this->name = name;
    this->population = population;
    this->avgDistEnemyBase = avgDistEnemyBase;
    this->avgDistFriendlyBase = avgDistFriendlyBase;
    this->estEnemyInfantryPower = estEnemyInfantryPower;
    this->estFriendlyInfantryPower = estFriendlyInfantryPower;
    this->factors[0] = this->population;
    this->factors[1] = this->avgDistFriendlyBase;
    this->factors[2] = this->avgDistEnemyBase;
    this->factors[3] = this->estEnemyInfantryPower;
    this->factors[4] = this->estFriendlyInfantryPower;
    this->riskFactor = 0;
    this->SerialNum = this->numCities;
    numCities++;
}

string City::getName() const {
    return name;
}

int City::getPopulation() const {
    return population;
}

double City::getAvgDistEnemyBase() const {
    return avgDistEnemyBase;
}

double City::getAvgDistFriendlyBase() const {
    return avgDistFriendlyBase;
}

int City::getEstEnemyInfantryPower() const {
    return estEnemyInfantryPower;
}

int City::getEstFriendlyInfantryPower() const {
    return estFriendlyInfantryPower;
}

int City::getNumCities() {
    return numCities;
}

int City::getSerialNum() const {
    return SerialNum;
}

double City::getRiskFactor() const {
    return riskFactor;
}

void City::setSerialNum(int SerialNum) {
    this->SerialNum = SerialNum;
}

void City::setName(string name) {
    this->name = name;
}

void City::setPopulation(int population) {
    this->population = population;
}

void City::setAvgDistEnemyBase(double avgDistEnemyBase) {
    this->avgDistEnemyBase = avgDistEnemyBase;
}

void City::setAvgDistFriendlyBase(double avgDistFriendlyBase) {
    this->avgDistFriendlyBase = avgDistFriendlyBase;
}

void City::setEstEnemyInfantryPower(int estEnemyInfantryPower) {
    this->estEnemyInfantryPower = estEnemyInfantryPower;
}

void City::setEstFriendlyInfantryPower(int estFriendlyInfantryPower) {
    this->estFriendlyInfantryPower = estFriendlyInfantryPower;
}

void City::defineRiskFactor(vector<City*>& cities) {
    for (int i = 0; i < cities.size(); i++) {
        cities[i]->riskFactor = (cities[i]->getAvgDistFriendlyBase() / cities[i]->getAvgDistEnemyBase()) +
            0.35 * (cities[i]->getPopulation() / 10000.0) +
            0.5 * (cities[i]->getEstEnemyInfantryPower() - cities[i]->getEstFriendlyInfantryPower());
    }
}

City::~City() {
    numCities--;
}