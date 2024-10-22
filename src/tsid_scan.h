// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <cstdint>
#include <csignal>
#include <vector>

#include "json.hpp"

#include "chset.h"
#include "config.h"
#include "px4_device.h"

namespace px4tsid
{

class TSIDScan
{
public:
	TSIDScan() = default;
	~TSIDScan() = default;

	static void signal_handler(int signum) { TSIDScan::has_stop_ = 1; }
	void init(int argc, char* argv[]);
	void scan();
	nlohmann::json json() const;
	std::string format() const { return config_.format(); }

private:
	static volatile std::sig_atomic_t has_stop_;
	Config config_;
	PX4Device px4_device_;

	std::vector<uint8_t> rest_buf_;
	std::vector<ChSet> chsets_bs_;
	std::vector<ChSet> chsets_cs_;

	void scan_tsid_bs();
	void scan_tsid_cs();
	int32_t get_transport_stream_id(const uint8_t* buf, size_t size, uint16_t& tsid);
};

}