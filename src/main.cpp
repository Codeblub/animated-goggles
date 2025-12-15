#include "engine.h"
#include <vector>

std::vector<Triangle> makeGrassPlane(float size=100.0f){
    float h = size * 0.5f;
    Vec3 green(0.2f, 0.8f, 0.2f);
    Vertex v0{Vec3(-h, 0, -h), green};
    Vertex v1{Vec3( h, 0, -h), green};
    Vertex v2{Vec3( h, 0,  h), green};
    Vertex v3{Vec3(-h, 0,  h), green};
    return { {v0,v1,v2}, {v0,v2,v3} };
}

int main(int argc,char** argv){
    (void)argc; (void)argv;
    const int WIDTH = 800;
    const int HEIGHT = 600;
    auto mesh = makeGrassPlane(100.0f);
    Engine engine(WIDTH, HEIGHT, mesh);
    if(!engine.init("Unkown")){
        return -1;
    }
    engine.runLoop();
    engine.shutdown();
    return 0;
}
