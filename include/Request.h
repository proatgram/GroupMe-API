/*
 * Request.h
 *
 *  Created on: Jun 9, 2022
 *      Author: thetimbrick
 */

#ifndef INCLUDE_REQUEST_H_
#define INCLUDE_REQUEST_H_

#include <string>
#include <cstdlib>
#include <sstream>
#include <curl/curl.h>
#include <curl/easy.h>
#include <utility>
#include <unistd.h>
#include <cstdio>
#include <indicators/progress_bar.hpp>
#include <indicators/cursor_control.hpp>
#ifdef __unix__
#include <glib-2.0/glib.h>
#endif
#include <Utils.hpp>
#include <SafeData.hpp>

struct Request {
public:

	Request() :
		m_baseURL("\0"),
		m_header(nullptr),
		m_progress()

	{
		indicators::show_console_cursor(false);
		m_progress.set_option(indicators::option::BarWidth{50});
		m_progress.set_option(indicators::option::Start{"["});
		m_progress.set_option(indicators::option::Fill{"-"});
		m_progress.set_option(indicators::option::Lead{">"});
		m_progress.set_option(indicators::option::Remainder{" "});
		m_progress.set_option(indicators::option::End{"]"});
		m_progress.set_option(indicators::option::ShowPercentage{true});
		m_progress.set_option(indicators::option::ForegroundColor{indicators::Color::green});
		m_progress.set_option(indicators::option::FontStyles{std::vector<indicators::FontStyle>{indicators::FontStyle::bold}});
	}
	~Request() {
		free(m_header);
		m_header = nullptr;
	}

	int download(std::string url, std::string output);

	int download(std::string output);

	int download();

	std::stringstream sendGET(std::string endpoint);

	void setBase(std::string baseURL);

	void setHeader(std::pair<std::string, std::string> keys);

	void clearHeader();

private:

	std::string m_baseURL;

	struct curl_slist* m_header;

protected:
	indicators::ProgressBar m_progress;
};



#endif /* INCLUDE_REQUEST_H_ */
