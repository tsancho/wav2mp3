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
    if(strstr(wavHdr->dataHeader.subChunk2ID,"data"))//checking if file is PCM-type and consist "data" section in the header
        cout << "data header is ok! " <<  endl;
    else
    {
        cout << "data header is wrong! " << endl;
        return false;
    }

    return true;
}

bool wavProcessor::initPopulateLameSettingsStr(lame_t *lameSettings, wavHeader_t *wavHdr, int *mp3bufSize)
{
    *lameSettings = lame_init();
    *mp3bufSize = (1.25 * (4096/(wavHdr->format.bitsPerSample/8)) + 7200); // got from LAME lib
    cout << "mp3bufsize is: " << *mp3bufSize << endl;
    lame_set_in_samplerate(*lameSettings, wavHdr->format.sampleRate );
    lame_set_out_samplerate(*lameSettings, wavHdr->format.sampleRate );
    lame_set_VBR(*lameSettings, vbr_default);
    lame_set_quality(*lameSettings, LAME_QUALITY);
    lame_init_params(*lameSettings);
    return true;
}

bool wavProcessor::initDecoder(lame_t *lameSettings, FILE *inWavFile, int *mp3bufSize)
{
    if(!extractWAVHeader(inWavFile, &wavhdr))
    {
        cout << "Error! file header cannot be read! " << endl;
        return true;
    }
    initPopulateLameSettingsStr(lameSettings, &wavhdr, mp3bufSize);
    return false;

}

bool wavProcessor::decodeProcess( short int *buf, uint32_t *bufSize, FILE *inWavFile)
{

    *bufSize = fread(buf, 2*sizeof(short int), BUFSIZE, inWavFile);
    return true;
}
