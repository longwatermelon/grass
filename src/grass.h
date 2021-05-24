#pragma once
#include <SDL.h>


class Grass
{
public:
	Grass();
	~Grass();

public:
	void mainloop();

private:
	SDL_Window* m_window;
	SDL_Renderer* m_rend;
};