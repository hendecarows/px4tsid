// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <cstdint>
#include <string>

#include "ptx_ioctl.h"

namespace px4tsid
{

class PX4Device
{
public:
	PX4Device() = default;
	~PX4Device() { close_tuner(); }

	void set_lnb_power(bool is_enable) { lnb_power_ = is_enable; }
	bool has_straming() const { return has_streaimng_; }
	void open_tuner(const std::string& device);
	void close_tuner();
	void set_channel_s(int32_t freq_num, int32_t slot_num);
	void start_streaming();
	void stop_streaming();
	ssize_t read_stream(uint8_t* buf, size_t size);

private:
	std::string device_;
	int32_t fd_ = -1;
	bool lnb_power_ = false;
	bool lnb_power_state_ = false;
	bool has_streaimng_ = false;
};

}