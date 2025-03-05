// SPDX-License-Identifier: GPL-3.0-or-later

#include <getopt.h>

#include <cstdint>
#include <algorithm>
#include <string>
#include <stdexcept>
#include <sstream>
#include <unordered_set>

#include "config.h"

namespace px4tsid
{

void Config::parse(int argc, char* argv[])
{
	const option long_options[] = {
		{"help", no_argument, 0, 'h'},
		{"lnb", no_argument, 0, 'l'},
		{"format", required_argument, 0, 'f'},
		{"ignore", required_argument, 0, 'i'},
		{"ts-number-size", required_argument, 0, 't'},
		{"retry-times", required_argument, 0, 'r'},
		{0,0,0,0},
	};
	const std::unordered_set<std::string> formats{
		"json",
		"dvbv5",
		"dvbv5lnb",
		"bondvb",
		"bonpt",
		"bonptx",
		"bonpx4",
	};

	while(true)
	{
		auto option_index = 0;
		auto c = getopt_long(argc, argv, "hlf:i:t:r:", long_options, &option_index);
		if (c == -1) { break; }

		switch (c)
		{
		case 'l':
		{
			lnb_power_ = true;
			break;
		}
		case 'f':
		{
			if (formats.count(optarg) == 0)
			{
				error_ = usage(argv[0], "unknown format");
				throw std::runtime_error(error_);
			}

			format_ = optarg;
			break;
		}
		case 'i':
		{
			uint16_t tsid;
			std::string arg = optarg;
			std::replace(arg.begin(), arg.end(), ',', ' ');
			std::istringstream ss(arg);
			while (ss >> tsid)
			{
				ignore_tsids_.emplace(tsid);
			}
			break;
		}
		case 't':
		{
			auto n = std::atoi(optarg);
			if (n < 1)
			{
				ts_number_size_ = 1;
			}
			else if (n > 8)
			{
				ts_number_size_ = 8;
			}
			break;
		}
		case 'r':
		{
			auto n = std::atoi(optarg);
			if (n < 1)
			{
				retry_count_ = 1;
			}
			else
			{
				retry_count_ = n;
			}
			break;
		}
		case 'h':
		default:
			error_ = usage(argv[0]);
			throw std::runtime_error(error_);
		}
	}

	argc -= optind;
	if (argc != 1)
	{
		error_ = usage(argv[0], "invalid number of arguments");
		throw std::runtime_error(error_);
	}
	device_ = argv[optind];
}

std::string Config::usage(const std::string& argv0, const std::string& msg) const
{
	std::ostringstream os;

	os << "\n"
		<< "usage: " << argv0
		<< " [options] DEVICE\n"
		<< "\n"
		<< "options:\n"
		<< "  --help                     show this help message\n"
		<< "  --lnb                      enable LNB power\n"
		<< "  --format=str               chset format str={json,dvbv5,dvbv5lnb,bondvb,bonpt,bonptx,bonpx4}\n"
		<< "  --ignore=TSID0,TSID1,...   ignore TSIDs\n"
		<< "  --ts-number-size=n         scan from 0 to n realtive TS number (4) (TS0,TS1,TS2,TS3)\n"
		<< "  --retry-times=n            retry times scan PAT (5)\n"
		<< "  DEVICE                     px4_drv device file\n";

	if (!msg.empty())
	{
		os << "\n"
			<< msg << "\n";
	}

	return os.str();
}

}