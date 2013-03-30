#ifndef __TIMER_H__
#define __TIMER_H__

class Timer {
  
private:
  
  int startTicks; // start time
  int pausedTicks; // save pause time
  
  bool started;
  bool paused;
  
public:
  
  Timer( );
  
  void start( );
  void stop( );
  void pause( );
  void unpause( );
  
  int getTicks( );
  
  bool isStarted( );
  bool isPaused( );
};


#endif