#include "../inc/mp3Processor.h"
#include "lame/lame.h"
#include "malloc.h"



using namespace std;

mp3Processor::mp3Processor()
{
    //ctor
}

mp3Processor::~mp3Processor()
{
    //dtor
}

bool mp3Processor::encodeProcess(lame_t *lameSettings, short int *wavBuf, uint32_t wavBufSize, int mp3bufSize, FILE *outMp3)
{
    int writeSize = 0;
    bool retval = false;
    int channels = lame_get_num_channels(*lameSettings);
    unsigned char *mp3_buffer = (unsigned char *)malloc(mp3bufSize);
        if (wavBufSize == 0)
        {
            writeSize = lame_encode_flush(*lameSettings, mp3_buffer, mp3bufSize);
            if(writeSize < 0)
            {
                cout << "lame flush failed ok!\n" << endl;
                retval = true;
            }
        }
        else
        {
            if (channels == 1)
            {
                writeSize = lame_encode_buffer(*lameSettings, wavBuf, NULL, wavBufSize, mp3_buffer, mp3bufSize);
                if(writeSize < 0)
                {
                    cout << "lame encode_buffer failed ok!\n" << endl;
                    retval = true;
                }
            }
            else
            {
                writeSize = lame_encode_buffer_interleaved(*lameSettings, wavBuf, wavBufSize, mp3_buffer, mp3bufSize);
                if(writeSize < 0)
                {
                    cout << "lame encode_buffer_interleaved failed ok!\n" << endl;
                    retval = true;
                }
            }
        }
        if(!retval)
            fwrite(mp3_buffer, writeSize, 1, outMp3);
        free(mp3_buffer);
    return retval;
}
