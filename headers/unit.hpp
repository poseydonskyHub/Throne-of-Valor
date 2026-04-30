#pragma once
class Unit {
 private:
  int age = 1;
  double dmg = 0;
  int lvl = 1;
  int atck_speed = 0;
  int hp = 0;

 public:
  Unit(double dmg, int age, int atck_speed, int hp) {
    this->dmg = dmg;
    this->age = age;
    this->atck_speed = atck_speed;
    this->hp = hp;
  }

  virtual char getUnit() const = 0;
  virtual char getAtck() const = 0;

  virtual int getAge() const { return age; }
  void setAge(int newAge) { age = newAge; }
  virtual int getAtckSpeed() const { return atck_speed; }
  virtual double getDmg() const { return dmg; }
  virtual int getMaxHp() const { return +(4 * getAge()); }

  virtual ~Unit() = default;
};