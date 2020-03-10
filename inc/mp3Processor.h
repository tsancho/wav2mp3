#ifndef MP3PROCESSOR_H_INCLUDED
#define MP3PROCESSOR_H_INCLUDED
#include <lame/lame.h>
#include <iostream>
#include <unistd.h>



class mp3Processor
{
    public:
        mp3Processor();
        virtual ~mp3Processor();


        bool encodeProcess(lame_t *lameSettings, short int *wavBuf, uint32_t wavBufSize, int mp3bufSize, FILE *outMp3);
    protected:

    private:

};

#endif // MP3PROCESSOR_H_INCLUDED
