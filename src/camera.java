public class Camera {

    public Vec3 pos = new Vec3(0, 1.7f, -5);
    public float yaw = 0;
    public float pitch = 0;

    float speed = 6f;
    float sens = 0.002f;

    public Vec3 forward() {
        return Vec3.normalize(new Vec3(
                (float)(Math.cos(pitch) * Math.sin(yaw)),
                (float)Math.sin(pitch),
                (float)(Math.cos(pitch) * Math.cos(yaw))
        ));
    }

    public Vec3 right() {
        return Vec3.normalize(Vec3.cross(forward(), new Vec3(0,1,0)));
    }

    public void update(Input in, float dt) {
        yaw   += in.mouseDX * sens;
        pitch -= in.mouseDY * sens;
        pitch = Math.max(-1.5f, Math.min(1.5f, pitch));

        Vec3 dir = new Vec3(0,0,0);
        if (in.w) dir = dir.add(forward());
        if (in.s) dir = dir.sub(forward());
        if (in.a) dir = dir.sub(right());
        if (in.d) dir = dir.add(right());
        if (in.space) dir.y += 1;
        if (in.ctrl)  dir.y -= 1;

        if (dir.length() > 0)
            pos = pos.add(Vec3.normalize(dir).mul(speed * dt));
    }
}
