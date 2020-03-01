#include <iostream>
#include <unistd.h>
#include <lame/lame.h>
#include <pthread.h>
#include <sys/stat.h>

using namespace std;
const int THREAD_NUMBER = 5;
const char* wavFileName = "file_example_WAV_1MG.wav";
const char* mp3FileName = "file_example_WAV_1MG.mp3";

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

struct header_t
{
  riff_t riff;
  format_t format;
  dataHeader_t dataHeader;
};

bool extractWAVHeader(FILE *inpwav, header_t *wavHdr)
{
    char arr[sizeof (header_t)];
    if(inpwav == NULL)
    {
        cout << "Error! input file does not exist!" << endl;
        return false;
    }
    if(!fread(arr, sizeof(char), sizeof(header_t), inpwav))
    {
        cout << "Error! file header cannot be read! " << endl;
        return false;
    }
//    hdr = (header_t *)arr;
    printf("size of header %d\n", sizeof(header_t));
    memcpy((char *)wavHdr, arr, sizeof(header_t));
    if(strcmp(wavHdr->dataHeader.subChunk2ID,"data"))
        cout << "data header is ok!\n" << endl;
    if(wavHdr->dataHeader.subChunk2Size == 0)
        throw()
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
    for(auto val = 0; val < sizeof(header_t); val++)
        printf("\\x%.2x", arr[val]);
    cout << "\n" << endl;
    return true;
}

bool initPopulateLameSettingsStr(lame_t *lameStr, header_t *wavHdr)
{
    *lameStr = lame_init();
    lame_set_in_samplerate(*lameStr, wavHdr->format.sampleRate );
    lame_set_out_samplerate(*lameStr, wavHdr->format.sampleRate );
    lame_set_VBR(*lameStr, vbr_default);
    lame_init_params(*lameStr);
    return true;
}

void *printHello(void *threadID)
{
    int tID = (int)threadID;
    printf("hello man! I'm thread %d\n", tID);
    pthread_exit(NULL);

    return NULL;
}

int main(int argc, char *argv[])
{

    pthread_t thread[THREAD_NUMBER];
    int rc;
    for(int t = 0; t < THREAD_NUMBER; t++)
    {
        printf("Creating thread # %d\n", t);
        rc = pthread_create(&thread[t], NULL, printHello, (void *)t);
    }
//    pthread_exit(NULL);
    cout << argv[1] << endl;
    if(!chdir(argv[1]))
        cout << "welcome" << endl;
    else
        cout << "Directory is unreachable!" << endl;
//    try
//    {
    int read, write;
    header_t header;// = (header_t *)malloc(sizeof(header_t));
    FILE *wav = fopen(wavFileName, "rb");  //source
    if(extractWAVHeader(wav, &header))
        cout << "welcome!" << endl;
    else
        cout << "UNwelcome!" << endl;
    struct stat info;
    stat(wavFileName, &info);
    printf("%s: size=%ld\n", wavFileName, info.st_size);
//    printf("length of file_example_WAV_1MG is %d\n", fsize(wav));
//    fseek(wav, 0, SEEK_SET);                                   //skip file header


    FILE *mp3 = fopen(mp3FileName, "wb");  //output
    const int WAV_SIZE = 8192;//be carefull with this size, it may cause some problems multiplier 100 causes reboot
    const int MP3_SIZE = 8192;
    short int wav_buffer[WAV_SIZE*2];
    unsigned char mp3_buffer[MP3_SIZE];
//    cout << "welcome!1111" << endl;
    printf("sample rate is: %d \n", header.format.sampleRate);
    lame_t lame;
    initPopulateLameSettingsStr(&lame, &header);

    int nTotalRead=0;

    do {
        read = fread(wav_buffer, 2*sizeof(short int), WAV_SIZE, wav);//add some exception handler

        nTotalRead++;
        int channel = 2;
        if (read == 0)
        {
            write = lame_encode_flush(lame, mp3_buffer, MP3_SIZE);
//            cout << "Hello world!11" << endl;
        }
        else
        {
            if (channel == 1)
            {
                write = lame_encode_buffer(lame, wav_buffer, NULL, read, mp3_buffer, MP3_SIZE);
//                cout << "Hello world!33" << endl;

            }
            else
            {
                write = lame_encode_buffer_interleaved(lame, wav_buffer, read, mp3_buffer, MP3_SIZE);
//                cout << "Hello world!22" << endl;
//                printf("write is %d\n", write);
//                printf("read is %d\n", read);
            }
        }
//            write = lame_encode_buffer_interleaved(lame,wav_buffer, read, mp3_buffer, MP3_SIZE);
//            write = lame_encode_buffer(lame, wav_buffer,wav_buffer, read, mp3_buffer, MP3_SIZE);

        fwrite(mp3_buffer, write, 1, mp3);
    } while (read != 0);

    lame_close(lame);
    fclose(mp3);
    fclose(wav);
//    }
//    catch (NSException *exception)
//    {
//        NSLog("%@",[exception description]);
//    }

    printf("total cycles done %d\n", nTotalRead);
    cout << "Hello world!" << endl;
    return 0;
}


