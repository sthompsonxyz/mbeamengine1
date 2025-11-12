#pragma once


bool initAudio() {
    //Initialize SDL
    if (SDL_Init(SDL_INIT_AUDIO) < 0)
    {
        printf("SDL could not initialize! SDL Error: %s\n", SDL_GetError());
        return false;
    }

    if (Mix_OpenAudio(44100, SDL_AUDIO_S16LSB, 2, 2048) < 0)
    {
        printf("SDL_mixer could not initialize! SDL_mixer Error: %s\n", SDL_GetError());

        return false;
    }
    
    std::cout << "opened sdl audio mixer" << std::endl;
        
    return true;
}

Mix_Chunk* setupwavchunk(std::string wavname) {
    std::string syndminiwav = "E:/source/biogamedev/vkBallsPong2/audiosamples/" + wavname;
    return Mix_LoadWAV(syndminiwav.c_str());
    //Mix_PlayChannel(-1, wavchunk, 0);
    /*if (wavchunk == NULL)
    {
        printf("Failed to load scratch sound effect! SDL_mixer Error: %s\n", SDL_GetError());
        return false;
    }
    std::cout << "loaded " + wavname << std::endl;
    return true;*/
}

void sdlaudiotest() {

    std::string evemus = "E:/source/biogamedev/vkBallsPong2/audiosamples/thingy1.wav";
    Mix_Music* gMusic = Mix_LoadMUS(evemus.c_str());
    if (gMusic == NULL)
    {
        printf("Failed to load beat music! SDL_mixer Error: %s\n", SDL_GetError());
        return;
    }

    Mix_VolumeMusic(50);
    Mix_PlayMusic(gMusic, -1);
    //std::cin.ignore();


    //Mix_PlayChannel(-1, syndminiwavchunk, 0);


    //std::cin.ignore();
}