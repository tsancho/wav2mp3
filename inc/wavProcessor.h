#ifndef WAVPROCESSOR_H_INCLUDED
#define WAVPROCESSOR_H_INCLUDED
#include <iostream>
#include <unistd.h>
#include <lame/lame.h>

class wavProcessor
{
    public:
        wavProcessor(FILE *inWavFile);
        virtual ~wavProcessor();


        bool initDecoder(lame_t *lameSettings, FILE *inWavFile);
        bool decodeProcess( short int *buf, uint32_t *bufSize, FILE *inWavFile);

    protected:

    private:
        struct riff_t
        {
            char chunkID[4];
            uint32_t chunkSize;
            uint32_t format;
        };

        struct format_t
        {
            char subChunk1ID[4];
            uint32_t subChunk1Size;
            uint16_t audioFormat;
            uint16_t numChannels;
            uint32_t sampleRate;
            uint32_t byteRate;
            uint16_t blockAlign;
            uint16_t bitsPerSample;
        };

        struct dataHeader_t
        {
            char subChunk2ID[4];
            uint32_t subChunk2Size;
        };

        struct wavHeader_t
        {
            riff_t riff;
            format_t format;
            dataHeader_t dataHeader;
        };
        wavHeader_t wavhdr;

        FILE *infile;
        bool extractWAVHeader(FILE *inpwav, wavHeader_t *wavHdr);
        bool initPopulateLameSettingsStr(lame_t *lameSettings, wavHeader_t *wavHdr);

};

#endif // WAVPROCESSOR_H_INCLUDED
