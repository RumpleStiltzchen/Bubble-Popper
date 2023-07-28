#include "Bubble.cpp"
#include "raylib.h"
#include "Utils.cpp"
#include <vector>
#include <fstream>



bool Pop(std::pair<bool, Vector2>& bigBubbleBursted, std::pair<bool, Vector2>& superBubbleBursed, std::vector<Bubble*>& bubbles, std::vector<ParticleExplosion*>& effects, int i, std::vector<Sound>& popSounds, Sound bigBounce, std::vector<Sound>& bigPops, int BubblesPopped);

//------------------------------------------------------------------------------------
// Program main entry point
//------------------------------------------------------------------------------------
int main(void)
{
    // Initialization
    //--------------------------------------------------------------------------------------

    // Set up the hook

    std::ifstream optionsFile("options.txt");
    std::string s;
    getline(optionsFile, s);
    getline(optionsFile, s);
    getline(optionsFile, s);
    getline(optionsFile, s);
    s.clear();
    getline(optionsFile, s);
    bool playMusic = s == std::string("true");
    s.clear();
    int screenHeight, screenWidth;
    getline(optionsFile, s);    
    try
    {
        screenWidth = stoi(s) - 4;
    }
    catch(const std::exception& e)
    {
        screenWidth = 1920 - 4;
    }
    
    s.clear();
    getline(optionsFile, s);    
    try
    {
        screenHeight = stoi(s) - 4;
    }
    catch(const std::exception& e)
    {
        screenHeight = 1080 - 4;
    }

    if(screenWidth < 100)
        screenWidth = 100;
    if(screenHeight < 100)
        screenHeight = 100;
    
    bool playing = true;
    int level = 0;
    bool respawing = false;
    int BubblesPopped = 0;
    int curSong = 0;
    float timePlayed = 0.0f;
    float volume = 0.3f;
    float angle = 0;

    SetConfigFlags(FLAG_MSAA_4X_HINT | FLAG_WINDOW_TRANSPARENT | FLAG_WINDOW_UNDECORATED | FLAG_WINDOW_TOPMOST | FLAG_WINDOW_MOUSE_PASSTHROUGH);

    InitWindow(1920, 1080, "BubblePopper");
    SetWindowSize(screenWidth, screenHeight);
    SetWindowPosition(2, 2);
    SetConfigFlags(FLAG_WINDOW_MOUSE_PASSTHROUGH);
    InitAudioDevice();
    SetExitKey(NULL);
    std::vector<Sound> popSounds = 
    {
        LoadSound("./resources/pops/1.wav"),
        LoadSound("./resources/pops/2.wav"),
        LoadSound("./resources/pops/3.wav"),
        LoadSound("./resources/pops/4.wav"),
        LoadSound("./resources/pops/5.wav")
    };
    Sound LevelComplete = LoadSound("./resources/Effects/Less aggressive Bubble Popper Level complete.wav");
    Sound bigBounce = LoadSound("./resources/pops/Bounce.wav");
    Sound chimes = LoadSound("./resources/pops/Chime1.wav");
    std::vector<Sound> bigPops =
    {
        LoadSound("./resources/pops/BIGPOP1.wav"),
        LoadSound("./resources/pops/BIGPOP2.wav")
    };
    std::vector<Music> songs = 
    {
        LoadMusicStream("./resources/Music/Bubble popper.wav"),
        LoadMusicStream("./resources/Music/Level Theme V2.wav"),
        LoadMusicStream("./resources/Music/Level Theme V4.wav"),
        LoadMusicStream("./resources/Music/BubblePopper4.wav")

    };

    
    std::vector<Bubble*> bubbles;
    std::vector<ParticleExplosion*> effects;
    
    
    
    for (int i = 0; i < songs.size(); i++)
    {
        songs[i].looping = false;
        SetMusicVolume(songs[i], 0.3f); 
    }
    for (int i = 0; i < popSounds.size(); i++)
    {
        SetSoundVolume(popSounds[i], 0.1f);
    }
    for (int i = 0; i < bigPops.size(); i++)
    {
        SetSoundVolume(bigPops[i], 0.3f);
    }
    
    SetSoundVolume(bigBounce, 0.5f);
    SetSoundVolume(popSounds[4], 0.5f);
    SetSoundVolume(LevelComplete, 0.5f);
    curSong = GetRandomValue(0, 3);
    PlayMusicStream(songs[curSong]);
    
    SetTargetFPS(144);               // Set our game to run at 60 frames-per-second
    //--------------------------------------------------------------------------------------

    // Main game loop
    while (!WindowShouldClose())    // Detect window close button or ESC key
    {
        
        if(IsWindowMinimized())
        {
            playing = false;
        }else if(IsWindowFocused())
        {
            playing = true;
        }
        
        // Update
        //----------------------------------------------------------------------------------
        std::vector<std::pair<Circle*, Circle*>> collidingCircles;
        std::pair<bool, Vector2> bigBubbleBursted;
        std::pair<bool, Vector2> superBubbleBursted;
        superBubbleBursted.first = false;
        bigBubbleBursted.first = false;
        if(!playing)
        {

            goto skip;
            
        }
        angle+=0.001f;
        if(playMusic)
            UpdateMusicStream(songs[curSong]);
        timePlayed = GetMusicTimePlayed(songs[curSong])/GetMusicTimeLength(songs[curSong]);
        
        if(timePlayed > 0.95f)
            SetMusicVolume(songs[curSong], ((timePlayed - 0.99f <= 0.3f) ? ((timePlayed - 0.99f) * 10) : 0.3f));
        else if(timePlayed < 0.05f)
            SetMusicVolume(songs[curSong], map(timePlayed, 0.0f, 0.01f, 0.0f, 0.3f));
        else
            SetMusicVolume(songs[curSong], 0.3f);
        
        if(timePlayed >= 0.99f)
        {
            curSong++;
            if(curSong >= 4)
                curSong = 0;
            PlayMusicStream(songs[curSong]);
        }
        if(bubbles.size() < 50 && !respawing)
        {
            level++;
            if(level != 1)
                PlaySound(LevelComplete);
            respawing = true;
        }
        if(respawing)
        {
            AddBubble(bubbles, false);
            if(bubbles.size() >= 200)
                respawing = false;
        }
        
        for (size_t i = 0; i < bubbles.size(); i++)
        {
            for (size_t j = 0; j < bubbles.size(); j++)
            {
                if( i != j)
                    ResolveCollision(bubbles[i], bubbles[j], collidingCircles);
                
            }
            

            bubbles[i]->Update(angle);

            if(bubbles[i]->pos.x < bubbles[i]->radius)
            {
                bubbles[i]->pos.x = bubbles[i]->radius;
                bubbles[i]->vel.x = fabs(bubbles[i]->vel.x) * 0.9;
            }
            if(bubbles[i]->pos.x > screenWidth-bubbles[i]->radius)
            {
                bubbles[i]->pos.x = screenWidth-bubbles[i]->radius;
                bubbles[i]->vel.x = -fabs(bubbles[i]->vel.x) * 0.9;
            }
            if(bubbles[i]->pos.y < bubbles[i]->radius)
            {
                bubbles[i]->pos.y = bubbles[i]->radius;
                bubbles[i]->vel.y = fabs(bubbles[i]->vel.y) * 0.9;
            }
            if(bubbles[i]->pos.y > screenHeight-bubbles[i]->radius)
            {
                bubbles[i]->pos.y = screenHeight-bubbles[i]->radius;
                bubbles[i]->vel.y = -fabs(bubbles[i]->vel.y) * 0.9;
            }
            
            

        }
        

        //Collision
        for (auto o : collidingCircles)
        {
            Circle* b1 = o.first;
            Circle* b2 = o.second;

            float fDistance = sqrtf((b1->pos.x - b2->pos.x)*(b1->pos.x - b2->pos.x) + (b1->pos.y - b2->pos.y)*(b1->pos.y - b2->pos.y));

			// Normal
			float nx = (b2->pos.x - b1->pos.x) / fDistance;
			float ny = (b2->pos.y - b1->pos.y) / fDistance;


            float kx = (b1->vel.x - b2->vel.x);
			float ky = (b1->vel.y - b2->vel.y);
			float p = 2.0 * (nx * kx + ny * ky) / (b1->mass + b2->mass);
			b1->vel.x = b1->vel.x - p * b2->mass * nx;
			b1->vel.y = b1->vel.y - p * b2->mass * ny;
			b2->vel.x = b2->vel.x + p * b1->mass * nx;
			b2->vel.y = b2->vel.y + p * b1->mass * ny;

        }
        
        //On Click
        for (int i = bubbles.size()-1; i >= 0; i--)
        {
            
            float dx = bubbles[i]->pos.x - GetMousePosition().x;
            float dy = bubbles[i]->pos.y - GetMousePosition().y;
            float distance = sqrt(fabs(dx * dx + dy * dy));
            if(distance <= bubbles[i]->radius)
            {
                
                
                if(IsMouseButtonDown(MOUSE_BUTTON_LEFT))
                {

                    if(Pop(bigBubbleBursted, superBubbleBursted, bubbles, effects, i, popSounds, bigBounce, bigPops, BubblesPopped))
                    {
                        BubblesPopped++;
                        break;
                    }
                    else if(bubbles[i]->health > 0)
                    {
                        if(bubbles[i]->type == BIG)
                        {
                            effects.push_back(new ParticleExplosion(bubbles[i]->pos));
                            PlaySound(bigBounce);
                        }
                    }
                }
                
                
                
                
            }
            
        
        }
        
        for (int i = effects.size()-1; i >= 0 ; i--)
        {
            if(!effects[i]->Update())
            {
                delete effects[i];
                effects.erase(effects.begin() + i);
            }
        }   
        if(bigBubbleBursted.first)
        {
            for (size_t b = 0; b < 18; b++)
            {
                float x = cos(b*20*(PI/180)) * 5;
                float y = sin(b*20*(PI/180)) * 5;
                AddBubble(bubbles, false, bigBubbleBursted.second + Vector2{x, y}, true);
            }
            
            TraceLog(LOG_INFO, "BIG BUBBLE POP!");
            bigBubbleBursted.first = false;
        }
        if(superBubbleBursted.first)
        {
            
            
            
            SetSoundVolume(chimes, 0.4);
            PlaySound(chimes);
            for (int i = bubbles.size()-1; i >= 0; i--)
            {
                Vector2 diff = bubbles[i]->pos - superBubbleBursted.second;
                if(diff.magnitude() < 300)
                {
                    if(bubbles[i]->type == SUPER)
                        continue;
                    if(bubbles[i]->type == BIG)
                    {
                        bubbles[i]->health -= 2;
                        if(bubbles[i]->health <= 0)
                        {
                            for (size_t b = 0; b < 18; b++)
                            {
                                float x = cos(b*20*(PI/180)) * 5;
                                float y = sin(b*20*(PI/180)) * 5;
                                AddBubble(bubbles, false, bubbles[i]->pos + Vector2{x, y}, true);
                            }
                            effects.push_back(new SuperParticleExplosion(bubbles[i]->pos));
                            delete bubbles[i];
                            bubbles.erase(bubbles.begin() + i);
                        }
                        continue;
                    }
                    if(bubbles[i]->type == FROZEN)
                    {
                        bubbles[i]->health -= 2;
                        if(bubbles[i]->health <= 0)
                        {
                            effects.push_back(new SuperParticleExplosion(bubbles[i]->pos));
                            delete bubbles[i];
                            bubbles.erase(bubbles.begin() + i);
                        }
                        continue;
                    }
                    
                    effects.push_back(new SuperParticleExplosion(bubbles[i]->pos));
                    delete bubbles[i];
                    bubbles.erase(bubbles.begin() + i);

                }
            }
            
        }

        // Draw
        //----------------------------------------------------------------------------------
        skip:
            
        BeginDrawing();

            ClearBackground(BLANK);
            for (size_t i = 0; i < effects.size(); i++)
            {
                effects[i]->Draw();
            }
            for (size_t i = 0; i < bubbles.size(); i++)
            {
            
                bubbles[i]->Draw();
            }


        EndDrawing();
        
            
        
        //----------------------------------------------------------------------------------
        
        collidingCircles.empty();
        
    }

    // De-Initialization
    //--------------------------------------------------------------------------------------
    CloseWindow();        // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

    return 0;
}

