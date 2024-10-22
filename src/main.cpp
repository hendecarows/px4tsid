// SPDX-License-Identifier: GPL-3.0-or-later

#include <cstdint>
#include <exception>
#include <iostream>

#include "tsid_scan.h"
#include "convert.h"

int main(int argc, char** argv)
{
	try
	{
		px4tsid::TSIDScan scan;
		scan.init(argc, argv);
		scan.scan();

		std::cout << px4tsid::Convert::dump(scan.format(), scan.json());
	}
	catch (const std::exception& ex)
	{
		std::cerr << ex.what() << '\n';
		return 1;
	}

	return 0;
}
