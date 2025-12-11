// engine.cpp
// Minimal CPU software renderer - your engine from scratch.
// Requires SDL2 for window/present only.
// Build: g++ engine.cpp -O2 -std=c++17 -lSDL2 -o engine

#include <SDL2/SDL.h>
#include <cmath>
#include <cstdint>
#include <vector>
#include <array>
#include <limits>
#include <chrono>
#include <iostream>

// ----------------- minimal math -----------------
struct Vec3 {
    float x,y,z;
    Vec3() : x(0),y(0),z(0) {}
    Vec3(float X,float Y,float Z):x(X),y(Y),z(Z){}
    Vec3 operator+(const Vec3& o) const { return Vec3(x+o.x,y+o.y,z+o.z); }
    Vec3 operator-(const Vec3& o) const { return Vec3(x-o.x,y-o.y,z-o.z); }
    Vec3 operator*(float s) const { return Vec3(x*s,y*s,z*s); }
};
inline Vec3 cross(const Vec3& a,const Vec3& b){
    return Vec3(a.y*b.z - a.z*b.y, a.z*b.x - a.x*b.z, a.x*b.y - a.y*b.x);
}
inline float dot(const Vec3& a,const Vec3& b){ return a.x*b.x + a.y*b.y + a.z*b.z; }
inline Vec3 normalize(const Vec3& v){
    float l = std::sqrt(dot(v,v));
    if(l==0) return v;
    return v*(1.0f/l);
}

struct Vec4 { float x,y,z,w; Vec4():x(0),y(0),z(0),w(1){} Vec4(float X,float Y,float Z,float W=1.0f):x(X),y(Y),z(Z),w(W){} };

struct Mat4 {
    float m[4][4];
    static Mat4 identity(){
        Mat4 r{};
        for(int i=0;i<4;i++) for(int j=0;j<4;j++) r.m[i][j] = (i==j)?1.0f:0.0f;
        return r;
    }
    Vec4 mul(const Vec4& v) const {
        Vec4 r;
        r.x = m[0][0]*v.x + m[0][1]*v.y + m[0][2]*v.z + m[0][3]*v.w;
        r.y = m[1][0]*v.x + m[1][1]*v.y + m[1][2]*v.z + m[1][3]*v.w;
        r.z = m[2][0]*v.x + m[2][1]*v.y + m[2][2]*v.z + m[2][3]*v.w;
        r.w = m[3][0]*v.x + m[3][1]*v.y + m[3][2]*v.z + m[3][3]*v.w;
        return r;
    }
    Mat4 operator*(const Mat4& b) const {
        Mat4 r{};
        for(int i=0;i<4;i++){
            for(int j=0;j<4;j++){
                float s=0;
                for(int k=0;k<4;k++) s += m[i][k]*b.m[k][j];
                r.m[i][j] = s;
            }
        }
        return r;
    }
    static Mat4 translate(const Vec3& t){
        Mat4 r = identity();
        r.m[0][3]=t.x; r.m[1][3]=t.y; r.m[2][3]=t.z;
        return r;
    }
    static Mat4 scale(float s){
        Mat4 r = identity();
        r.m[0][0]=r.m[1][1]=r.m[2][2]=s;
        return r;
    }
    static Mat4 rotateY(float a){
        Mat4 r = identity();
        float c = std::cos(a), s = std::sin(a);
        r.m[0][0]= c; r.m[0][2]= s;
        r.m[2][0]=-s; r.m[2][2]= c;
        return r;
    }
    static Mat4 rotateX(float a){
        Mat4 r = identity();
        float c = std::cos(a), s = std::sin(a);
        r.m[1][1]= c; r.m[1][2]=-s;
        r.m[2][1]= s; r.m[2][2]= c;
        return r;
    }
    static Mat4 perspective(float fovRadians, float aspect, float near, float far){
        Mat4 r{};
        float f = 1.0f / std::tan(fovRadians * 0.5f);
        r.m[0][0] = f / aspect;
        r.m[1][1] = f;
        r.m[2][2] = (far+near)/(near-far);
        r.m[2][3] = (2*far*near)/(near-far);
        r.m[3][2] = -1.0f;
        r.m[3][3] = 0.0f;
        return r;
    }
    static Mat4 lookAt(const Vec3& eye, const Vec3& center, const Vec3& up){
        Vec3 f = normalize(center - eye);
        Vec3 s = normalize(cross(f, up));
        Vec3 u = cross(s, f);
        Mat4 r = identity();
        r.m[0][0]=s.x; r.m[0][1]=s.y; r.m[0][2]=s.z; r.m[0][3]=-dot(s,eye);
        r.m[1][0]=u.x; r.m[1][1]=u.y; r.m[1][2]=u.z; r.m[1][3]=-dot(u,eye);
        r.m[2][0]=-f.x; r.m[2][1]=-f.y; r.m[2][2]=-f.z; r.m[2][3]=dot(f,eye);
        return r;
    }
};

