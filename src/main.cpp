#include <iostream>
#include <unistd.h>
#include <lame/lame.h>
#include <pthread.h>
#include <sys/stat.h>
#include <malloc.h>
#include <vector>
#include <io.h>
#include <dirent.h>

#include "../inc/main.h"
#include "../inc/mp3Processor.h"
#include "../inc/wavProcessor.h"

const int WAV_SIZE = 8192;
using namespace std;


void *runDecoding(void *arg)
{

    uint32_t read;
    cout <<"filename: " <<' ' <<(char *)arg << "\n" << endl;
    string wavFileName = (char *) arg;

    FILE *wav = fopen(wavFileName.c_str(), "rb");  //source
//    const int WAV_SIZE = 8192;//be carefull with this size, it may cause some problems multiplier 100 causes reboot
//    const int MP3_SIZE = 8192;
    lame_t lame;
    short int *wavBuf = (short int *)malloc(WAV_SIZE*4);
//    uint32_t wavBufSize = 0;
    wavProcessor wavPrc(wav);
    mp3Processor mp3Prc;
    string mp3FileName=wavFileName.substr(0,wavFileName.find_last_of('.'))+".mp3";
//    mp3FileName = wavFileName.
    FILE *mp3 = fopen(mp3FileName.c_str(), "wb");  //output

//    printf("wav filename is: %s\n",wavFileName );
//    printf("mp3 filename is: %s\n",mp3FileName );
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
//    int tID = (int)threadID;
//    printf("hello man! I'm thread %d and %s\n", tID, arg);
//    pthread_exit(NULL);

//    return NULL;
}

int getFilesNumber(char *folderName)
{
    DIR *dir;
    struct dirent *ent;
    int val = 0;
    if ((dir = opendir (folderName)) != NULL)
    {
        while ((ent = readdir (dir)) != NULL)
        {
            printf("curval %d\n", val);
            val++;
        }
        closedir (dir);
    }
    return val;
}

int getFilesList(char *folderName, std::vector <string> *names)
{
    std::vector <char *> fileList;
    std::vector <char *>::iterator it;
    std::string::size_type idx;

    DIR *dir;
    struct dirent *ent;
    auto val = 0;

    if ((dir = opendir (folderName)) != NULL)
    {
        while ((ent = readdir (dir)) != NULL)
        {
            std::string filename(ent->d_name);

            idx = filename.rfind('.');

            if(idx != std::string::npos)
            {
                std::string extension = filename.substr(idx+1);
                if(extension == "wav")
                    names->insert(names->end(),ent->d_name);

            }
            else
            {
                // No extension found
            }
            printf ("%s\n", ent->d_name);
            val++;
        }
        closedir (dir);
    }
    else
    {
        perror ("Directory is unreachable!");
    }
    return val;
}

int main(int argc, char *argv[])
{

    pthread_t thread[THREAD_NUMBER];


    cout << argv[1] << endl;
    std::vector <string> files1;
    int sz = getFilesList(argv[1],&files1);
    chdir(argv[1]);
    printf("files1 are read %d\n", sz);
//
//    for(uint32_t v = 0; v < files1.size(); v++)
//        std::cout << ' ' << files1.at(v)<< "\n";

    std::cout << ' ' << files1.size()<< "\n";
    int rc;
    for(uint32_t t = 0; t < THREAD_NUMBER; t++)
    {
        printf("Creating thread # %d \n", t);
        std::cout << ' ' << files1.at(t)<< "\n";
        const char *sendfile = files1.at(t).c_str();
//        runDecoding((void *) files1.at(t).c_str());   //without multithreading works really nice
        rc = pthread_create(&thread[t], NULL, runDecoding, (void *) files1.at(t).c_str());//"file_example_WAV_1MG.wav");// &files1.at(t));
    }


//    printf("total cycles done %d\n", nTotalRead);
    return 0;
}


