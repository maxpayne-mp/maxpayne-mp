/*

            .        :    :::.        .,::      .:   ::::::::::. :::.    .-:.     ::-.:::.    :::..,::::::
            ;;,.    ;;;   ;;`;;       `;;;,  .,;;     `;;;```.;;;;;`;;    ';;.   ;;;;'`;;;;,  `;;;;;;;''''
            [[[[, ,[[[[, ,[[ '[[,       '[[,,[['       `]]nnn]]',[[ '[[,    '[[,[[['    [[[[[. '[[ [[cccc
            $$$$$$$$"$$$c$$$cc$$$c       Y$$$P          $$$""  c$$$cc$$$c     c$$"      $$$ "Y$c$$ $$""""
            888 Y88" 888o888   888     oP"``"Yo,        888o    888   888   ,8P"`       888    Y88 888oo,__
            MMM  M'  "MMMYMM   \"\"`,m"       "Mm,      YMMMb   YMM   \"\"`mM"          MMM     YM """"YUMMM

    ______________________________________________________________________________________________________________

    - Created by: mateusware
    - Module: server
    - Description:
        This is the server of the maxpayne-mp project.

*/

#include <cstdio>
#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "winmm.lib")

#include <enet/enet.h>
#include <conio.h>

int main() {
    printf("                      maxpayne-mp\n");
    printf("                  server initialized\n");
    printf("--------------------------------------------------------\n");
    printf("         Server running! Press ENTER to stop.\n");
    getchar();
    return 0;
}