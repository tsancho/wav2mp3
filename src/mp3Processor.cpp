#include "../inc/mp3Processor.h"
#include "lame/lame.h"
#include "malloc.h"

#define MP3_BUF_SIZE 8192
#define MP3_QUALITY  2


using namespace std;

mp3Processor::mp3Processor()
{

    //ctor
}

mp3Processor::~mp3Processor()
{
    //dtor
}

bool mp3Processor::initEcnoder(lame_t *lameSettings, int quality, int *mp3BufSize)
{
    lame_set_quality(*lameSettings, quality);
    lame_init_params(*lameSettings);
}

bool mp3Processor::encodeProcess(lame_t *lameSettings, short int *wavBuf, uint32_t wavBufSize, int mp3bufSize, FILE *outMp3)
{
    int writeSize = 0;
    int channels = lame_get_num_channels(*lameSettings);
    unsigned char *mp3_buffer = (unsigned char *)malloc(mp3bufSize);
        if (wavBufSize == 0)
        {
            writeSize = lame_encode_flush(*lameSettings, mp3_buffer, mp3bufSize);
        }
        else
        {
            if (channels == 1)
            {
                writeSize = lame_encode_buffer(*lameSettings, wavBuf, NULL, wavBufSize, mp3_buffer, mp3bufSize);

            }
            else
            {
                writeSize = lame_encode_buffer_interleaved(*lameSettings, wavBuf, wavBufSize, mp3_buffer, mp3bufSize);
            }
        }
        fwrite(mp3_buffer, writeSize, 1, outMp3);
        free(mp3_buffer);
    return true;
}
