c:\gbdk\bin\lcc -Wa-l -Wl-m -Wf--debug -Wl-y -Wl-w -c -o main.o main.c
c:\gbdk\bin\lcc -Wa-l -Wl-m -Wf--debug -Wl-y -Wl-w -c -o level2.o level2.c
c:\gbdk\bin\bankpack -ext=.rel -v -yt19 main.o level2.o
c:\gbdk\bin\lcc -Wa-l -Wl-m  -Wl-yt19 -Wl-yo4 -Wl-ya4 -o main.gb main.rel level2.rel