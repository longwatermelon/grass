#include "grass.h"


Grass::Grass()
{
	SDL_Init(SDL_INIT_VIDEO);

	m_window = SDL_CreateWindow("Grass", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 1000, 1000, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
	m_rend = SDL_CreateRenderer(m_window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

	SDL_RenderClear(m_rend);
	SDL_RenderPresent(m_rend);
}


Grass::~Grass()
{
	SDL_DestroyRenderer(m_rend);
	SDL_DestroyWindow(m_window);

	SDL_Quit();
}


void Grass::mainloop()
{
	bool running = true;
	SDL_Event evt;

	while (running)
	{
		while (SDL_PollEvent(&evt))
		{
			switch (evt.type)
			{
			case SDL_QUIT:
				running = false;
				break;
			}
		}

		SDL_RenderClear(m_rend);

		SDL_RenderPresent(m_rend);
	}
}