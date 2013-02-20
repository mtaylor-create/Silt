/*
 * Sifteo SDK Example.
 */

#include <sifteo.h>
#include <sifteo/math.h>
#include "assets.gen.h"


using namespace Sifteo;

//static AssetSlot MainSlot = AssetSlot::allocate().bootstrap(gigaGroup);

static AssetSlot UL = AssetSlot::allocate().bootstrap(ulGroup);
static AssetSlot UR = AssetSlot::allocate().bootstrap(urGroup);
static AssetSlot LL = AssetSlot::allocate().bootstrap(llGroup);
static AssetSlot LR = AssetSlot::allocate().bootstrap(lrGroup);

static Metadata M = Metadata()
    .title("gviewer")
    .package("com.sifteo.sdk.sensors", "1.1")
    .icon(Icon)
    .cubeRange(0, CUBE_ALLOCATION);

static VideoBuffer vid[CUBE_ALLOCATION];
static TiltShakeRecognizer motion[CUBE_ALLOCATION];

class SensorListener {
public:
    
    static const float panSpeed = 3.0f;
    
    static const int cubeWidth = 24;
    static const int scrWidth = 16;
    
    unsigned numCubes = 0;
    
    unsigned imWidth = 2 * ul.tileWidth() - scrWidth;
    unsigned imHeight = 2 * ul.tileHeight() - scrWidth;
    signed bWidth = ul.tileWidth() - scrWidth;
    signed bHeight = ul.tileHeight() - scrWidth;
    
    struct Counter {
        unsigned touch;
        unsigned neighborAdd;
        unsigned neighborRemove;
        unsigned orientation;
        signed x;
        signed y;
    } counters[CUBE_ALLOCATION];
    
    struct Pan {
        signed px;
        signed py;
        signed x;
        signed y;
    } pan;
    
    struct Free {
        bool l;
        bool r;
        bool d;
        bool u;
    } free;
    
    
    unsigned newest;
    
    void install()
    {
        Events::neighborAdd.set(&SensorListener::onNeighborAdd, this);
        Events::neighborRemove.set(&SensorListener::onNeighborRemove, this);
        Events::cubeAccelChange.set(&SensorListener::onAccelChange, this);
        Events::cubeTouch.set(&SensorListener::onTouch, this);
        Events::cubeBatteryLevelChange.set(&SensorListener::onBatteryChange, this);
        Events::cubeConnect.set(&SensorListener::onConnect, this);

        // Handle already-connected cubes
        for (CubeID cube : CubeSet::connected())
            onConnect(cube);
    }
    
    void update(TimeDelta ts)
    {

        
        unsigned mag = 0;
        unsigned idx = 0;
        
        pan.x = 0;
        pan.y = 0;
     
        
        Float2 accel;
        
        for (unsigned id = 0; id < numCubes; id++)
        {
            
            if (counters[id].x < 1)
            {
                counters[id].x = 0;
                free.l = false;
            }
            
            if (counters[id].x > imWidth - cubeWidth - 1)
            {
                counters[id].x = imWidth - cubeWidth;
                free.r = false;
            }
            
            if (counters[id].y < 1)
            {
                counters[id].y = 0;
                free.u = false;
            }
            
            if (counters[id].y > imHeight - cubeWidth - 1)
            {
                counters[id].y = imHeight - cubeWidth;
                free.d = false;
            }
            
            accel = vid[id].virtualAccel().xy();
            
            if (accel.len() > mag)
            {
                mag = accel.len();
                idx = id;
            }
            
        }
                
        accel = vid[idx].virtualAccel().xy();
        
        float tiltx = accel.x * panSpeed * ts;
        float tilty = accel.y * panSpeed * ts;

        pan.px += int(tiltx);
        pan.py += int(tilty);

        
        while ((pan.px>7) && (free.r))
        {
            pan.px -= 8;
            pan.x++;
        }
    
        while ((pan.px<0) && (free.l))
        {
            pan.px += 8;
            pan.x--;
        }
        
        while ((pan.py>7) && (free.d))
        {
            pan.py -= 8;
            pan.y++;
        }
        
        while ((pan.py<0) && (free.u))
        {
            pan.py += 8;
            pan.y--;
        }
        
        free.u = free.d = free.l = free.r = true;
        
        pan.px = clamp(pan.px, 0, 8);
        
        pan.py = clamp(pan.py, 0, 8);

        for (unsigned id = 0; id < numCubes; id++)
        {
            counters[id].x += pan.x;
            
            counters[id].y += pan.y;
            
        }
        
        drawNeighbors();
        
    }
    
private:
    void onConnect(unsigned id)
    {
        numCubes++;
        CubeID cube(id);
        uint64_t hwid = cube.hwID();

        bzero(counters[id]);
        counters[cube].x = (imWidth - cubeWidth)/2;
        counters[cube].y = (imHeight - cubeWidth)/2;
        LOG("Cube %d connected at %d, %d\n", id, counters[cube].x, counters[cube].y);

        vid[cube].initMode(BG0);
        vid[cube].attach(cube);
        
        // Draw initial state for all sensors
        onAccelChange(cube);
        onBatteryChange(cube);
        onTouch(cube);
      
    }

