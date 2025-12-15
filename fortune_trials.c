/* RESPONSI II (build 1.0.0 12/16/2025 12:05 AM)
* Nama  : Gamma Assyafi Fadhillah Ar Rasyad
 * NIM.  : L0125013
 * Kelas : A
 * Prodi : Informatika
 */

                // ==========                   ========== //
                // ==========  Fortune Trials   ========== //
                // ==========                   ========== //

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <conio.h>
#include <time.h>
#include <windows.h>

// Delays
#define DELAY_VFAST 150
#define DELAY_FAST 300
#define DELAY_MEDIUM 500
#define DELAY_SLOW 900
#define DELAY_VSLOW 1300

// Colors
#define GREEN 		"\033[32m"
#define RED 		"\033[31m"
#define YELLOW 		"\033[33m"
#define BLUE 		"\033[34m"
#define MAGENTA		"\033[35m"
#define CYAN		"\033[36m"
#define GRAY		"\033[90m"
#define BG_RED		"\033[41m"
#define BG_BLACK    "\033[47m"
#define COLOR_RESET "\033[0m"

// Map Size
#define MH 20
#define MW 40

// Message Log
#define MAX_LOG_LINES 6
#define MAX_LOG_LENGTH 80
#define UI_MARGIN "   "
#define LOG_WIDTH 47

// Clear Screen
void CLEAR_SCREEN() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

// Text Delay
void delay(int ms) {
#ifdef _WIN32
    Sleep(ms);
#else
    usleep(ms * 1000);
#endif
}

typedef struct {
    char name[32];
    int baseHp;
    int baseStr;
    int baseAgi;
    int baseInt;
    char roleDescription[64];
    const char *colorCode;
} HeroBlueprint;

typedef struct {
    char name[32];
    int currentHp;
    int maxHp;
    int str;
    int agi;
    int intel;
    int totalAtk;
    int totalDef;
    int atkBonus;
    int isDodging;
    int x, y;
    char symbol;
    char color[10];
    int skillCD[3];
} PlayerStats;

typedef struct {
    char name[32];
    char description[64];
} StoredSkill;

typedef struct SkillNode {
    char skillName[32];
   int reqLevel;
    char description[64];
    struct SkillNode *left;
    struct SkillNode *right;
} SkillNode;

typedef struct TurnNode {
    char entityName[32];
   int isPlayer;        // 1 Player, 0 Enemy
    struct TurnNode *next;
} TurnNode;

typedef struct {
    TurnNode *front;
    TurnNode *rear;
} BattleQueue;

typedef enum {
    TYPE_CONSUMABLE,
    TYPE_WEAPON,
    TYPE_ARMOR,
    TYPE_EQUIP,
    TYPE_KEY_ITEM
} ItemType;

typedef enum {
    START,
    LOAD,
    CREDITS,
    EXIT
} MenuState;

typedef enum {
    LOG_NORMAL,
    LOG_GOOD,
    LOG_LOOT,
    LOG_BATTLE,
    LOG_WARNING
} LogType;

typedef struct {
    char name[32];
   int baseHp;
   int baseAtk;
   int xpReward;
    char symbol;
    const char *color;
} EnemyBlueprint;

typedef struct {
    char name[32];
    ItemType type;
   int value;
    char description[64];
} ItemBlueprint;

typedef struct {
    char name[32];
   int currentHp;
   int maxHp;
   int attack;
   int x, y;
   int isAlive; // 1 = Live, 0 = Dead
    char symbol;
    char color[10];
} Enemy;

typedef struct InventoryNode {
   int itemId;
   int quantity;
   int isEquipped;
    struct InventoryNode *next;
} InventoryNode;

// Displays
void printBanner(void);
void Splash(void);
void flushInput(void);
void encounterEffect(void);
int showMainMenu(void);
void addMessage(const char *text);
void drawMessageLog(void);
void logItemAdded(const char *itemName,int qty,int total,int isNew);
const char *getLogColor(LogType type);
void addMessageBeauty(const char *text, LogType type);


// Game Core / State Machine
void initPlayer(PlayerStats *player,int heroIndex);
int characterSelectionScreen(void);

// World / Environment Systems
void initMap(void);
void renderMap(PlayerStats *p);
void spawnEnemy(Enemy *enemy,int enemyIndex,int x,int y);


// Player Systems
void updateStats(PlayerStats *p);
void consumeConsumable(PlayerStats *player, int itemId);
int startBattle(PlayerStats *player, Enemy *enemy);
int calculateDamage(int baseDmg, int targetDef, PlayerStats *player);
int executeSkill(PlayerStats *player, Enemy *enemy, int skillIndex);
void showSkillsInBattle(int heroIndex);

// Inventory / Item Systems
void addItem(int itemId,int quantity);
void removeItem(int itemId,int quantity);
void showInventory(void);
void freeInventory(void);
int countItemsRecursive(InventoryNode *node);
int isConsumable(int itemId);
void showConsumableItems(void);
int useItemInBattle(int itemId, PlayerStats *player);
void toggleEquipItem(int ItemId);
void openChest(int x, int y, PlayerStats *player);
int getRandomChestItem();

// Skill Systems (using Binary Tree)
SkillNode* insertSkill(SkillNode* root, char* name,int level, char* desc);
void showSkillTree(SkillNode* root);
SkillNode* initHeroSkills(int heroIndex);

// Persistence / Save System
void saveGame(PlayerStats *player);
int loadGame(PlayerStats *player);

// Turns / Queue
void initQueue(BattleQueue *q);
void enqueueTurn(BattleQueue *q, const char *name,int isPlayer);
int dequeueTurn(BattleQueue *q, char *outName,int *outType);

// Global Variables
char map[MH][MW];
InventoryNode *inventoryHead = NULL;
char messageLog[MAX_LOG_LINES][MAX_LOG_LENGTH];
LogType messageType[MAX_LOG_LINES];
int messageCount = 0;

const HeroBlueprint heroDB[] = {
    {
        "Kasane Teto",
        140,
        18,
        8,
        9,
        "Chimera / Tank",
        RED
        },
    { "Hatsune Miku",
        100,
        10,
        12,
        22,
        "Virtual Diva / Mage",
        CYAN
        },
    { "Akita Neru",
        120,
        1,
        20,
        10,
        "Phone Addict / Rogue",
        YELLOW
        }
};

    // Enemy Database
const EnemyBlueprint enemyDB[] = {
    {
        "Green Slime",
        30,
        5,
        10,
        's',
        GREEN
    },
    {
        "Naughty Goblin",
        60,
        12,
        25,
        'g',
        YELLOW
    },
    {
        "Lone Guardian",
        120,
        15,
        500,
        'L',
        MAGENTA
    }
};

    // Item Database
