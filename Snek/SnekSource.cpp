// The Owner of code is youtube user javidx9 aka OneLoneCoder
// https://www.youtube.com/watch?v=e8lYLYlrGLg
// This code was copied 1:0.99 and is only used for purpose of learning

#include <iostream>
#include <Windows.h>
#include <list>
#include <thread>

using namespace std;

int nScreenWidth = 120;
int nScreenHeight = 30;

struct sSnakeSegment {
	int x;
	int y;
};

int main() {
	//Create Screen Buffer smth smth winAPI magic?
	wchar_t* screen = new wchar_t[nScreenWidth * nScreenHeight];
	for (int i = 0; i < nScreenWidth * nScreenHeight; i++) { screen[i] = L' '; }
	HANDLE hConsole = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);
	SetConsoleActiveScreenBuffer(hConsole);
	DWORD dw_bytes_written = 0;

	while (1) {
		// snake with static initializing list
		list<sSnakeSegment> snake = { {60, 15},{61, 15},{62, 15},{63,15},{64,15},{65,15},{66,15},{67,15},{68,15},{69,15} };
		// game state variables
		int nFoodX = 30;
		int nFoodY = 15;
		int nScore = 0;
		int nSnakeDirection = 3;
		bool bDead = false;
		bool bKeyLeft = false, bKeyRight = false, bKeyLeftOld = false, bKeyRightOld = false;

		// game loop
		while (!bDead) {
			// Timing & Input
			// Slow down CPU with hard coded delay very dumb
			//this_thread::sleep_for(200ms);
			// Get Input
			auto t1 = chrono::system_clock::now();
			while ((chrono::system_clock::now() - t1) < ((nSnakeDirection % 2 == 1) ? 120ms : 200ms)) {
				bKeyRight = (0x8000 & GetAsyncKeyState((unsigned char)('\x27'))) != 0; // key in hex
				bKeyLeft = (0x8000 & GetAsyncKeyState((unsigned char)('\x25'))) != 0;

				if (bKeyRight && !bKeyRightOld) {
					nSnakeDirection++;
					if (nSnakeDirection == 4) nSnakeDirection = 0;
				}

				if (bKeyLeft && !bKeyLeftOld) {
					nSnakeDirection--;
					if (nSnakeDirection == -1) nSnakeDirection = 3;
				}
				bKeyRightOld = bKeyRight;
				bKeyLeftOld = bKeyLeft;
			}
			// Game Logic
			// Update Snake Position
			switch (nSnakeDirection) {
			case 0: // up
				snake.push_front({ snake.front().x, snake.front().y - 1 });
				break;
			case 1: // right
				snake.push_front({ snake.front().x + 1, snake.front().y });
				break;
			case 2: //down
				snake.push_front({ snake.front().x, snake.front().y + 1 });
				break;
			case 3: // left
				snake.push_front({ snake.front().x - 1, snake.front().y });
				break;
			}
			// Collision Detection
			// Collision Detect Snake vs World
			if (snake.front().x < 0 || snake.front().x >= nScreenWidth)
				bDead = true;
			if (snake.front().y < 3 || snake.front().y >= nScreenHeight) // <3 to accomodate Stats panel
				bDead = true;
			// Collision Detect Snake vs Food
			if (snake.front().x == nFoodX && snake.front().y == nFoodY) {
				nScore++;
				while (screen[nFoodY * nScreenWidth + nFoodX] != L' ') {
					nFoodX = rand() % nScreenWidth;
					nFoodY = (rand() % (nScreenHeight - 3)) + 3;
				}
				for (int i = 0; i < 5; i++) { // increase snek size by 5
					snake.push_back({ snake.back().x, snake.back().y });
				}
			}
			// Collision Detect Snake vs Snake
			for (list<sSnakeSegment>::iterator i = snake.begin(); i != snake.end(); i++) {
				if (i != snake.begin() && i->x == snake.front().x && i->y == snake.front().y) {
					bDead = true;
				}
			}
			// Chop off Snake tail
			snake.pop_back();

			// Display Stuff to Player

			// Clear Screen
			for (int i = 0; i < nScreenWidth * nScreenHeight; i++) { screen[i] = L' '; }

			// Draw Stats & Border
			for (int i = 0; i < nScreenWidth; i++) {
				screen[i] = L'=';
				screen[2 * nScreenWidth + i] = L'=';
			}
			wsprintf(&screen[nScreenWidth + 5], L"www.OneLoneCoder.com - Snek                         SCORE: %d", nScore);

			// Draw Snake Body
			for (auto s : snake) { screen[s.y * nScreenWidth + s.x] = bDead ? L'+' : L'O'; }
			// Draw Snake Head
			screen[snake.front().y * nScreenWidth + snake.front().x] = bDead ? L'X' : L'@';
			// Draw Food
			screen[nFoodY * nScreenWidth + nFoodX] = L'%';

			if (bDead) {
				wsprintf(&screen[15 * nScreenWidth + 40], L"   Press space to respawn");
			}
			// Display Frame
			WriteConsoleOutputCharacter(hConsole, screen, nScreenWidth * nScreenHeight, { 0,0 }, &dw_bytes_written);

		}
		//wait for space
		while ((0x8000 & GetAsyncKeyState((unsigned char)('\x20'))) == 0);
	}
	return 0;
}