/*
 * Request.cpp
 *
 *  Created on: Jun 9, 2022
 *      Author: thetimbrick
 */

#include "Request.h"

static size_t write_cb(char *ptr, size_t size, size_t nmemb, void *userdata) {
	std::stringstream* stream = (std::stringstream*)userdata;
	*stream << ptr;
	return nmemb * size;
}
static int progress_callback(void* clientp, curl_off_t dltotal, curl_off_t dlnow, curl_off_t ultotal, curl_off_t ulnow) {
	indicators::ProgressBar* prg = (indicators::ProgressBar*)clientp;
	if (dlnow > 0 && prg->is_completed() != true) {
		prg->set_progress(Utils<size_t>::changeRange(dlnow, dltotal, 0, 100, 0));
	}
	return CURL_PROGRESSFUNC_CONTINUE;
}


/*     size_t  pos = 0; */
/*     int     cnt = 0; */

/*     while( cnt != nth ) */
/*     { */
/*         pos+=1; */
/*         pos = str.find(findMe, pos); */
/*         if ( pos == std::string::npos ) */
/*             return -1; */
/*         cnt++; */
/*     } */
/*     return pos; */
/* } */


/* 	std::map<char, std::string> escapeMap; */

/* 	for(int i = 0; i < strlen(escapeChars); i++) { */
/* 		char escape[3]; */
/* 		sprintf(escape, "%%%02X", static_cast<uint8_t>(escapeChars[i])); */
/* 		escapeMap[escapeChars[i]] = std::string(escape); */
/* 	} */

/* 	std::stringstream str; */

/* 	for(int i = 0; i < strlen(str); i++) { */
/* 		if(i < pos) { */
/* 			str << str[i]; */
/* 		} else { */
/* 			if(escapeMap.find(str[i]) != escapeMap.end()) { */
/*         	                str << escapeMap[str[i]].c_str(); */
/*                 	} else { */
/*                         	str << str[i]; */
/*                 	} */
/* 		} */
/* 	} */

/* 	return str.str(); */
/* } */

int Request::download(std::string url, std::string output) {
	m_progress.set_option(indicators::option::PostfixText{output});
	m_progress.set_option(indicators::option::Completed{false});
	m_progress.set_progress(0);
	CURL *handle = curl_easy_init();
	std::string dUrl = url;
#ifdef __unix__
    char* encodedUri = g_uri_escape_string(dUrl.c_str(), "!*'();:@&=+$,/?#[]", true);
    dUrl = encodedUri;
#endif
	CURLcode res;
	FILE* fp = std::fopen(output.c_str(), "wb");
	curl_easy_setopt(handle, CURLOPT_FOLLOWLOCATION, 1L);
	curl_easy_setopt(handle, CURLOPT_URL, dUrl.c_str());
	curl_easy_setopt(handle, CURLOPT_WRITEDATA, fp);
	curl_easy_setopt(handle, CURLOPT_XFERINFOFUNCTION, &progress_callback);
	curl_easy_setopt(handle, CURLOPT_XFERINFODATA, &m_progress);
	curl_easy_setopt(handle, CURLOPT_NOPROGRESS, 0L);
	curl_easy_setopt(handle, CURLOPT_STDERR, std::fopen("/dev/null", "ro"));
	m_progress.set_option(indicators::option::PostfixText{output});
	res = curl_easy_perform(handle);
	if (res != CURLE_OK) {
		std::printf("Error. Please see error code for more details. ");
		std::printf("Errno: %d: \n", res);
		std::printf("Error: %s\n", curl_easy_strerror(res));
		m_progress.mark_as_completed();
		return EXIT_FAILURE;
	}
	curl_easy_cleanup(handle);
	std::fclose(fp);
	return EXIT_SUCCESS;
}

