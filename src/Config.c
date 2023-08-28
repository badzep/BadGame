#include "Config.h"


void default_config(Config* config){
    config->target_fps = 1000;
    config->fov = 60.0f;
    config->sensitivity = 0.25f;
    config->fog_density = 0.09f;
}
