#include "Sound.h"

namespace Sound
{
    using namespace irrklang;

    ISoundEngine* engine;

    bool InitEngine()
    {
        engine = createIrrKlangDevice();
        return engine != 0;
    }

    unsigned int play(const char * filename)
    {
        return (unsigned int)engine->play2D(filename, false, false, true);
    }

    unsigned int playInLoop(const char * filename)
    {
        return (unsigned int)engine->play2D(filename, true, false, true);
    }

    void stop(unsigned int id)
    {
        ((ISound *)id)->stop();
    }
}
