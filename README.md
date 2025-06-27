# harryIA

this is a implementation of the api of a llama file without using the integrated browser. using c++.

## how to build
use "make STATIC=1" will generate harry.exe . currently only builds on windows but it can be changed to work in others os too!

## after building notes
needs to have a LLM llamafile path on the "EJECUTABLE" option on config.ini. if you want to run the LLM yourself, set HABILITAR_SERVIDOR to 0 on config.ini 

## speak sopport 
uses VLC to reproduce audio. put the VLC executable on VLC_EXE option on config.ini

