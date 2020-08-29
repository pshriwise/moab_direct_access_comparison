
#ifndef __TIMER_H__
#define __TIMER_H__

#include <chrono>

class Timer {
public:
  using clock = std::chrono::high_resolution_clock;

  Timer() {};

  //! Start running the timer
  void start();

  //! Get total elapsed time in seconds
  //! \return Elapsed time in [s]
  double elapsed();

  //! Stop running the timer
  void stop();

  //! Stop the timer and reset its elapsed time
  void reset();

private:
  bool running_ {false}; //!< is timer running?
  std::chrono::time_point<clock> start_; //!< starting point for clock
  double elapsed_ {0.0}; //!< elapsed time in [s]
};

#endif
