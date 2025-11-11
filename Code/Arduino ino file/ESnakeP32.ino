//   ____                _           _   _           
//  / ___|_ __ ___  __ _| |_ ___  __| | | |__  _   _ 
// | |   | '__/ _ \/ _` | __/ _ \/ _` | | '_ \| | | |
// | |___| | |  __/ (_| | ||  __/ (_| | | |_) | |_| |
//  \____|_|  \___|\__,_|\__\___|\__,_| |_.__/ \__, |
//  ____  _ _         _____                    |___/ 
// / ___|| (_)____   / ___ \                         
// \___ \| | |_  /  / | _ \ \                        
//  ___) | | |/ /  |  |   /  |                       
// |____/|_|_/___|  \ |_|_\ /                        
//                   \_____/                         

#include <M5StickCPlus2.h>
#include <vector>

#define GRID_WIDTH 16
#define GRID_HEIGHT 8
#define CELL_SIZE 10

enum Direction { UP, DOWN, LEFT, RIGHT };

struct Point {
    int x, y;
};

std::vector<Point> snake;
Point apple;
Direction dir = RIGHT;
bool gameOver = false;
int score = 0;

unsigned long lastPressA = 0, lastPressB = 0;
const unsigned long doublePressDelay = 300;

void initGame() {
    snake.clear();
    snake.push_back({GRID_WIDTH / 2, GRID_HEIGHT / 2});
    dir = RIGHT;
    gameOver = false;
    score = 0;
    spawnApple();
}

void spawnApple() {
    bool valid = false;
    while (!valid) {
        apple.x = random(0, GRID_WIDTH);
        apple.y = random(0, GRID_HEIGHT);
        valid = true;
        for (const auto& segment : snake) {
            if (apple.x == segment.x && apple.y == segment.y) {
                valid = false;
                break;
            }
        }
    }
}

void playIntroAnimation() {
    M5.Lcd.fillScreen(TFT_BLACK);
    
    std::vector<Point> introSnake = {{0, GRID_HEIGHT / 2}};
    Point introApple = {GRID_WIDTH / 2, GRID_HEIGHT / 2};
    Direction introDir = RIGHT;
    
    while (introSnake[0].x < GRID_WIDTH) {
        M5.Lcd.fillRect(introApple.x * CELL_SIZE + 3, introApple.y * CELL_SIZE + 3, 4, 4, TFT_RED);
        
        M5.Lcd.fillScreen(TFT_BLACK);
        for (size_t i = 0; i < introSnake.size(); i++) {
            M5.Lcd.fillRect(introSnake[i].x * CELL_SIZE, introSnake[i].y * CELL_SIZE, CELL_SIZE - 1, CELL_SIZE - 1, TFT_GREEN);
        }
        M5.Lcd.fillRect(introApple.x * CELL_SIZE + 3, introApple.y * CELL_SIZE + 3, 4, 4, TFT_RED);
        
        introSnake.insert(introSnake.begin(), {introSnake[0].x + 1, introSnake[0].y});
        if (introSnake[0].x != introApple.x || introSnake[0].y != introApple.y) {
            introSnake.pop_back();
        } else {
            introApple = {GRID_WIDTH + 1, GRID_HEIGHT + 1};
        }
        delay(100);
    }
    
    M5.Lcd.fillScreen(TFT_BLACK);
    M5.Lcd.setTextColor(TFT_WHITE);
    M5.Lcd.setTextSize(1);
    M5.Lcd.setCursor(GRID_WIDTH / 2 * CELL_SIZE - 20, GRID_HEIGHT / 2 * CELL_SIZE);
    M5.Lcd.print("ESnake32");
    
    introSnake = {{0, GRID_HEIGHT / 2}};
    while (introSnake[0].x < GRID_WIDTH) {
        M5.Lcd.fillScreen(TFT_BLACK);
        for (size_t i = 0; i < introSnake.size(); i++) {
            M5.Lcd.fillRect(introSnake[i].x * CELL_SIZE, introSnake[i].y * CELL_SIZE, CELL_SIZE - 1, CELL_SIZE - 1, TFT_GREEN);
        }
        if (introSnake[0].x < GRID_WIDTH / 2) {
            M5.Lcd.setCursor(GRID_WIDTH / 2 * CELL_SIZE - 20, GRID_HEIGHT / 2 * CELL_SIZE);
            M5.Lcd.print("ESnake32");
        }
        introSnake.insert(introSnake.begin(), {introSnake[0].x + 1, introSnake[0].y});
        if (introSnake[0].x != GRID_WIDTH / 2) {
            introSnake.pop_back();
        }
        delay(3000);
    }
    
    M5.Lcd.fillScreen(TFT_BLACK);
    M5.Lcd.setCursor(GRID_WIDTH / 2 * CELL_SIZE - 30, GRID_HEIGHT / 2 * CELL_SIZE);
    M5.Lcd.print("Made By Sliz");
    delay(5000);
}