bool Pop(std::pair<bool, Vector2>& bigBubbleBursted, std::pair<bool, Vector2>& superBubbleBursed, std::vector<Bubble*>& bubbles, std::vector<ParticleExplosion*>& effects, int i, std::vector<Sound>& popSounds, Sound bigBounce, std::vector<Sound>& bigPops, int BubblesPopped)
{
    bubbles[i]->health--;
    if(bubbles[i]->health <= 0)
    {
        BubblesPopped++;
        for (int j = 0; j < bubbles.size(); j++)
        {
            if(i != j)
            {
                float dx = bubbles[j]->pos.x - bubbles[i]->pos.x;
                float dy = bubbles[j]->pos.y - bubbles[i]->pos.y;
                float distance = sqrt(fabs(dx * dx + dy * dy));
                distance = (distance != 0) ? distance : 1;
                if(distance < bubbles[i]->radius + bubbles[j]->radius + 50)
                {
                    Vector2 dir = normalize(bubbles[i]->pos - bubbles[j]->pos); 
                    bubbles[j]->AddForce(dir * -3);
                }
            }
        }
        effects.push_back(new ParticleExplosion(bubbles[i]->pos));
        if(bubbles[i]->type == BIG)
        {
            
            bigBubbleBursted.first = true;
            bigBubbleBursted.second = bubbles[i]->pos;
        }
        if(bubbles[i]->radius > 35)
        {
            if(bubbles[i]->type == SUPER)
                PlaySound(popSounds[getRandomIntInRangeFast(0, 3)]);
            if(bubbles[i]->type == BIG)
                PlaySound(bigPops[getRandomIntInRangeFast(0, 1)]);
            if(bubbles[i]->type == NORMAL)
                PlaySound(popSounds[getRandomIntInRangeFast(0, 3)]);

        }
        else
        {
            PlaySound(popSounds[4]);
        }
        if(bubbles[i]->type == SUPER)
        {
            superBubbleBursed.first = true;
            superBubbleBursed.second = bubbles[i]->pos;            
        }


                        
                    
        delete bubbles[i];
        bubbles.erase(bubbles.begin()+i);
        
        return true;

    }
    return false;
}