// ----------------- basic mesh (cube) -----------------
struct Vertex {
    Vec3 pos;
    Vec3 color;
};

struct Triangle {
    Vertex v0, v1, v2;
};

std::vector<Triangle> makeColoredCube(float s=1.0f){
    float h = s*0.5f;
    // 8 cube corners
    std::array<Vec3,8> p = {
        Vec3(-h,-h,-h), Vec3( h,-h,-h), Vec3( h, h,-h), Vec3(-h, h,-h),
        Vec3(-h,-h, h), Vec3( h,-h, h), Vec3( h, h, h), Vec3(-h, h, h)
    };
    // colors per vertex (for variation)
    std::array<Vec3,8> c = {
        Vec3(1,0,0), Vec3(0,1,0), Vec3(0,0,1), Vec3(1,1,0),
        Vec3(1,0,1), Vec3(0,1,1), Vec3(1,1,1), Vec3(0.2f,0.5f,0.8f)
    };
    // faces (two triangles per face)
    int faces[36] = {
        0,1,2,  0,2,3,  // back
        4,6,5,  4,7,6,  // front
        0,4,5,  0,5,1,  // bottom
        3,2,6,  3,6,7,  // top
        1,5,6,  1,6,2,  // right
        0,3,7,  0,7,4   // left
    };
    std::vector<Triangle> tris;
    tris.reserve(12);
    for(int i=0;i<36;i+=3){
        Vertex a{p[faces[i+0]], c[faces[i+0]]};
        Vertex b{p[faces[i+1]], c[faces[i+1]]};
        Vertex d{p[faces[i+2]], c[faces[i+2]]};
        tris.push_back({a,b,d});
    }
    return tris;
}

// ----------------- rasterization helpers -----------------
struct ScreenVertex {
    float x,y,z; // x,y in screen pixel coords, z is depth (0..1)
    Vec3 color;  // simple attribute to interpolate
};

// edge function
inline float edgeFunction(float x0,float y0,float x1,float y1,float x2,float y2){
    return (x2 - x0) * (y1 - y0) - (y2 - y0) * (x1 - x0);
}

// clamp utility
inline int clampi(int v,int a,int b){ if(v<a) return a; if(v>b) return b; return v; }

// ----------------- engine / renderer -----------------
struct Engine {
    int width, height;
    SDL_Window* window = nullptr;
    SDL_Renderer* renderer = nullptr;
    SDL_Texture* texture = nullptr;
    std::vector<uint32_t> framebuffer;
    std::vector<float> depthbuffer;
    std::vector<Triangle> mesh;

    Mat4 view, proj;

    Engine(int w,int h):width(w),height(h){
        framebuffer.resize(w*h);
        depthbuffer.resize(w*h);
        mesh = makeColoredCube(1.0f);
    }

    bool init(const char* title){
        if(SDL_Init(SDL_INIT_VIDEO|SDL_INIT_EVENTS) != 0){
            std::cerr << "SDL Init failed: " << SDL_GetError() << "\n";
            return false;
        }
        window = SDL_CreateWindow(title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                  width, height, SDL_WINDOW_SHOWN);
        if(!window){ std::cerr << "SDL CreateWindow failed\n"; return false; }
        renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
        if(!renderer){ std::cerr << "SDL CreateRenderer failed\n"; return false; }
        texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888,
                                    SDL_TEXTUREACCESS_STREAMING, width, height);
        if(!texture){ std::cerr << "SDL CreateTexture failed\n"; return false; }

