
#ifndef NV_TIMER_H_
#define NV_TIMER_H_

#ifdef WIN32
#  include <windows.h>
#else
#  include <unistd.h>
#  include <sys/time.h>
#endif

#include <iostream>

class timer
{
public:
    
    timer(int maxFrames = 100)
    {
#ifdef _WIN32
        QueryPerformanceFrequency(&frequency);
#endif
        max_frames = maxFrames;
        framecount = 0;
        updated_timing = false;
        fps = 0;
    }
    
    // call this after swapbuffers
    void frame(void)
    {
        
        framecount++;
        
        if(framecount >= max_frames)
        {
#        ifdef _WIN32
            QueryPerformanceCounter(&newCount);
            fps = max_frames * (double)frequency.QuadPart	/ (double)(newCount.QuadPart - oldCount.QuadPart);
#        else
            timeval tmptime;
            gettimeofday (&tmptime,NULL);
            newCount = tmptime.tv_sec+tmptime.tv_usec*.000001;
            
            fps = max_frames * 1.0 / (newCount-oldCount);
#        endif
            
#        ifdef _WIN32
            QueryPerformanceCounter(&oldCount);
#        else
            gettimeofday (&tmptime,NULL);
            oldCount = tmptime.tv_sec+tmptime.tv_usec*.000001;
#        endif
            framecount = 0;
            updated_timing = true;
        }
        else
        {
            updated_timing = false;
        }
        
    }

    void frame(bool print_if_update)
    {
        frame();
        if(print_if_update)
        {
            if(updated_timing)
                std::cerr << fps << " fps" << std::endl;
        }
    }

    float get_fps()
    { return fps; }

    bool timing_updated()
    { return updated_timing; }

private:
    int framecount;
#ifdef _WIN32
    LARGE_INTEGER oldCount, newCount;
    LARGE_INTEGER frequency;
#else
    double oldCount, newCount;
#endif
    int max_frames;    
    float fps;    
    bool updated_timing;    
};

#endif
