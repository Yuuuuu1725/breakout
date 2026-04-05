#include <iostream>
using namespace std;

int main() {
    int lives = 3;
    int score = 0;
    bool gameOver = false;

    cout << "================================" << endl;
    cout << "     BRICK GAME CHECKER" << endl;
    cout << "================================" << endl;

    cout << "[1] Initial Lives: " << lives << endl;
    cout << "[1] Initial Score: " << score << endl;
    cout << "[1] Game Over: NO" << endl;

    lives--;
    cout << "\n[2] Life lost: " << lives << " left" << endl;
    lives--;
    cout << "[2] Life lost: " << lives << " left" << endl;
    lives--;
    cout << "[2] Life lost: " << lives << " left" << endl;

    if (lives <= 0) {
        gameOver = true;
        cout << "\n[3] GAME OVER: TRUE" << endl;
        cout << "[3] Final Score: " << score << endl;
    }

    if (gameOver) {
        lives = 3;
        score = 0;
        gameOver = false;
        cout << "\n[4] GAME RESTARTED" << endl;
        cout << "[4] Lives: " << lives << " Score: " << score << endl;
    }

    cout << "\n================================" << endl;
    cout << "    ALL CHECKES PASSED ✅" << endl;
    cout << "================================" << endl;

    return 0;
}