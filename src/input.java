import java.awt.event.*;

public class Input implements KeyListener, MouseMotionListener {

    public boolean w,a,s,d,space,ctrl;
    public float mouseDX, mouseDY;

    private int lastX, lastY;
    private boolean init = false;

    public void beginFrame() {
        mouseDX = mouseDY = 0;
    }

    @Override
    public void keyPressed(KeyEvent e) {
        switch (e.getKeyCode()) {
            case KeyEvent.VK_W -> w = true;
            case KeyEvent.VK_A -> a = true;
            case KeyEvent.VK_S -> s = true;
            case KeyEvent.VK_D -> d = true;
            case KeyEvent.VK_SPACE -> space = true;
            case KeyEvent.VK_CONTROL -> ctrl = true;
        }
    }

    @Override
    public void keyReleased(KeyEvent e) {
        switch (e.getKeyCode()) {
            case KeyEvent.VK_W -> w = false;
            case KeyEvent.VK_A -> a = false;
            case KeyEvent.VK_S -> s = false;
            case KeyEvent.VK_D -> d = false;
            case KeyEvent.VK_SPACE -> space = false;
            case KeyEvent.VK_CONTROL -> ctrl = false;
        }
    }

    @Override public void keyTyped(KeyEvent e) {}

    @Override
    public void mouseMoved(MouseEvent e) {
        if (!init) {
            lastX = e.getX();
            lastY = e.getY();
            init = true;
            return;
        }
        mouseDX += e.getX() - lastX;
        mouseDY += e.getY() - lastY;
        lastX = e.getX();
        lastY = e.getY();
    }

    @Override public void mouseDragged(MouseEvent e) {}
}