const ItemBlueprint itemDB[] = {
    {
        "Small Potion",
        TYPE_CONSUMABLE,
        20,
        "Heals 20 HP"
    },
    {
        "Baguette",
        TYPE_CONSUMABLE,
        2,
        "Delicious! Add 2 Strength"
    },
    {
    "Leek",
    TYPE_CONSUMABLE,
    2,
    "I feel... Smarter! Add 2 Intelligence"
    },
    {
    "Phone",
    TYPE_CONSUMABLE,
    2,
    "This is giving me... FUN! Add 2 Agility"
    },
    {
        "Rusty Sword",
        TYPE_WEAPON,
        10,
        "Adds 10 Attack"
    },
    {
        "Old Breastplate",
        TYPE_ARMOR,
        10,
        "Adds 10 Armor"
    },
    {
    "Wooden Shield",
    TYPE_ARMOR,
    3,
    "Adds 3 Armor"
    },

    // VERY RARE DROP!!
    // Teto will get Big Baguette(TM)
    // Miku will get "MIKU MIKU BEAM"
    // Neru will get "STOP NAGGING ME!"

    {
    "Super Thing",  // Symbol: ?
    TYPE_CONSUMABLE,
    50,
    "Add Damage by 50"
    },
};

StoredSkill heroSkills[3][3] = {
    // Teto
    {
        {"Baguette Bonk", "Smash enemy with Baguette"},
        {"Chimera Roar", "Stun enemy with a loud scream"},
        {"Drill Hair Shield", "Spin twin-drills to deflect dmg"}
    },
    // Miku
    {
            {"Sonic Voice", "Magic damage using sound waves"},
            {"Leek Spin", "Rapid hits with a vegetable"},
            {"Virtual Diva", "Heal HP significantly"}
    },
    // Neru
    {
            {"Speed Texting", "Boost Agility temporarily"},
            {"Ignore Mode", "High chance to dodge attacks"},
            {"Phone Throw", "High critical physical damage"}
    }
};

int main() {
    srand(( int)time(NULL));

    Splash();

    initMap();

   int menuChoice;
   int gameRunning = 1;

    while (gameRunning) {
        menuChoice = showMainMenu();
        switch (menuChoice) {
            case 0: // New Game

                freeInventory();
                messageCount = 0;

                CLEAR_SCREEN();
                int selectedHero = characterSelectionScreen();
                PlayerStats player;
                initPlayer(&player, selectedHero);

                SkillNode* mySkills = initHeroSkills(selectedHero);

                addItem(0, 2); // Give heal potion
                addItem(1, 1); // Give baguette

               int playing = 1;
                while (playing) {
                    CLEAR_SCREEN();
                    renderMap(&player);
                    printf("   Stats: HP %d/%d | ATK %d\n", player.currentHp, player.maxHp, player.totalAtk);

                    printf("\n   [W/A/S/D] Move  [I] Inventory  [K] Skills  [M] Menu\n");
                    drawMessageLog();

                    char action = tolower(_getch());

                   int newX = player.x;
                   int newY = player.y;

                    if (action == 'w') newY--;
                    else if (action == 's') newY++;
                    else if (action == 'a') newX--;
                    else if (action == 'd') newX++;
                    else if (action == 'i') showInventory();
                    else if (action == 'k') {
                        CLEAR_SCREEN();
                        printf("\n   === SKILL TREE ===\n");
                        showSkillTree(mySkills);
                        _getch();
                    }
                    else if (action == 'm') {
                        printf("\n   Save progress? (y/n): ");
                        char saveChoice = tolower(_getch());
                        if (saveChoice == 'y') saveGame(&player);

                        playing = 0;
                    }

                    char targetCell = map[newY][newX];

                    if (targetCell != '#') {

                        if (targetCell == '?') {
                            addMessageBeauty("You found a hidden chest!", LOG_LOOT);
                            addItem(0, 1);
                            addItem(1, 1);
                            map[newY][newX] = '.';
                        }

                        else if (targetCell == 'L') {
                            Enemy boss = {0};
                            spawnEnemy(&boss, 2, newX, newY);

                            addMessageBeauty("WARNING: BOSS APPROACHING!", LOG_WARNING);
                            drawMessageLog();
                            delay(DELAY_SLOW);
                            encounterEffect();

                            if (startBattle(&player, &boss)) {
                                CLEAR_SCREEN();
                                printf(YELLOW "\n\n   CONGRATULATIONS, HERO!\n");
                                printf("   You have defeated the Lone Guardian!\n");
                                printf("   [Press ANY KEY to continue" COLOR_RESET);
                                _getch();
                                menuChoice = 2;
                                playing = 0;
                                break;
                            } else {
                                printf(RED "   GAME OVER.\n" COLOR_RESET);
                                _getch();
                                playing = 0;
                            }
                        }

                        else {
                            player.x = newX;
                            player.y = newY;

                            if (targetCell == '.' && rand() % 100 < 10) {
                                Enemy randomEnemy = {0};
                                spawnEnemy(&randomEnemy, rand() % 2, 0, 0);
                                if (!startBattle(&player, &randomEnemy)) {
                                    printf("GAME OVER\n");
                                    _getch();
                                    playing = 0;
                                }
                            }
                        }
                    }
                }
                break;

    case 1: { // Load Game
                CLEAR_SCREEN();
                PlayerStats player;

                if (loadGame(&player)) {

                    SkillNode* mySkills = NULL;
                    if (strstr(player.name, "Teto")) mySkills = initHeroSkills(0);
                    else if (strstr(player.name, "Miku")) mySkills = initHeroSkills(1);
                    else mySkills = initHeroSkills(2);

                   int playing = 1;

                    while (playing) {
                        CLEAR_SCREEN();
                        renderMap(&player);

                        printf("   Stats: HP %d/%d | ATK %d | AGI %d\n",
                            player.currentHp, player.maxHp, player.totalAtk, player.agi);

                        printf("\n   [W/A/S/D] Move  [I] Inventory  [K] Skills  [M] Save & Menu\n");
                        drawMessageLog();

                        char action = tolower(_getch());

                       int newX = player.x;
                       int newY = player.y;

                        if (action == 'w') newY--;
                        else if (action == 's') newY++;
                        else if (action == 'a') newX--;
                        else if (action == 'd') newX++;
                        else if (action == 'i') showInventory();
                        else if (action == 'k') {
                            CLEAR_SCREEN();
                            printf("\n   === SKILL TREE ===\n");
                            showSkillTree(mySkills);
                            _getch();
                        }
                        else if (action == 'm') {
                            // Ask to save before back to main menu
                            printf("\n   Save progress? (y/n): ");
                            char saveChoice = tolower(_getch());
                            if (saveChoice == 'y') saveGame(&player);
                            playing = 0;
                        }

                        // Collision Check
                        char targetCell = map[newY][newX];

                        if (targetCell != '#') {

                            // Chest
                            if (targetCell == '?') {
                                addMessageBeauty("You found a hidden chest!", LOG_LOOT);
                                addItem(0, 1);
                                addItem(1, 1);
                                map[newY][newX] = '.';
                            }

                            // Boss Battle
                            else if (targetCell == 'L') {
                                Enemy boss = {0};
                                spawnEnemy(&boss, 2, newX, newY); // Index 2 = Lone Guardian

                                addMessageBeauty("WARNING: BOSS APPROACHING!", LOG_WARNING);
                                drawMessageLog();
                                delay(DELAY_SLOW);
                                encounterEffect();

                                if (startBattle(&player, &boss)) {
                                    // MENANG LAWAN BOSS
                                    CLEAR_SCREEN();
                                    printf(YELLOW "\n\n   CONGRATULATIONS, HERO!\n");
                                    printf("   You have defeated the Lone Guardian!\n");
                                    printf("   You get $676942 worth of treasure.\n\n");
                                    printf("   [Press ANY KEY to Continue]\n" COLOR_RESET);
                                    _getch();

                                    // Pindah ke Credits
                                    menuChoice = 2;
                                    playing = 0;
                                    break;
                                } else {
                                    // KALAH LAWAN BOSS
                                    printf(RED "   GAME OVER. The Guardian stood tall.\n" COLOR_RESET);
                                    _getch();
                                    playing = 0;
                                }
                            }
                            else {
                                player.x = newX;
                                player.y = newY;

                                // Random Encounter (Normal floor '.')
                                if (targetCell == '.' && rand() % 100 < 10) {
                                    Enemy randomEnemy = {0};
                                    // Spawn Slime (0) or Goblin (1) only
                                    spawnEnemy(&randomEnemy, rand() % 2, 0, 0);

                                    if (!startBattle(&player, &randomEnemy)) {
                                        printf(RED "GAME OVER\n" COLOR_RESET);
                                        _getch();
                                        playing = 0;
                                    }
                                }
                            }
                        }
                    }
                } else {
                    printf("\nPress any key to return...");
                    _getch();
                }
                break;
            }

            case 2: // Credits
                CLEAR_SCREEN();
                char *margin = "                                  ";
                printf("\n\n");
                printf("%sProgram Name : Fortune Trials\n", margin);
                printf("%sCreator      : Gamma Assyafi Fadhillah Ar Rasyad\n", margin);
                printf("%sID Number    : L0125013\n", margin);
                printf("%sClass        : A '2025\n", margin);
                printf("%sMajor        : Informatics\n", margin);
                printf("%sSubject      : Programming Concept\n", margin);
                printf("%sAssistant    : Andradhi Bondan Pamungkas\n", margin);
                printf("%s               Muhamad Nabil Fannani\n", margin);

                printf("\n\n%sPress any key to return...", margin);
                _getch();
                break;

            case 3:
                CLEAR_SCREEN();
                printf("Thanks for playing!\n");
                delay(DELAY_VSLOW);
                gameRunning = 0;
                break;
        }
    }
    return 0;
}

