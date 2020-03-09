#include "../inc/wavProcessor.h"
#include <stdio.h>
#include <string.h>

using namespace std;

#define BUFSIZE 8192
#define PCM_HEADER_LENGTH 44

wavProcessor::wavProcessor()
{
//    infile = inWavFile;
//    lame_t lameSettings;
//    initDecoder(&lameSettings, inWavFile);
    //ctor
}

wavProcessor::~wavProcessor()
{

    //dtor
}

//int wavProcessor::getWavHeader(wavHeader_t *wavHeader)
//{
//
//}


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

//    hdr = (header_t *)arr;
//    printf("size of header %d\n", sizeof(wavHeader_t));
    memcpy((char *)wavHdr, arr, sizeof(wavHeader_t));
    if(strcmp(wavHdr->dataHeader.subChunk2ID,"data"))
        cout << "data header is ok!\n" << endl;
    else
        cout << "data header is not ok!\n" << endl;


    return true;
}

int wavProcessor::initPopulateLameSettingsStr(lame_t *lameSettings, wavHeader_t wavHdr, int *mp3bufSize)
{
//    wavHeader_t wavHdr;
//    if(getWavHeader(&wavHdr))
//    {
//        cout << "Error! Unreadable header!\n" << endl;
//        return -1;
//    }
    *lameSettings = lame_init();
    lame_set_in_samplerate(*lameSettings, wavHdr.format.sampleRate );
    lame_set_out_samplerate(*lameSettings, wavHdr.format.sampleRate );
    lame_set_VBR(*lameSettings, vbr_default);
    *mp3bufSize = (1.25 * (4096/(wavHdr.format.bitsPerSample/8)) + 7200); // worst-case estimate from lame/API
    if(lame_init_params(*lameSettings) < 0)//check condition
    {
        cout << "Lame init failed!\n" << endl;
        return -2;
    }
    return 0;
}

int wavProcessor::initDecoder(lame_t *lameSettings, FILE *inWavFile)
{
    int mp3bufsize;
    wavHeader_t *wavHdr;
    if(!extractWAVHeader(inWavFile, wavHdr))
        cout << "wtf!\n" << endl;
    if(initPopulateLameSettingsStr(lameSettings, *wavHdr ,&mp3bufsize) < 0)
    {
        cout << "init lame settings failed!\n" << endl;
        return -1;
    }
    cout << "bufsize is! " << mp3bufsize<< endl;

    mp3bufsize = 8192;
    return mp3bufsize;
}

bool wavProcessor::decodeProcess( short int *buf, uint32_t *bufSize, FILE *inWavFile)
{

    *bufSize = fread(buf, 2*sizeof(short int), BUFSIZE, inWavFile);
//    printf("read bufsize is %d\n", *bufSize);
//    infile

    return true;
}