    void onBatteryChange(unsigned id)
    {
    }

    void onTouch(unsigned id)
    {
        newest=id;
    }


    
    void onAccelChange(unsigned id)
    {
    }

    void onNeighborRemove(unsigned firstID, unsigned firstSide, unsigned secondID, unsigned secondSide)
    {
        LOG("Neighbor Remove: %02x:%d - %02x:%d\n", firstID, firstSide, secondID, secondSide);

        if (firstID < arraysize(counters)) {
            counters[firstID].neighborRemove++;
            //drawNeighbors(firstID);
        }
        if (secondID < arraysize(counters)) {
            counters[secondID].neighborRemove++;
            //drawNeighbors(secondID);
        }
    }

    void onNeighborAdd(unsigned firstID, unsigned firstSide, unsigned secondID, unsigned secondSide)
    {
        LOG("Neighbor Add: %02x:%d:%d - %02x:%d:%d\n",
            firstID, firstSide, counters[firstID].orientation,
            secondID, secondSide, counters[secondID].orientation);
        
        
        if(newest==firstID) {
            counters[firstID].orientation=umod(secondSide
                                                      +counters[secondID].orientation
                                                      -firstSide
                                                      +2,4);
            
            switch (umod(firstSide + counters[firstID].orientation, 4)) {
            case 0:
                counters[firstID].y=counters[secondID].y+cubeWidth;
                counters[firstID].x=counters[secondID].x;
                break;
            case 1:
                counters[firstID].x=counters[secondID].x+cubeWidth;
                counters[firstID].y=counters[secondID].y;
                break;
            case 2:
                counters[firstID].y=counters[secondID].y-cubeWidth;
                counters[firstID].x=counters[secondID].x;
                break;
            case 3:
                counters[firstID].x=counters[secondID].x-cubeWidth;
                counters[firstID].y=counters[secondID].y;
                break;
                
            default:
                break;
            }
            
            
        }
        if (newest==secondID) {
            counters[secondID].orientation=umod(firstSide
                                                      +counters[firstID].orientation
                                                      -secondSide
                                                      +2,4);
            
            switch (umod(secondSide + counters[secondID].orientation, 4)) {
            case 0:
                counters[secondID].y=counters[firstID].y+cubeWidth;
                counters[secondID].x=counters[firstID].x;
                break;
            case 1:
                counters[secondID].x=counters[firstID].x+cubeWidth;
                counters[secondID].y=counters[firstID].y;
                break;
            case 2:
                counters[secondID].y=counters[firstID].y-cubeWidth;
                counters[secondID].x=counters[firstID].x;
                break;
            case 3:
                counters[secondID].x=counters[firstID].x-cubeWidth;
                counters[secondID].y=counters[firstID].y;
                break;
                
            default:
                break;
            }
        }


        if (firstID < arraysize(counters)) {
            counters[firstID].neighborAdd++;
            vid[firstID].initMode(BG0);
            vid[firstID].setOrientation(Side(umod(-counters[firstID].orientation,4)));
        }
        if (secondID < arraysize(counters)) {
            counters[secondID].neighborAdd++;
            vid[secondID].initMode(BG0);
            vid[secondID].setOrientation(Side(umod(-counters[secondID].orientation,4)));
        }
    }
        
    void drawNeighbors()
    {
            
        System::finish();
        
        for (unsigned cube = 0; cube < numCubes; cube++)
        {
            vid[cube].bg0.setPanning(vec(pan.px,pan.py));
            
            
            if (counters[cube].x >= bWidth) {
                if (counters[cube].y >= bHeight) {
                    vid[cube].bg0.image(vec(0,0), vec(18,18), lr, vec(counters[cube].x - bWidth,counters[cube].y - bHeight));
                }
                else {
                    vid[cube].bg0.image(vec(0,0), vec(18,18), ur, vec(counters[cube].x - bWidth,counters[cube].y));
                }
            }
            else {
                if (counters[cube].y >= bHeight) {
                    vid[cube].bg0.image(vec(0,0), vec(18,18), ll, vec(counters[cube].x,counters[cube].y - bHeight));
                }
                else {
                    vid[cube].bg0.image(vec(0,0), vec(18,18), ul, vec(counters[cube].x,counters[cube].y));
                }
            }
        }


    }
};


void main()
{
    static SensorListener sensors;

    sensors.install();
    
    TimeStep ts;
    
    while (1)
    {
        sensors.update(ts.delta());
        System::paint();
        ts.next();
    }
}