// Displays
void printBanner(void) {
    char *margin = "                  ";
    printf("\n");
    printf(YELLOW);
    printf("%s%s", margin, "::::::::::  ::::::::   :::::::::  :::::::::::  :::    :::  ::::    :::  :::::::::: \n");
    printf("%s%s", margin, ":+:        :+:    :+:  :+:    :+:     :+:      :+:    :+:  :+:+:   :+:  :+:        \n");
    printf("%s%s", margin, "+:+        +:+    +:+  +:+    +:+     +:+      +:+    +:+  :+:+:+  +:+  +:+        \n");
    printf("%s%s", margin, ":#::+::#   +#+    +:+  +#++:++#:      +#+      +#+    +:+  +#+ +:+ +#+  +#++:++#   \n");
    printf("%s%s", margin, "+#+        +#+    +#+  +#+    +#+     +#+      +#+    +#+  +#+  +#+#+#  +#+        \n");
    printf("%s%s", margin, "#+#        #+#    #+#  #+#    #+#     #+#      #+#    #+#  #+#   #+#+#  #+#        \n");
    printf("%s%s", margin, "###         ########   ###    ###     ###       ########   ###    ####  ########## \n\n");
    printf("%s%s", margin, "    :::::::::::  :::::::::   :::::::::::      :::      :::         ::::::::        \n");
    printf("%s%s", margin, "        :+:      :+:    :+:      :+:        :+: :+:    :+:        :+:    :+:       \n");
    printf("%s%s", margin, "        +:+      +:+    +:+      +:+       +:+   +:+   +:+        +:+              \n");
    printf("%s%s", margin, "        +#+      +#++:++#:       +#+      +#++:++#++:  +#+        +#++:++#++       \n");
    printf("%s%s", margin, "        +#+      +#+    +#+      +#+      +#+     +#+  +#+               +#+       \n");
    printf("%s%s", margin, "        #+#      #+#    #+#      #+#      #+#     #+#  #+#        #+#    #+#       \n");
    printf("%s%s", margin, "        ###      ###    ###  ###########  ###     ###  ##########  ########        \n\n\n");
    printf(COLOR_RESET);
}

void Splash(void){
    printBanner();
    delay(DELAY_MEDIUM);
    printf("%62s", "Loading");
    for (int i = 0; i < 3; i++) {
        delay(DELAY_MEDIUM);
        printf( ".");
        fflush(stdout);
    }
    delay(DELAY_FAST);
}

void flushInput() {
    while (_kbhit()) {
        _getch();
    }
}

void encounterEffect(void) {
    for (int i = 0; i < 4; i++) {
        printf(BG_RED);
        for (int j = 0; j < MH + 5; j++) printf("\n");
        printf("COLOR_RESET");
        delay(DELAY_VFAST);

        CLEAR_SCREEN();
        printf(BG_BLACK);
        for (int j = 0; j < MH + 5; j++) printf("\n");
        printf(COLOR_RESET);
        delay(DELAY_MEDIUM);
        }
    CLEAR_SCREEN();
}

int showMainMenu() {
    char *margin = "                                 ";

   int cursor = 0;
    const int totalOptions = 4;
    char *options[] = {
        "NEW GAME",
        "LOAD GAME",
        "CREDITS",
        "EXIT"
     };
    char input;

    while (1) {
        CLEAR_SCREEN();
        printBanner();

        printf("\n%s   Use 'W' and 'S' to Navigate, 'ENTER' to Select\n\n", margin);

        for (int i = 0; i < totalOptions; i++) {
            if (i == cursor) {
                printf(YELLOW "%s                   > %s < \n" COLOR_RESET, margin, options[i]);
            } else {
                printf(GRAY   "%s                   %s   \n" COLOR_RESET, margin, options[i]);
            }
        }

        input = tolower(_getch());

        if (input == 'w') {
            cursor--;
            if (cursor < 0) cursor = totalOptions - 1;
        }
        else if (input == 's') {
            cursor++;
            if (cursor >= totalOptions) cursor = 0;
        }
        else if (input == 13) { // 13 is Enter
            return cursor;
        }
    }
}

