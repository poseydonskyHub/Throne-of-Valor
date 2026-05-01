#include <iostream>
#include <fstream>
#include <string>
#include <memory>
#include <conio.h>
#include <chrono>
#include <thread>
#include <windows.h>
#include "classes.hpp"
constexpr auto BLUE = "\033[34m";
constexpr auto BRIGHT_BLUE = "\033[94m";
constexpr auto RED = "\033[31m";
constexpr auto BRIGHT_RED = "\033[91m";
constexpr auto YELLOW = "\033[33m";
constexpr auto BRIGHT_YELLOW = "\033[93m";
constexpr auto BRIGHT_MAGENTA = "\033[95m";
constexpr auto GREEN = "\033[32m";
constexpr auto LIME = "\033[38;5;118m";
constexpr auto BRIGHT_GREEN = "\033[92m";
constexpr auto ORANGE = "\033[38;5;208m";
constexpr auto PURPLE = "\033[38;5;129m";
constexpr auto TURQUOISE = "\033[38;5;51m";
constexpr auto GRAY = "\033[38;5;244m";
constexpr auto PINK = "\033[38;5;213m";
constexpr auto GOLD = "\033[38;5;220m";
constexpr auto RESET = "\033[0m";

using namespace std;


struct Enemy {
  int x, y;
  bool alive;

  double hp;
  double maxHp;
  double dmg;

  int shootCooldown;
  bool enemyattackAlive[3];
  int enemyattack_x[3];
  int enemyattack_y[3];

  int enemyattack_dx[3];
  int enemyattack_dy[3];
  char type;
};

struct Boss {
  int x, y;
  int baseX, baseY;
  bool alive;
  double hp;

  int moveTimer;
  int moveStep;

  int shootCooldown;
  bool attackAlive[5];
  int attack_x[5];
  int attack_y[5];
  int attack_dx[5];
  int attack_dy[5];
};

Boss boss = {};

const int bossPartsCount = 6;
int bossOffsetX[bossPartsCount] = {0, 1, 2, 0, 1, 2};
int bossOffsetY[bossPartsCount] = {0, 0, 0, 1, 1, 1};

int bossPathX[12] = {0, 1, 2, 2, 2, 1, 0, -1, -2, -2, -2, -1};
int bossPathY[12] = {-2, -2, -1, 0, 1, 2, 2, 2, 1, 0, -1, -2};

const int bossH = 1;
const int bossW = 5;

char bossShape[bossH][bossW] = {{'\\', 'B', 'B', 'B', '/'}};

const int W = 40;
const int H = 20;

bool attackAlive[5] = {false};

Enemy enemies[15];
int enemyCount = 0;

bool portalActive = false;

static void removeEnemyAt(int index) {
  for (int i = index; i < enemyCount - 1; i++) {
    enemies[i] = enemies[i + 1];
  }

  enemies[enemyCount - 1] = {};
  enemyCount--;
}

static bool allEnemiesDead() {
  for (int i = 0; i < enemyCount; i++) {
    if (enemies[i].alive)
      return false;
  }
  return true;
}

static int getAliveEnemyCount() {
  int aliveCount = 0;

  for (int i = 0; i < enemyCount; i++) {
    if (enemies[i].alive) {
      aliveCount++;
    }
  }

  return aliveCount;
}

static void updateEnemies() {
  for (int i = 0; i < enemyCount; i++) {
    if (!enemies[i].alive)
      continue;
  }
}

static void loadLevel(string& filename,
                      char field[][W],
                      char objects[][W],
                      int& playerX,
                      int& playerY) {
  ifstream file(filename);
  string line;
  int y = 0;

  while (getline(file, line)) {
    if (y >= H)
      break;

    for (int x = 0; x < line.size() && x < W; x++) {
      char c = line[x];

      if (c == '#')
        field[y][x] = '#';
      else
        field[y][x] = '.';

      if (c == '$' || c == '+')
        objects[y][x] = c;
      else
        objects[y][x] = ' ';


      if (c == 'U') {
        playerX = x;
        playerY = y;
      }

      if ((c == 'E' || c == 'H' || c == 'R') && enemyCount < 15) {
        enemies[enemyCount] = {};
        enemies[enemyCount].x = x;
        enemies[enemyCount].y = y;
        enemies[enemyCount].alive = true;
        enemies[enemyCount].maxHp = 20;
        enemies[enemyCount].hp = 20;
        enemies[enemyCount].dmg = 10;
        enemies[enemyCount].shootCooldown = 0;
        enemies[enemyCount].type = c;
        enemyCount++;
      }
    }

    y++;
  }
}