        // camera
        Vec3 eye{2.5f, 2.0f, 2.5f};
        view = Mat4::lookAt(eye, Vec3{0,0,0}, Vec3{0,1,0});
        proj = Mat4::perspective(60.0f * (3.14159265f/180.0f), float(width)/float(height), 0.1f, 100.0f);

        return true;
    }

    void shutdown(){
        if(texture) SDL_DestroyTexture(texture);
        if(renderer) SDL_DestroyRenderer(renderer);
        if(window) SDL_DestroyWindow(window);
        SDL_Quit();
    }

    void clearBuffers(){
        std::fill(framebuffer.begin(), framebuffer.end(), 0xff202020u); // dark gray
        std::fill(depthbuffer.begin(), depthbuffer.end(), std::numeric_limits<float>::infinity());
    }

    // convert clip-space Vec4 to screen coords (with perspective divide), returns false if outside clip w range
    bool ndcToScreen(const Vec4& clip, ScreenVertex& out){
        if(std::fabs(clip.w) < 1e-6f) return false;
        float invW = 1.0f / clip.w;
        float nx = clip.x * invW;
        float ny = clip.y * invW;
        float nz = clip.z * invW; // -1 .. 1 (after projection)
        // simple frustum cull (z between -1 and 1) - could clip triangles for correctness
        if(nx < -1.1f || nx > 1.1f || ny < -1.1f || ny > 1.1f || nz < -1.1f || nz > 1.1f) {
            // allow some leeway to avoid popping triangles on edges when perspective divide slightly >1
            // but mostly we skip far outside verts
        }
        // map to screen
        out.x = (nx * 0.5f + 0.5f) * float(width);
        out.y = (1.0f - (ny * 0.5f + 0.5f)) * float(height); // flip Y
        // map depth to 0..1
        out.z = (nz * 0.5f + 0.5f);
        return true;
    }

    // draw a single triangle in screen space with barycentric interpolation of color and depth
    void rasterizeTriangle(const ScreenVertex& v0, const ScreenVertex& v1, const ScreenVertex& v2){
        // backface cull in screen space using cross of edges (signed area)
        float area = edgeFunction(v0.x, v0.y, v1.x, v1.y, v2.x, v2.y);
        if(area == 0.0f) return;
        if(area < 0.0f) return; // cull clockwise - consistent winding

        // bounding box
        int minX = clampi(int(std::floor(std::min({v0.x,v1.x,v2.x}))), 0, width-1);
        int maxX = clampi(int(std::ceil (std::max({v0.x,v1.x,v2.x}))), 0, width-1);
        int minY = clampi(int(std::floor(std::min({v0.y,v1.y,v2.y}))), 0, height-1);
        int maxY = clampi(int(std::ceil (std::max({v0.y,v1.y,v2.y}))), 0, height-1);

        // precompute reciprocal area
        float invArea = 1.0f / area;

        for(int y=minY;y<=maxY;y++){
            for(int x=minX;x<=maxX;x++){
                // sample at pixel center
                float px = x + 0.5f;
                float py = y + 0.5f;
                float w0 = edgeFunction(v1.x, v1.y, v2.x, v2.y, px, py);
                float w1 = edgeFunction(v2.x, v2.y, v0.x, v0.y, px, py);
                float w2 = edgeFunction(v0.x, v0.y, v1.x, v1.y, px, py);
                if(w0 >= 0 && w1 >= 0 && w2 >= 0){
                    w0 *= invArea; w1 *= invArea; w2 *= invArea;
                    // perspective-correct depth interpolation would require 1/w; we use linear for simplicity
                    float depth = v0.z * w0 + v1.z * w1 + v2.z * w2;
                    int idx = y*width + x;
                    if(depth < depthbuffer[idx]) {
                        depthbuffer[idx] = depth;
                        // simple color interpolation
                        Vec3 col = v0.color * w0 + v1.color * w1 + v2.color * w2;
                        // convert to 8-bit ARGB (opaque)
                        uint8_t r = uint8_t(clampf(col.x,0.0f,1.0f) * 255.0f);
                        uint8_t g = uint8_t(clampf(col.y,0.0f,1.0f) * 255.0f);
                        uint8_t b = uint8_t(clampf(col.z,0.0f,1.0f) * 255.0f);
                        framebuffer[idx] = (0xFFu<<24) | (uint32_t(r)<<16) | (uint32_t(g)<<8) | uint32_t(b);
                    }
                }
            }
        }
    }

    // helper clamp for floats
    static float clampf(float v,float a,float b){ if(v<a) return a; if(v>b) return b; return v; }

    // transform a world-space vertex through model-view-proj to clip-space
    Vec4 transformClip(const Vec3& p, const Mat4& mvp){
        Vec4 v(p.x,p.y,p.z,1.0f);
        return mvp.mul(v);
    }

    void drawMesh(const Mat4& model){
        Mat4 mvp = proj * (view * model); // column-major style as implemented
        // For each triangle:
        for(const Triangle& tri : mesh){
            Vec4 c0 = transformClip(tri.v0.pos, mvp);
            Vec4 c1 = transformClip(tri.v1.pos, mvp);
            Vec4 c2 = transformClip(tri.v2.pos, mvp);

            ScreenVertex sv0, sv1, sv2;
            ndcToScreen(c0, sv0); sv0.color = tri.v0.color;
            ndcToScreen(c1, sv1); sv1.color = tri.v1.color;
            ndcToScreen(c2, sv2); sv2.color = tri.v2.color;

            // trivial reject if all vertices outside a huge clip (not rigorous clipping)
            // Backface cull in world space too (optional)
            // Rasterize
            rasterizeTriangle(sv0, sv1, sv2);
        }
    }

    void present(){
        // upload pixels to SDL texture and render
        void* pixels = nullptr;
        int pitch = 0;
        if(SDL_LockTexture(texture, nullptr, &pixels, &pitch) != 0){
            std::cerr << "SDL_LockTexture failed: " << SDL_GetError() << "\n";
            return;
        }
        // pitch may be width*4 but handle generic
        uint8_t* dst = static_cast<uint8_t*>(pixels);
        for(int y=0;y<height;y++){
            uint32_t* row = (uint32_t*)(dst + y * pitch);
            for(int x=0;x<width;x++){
                row[x] = framebuffer[y*width + x];
            }
        }
        SDL_UnlockTexture(texture);

        SDL_RenderClear(renderer);
        SDL_RenderCopy(renderer, texture, nullptr, nullptr);
        SDL_RenderPresent(renderer);
    }

    void runLoop(){
        bool running = true;
        SDL_Event ev{};
        auto t0 = std::chrono::high_resolution_clock::now();
        float angle = 0.0f;
        while(running){
            while(SDL_PollEvent(&ev)){
                if(ev.type == SDL_QUIT) running = false;
                if(ev.type == SDL_KEYDOWN){
                    if(ev.key.keysym.sym == SDLK_ESCAPE) running = false;
                }
            }

            // update
            auto now = std::chrono::high_resolution_clock::now();
            float elapsed = std::chrono::duration<float>(now - t0).count();
            angle = elapsed * 0.8f; // radians

            Mat4 model = Mat4::rotateY(angle) * Mat4::rotateX(angle*0.5f);
            clearBuffers();
            drawMesh(model);
            present();

            // simple frame limiter ~60 fps
            SDL_Delay(1);
        }
    }
};

// ----------------- small helpers -----------------
inline float clampf(float v,float a,float b){ if(v<a) return a; if(v>b) return b; return v; }
inline uint8_t clampf_to_u8(float v){ if(v<0) return 0; if(v>1) return 255; return uint8_t(v*255.0f); }

// ----------------- main -----------------
int main(int argc,char** argv){
    (void)argc; (void)argv;
    const int WIDTH = 800;
    const int HEIGHT = 600;
    Engine engine(WIDTH, HEIGHT);
    if(!engine.init("Software Engine - engine.cpp")){
        return -1;
    }
    engine.runLoop();
    engine.shutdown();
    return 0;
}
