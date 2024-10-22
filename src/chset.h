// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <cstdint>

#include <string>
#include <vector>

#include "json.hpp"
#include "config.h"

namespace px4tsid
{

class ChSet
{
public:
	ChSet() :
		number_(0),
		frequency_idx_(0),
		frequency_khz_(0),
		frequency_if_khz_(0),
		transport_stream_id_(8, 0xffff)
	{}
	~ChSet() = default;

	const std::string& transponder() const { return transponder_; }
	int32_t number() const { return number_; }
	int32_t frequency_idx() const { return frequency_idx_; }
	uint32_t frequency_khz() const { return frequency_khz_; }
	uint32_t frequency_if_khz() const { return frequency_if_khz_; }
	bool has_lock() const { return has_lock_; }
	const std::vector<uint16_t>& transport_stream_id() const { return transport_stream_id_; }
	uint16_t transport_stream_id(size_t slot) const { return transport_stream_id_.at(slot); }

	void set_transponder(const std::string& transponder) { transponder_ = transponder; }
	void set_number(int32_t number) { number_ = number; }
	void set_frequency_idx(int32_t idx) { frequency_idx_ = idx; }
	void set_frequency_khz(uint32_t freq)
	{
		frequency_khz_ = freq;
		frequency_if_khz_ = freq - 10678000;
	}
	void has_lock(bool is_enable) { has_lock_ = is_enable; }
	void set_transport_stream_id(int32_t slot, uint16_t tsid)
	{
		if (transport_stream_id_.size() == 0) {
			transport_stream_id_.resize(8, 0xffff);
		}
		auto result = std::find(transport_stream_id_.begin(), transport_stream_id_.end(), tsid);
		if (result == transport_stream_id_.end()) {
			transport_stream_id_.at(slot) = tsid;
		}
	}
	void set_transport_stream_ids(const std::vector<uint16_t> tsids)
	{
		transport_stream_id_ = tsids;
	}

private:
	std::string transponder_;
	int32_t number_ = 0;
	int32_t frequency_idx_ = 0;
	uint32_t frequency_khz_ = 0;
	uint32_t frequency_if_khz_ = 0;
	bool has_lock_ = false;
	std::vector<uint16_t> transport_stream_id_;
};

void to_json(nlohmann::json& j, const ChSet& p);
void from_json(const nlohmann::json& j, ChSet& p);

}