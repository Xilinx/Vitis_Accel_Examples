#pragma once

#include <chrono>
#include <ctime>
#include <iomanip>

struct cPerfTimer {
  std::chrono::high_resolution_clock::time_point m_start;
  std::chrono::high_resolution_clock::time_point m_end;
  std::chrono::duration<double> m_total;

  std::string m_name;

  cPerfTimer() { initialize(); }
  void initialize() { m_total = std::chrono::duration<double>(0.0); }
  void start() { m_start = std::chrono::high_resolution_clock::now(); }
  void stop() {
    m_end = std::chrono::high_resolution_clock::now();
    m_total += (m_end - m_start);
  }
  double get_ms() { return 1000 * m_total.count(); }
};

extern cPerfTimer *_g_timer;
extern int _g_timer_last_id;

#ifndef __DISABLE_TIMERS__
#define TIMER_INIT(a)                                                          \
  cPerfTimer *_g_timer = new cPerfTimer[(a)];                                  \
  int _g_timer_last_id = 0;
#define TIMER_START(a)                                                         \
  _g_timer[(a)].start();                                                       \
  _g_timer_last_id = (a);
#define TIMER_STOP _g_timer[_g_timer_last_id].stop();
#define TIMER_STOP_ID(a) _g_timer[(a)].stop();
#define TIMER_REPORT_MS(a) _g_timer[(a)].get_ms()
#else
#define TIMER_INIT(a)
#define TIMER_START(a)
#define TIMER_STOP
#define TIMER_STOP_ID(a)
#define TIMER_REPORT(a)
#endif
