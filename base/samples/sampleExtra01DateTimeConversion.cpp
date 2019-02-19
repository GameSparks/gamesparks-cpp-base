//#include <iostream>
#include <GameSparks/GSDateTime.h>


int main(int argc, const char* argv[])
{
    (void)(argc);
    (void)(argv);
	using namespace GameSparks::Core;

	GSDateTime now = GSDateTime::Now();
	GSDateTime nowLocal = now.ToLocalTime();
	GSDateTime nowGM = now.ToGMTime();

	gsstl::cout << "now: " << now.ToString() << "hour: " << gsstl::endl;
	gsstl::cout << "nowLocal: " << nowLocal.ToString() << gsstl::endl;
	gsstl::cout << "nowGM: " << nowGM.ToString() << gsstl::endl;

	gsstl::cout << "conversion: " << GSDateTime(now.ToString()).ToString() << gsstl::endl;

	gsstl::cout << "try '2014-12-15T17:09:43.480Z'" << gsstl::endl;
	gsstl::cout << "ToString: " << GSDateTime("2014-12-15T17:09:43.480Z").ToString() << gsstl::endl;
	gsstl::cout << "ToLocalTime().ToString: " << GSDateTime("2014-12-15T17:09:43.480Z").ToLocalTime().ToString() << gsstl::endl;
	gsstl::cout << "ToGMTime().ToString(): " << GSDateTime("2014-12-15T17:09:43.480Z").ToGMTime().ToString() << gsstl::endl;

	GSDateTime now2 = now;

	gsstl::cout << now2.ToString() << gsstl::endl;

	return 0;
}


