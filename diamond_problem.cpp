#include <iostream>

using namespace std;

class SpaceShip 
{
    virtual void CollideWith(Asteroid& inAsteroid) 
    {
        inAsteroid.CollideWith(*this);
    }
};

class GiantSpaceShip : public SpaceShip 
{
    virtual void CollideWith(Asteroid& inAsteroid) 
    {
        inAsteroid.CollideWith(*this);
    }
};

class Asteroid {
public:
  virtual void CollideWith(SpaceShip&) {
    cout << "Asteroid hit a SpaceShip" << endl;
  }
  virtual void CollideWith(GiantSpaceShip&) {
    cout << "Asteroid hit a GiantSpaceShip" << endl;
  }
};

class ExplodingAsteroid : public Asteroid {
public:
  virtual void CollideWith(SpaceShip&) {
    cout << "ExplodingAsteroid hit a SpaceShip" << endl;
  }
  virtual void CollideWith(GiantSpaceShip&) {
    cout << "ExplodingAsteroid hit a GiantSpaceShip" << endl;
  }
};