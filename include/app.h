#pragma once

#include <iostream>
#include <SDL3/SDL.h>
#include <cstdlib>
#include <optional>
#include <chrono>

struct Circle {
	float x;
	float y;
	float radius;
};

class App {
private:
	size_t width{ 900 };
	size_t height{ 900 };

	SDL_Window* window = nullptr;
	SDL_Renderer* renderer = nullptr;
	SDL_Event event;

	SDL_Texture* rectTex = nullptr;
	SDL_Texture* indexTex = nullptr;

	double rectAngle{ 0};
    double indexAngle{ 0};
	double baseIndexSpeed{ 40};
	double indexSpeed{ baseIndexSpeed };
	double indexSpeedCap{ 200};
	double speedStep{ 20};

	size_t peakScore{ 0 };
	size_t score{ 0 };

	void init();
	void menu();
	void loop();
	void exit();
	void drawCircle(SDL_Renderer* renderer, Circle circle, bool perimOnly, std::optional<float> perimThickness = std::nullopt);
	SDL_Texture* createSolidTexture(SDL_Renderer* renderer, SDL_Color color, int w = 1, int h = 1);
	void handleInput();
	void lossTransition();

	
public:
	void run();
};
