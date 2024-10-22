// SPDX-License-Identifier: GPL-3.0-or-later

#include <cstdint>

#include <string>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <vector>

#include "json.hpp"

#include "chset.h"
#include "config.h"
#include "convert.h"

namespace px4tsid
{

std::string Convert::dump(const std::string& format, const nlohmann::json& json)
{
	if (format == "json")
	{
		return json.dump(4);
	}
	else if (format == "dvbv5")
	{
		return libdvbv5(json);
	}
	else if (format == "bondvb")
	{
		return bondriver_dvb(json);
	}
	else if (format == "bonpt")
	{
		return bondriver_pt(json);
	}
	else if (format == "bonptx")
	{
		return bondriver_ptx(json);
	}
	else if (format == "bonpx4")
	{
		return bondriver_px4(json);
	}

	throw std::runtime_error("failed to dump invalid format");
}

std::string Convert::libdvbv5(const nlohmann::json& json)
{
	std::ostringstream os;

	if (json.at("BS").size() > 0)
	{
		for (const ChSet& c : json.at("BS"))
		{
			if (!c.has_lock()) continue;
			for (size_t tsnum = 0; tsnum < c.transport_stream_id().size(); tsnum++)
			{
				auto tsid = c.transport_stream_id(tsnum);
				if (tsid == 0xffff) continue;
				os << "[BS" << std::setw(2) << std::setfill('0') << c.number() << "_" << tsnum << "]\n"
					<< '\t' << "DELIVERY_SYSTEM = ISDBS\n"
					<< '\t' << "FREQUENCY = " << c.frequency_if_khz() << '\n'
					<< '\t' << "STREAM_ID = " << tsid << '\n';
			}
		}
	}

	if (json.at("CS").size() > 0)
	{
		for (const ChSet& c : json.at("CS"))
		{
			if (!c.has_lock()) continue;
			auto tsnum = 0;
			auto tsid = c.transport_stream_id(tsnum);
			if (tsid == 0xffff) continue;
			os << "[CS" << c.number() << "]\n"
				<< '\t' << "DELIVERY_SYSTEM = ISDBS\n"
				<< '\t' << "FREQUENCY = " << c.frequency_if_khz() << '\n'
				<< '\t' << "STREAM_ID = " << tsid << '\n';
		}
	}

	return os.str();
}

std::string Convert::bondriver_pt(const nlohmann::json& json)
{
	auto bonch = 0;
	std::ostringstream os;

	os << "#ISDB_S\n";

	if (json.at("BS").size() > 0)
	{
		os << "; BS\n";
		for (const ChSet& c : json.at("BS"))
		{
			if (!c.has_lock()) continue;
			for (size_t tsnum = 0; tsnum < c.transport_stream_id().size(); tsnum++)
			{
				auto tsid = c.transport_stream_id(tsnum);
				if (tsid == 0xffff) continue;
				os << "BS" << std::setw(2) << std::setfill('0') << c.number() << "/TS" << tsnum
					<< '\t' << bonch
					<< '\t' << c.frequency_idx()
					<< '\t' << tsnum << '\n';
				bonch++;
			}
		}
	}

	if (json.at("CS").size() > 0)
	{
		auto tsnum = 0;
		os << "\n; CS110\n";
		for (const ChSet& c : json.at("CS"))
		{
			if (!c.has_lock()) continue;
			auto tsid = c.transport_stream_id(tsnum);
			if (tsid == 0xffff) continue;
			os << "ND" << std::setw(2) << std::setfill('0') << c.number()
				<< '\t' << bonch
				<< '\t' << c.frequency_idx()
				<< '\t' << tsnum << '\n';
			bonch++;
		}
	}

	return os.str();
}

std::string Convert::bondriver_dvb(const nlohmann::json& json)
{
	auto bonch = 0;
	auto is_start_cs = false;
	std::ostringstream os;

	os << "#ISDB_S\n";

	if (json.at("BS").size() > 0)
	{
		os << "; BS\n";
		for (const ChSet& c : json.at("BS"))
		{
			if (!c.has_lock()) continue;
			for (size_t tsnum = 0; tsnum < c.transport_stream_id().size(); tsnum++)
			{
				auto tsid = c.transport_stream_id(tsnum);
				if (tsid == 0xffff) continue;
				os << "BS" << std::setw(2) << std::setfill('0') << c.number() << "/TS" << tsnum
					<< '\t' << bonch
					<< '\t' << c.frequency_idx()
					<< '\t' << "0x" << std::hex << tsid << std::dec << '\n';
				bonch++;
			}
		}
	}

	if (json.at("CS").size() > 0)
	{
		auto tsnum = 0;
		os << "\n; CS110\n";
		for (const ChSet& c : json.at("CS"))
		{
			if (!c.has_lock()) continue;
			auto tsid = c.transport_stream_id(tsnum);
			if (tsid == 0xffff) continue;
			os << "ND" << std::setw(2) << std::setfill('0') << c.number()
				<< '\t' << bonch
				<< '\t' << c.frequency_idx()
				<< '\t' << "0x" << std::hex << tsid << std::dec << '\n';
			bonch++;
		}
	}

	return os.str();
}

std::string Convert::bondriver_ptx(const nlohmann::json& json)
{
	auto space = 0;
	auto bonch = 0;
	std::ostringstream os;

	if (json.at("BS").size() > 0)
	{
		os << "[Space.BS]\n" << "Name=BS\n" << "System=ISDB-S\n\n" << "[Space.BS.Channel]\n";
		for (const ChSet& c : json.at("BS"))
		{
			if (!c.has_lock()) continue;
			for (size_t tsnum = 0; tsnum < c.transport_stream_id().size(); tsnum++)
			{
				auto tsid = c.transport_stream_id(tsnum);
				if (tsid == 0xffff) continue;
				os << "Ch" << bonch << '='
					<< "BS" << std::setw(2) << std::setfill('0') << c.number() << "/TS" << tsnum
					<< ',' << c.frequency_idx()
					<< ',' << tsnum << '\n';
				bonch++;
			}
		}
	}

	if (json.at("CS").size() > 0)
	{
		auto tsnum = 0;
		bonch = 0;
		os << "\n[Space.CS110]\n" << "Name=CS110\n" << "System=ISDB-S\n\n" << "[Space.CS110.Channel]\n";
		for (const ChSet& c : json.at("CS"))
		{
			if (!c.has_lock()) continue;
			auto tsid = c.transport_stream_id(tsnum);
			if (tsid == 0xffff) continue;
			os << "Ch" << bonch << '='
				<< "ND" << std::setw(2) << std::setfill('0') << c.number() << "/TS0"
				<< ',' << c.frequency_idx()
				<< ',' << tsnum << '\n';
			bonch++;
		}
	}

	return os.str();
}

std::string Convert::bondriver_px4(const nlohmann::json& json)
{
	auto space = 0;
	auto bonch = 0;
	std::ostringstream os;

	if (json.at("BS").size() > 0)
	{
		os << "; [BS]\n";
		for (const ChSet& c : json.at("BS"))
		{
			if (!c.has_lock()) continue;
			for (size_t tsnum = 0; tsnum < c.transport_stream_id().size(); tsnum++)
			{
				auto tsid = c.transport_stream_id(tsnum);
				if (tsid == 0xffff) continue;
				os << "BS" << std::setw(2) << std::setfill('0') << c.number() << "/TS" << tsnum
					<< '\t' << space
					<< '\t' << bonch
					<< '\t' << c.frequency_idx()
					<< '\t' << tsid << '\n';
				bonch++;
			}
		}
	}

	if (json.at("CS").size() > 0)
	{
		auto tsnum = 0;
		space = 1;
		bonch = 0;
		os << "; [CS]\n";
		for (const ChSet& c : json.at("CS"))
		{
			if (!c.has_lock()) continue;
			auto tsid = c.transport_stream_id(tsnum);
			if (tsid == 0xffff) continue;
			os << "ND" << std::setw(2) << std::setfill('0') << c.number()
				<< '\t' << space
				<< '\t' << bonch
				<< '\t' << c.frequency_idx()
				<< '\t' << tsid << '\n';
			bonch++;
		}
	}

	return os.str();
}

}