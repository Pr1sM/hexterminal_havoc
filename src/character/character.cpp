//
//  character.cpp
//  cs_327
//
//  Created by Srinivas Dhanwada on 10/25/16.
//  Copyright � 2016 dhanwada. All rights reserved.
//

#include <stdlib.h>


#include "character.h"
#include "ai.h"
#include "pc_control.h"
#include "../point/point.h"
#include "../logger/logger.h"
#include "../dungeon/dungeon.h"

static character* gPLAYER_CHARACTER = NULL;

character::character(character_type type, point* spawn) {
    type = type;
    if(type== PC) {
        speed = 10;
        attrs = 0;
    } else if(type == NPC) {
        speed = (rand() & 0xf) + 5;
        attrs = rand() & 0xf;
    } else {
        logger::w("Invalid type passed into character constructor!");
        speed = 1;
        attrs = 0;
    }
    turn_count = 100 / speed;
    is_dead = 0;
    event_count = 0;
    id = -1;
    
    if(spawn == NULL) {
        logger::w("NULL point passed into character constructor!");
        position = new point(0, 0);
    } else {
        position = new point(spawn);
    }
    destination = NULL;
}

character::character(character_type type, point* spawn, monster_description* descriptor) {
    type = type;
    if(type == PC) {
        speed = 10;
        attrs = 0;
    } else if(type == NPC) {
        // Use descriptor
        
    } else {
        logger::w("Invalid type passed into character constructor!");
        speed = 1;
        attrs = 0;
    }
    turn_count = 100 / speed;
    is_dead = 0;
    event_count = 0;
    id = -1;
    
    if(spawn == NULL) {
        logger::w("NULL point passed into character constructor!");
        position = new point(0, 0);
    } else {
        position = new point(spawn);
    }
    destination = NULL;
}

character::~character() {
    static int char_count = 0;
    logger::i("character destructor called - %d", ++char_count);
    if(position != NULL) {
        logger::i("destructing position");
        delete position;
    }
    if(destination != NULL) {
        logger::i("destructing destination");
        delete destination;
    }
}

void character::set_position(point* p) {
    if(p == NULL) {
        logger::w("NULL point passed into set_position! position will remain unchanged");
        return;
    }
    if(position == NULL) {
        position = new point(p);
    } else {
        position->x = p->x;
        position->y = p->y;
    }
}

void character::set_destination(point* p) {
    if(p == NULL) {
        logger::w("NULL point passed into set_point! destructing point!");
        delete destination;
        destination = NULL;
        return;
    }
    if(destination == NULL) {
        destination = new point(p);
    } else {
        destination->x = p->x;
        destination->y = p->y;
    }
}

void character::perform() {
    if(type == PC) {
        if(env_constants::PC_AI_MODE) {
            ai::handle_pc_move();
        } else {
            pc_control::handle_control_move();
        }
    } else if(type == NPC) {
        ai::handle_npc_move(this);
    } else {
        logger::w("performed called on character with invalid type! doing nothing");
    }
}

character* character::get_pc() {
    if(gPLAYER_CHARACTER == NULL) {
        point spawn(0, 0);
        if(env_constants::X_START < 80 && env_constants::Y_START < 21) {
            spawn.x = env_constants::X_START;
            spawn.y = env_constants::Y_START;
            
            // check if this point is valid
            dungeon* d = dungeon::get_dungeon();
            if(d->tiles[spawn.y][spawn.x]->content == tc_ROCK) {
                logger::e("Spawn point from CLI is invalid, setting new random point in the dungeon!");
                dungeon::rand_point(d, &spawn);
            }
        } else {
            dungeon::rand_point(dungeon::get_dungeon(), &spawn);
        }
        gPLAYER_CHARACTER = new character(PC, &spawn);
        gPLAYER_CHARACTER->id = 0;
    }
    return gPLAYER_CHARACTER;
}

void character::teardown_pc() {
    if(gPLAYER_CHARACTER != NULL) {
        delete gPLAYER_CHARACTER;
        gPLAYER_CHARACTER = NULL;
    }
}

char character::char_for_npc_type() {
    if(type == PC) {
        return '@';
    }
    if(attrs < 10) {
        return '0' + attrs;
    }
    return 'a' + attrs - 10;
}
