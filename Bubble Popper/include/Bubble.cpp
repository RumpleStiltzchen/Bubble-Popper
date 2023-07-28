#pragma once
#include "raylib.h"
#include "definitions.h"
#include "Utils.cpp"
#include <vector>


struct Circle
{
    Circle(){}
    Circle(Vector2 pos_, float radius_)
    : pos(pos_), radius(radius_) {}
    Circle(Vector2 pos_, float radius_, Vector2 initAcc)
    : pos(pos_), radius(radius_), acc(initAcc) {}
    void Update()
    {
        vel+=acc;
        pos+=vel;
        acc.x = 0;
        acc.y = 0;

        if(fabs(vel.x) < 0.1f )
            vel.x = 0;
        if(fabs(vel.y) < 0.1f )
            vel.y = 0;
        vel *= 0.9;
    }
    void Draw()
    {
        DrawCircleV(pos, radius, BLACK);
    }
    float radius = 40;
    Vector2 pos = Vector2{0, 0};
    Vector2 vel = Vector2{0, 0};
    Vector2 acc = Vector2{0, 0};
    float friction = 0;
    float mass = 1;
};

struct Bubble:Circle
{
    Bubble(Vector2 pos_, float radius_, Vector2 initAcc, int type) : Circle(pos_, radius_, initAcc)
    {
        this->type = type;
        switch (type)
        {
        case NORMAL:
            this->health = 1;
            break;
        case SUPER:
            this->health = 1;
            break;
        case FROZEN:
            this->health = 10;
            break;
        case BIG:
            this->health = 30;
            this->radius = 100;
            this->mass = 8;
            break;
        default:
            this->health = 1;
            break;
        }
    }
    

    void Update(float angle)
    {
        if(type == SUPER)
            Super();
        if(type == FROZEN)
            Frozen();
        if(type == BIG)
            Big();


        if(pos.x > GetScreenWidth() * 0.7) // right
        {
            AddForce((VectorFromAngle(angle) * 0.01f) + Vector2{0, 0.01f});
        }
        if(pos.x < GetScreenWidth() * 0.3) // left
        {
            AddForce((VectorFromAngle(angle) * 0.01f) + Vector2{0, -0.01f});
        }
        if(pos.y > GetScreenHeight() * 0.5f) // bottom
        {
            AddForce((VectorFromAngle(angle) * 0.003f) + Vector2{-0.01f, 0});
        }
        if(pos.y < GetScreenHeight() * 0.5f) // bottom
        {
            AddForce((VectorFromAngle(angle) * 0.01f) + Vector2{0.01f, 0});
        }
        
        vel+=acc;
        pos+=vel;
        acc.x = 0;
        acc.y = 0;
        if(vel.x > 10) 
            vel.x = 10;
        if(vel.x < -10)
            vel.x = -10;
        if(vel.y > 10) 
            vel.y = 10;
        if(vel.y < -10)
            vel.y = -10;
        vel *= 0.99f;
    }
    void Big()
    {

    }
    void Super()
    {

    }
    void Frozen()
    {

    }
    void Draw()
    {
        if(type == SUPER)
        {
            DrawCircle(pos.x, pos.y, radius, ColorAlpha(ColorFromHSV(pos.y, 1, 1), 0.8f));
        }
        else
        {
            DrawCircle(pos.x, pos.y, radius, bubbleColor);
        }
        DrawCircleLines(pos.x, pos.y, radius, BLACK);

    }
    void AddForce(Vector2 force)
    {
        acc+=force;
    }
    Color bubbleColor = ColorAlpha(Color{100, 150, 255}, 0.7f);
    int health = 1;
    int type = 0;
};
struct Particle
{
    Particle(Vector2 pos,  Vector2 acc, int countdown)
    {
        this->pos = pos;
        this->vel = Vector2{0, 0};
        this->acc = acc;
        this->countdown = countdown;
        this->super = false;
    }
    Particle(Vector2 pos,  Vector2 acc, int countdown, bool super)
    {
        this->pos = pos;
        this->vel = Vector2{0, 0};
        this->acc = acc;
        this->countdown = countdown;
        this->super = super;

    }
    bool Update()
    {
        if(super)
            acc += Vector2(0, 4.9/GetRandomValue(300, 1000));
        else
            acc += Vector2(0, 4.9/GetRandomValue(144, 500));
        vel += acc;
        pos += vel;
        acc *= 0;
        countdown -= 1;
        return countdown > 0;
    }
    void Draw()
    {
        if(!super)
            DrawCircleV(pos, map(countdown, 0, 144, 0, 20), ColorAlpha(Color{140, 175, 255}, map(countdown, 0, 60, 0, 0.3f)));
        else
            DrawCircleV(pos, map(countdown, 0, 144, 0, 20), ColorAlpha(ColorFromHSV(pos.y, 1, 1), map(countdown, 0, 60, 0, 1.0f)));

    }
    float countdown;
    Vector2 pos;
    Vector2 vel;
    Vector2 acc;
    bool super;

};
struct ParticleExplosion
{
    ParticleExplosion(Vector2 pos)
    {
        this->pos = pos;
        int numParticles = GetRandomValue(3, 5);
        for (int i = 0; i < numParticles; i++)
        {
            particles.push_back(Particle(this->pos, RandUpVector()/GetRandomValue(1, 8) * 3, GetRandomValue(60, 244)));
        }
    }
    bool Update()
    {
        bool stillRunning = false;
        for (int i = 0; i < particles.size(); i++)
        {
            if(particles[i].Update())
                stillRunning = true;
        }
        
        
        return stillRunning;
        
    }
    void Draw()
    {
        for (int i = 0; i < particles.size(); i++)
        {
            particles[i].Draw();
        }
        
    }
    std::vector<Particle> particles;
    
    
    Vector2 pos;
};
struct SuperParticleExplosion:ParticleExplosion
{
    SuperParticleExplosion(Vector2 pos) : ParticleExplosion(pos)
    {
        this->pos = pos;
        int numParticles = GetRandomValue(10, 20);
        for (int i = 0; i < numParticles; i++)
        {
            
            particles.push_back(Particle(this->pos, randUnitVector() * GetRandomValue(4, 8), GetRandomValue(144, 366), true));
        }
    }
};


