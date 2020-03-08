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
std::vector <string> files1;
int globalFileIndex;
pthread_mutex_t lock;
//using namespace thread;
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
    string *filename;

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
    uint32_t read;
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
            cout <<"file is unavailable: " <<' ' << wavFileName << "\n" << endl;

        lame_t lame;
        short int *wavBuf = (short int *)malloc(WAV_SIZE*4);

        wavProcessor wavPrc(wav);
        mp3Processor mp3Prc;
        wavFileName = wavFileName.substr(0,wavFileName.find_last_of('.'));
        string mp3FileName =wavFileName +".mp3";

        FILE *mp3 = fopen(mp3FileName.c_str(), "wb");  //output
        wavPrc.initDecoder(&lame, wav);

        do
        {
            wavPrc.decodeProcess(wavBuf, &read, wav);
            mp3Prc.encodeProcess(&lame, wavBuf, &read, mp3);
        }
        while(read != 0);
        free(wavBuf);
        lame_close(lame);
        fclose(mp3);
        fclose(wav);
        cout << "successfully converted file: " << mp3FileName.c_str() << endl;
    }
    return NULL;
}



void *converterWrap(void *object)
{
    string *filen = (string *)((wrapStruct_t *)object)->filename;
    ((fileConverter *)object)->runConverter((void *)filen);
    return NULL;
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

    int numCPU = 1, systemtype = 0;
    globalFileIndex = 0;
    #if __WIN32__
    {
        SYSTEM_INFO sysinfo;
        GetSystemInfo(&sysinfo);
        numCPU = sysinfo.dwNumberOfProcessors;
        systemtype = 1;
    }
    #elif __UNIX__
        numCPU = sysconf(_SC_NPROCESSORS_ONLN);
        systemtype = 2;
    #endif
    if (pthread_mutex_init(&lock, NULL) != 0)
    {
        printf("\n mutex init failed\n");
        return 1;
    }
    pthread_t thread[THREAD_NUMBER];
    std::cout << "num CPUs"<< ' ' <<numCPU << "system type is" << systemtype <<std::endl;
    cout << argv[1] << endl;
    int sz = getFilesList(argv[1],&files1);
    chdir(argv[1]);
    printf("files1 are read %d\n", sz);
    std::cout << ' ' << files1.size()<< "\n";
    int rc;
    std::vector<fileConverter*> threads;
    for(uint32_t t = 0; t < THREAD_NUMBER; t++)
    {
        printf("Creating thread # %d \n", t);

        wrapStruct_t wrapStruct;// =  (wrapStruct_t *)malloc(sizeof(wrapStruct_t));

        threads.push_back(new fileConverter());

        std::cout << ' ' << files1.at(t)<< "\n";

        wrapStruct.fc = threads.at(t);
//
        if(getNextFileName(wrapStruct.filename))//"file_example_WAV_10MG.wav";//
            break;

        rc = pthread_create(&thread[t], NULL, &converterWrap,(void *)&wrapStruct); //(void *)threads.at(t));////(void *) files1.at(t).c_str());//"file_example_WAV_1MG.wav");// &files1.at(t));
        if(rc)
        {
            std::cout << "warning! thread can't be created with error %d \n" << rc << endl;
            return rc;
        }
    }
        for(auto t = 0; t < THREAD_NUMBER; t++)
            pthread_join(thread[t], NULL);

    return 0;
}


