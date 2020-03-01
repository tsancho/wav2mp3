#include <iostream>
#include <unistd.h>
#include <lame/lame.h>
#include <pthread.h>
#include <sys/stat.h>
#include <malloc.h>

#include "../inc/main.h"
#include "../inc/mp3Processor.h"
#include "../inc/wavProcessor.h"

using namespace std;


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
    uint32_t read, write;
    FILE *wav = fopen(wavFileName, "rb");  //source
    const int WAV_SIZE = 8192;//be carefull with this size, it may cause some problems multiplier 100 causes reboot
//    const int MP3_SIZE = 8192;
    lame_t lame;
    short int *wavBuf = (short int *)malloc(WAV_SIZE*4);
    uint32_t wavBufSize = 0;
    wavProcessor wavPrc(wav);
    mp3Processor mp3Prc;
    FILE *mp3 = fopen(mp3FileName, "wb");  //output
//    short int wav_buffer[WAV_SIZE*2];
//    unsigned char mp3_buffer[MP3_SIZE];

    int nTotalRead=0;
    wavPrc.initDecoder(&lame);
    do
    {
        wavPrc.decodeProcess(wavBuf, &read);
        mp3Prc.encodeProcess(&lame, wavBuf, &read, mp3);
    }
    while(read != 0);
    free(wavBuf);
    lame_close(lame);
    fclose(mp3);
    fclose(wav);


    printf("total cycles done %d\n", nTotalRead);
    return 0;
}