void addMessage(const char *text) {
    addMessageBeauty(text, LOG_NORMAL);
}

void drawMessageLog() {
    printf("\n");
    printf(UI_MARGIN GRAY "==================== GAME LOG =====================\n" COLOR_RESET);

    for (int i = 0; i < messageCount; i++) {
        const char *color = getLogColor(messageType[i]);

        if (i == messageCount - 1) {
            printf(UI_MARGIN "| %s%-*.*s%s |\n",
                   color,
                   LOG_WIDTH, LOG_WIDTH,
                   messageLog[i],
                   COLOR_RESET);
        } else {
            printf(UI_MARGIN "| " GRAY "%-*.*s" COLOR_RESET " |\n",
                   LOG_WIDTH, LOG_WIDTH,
                   messageLog[i]);
        }
    }

    printf(UI_MARGIN GRAY "===================================================\n" COLOR_RESET);
}


void logItemAdded(const char *itemName,int qty,int total,int isNew) {
    char buffer[MAX_LOG_LENGTH];

    if (isNew) {
        snprintf(buffer, sizeof(buffer),
                 "You obtained %s x%d.", itemName, qty);
    } else {
        snprintf(buffer, sizeof(buffer),
                 "%s x%d added to inventory (Total: %d).",
                 itemName, qty, total);
    }

    addMessageBeauty(buffer, LOG_LOOT);
}

const char *getLogColor(LogType type) {
    switch (type) {
        case LOG_LOOT:    return YELLOW;
        case LOG_GOOD:    return GREEN;
        case LOG_BATTLE:  return RED;
        case LOG_WARNING: return MAGENTA;
        default:          return GRAY;
    }
}

void addMessageBeauty(const char *text, LogType type) {
    if (messageCount < MAX_LOG_LINES) {
        strncpy(messageLog[messageCount], text, MAX_LOG_LENGTH - 1);
        messageLog[messageCount][MAX_LOG_LENGTH - 1] = '\0';
        messageType[messageCount] = type;
        messageCount++;
    } else {
        for (int i = 0; i < MAX_LOG_LINES - 1; i++) {
            strcpy(messageLog[i], messageLog[i + 1]);
            messageType[i] = messageType[i + 1];
        }
        strncpy(messageLog[MAX_LOG_LINES - 1], text, MAX_LOG_LENGTH - 1);
        messageLog[MAX_LOG_LINES - 1][MAX_LOG_LENGTH - 1] = '\0';
        messageType[MAX_LOG_LINES - 1] = type;
    }
}

void addItem(int itemId,int quantity) {
    if (itemId < 0 || itemId >= sizeof(itemDB) / sizeof(itemDB[0])) {
        return;
    }

    InventoryNode *current = inventoryHead;
    while (current != NULL) {
        if (current->itemId == itemId) {
            current->quantity += quantity;

            logItemAdded(
                itemDB[itemId].name,
                quantity,
                current->quantity,
                0
            );
            return;
        }
        current = current->next;
    }

    InventoryNode *newNode = malloc(sizeof(InventoryNode));
    if (!newNode) {
        addMessage("Inventory is full...");
        return;
    }

    newNode->itemId = itemId;
    newNode->quantity = quantity;
    newNode->next = inventoryHead;
    inventoryHead = newNode;

    logItemAdded(
        itemDB[itemId].name,
        quantity,
        quantity,
        1
    );
}

int useItem(int itemId) {
    InventoryNode *current = inventoryHead;
    InventoryNode *prev = NULL;

    while (current != NULL) {
        if (current->itemId == itemId) {
            printf(CYAN "Using %s...\n" COLOR_RESET, itemDB[itemId].name);

            current->quantity--;

            if (current->quantity <= 0) {
                if (prev == NULL) {
                    inventoryHead = current->next;
                } else {
                    prev->next = current->next;
                }
                free(current);
                printf(GRAY "Item %s has been consumed and removed.\n" COLOR_RESET,
                       itemDB[itemId].name);
            }
            return 1;
        }

        prev = current;
        current = current->next;
    }

    printf(RED "You don't have that item!\n" COLOR_RESET);
    return 0;
}

void updateStats(PlayerStats *player) {
    if (player == NULL) return;

    player->totalAtk = player->str;
    player->totalDef = 0;

    InventoryNode *current = inventoryHead;
    while (current != NULL) {
        if (current->isEquipped) {
            if (current->itemId >= 0 && current->itemId < sizeof(itemDB) / sizeof(itemDB[0])) {
                ItemBlueprint item = itemDB[current->itemId];

                switch (item.type) {
                    case TYPE_WEAPON:
                        player->totalAtk += item.value;
                        break;
                    case TYPE_ARMOR:
                        player->totalDef += item.value;
                        break;
                    case TYPE_EQUIP:
                        if (strcmp(item.name, "Torch") == 0) {
                        }
                        break;
                    default:
                        break;
                }
            }
        }
        current = current->next;
    }
}

void consumeConsumable(PlayerStats *player, int itemId) {
    if (itemId < 0 || itemId >= sizeof(itemDB) / sizeof(itemDB[0])) {
        return;
    }

    ItemBlueprint item = itemDB[itemId];
    char buffer[MAX_LOG_LENGTH];

    // Small Potion
    if (strcmp(item.name, "Small Potion") == 0) {
        player->currentHp += item.value;
        if (player->currentHp > player->maxHp) {
            player->currentHp = player->maxHp;
        }
        snprintf(buffer, MAX_LOG_LENGTH, "Used Small Potion! +%d HP", item.value);
        addMessageBeauty(buffer, LOG_GOOD);
    }
    // Baguette (Strength boost)
    else if (strcmp(item.name, "Baguette") == 0) {
        player->str += item.value;
        snprintf(buffer, MAX_LOG_LENGTH, "Ate Baguette! STR +%d", item.value);
        addMessageBeauty(buffer, LOG_GOOD);
    }
    // Leek (Intelligence boost)
    else if (strcmp(item.name, "Leek") == 0) {
        player->intel += item.value;
        snprintf(buffer, MAX_LOG_LENGTH, "Ate Leek! INT +%d", item.value);
        addMessageBeauty(buffer, LOG_GOOD);
    }
    // Phone (Agility boost)
    else if (strcmp(item.name, "Phone") == 0) {
        player->agi += item.value;
        snprintf(buffer, MAX_LOG_LENGTH, "Used Phone! AGI +%d", item.value);
        addMessageBeauty(buffer, LOG_GOOD);
    }
    // Super Thing (Damage boost, temporary for this turn)
    else if (strcmp(item.name, "Super Thing") == 0) {
        player->atkBonus += item.value;  // Add to bonus, not totalAtk
        snprintf(buffer, MAX_LOG_LENGTH, "SUPER THING ACTIVATED! ATK +%d THIS TURN!", item.value);
        addMessageBeauty(buffer, LOG_GOOD);
    }
    updateStats(player);
}

