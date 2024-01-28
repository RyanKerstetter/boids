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
    Vector2 last_position;
    Vector2 position;
    Vector2 velocity;
    
    Vector2& get_position() override{
        return position;
    }

    void update() override{
        amount_updated++;
        UpdatePositions();
        seperation(tree->get_in_range(position, avoid_radius));
        vector<Boid*> boids = tree->get_in_range(position, match_radius);
        align(boids);
        center(boids);
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

    void seperation(vector<Boid*> boids){
        Vector2 steer = {0,0};
        for(Boid* other : boids){
            Vector2 diff = Vector2Subtract(position, other->position);
            diff = Vector2Scale(Vector2Normalize(diff),avoid_radius);
            steer = Vector2Add(steer, diff);
        }
        velocity.x += steer.x * avoid_factor;
        velocity.y += steer.y * avoid_factor;
    }

    void align(vector<Boid*> boids){
        Vector2 sum = {0,0};
        int count = 0;
        for(Boid* other : boids){
            sum.x += other->velocity.x;
            sum.y += other->velocity.y;
            count++;
        }
        if(count > 0){
            sum.x = sum.x / count - velocity.x;
            sum.y = sum.y / count - velocity.y;
            velocity.x += sum.x * match_factor;
            velocity.y += sum.y * match_factor;
        }
    }

    void center(vector<Boid*> boids){
        Vector2 sum = {0,0};
        int count = 0;
        for(Boid* other : boids){
            sum.x += other->position.x;
            sum.y += other->position.y;
            count++;
        }
        if(count > 0){
            sum.x = sum.x / count - position.x;
            sum.y = sum.y / count - position.y;
            velocity.x += sum.x * center_factor;
            velocity.y += sum.y * center_factor;
        }
    }

    void draw() override{
        amount_updated++;
        DrawRectangle(position.x, position.y, 1, 1, BLACK);
    }
};

