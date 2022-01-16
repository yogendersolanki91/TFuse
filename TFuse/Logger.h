/*
 ***************************************************************************** 
 * Author: Yogender Solanki <yogendersolanki91@gmail.com> 
 *
 * Copyright (c) 2011 Yogender Solanki
 *
 * This program is free software: you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License as published by the
 * Free Software Foundation, either version 3 of the License, or (at your
 * option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License
 * for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *****************************************************************************
 */
#pragma once

#include <boost/log/core.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/log/utility/setup/console.hpp>
#include <boost/log/utility/setup/file.hpp>
#include <iostream>

#define LOG_TRACE BOOST_LOG_TRIVIAL(trace) << __FUNCTION__ << " "
#define LOG_DEBUG BOOST_LOG_TRIVIAL(debug) << __FUNCTION__ << " "
#define LOG_INFO BOOST_LOG_TRIVIAL(info) << __FUNCTION__ << " "
#define LOG_WARNING BOOST_LOG_TRIVIAL(warning) << __FUNCTION__ << " "
#define LOG_ERROR BOOST_LOG_TRIVIAL(error) << __FUNCTION__ << " "
#define LOG_FATAL BOOST_LOG_TRIVIAL(fatal) << __FUNCTION__ << " "

namespace logging = boost::log;

static bool InitDone = false;

static void init_logging()
{
    logging::core::get()->set_filter(logging::trivial::severity >= logging::trivial::info);
    boost::log::add_console_log(
        std::cout,
        boost::log::keywords::format = "[%TimeStamp%] [%ThreadID%] [%Severity%] %Message%");
    logging::add_common_attributes();
    InitDone = true;
}