static void clear(char field[][W], char objects[][W]) {
  for (int i = 0; i < 5; i++) {
    attackAlive[i] = false;
  }

  for (int i = 0; i < 15; i++) {
    enemies[i] = {};
  }
  enemyCount = 0;

  portalActive = false;
  for (int i = 0; i < H; i++) {
    for (int j = 0; j < W; j++) {
      field[i][j] = '.';
      objects[i][j] = ' ';
    }
  }

  cout << "             lvl complete" << endl;
  this_thread::sleep_for(chrono::seconds(2));
}

static void nextLevel(vector<string>& levels,
               int& currentLevel,
               char field[][W],
               char objects[][W],
               int& x,
                      int& y) {

  if (currentLevel + 1< levels.size()) {
    clear(field, objects);
    currentLevel++;
    loadLevel(levels[currentLevel], field, objects, x, y);
    boss = {};

    if (currentLevel == 11) {
      boss.x = 20;
      boss.y = 10;
      boss.baseX = boss.x;
      boss.baseY = boss.y;
      boss.alive = true;
      boss.hp = 1000;
      boss.moveTimer = 0;
      boss.moveStep = 0;
      boss.shootCooldown = 2;
    }
  } else {
    cout << "             end" << endl;
  }
}

int main() {
  string cmd =
      "mode con cols=" + to_string(W + 10) + " lines=" + to_string(H + 10);
  system(cmd.c_str());
  int game_duration = 0;
  bool lvl_complete = false;
  int currentLevel = 0;
  char unit_char = ' ';
  char man_woman = ' ';


  int portalX = -1;
  int portalY = -1;


  string str =
      "enter class: people = {magician(i), archer(^)}; undead = "
      "{goblin(@), skeleton(|), spider(~)}";
  cout << str << endl;
  cin >> unit_char;

  if (unit_char == 'i' || unit_char == '^') {
    cout << "Man/Woman (m/w)" << endl;
    cin >> man_woman;
  } else {
    man_woman = 'f';
  }


  unique_ptr<Unit> unit;
  if (unit_char == '^' && man_woman == 'm') {
    unit = make_unique<Archer>(false);
  } else if (unit_char == '^' && man_woman == 'w') {
    unit = make_unique<Archer>(true);
  } else if (unit_char == 'i' && man_woman == 'm') {
    unit = make_unique<Magician>(false);
  } else if (unit_char == 'i' && man_woman == 'w') {
    unit = make_unique<Magician>(true);
  } else if (unit_char == '@') {
    unit = make_unique<Goblin>();
  } else if (unit_char == '|') {
    unit = make_unique<Skeleton>();
  } else if (unit_char == '~') {
    unit = make_unique<Spider>();
  } else {
    // РќР° СЃР»СѓС‡Р°Р№ РЅРµРєРѕСЂСЂРµРєС‚РЅРѕРіРѕ РІРІРѕРґР°, С‡С‚РѕР±С‹ РЅРµ Р±С‹Р»Рѕ unit == nullptr.
    unit = make_unique<Archer>(false);
  }

  // game_duration
  if (man_woman == 'w') {
    game_duration = 15;
  } else if (man_woman == 'm') {
    game_duration = 8;
  } else {
    game_duration = 5;
  }

  int attackCooldown = 0;
  int attackDelay = unit->getAtckSpeed();

  vector<string> levels = {"stage1_0.txt", "stage1_1.txt", "stage1_2.txt",
                           "stage1_3.txt", "stage1_4.txt", "stage1_5.txt",
                           "stage2_1.txt", "stage2_2.txt", "stage2_3.txt",
                           "stage2_4.txt", "stage2_5.txt", "boss.txt"};

  int x = W / 2, y = H / 2;

  int playerDirX = 1;
  int playerDirY = 0;



  double curHp = unit->getMaxHp();
  bool UnitAlive = 1;

  int attack_x[5] = {}, attack_y[5] = {};

  int attack_dx[5] = {}, attack_dy[5] = {};

  char field[H][W] = {};
  char trailLife[H][W] = {};
  char objects[H][W] = {};

  if (currentLevel == 0) {
    for (int i = 0; i < H; ++i) {
      for (int j = 0; j < W; ++j) {
        field[i][j] = '.';
        trailLife[i][j] = 0;
        objects[i][j] = ' ';
      }
    }

    loadLevel(levels[currentLevel], field, objects, x, y);
  }


  long long elapsedSec = 0;
  const auto startTime = chrono::steady_clock::now();
  long long lastAgeSec = -1;


  //render
  auto render = [&]() {
    system("cls");
    if (currentLevel <= 5) {
      cout << GREEN <<"             Trone of Valor" << RESET <<endl;
    } else if (currentLevel > 5 && currentLevel <= 10) {
      cout << YELLOW << "             Trone of Valor" << RESET << endl;
    } else {
      cout << RED << "             Trone of Valor" << RESET << endl; 
    }
    for (int j = 0; j < H; ++j) {
      for (int i = 0; i < W; ++i) {
        bool drawAttack = false;
        bool enemydrawAttack = false;
        for (int k = 0; k < 5; k++) {
          if (attackAlive[k] && i == attack_x[k] && j == attack_y[k]) {
            drawAttack = true;
            break;
          }
        }

        bool drawBossAttack = false;

        for (int k = 0; k < 5; k++) {
          if (boss.attackAlive[k] && i == boss.attack_x[k] &&
              j == boss.attack_y[k]) {
            drawBossAttack = true;
            break;
          }
        }

        bool drawBoss = false;
        char bossChar = ' ';

        if (boss.alive) {
          for (int by = 0; by < bossH; by++) {
            for (int bx = 0; bx < bossW; bx++) {
              if (bossShape[by][bx] == ' ')
                continue;

              if (i == boss.x + bx && j == boss.y + by) {
                drawBoss = true;
                bossChar = bossShape[by][bx];
                break;
              }
            }
            if (drawBoss)
              break;
          }
        }

        bool drawEnemy = false;

        for (int b = 0; b < enemyCount; b++) {
          if (enemies[b].alive && i == enemies[b].x && j == enemies[b].y) {
            drawEnemy = true;
            break;
          }
        }

        for (int b = 0; b < enemyCount; b++) {
          for (int k = 0; k < 3; k++) {
            if (enemies[b].alive && enemies[b].enemyattackAlive[k] &&
                i == enemies[b].enemyattack_x[k] &&
                j == enemies[b].enemyattack_y[k]) {
              enemydrawAttack = true;
              break;
            }
          }
          if (enemydrawAttack)
            break;
        }

          if (drawAttack && (enemyCount > 0 || boss.alive))
            cout << ORANGE << unit->getAtck() << RESET;
          else if (enemydrawAttack && enemyCount > 0)
            cout << GOLD << '-' << RESET;
          else if (drawBossAttack)
            cout << BRIGHT_MAGENTA << '*' << RESET;
          else if (drawBoss)
            cout << BRIGHT_MAGENTA << bossChar << RESET;
          else if (portalActive && i == portalX && j == portalY)
            cout << TURQUOISE << 'T' << RESET;
          else if (drawEnemy) {
            char enemyChar = 'E';
            for (int b = 0; b < enemyCount; b++) {
              if (enemies[b].alive && i == enemies[b].x && j == enemies[b].y) {
                enemyChar = enemies[b].type;
                break;
              }
            }
            if (enemyChar == 'E')
                cout << BRIGHT_RED << enemyChar << RESET;
            else if (enemyChar == 'H')
              cout << BRIGHT_MAGENTA << enemyChar << RESET;
            else if (enemyChar == 'R')
              cout << PINK << enemyChar << RESET;
          }
          else if (i == x && j == y)
            cout << BRIGHT_GREEN << unit->getUnit() << RESET;
          else if (objects[j][i] == '+')
            cout << LIME << objects[j][i] << RESET;
          else if (objects[j][i] == '$')
            cout << BRIGHT_BLUE << objects[j][i] << RESET;
          else if (field[j][i] == '#')
            cout << PURPLE << field[j][i] << RESET;
          else
            cout << GRAY << field[j][i] << RESET;
        }
      cout << "\n";
    }


    int aliveEnemies = getAliveEnemyCount();
    if (aliveEnemies > 0) {
      cout << "Enemies: " << enemyCount << "\n";
    }
    if (boss.alive) {
      cout << "Boss HP: " << boss.hp << "\n";
    }
    if (UnitAlive) {
      cout << "Player age: " << unit->getAge() << "\n";
      cout << "Your hp: " << curHp << '/' << unit->getMaxHp() << "\n";
      cout << "Time left: " << (game_duration - elapsedSec)
           << " min\n";
      cout << "Current level:" << currentLevel << '/' << levels.size() - 1 << endl;
      if (currentLevel  == 5 || currentLevel == 10) {
        cout << RED <<"!LAST LVL OF STAGE!" << RESET << endl;
      } else if (currentLevel == 11){
        cout << RED << "!BOSS FIGHT!" << RESET << endl;
      }
      cout << "Move: WASD, Attack: F , Exit: Esc.\n";
    }
  };

  render();

  // main cycle
  while (true) {
    elapsedSec = chrono::duration_cast<chrono::minutes>(
                     chrono::steady_clock::now() - startTime)
                     .count();
    if (elapsedSec != lastAgeSec) {
      unit->setAge(static_cast<int>(elapsedSec + 1));
      lastAgeSec = elapsedSec;
    }
    if (elapsedSec >= game_duration) {
      system("cls");
      cout << "             Time is over.\n";
      break;
    }
    if (!UnitAlive) {
      system("cls");
      cout << "             u die!";
      this_thread::sleep_for(chrono::seconds(3));
      break;
    }

    updateEnemies();

    int prevX = x;
    int prevY = y;

    bool moved = false;
    bool enemyRequest = true;

    bool shootHeld = GetAsyncKeyState('F');


    if (_kbhit()) {
      int c = _getch();
      if (c == 27)
        break;


      if (c == 'w' || c == 'W') {
        y--;
        moved = true;
        playerDirX = 0;
        playerDirY = -1;
      } else if (c == 's' || c == 'S') {
        y++;
        moved = true;
        playerDirX = 0;
        playerDirY = 1;
      } else if (c == 'a' || c == 'A') {
        x--;
        moved = true;
        playerDirX = -1;
        playerDirY = 0;
      } else if (c == 'd' || c == 'D') {
        x++;
        moved = true;
        playerDirX = 1;
        playerDirY = 0;
      }
    }


    if (x < 0)
      x = 0;
    if (x >= W)
      x = W - 1;
    if (y < 0)
      y = 0;
    if (y >= H)
      y = H - 1;


    if (objects[y][x] == '$') {
      x = prevX;
      y = prevY;
    }

    if (objects[y][x] == '+') {
      curHp += 50;

      if (curHp > unit->getMaxHp()) {
        curHp = unit->getMaxHp();
      }

      objects[y][x] = ' ';
    }

    if (boss.alive) {
      bool touchedBoss = false;

      for (int by = 0; by < bossH; by++) {
        for (int bx = 0; bx < bossW; bx++) {
          if (bossShape[by][bx] == ' ')
            continue;

          if (x == boss.x + bx && y == boss.y + by) {
            curHp -= 35;

            if (curHp <= 0) {
              curHp = 0;
              UnitAlive = false;
            }

            x = prevX;
            y = prevY;
            touchedBoss = true;
            break;
          }
        }
        if (touchedBoss)
          break;
      }
    }

    if (boss.alive) {
      boss.moveTimer++;

      if (boss.moveTimer >= 6) {
        boss.moveTimer = 0;
        boss.moveStep = (boss.moveStep + 1) % 12;

        boss.x = boss.baseX + bossPathX[boss.moveStep];
        boss.y = boss.baseY + bossPathY[boss.moveStep];
      }
    }

    for (int b = 0; b < enemyCount; b++) {
      if (enemies[b].alive && x == enemies[b].x && y == enemies[b].y) {
        curHp -= enemies[b].dmg;

        if (curHp <= 0) {
          curHp = 0;
          UnitAlive = false;
        }

        x = prevX;
        y = prevY;
        break;
      }
    }


    if (!portalActive) {
      if (currentLevel == 11) {
        if (allEnemiesDead() && !boss.alive) {
          portalActive = true;
          portalX = W - 2;
          portalY = H / 2;
        }
      } else {
        if (allEnemiesDead()) {
          portalActive = true;
          portalX = W - 2;
          portalY = H / 2;
        }
      }
    }

    if (portalActive && x == portalX && y == portalY) {
      nextLevel(levels, currentLevel, field, objects, x, y);
      portalActive = false;
      moved = false;
      render();
      continue;
    }


    if (moved) {
      field[prevY][prevX] = '#';
      trailLife[prevY][prevX] = 10;
    }



    if (attackCooldown > 0)
      attackCooldown--;


    if (shootHeld && attackCooldown == 0) {
      int freeIdx = -1;

      for (int i = 0; i < 5; i++) {
        if (!attackAlive[i]) {
          freeIdx = i;
          break;
        }
      }

      if (freeIdx != -1) {
        attack_x[freeIdx] = x + playerDirX;
        attack_y[freeIdx] = y + playerDirY;
        attack_dx[freeIdx] = playerDirX;
        attack_dy[freeIdx] = playerDirY;
        attackAlive[freeIdx] = true;

        attackCooldown = attackDelay;
      }
    }


    if (boss.alive) {
      boss.shootCooldown--;

      if (boss.shootCooldown <= 0) {
        int freeIdx = -1;

        for (int i = 0; i < 5; i++) {
          if (!boss.attackAlive[i]) {
            freeIdx = i;
            break;
          }
        }

        if (freeIdx != -1) {
          int centerX = boss.x + 2;
          int centerY = boss.y;

          boss.attack_x[freeIdx] = centerX;
          boss.attack_y[freeIdx] = centerY;

          int diffX = x - centerX;
          int diffY = y - centerY;

          boss.attack_dx[freeIdx] = 0;
          boss.attack_dy[freeIdx] = 0;

          if (diffX > 0)
            boss.attack_dx[freeIdx] = 1;
          else if (diffX < 0)
            boss.attack_dx[freeIdx] = -1;

          if (diffY > 0)
            boss.attack_dy[freeIdx] = 1;
          else if (diffY < 0)
            boss.attack_dy[freeIdx] = -1;

          boss.attackAlive[freeIdx] = true;
          boss.shootCooldown = 14;
        }
      }
    }


    for (int b = 0; b < enemyCount; b++) {
      if (!enemies[b].alive)
        continue;

      enemies[b].shootCooldown--;

      if (enemies[b].shootCooldown <= 0) {
        int freeIdx = -1;

        for (int i = 0; i < 3; i++) {
          if (!enemies[b].enemyattackAlive[i]) {
            freeIdx = i;
            break;
          }
        }

        if (freeIdx != -1) {
          enemies[b].enemyattack_x[freeIdx] = enemies[b].x;
          enemies[b].enemyattack_y[freeIdx] = enemies[b].y;
          enemies[b].enemyattackAlive[freeIdx] = true;

          if (enemies[b].type == 'H' || enemies[b].type == 'R') {
            int diffX = x - enemies[b].x;
            int diffY = y - enemies[b].y;

            enemies[b].enemyattack_dx[freeIdx] = 0;
            enemies[b].enemyattack_dy[freeIdx] = 0;

            if (diffX > 0)
              enemies[b].enemyattack_dx[freeIdx] = 1;
            else if (diffX < 0)
              enemies[b].enemyattack_dx[freeIdx] = -1;

            if (diffY > 0)
              enemies[b].enemyattack_dy[freeIdx] = 1;
            else if (diffY < 0)
              enemies[b].enemyattack_dy[freeIdx] = -1;
          } else {
            enemies[b].enemyattack_dx[freeIdx] = -1;
            enemies[b].enemyattack_dy[freeIdx] = 0;
          }

          if (enemies[b].type == 'H')
            enemies[b].shootCooldown = 18;
          else
            enemies[b].shootCooldown = 7;
        }
      }
    }

    for (int i = 0; i < 5; i++) {
      if (!attackAlive[i])
        continue;

      attack_x[i] += attack_dx[i];
      attack_y[i] += attack_dy[i];

      if (attack_x[i] >= W || attack_x[i] < 0 || attack_y[i] >= H || attack_y[i] < 0) {
        attackAlive[i] = false;
        continue;
      }

      if (objects[attack_y[i]][attack_x[i]] == '$') {
        attackAlive[i] = false;
        continue;
      }

      if (boss.alive) {
        bool bossHit = false;

        for (int by = 0; by < bossH; by++) {
          for (int bx = 0; bx < bossW; bx++) {
            if (bossShape[by][bx] == ' ')
              continue;

            if (attack_x[i] == boss.x + bx && attack_y[i] == boss.y + by) {
              boss.hp -= unit->getDmg() * unit->getAge();

              if (boss.hp <= 0) {
                boss.hp = 0;
                boss.alive = false;
              }

              attackAlive[i] = false;
              bossHit = true;
              break;
            }
          }
          if (bossHit)
            break;
        }
        if (bossHit)
          continue;
      }
        

      for (int b = 0; b < enemyCount; b++) {
        if (enemies[b].alive && attack_x[i] == enemies[b].x &&
            attack_y[i] == enemies[b].y) {
          enemies[b].hp -= unit->getDmg();
          attackAlive[i] = false;

          if (enemies[b].hp <= 0) {
            removeEnemyAt(b);
            attackAlive[i] = false;
            break;
          }
        }
      }
    }

    for (int i = 0; i < 5; i++) {
      if (!boss.attackAlive[i])
        continue;

      boss.attack_x[i] += boss.attack_dx[i];
      boss.attack_y[i] += boss.attack_dy[i];

      int sx = boss.attack_x[i];
      int sy = boss.attack_y[i];

      if (sx < 0 || sx >= W || sy < 0 || sy >= H) {
        boss.attackAlive[i] = false;
        continue;
      }

      if (objects[sy][sx] == '$') {
        boss.attackAlive[i] = false;
        continue;
      }

      if (sx == x && sy == y && UnitAlive) {
        curHp -= 25;

        if (curHp <= 0) {
          curHp = 0;
          UnitAlive = false;
        }

        boss.attackAlive[i] = false;
      }
    }


    for (int b = 0; b < enemyCount; b++) {
      for (int i = 0; i < 3; i++) {
        if (!enemies[b].enemyattackAlive[i])
          continue;


        enemies[b].enemyattack_x[i] += enemies[b].enemyattack_dx[i];
        enemies[b].enemyattack_y[i] += enemies[b].enemyattack_dy[i];

        int sx = enemies[b].enemyattack_x[i];
        int sy = enemies[b].enemyattack_y[i];

        if (sx < 0 || sx >= W || sy < 0 || sy >= H) {
          enemies[b].enemyattackAlive[i] = false;
          continue;
        }

        if (objects[sy][sx] == '$') {
          enemies[b].enemyattackAlive[i] = false;
          continue;
        }

        if (enemies[b].alive && UnitAlive && sy == y && sx == x) {
          curHp -= enemies[b].dmg;
          if (curHp <= 0) {
            curHp = 0;
            UnitAlive = false;
          }
          enemies[b].enemyattackAlive[i] = false;
        }
      }
    }


    for (int j = 0; j < H; ++j) {
      for (int i = 0; i < W; ++i) {
        if (trailLife[j][i] > 0) {
          trailLife[j][i]--;
          if (trailLife[j][i] == 0) {
            field[j][i] = '.';
          }
        }
      }
    }


    render();
  }
    return 0;
  }
