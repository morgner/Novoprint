#include <iostream>
#include <fstream>

int main(int argc, char* argv[])
{
	//open printer device file (linux)
        std::ofstream printer;
	printer.open( "/dev/ttyACM0");

	//write to it
        printer << "Hello from C++!";
	printer.close();

	return 0;
}
