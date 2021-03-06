//
//  character.h
//  cs_327
//
//  Created by Srinivas Dhanwada on 11/1/16.
//  Copyright © 2016 dhanwada. All rights reserved.
//

#ifndef character_h
#define character_h

#include <string>

#include "equip_list.h"
#include "../point/point.h"
#include "../parser/monster_description.h"
#include "../parser/dice.h"
#include "../items/item.h"

#define INTEL_BIT 0
#define TELEP_BIT 1
#define TUNNL_BIT 2
#define ERATC_BIT 3
#define PASS_BIT  4
#define INTEL_VAL (1 << INTEL_BIT)
#define TELEP_VAL (1 << TELEP_BIT)
#define TUNNL_VAL (1 << TUNNL_BIT)
#define ERATC_VAL (1 << ERATC_BIT)
#define PASS_VAL  (1 << PASS_BIT)

typedef enum character_type {
    NONE = 0, PC, NPC
} character_type;

typedef int character_id_t;

class character {
private:
    char char_for_npc_type();
    void update_stats();
    int  calc_damage();
public:
    character_id_t id;
    character_type type;
    point* position;
    point* destination;
    int event_count;
    uint8_t attrs;
    uint8_t base_speed;
    uint8_t speed;
    uint8_t turn_count;
    uint8_t is_dead;
    
    std::string name;
    std::string desc;
    int color;
    int hitpoints;
    char symb;
    dice* damage;
    
    bool is_seen; // is the character within the light of the pc
    
    // PC only attributes
    item** inventory;
    int inventory_size;
    int inventory_len;
    equip_list* equipment;
    
    character(character_type type, point* spawn);
    character(character_type type, point* spawn, monster_description* descriptor);
    ~character();
    void set_position(point* p);
    void set_destination(point* p);
    void perform();
    char get_print_symb(int mode);
    
    static int        pc_expunge_item(item* i, int index);
    static int        pc_drop_item(item* i, int index);
    static int        pc_equip_item(item* i, int index);
    static int        pc_unequip_item(item* i, int index);
    static int        pc_pickup_item(item* i);
    static character* get_pc();
    static void       teardown_pc();
};

#endif /* character_h */
