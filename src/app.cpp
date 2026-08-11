#include "app.h"

void App::init()
{
	SDL_Init(SDL_INIT_VIDEO);

	SDL_CreateWindowAndRenderer("OTAW", width, height, 0, &window, &renderer);
}

void App::menu()
{
    while (true)
    {
        SDL_PollEvent(&event);
        if (event.type == SDL_EVENT_QUIT) break;

        const bool* key_states = SDL_GetKeyboardState(NULL);
        if (key_states[SDL_SCANCODE_S]) loop();
         
        // Clear renderer
        SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0x00);
        SDL_RenderClear(renderer);

        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_SetRenderScale(renderer, 3.0f, 3.0f);
        SDL_RenderDebugText(renderer, 10, 10, "Press 'S' to Start");
        SDL_RenderDebugText(renderer, 10, 30,  std::string("Peak Score: " + std::to_string(peakScore)).c_str());
        SDL_RenderDebugText(renderer, 10, 50, "Controls: W/Space/Up to Click");
        SDL_SetRenderScale(renderer, 1.0f, 1.0f);

        // render call
        SDL_RenderPresent(renderer);
    }

    exit();
}

void App::loop()
{
    Circle circleOuter{ width / 2.0f, height / 2.0f, 200.0f };
    Circle circleInner{ width / 2.0f, height / 2.0f, 170.0f };
    Circle circleCenter{ width / 2.0f, height / 2.0f, 10.0f };

    rectTex = createSolidTexture(renderer, { 255, 0, 0, 255 });
    SDL_FRect rect{ (width / 2.0f) - 10.0f, (height / 2.0f) + circleInner.radius, 20.0f, 25.0f };
    SDL_FPoint rectPivot{ 10.0f, -circleInner.radius };
    rectAngle = static_cast<double>(std::rand() % 360);

    indexTex = createSolidTexture(renderer, { 0, 255, 0, 255 });
    SDL_FRect index{ (width / 2.0f) - 2.5f, height / 2.0f, 5.0f, circleOuter.radius };
    SDL_FPoint indexPivot{ 2.5f, 0 };

    size_t lastSpeedUpScore{ 0 };

    auto lastTime = std::chrono::high_resolution_clock::now();

	while (true)
	{ 
        auto currentTime = std::chrono::high_resolution_clock::now();
        double deltaTime = std::chrono::duration<double>(currentTime - lastTime).count();
        lastTime = currentTime; 

        indexAngle = std::fmod(indexAngle + deltaTime * indexSpeed, 360);
        //std::cout << time << std::endl;

        SDL_PollEvent(&event);
		if (event.type == SDL_EVENT_QUIT) break;

        handleInput();

        if (score != 0 && score != lastSpeedUpScore && score % 5 == 0 && indexSpeed <= indexSpeedCap)
        {
            indexSpeed >= 0 ? indexSpeed += speedStep : indexSpeed -= speedStep;
            lastSpeedUpScore = score;
        }

        // Clear renderer
        SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0x00);
        SDL_RenderClear(renderer);
        
        SDL_RenderTextureRotated(renderer, rectTex, nullptr, &rect, rectAngle, &rectPivot, SDL_FLIP_NONE);

        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);           
        drawCircle(renderer, circleOuter, true, 6.0f);
        drawCircle(renderer, circleInner, true, 4.0f);        

        SDL_RenderTextureRotated(renderer, indexTex, nullptr, &index, indexAngle, &indexPivot, SDL_FLIP_NONE);

        drawCircle(renderer, circleCenter, false);

        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_SetRenderScale(renderer, 3.0f, 3.0f); 
        SDL_RenderDebugText(renderer, 10, 10, std::string("Score: " + std::to_string(score)).c_str());
        SDL_RenderDebugText(renderer, 10, 20, std::string("Speed: " + std::to_string(std::abs(static_cast<int>(indexSpeed)))).c_str());
        SDL_SetRenderScale(renderer, 1.0f, 1.0f);

        // render call
        SDL_RenderPresent(renderer);
	}
    
    exit();
}

void App::exit()
{
    SDL_DestroyTexture(rectTex);
    SDL_DestroyTexture(indexTex);

    SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
}

void App::run()
{
	init();
    menu();
    exit();
}

void App::drawCircle(SDL_Renderer* renderer, Circle circle, bool perimOnly, std::optional<float> perimThickness)
{
    float low_x = circle.x - circle.radius;
    float low_y = circle.y - circle.radius;
    float high_x = circle.x + circle.radius;
    float high_y = circle.y + circle.radius;

    float radiusSquared = circle.radius * circle.radius;

    for (float x{ low_x }; x < high_x; ++x)
    {
        for (float y{ low_y }; y < high_y; ++y)
        {
            // check if coordinate is within circle
            float centerDistSquared = (x - circle.x) * (x - circle.x) + (y - circle.y) * (y - circle.y);

            if (centerDistSquared <= radiusSquared)
            {
                if (perimOnly && perimThickness.has_value() && (std::abs(std::sqrt(centerDistSquared) - circle.radius)) > perimThickness.value()) continue;

                SDL_FRect pixel{ x, y, 1, 1 };

                SDL_RenderFillRect(renderer, &pixel);
            }
        }
    }
}

SDL_Texture* App::createSolidTexture(SDL_Renderer* renderer, SDL_Color color, int w, int h)
{
    SDL_Texture* texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, w, h);

    SDL_SetRenderTarget(renderer, texture);
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
    SDL_RenderClear(renderer);
    SDL_SetRenderTarget(renderer, nullptr); 

    return texture;
}

void App::handleInput()
{
    const bool* key_states = SDL_GetKeyboardState(NULL);
    static bool wasClicking = false;

    double diff = std::fmod(std::abs(rectAngle - indexAngle), 360.0);
    double circularDiff = std::min(diff, 360.0 - diff);

    bool isClicking = key_states[SDL_SCANCODE_SPACE] || key_states[SDL_SCANCODE_UP] || key_states[SDL_SCANCODE_W];
    if (isClicking && !wasClicking)
    {
        if (circularDiff <= 5.00) // value that looked most accurate when timing the index
        {
            ++score;
            //std::cout << rectAngle << std::endl << indexAngle << std::endl << std::endl;

            // direction switch
            if (score >= 10)
            {
                bool isOdd = std::rand() % 2;
                if (isOdd) indexSpeed *= 1;
                else indexSpeed *= -1;
            }

            if (indexSpeed >= 0) rectAngle += (120 + static_cast<double>(std::rand() % 60));
            else rectAngle -= (120 + static_cast<double>(std::rand() % 60));
        }
        else
        {
            lossTransition();
        }
    }
    wasClicking = isClicking;

    double angleToCompare = rectAngle - 30; // only used to prevent wrap around on reverse direction
    if (angleToCompare < 0) angleToCompare += 360;
    if ((std::abs(indexAngle - std::fmod(rectAngle + 30.00, 360)) < 2 && indexSpeed >= 0) || (std::abs(indexAngle - std::fmod(angleToCompare, 360)) < 2 && indexSpeed < 0))
    {
        lossTransition();
    }
}

void App::lossTransition()
{
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
    SDL_SetRenderScale(renderer, 3.0f, 3.0f);
    SDL_RenderDebugText(renderer, 10, 60, "missed"); 
    SDL_SetRenderScale(renderer, 1.0f, 1.0f);
    SDL_RenderPresent(renderer);

    SDL_Delay(2000);

    if (score > peakScore) peakScore = score;
    score = 0;
    indexAngle = 0;
    indexSpeed = baseIndexSpeed;
    menu();
}