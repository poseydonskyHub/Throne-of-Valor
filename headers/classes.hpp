#pragma once
#include "unit.hpp"

class Archer : public Unit {
 private:
  char symb = '^';
  char attack = '>';

 public:
  Archer(bool IsWoman = false)
      : Unit(70,     // dmg
             1,       // age
             2,      // atck_speed
             200) {}  // hp

  char getUnit() const override { return symb; }
  int getAtckSpeed() const override { return 2; }
  char getAtck() const override { return attack; }
  int getMaxHp() const override { return 200 + (4 * getAge()); }
  int getAge() const override { return Unit::getAge(); }
};

class Magician : public Unit {
 private:
  char symb = 'i';
  char attack = '*';

 public:
  Magician(bool IsWoman = false)
      : Unit(50,       // dmg
             1,       // age
             6,     // atck_speed
             180) {}  // hp

  char getUnit() const override { return symb; }
  int getAtckSpeed() const override { return 6; }
  char getAtck() const override { return attack; }
  int getMaxHp() const override { return 180 + (4 * getAge()); }
  int getAge() const override { return Unit::getAge(); }
};


class Goblin : public Unit {
 private:
  char symb = '@';
  char attack = '+';

 public:
  Goblin()
      : Unit(20,     // dmg
             1,       // age
             5,     // atck_speed
             200) {}  // hp

  char getUnit() const override { return symb; }
  int getAtckSpeed() const override { return 5; }
  char getAtck() const override { return attack; }
  int getMaxHp() const override { return 200 + (4 * getAge()); }
  int getAge() const override { return Unit::getAge(); }
  virtual ~Goblin() = default;
};

class Skeleton : public Unit {
 private:
  char symb = '|';
  char attack = '-';

 public:
  Skeleton()
      : Unit(19,     // dmg
             1,       // age
             6,      // atck_speed
             200) {}  // hp

  char getUnit() const override { return symb; }
  int getAtckSpeed() const override { return 6; }
  char getAtck() const override { return attack; }
  int getMaxHp() const override { return 100; }
  int getAge() const override { return Unit::getAge(); }
};

class Spider : public Unit {
 private:
  char symb = '~';
  char attack = '&';

 public:
  Spider()
      : Unit(10,      // dmg
             1,      // age
             8,     // atck_speed
             30) {}  // hp

  char getUnit() const override { return symb; }
  int getAtckSpeed() const override { return 8; }
  char getAtck() const override { return attack; }
  int getMaxHp() const override { return 30 + (4 * getAge()); }
  int getAge() const override { return Unit::getAge(); }
};