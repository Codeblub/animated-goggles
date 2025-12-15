import javax.swing.*;
import java.awt.*;

public class Engine extends JPanel implements Runnable {

    public static final int W = 1280;
    public static final int H = 720;

    JFrame frame;
    Renderer renderer;
    Input input;
    Camera camera;
    Terrain terrain;

    public Engine() {
        renderer = new Renderer(W, H);
        input = new Input();
        camera = new Camera();
        terrain = new Terrain();

        frame = new JFrame("My Java Engine");
        frame.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
        frame.setSize(W, H);
        frame.add(this);
        frame.setVisible(true);

        frame.addKeyListener(input);
        frame.addMouseMotionListener(input);

        new Thread(this).start();
    }

    @Override
    public void run() {
        long last = System.nanoTime();

        while (true) {
            long now = System.nanoTime();
            float dt = (now - last) / 1e9f;
            last = now;

            input.beginFrame();
            camera.update(input, dt);

            renderer.clear(0xFF204857); // sky color

            renderer.renderTerrain(terrain, camera);
            repaint();
        }
    }

    @Override
    public void paintComponent(Graphics g) {
        g.drawImage(renderer.image, 0, 0, null);
    }
}
