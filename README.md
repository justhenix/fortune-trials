<div align="center">

# ⚔️ Fortune Trials

**A CLI-Based Turn-Based RPG Dungeon Crawler**

![C Language](https://img.shields.io/badge/Language-C-00599C?style=for-the-badge&logo=c&logoColor=white)
![Platform](https://img.shields.io/badge/Platform-Windows-0078D6?style=for-the-badge&logo=windows&logoColor=white)
![Status](https://img.shields.io/badge/Status-Completed-success?style=for-the-badge)

<p align="center">
  <img src="https://capsule-render.vercel.app/api?type=waving&color=d01328&height=120&section=header&text=Fortune%20Trials&fontSize=50&fontColor=ffffff" width="100%"/>
</p>

</div>

## 📜 Description

**Fortune Trials** is a dungeon crawler RPG game built entirely in C. Players choose a hero, explore a procedurally generated map, battle enemies using a turn-based system, manage inventory, and upgrade skills to defeat the final boss, the *Lone Guardian*.

This project demonstrates the implementation of advanced Data Structures (Linked Lists, Binary Trees, Queues) and File Handling in a pure C environment.

---

## 🎮 Gameplay Features

* **3 Playable Heroes:**
    * 🥖 **Kasane Teto:** Tank / Chimera (High HP & Defense).
    * 🎤 **Hatsune Miku:** Mage / Diva (High Magic Damage).
    * 📱 **Akita Neru:** Rogue (High Agility & Criticals).
* **Turn-Based Combat:** Tactical battles using a Queue system for turn order.
* **Exploration:** Navigate through the dungeon, find hidden chests (`?`), and avoid obstacles.
* **Inventory System:** Manage items using a dynamic Linked List.
* **Skill Tree:** Unlock and view skills organized in a Binary Search Tree (BST).
* **Save/Load System:** Persist your progress (Stats, Position, Inventory) to a binary file.

---

## 🛠️ Technical Implementation

This project was created to fulfill the **Responsi Praktikum Konsep Pemrograman 2025** assignment. It implements the following core concepts:

| Feature | Data Structure / Concept | Implementation Details |
| :--- | :--- | :--- |
| **Inventory** | **Linked List** | Dynamic item storage (`InventoryNode`), allowing unlimited item collection. |
| **Skill System** | **Binary Search Tree** | Skills are stored and retrieved using a BST (`SkillNode`) for efficient sorting. |
| **Battle System** | **Queue** | Turn order is managed using a FIFO Queue (`BattleQueue`), determining who attacks first based on Agility. |
| **Save System** | **File Handling** | Uses `fwrite` and `fread` to serialize structs and map data into `savegame.dat`. |
| **Entities** | **Structs** | Player, Enemies, and Items are defined using complex `typedef struct`. |

---

## 📸 Screenshots
<p align="center">
  <img width="825" height="492" alt="image" src="https://github.com/user-attachments/assets/78bda70d-38d7-4040-877e-eb67a73ea292" />
</p>

```text
Map Preview:
   # # # # # # # # #
   # . . . P . . . #
   # . # # # # . . #
   # . . ? . . . L #
   # # # # # # # # #

Battle Log:
   > You hit Green Slime for 15 damage!
   > Green Slime attacks! You dodged!
