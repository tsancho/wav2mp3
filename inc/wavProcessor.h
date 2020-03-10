#ifndef WAVPROCESSOR_H_INCLUDED
#define WAVPROCESSOR_H_INCLUDED
#include <iostream>
#include <unistd.h>
#include <lame/lame.h>

class wavProcessor
{
    public:
        wavProcessor();
        virtual ~wavProcessor();


        bool initDecoder(lame_t *lameSettings, FILE *inWavFile, int *mp3bufSize);
        bool decodeProcess( short int *buf, uint32_t *bufSize, FILE *inWavFile);

    protected:

    private:
        struct riff_t
        {
            char chunkID[4];
            int32_t chunkSize;
            int32_t format;
        };

        struct format_t
        {
            char subChunk1ID[4];
            int32_t subChunk1Size;
            int16_t audioFormat;       //if PCM it is 1
            int16_t numChannels;
            int32_t sampleRate;
            int32_t byteRate;
            int16_t blockAlign;
            int16_t bitsPerSample;
        };

        struct dataHeader_t
        {
            char subChunk2ID[4];
            int32_t subChunk2Size;
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
        bool initPopulateLameSettingsStr(lame_t *lameSettings, wavHeader_t *wavHdr, int *mp3bufSize);

};

#endif // WAVPROCESSOR_H_INCLUDED