int Request::download(std::string output) {
	m_progress.set_option(indicators::option::PostfixText{output});
	m_progress.set_option(indicators::option::Completed{false});
	m_progress.set_progress(0);
	CURL *handle = curl_easy_init();
	std::string dUrl = m_baseURL;
#ifdef __unix__
	char* encodedUri = g_uri_escape_string(dUrl.c_str(), "!*'();:@&=+$,/?#[]", true);
    dUrl = encodedUri;
#endif
    CURLcode res;
	FILE* fp = std::fopen(output.c_str(), "wb");
	curl_easy_setopt(handle, CURLOPT_FOLLOWLOCATION, 1L);
	curl_easy_setopt(handle, CURLOPT_URL, dUrl.c_str());
	curl_easy_setopt(handle, CURLOPT_WRITEDATA, fp);
	curl_easy_setopt(handle, CURLOPT_XFERINFOFUNCTION, progress_callback);
	curl_easy_setopt(handle, CURLOPT_XFERINFODATA, &m_progress);
	curl_easy_setopt(handle, CURLOPT_NOPROGRESS, 0L);
	curl_easy_setopt(handle, CURLOPT_STDERR, std::fopen("/dev/null", "ro"));
	res = curl_easy_perform(handle);
	if (res != CURLE_OK) {
		std::printf("Error. Please see error code for more details. ");
		std::printf("Errno: %d\n", res);
		std::printf("Error: %s\n", curl_easy_strerror(res));
		m_progress.mark_as_completed();
		return EXIT_FAILURE;
	}
	curl_easy_cleanup(handle);
	std::fclose(fp);
	return EXIT_SUCCESS;
}

int Request::download() {
	m_progress.set_option(indicators::option::PostfixText{m_baseURL});
	m_progress.set_option(indicators::option::Completed{false});
	m_progress.set_progress(0);
	CURL *handle = curl_easy_init();
	std::string dUrl = m_baseURL;
#ifdef __unix__
    char* encodedUri = g_uri_escape_string(dUrl.c_str(), "!*'();:@&=+$,/?#[]", true);
    dUrl = encodedUri;
#endif
    CURLcode res;
	FILE* fp = std::fopen("Downloads", "wb");
	curl_easy_setopt(handle, CURLOPT_FOLLOWLOCATION, 1L);
	curl_easy_setopt(handle, CURLOPT_URL, dUrl.c_str());
	curl_easy_setopt(handle, CURLOPT_WRITEDATA, fp);
	curl_easy_setopt(handle, CURLOPT_XFERINFOFUNCTION, progress_callback);
	curl_easy_setopt(handle, CURLOPT_XFERINFODATA, &m_progress);
	curl_easy_setopt(handle, CURLOPT_NOPROGRESS, 0L);
	curl_easy_setopt(handle, CURLOPT_STDERR, std::fopen("/dev/null", "ro"));
	res = curl_easy_perform(handle);
	curl_easy_cleanup(handle);
	std::fclose(fp);
	free(handle);
	free(fp);
	handle = nullptr;
	fp = nullptr;
	if (res != CURLE_OK) {
		std::printf("Error. Please see error code for more details. ");
		std::printf("Errno: %d\n", res);
		std::printf("Error: %s\n", curl_easy_strerror(res));
		m_progress.mark_as_completed();
		return EXIT_FAILURE;
	}
	return EXIT_SUCCESS;
}


std::stringstream Request::sendGET(std::string endpoint) {
	std::stringstream output;
	CURL* handle = curl_easy_init();
	std::string dUrl = m_baseURL;
	CURLcode res;
	curl_easy_setopt(handle, CURLOPT_FOLLOWLOCATION, 1L);
	dUrl.append(endpoint);
	curl_easy_setopt(handle, CURLOPT_URL, dUrl.c_str());
	curl_easy_setopt(handle, CURLOPT_WRITEDATA, (void*)&output);
	curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, write_cb);
	curl_easy_setopt(handle, CURLOPT_HTTPGET, 1L);
	curl_easy_setopt(handle, CURLOPT_USERAGENT, "ModDown/dev");
	curl_easy_setopt(handle, CURLOPT_HTTPHEADER, m_header);
	curl_easy_setopt(handle, CURLOPT_HEADEROPT, CURLHEADER_UNIFIED);
	curl_easy_setopt(handle, CURLOPT_USE_SSL, CURLUSESSL_ALL);
	curl_easy_setopt(handle, CURLOPT_TIMEOUT, 60L);
	curl_easy_setopt(handle, CURLOPT_CONNECTTIMEOUT, 60L);
	res = curl_easy_perform(handle);
	sleep(1);
	if (res != CURLE_OK) {
		std::fprintf(stderr, "Error: Please see error code for more details. ");
		std::fprintf(stderr, "Errno: %d\n", res);
		std::printf("Error: %s\n", curl_easy_strerror(res));
		return std::stringstream("LIBCURLERR");
	}
	handle = nullptr;
	free(handle);
	return output;
}

void Request::setBase(std::string baseURL) {
	m_baseURL = baseURL;
}

void Request::setHeader(std::pair<std::string, std::string> key) {
	std::stringstream st;
	st << key.first << ": " << key.second;
	m_header = curl_slist_append(m_header, st.str().c_str());
}

void Request::clearHeader() {
	curl_slist_free_all(m_header);
}
