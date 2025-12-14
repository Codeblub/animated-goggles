#pragma once
#include <vector>
#include <cmath>

struct Terrain {
    std::vector<Triangle> tris;

    void generate(int size, float scale) {
        tris.clear();

        auto height = [&](int x,int z){
            return 0.4f * sinf(x * 0.2f) * cosf(z * 0.2f);
        };

        for(int z=0;z<size-1;z++){
            for(int x=0;x<size-1;x++){
                Vec3 p0{x*scale, height(x,z), z*scale};
                Vec3 p1{(x+1)*scale, height(x+1,z), z*scale};
                Vec3 p2{x*scale, height(x,z+1), (z+1)*scale};
                Vec3 p3{(x+1)*scale, height(x+1,z+1), (z+1)*scale};

                Vec3 grass{0.2f,0.6f,0.2f};

                tris.push_back({{p0,grass},{p1,grass},{p2,grass}});
                tris.push_back({{p1,grass},{p3,grass},{p2,grass}});
            }
        }
    }
};
