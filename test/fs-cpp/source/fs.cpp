
#include <gba_console.h>
#include <gba_video.h>
#include <gba_interrupt.h>
#include <gba_systemcalls.h>
#include <gba_sys.h>

#include <fstream>
#include <iostream>
#include <sstream>

using namespace std;

//---------------------------------------------------------------------------------
// Program entry point
//---------------------------------------------------------------------------------
int main(void) {
//---------------------------------------------------------------------------------


	// the vblank interrupt must be enabled for VBlankIntrWait() to work
	// since the default dispatcher handles the bios flags no vblank handler
	// is required
	irqInit();
	irqEnable(IRQ_VBLANK);

	consoleDemoInit();

    fsInit();

    cout << "test.txt:" << endl;

    ifstream in("test.txt");
    stringstream buffer;
    buffer << in.rdbuf();

    cout << buffer.str();

	while (1) {
		VBlankIntrWait();
	}
}


