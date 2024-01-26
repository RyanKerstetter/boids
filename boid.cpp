#include "raylib.h"
#include "raymath.h"
#include "qtree.cpp"

float dt = 2;
float max_speed = 2;
float min_speed = 1;
float avoid_radius = 4;
float avoid_factor = .05;
float match_radius = 16;
float match_factor = .1;
float center_radius = 16;
float center_factor = .01;

float clamp(float value, float min, float max){
    if(value < min){
        return min;
    }
    if(value > max){
        return max;
    }
    return value;
}

class Boid: public vectorizable{
public:
    static QuadTree<Boid>* tree;
    static int amount_updated;
    bool updated = false;
    Color color = BLACK;
    Vector2 last_position;
    Vector2 position;
    Vector2 velocity;
    
    Vector2 get_position() override{
        return position;
    }

    void update() override{
        amount_updated++;
        UpdatePositions();
        seperation(*this, tree->get_in_range(position, avoid_radius));
        align(*this, tree->get_in_range(position, match_radius));
        center(*this, tree->get_in_range(position, center_radius));
    }

    void UpdatePositions(){
        last_position = position;
        float speed = Vector2Length(velocity);
        if(speed > max_speed){
            velocity = Vector2Scale(Vector2Normalize(velocity), max_speed);
        } else if(speed < min_speed){
            velocity = Vector2Scale(Vector2Normalize(velocity), min_speed);
        }
        position.x = (position.x * 2) - last_position.x + (velocity.x * dt);
        position.y = (position.y * 2) - last_position.y + (velocity.y * dt);
        position.x = position.x < 0 ? position.x + 800 : position.x;
        position.y = position.y < 0 ? position.y + 600 : position.y;
        position.x = fmod(position.x, 800);
        position.y = fmod(position.y, 600);
    }

    void seperation(Boid& b,vector<Boid> boids){
        Vector2 steer = {0,0};
        for(Boid& other : boids){
            Vector2 diff = Vector2Subtract(b.position, other.position);
            diff = Vector2Scale(Vector2Normalize(diff),avoid_radius);
            steer = Vector2Add(steer, diff);
        }
        b.velocity = Vector2Add(b.velocity, Vector2Scale(steer, avoid_factor));
    }

    void align(Boid& b,vector<Boid> boids){
        Vector2 sum = {0,0};
        int count = 0;
        for(Boid& other : boids){
            sum = Vector2Add(sum, other.velocity);
            count++;
        }
        if(count > 0){
            sum = Vector2Scale(sum, 1.0f / count);
            Vector2 diff = Vector2Subtract(sum, b.velocity);
            b.velocity = Vector2Add(b.velocity, Vector2Scale(diff, match_factor));
        }
    }

    void center(Boid& b,vector<Boid> boids){
        Vector2 sum = {0,0};
        int count = 0;
        for(Boid& other : boids){
            sum = Vector2Add(sum, other.position);
            count++;
        }
        if(count > 0){
            sum = Vector2Scale(sum, 1.0f / count);
            Vector2 diff = Vector2Subtract(sum, b.position);
            b.velocity = Vector2Add(b.velocity, Vector2Scale(diff, center_factor));
        }
    }

    void draw() override{
        amount_updated++;
        //cout << "position: " << position.x << ", " << position.y << endl;
        //cout << "velocity: " << velocity.x << ", " << velocity.y << endl;
        DrawCircle(position.x, position.y, 1, BLACK);
    }
};

