/**
 * @file audio.h
 */
#pragma once

#include <soundio/soundio.h>

class AudioCtrl {
private:
    AudioCtrl(){}
    ~AudioCtrl(){}

    AudioCtrl(const AudioCtrl&);
    AudioCtrl& operator=(const AudioCtrl&);
    static AudioCtrl* instance_;

    bool Initialize();

    struct SoundIo          *soundio_;
    struct SoundIoOutStream *outstream_;
    struct SoundIoDevice    *device_;


public:
    static AudioCtrl* Create();
    static void       Destroy();
    static AudioCtrl* GetInstance();

    void (*write_sample_)(char *ptr, double sample);

    bool Start();
    int  SampleRateGet();

    typedef double (*SignalCallbackFunc)( void* );
    SignalCallbackFunc signal_callback_func_;
    void*              signal_callback_userdata_;
    void SignalCallbackSet( SignalCallbackFunc func, void* userdata );
    void SignalCallbackUnset();

};