void initPlayer(PlayerStats *player,int heroIndex) {
    if (heroIndex < 0 || heroIndex > 2) return;

    HeroBlueprint blueprint = heroDB[heroIndex];

    strcpy(player->name, blueprint.name);
    player->maxHp = blueprint.baseHp;
    player->currentHp = blueprint.baseHp;
    player->str = blueprint.baseStr;
    player->agi = blueprint.baseAgi;
    player->intel = blueprint.baseInt;
    player->atkBonus = 0;
    player->x = 1;
    player->y = 1;
    player->symbol = 'P';
    strcpy(player->color, blueprint.colorCode);

    for(int i=0; i<3; i++) {
        player->skillCD[i] = 0;
    }

    updateStats(player);
}

void initMap() {
    // 1. Reset Map & Wall
    for (int i = 0; i < MH; i++) {
        for (int j = 0; j < MW; j++) {
            if (i == 0 || i == MH - 1 || j == 0 || j == MW - 1) {
                map[i][j] = '#';
            } else {
                map[i][j] = '.';
            }
        }
    }

    // 2. Obstacle
    for(int x = 10; x < 30; x++) map[5][x] = '#';
    for(int x = 5; x < 25; x++) map[12][x] = '#';

    // 3. Add Chest (Free Items
    map[3][35] = '?';  // Chest 1
    map[15][5] = '?';  // Chest 2
    map[8][20] = '?';  // Chest 3

    // 4. Boss
    map[MH-3][MW-3] = 'L';
}

void renderMap(PlayerStats *p) {
    printf("\n");
    for (int y = 0; y < MH; y++) {
        printf(UI_MARGIN);
        for (int x = 0; x < MW; x++) {
            char cell = map[y][x];

            // 1. Player Position Check
            if (x == p->x && y == p->y) {
                printf("%s%c" COLOR_RESET, p->color, p->symbol);
            }

            // 2. Check other map element(s)
            else if (cell == '?') {
                // Chest YELLOW
                printf(YELLOW "%c" COLOR_RESET, cell);
            }
            else if (cell == 'L') {
                // Boss MAGENTA
                printf(MAGENTA "%c" COLOR_RESET, cell);
            }
            else if (cell == '#') {
                // Wall GRAY
                printf(GRAY "%c" COLOR_RESET, cell);
            }
            else {
                // Floor GRAY
                printf(GRAY "%c" COLOR_RESET, cell);
            }
        }
        printf("\n");
    }
}

void spawnEnemy(Enemy *enemy,int enemyIndex,int x,int y) {
    if (!enemy) return;

    if (enemyIndex < 0 || enemyIndex >= (int)(sizeof(enemyDB) / sizeof(enemyDB[0]))) {
        enemyIndex = 0;
    }

    EnemyBlueprint bp = enemyDB[enemyIndex];

    strcpy(enemy->name, bp.name);
    enemy->maxHp = bp.baseHp;
    enemy->currentHp = bp.baseHp;
    enemy->attack = bp.baseAtk;
    enemy->x = x;
    enemy->y = y;
    enemy->symbol = bp.symbol;
    enemy->isAlive = 1;

    strcpy(enemy->color, bp.color);
}

int characterSelectionScreen() {
   int cursor = 0;
   int totalHeroes = 3;
    char input;

    while (1) {
        CLEAR_SCREEN();
        printBanner();
        printf("\n   CHOOSE YOUR HERO:\n\n");

        for (int i = 0; i < totalHeroes; i++) {
            if (i == cursor) {

                printf(YELLOW "   > %s [%s]\n" COLOR_RESET, heroDB[i].name, heroDB[i].roleDescription);
                printf(GRAY   "     HP: %d | STR: %d | AGI: %d | INT: %d\n\n" COLOR_RESET,
                    heroDB[i].baseHp, heroDB[i].baseStr, heroDB[i].baseAgi, heroDB[i].baseInt);
            } else {
                printf("     %s\n\n", heroDB[i].name);
            }
        }

        printf(GRAY "   [W/S] Navigate  [ENTER] Select\n" COLOR_RESET);

        input = tolower(_getch());
        if (input == 'w') {
            cursor--;
            if (cursor < 0) cursor = totalHeroes - 1;
        } else if (input == 's') {
            cursor++;
            if (cursor >= totalHeroes) cursor = 0;
        } else if (input == 13) { // Enter
            return cursor;
        }
    }
}

int executeSkill(PlayerStats *player, Enemy *enemy, int skillIndex) {
    char buffer[MAX_LOG_LENGTH];
    int dmg = 0;

    // Different skills based on hero
    // Determine hero by name or add a hero field to PlayerStats

    if (strstr(player->name, "Teto")) {
        // Kasane Teto Skills
        switch(skillIndex) {
            case 0: // Baguette Bonk
                dmg = calculateDamage(player->totalAtk + 15, 0, player);
                enemy->currentHp -= dmg;
                snprintf(buffer, MAX_LOG_LENGTH, "Baguette Bonk! You smash for %d dmg!", dmg);
                addMessageBeauty(buffer, LOG_BATTLE);
                return 1;

            case 1: // Chimera Roar
                dmg = calculateDamage(player->totalAtk + 20, 0, player);
                enemy->currentHp -= dmg;
                snprintf(buffer, MAX_LOG_LENGTH, "Chimera Roar! Stunning blow for %d dmg!", dmg);
                addMessageBeauty(buffer, LOG_BATTLE);
                return 1;

            case 2: // Drill Hair Shield
                snprintf(buffer, MAX_LOG_LENGTH, "Drill Hair Shield! DEF +10 next turn!");
                player->totalDef += 10;
                addMessageBeauty(buffer, LOG_GOOD);
                return 1;
        }
    }
    else if (strstr(player->name, "Miku")) {
        // Hatsune Miku Skills
        switch(skillIndex) {
            case 0: // Sonic Voice (magic dmg based on INT)
                dmg = calculateDamage(player->intel * 2 + 10, 0, player);
                enemy->currentHp -= dmg;
                snprintf(buffer, MAX_LOG_LENGTH, "Sonic Voice! Magic blast for %d dmg!", dmg);
                addMessageBeauty(buffer, LOG_BATTLE);
                return 1;

            case 1: // Leek Spin (rapid hits)
                dmg = calculateDamage(player->totalAtk + 10, 0, player);
                enemy->currentHp -= dmg;
                snprintf(buffer, MAX_LOG_LENGTH, "Leek Spin! Rapid hits for %d dmg!", dmg);
                addMessageBeauty(buffer, LOG_BATTLE);
                return 1;

            case 2: // Virtual Diva (heal)
                player->currentHp += 30;
                if (player->currentHp > player->maxHp)
                    player->currentHp = player->maxHp;
                snprintf(buffer, MAX_LOG_LENGTH, "Virtual Diva! Heal +30 HP!");
                addMessageBeauty(buffer, LOG_GOOD);
                return 1;
        }
    }
    else if (strstr(player->name, "Neru")) {
        switch(skillIndex) {
            case 0:
                player->agi += 5;
                snprintf(buffer, MAX_LOG_LENGTH, "Speed Texting! AGI +5 (Temporarily)!");
                addMessageBeauty(buffer, LOG_GOOD);
                return 1;

            case 1:
                player->isDodging = 1;
                snprintf(buffer, MAX_LOG_LENGTH, "Ignore Mode ON! Next attack will miss.");
                addMessageBeauty(buffer, LOG_GOOD);
                return 1;

            case 2:
                dmg = calculateDamage(player->totalAtk, 0, player) * 2;
                enemy->currentHp -= dmg;
                snprintf(buffer, MAX_LOG_LENGTH, "Phone Throw! CRITICAL HIT %d dmg!", dmg);
                addMessageBeauty(buffer, LOG_BATTLE);
                return 1;
        }
    }

    return 0;
}

