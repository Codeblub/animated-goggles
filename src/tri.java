public class Tri {

    public Vec3[] p = new Vec3[3];
    public int color;

    public Tri(Vec3 a, Vec3 b, Vec3 c, int col) {
        p[0]=a; p[1]=b; p[2]=c;
        color = col;
    }

    public Tri transform(Camera cam) {
        Vec3[] np = new Vec3[3];
        for (int i=0;i<3;i++) {
            Vec3 v = p[i].sub(cam.pos);
            float x=v.x, z=v.z;
            np[i]=new Vec3(
                    (float)(x*Math.cos(cam.yaw)-z*Math.sin(cam.yaw)),
                    v.y,
                    (float)(x*Math.sin(cam.yaw)+z*Math.cos(cam.yaw))
            );
        }
        return new Tri(np[0],np[1],np[2],color);
    }
}
