# Multithreaded terminal game in C

## REQ:
* min terminal size: 32 row, 110 col
* xterm
- ncurses

Before run install: xterm # to resize terminal size ncurses # to display windows in terminal

Install before compilation (on ubuntu):
    sudo apt install xterm
    sudo apt-get install libncurses5-dev libncursesw5-dev

to clear terminal mesh after game crash use:
    stty sane
    if {ENTER} dosnt work use: stty sane ^J {CTRL + J}

Good for debug:
    watch -n 1 ls /dev/shm
    rm /dev/shm/C_SAVE_SHM /dev/shm/sem.C_SAVE_SEM /dev/shm/S_SAVE_SHM /dev/shm/sem.S_SAVE_SEM
