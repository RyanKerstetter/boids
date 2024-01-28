#include <raylib.h>
#include <vector>
#include <iostream>
#include "raymath.h"
using namespace std;

#pragma once

int max_values = 4;
bool draw_lines = false;

template <typename T>
struct arr{
    T* data;
    int size;
};

class vectorizable{
public:
    static int id_counter;
    int id;
    bool updated = false;
    vectorizable(){
        id = id_counter++;
    }
    virtual Vector2& get_position() = 0;
    virtual void update() = 0;
    virtual void draw() = 0;
};

template <typename T>
class QuadNode{
    static_assert(std::is_base_of<vectorizable,T>::value,"T must be a subclass of vectorizable");
public:
    QuadNode* parent;
    QuadNode* children[4];
    vector<T> objects;
    int level;
    Rectangle bounds;
    QuadNode(QuadNode* parent,int level, Rectangle bounds){
        this->level = level;
        this->bounds = bounds;
        for(int i = 0; i < 4; i++){
            children[i] = nullptr;
        }
        this->parent = parent;
    }

    ~QuadNode(){
        for(int i = 0; i < 4; i++){
            delete children[i];
        }
    }

    void split(){
        float w = bounds.width / 2;
        float h = bounds.height / 2;
        float x = bounds.x;
        float y = bounds.y;
        children[0] = new QuadNode(this,level + 1, {x + w, y, w, h});
        children[1] = new QuadNode(this,level + 1, {x, y, w, h});
        children[2] = new QuadNode(this,level + 1, {x, y + h, w, h});
        children[3] = new QuadNode(this,level + 1, {x + w, y + h, w, h});
        for(int j = 0; j < objects.size(); j++){
            for(int i = 0; i < 4; i++){
                if(children[i]->add(objects[j]))
                    break;
            }
        }
    }

    bool add(T& t){
        if(!CheckCollisionPointRec(t.get_position(),bounds)){
            return false;
        }
        if(objects.size() < max_values){
            objects.push_back(t);
            return true;
        }
        if(children[0] == nullptr){
            split();
        }
        for(int i = 0; i < 4; i++){
            if(children[i]->add(t))
                return true;
        }
        return false;
    }

    void propogate_up(T& t){
        Vector2 p = t.get_position();
        if(!CheckCollisionPointRec(p,bounds)){
            if(parent == nullptr){
                cout << "Object is not in bounds" << endl;
                return;
            }
            parent->propogate_up(t);
            return;
        }
        if(objects.size() < max_values){
            objects.push_back(t);
            return;
        }
        if(children[0] == nullptr){
            split();
            for(int i = 0; i < 4; i++){
                if(children[i]->add(t))
                    return;
            }
        } else {
            for(int i = 0; i < 4; i++){
                if(children[i]->add(t))
                    return;
            }
        }
    }

    void collapse(){
        objects.clear();
        arr<T> array = get_objects();
        for(int i = 0; i < array.size; i++){
            Vector2 p = array.data[i].get_position();
        }
        for(int i = 0; i < 4; i++){
            delete children[i];
            children[i] = nullptr;
        }
        for(int i = 0; i < array.size; i++){
            if(!add(array.data[i])){
                cout << "Failed to add object" << endl;
                cout << "Object position: " << array.data[i].get_position().x << ", " << array.data[i].get_position().y << endl;
                cout << "Bounds: " << bounds.x << ", " << bounds.y << ", " << bounds.width << ", " << bounds.height << endl;
                cout << "array size: " << array.size << endl;
            }
        }
        delete[] array.data;
    }

    void update(){
        if(children[0] == nullptr){
            for(int i = 0; i < objects.size(); i++){
                if(objects[i].updated)
                    continue;
                objects[i].update();
                objects[i].updated = true;
                if(!CheckCollisionPointRec(objects[i].get_position(),bounds)){
                    propogate_up(objects[i]);
                    objects.erase(objects.begin() + i);
                }
            }
            return;
        } else{
            for(int i = 0; i < 4; i++){
                children[i]->update();
            }
        }
        if(get_amount() < max_values){
            arr<T> array = get_objects();
            for(int i = 0; i < array.size;i++){
                Vector2 p = array.data[i].get_position();
            }
            collapse();
        }
    }

    void done_updating(){
        if(level > 10)
            cout << "Level: " << level << endl;
        if(children[0] == nullptr){
            for(int i = 0; i < objects.size(); i++){
                objects[i].updated = false;
            }
            return;
        }
        for(int i = 0; i < 4; i++){
            children[i]->done_updating();
        }
    }

    void draw(){
        if(draw_lines){
            DrawRectangleLinesEx(bounds,1,RED);
        }
        if(children[0] == nullptr){
            for(int i = 0; i < objects.size(); i++){
                objects[i].draw();
            }
            return;
        }
        for(int i = 0; i < 4; i++){
            children[i]->draw();
        }
    }

    int get_amount(){
        if(children[0] == nullptr){
            int amount = objects.size();
            return amount;
        }
        int amount = 0;
        for(int i = 0; i < 4; i++){
            amount += children[i]->get_amount();
        }
        return amount;
    }

    arr<T> get_objects(){
        int amount = get_amount();
        arr<T> array = {new T[amount],amount};
        if(children[0] == nullptr){
            for(int i = 0; i < amount; i++){
                array.data[i] = this->objects[i];
            }
            return array;
        }
        int count = 0;
        for(int i = 0; i < 4; i++){
            arr<T> child_arr = children[i]->get_objects();
            for(int j = 0; j < child_arr.size; j++){
                array.data[count++] = child_arr.data[j];
            }
            delete[] child_arr.data;
        }
        return array;
    }

    void get_in_range(Vector2 p,float dist,vector<T*>* accum){
        if(!CheckCollisionCircleRec(p,dist,bounds)){
            return;
        }
        float d = dist * dist;
        if(children[0] == nullptr){
            for(int i = 0; i < objects.size(); i++){
                Vector2 o = objects[i].get_position(); 
                double off_x = o.x - p.x;
                double off_y = o.y - p.y;
                double sq = off_x * off_x + off_y * off_y;
                if(sq < d){
                    accum->push_back(&objects[i]);
                }
            }
            return; 
        }
        for(int i = 0; i < 4; i++){
            QuadNode* child = children[i];
            child->get_in_range(p,dist,accum);
        }
    }
};

template <typename T>
class QuadTree{
    static_assert(std::is_base_of<vectorizable,T>::value,"T must be a subclass of vectorizable");
public:
    QuadNode<T>* root;
    QuadTree(Rectangle bounds){
        root = new QuadNode<T>(nullptr,0,bounds);
    }

    void add(T& t){
        root->add(t);
    }

    void update(){
        root->update();
        root->done_updating();
    }

    void draw(){
        root->draw();
    }

    vector<T*> get_in_range(Vector2 p,float dist){
        vector<T*> accum;
        root->get_in_range(p,dist,&accum);
        return accum;
    }

    int get_amount(){
        return root->get_amount();
    }

    arr<T> get_objects(){
        return root->get_objects();
    }
};
