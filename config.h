/*
 * config.h
 *
 *  Created on: 06/07/2011
 *      Author: code
 */

#ifndef CONFIG_H_
#define CONFIG_H_

#include "game_categories_light.h"

typedef struct {
    u32 mode;
    u32 prefix;
    u32 uncategorized;
    u32 selection;
} CategoryConfig;

enum uncat {
    NONE,
    ONLY_MS,
    ONLY_IE,
    BOTH,
};

extern CategoryConfig config;

int load_config(CategoryConfig *config);
int save_config(CategoryConfig *config);

#endif /* CONFIG_H_ */