void updateGame() {
    if (gameOver) return;

    M5.update();
    
    if (M5.BtnA.wasPressed()) {
        unsigned long currentTime = millis();
        if (currentTime - lastPressA < doublePressDelay) {
            if (dir != DOWN) dir = UP;
        } else {
            if (dir != RIGHT) dir = LEFT;
        }
        lastPressA = currentTime;
    }
    
    if (M5.BtnB.wasPressed()) {
        unsigned long currentTime = millis();
        if (currentTime - lastPressB < doublePressDelay) {
            if (dir != UP) dir = DOWN;
        } else {
            if (dir != LEFT) dir = RIGHT;
        }
        lastPressB = currentTime;
    }

    Point newHead = snake[0];
    switch (dir) {
        case UP: newHead.y--; break;
        case DOWN: newHead.y++; break;
        case LEFT: newHead.x--; break;
        case RIGHT: newHead.x++; break;
    }

    if (newHead.x < 0 || newHead.x >= GRID_WIDTH || newHead.y < 0 || newHead.y >= GRID_HEIGHT) {
        gameOver = true;
        return;
    }
    for (const auto& segment : snake) {
        if (newHead.x == segment.x && newHead.y == segment.y) {
            gameOver = true;
            return;
        }
    }

    snake.insert(snake.begin(), newHead);
    if (newHead.x == apple.x && newHead.y == apple.y) {
        score++;
        spawnApple();
    } else {
        snake.pop_back();
    }
}

void drawGame() {
    M5.Lcd.fillScreen(TFT_BLACK);
    
    for (const auto& segment : snake) {
        M5.Lcd.fillRect(segment.x * CELL_SIZE, segment.y * CELL_SIZE, CELL_SIZE - 1, CELL_SIZE - 1, TFT_GREEN);
    }
    
    M5.Lcd.fillRect(apple.x * CELL_SIZE + 3, apple.y * CELL_SIZE + 3, 4, 4, TFT_RED);
    
    M5.Lcd.setTextColor(TFT_WHITE);
    M5.Lcd.setTextSize(1);
    M5.Lcd.setCursor(0, 0);
    M5.Lcd.print("Score: ");
    M5.Lcd.print(score);
    
    if (gameOver) {
        M5.Lcd.setCursor(40, 30);
        M5.Lcd.print("Game Over!");
        M5.Lcd.setCursor(40, 40);
        M5.Lcd.print("Score: ");
        M5.Lcd.print(score);
    }
}

void setup() {
    M5.begin();
    M5.Lcd.setRotation(1);
    randomSeed(analogRead(0));
    
    esp_sleep_enable_ext0_wakeup(GPIO_NUM_39, 1);
    
    playIntroAnimation();
    
    initGame();
}

void loop() {
    M5.update();
    
    if (M5.BtnPWR.wasPressed()) {
        esp_deep_sleep_start();
    }
    
    updateGame();
    drawGame();
    
    delay(200);
}