void showSkillsInBattle(int heroIndex) {
    printf("\n   Available Skills:\n");
    printf("   ----------------\n");
    for (int i = 0; i < 3; i++) {
        printf("   %d. %s - %s\n",
               i + 1,
               heroSkills[heroIndex][i].name,
               heroSkills[heroIndex][i].description);
    }
    printf("   0. Cancel\n");
}

int startBattle(PlayerStats *player, Enemy *enemy) {
    flushInput();

    int originalAgi = player->agi;
    int originalDef = player->totalDef;

    int isBoss = (enemy->symbol == 'L');

    BattleQueue queue;
    initQueue(&queue);

    char buffer[64];
    snprintf(buffer, 64, "Encountered %s!", enemy->name);
    delay(DELAY_MEDIUM);
    addMessageBeauty(buffer, LOG_BATTLE);
    drawMessageLog();
    _getch();

   int battleOver = 0;
   char currentActor[32];
   int currentType;

    while (!battleOver) {
        CLEAR_SCREEN();
        printBanner();
        printf("\n   %sVS %s%s\n", player->color, enemy->color, enemy->name);
        printf(GRAY "   -----------------------------------\n" COLOR_RESET);
        printf("   %s HP: %d/%d  |  %s HP: %d/%d\n\n",
            player->name, player->currentHp, player->maxHp,
            enemy->name, enemy->currentHp, enemy->maxHp);

        drawMessageLog();

        if (queue.front == NULL) {
            if (player->agi >= enemy->attack) {
                enqueueTurn(&queue, player->name, 1);
                enqueueTurn(&queue, enemy->name, 0);
            } else {
                enqueueTurn(&queue, enemy->name, 0);
                enqueueTurn(&queue, player->name, 1);
            }
        }

        if (dequeueTurn(&queue, currentActor, &currentType)) {
           if (currentType == 1) {
               for(int i=0; i<3; i++) {
                   if (player->skillCD[i] > 0) player->skillCD[i]--;
               }
            int validAction = 0;

            while (!validAction) {
                printf("\n   [Your Turn]\n");
                printf("   1. Attack\n");
                printf("   2. Use Skill\n");
                printf("   3. Use Item\n");

                if (isBoss) {
                    printf(GRAY "   4. Run [LOCKED - BOSS BATTLE]\n" COLOR_RESET);
                } else {
                    printf("   4. Run\n");
                }
                
                printf("   > ");
                flushInput();
                char choice = _getch();

                if (choice == '1') {
                   int dmg = calculateDamage(player->totalAtk, 0, player);
                    enemy->currentHp -= dmg;
                    snprintf(buffer, 64, "You hit %s for %d damage!", enemy->name, dmg);
                    addMessage(buffer);
                    delay(DELAY_MEDIUM);
                    validAction = 1;
                }
                else if (choice == '2') {
                    int heroIndex = 0;
                    if (strstr(player->name, "Miku")) heroIndex = 1;
                    else if (strstr(player->name, "Neru")) heroIndex = 2;

                    CLEAR_SCREEN();
                    printf("\n   === SELECT SKILL ===\n");

                    for (int i = 0; i < 3; i++) {
                        if (player->skillCD[i] > 0) {
                            printf(GRAY "   %d. %s (Cooldown: %d turns)\n" COLOR_RESET,
                                   i + 1, heroSkills[heroIndex][i].name, player->skillCD[i]);
                        } else {
                            printf(CYAN "   %d. %s - %s\n" COLOR_RESET,
                                   i + 1,
                                   heroSkills[heroIndex][i].name,
                                   heroSkills[heroIndex][i].description);
                        }
                    }
                    printf("   0. Cancel\n");

                    char skillChoice = _getch();
                    if (skillChoice == '0') continue;

                    int skillIndex = skillChoice - '1';

                    if (skillIndex >= 0 && skillIndex < 3) {
                        if (player->skillCD[skillIndex] > 0) {
                            printf(RED "\n   Skill is not ready yet! Wait %d turns.\n" COLOR_RESET,
                                   player->skillCD[skillIndex]);
                            delay(DELAY_MEDIUM);
                        } else {
                            if (executeSkill(player, enemy, skillIndex)) {
                                player->skillCD[skillIndex] = 4;
                                delay(DELAY_MEDIUM);
                                validAction = 1;
                            }
                        }
                    } else {
                        printf(RED "\n   Invalid selection!\n" COLOR_RESET);
                        delay(DELAY_FAST);
                    }
                }
                else if (choice == '3') {
                    // ITEM USAGE
                    CLEAR_SCREEN();
                    showConsumableItems();

                    printf("\n   Select item (0 to cancel): ");
                    char itemChoice = _getch();

                    if (itemChoice == '0') {
                        printf("\n   Cancelled.\n");
                        continue;
                    }

                    int itemIndex = itemChoice - '1';
                    int consumableCount = 0;
                    InventoryNode *temp = inventoryHead;
                    int foundItemId = -1;

                    while (temp != NULL) {
                        if (isConsumable(temp->itemId)) {
                            if (consumableCount == itemIndex) {
                                foundItemId = temp->itemId;
                                break;
                            }
                            consumableCount++;
                        }
                        temp = temp->next;
                    }

                    if (foundItemId != -1) {
                        useItemInBattle(foundItemId, player);
                        delay(DELAY_MEDIUM);
                        validAction = 1;
                    } else {
                        printf(RED "\n   Invalid selection!\n" COLOR_RESET);
                        delay(DELAY_MEDIUM);
                        continue;
                    }
                }
                else if (choice == '4') {
                    if (isBoss) {
                        printf(RED "\n   You cannot escape from the Guardian! FIGHT!\n" COLOR_RESET);
                        delay(DELAY_MEDIUM);
                    } else {
                        addMessage("You ran away safely!");

                        player->agi = originalAgi;
                        player->totalDef = originalDef;
                        player->isDodging = 0;

                        delay(DELAY_MEDIUM);
                        battleOver = 1;
                        return 1;
                    }
                }
                else {
                    printf(RED "   [!] Invalid input! Press 1, 2, 3, or 4!\n" COLOR_RESET);
                }
            }
        }
           else {
               printf("\n   [Enemy Turn] %s is attacking...\n", enemy->name);
               delay(DELAY_MEDIUM);

               if (player->isDodging == 1) {
                   snprintf(buffer, 64, "%s attacked, but you are ignoring it!", enemy->name);
                   addMessageBeauty(buffer, LOG_GOOD);
                   player->isDodging = 0;
               }
               else {
                   int dmg = calculateDamage(enemy->attack, player->totalDef, player);

                   if (rand() % 100 < (player->agi * 2)) {
                       snprintf(buffer, 64, "You dodged %s's attack naturally!", enemy->name);
                       addMessageBeauty(buffer, LOG_GOOD);
                   } else {
                       player->currentHp -= dmg;
                       snprintf(buffer, 64, "%s hits you for %d damage!", enemy->name, dmg);
                       addMessageBeauty(buffer, LOG_BATTLE);
                   }
               }
               delay(DELAY_MEDIUM);
           }
        }

        if (enemy->currentHp <= 0) {
            addMessageBeauty("Enemy defeated!", LOG_GOOD);
            enemy->isAlive = 0;
            battleOver = 1;
            drawMessageLog();
            _getch();

            player->agi = originalAgi;
            player->totalDef = originalDef;
            player->isDodging = 0;

            return 1;
        }
        if (player->currentHp <= 0) {
            addMessage("You have been defeated...");
            battleOver = 1;
            drawMessageLog();
            _getch();

            player->agi = originalAgi;
            player->totalDef = originalDef;
            player->isDodging = 0;

            return 0;
        }
    }
    player->agi = originalAgi;
    player->totalDef = originalDef;
    return 1;
}

