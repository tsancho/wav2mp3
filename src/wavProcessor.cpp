#include "../inc/wavProcessor.h"
#include <stdio.h>
#include <string.h>
using namespace std;
#define BUFSIZE 8192
#define PCM_HEADER_LENGTH 44
wavProcessor::wavProcessor(FILE *inWavFile)
{
    infile = inWavFile;
//    lame_t lameSettings;
//    initDecoder(&lameSettings, inWavFile);
    //ctor
}

wavProcessor::~wavProcessor()
{

    //dtor
}

bool wavProcessor::extractWAVHeader(FILE *inWavFile, wavHeader_t *wavHdr)
{
    char arr[PCM_HEADER_LENGTH];
    auto ret = 0;
    if(inWavFile == NULL)
    {
        cout << "Error! input file does not exist!" << endl;
        return false;
    }
    ret = fread(arr, sizeof(char), sizeof(wavHeader_t), inWavFile);
    if( ret == 0)
    {
        cout << "Error! file header cannot be read! " << endl;
        return false;
    }

//    hdr = (header_t *)arr;
    printf("size of header %d\n", sizeof(wavHeader_t));
    memcpy((char *)wavHdr, arr, sizeof(wavHeader_t));
    if(strcmp(wavHdr->dataHeader.subChunk2ID,"data"))
        cout << "data header is ok!\n" << endl;
    else
        cout << "data header is not ok!\n" << endl;
    for(auto val = 0; val < sizeof(wavHeader_t); val++)
        printf("\\x%.2x", arr[val]);
    cout << "\n" << endl;
    if(wavHdr->dataHeader.subChunk2Size == 0)
        cout << "data header is not ok!\n" << endl;
    printf("channels number is: %d \n", wavHdr->format.numChannels);
    printf("sample rate is: %d \n", wavHdr->format.sampleRate);
    printf("subChunk1Size is: %d \n", wavHdr->format.subChunk1Size);
    printf("blockAlign is: %d \n", wavHdr->format.blockAlign);
    printf("byteRate is: %d \n", wavHdr->format.byteRate);
    printf("audioFormat is: %d \n", wavHdr->format.audioFormat);
    printf("bitsPerSample is: %d \n", wavHdr->format.bitsPerSample);

    printf("data size is: %d \n", wavHdr->dataHeader.subChunk2Size);
    for(auto i = 0; i < sizeof(wavHdr->dataHeader.subChunk2ID); i++)
        printf("\\x%.2x", wavHdr->dataHeader.subChunk2ID[i]);
    cout << "\n" << endl;

//    auto val = 0;
//    while( val < sizeof(header_t))
//    {
//        char tmp = ((char *)hdr)[val];
//        printf("\\x%.2x", tmp);
//        val++;
//    }

    return true;
}

bool wavProcessor::initPopulateLameSettingsStr(lame_t *lameSettings, wavHeader_t *wavHdr)
{
    *lameSettings = lame_init();
    lame_set_in_samplerate(*lameSettings, wavHdr->format.sampleRate );
    lame_set_out_samplerate(*lameSettings, wavHdr->format.sampleRate );
    lame_set_VBR(*lameSettings, vbr_default);
    lame_init_params(*lameSettings);
    return true;
}

bool wavProcessor::initDecoder(lame_t *lameSettings)
{
    if(!extractWAVHeader(infile, &wavhdr))
        cout << "wtf!\n" << endl;
    initPopulateLameSettingsStr(lameSettings, &wavhdr);

}

bool wavProcessor::decodeProcess(lame_t *lameSettings, short int *buf, uint32_t *bufSize)
{

    *bufSize = fread(buf, 2*sizeof(short int), BUFSIZE, infile);
    printf("read bufsize is %d\n", *bufSize);
//    infile.

    return true;
}
