#include <iostream>
#include <unistd.h>
#include <lame/lame.h>
#include <pthread.h>
#include <sys/stat.h>
#include <malloc.h>
#include <vector>
#include <dirent.h>

#include "../inc/main.h"
#include "../inc/mp3Processor.h"
#include "../inc/wavProcessor.h"


const int WAV_SIZE = 8192;
using namespace std;
std::vector <std::string> files1;
unsigned int globalFileIndex;
pthread_mutex_t lock;
int getNextFileName( string *filename  )
{
    int retval = 0;
    pthread_mutex_lock(&lock);
    if(globalFileIndex < files1.size())
    {
        *filename = files1.at(globalFileIndex);
        retval = 0;
    }
    else
    {
        *filename = "";
        retval = -1;
    }
    globalFileIndex++;
    pthread_mutex_unlock(&lock);
    return retval;
}
struct wrapStruct_t
{
    fileConverter *fc;
    std::string filename;

};

fileConverter::fileConverter()
{
    //ctor
}

fileConverter::~fileConverter()
{
    //dtor
}

void  *fileConverter::runConverter(void *arg)
{
    bool firstCycle = true;
    string wavFileName;
    while(1)
    {
        if(firstCycle)
            wavFileName = *((string *) arg);
        else
        {
            if(getNextFileName(&wavFileName) == -1)
                return NULL;
        }
        firstCycle = false;
        cout <<"converting file: " <<' ' << wavFileName << "\n" << endl;

        FILE *wav = fopen(wavFileName.c_str(), "rb");  //source
        if(wav == NULL)
            cout <<"file is unavailable: " << wavFileName << "\n" << endl;

        lame_t lame;
        short int *wavBuf = (short int *)malloc(WAV_SIZE*4);

        wavProcessor wavPrc;//(wav);
        mp3Processor mp3Prc;
        wavFileName = wavFileName.substr(0,wavFileName.find_last_of('.'));
        string mp3FileName =wavFileName +".mp3";
        int mp3bufsz = 0;
        uint32_t readBufSize = 0;
        FILE *mp3 = fopen(mp3FileName.c_str(), "wb");
        if(wavPrc.initDecoder(&lame, wav, &mp3bufsz))
        {
            cout << "file conversion aborted: " << mp3FileName.c_str() << endl;
            fclose(mp3);
            remove(mp3FileName.c_str());
        }
        else
        {
            do
            {
                wavPrc.decodeProcess(wavBuf, &readBufSize, wav);
                mp3Prc.encodeProcess(&lame, wavBuf, readBufSize, mp3bufsz, mp3);
            }
            while(readBufSize != 0);
            cout << "successfully converted file: " << mp3FileName.c_str() << endl;
            fclose(mp3);
        }
        free(wavBuf);
        lame_close(lame);
        fclose(wav);
    }
    return NULL;
}



void *converterWrap(void *object)
{
    string filen = (string )((wrapStruct_t *)object)->filename;
    ((fileConverter *)object)->runConverter((void *)&filen);
    return NULL;
}

int getFilesList(char *folderName, std::vector <string> *names)
{
    std::vector <char *> fileList;
    std::vector <char *>::iterator it;
    std::string::size_type idx;

    DIR *dir;
    struct dirent *ent;
    auto val = 0, wavs = 0;

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
                {
                    names->insert(names->end(),ent->d_name);
                    wavs++;
                }

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
        printf ("Directory is unreachable!\n");
        val = -1;
    }
    if(!wavs)
    {
        printf ("No .WAV files detected in directory!\n");
        val = -1;
    }

    return val;
}

int main(int argc, char *argv[])
{

    int numCPU = 1;
    string systemtype = "unknown";
    globalFileIndex = 0;
    #if __WIN32__ or __WIN32 or __WIN64
    {
        SYSTEM_INFO sysinfo;
        GetSystemInfo(&sysinfo);
        numCPU = sysinfo.dwNumberOfProcessors;
        systemtype = "windows";
    }
    #elif unix or __unix or __unix__ or __linux__
        numCPU = sysconf(_SC_NPROCESSORS_ONLN);
        systemtype = "unix";
    #endif
    if (pthread_mutex_init(&lock, NULL) != 0)
    {
        printf("\n mutex init failed\n");
        return 1;
    }
    int threadNumber = numCPU;
    pthread_t thread[threadNumber];
    std::cout <<" system type is: " << systemtype << " number of available CPUs is: " << numCPU  <<std::endl;
    if(chdir(argv[1]))
    {
        cout << "Error! WAV storage directory is unreachable: " <<argv[1] << endl;
        return 2;
    }
    else
        cout << "Entering directory: " <<argv[1] << endl;
    int sz = getFilesList(argv[1],&files1);
    if(sz <= 2)
    {
        cout << "Error! Something is wrong with WAV storage directory, probably not enough WAV files? " <<argv[1] << endl;
        return 3;
    }
    cout << ' ' << files1.size()<< "\n";
    if(files1.size() < threadNumber)
        threadNumber = files1.size();
    int rc;
    vector<fileConverter*> threads;
    wrapStruct_t wrapStruct;
    for(auto t = 0; t < threadNumber; t++)
    {
        printf("Creating thread # %d \n", t);
        threads.push_back(new fileConverter());
        std::cout << ' ' << files1.at(t)<< "\n";
        wrapStruct.fc = threads.at(t);
        if(getNextFileName(&(wrapStruct.filename)))
            break;
        rc = pthread_create(&thread[t], NULL, &converterWrap,(void *)&wrapStruct);
        if(rc)
        {
            std::cout << "error! thread can't be created with error %d \n" << rc << endl;
            return rc;
        }
    }
    for(auto t = 0; t < threadNumber; t++)
        pthread_join(thread[t], NULL);

    return 0;
}
