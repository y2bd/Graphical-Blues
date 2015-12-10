#ifndef SOUND_H
#define SOUND_H

#pragma comment(lib, "irrKlang.lib")
#include <ik/irrKlang.h>

#include <string>

namespace Sound
{
    using namespace irrklang;
    
    bool InitEngine();
    unsigned int play(const char *filename);
    unsigned int playInLoop(const char *filename);
    void stop(unsigned int id);
}

#endif