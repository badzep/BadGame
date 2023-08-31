mkdir -p target
g++ -o target/test src/Config.c src/Simulation.c src/Sprite.c src/Hitbox.c src/Chunk.cpp src/GameObject.cpp src/main.cpp -lraylib -lode
echo "Done Building Linux"
