public class Vec3 {
    public float x,y,z;

    public Vec3(float x,float y,float z){this.x=x;this.y=y;this.z=z;}

    public Vec3 add(Vec3 o){return new Vec3(x+o.x,y+o.y,z+o.z);}
    public Vec3 sub(Vec3 o){return new Vec3(x-o.x,y-o.y,z-o.z);}
    public Vec3 mul(float s){return new Vec3(x*s,y*s,z*s);}

    public float length(){return (float)Math.sqrt(x*x+y*y+z*z);}

    public static Vec3 normalize(Vec3 v){
        float l=v.length();
        return l==0?v:new Vec3(v.x/l,v.y/l,v.z/l);
    }

    public static Vec3 cross(Vec3 a, Vec3 b){
        return new Vec3(
                a.y*b.z-a.z*b.y,
                a.z*b.x-a.x*b.z,
                a.x*b.y-a.y*b.x
        );
    }
}
