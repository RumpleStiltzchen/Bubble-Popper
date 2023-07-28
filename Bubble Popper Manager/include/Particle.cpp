#pragma once
#include "raylib.h"
#include <vector>
class Particle
{
    public:
        Particle(Vector2 pos_, float r_)
        : pos(pos_), r(r_)
        {}

        void update(Vector2 AttractionPoint, int width, int height, std::vector<Particle*>& _particles)
        {
            // float dist = (pos - AttractionPoint).magnitude();
            // Vector2 dir = (pos - AttractionPoint).normalize();
            // if(dist > 20)
            //     acc = acc +( dir * (float)((9.8*(1/dist))/60) * 10);

            for(int i = 0; i < _particles.size(); i++)
            {
                float dist = (pos - _particles[i]->pos).magnitude();
                Vector2 dir = (pos - _particles[i]->pos).normalize();
                if(dist > 5 && dist < 200)
                    acc = acc +( dir * (float)((9.8*(1/dist))/60));

            }



            vel = vel + acc;

            if(pos.x > width - r)
            {
                pos.x = width - r;
                vel.x = vel.x * -0.8;
            }
            if(pos.x < r)
            {
                pos.x = r;
                vel.x = vel.x * -0.8;
            }
            if(pos.y > height - r)
            {
                pos.y = height - r;
                vel.y = vel.y * -0.8;
            }
            if(pos.y < r)
            {
                pos.y = r;
                vel.y = vel.y * -0.8;
            }

            if (vel.x > 10)
                vel.x = 10;
            
            if (vel.x < -10)
                vel.x = -10;
                
            
            if (vel.y > 10)
                vel.y = 10;
                
            
            if (vel.y < -10)
                vel.y = -10;
                
            pos = pos + vel;
            acc.x = 0;
            acc.y = 0;
        }

        void show(int frame_)
        {
            Color lineColor = ColorFromHSV((frame_/20) % 360, 1, 1);
            DrawCircleV(pos, r, lineColor);
        }


    public:
        float r;
        Vector2 vel, acc, pos;

};