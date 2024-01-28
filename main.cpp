#include "raylib.h"
#include "raymath.h"
#include "qtree.cpp"
#include "boid.cpp"
#include <vector>
#include <iostream>
using namespace std;

QuadTree<Boid>* Boid::tree;
int Boid::amount_updated;
int vectorizable::id_counter;


void print(Boid& b){
    cout << "position: " << b.position.x << ", " << b.position.y << endl;
    cout << "velocity: " << b.velocity.x << ", " << b.velocity.y << endl;
}

int main(){
    const int screenWidth = 800;
    const int screenHeight = 600;

    InitWindow(screenWidth, screenHeight, "raylib [core] example - basic window");
    srand(time(NULL));
    Boid* boid;
    QuadTree<Boid> t({0,0,800,600});
    Boid::tree = &t;
    for(int i = 0; i < 3000; i++){
        Boid b;
        b.position = { static_cast<float>(rand() % screenWidth), static_cast<float>(rand() % screenHeight) }; 
        b.last_position = b.position;
        b.velocity = { 2 + static_cast<float>(rand() % 2), 2 + static_cast<float>(rand() % 2) }; 
        if(rand() % 2 == 0)
            b.velocity.x *= -1;
        if(rand() % 2 == 0)
            b.velocity.y *= -1;
        t.add(b);
    }
    

    SetTargetFPS(60);               
    // Main game loop
    while (!WindowShouldClose()){
        BeginDrawing();
        ClearBackground(RAYWHITE);

        t.draw();
        t.update();

        int amount = t.get_amount();
        if(amount != t.get_amount()){
            cout << "amount changed from " << amount << " to " << t.get_amount() << endl;
            exit(1);
        }

        int get_fps = GetFPS();
        DrawText(TextFormat("fps: %i", get_fps), 10, 10, 20, BLACK);
        EndDrawing();
    }
}
