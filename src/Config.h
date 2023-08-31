#ifndef BADGAME_CONFIG_H
#define BADGAME_CONFIG_H


typedef struct {
    /* Ignored if value = 0. An unstable framerate can cause physics errors */
    int target_fps;
    float fov;
    float sensitivity;
    unsigned short show_fps;
} Config;


void default_config(Config* config);

#endif //BADGAME_CONFIG_H
