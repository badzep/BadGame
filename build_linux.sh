mkdir -p target
g++ -o target/test src/Config.c src/Simulation.c src/Sprite.c src/Hitbox.c src/Scene.cpp src/main.cpp -I lib -L lib/raylib -lraylib -I lib/ode -lode
echo "Done Building Linux"
