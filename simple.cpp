#include "raylib.h"
#include "raymath.h"
#include "qtree.cpp"
#include <string>

float dt = 2;
float max_speed = 2;
float min_speed = 1;
float avoid_radius = 4;
float avoid_factor = .05;
float match_radius = 16;
float match_factor = .1;
float center_radius = 16;
float center_factor = .01;

struct Boid{
    Vector2 position;
    Vector2 last_position;
    Vector2 velocity;
};

void UpdatePositions(Boid& b){
    b.last_position = b.position;
    float speed = Vector2Length(b.velocity);
    if(speed > max_speed){
        b.velocity = Vector2Scale(Vector2Normalize(b.velocity), max_speed);
    } else if(speed < min_speed){
        b.velocity = Vector2Scale(Vector2Normalize(b.velocity), min_speed);
    }
    b.position.x = (b.position.x * 2) - b.last_position.x + (b.velocity.x * dt);
    b.position.y = (b.position.y * 2) - b.last_position.y + (b.velocity.y * dt);
    b.position.x = b.position.x < 0 ? b.position.x + 800 : b.position.x;
    b.position.y = b.position.y < 0 ? b.position.y + 600 : b.position.y;
    b.position.x = fmod(b.position.x, 800);
    b.position.y = fmod(b.position.y, 600);
}

void seperation(Boid& b,vector<Boid> boids){
    Vector2 steer = {0,0};
    for(Boid& other : boids){
        Vector2 diff = Vector2Subtract(b.position, other.position);
        float d = Vector2Length(diff);
        if(d < avoid_radius && d > 0){
            diff = Vector2Scale(Vector2Normalize(diff),avoid_radius);
            steer = Vector2Add(steer, diff);
        }
    }
    b.velocity = Vector2Add(b.velocity, Vector2Scale(steer, avoid_factor));
}

void alignment(Boid& b,vector<Boid> boids){
    Vector2 steer = {0,0};
    int count = 0;
    for(Boid& other : boids){
        Vector2 diff = Vector2Subtract(b.position, other.position);
        float d = Vector2Length(diff);
        if(d < match_radius && d > 0){
            steer = Vector2Add(steer, other.velocity);
            count++;
        }
    }
    if(count > 0){
        steer = Vector2Scale(steer, 1.0f/count);
        Vector2 diff = Vector2Subtract(steer, b.velocity);
        b.velocity = Vector2Add(b.velocity, Vector2Scale(diff, match_factor));
    }
}

void center(Boid& b,vector<Boid> boids){
    Vector2 steer = {0,0};
    int count = 0;
    for(Boid& other : boids){
        Vector2 diff = Vector2Subtract(b.position, other.position);
        float d = Vector2Length(diff);
        if(d < match_radius && d > 0){
            steer = Vector2Add(steer, other.velocity);
            count++;
        }
    }
    if(count > 0){
        steer = Vector2Scale(steer, 1.0f/count);
        Vector2 diff = Vector2Subtract(steer, b.velocity);
        b.velocity = Vector2Add(b.velocity, Vector2Scale(diff, match_factor));
    }
}

float clamp(float value, float min, float max){
    if(value < min){
        return min;
    }
    if(value > max){
        return max;
    }
    return value;
}


vector<Boid> boids;
int main(){
    const int screenWidth = 800;
    const int screenHeight = 600;

    InitWindow(screenWidth, screenHeight, "raylib [core] example - basic window");
    srand(time(NULL));
    for(int i = 0; i < 3000; i++){
        Boid b;
        b.position = { static_cast<float>(rand() % screenWidth), static_cast<float>(rand() % screenHeight) }; 
        b.last_position = b.position;
        b.velocity = { 2 + static_cast<float>(rand() % 2), 2 + static_cast<float>(rand() % 2) }; 
        if(rand() % 2 == 0)
            b.velocity.x *= -1;
        if(rand() % 2 == 0)
            b.velocity.y *= -1;
        boids.push_back(b);
    }
    

    SetTargetFPS(60);               

    // Main game loop
    while (!WindowShouldClose()){
        BeginDrawing();
        ClearBackground(RAYWHITE);
        for(Boid &b:boids){
            UpdatePositions(b);
            seperation(b,boids);
            alignment(b,boids);
            center(b,boids);
            DrawCircle(b.position.x, b.position.y, 1, BLACK);
        }
        int fps = GetFPS();
        DrawText(TextFormat("%i", fps), 10, 10, 20, BLACK);
        EndDrawing();
    }
}
