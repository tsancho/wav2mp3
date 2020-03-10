# wav2mp3
test project to evaluate lame library and code::blocks IDE as a cross-platform development tool 

Installation process:
There are two ways of compiling project on Linux and Windows.
Both of them requires installation of the pthreads and the libmp3lame library;
For windows both ways require installation of the mingw32;

1. With code::blocks IDE.
Using mingw32 as a default GCC compiler.
After that paths to libraries should be added to the project (check notes howto).

2. With makefile. 
For windows using following command in Msys or cmd:
mingw32-make -f wav2mp3.cbp.mak
before running check wav2mp3.cbp.mak (makefile) if paths to libraries files and files themselve are correct.

For Linux way is the same but file wav2mp3.cbp.mak.unix should be used instead. Also paths to files as well as files themselves for my ubuntu were like these:
/usr/lib/x86_64-linux-gnu/libmp3lame.a 
/usr/lib/x86_64-linux-gnu/libthread.so

***
notes: 
- project is being developed in Win with MinGW;
- to work with LAME lib it's enough to install it in MinGW with these options: ./configure --prefix=/mingw --disable-shared --enable-static --disable-frontend;
- after creating the project we need to add the compiled LAME library (libmp3lame.a) like this:
1. Choose Project, Build Options.
The Project Build Options dialog box appears.
2. Click the Linker Settings tab.
3. Click the Add button.
4. In Add Library dialog box, click the Ellipsis (…) button to browse to the location of the library file.
- to install pthreads we need to run msys with this command:
mingw-get
mingw-get install pthreads

