#include "download.hpp"
#include <vector>
#include <malloc.h>
#include <stdio.h>
using namespace std;

static size_t header_function(char* b, size_t size, size_t nitems, void *userdata) 
{
	size_t numbytes = size * nitems;
	std::string tmp_string(b, numbytes);
	tmp_string.erase(tmp_string.size() - 2);
	std::vector<std::string> *data = static_cast<std::vector<std::string>*>(userdata);
	data->push_back(tmp_string);	
    return size * nitems;
}

size_t write_callback_stdio(char *ptr, size_t size, size_t nmemb, void *userdata)
{
    size_t written = fwrite(ptr, size, nmemb, (FILE*)userdata);
    return written;
}

void DownloadManager::SetProgressMeterCallback(int (*progress_callback)(void *clientp,double dltotal,double dlnow,double ultotal,double ulnow), void *userdata)
{
    curl_easy_setopt(this->curl_handle, CURLOPT_NOPROGRESS, 0L);
    curl_easy_setopt(this->curl_handle, CURLOPT_PROGRESSFUNCTION, progress_callback);
    curl_easy_setopt(this->curl_handle, CURLOPT_PROGRESSDATA, userdata);
}

void DownloadManager::SetHeaderFunctionCallback(size_t (*header_function)(char* b, size_t size, size_t nitems, void *userdata), void *headers)
{
    curl_easy_setopt(this->curl_handle, CURLOPT_HEADERFUNCTION, header_function);
    curl_easy_setopt(this->curl_handle, CURLOPT_HEADERDATA, headers);
}

void DownloadManager::SetWriteFunctionCallback(size_t (*write_callback)(char *ptr, size_t size, size_t nmemb, void *userdata), void *userdata)
{
    curl_easy_setopt(this->curl_handle, CURLOPT_WRITEFUNCTION, write_callback);
    curl_easy_setopt(this->curl_handle, CURLOPT_WRITEDATA, userdata);
}

pair<Result, std::string> DownloadManager::Initialize(void)
{
    u32 *socubuf = (u32 *)memalign(0x1000, 0x100000);
    if (!socubuf) {
        return make_pair((Result)0x1, "Failed to Allocate soc buffer");
    }

    if (R_FAILED(this->retcode = socInit(socubuf, 0x100000))) {
        return make_pair(this->retcode, "SocInit Failed");
    }

    this->curl_handle = curl_easy_init();

    curl_easy_setopt(this->curl_handle, CURLOPT_FOLLOWLOCATION, 1L);
    curl_easy_setopt(this->curl_handle, CURLOPT_MAXREDIRS, 50L);
    curl_easy_setopt(this->curl_handle, CURLOPT_SSL_VERIFYPEER, 0L);
    curl_easy_setopt(this->curl_handle, CURLOPT_VERBOSE, 1L);
    this->SetHeaderFunctionCallback(header_function, &this->headers);
    
    return make_pair((Result)0, "Init successful");

}

void DownloadManager::SetURL(std::string url, std::string userAgent)
{
    fprintf(stderr, "SETURL: %s", url.c_str());
    curl_easy_setopt(this->curl_handle, CURLOPT_URL, url.c_str());
    curl_easy_setopt(this->curl_handle, CURLOPT_USERAGENT, userAgent.c_str());
}

void DownloadManager::SetDownloadLocation(std::string dl)
{
    this->download_location = dl;
}

pair<CURLcode, std::string> DownloadManager::Perform()
{
    this->ret = curl_easy_perform(this->curl_handle);
    return make_pair(this->ret, curl_easy_strerror(this->ret));
}

pair<CURLcode, std::string> DownloadManager::DownloadDirectly(void)
{   
    std::string _download_location;
    if(this->download_location != "/") _download_location = this->download_location + "/sometext"; 
    else  _download_location = this->download_location + "sometext"; 
    FILE *file = fopen(_download_location.c_str(), "wb+");
    curl_easy_setopt(this->curl_handle, CURLOPT_WRITEFUNCTION, write_callback_stdio);
    curl_easy_setopt(this->curl_handle, CURLOPT_WRITEDATA, file);
    auto r = this->Perform(); //Get the file
    fclose(file);
    std::string filename;
    for(auto i : this->headers)
    {
        if(i.find("Content-Disposition") != string::npos)
        {
            filename = i.substr(i.find("=") + 1);
            break;
        }
    }

    printf(this->download_location.c_str());
    if(this->download_location != "/")
        std::rename(_download_location.c_str(), std::string(this->download_location + "/" + filename).c_str());
    else
        std::rename(_download_location.c_str(), std::string(this->download_location + filename).c_str());
    return make_pair(r.first, r.second);
}