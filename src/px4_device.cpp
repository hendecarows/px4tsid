// SPDX-License-Identifier: GPL-3.0-or-later

#include <fcntl.h>
#include <sys/ioctl.h>
#include <unistd.h>

#include <cstdint>
#include <stdexcept>
#include <string>
#include <sstream>

#include "ptx_ioctl.h"
#include "px4_device.h"

namespace px4tsid
{

void PX4Device::open_tuner(const std::string& device)
{
	if (fd_ != -1)
	{
		close_tuner();
	}

	fd_ = ::open(device.c_str(), O_RDONLY);
	if (fd_ == -1)
	{
		device_.clear();
		std::ostringstream os;
		os << "failed to open tuner " << device;
		throw std::runtime_error(os.str());
	}

	device_ = device;
}

void PX4Device::close_tuner()
{
	if (fd_ == -1) { return; }

	stop_streaming();

	if (lnb_power_state_)
	{
		::ioctl(fd_, PTX_DISABLE_LNB_POWER);
		lnb_power_state_ = false;
	}

	::close(fd_);
	fd_ = -1;
}

void PX4Device::set_channel_s(int32_t freq_num, int32_t slot_num)
{
	if (fd_ == -1)
	{
		throw std::runtime_error("no open device");
	}

	if (::ioctl(fd_, PTX_SET_SYSTEM_MODE, ptx_system_type::PTX_ISDB_S_SYSTEM) == -1)
	{
		throw std::runtime_error("failed to ioctl(PTX_SET_SYSTEM_MODE)");
	}

	if (lnb_power_ && !lnb_power_state_)
	{
		if (::ioctl(fd_, PTX_ENABLE_LNB_POWER, 2) == -1)
		{
			throw std::runtime_error("failed to ioctl(PTX_ENABLE_LNB_POWER)");
		}
		lnb_power_state_ = true;
	}

	::ptx_freq freq = { freq_num, slot_num };
	if (::ioctl(fd_, PTX_SET_CHANNEL, &freq) == -1)
	{
		std::ostringstream os;
		os << "failed to ioctl(PTX_SET_CHANNEL) freq: " << freq_num << " slot: " << slot_num;
		throw std::runtime_error(os.str());
	}
}

void PX4Device::start_streaming()
{
	if (fd_ == -1)
	{
		throw std::runtime_error("no open device");
	}

	if (!has_streaimng_)
	{
		if (::ioctl(fd_, PTX_START_STREAMING) == -1)
		{
			throw std::runtime_error("failed to ioctl(PTX_START_STREAMING)");
		}
		has_streaimng_ = true;
	}
}

void PX4Device::stop_streaming()
{
	if (fd_ == -1)
	{
		throw std::runtime_error("no open device");
	}

	if (has_streaimng_)
	{
		::ioctl(fd_, PTX_STOP_STREAMING);
		has_streaimng_ = false;
	}
}

ssize_t PX4Device::read_stream(uint8_t* buf, size_t size)
{
	if (fd_ == -1)
	{
		throw std::runtime_error("no open device");
	}

	if (has_streaimng_)
	{
		return ::read(fd_, buf, size);
	}

	return -ENODATA;
}

}