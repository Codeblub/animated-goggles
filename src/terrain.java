import java.util.ArrayList;

public class Terrain {

    ArrayList<Tri> tris = new ArrayList<>();

    public Terrain() {
        for (int z = 0; z < 80; z++) {
            for (int x = 0; x < 80; x++) {
                float h = height(x, z);
                tris.add(new Tri(
                        new Vec3(x, h, z),
                        new Vec3(x+1, height(x+1,z), z),
                        new Vec3(x, height(x,z+1), z+1),
                        0xFF2E8B57
                ));
            }
        }
    }

    float height(int x, int z) {
        return (float)(0.6 * Math.sin(x * 0.2) * Math.cos(z * 0.2));
    }
}
