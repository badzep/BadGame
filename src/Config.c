#include "Config.h"


void default_config(Config* config){
    config->target_fps = 500;
    config->fov = 60.0f;
    config->sensitivity = 0.25f;
    config->show_fps = 1;
}
