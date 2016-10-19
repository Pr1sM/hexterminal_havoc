//
//  character_store.c
//  cs_327
//
//  Created by Srinivas Dhanwada on 10/5/16.
//  Copyright © 2016 dhanwada. All rights reserved.
//

#include <stdlib.h>
#include <stdio.h>

#include "character_store.h"
#include "character.h"
#include "ai.h"
#include "pc_control.h"
#include "../logger/logger.h"
#include "../dungeon/dungeon.h"
#include "../point/point.h"
#include "../events/event_queue.h"
#include "../env/env.h"
#include "../dungeon/pathfinder.h"

static character_t** _characters = NULL;
static character_id_t* _alive_characters = NULL;
static int _characters_size = 0;
static int _characters_count = 0;

int CHARACTER_COUNT = 0;

static void setup_npc(character_t* npc);
static character_t* npc_for_id(character_id_t id);

static void print_char(character_t* npc) {
    if(DEBUG_MODE && !NCURSES_MODE) {
        printf("NPC: spawn: (%2d, %2d) ",
               npc->position->x,
               npc->position->y);
        if(npc->destination != NULL) {
            printf("dest: (%2d, %2d) ",
                   npc->destination->x,
                   npc->destination->y);
        } else {
            printf("dest: NONE       ");
        }
        printf("attrs: %x\n", npc->attrs);
    }
    logger.d("NPC: spawn: (%2d, %2d) ",
           npc->position->x,
           npc->position->y);
    if(npc->destination != NULL) {
        logger.d("dest: (%2d, %2d) ",
               npc->destination->x,
               npc->destination->y);
    } else {
        logger.d("dest: NONE     ");
    }
    logger.d("attrs: %x\n", npc->attrs);
}

static void setup_impl() {
    int nummon = NUM_MONSTERS;
    int i;
    dungeon_t* d = dungeonAPI.get_dungeon();
    _characters_size = nummon; // num of monsters
    _characters = (character_t**) calloc(_characters_size, sizeof(*_characters));
    _characters_count = _characters_size;
    CHARACTER_COUNT = _characters_count;
    _alive_characters = (character_id_t*) calloc(_characters_count, sizeof(*_alive_characters));
    point_t* pc_pos = characterAPI.get_pc()->position;
    if(PC_AI_MODE) {
        setup_pc_movement();
    } else {
        setup_control_movement();
    }
    for(i = 0; i < _characters_size; i++) {
        point_t* spawn = pointAPI.construct(0, 0);
        do {
            dungeonAPI.rand_point(d, spawn);
        } while(spawn->distance(spawn, pc_pos) <= 9); // have a radius of 3 blocks
        
        character_t* npc = characterAPI.construct_npc(spawn);
        npc->id = i;
        
        setup_npc(npc);
        
        eventQueueAPI.add_event(npc);
        _characters[i] = npc;
        _alive_characters[i] = npc->id;
        
        free(spawn);
        print_char(npc);
    }
}

static void teardown_impl() {
    int i;
    for(i = 0; i < _characters_size; i++) {
        if(_characters[i] != NULL) {
            characterAPI.destruct(_characters[i]);
        }
    }
    characterAPI.destruct(characterAPI.get_pc());
    free(_characters);
    free(_alive_characters);
    if(_PLAYER_PATH != NULL) {
        pathfinderAPI.destruct(_PLAYER_PATH);
    }
}

static int contains_npc_impl(point_t* p) {
    int i;
    // check npcs first, then player
    for(i = 0; i < _characters_count; i++) {
        if(p->distance(p, _characters[i]->position) == 0 && !_characters[i]->is_dead) {
            return i+1;
        }
    }
    if(p->distance(p, characterAPI.get_pc()->position) == 0) {
        return 0;
    }
    return -1;
}

static char get_char_for_npc_at_index_impl(int i) {
    if(i == 0) {
        return characterAPI.char_for_npc_type(characterAPI.get_pc());
    }
    return characterAPI.char_for_npc_type(_characters[i-1]);
}

static character_t** get_characters_impl() {
    return _characters;
}

static void setup_npc(character_t* npc) {
    point_t* pc_pos = characterAPI.get_pc()->position;
    // telepathic npcs get the pc position
    if(npc->attrs & TELEP_VAL) {
        if(npc->destination == NULL) {
            npc->destination = pointAPI.construct(pc_pos->x, pc_pos->y);
        } else {
            npc->destination->x = pc_pos->x;
            npc->destination->y = pc_pos->y;
        }
    } else {
        // check los on other npcs
        path_node_t* los_path = los_to_pc(npc->position);
        if(los_path != NULL) {
            if(npc->destination == NULL) {
                npc->destination = pointAPI.construct(pc_pos->x, pc_pos->y);
            } else {
                npc->destination->x = pc_pos->x;
                npc->destination->y = pc_pos->y;
            }
            graphAPI.destruct_path(los_path);
        }
    }
}

static int is_finished_impl() {
    int i;
    point_t* pc_pos = characterAPI.get_pc()->position;
    // only 1 character left (pc) so pc has won
    if(_characters_count == 0) {
        return 2;
    }
    // check all npcs that ARE NOT the pc for collision
    for(i = 0; i < _characters_size; i++) {
        if(pc_pos->distance(pc_pos, _characters[i]->position) == 0 && !_characters[i]->is_dead) {
            return 1;
        }
    }
    return 0;
}

static void npc_cleanup_impl() {
    int i;
    int j;
    int old_count = _characters_count;
    // check if NPCs are dead and shift others over
    for(i = 0; i < _characters_count; i++) {
        character_t* npc = npc_for_id(_alive_characters[i]);
        if(npc->is_dead) {
            // shift over other npcs
            for(j = i; j < _characters_count-1; j++) {
                _alive_characters[j] = _alive_characters[j+1];
            }
            _characters_count--;
        }
    }
    logger.d("NPC cleanup: %d ~> %d", old_count, _characters_count);
}

static character_t* npc_for_id(character_id_t id) {
    int i;
    for(i = 0; i < _characters_size; i++) {
        if(id == _characters[i]->id) {
            return _characters[i];
        }
    }
    return NULL;
}

character_store_namespace const characterStoreAPI = {
    setup_impl,
    teardown_impl,
    contains_npc_impl,
    get_char_for_npc_at_index_impl,
    get_characters_impl,
    is_finished_impl,
    npc_cleanup_impl
};
