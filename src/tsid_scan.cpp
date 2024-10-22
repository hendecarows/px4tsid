// SPDX-License-Identifier: GPL-3.0-or-later

#include <unistd.h>
#include <signal.h>

#include <cstdint>
#include <cstring>
#include <csignal>
#include <iostream>
#include <iomanip>
#include <string>
#include <thread>
#include <vector>

#include "json.hpp"

#include "chset.h"
#include "config.h"
#include "px4_device.h"
#include "tsid_scan.h"

namespace px4tsid
{

volatile std::sig_atomic_t TSIDScan::has_stop_ = 0;

void TSIDScan::init(int argc, char* argv[])
{
	config_.parse(argc, argv);
	struct ::sigaction sa;
	std::memset(&sa, 0, sizeof(sa));
	sa.sa_handler = TSIDScan::signal_handler;
	sa.sa_flags = SA_RESTART;
	sigemptyset(&sa.sa_mask);
	sigaction(SIGINT, &sa, nullptr);
	sigaction(SIGTERM, &sa, nullptr);
	sigaction(SIGPIPE, &sa, nullptr);
}

void TSIDScan::scan()
{
	chsets_bs_.clear();
	chsets_cs_.clear();
	px4_device_.open_tuner(config_.device());
	scan_tsid_bs();
	scan_tsid_cs();
	px4_device_.close_tuner();
}

nlohmann::json TSIDScan::json() const
{
	auto j = nlohmann::json::object();
	j["BS"] = nlohmann::json::array();
	j["CS"] = nlohmann::json::array();

	for (const auto& p : chsets_bs_)
	{
		j.at("BS").emplace_back(p);
	}

	for (const auto& p : chsets_cs_)
	{
		j.at("CS").emplace_back(p);
	}

	return j;
}

void TSIDScan::scan_tsid_bs()
{
	using namespace std::chrono_literals;
	std::vector<uint8_t> buf(config_.buffer_size());

	chsets_bs_.resize(config_.transponder_size_bs());
	for (auto idx = 0; idx < chsets_bs_.size(); idx++)
	{
		if (TSIDScan::has_stop_) { break; }
		auto& chset = chsets_bs_.at(idx);
		auto tpnum = idx * 2 + 1;
		auto fqidx = idx;
		uint32_t freq = 11727480 + 38360 * idx;
		std::ostringstream os1;
		os1 << "BS" << tpnum;
		chset.set_transponder(os1.str());
		chset.set_number(tpnum);
		chset.set_frequency_idx(fqidx);
		chset.set_frequency_khz(freq);

		try
		{
			for (auto tsnum = 0; tsnum < config_.ts_number_size(); tsnum++)
			{
				if (TSIDScan::has_stop_) { break; }
				rest_buf_.clear();
				std::cerr << "BS" << std::setw(2) << std::setfill('0') << tpnum << "/TS" << tsnum
					<< " : Frequency = " << chset.frequency_khz()
					<< '(' << chset.frequency_if_khz() << ')';

				px4_device_.set_channel_s(fqidx, tsnum);
				px4_device_.start_streaming();
				chset.has_lock(true);
				std::cerr << " : locked";

				for (auto retry = 0; retry < config_.retry_count(); retry++)
				{
					if (TSIDScan::has_stop_) { break; }
					auto size = px4_device_.read_stream(buf.data(), buf.size());
					if (size <= 0)
					{
						std::this_thread::sleep_for(100ms);
						continue;
					}
					uint16_t tsid = 0xffff;
					get_transport_stream_id(buf.data(), size, tsid);
					if (tsid != 0xffff && !config_.is_ignore_tsid(tsid))
					{
						chset.set_transport_stream_id(tsnum, tsid);
						std::cerr << " : TSID = " << tsid;
						break;
					}
				}
				std::cerr << '\n';
				px4_device_.stop_streaming();
			}
		}
		catch(const std::exception& e)
		{
			std::cerr << " : " << e.what() << '\n';
			px4_device_.stop_streaming();
		}
	}

	px4_device_.stop_streaming();
	if (TSIDScan::has_stop_)
	{
		throw std::runtime_error("catch signal");
	}
}

void TSIDScan::scan_tsid_cs()
{
	using namespace std::chrono_literals;
	std::vector<uint8_t> buf(config_.buffer_size());

	chsets_cs_.resize(config_.transponder_size_bs());
	for (size_t idx = 0; idx < chsets_cs_.size(); idx++)
	{
		if (TSIDScan::has_stop_) { break; }
		auto& chset = chsets_cs_.at(idx);
		auto tpnum = (idx + 1) * 2;
		auto fqidx = idx + config_.transponder_size_bs();
		uint32_t freq = 12291000 + 40000 * idx;
		std::ostringstream os1;
		os1 << "ND" << tpnum;
		chset.set_transponder(os1.str());
		chset.set_number(tpnum);
		chset.set_frequency_idx(fqidx);
		chset.set_frequency_khz(freq);

		auto tsnum = 0;
		rest_buf_.clear();
		std::cerr << "ND" << std::setw(2) << std::setfill('0') << tpnum
			<< " : Frequency = " << chset.frequency_khz()
			<< '(' << chset.frequency_if_khz() << ')';
		try
		{
			px4_device_.set_channel_s(fqidx, tsnum);
			px4_device_.start_streaming();
			chset.has_lock(true);
			std::cerr << " : locked";

			for (auto retry = 0; retry < config_.retry_count(); retry++)
			{
				if (TSIDScan::has_stop_) { break; }
				auto size = px4_device_.read_stream(buf.data(), buf.size());
				if (size <= 0)
				{
					std::this_thread::sleep_for(100ms);
					continue;
				}
				uint16_t tsid = 0xffff;
				get_transport_stream_id(buf.data(), size, tsid);
				if (tsid != 0xffff && !config_.is_ignore_tsid(tsid))
				{
					chset.set_transport_stream_id(tsnum, tsid);
					std::cerr << " : TSID = " << tsid;
					break;
				}
			}
			std::cerr << '\n';
			px4_device_.stop_streaming();
		}
		catch(const std::exception& e)
		{
			std::cerr << " : " << e.what() << '\n';
			px4_device_.stop_streaming();
		}
	}

	px4_device_.stop_streaming();
	if (TSIDScan::has_stop_)
	{
		throw std::runtime_error("catch signal");
	}
}

int32_t TSIDScan::get_transport_stream_id(const uint8_t* buf, size_t size, uint16_t& tsid)
{
	int32_t error_counter = 0;
	tsid = 0xffff;

	auto rest_size = rest_buf_.size();
	rest_buf_.resize(rest_size + size);
	std::memcpy(rest_buf_.data() + rest_size, buf, size);

	auto p = rest_buf_.data();
	auto tail = p + rest_buf_.size();

	while (p < tail - 188)
	{
		if (!((p[0] == 0x47) && (p[188] == 0x47)))
		{
			p++;
			continue;
		}

		bool transport_error_indicator = (p[1] & 0x80) ? true : false;
		bool payload_start_indicator = (p[1] & 0x40) ? true : false;
		uint16_t pid = ((p[1] & 0x1f) << 8) | p[2];
		bool adaptation_field_control = (p[3] & 0x20) ? true : false;
		uint8_t pointer_field = p[4];
		if (transport_error_indicator)
		{
			error_counter++;
		}
		else if (payload_start_indicator && pid == 0 && !adaptation_field_control && pointer_field == 0)
		{
			tsid = (p[8] << 8) | p[9];
		}
		p += 188;
	}

	rest_size = tail - p;
	if (rest_size > 0)
	{
		std::memmove(rest_buf_.data(), p, rest_size);
		rest_buf_.resize(rest_size);
	}

	return error_counter;
}

}