int calculateDamage(int baseDmg, int targetDef, PlayerStats *player) {
    int dmg = baseDmg + player->atkBonus - (targetDef / 2);  // Include bonus
    float variance = ((rand() % 41) + 80) / 100.0;
    dmg = (int)(dmg * variance);
    player->atkBonus = 0;  // Reset bonus after use
    return (dmg < 1) ? 1 : dmg;
}

void showInventory() {
    CLEAR_SCREEN();
    printf("\n========== YOUR INVENTORY ==========\n\n");

    InventoryNode *current = inventoryHead;

    if (current == NULL) {
        printf(GRAY " Inventory is empty...\n" COLOR_RESET);
    } else {
       int index = 1;
        printf("%-4s %-20s %-5s %s\n", "No", "Item Name", "Qty", "Description");
        printf("--------------------------------------------------\n");

        while (current != NULL) {
            ItemBlueprint item = itemDB[current->itemId];

            printf("%-4d %-20s x%-4d %s\n",
                   index, item.name, current->quantity, item.description);

            current = current->next;
            index++;
        }
    }

    printf("\n=====================================\n");
    printf("Press any key to return...");
    _getch();
}

void freeInventory() {
    InventoryNode *current = inventoryHead;
    while (current != NULL) {
        InventoryNode *temp = current;
        current = current->next;
        free(temp);
    }
    inventoryHead = NULL;
}

void saveGame(PlayerStats *player) {
    FILE *fp = fopen("savegame.dat", "wb");
    if (fp == NULL) {
        printf(RED "\n[!] Failed to save data (File Error).\n" COLOR_RESET);
        return;
    }

    fwrite(player, sizeof(PlayerStats), 1, fp);
    fwrite(map, sizeof(char), MH * MW, fp);

    InventoryNode *current = inventoryHead;
    int hasItem;

    while (current != NULL) {
        hasItem = 1;
        fwrite(&hasItem, sizeof(int), 1, fp);
        fwrite(&current->itemId, sizeof(int), 1, fp);
        fwrite(&current->quantity, sizeof(int), 1, fp);
        fwrite(&current->isEquipped, sizeof(int), 1, fp);

        current = current->next;
    }

    hasItem = 0;
    fwrite(&hasItem, sizeof(int), 1, fp);

    fclose(fp);
    printf(GREEN "\n[V] Game saved successfully.\n" COLOR_RESET);
}

int loadGame(PlayerStats *player) {
    FILE *fp = fopen("savegame.dat", "rb");
    if (fp == NULL) {
        printf(RED "\n[!] Save file not found.\n" COLOR_RESET);
        return 0;
    }

    fread(player, sizeof(PlayerStats), 1, fp);
    fread(map, sizeof(char), MH * MW, fp);

    freeInventory();

    int hasItem;
    int tempId, tempQty, tempEquip;

    while (fread(&hasItem, sizeof(int), 1, fp) && hasItem == 1) {
        fread(&tempId, sizeof(int), 1, fp);
        fread(&tempQty, sizeof(int), 1, fp);
        fread(&tempEquip, sizeof(int), 1, fp);

        InventoryNode *newNode = (InventoryNode*)malloc(sizeof(InventoryNode));
        if (newNode != NULL) {
            newNode->itemId = tempId;
            newNode->quantity = tempQty;
            newNode->isEquipped = tempEquip;
            newNode->next = inventoryHead;
            inventoryHead = newNode;
        }
    }

    fclose(fp);
    updateStats(player);

    printf(GREEN "\n[V] Game loaded successfully.\n" COLOR_RESET);
    return 1;
}


int countItemsRecursive(InventoryNode *node) {
    if (node == NULL) {
        return 0;
    }
    return 1 + countItemsRecursive(node->next);
}

int isConsumable(int itemId) {
    if (itemId < 0 || itemId >= sizeof(itemDB) / sizeof(itemDB[0])) {
        return 0;
    }
    return itemDB[itemId].type == TYPE_CONSUMABLE;
}

