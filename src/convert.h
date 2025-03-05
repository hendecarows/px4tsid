// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <cstdint>

#include <string>
#include <vector>

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
	static std::string libdvbv5(const nlohmann::json& json);
	static std::string libdvbv5lnb(const nlohmann::json& json);
	static std::string bondriver_pt(const nlohmann::json& json);
	static std::string bondriver_dvb(const nlohmann::json& json);
	static std::string bondriver_ptx(const nlohmann::json& json);
	static std::string bondriver_px4(const nlohmann::json& json);
};

}