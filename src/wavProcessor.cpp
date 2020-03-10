#include "../inc/wavProcessor.h"
#include <stdio.h>
#include <string.h>

using namespace std;

#define BUFSIZE 8192
#define PCM_HEADER_LENGTH 44
#define LAME_QUALITY 2

wavProcessor::wavProcessor()
{

}

wavProcessor::~wavProcessor()
{

}

bool wavProcessor::extractWAVHeader(FILE *inWavFile, wavHeader_t *wavHdr)
{
    char *arr = (char *)malloc(PCM_HEADER_LENGTH);
//    auto ret = 0;
    if(inWavFile == NULL)
    {
        cout << "Error! input file does not exist!" << endl;
        return false;
    }

    if( !fread(arr, sizeof(char), sizeof(wavHeader_t), inWavFile))
    {
        cout << "Error! file header cannot be read! " << endl;
        return false;
    }


    memcpy((char *)wavHdr, arr, sizeof(wavHeader_t));
    if(strcmp(wavHdr->dataHeader.subChunk2ID,"data"))
        cout << "data header is ok!\n" << endl;
    else
        cout << "data header is not ok!\n" << endl;

    return true;
}

bool wavProcessor::initPopulateLameSettingsStr(lame_t *lameSettings, wavHeader_t *wavHdr)
{
    *lameSettings = lame_init();
    lame_set_in_samplerate(*lameSettings, wavHdr->format.sampleRate );
    lame_set_out_samplerate(*lameSettings, wavHdr->format.sampleRate );
    lame_set_VBR(*lameSettings, vbr_default);
    lame_set_quality(*lameSettings, LAME_QUALITY);
    lame_init_params(*lameSettings);
    return true;
}

bool wavProcessor::initDecoder(lame_t *lameSettings, FILE *inWavFile)
{
    if(!extractWAVHeader(inWavFile, &wavhdr))
        cout << "wtf!\n" << endl;
    initPopulateLameSettingsStr(lameSettings, &wavhdr);
    return true;

}

bool wavProcessor::decodeProcess( short int *buf, uint32_t *bufSize, FILE *inWavFile)
{

    *bufSize = fread(buf, 2*sizeof(short int), BUFSIZE, inWavFile);
//    printf("read bufsize is %d\n", *bufSize);
//    infile

    return true;
}