void showConsumableItems(void) {
    printf("\n   Available Consumables:\n");
    printf("   ---------------------\n");

    InventoryNode *current = inventoryHead;
    int count = 0;

    while (current != NULL) {
        if (isConsumable(current->itemId)) {
            ItemBlueprint item = itemDB[current->itemId];
            printf("   %d. %s x%d - %s\n",
                   count + 1, item.name, current->quantity, item.description);
            count++;
        }
        current = current->next;
    }

    if (count == 0) {
        printf(RED "   No consumables available!\n" COLOR_RESET);
    }
}

int useItemInBattle(int itemId, PlayerStats *player) {
    InventoryNode *current = inventoryHead;
    InventoryNode *prev = NULL;

    while (current != NULL) {
        if (current->itemId == itemId) {
            if (!isConsumable(itemId)) {
                addMessage(RED "That item cannot be used in battle!" COLOR_RESET);
                return 0;
            }

            // Use the item
            consumeConsumable(player, itemId);
            current->quantity--;

            if (current->quantity <= 0) {
                if (prev == NULL) {
                    inventoryHead = current->next;
                } else {
                    prev->next = current->next;
                }
                char buffer[MAX_LOG_LENGTH];
                snprintf(buffer, MAX_LOG_LENGTH,
                         "%s removed from inventory.",
                         itemDB[itemId].name);
                free(current);
            }
            return 1;
        }

        prev = current;
        current = current->next;
    }

    addMessage(RED "You don't have that item!" COLOR_RESET);
    return 0;
}

void toggleEquipItem(int itemId) {
    InventoryNode *current = inventoryHead;

    while (current != NULL) {
        if (current->itemId == itemId) {
            ItemBlueprint item = itemDB[itemId];

            // Only weapons and armor can be equipped
            if (item.type != TYPE_WEAPON && item.type != TYPE_ARMOR) {
                printf(RED "\n   This item cannot be equipped!\n" COLOR_RESET);
                return;
            }

            current->isEquipped = !current->isEquipped;

            char buffer[MAX_LOG_LENGTH];
            if (current->isEquipped) {
                snprintf(buffer, MAX_LOG_LENGTH, "%s equipped!", item.name);
            } else {
                snprintf(buffer, MAX_LOG_LENGTH, "%s unequipped!", item.name);
            }
            addMessage(buffer);

            return;
        }
        current = current->next;
    }

    printf(RED "\n   Item not found!\n" COLOR_RESET);
}

int getRandomChestItem() {
    // Chest can contain: Potion, Baguette, Leek, Phone, Rusty Sword, Shield, Super Thing
	int possibleItems[] = {
    0, 0, 0, 0,       // Small Potion   (4 slot = 20%)
    1, 1,             // Baguette       (2 slot = 10%)
    2, 2,             // Leek           (2 slot = 10%)
    3, 3,             // Phone          (2 slot = 10%)
    4, 4, 4, 4,       // Rusty Sword    (4 slot = 20%)
    6, 6, 6,          // Wooden Shield  (3 slot = 15%)
    9,                // Super Thing    (1 slot = 5%) - !SUPER RARE!
	};

    int totalItems = sizeof(possibleItems) / sizeof(possibleItems[0]);
    int randomIndex = rand() % totalItems;

    return possibleItems[randomIndex];

}

void openChest(int x, int y, PlayerStats *player) {
    // Change chest to empty floor
    map[y][x] = '.';

    // Get random item
    int itemId = getRandomChestItem();

    // Add item to inventory
    addItem(itemId, 1);

    char buffer[MAX_LOG_LENGTH];
    snprintf(buffer, MAX_LOG_LENGTH,
             "Chest opened! You found %s!",
             itemDB[itemId].name);

    addMessageBeauty(buffer, LOG_LOOT);
}

SkillNode* insertSkill(SkillNode* root, char* name,int level, char* desc) {
    if (root == NULL) {
        SkillNode* newNode = (SkillNode*)malloc(sizeof(SkillNode));
        strcpy(newNode->skillName, name);
        newNode->reqLevel = level;
        strcpy(newNode->description, desc);
        newNode->left = newNode->right = NULL;
        return newNode;
    }

    if (strcmp(name, root->skillName) < 0) {
        root->left = insertSkill(root->left, name, level, desc);
    } else {
        root->right = insertSkill(root->right, name, level, desc);
    }
    return root;
}

void showSkillTree(SkillNode* root) {
    if (root != NULL) {
        showSkillTree(root->left);
        printf(CYAN "   [*] %s " COLOR_RESET "- %s (Lv. %d)\n",
            root->skillName, root->description, root->reqLevel);
        showSkillTree(root->right);
    }
}

SkillNode* initHeroSkills(int heroIndex) {
    SkillNode* root = NULL;

    switch (heroIndex) {
        case 0: // Kasane Teto
            // INSERT IN ALPHABETICAL ORDER to get proper tree balance
            root = insertSkill(root, "Baguette Bonk", 1, "Smash enemy with Baguette");
            root = insertSkill(root, "Chimera Roar", 2, "Stun enemy with a loud scream");
            root = insertSkill(root, "Drill Hair Shield", 3, "Spin twin-drills to deflect dmg");
            break;

        case 1: // Hatsune Miku
            // INSERT IN ALPHABETICAL ORDER
            root = insertSkill(root, "Leek Spin", 1, "Rapid hits with a vegetable");
            root = insertSkill(root, "Sonic Voice", 2, "Magic damage using sound waves");
            root = insertSkill(root, "Virtual Diva", 3, "Heal HP significantly");
            break;

        case 2: // Akita Neru
            // INSERT IN ALPHABETICAL ORDER
            root = insertSkill(root, "Speed Texting", 3, "Boost Agility temporarily");
            root = insertSkill(root, "Ignore Mode", 1, "High chance to dodge attacks");
            root = insertSkill(root, "Phone Throw", 2, "High critical physical damage");
            break;

        default:
            break;
    }
    return root;
}

void initQueue(BattleQueue *q) {
    q->front = NULL;
    q->rear = NULL;
}

void enqueueTurn(BattleQueue *q, const char *name,int isPlayer) {
    TurnNode *newNode = (TurnNode*)malloc(sizeof(TurnNode));
    if (!newNode) return;

    strcpy(newNode->entityName, name);
    newNode->isPlayer = isPlayer;
    newNode->next = NULL;

    if (q->rear == NULL) {
        q->front = q->rear = newNode;
        return;
    }

    q->rear->next = newNode;
    q->rear = newNode;
}

int dequeueTurn(BattleQueue *q, char *outName,int *outType) {
    if (q->front == NULL) return 0;

    TurnNode *temp = q->front;

    strcpy(outName, temp->entityName);
    *outType = temp->isPlayer;

    q->front = q->front->next;

    if (q->front == NULL) {
        q->rear = NULL;
    }

    free(temp);
    return 1;
}
