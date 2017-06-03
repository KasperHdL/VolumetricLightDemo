#pragma once


class Timer{
    public:
        Uint64 start_time = 0;
        float duration = 0;

        void start(){
            start_time = SDL_GetPerformanceCounter();
        }

        float stop(){
            Uint64 end_time = SDL_GetPerformanceCounter();

            duration = (float)(end_time - start_time) / SDL_GetPerformanceFrequency();
            return duration;
        }
};
