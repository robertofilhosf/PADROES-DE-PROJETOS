#ifndef CONFIG_H
#define CONFIG_H

typedef struct {
    int modo_debug;
} Config;

Config* getConfig(void);

#endif
