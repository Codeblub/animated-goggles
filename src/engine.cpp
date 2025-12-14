#include <SDL2/SDL.h>
#include <vector>
#include <cmath>
#include <cstdint>
#include <algorithm>

// =======================
// Math
// =======================
struct Vec3 {
    float x,y,z;
    Vec3 operator+(const Vec3& o) const { return {x+o.x,y+o.y,z+o.z}; }
    Vec3 operator-(const Vec3& o) const { return {x-o.x,y-o.y,z-o.z}; }
    Vec3 operator*(float s) const { return {x*s,y*s,z*s}; }
};

static Vec3 cross(const Vec3&a,const Vec3&b){
    return {a.y*b.z-a.z*b.y, a.z*b.x-a.x*b.z, a.x*b.y-a.y*b.x};
}
static float dot(const Vec3&a,const Vec3&b){
    return a.x*b.x+a.y*b.y+a.z*b.z;
}
static Vec3 normalize(Vec3 v){
    float l = std::sqrt(dot(v,v));
    if(l==0) return v;
    return {v.x/l,v.y/l,v.z/l};
}

// =======================
// Camera
// =======================
struct Camera {
    Vec3 pos{0,1.7f,-5};
    float yaw = 0;
    float pitch = 0;
    float speed = 6.0f;
    float sens  = 0.002f;

    Vec3 forward() const {
        return normalize({
            std::cos(pitch)*std::sin(yaw),
            std::sin(pitch),
            std::cos(pitch)*std::cos(yaw)
        });
    }
    Vec3 right() const {
        return normalize(cross(forward(),{0,1,0}));
    }
};

// =======================
// Triangle
// =======================
struct Tri {
    Vec3 p[3];
    uint32_t color;
};

// =======================
// Engine Globals
// =======================
const int W = 1280;
const int H = 720;

uint32_t framebuffer[W*H];
float zbuffer[W*H];

// =======================
// Software Rasterizer
// =======================
static void clear(uint32_t color){
    std::fill(framebuffer, framebuffer+W*H, color);
    std::fill(zbuffer, zbuffer+W*H, 1e9f);
}

static void putPixel(int x,int y,float z,uint32_t c){
    if(x<0||y<0||x>=W||y>=H) return;
    int i = y*W+x;
    if(z < zbuffer[i]){
        zbuffer[i] = z;
        framebuffer[i] = c;
    }
}

static Vec3 project(const Vec3& v){
    float f = 600;
    return {
        v.x/v.z * f + W/2,
        -v.y/v.z * f + H/2,
        v.z
    };
}

static void drawTri(const Tri& t){
    Vec3 p[3];
    for(int i=0;i<3;i++){
        if(t.p[i].z <= 0.1f) return;
        p[i] = project(t.p[i]);
    }

    int minx = std::max(0,(int)std::floor(std::min({p[0].x,p[1].x,p[2].x})));
    int maxx = std::min(W-1,(int)std::ceil (std::max({p[0].x,p[1].x,p[2].x})));
    int miny = std::max(0,(int)std::floor(std::min({p[0].y,p[1].y,p[2].y})));
    int maxy = std::min(H-1,(int)std::ceil (std::max({p[0].y,p[1].y,p[2].y})));

    auto edge=[&](const Vec3&a,const Vec3&b,const Vec3&c){
        return (c.x-a.x)*(b.y-a.y)-(c.y-a.y)*(b.x-a.x);
    };

    for(int y=miny;y<=maxy;y++){
        for(int x=minx;x<=maxx;x++){
            Vec3 pnt{(float)x,(float)y,0};
            float w0=edge(p[1],p[2],pnt);
            float w1=edge(p[2],p[0],pnt);
            float w2=edge(p[0],p[1],pnt);
            if(w0>=0&&w1>=0&&w2>=0){
                float z=(p[0].z+p[1].z+p[2].z)/3;
                putPixel(x,y,z,t.color);
            }
        }
    }
}

// =======================
// Terrain
// =======================
std::vector<Tri> terrain;

static float height(int x,int z){
    return 0.6f * std::sin(x*0.2f) * std::cos(z*0.2f);
}

static void buildTerrain(){
    for(int z=0;z<80;z++){
        for(int x=0;x<80;x++){
            Vec3 p0{x*1.0f, height(x,z), z*1.0f};
            Vec3 p1{(x+1)*1.0f, height(x+1,z), z*1.0f};
            Vec3 p2{x*1.0f, height(x,z+1), (z+1)*1.0f};
            Vec3 p3{(x+1)*1.0f, height(x+1,z+1), (z+1)*1.0f};

            uint32_t grass = 0xFF2E8B57;

            terrain.push_back({p0,p1,p2,grass});
            terrain.push_back({p1,p3,p2,grass});
        }
    }
}

// =======================
// Main
// =======================
int main(){
    SDL_Init(SDL_INIT_VIDEO);
    SDL_Window* win = SDL_CreateWindow(
        "My Engine",
        SDL_WINDOWPOS_CENTERED,SDL_WINDOWPOS_CENTERED,
        W,H,0
    );
    SDL_Renderer* ren = SDL_CreateRenderer(win,-1,0);
    SDL_Texture* tex = SDL_CreateTexture(
        ren, SDL_PIXELFORMAT_ARGB8888,
        SDL_TEXTUREACCESS_STREAMING,W,H
    );

    SDL_SetRelativeMouseMode(SDL_TRUE);

    Camera cam;
    buildTerrain();

    bool run=true;
    uint32_t last=SDL_GetTicks();

    while(run){
        uint32_t now=SDL_GetTicks();
        float dt=(now-last)/1000.0f;
        last=now;

        SDL_Event e;
        while(SDL_PollEvent(&e)){
            if(e.type==SDL_QUIT) run=false;
            if(e.type==SDL_MOUSEMOTION){
                cam.yaw   += e.motion.xrel * cam.sens;
                cam.pitch -= e.motion.yrel * cam.sens;
                cam.pitch = std::clamp(cam.pitch,-1.5f,1.5f);
            }
        }

        const uint8_t* k=SDL_GetKeyboardState(nullptr);
        Vec3 dir{0,0,0};
        if(k[SDL_SCANCODE_W]) dir = dir + cam.forward();
        if(k[SDL_SCANCODE_S]) dir = dir - cam.forward();
        if(k[SDL_SCANCODE_A]) dir = dir - cam.right();
        if(k[SDL_SCANCODE_D]) dir = dir + cam.right();
        if(k[SDL_SCANCODE_SPACE]) dir.y+=1;
        if(k[SDL_SCANCODE_LCTRL]) dir.y-=1;
        if(dot(dir,dir)>0) cam.pos = cam.pos + normalize(dir)*cam.speed*dt;

        clear(0xFF204857); // sky

        for(const Tri& t:terrain){
            Tri tr=t;
            for(int i=0;i<3;i++){
                tr.p[i] = tr.p[i] - cam.pos;
                float x=tr.p[i].x, z=tr.p[i].z;
                tr.p[i].x = x*std::cos(cam.yaw) - z*std::sin(cam.yaw);
                tr.p[i].z = x*std::sin(cam.yaw) + z*std::cos(cam.yaw);
            }
            drawTri(tr);
        }

        SDL_UpdateTexture(tex,nullptr,framebuffer,W*4);
        SDL_RenderClear(ren);
        SDL_RenderCopy(ren,tex,nullptr,nullptr);
        SDL_RenderPresent(ren);
    }

    SDL_Quit();
    return 0;
}
