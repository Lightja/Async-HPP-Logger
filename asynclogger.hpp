#pragma once
//
// asynchronous header-only shared buffer logger for c++
// PUBLIC DOMAIN
// Author: Lightja 2/8/25
// Known issues: log_level doesn't function and name_current_thread() is sometimes ignored.
//
// Example Usage: (similar to cout)
// name_current_thread("example thread")
// log_debug << "example value: " << value << endl;
// log_info << "Done with example!" << endl;
//
// Example Output:
// [2025-02-08 13:09:18.840][ DEBUG ][example thread  ] example value: 5
// [2025-02-08 13:09:18.940][ INFO  ][example thread  ] Done with example!
#include <iomanip>
#include <iostream>
#include <fstream>
#include <sstream>
#include <map>
#include <syncstream>
#include <string>
#include <unordered_map>
#include <thread>
// only std includes for portability
using std::string;

enum class log_stream_log_level { DEBUG=0, INFO=1, WARNING=2, ERROR_ONLY=3 };
static log_stream_log_level log_stream_global_log_level = log_stream_log_level::DEBUG;
static inline std::string log_stream_file_name = "apl_log.txt";
static inline std::atomic<int> log_stream_thread_counter = 1;
static inline std::unordered_map <std::thread::id, string> log_stream_thread_map;
static inline std::unordered_map <std::thread::id, string> log_stream_thread_id_map;
static inline std::mutex log_stream_mtx;
static inline std::condition_variable log_stream_cv;
static inline std::atomic<bool> log_stream_new_line = true;
static inline std::thread::id log_stream_current_thread;

