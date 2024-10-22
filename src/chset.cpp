// SPDX-License-Identifier: GPL-3.0-or-later

#include "json.hpp"
#include "chset.h"
#include "config.h"

namespace px4tsid
{

void to_json(nlohmann::json& j, const ChSet& p)
{
	j = nlohmann::json{
		{"transponder", p.transponder()},
		{"number", p.number()},
		{"frequency_idx", p.frequency_idx()},
		{"frequency_khz", p.frequency_khz()},
		{"frequency_if_khz", p.frequency_if_khz()},
		{"has_lock", p.has_lock()},
		{"transport_stream_id", p.transport_stream_id()},
	};
}

void from_json(const nlohmann::json& j, ChSet& p)
{
	p.set_transponder(j.at("transponder"));
	p.set_number(j.at("number"));
	p.set_frequency_idx(j.at("frequency_idx"));
	p.set_frequency_khz(j.at("frequency_khz"));
	p.has_lock(j.at("has_lock"));
	p.set_transport_stream_ids(j.at("transport_stream_id"));
}

}