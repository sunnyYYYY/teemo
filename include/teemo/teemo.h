/*******************************************************************************
* Copyright (C) 2019 - 2023, winsoft666, <winsoft666@outlook.com>.
*
* THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,
* EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED
* WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
*
* Expect bugs
*
* Please use and enjoy. Please let me know of any bugs/improvements
* that you have found/implemented and I will fix/incorporate them into this
* file.
*******************************************************************************/
#ifndef TEEMO_H_
#define TEEMO_H_
#pragma once

#include <string>
#include <memory>
#include <future>

#ifdef TEEMO_STATIC
#define TEEMO_API
#else
#if defined(TEEMO_EXPORTS)
#if defined(_MSC_VER)
#define TEEMO_API __declspec(dllexport)
#else
#define TEEMO_API
#endif
#else
#if defined(_MSC_VER)
#define TEEMO_API __declspec(dllimport)
#else
#define TEEMO_API
#endif
#endif
#endif

namespace teemo {
enum Result {
  SUCCESSED = 0,
  UNKNOWN_ERROR,
  INVALID_URL,
  INVALID_INDEX_FORMAT,
  INVALID_TARGET_FILE_PATH,
  INVALID_THREAD_NUM,
  INVALID_NETWORK_CONN_TIMEOUT,
  INVALID_NETWORK_READ_TIMEOUT,
  INVALID_FETCH_FILE_INFO_RETRY_TIMES,
  ALREADY_DOWNLOADING,
  CANCELED,
  RENAME_TMP_FILE_FAILED,
  OPEN_INDEX_FILE_FAILED,
  TMP_FILE_EXPIRED,
  INIT_CURL_FAILED,
  INIT_CURL_MULTI_FAILED,
  SET_CURL_OPTION_FAILED,
  ADD_CURL_HANDLE_FAILED,
  CREATE_TARGET_FILE_FAILED,
  CREATE_TMP_FILE_FAILED,
  OPEN_TMP_FILE_FAILED,
  URL_DIFFERENT,
  TMP_FILE_SIZE_ERROR,
  TMP_FILE_CANNOT_RW,
  FLUSH_TMP_FILE_FAILED,
  UPDATE_INDEX_FILE_FAILED,
  SLICE_DOWNLOAD_FAILED
};

TEEMO_API const char* GetResultString(int enumVal);

class TEEMO_API Event {
 public:
  Event(bool setted = false);
  ~Event();

  void set() noexcept;
  void unset() noexcept;
  bool isSetted() noexcept;
  bool wait(int32_t millseconds) noexcept;

 protected:
  Event(const Event&) = delete;
  Event& operator=(const Event&) = delete;
  class EventImpl;
  EventImpl* impl_;
};

typedef std::string utf8string;
typedef std::function<void(Result ret)> ResultFunctor;
typedef std::function<void(int64_t total, int64_t downloaded)> ProgressFunctor;
typedef std::function<void(int64_t byte_per_sec)> RealtimeSpeedFunctor;
typedef std::function<void(const utf8string& verbose)> VerboseOuputFunctor;

class TEEMO_API Teemo {
 public:
  Teemo();
  ~Teemo();

  static void GlobalInit();
  static void GlobalUnInit();

  void setVerboseOutput(VerboseOuputFunctor verbose_functor) noexcept;

  // Pass an int specifying the maximum thread number.
  // teemo will use these threads as much as possible.
  // Set to 0 or negative to switch to the default built-in thread number - 1.
  // The number of threads cannot be greater than 100, otherwise teemo will return INVALID_THREAD_NUM.
  //
  Result setThreadNum(int32_t thread_num) noexcept;
  int32_t threadNum() const noexcept;


  // Pass an int. It should contain the maximum time in milliseconds that you allow the connection phase to the server to take. 
  // This only limits the connection phase, it has no impact once it has connected. 
  // Set to 0 or negative to switch to the default built-in connection timeout - 3000 milliseconds.
  //
  Result setNetworkConnectionTimeout(int32_t milliseconds) noexcept;  // milliseconds
  int32_t networkConnectionTimeout() const noexcept;                  // milliseconds


  // Pass an int specifying the retry times when request file information(such as file size) failed.
  // Set to 0 or negative to switch to the default built-in retry times - 1.
  //
  Result setFetchFileInfoRetryTimes(int32_t retry_times) noexcept;
  int32_t fetchFileInfoRetryTimes() const noexcept;


  // Pass an int as parameter.
  // If the interval seconds that from the saved time of temporary file to present greater than or equal to this parameter, the temporary file will be discarded.
  // Default to -1, never expired.
  //
  Result setTmpFileExpiredTime(int32_t seconds) noexcept;  // seconds
  int32_t tmpFileExpiredTime() const noexcept;             // seconds


  // Pass an int as parameter.
  // If a download exceeds this speed (counted in bytes per second) the transfer will pause to keep the speed less than or equal to the parameter value.
  // Defaults to -1, unlimited speed.
  // This option doesn't affect transfer speeds done with FILE:// URLs.
  //
  Result setMaxDownloadSpeed(int32_t byte_per_seconds) noexcept;
  int32_t maxDownloadSpeed() const noexcept;

  // Pass an unsigned int specifying your maximal size for the disk cache total buffer in teemo.
  // This buffer size is by default 20971520 byte (20MB).
  //
  Result setDiskCacheSize(int32_t cache_size) noexcept;  // byte
  int32_t diskCacheSize() const noexcept;                // byte

  // Set an event, teemo will stop downloading when this event set.
  // If download is stopped for stop_event set or call stop, teemo will return CANCELED.
  //
  Result setStopEvent(Event* stop_event) noexcept;
  Event* stopEvent() noexcept;

  // Set true, teemo will not check whether the url passed by *start* is the same as in the index file, 
  // and in this case, if the url passed by *start* is empty, teemo will use the url in index file.
  // Default to false, if the url passed by *start* is different from the url in the index file, teemo will return URL_DIFFERENT error.
  //
  Result setSkippingUrlCheck(bool skip) noexcept;
  bool skippingUrlCheck() const noexcept;

  // Start to download.
  // Supported url protocol is the same as libcurl.
  //
  std::shared_future<Result> start(const utf8string& url,
                                   const utf8string& target_file_path,
                                   ResultFunctor result_functor,
                                   ProgressFunctor progress_functor,
                                   RealtimeSpeedFunctor realtime_speed_functor) noexcept;

  // Stop downloading, teemo will return CANCELED.
  //
  void stop() noexcept;

  utf8string url() const noexcept;
  utf8string targetFilePath() const noexcept;

 protected:
  class TeemoImpl;
  TeemoImpl* impl_;

  Teemo(const Teemo&) = delete;
  Teemo& operator=(const Teemo&) = delete;
};
}  // namespace teemo
#endif  // !TEEMO_H_