#ifndef CITY_H
#define CITY_H

#include <string>
#include <vector>
using namespace std;

class City {
private:
    string name;
    int population;
    double avgDistEnemyBase;
    double avgDistFriendlyBase;
    int estEnemyInfantryPower;
    int estFriendlyInfantryPower;
    int SerialNum;
    static int numCities;
public:
    double factors[6];
    double riskFactor;
    City();
    City(string name, int population, double avgDistEnemyBase, double avgDistFriendlyBase,
        int estEnemyInfantryPower, int estFriendlyInfantryPower);
    string getName() const;
    int getPopulation() const;
    double getAvgDistEnemyBase() const;
    double getAvgDistFriendlyBase() const;
    int getEstEnemyInfantryPower() const;
    int getEstFriendlyInfantryPower() const;
    static int getNumCities();
    int getSerialNum() const;
    double getRiskFactor() const;
    void setSerialNum(int serialNum);
    void setName(string name);
    void setPopulation(int population);
    void setAvgDistEnemyBase(double avgDistEnemyBase);
    void setAvgDistFriendlyBase(double avgDistFriendlyBase);
    void setEstEnemyInfantryPower(int estEnemyInfantryPower);
    void setEstFriendlyInfantryPower(int estFriendlyInfantryPower);
    static void defineRiskFactor(vector<City*>& cities);
    ~City();
};

#endif