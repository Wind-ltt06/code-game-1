class Bullet {
public:
    int x, y;
    int dx, dy;
    int speed; // Thêm biến tốc độ
    SDL_Rect rect;
    bool active;

    Bullet(int startX, int startY, int dirX, int dirY, int bulletSpeed = 10) {
        x = startX;
        y = startY;
        dx = dirX;
        dy = dirY;
        speed = bulletSpeed; // Lưu tốc độ vào biến
        active = true;
        rect = {x, y, 10, 10}; // Kích thước viên đạn
    }

    void move() {
        x += dx * speed; // Nhân thêm hệ số tốc độ
        y += dy * speed;
        rect.x = x;
        rect.y = y;

        // Kiểm tra nếu đạn ra khỏi màn hình thì tắt
        if (x < TILE_SIZE || x > SCREEN_WIDTH - TILE_SIZE ||
            y < TILE_SIZE || y > SCREEN_HEIGHT - TILE_SIZE) {
            active = false;
        }
    }

    void render(SDL_Renderer* renderer) {
        if (active) {
            SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
            SDL_RenderFillRect(renderer, &rect);
        }
    }
};
