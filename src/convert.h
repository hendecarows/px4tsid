// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <functional>
#include <string>
#include <unordered_map>

#include "json.hpp"

#include "chset.h"
#include "config.h"

namespace px4tsid
{

class Convert
{
public:
	Convert() = delete;
	~Convert() = delete;

	static std::string dump(const std::string& format, const nlohmann::json& json);

private:
	static std::string json(const nlohmann::json& json);
	static std::string libdvbv5(const nlohmann::json& json);
	static std::string libdvbv5lnb(const nlohmann::json& json);
	static std::string mirakurun(const nlohmann::json& json);
	static std::string libdvbv5_tsid(const nlohmann::json& json);
	static std::string libdvbv5lnb_tsid(const nlohmann::json& json);
	static std::string mirakurun_tsid(const nlohmann::json& json);
	static std::string bondriver_pt(const nlohmann::json& json);
	static std::string bondriver_dvb(const nlohmann::json& json);
	static std::string bondriver_ptx(const nlohmann::json& json);
	static std::string bondriver_px4(const nlohmann::json& json);

	static const inline std::unordered_map<std::string, std::function<std::string(const nlohmann::json&)>> convert_
	{
		{"json", Convert::json},
		{"dvbv5", Convert::libdvbv5},
		{"dvbv5lnb", Convert::libdvbv5lnb},
		{"mirakurun", Convert::mirakurun},
		{"dvbv5tsid", Convert::libdvbv5_tsid},
		{"dvbv5lnbtsid", Convert::libdvbv5lnb_tsid},
		{"mirakuruntsid", Convert::mirakurun_tsid},
		{"bondvb", Convert::bondriver_dvb},
		{"bonpt", Convert::bondriver_pt},
		{"bonptx", Convert::bondriver_ptx},
		{"bonpx4", Convert::bondriver_px4},
	};

};

}