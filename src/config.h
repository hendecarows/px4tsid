// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <cstdint>
#include <string>
#include <unordered_set>

namespace px4tsid
{

class Config
{
public:
	Config() = default;
	~Config() = default;

	const std::string& format() const { return format_; }
	const std::string& error() const { return error_; }
	const std::string& device() const { return device_; }
	bool lnb_power() const { return lnb_power_; }
	bool is_ignore_tsid(uint16_t tsid) const { return ignore_tsids_.count(tsid) ? true : false;}
	int32_t transponder_size_bs() const { return TRANSPONDER_SIZE_BS; }
	int32_t transponder_size_cs() const { return TRANSPONDER_SIZE_CS; }
	int32_t buffer_size() const { return BUFFER_SIZE; }
	int32_t ts_number_size() const { return ts_number_size_; }
	int32_t retry_count() const { return retry_count_; }
	void parse(int argc, char* argv[]);

private:
	static constexpr int32_t TRANSPONDER_SIZE_BS = 12;
	static constexpr int32_t TRANSPONDER_SIZE_CS = 12;
	static constexpr int32_t BUFFER_SIZE = 188*1024;

	std::string format_ = "json";
	std::string error_;
	std::string device_;
	bool lnb_power_ = false;
	int32_t ts_number_size_ = 4;
	int32_t retry_count_ = 5;
	std::unordered_set<uint16_t> ignore_tsids_;

	std::string usage(const std::string& argv0, const std::string& msg = "") const;
};

}