class log : public std::osyncstream {
public: 
    log(enum log_stream_log_level level) : std::osyncstream(std::cout), log_level(level) { log_file.open(log_stream_file_name, std::ios::app); }
    log& operator<<(const char* str)                        { if (log_level_too_low()) { return *this; } std::unique_lock<std::mutex> lock(log_stream_mtx); wait_if_busy(lock); prepend_new_line(); std::cout << str;  log_file << str;  return *this; }
    log& operator<<(const string& str)                      { if (log_level_too_low()) { return *this; } std::unique_lock<std::mutex> lock(log_stream_mtx); wait_if_busy(lock); prepend_new_line(); std::cout << str;  log_file << str;  return *this; }
    log& operator<<(char c)                                 { if (log_level_too_low()) { return *this; } std::unique_lock<std::mutex> lock(log_stream_mtx); wait_if_busy(lock); prepend_new_line(); std::cout << c;    log_file << c;    return *this; }
    log& operator<<(bool b)                                 { if (log_level_too_low()) { return *this; } std::unique_lock<std::mutex> lock(log_stream_mtx); wait_if_busy(lock); prepend_new_line(); std::cout << b;    log_file << b;    return *this; }
    log& operator<<(long l)                                 { if (log_level_too_low()) { return *this; } std::unique_lock<std::mutex> lock(log_stream_mtx); wait_if_busy(lock); prepend_new_line(); std::cout << l;    log_file << l;    return *this; }
    log& operator<<(unsigned long l)                        { if (log_level_too_low()) { return *this; } std::unique_lock<std::mutex> lock(log_stream_mtx); wait_if_busy(lock); prepend_new_line(); std::cout << l;    log_file << l;    return *this; }
    log& operator<<(long long ll)                           { if (log_level_too_low()) { return *this; } std::unique_lock<std::mutex> lock(log_stream_mtx); wait_if_busy(lock); prepend_new_line(); std::cout << ll;   log_file << ll;   return *this; }
    log& operator<<(unsigned long long ll)                  { if (log_level_too_low()) { return *this; } std::unique_lock<std::mutex> lock(log_stream_mtx); wait_if_busy(lock); prepend_new_line(); std::cout << ll;   log_file << ll;   return *this; }
    log& operator<<(double d)                               { if (log_level_too_low()) { return *this; } std::unique_lock<std::mutex> lock(log_stream_mtx); wait_if_busy(lock); prepend_new_line(); std::cout << d;    log_file << d;    return *this; }
    log& operator<<(long double ld)                         { if (log_level_too_low()) { return *this; } std::unique_lock<std::mutex> lock(log_stream_mtx); wait_if_busy(lock); prepend_new_line(); std::cout << ld;   log_file << ld;   return *this; }
    log& operator<<(const void* p)                          { if (log_level_too_low()) { return *this; } std::unique_lock<std::mutex> lock(log_stream_mtx); wait_if_busy(lock); prepend_new_line(); std::cout << p;    log_file << p;    return *this; }
    log& operator<<(const volatile void* p)                 { if (log_level_too_low()) { return *this; } std::unique_lock<std::mutex> lock(log_stream_mtx); wait_if_busy(lock); prepend_new_line(); std::cout << p;    log_file << p;    return *this; }
    log& operator<<(std::nullptr_t p)                       { if (log_level_too_low()) { return *this; } std::unique_lock<std::mutex> lock(log_stream_mtx); wait_if_busy(lock); prepend_new_line(); std::cout << p;    log_file << p;    return *this; }
    log& operator<<(short s)                                { if (log_level_too_low()) { return *this; } std::unique_lock<std::mutex> lock(log_stream_mtx); wait_if_busy(lock); prepend_new_line(); std::cout << s;    log_file << s;    return *this; }
    log& operator<<(unsigned short s)                       { if (log_level_too_low()) { return *this; } std::unique_lock<std::mutex> lock(log_stream_mtx); wait_if_busy(lock); prepend_new_line(); std::cout << s;    log_file << s;    return *this; }
    log& operator<<(int i)                                  { if (log_level_too_low()) { return *this; } std::unique_lock<std::mutex> lock(log_stream_mtx); wait_if_busy(lock); prepend_new_line(); std::cout << i;    log_file << i;    return *this; }
    log& operator<<(unsigned int i)                         { if (log_level_too_low()) { return *this; } std::unique_lock<std::mutex> lock(log_stream_mtx); wait_if_busy(lock); prepend_new_line(); std::cout << i;    log_file << i;    return *this; }
    log& operator<<(float f)                                { if (log_level_too_low()) { return *this; } std::unique_lock<std::mutex> lock(log_stream_mtx); wait_if_busy(lock); prepend_new_line(); std::cout << f;    log_file << f;    return *this; }
    log& operator<<(uint8_t i)                              { if (log_level_too_low()) { return *this; } std::unique_lock<std::mutex> lock(log_stream_mtx); wait_if_busy(lock); prepend_new_line(); std::cout << i;    log_file << i;    return *this; }
    log& operator<<(std::streambuf* sb)                     { if (log_level_too_low()) { return *this; } std::unique_lock<std::mutex> lock(log_stream_mtx); wait_if_busy(lock); prepend_new_line(); std::cout << sb;   log_file << sb;   return *this; }
    log& operator<<(std::ios_base& (*_Pfn)(std::ios_base&)) { if (log_level_too_low()) { return *this; } std::unique_lock<std::mutex> lock(log_stream_mtx); wait_if_busy(lock); prepend_new_line(); std::cout << _Pfn; log_file << _Pfn; return *this; }
    log& operator<<(basic_ostream<log::basic_ostream::char_type, log::basic_ostream::traits_type>& (*_Pfn)(basic_ostream<log::basic_ostream::char_type, log::basic_ostream::traits_type>&)) {
        //endl handler - apparently endl is a function
        if (log_level_too_low()) { return *this; }
        std::unique_lock<std::mutex> lock(log_stream_mtx); wait_if_busy(lock);
        if (_Pfn == std::endl<log::basic_ostream::char_type, log::basic_ostream::traits_type>) {
            log_stream_new_line.store(true);
            log_stream_cv.notify_all();
            std::cout << std::endl;
            log_file << std::endl;
        }
        return *this;
    }
    static inline void name_thread(std::thread& thread, string name) { const char* test = name.c_str(); log_stream_thread_map[thread.get_id()]            = name; log_stream_thread_id_map[thread.get_id()]            = std::to_string(log_stream_thread_counter.fetch_add(1)); }
    static inline void name_current_thread(string name)              { const char* test = name.c_str(); log_stream_thread_map[std::this_thread::get_id()] = name; log_stream_thread_id_map[std::this_thread::get_id()] = std::to_string(log_stream_thread_counter.fetch_add(1)); }
    static inline void set_log_level(log_stream_log_level level)     { log_stream_global_log_level = level; }
    static inline void set_log_level(string level)                   { if (level == "DEBUG") { log_stream_global_log_level = log_stream_log_level::DEBUG; } else if (level == "INFO") { log_stream_global_log_level = log_stream_log_level::INFO; } else if (level == "WARNING") { log_stream_global_log_level = log_stream_log_level::WARNING; } else if (level == "ERROR") { log_stream_global_log_level = log_stream_log_level::ERROR_ONLY; } }

private: 
    std::ofstream log_file;
    std::unordered_map<log_stream_log_level, string> log_level_strings = { { log_stream_log_level::DEBUG,      " DEBUG " }, { log_stream_log_level::INFO,       " INFO  " }, { log_stream_log_level::WARNING,    "WARNING" }, { log_stream_log_level::ERROR_ONLY, " ERROR " } };
    log_stream_log_level log_level;
    //
    inline bool log_level_too_low()                              { return log_level < log_stream_global_log_level; }
    inline void wait_if_busy(std::unique_lock<std::mutex>& lock) { log_stream_cv.wait_for(lock, std::chrono::milliseconds(100), [this] { return log_stream_current_thread == std::this_thread::get_id() || log_stream_new_line.load(); }); log_stream_current_thread = std::this_thread::get_id(); }
    inline void prepend_new_line()                               { if (log_stream_new_line.load()) {std::osyncstream(std::cout) << "[" << this->get_time() << "][" << log_level_strings[this->log_level] << "][" << this->get_thread() << "] "; log_file << "[" << this->get_time() << "][" << log_level_strings[this->log_level] << "][" << this->get_thread() << "] "; log_stream_new_line.store(false); } }
    inline string get_level()                                    { return log_level_strings[log_level]; }
    inline string get_time() {
        std::stringstream ss; struct tm time; char time_buf[23];
        __time64_t time64 = _time64(&time64); 
        localtime_s(&time, &time64);
        std::strftime(time_buf, sizeof(time_buf), "%Y-%m-%d %H:%M:%S", &time);
        unsigned long long ms = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
        ss << time_buf << '.' << std::setfill('0') << std::setw(3) << ms % 1000 << std::setfill(' ') << std::setw(0);
        return ss.str();
    }
    inline string get_thread() {
        bool thread_named = log_stream_thread_map.find(std::this_thread::get_id()) != log_stream_thread_map.end();
        string thread_name;
        if (thread_named) { thread_name = log_stream_thread_map[std::this_thread::get_id()]; }
        else { log_stream_thread_id_map[std::this_thread::get_id()] = std::to_string(log_stream_thread_counter.fetch_add(1)); log_stream_thread_map[std::this_thread::get_id()] = "thread " + log_stream_thread_id_map[std::this_thread::get_id()]; thread_name = log_stream_thread_map[std::this_thread::get_id()];}
        thread_name.resize(20, ' ');
        return thread_name;
    }
};

static class log log_debug  (log_stream_log_level::DEBUG);
static class log log_info   (log_stream_log_level::INFO);
static class log log_warning(log_stream_log_level::WARNING);
static class log log_error  (log_stream_log_level::ERROR_ONLY);

