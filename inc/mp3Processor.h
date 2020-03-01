#ifndef MP3PROCESSOR_H_INCLUDED
#define MP3PROCESSOR_H_INCLUDED
#include <lame/lame.h>
#include <iostream>
#include <unistd.h>

class mp3Processor
{
    public:
        mp3Processor(FILE *infile);
        virtual ~mp3Processor();

//        unsigned int Getm_Counter() { return m_Counter; }
//        void Setm_Counter(unsigned int val) { m_Counter = val; }
        bool encodeProcess(lame_t *lameSettings, FILE *outfile);
    protected:

    private:

};

#endif // MP3PROCESSOR_H_INCLUDED
