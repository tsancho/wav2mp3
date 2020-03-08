#ifndef MAIN_H_INCLUDED
#define MAIN_H_INCLUDED

//const int THREAD_NUMBER = 8;
//const char* wavFileName = "file_example_WAV_1MG.wav";
//const char* mp3FileName = "file_example_WAV_1MG.mp3";

class fileConverter
{
    public:
        fileConverter();
        virtual ~fileConverter();
        void *runConverter(void *wavFileName);
    private:
    protected:

};


#endif // MAIN_H_INCLUDED
