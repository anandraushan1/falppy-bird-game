#include <iostream>
#include <unistd.h>
#include <termios.h>
#include <fcntl.h>
#include <cstdlib>
#include <ctime>

using namespace std;

int width = 40;
int height = 20;

int birdY, velocity;
int pipeX, gapY;
int score;
bool gameOver;

// Non-blocking keyboard input
int kbhit() {
    termios oldt, newt;
    int ch;
    int oldf;

    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
    fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);

    ch = getchar();

    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    fcntl(STDIN_FILENO, F_SETFL, oldf);

    if (ch != EOF) {
        ungetc(ch, stdin);
        return 1;
    }

    return 0;
}

char getch() {
    char buf = 0;
    termios old = {0};
    tcgetattr(STDIN_FILENO, &old);
    old.c_lflag &= ~ICANON;
    old.c_lflag &= ~ECHO;
    tcsetattr(STDIN_FILENO, TCSANOW, &old);
    read(STDIN_FILENO, &buf, 1);
    tcsetattr(STDIN_FILENO, TCSANOW, &old);
    return buf;
}

// Draw game
void draw() {
    cout << "\033[2J\033[1;1H"; // clear screen

    for (int i = 0; i < width + 2; i++) cout << "#";
    cout << endl;

    for (int i = 0; i < height; i++) {
        cout << "#";
        for (int j = 0; j < width; j++) {
            if (j == 5 && i == birdY)
                cout << "O";
            else if (j == pipeX && (i < gapY || i > gapY + 4))
                cout << "|";
            else
                cout << " ";
        }
        cout << "#" << endl;
    }

    for (int i = 0; i < width + 2; i++) cout << "#";
    cout << endl;

    cout << "Score: " << score << endl;
}

// Setup
void setup() {
    srand(time(0));
    gameOver = false;
    birdY = height / 2;
    velocity = 0;
    pipeX = width - 1;
    gapY = rand() % (height - 6);
    score = 0;
}

// Input
void input() {
    if (kbhit()) {
        char ch = getch();
        if (ch == ' ')
            velocity = -3;
    }
}

// Logic
void logic() {
    velocity += 1;
    birdY += velocity;

    pipeX--;

    if (pipeX < 0) {
        pipeX = width - 1;
        gapY = rand() % (height - 6);
        score++;
    }

    if (birdY < 0 || birdY >= height)
        gameOver = true;

    if (pipeX == 5 && (birdY < gapY || birdY > gapY + 4))
        gameOver = true;
}

// Main
int main() {
    setup();

    while (!gameOver) {
        draw();
        input();
        logic();
        usleep(100000); // 100 ms
    }

    cout << "\nGame Over! Final Score: " << score << endl;
    return 0;
}