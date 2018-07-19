#pragma once
#include <curl/curl.h>
#include <3ds.h>
#include <vector>
#include <utility>
#include <string>
#include "archive.h"

class DownloadManager : private archive
{
    public:
    std::pair<Result, std::string> Initialize(void);
    void SetURL(std::string url, std::string userAgent = "MULTIDOWNLOAD 2018");
    void SetDownloadLocation(std::string dl);
    void SetProgressMeterCallback(int (*progress_callback)(void *clientp,double dltotal,double dlnow,double ultotal,double ulnow), void *userdata);
    void SetHeaderFunctionCallback(size_t (*header_function)(char* b, size_t size, size_t nitems, void *userdata), void *headers);
    void SetWriteFunctionCallback(size_t (*write_callback)(char *ptr, size_t size, size_t nmemb, void *userdata), void *userdata);
    std::pair<CURLcode, std::string> DownloadDirectly(void);
    std::pair<CURLcode, std::string> Perform();
    void ExtractFileIfArchive(bool _extract = false){ this->m_extract = _extract; }
    private:
    std::vector<std::string> headers;
    std::string download_location = "";
    CURL *curl_handle = nullptr;
    bool m_extract = false;
    CURLcode ret;
    Result retcode = 0;
};