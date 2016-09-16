//
//  main.c
//  cs_327
//
//  Created by Srinivas Dhanwada on 9/1/16.
//  Copyright © 2016 dhanwada. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <errno.h>
#include <limits.h>

#include "src/env.h"
#include "src/dungeon.h"
#include "src/logger.h"

int main(int argc, char * argv[]) {
    
    envAPI.setup_environment();
    envAPI.parse_args(argc, argv);
    
    if(DEBUG_MODE) {
        logger.set_modes_enabled(LOG_T | LOG_D | LOG_I | LOG_W | LOG_E | LOG_F);
    } else {
        logger.set_modes_enabled(LOG_I | LOG_W | LOG_E | LOG_F);
    }
    
    dungeonAPI.construct();
    
    if(LOAD_DUNGEON) {
        dungeonAPI.load();
    } else {
        dungeonAPI.generate();
    }
    
     dungeonAPI.print();
    
    if(SAVE_DUNGEON) {
        dungeonAPI.save();
    }
    
    dungeonAPI.destruct();
    envAPI.cleanup();
    
    return 0;
}
