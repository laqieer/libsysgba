
#include <gba_console.h>
#include <gba_video.h>
#include <gba_interrupt.h>
#include <gba_systemcalls.h>
#include <gba_sys.h>

#include <fstream>
#include <iostream>
#include <sstream>

using namespace std;

#define BUFFER_SIZE 100

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

    char buf[BUFFER_SIZE] = {0};
    stringstream buffer;
    int filesize = 0;

    cout << "test.txt:" << endl;

    //ifstream in("test.txt");
    ifstream in;
    in.open("test.txt", ios::in|ios::binary);

    buffer << in.rdbuf();
    cout << buffer.str();

    in.seekg(0, ios::beg);

    in >> buf;
    cout << buf << endl;

    in.seekg(0, ios::end);
    try{
        filesize = in.tellg();
        //throw invalid_argument("throw test exception");
    } catch(exception const &e) {
        cout << e.what() <<endl;
    }
    cout << "size: " << filesize << endl;
    in.seekg(0, ios::beg);

    in.read(buf, 10);
    if (in.bad())
    {
        cerr << "Read/writing error on i/o operation";
    }
    if (in.fail())
    {
        cerr << "Logical error on i/o operation";
    }
    if (in.eof())
    {
        cout << "End-of-File reached on input operation" << endl;
    }
    if (in.good())
    {
        cout << "No errors" << endl;
    }
    if (in)
    {
        cout << buf << endl;
    }
    cout << "Read " << in.gcount() << " chars" << endl;

    cout << "tellg: " << in.tellg() << endl;

    in.seekg(0, ios::beg);
    in.getline(buf, BUFFER_SIZE);
    if (in.bad())
    {
        cerr << "Read/writing error on i/o operation";
    }
    if (in.fail())
    {
        cerr << "Logical error on i/o operation";
    }
    if (in.eof())
    {
        cout << "End-of-File reached on input operation" << endl;
    }
    if (in.good())
    {
        cout << "No errors" << endl;
    }
    cout << buf;

    in.close();

	while (1) {
		VBlankIntrWait();
	}
}