bool IsColliding(Circle* circle1, Circle* circle2)
{
    float dx = circle1->pos.x - circle2->pos.x;
    float dy = circle1->pos.y - circle2->pos.y;
    float distance = sqrt(dx * dx + dy * dy);

    return distance < circle1->radius + circle2->radius;
}
void ResolveCollision(Circle* circle1, Circle* circle2, std::vector<std::pair<Circle*, Circle*>>& collidingCircles)
{
    float dx = circle1->pos.x - circle2->pos.x;
    float dy = circle1->pos.y - circle2->pos.y;
    float distance = sqrt(dx * dx + dy * dy);
        
    if(distance >= circle1->radius + circle2->radius)
        return;
        //if actually colliding
    distance = (distance != 0) ? distance : 1;
    collidingCircles.push_back({circle1, circle2});
    float overlapAmount = (circle1->radius + circle2->radius) - distance;
    Vector2 direction = circle2->pos - circle1->pos;
    direction.normalizeThis();

    circle2->pos += direction * (overlapAmount/2);
    circle1->pos -= direction * (overlapAmount/2);
    
    
}


void AddBubble(std::vector<Bubble*>& bubbles, bool frozenLevel)
{
    
    int rand = GetRandomValue(0, 100);
    int type = NORMAL;
    if(rand < 6)
        type = SUPER;
    if(rand < 3)
        type = BIG;
    int radius = GetRandomValue(30, 50);
    Vector2 newPos, newAcc;
    rand = (GetRandomValue(0, 3));
    switch (rand)
    {
    case 0:
        newPos = Vector2{0, GetScreenHeight()/2};
        newAcc = Vector2{5, GetRandomValue(-10, 10)/10};
        break;
    case 1:
        newPos = Vector2{GetScreenWidth(), GetScreenHeight()/2};
        newAcc = Vector2{-5, GetRandomValue(-10, 10)/10};
        break;
    case 2:
        newPos = Vector2{GetScreenWidth()/2, 0};
        newAcc = Vector2{GetRandomValue(-10, 10)/10, 5};
        break;
    case 3:
        newPos = Vector2{GetScreenWidth()/2, GetScreenHeight()};
        newAcc = Vector2{GetRandomValue(-10, 10)/10, -5};
        break;
    
    default:
        newPos = Vector2{0, GetScreenHeight()/2};
        newAcc = Vector2{5, GetRandomValue(-10, 10)/10};
        break;
    }
    
    bubbles.push_back(new Bubble(newPos, radius, newAcc, type));
}

void AddBubble(std::vector<Bubble*>& bubbles, bool frozenLevel, Vector2 pos)
{
    int rand = GetRandomValue(0, 100);
    int type = NORMAL;
    if(rand < 6)
        type = SUPER;
    if(rand < 3)
        type = BIG;

    if(frozenLevel && type == NORMAL)
        type = FROZEN;
    
    int radius = GetRandomValue(30, 50);
    
    bubbles.push_back(new Bubble(Vector2{pos}, radius, randUnitVector(), type));
}
void AddBubble(std::vector<Bubble*>& bubbles, bool frozenLevel, Vector2 pos, bool isFromBig)
{
    int type = NORMAL;
    

    if(frozenLevel && type == NORMAL)
        type = FROZEN;
    
    int radius = GetRandomValue(20, 30);
    
    bubbles.push_back(new Bubble(Vector2{pos}, radius, randUnitVector(), type));
}
