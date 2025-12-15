/* RESPONSI II (build 0.0.4 12/15/2025 12:05 AM)
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
#define COLOR_RESET "\033[0m"

// Map Size
#define MH 20
#define MW 40

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
    unsigned int baseHp;
    unsigned int baseStr;
    unsigned int baseAgi;
    unsigned int baseInt;
    char roleDescription[64];
    const char *colorCode;
} HeroBlueprint;

typedef struct {
    char name[32];
    unsigned int currentHp;
    unsigned int maxHp;
    unsigned int str;
    unsigned int agi;
    unsigned int intel;
    int x, y;
    char symbol;
    char color[10];
} PlayerStats;

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
    struct InventoryNode *next;
} InventoryNode;

// Displays
void printBanner(void);
void Splash(void);
int showMainMenu(void);

// Game Systems (Init)
void initPlayer(PlayerStats *player, int heroIndex);
void displayHeroInfo(int index);

// Game Systems (Environment)
void renderMap();
void spawnEnemy(Enemy *enemy, int enemyIndex, int x, int y);

// Game Systems (Player)
void addItem(int itemId, int quantity);
void removeItem(int itemId, int quantity);
void showInventory();
void freeInventory();
int startBattle(PlayerStats *player);

// Global Variables
char map[MH][MW];
InventoryNode *inventoryHead = NULL;

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
        4,
        12,
        22,
        "Virtual Diva / Mage",
        CYAN
        },
    { "Akita Neru",
        120,
        9,
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
        200,
        20,
        500,
        'L',
        MAGENTA
    }
};

    // Item Database
const ItemBlueprint itemDB[] = {
    {
        "Small Potion", // Symbol: +
        TYPE_CONSUMABLE,
        20,
        "Heals 20 HP"
    },
    {
        "Baguette", // Symbol: /
        TYPE_CONSUMABLE,
        2,
        "Delicious! Add 2 Strength"
    },
    {
    "Leek", // Symbol: Y
    TYPE_CONSUMABLE,
    2,
    "I feel... Smarter! Add 2 Intelligence"
    },
    {
    "Phone", // Symbol: H
    TYPE_CONSUMABLE,
    2,
    "This is giving me... FUN! Add 2 Agility"
    },
    {
        "Rusty Sword", // Symbol: |
        TYPE_WEAPON,
        10,
        "Adds 10 Attack"
    },
    {
        "Old Breastplate", // Symbol: B
        TYPE_ARMOR,
        10,
        "Adds 10 Armor"
    },
    {
    "Wooden Shield", // Symbol: O
    TYPE_ARMOR,
    3,
    "Adds 3 Armor"
    },
    {
    "Torch", // Symbol: *
    TYPE_EQUIP,
    3,
    "See Farther"
    },

    // Superskill
    // Teto will get Big BAGUETTE
    // Miku will get MIKU MIKU BEAM
    // Neru will get STOP NAGGING ME!

    {
    "Super Thing",  // Symbol: ?
    TYPE_CONSUMABLE,
    50,
    "Add Damage by 50"
    },
};

int main() {
    srand((unsigned int)time(NULL));

    Splash();

    int menuChoice;
    int gameRunning = 1;

    while (gameRunning) {
        menuChoice = showMainMenu();
        switch (menuChoice) {
            case 0: // New Game
                CLEAR_SCREEN();
                printf("test\n");
                _getch();
                break;

            case 1: { // Load Game
                char inputLoad;
                CLEAR_SCREEN();
                printf("test\n");
                printf("Press Q to return...");

                while (1) {
                    inputLoad = tolower(_getch());
                    if (inputLoad == 'q') {
                        break;
                    }
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
                printf(YELLOW "\n\n\n%sThat's it? Bye!\n", margin, COLOR_RESET);
                delay(DELAY_VSLOW);
                gameRunning = 0;
                break;
        }
    }
    return 0;
}

// Displays
void PrintBanner(void) {
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
    PrintBanner();
    delay(DELAY_MEDIUM);
    printf("%62s", "Loading");
    for (int i = 0; i < 3; i++) {
        delay(DELAY_MEDIUM);
        printf( ".");
        fflush(stdout);
    }
    delay(DELAY_FAST);
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
        PrintBanner();

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

void addItem(int itemId, int quantity) {
    if (itemId < 0 || itemId >= sizeof(itemDB) / sizeof(itemDB[0])) {
        return;
    }

    InventoryNode *current = inventoryHead;
    while (current != NULL) {
        if (current->itemId == itemId) {
            current->quantity += quantity;
            printf(GREEN "\n[!] Added %d %s to inventory (Total: %d)\n" COLOR_RESET,
                   quantity, itemDB[itemId].name, current->quantity);
            return;
        }
        current = current->next;
    }

    InventoryNode *newNode = (InventoryNode *)malloc(sizeof(InventoryNode));
    if (newNode == NULL) {
        printf(RED "Inventory is full!\n" COLOR_RESET);
        return;
    }

    newNode->itemId = itemId;
    newNode->quantity = quantity;
    newNode->next = inventoryHead;
    inventoryHead = newNode;

    printf(GREEN "\n[!] New item obtained: %s (%d)\n" COLOR_RESET,
           itemDB[itemId].name, quantity);
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

void showInventory() {
    CLEAR_SCREEN();
    PrintBanner();
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
