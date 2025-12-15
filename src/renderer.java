import java.awt.image.BufferedImage;

public class Renderer {

    BufferedImage image;
    int[] pixels;
    float[] zbuffer;
    int W, H;

    public Renderer(int w, int h) {
        W = w; H = h;
        image = new BufferedImage(W, H, BufferedImage.TYPE_INT_ARGB);
        pixels = ((java.awt.image.DataBufferInt)
                image.getRaster().getDataBuffer()).getData();
        zbuffer = new float[W * H];
    }

    public void clear(int color) {
        for (int i = 0; i < pixels.length; i++) {
            pixels[i] = color;
            zbuffer[i] = Float.POSITIVE_INFINITY;
        }
    }

    public void renderTerrain(Terrain terrain, Camera cam) {
        for (Tri t : terrain.tris) {
            Tri tr = t.transform(cam);
            drawTri(tr);
        }
    }

    void drawTri(Tri t) {
        // (same triangle rasterizer logic you already saw)
        // kept short here intentionally
    }
}
