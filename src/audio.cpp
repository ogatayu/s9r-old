/**
 * @file audio.cpp
 */
#include <iostream>
#include <cstdlib>
#include <string.h>
#include <math.h>

#include <soundio/soundio.h>
#include "audio.h"

static void write_sample_s16ne(char *ptr, double sample);
static void write_sample_s32ne(char *ptr, double sample);
static void write_sample_float32ne(char *ptr, double sample);
static void write_sample_float64ne(char *ptr, double sample);
static void write_callback(
    struct SoundIoOutStream *outstream,
    int frame_count_min,
    int frame_count_max
    );

AudioCtrl* AudioCtrl::instance_ = nullptr;


/**
 * @brief Create
 */
AudioCtrl* AudioCtrl::Create()
{
    if (!instance_) {
        instance_ = new AudioCtrl;
        if( !(instance_->Initialize()) ) {
            delete instance_;
            instance_ = nullptr;
        }
    }
    return instance_;
}

/**
 * @brief Destroy
 */
void AudioCtrl::Destroy()
{
    soundio_outstream_destroy(instance_->outstream_);
    soundio_device_unref(instance_->device_);
    soundio_destroy(instance_->soundio_);

    delete instance_;
    instance_ = nullptr;
}

/**
 * @brief GetInstance
 */
AudioCtrl* AudioCtrl::GetInstance()
{
    return instance_;
}

static void underflow_callback(struct SoundIoOutStream *outstream) {
    static int count = 0;
    fprintf(stderr, "underflow %d\n", count++);
}

/**
 * @brief initialize class
 */
bool AudioCtrl::Initialize()
{
    int err;
    enum SoundIoBackend backend = SoundIoBackendNone;
    //backend = SoundIoBackendDummy;

    // init libsoundio
    soundio_ = soundio_create();
    if (!soundio_) {
        fprintf(stderr, "out of memory\n");
        return false;
    }

    err = (backend == SoundIoBackendNone) ?
        soundio_connect(soundio_) : soundio_connect_backend(soundio_, backend);
    if (err) {
        fprintf(stderr, "Unable to connect to backend: %s\n", soundio_strerror(err));
        return false;
    }

    fprintf(stderr, "Backend: %s\n", soundio_backend_name(soundio_->current_backend));

    soundio_flush_events(soundio_);

    // select device
    int default_out_device_index = soundio_default_output_device_index(soundio_);
    if (default_out_device_index < 0) {
        fprintf(stderr, "no output device found\n");
        return false;
    }

    device_ = soundio_get_output_device(soundio_, default_out_device_index);
    if (!device_) {
        fprintf(stderr, "out of memory\n");
        return false;
    }
    fprintf(stderr, "Output device: %s\n", device_->name);

    if (device_->probe_error) {
        fprintf(stderr, "Cannot probe device: %s\n", soundio_strerror(device_->probe_error));
        return 1;
    }

    // initialize outstream
    outstream_ = soundio_outstream_create(device_);
    if (!outstream_) {
        fprintf(stderr, "out of memory\n");
        return false;
    }
    outstream_->underflow_callback = underflow_callback;
    outstream_->write_callback     = write_callback;
    outstream_->userdata           = this;

    if (soundio_device_supports_format(device_, SoundIoFormatFloat32NE)) {
        outstream_->format = SoundIoFormatFloat32NE;
        write_sample_ = write_sample_float32ne;
    } else if (soundio_device_supports_format(device_, SoundIoFormatFloat64NE)) {
        outstream_->format = SoundIoFormatFloat64NE;
        write_sample_ = write_sample_float64ne;
    } else if (soundio_device_supports_format(device_, SoundIoFormatS32NE)) {
        outstream_->format = SoundIoFormatS32NE;
        write_sample_ = write_sample_s32ne;
    } else if (soundio_device_supports_format(device_, SoundIoFormatS16NE)) {
        outstream_->format = SoundIoFormatS16NE;
        write_sample_ = write_sample_s16ne;
    } else {
        fprintf(stderr, "No suitable device format available.\n");
        return false;
    }

    if ((err = soundio_outstream_open(outstream_))) {
        fprintf(stderr, "unable to open device: %s", soundio_strerror(err));
        return false;
    }

    if (outstream_->layout_error) {
        fprintf(stderr, "unable to set channel layout: %s\n", soundio_strerror(outstream_->layout_error));
    }

    fprintf(stderr, "Software latency: %f sec\n", outstream_->software_latency);

    return true;
}

/**
 * @brief start
 */
bool AudioCtrl::Start()
{
    int err;

    fprintf(stderr, "outstream_->sample_rate: %d\n", outstream_->sample_rate);
    if ((err = soundio_outstream_start(outstream_))) {
        fprintf(stderr, "unable to start device: %s\n", soundio_strerror(err));
        return false;
    }

    for (;;) {
        soundio_wait_events(soundio_);
    }
}

///////////////////////////////////////////////////////////////////////////////

static void write_sample_s16ne(char *ptr, double sample) {
    int16_t *buf = (int16_t *)ptr;
    double range = (double)INT16_MAX - (double)INT16_MIN;
    double val = sample * range / 2.0;
    *buf = val;
}

static void write_sample_s32ne(char *ptr, double sample) {
    int32_t *buf = (int32_t *)ptr;
    double range = (double)INT32_MAX - (double)INT32_MIN;
    double val = sample * range / 2.0;
    *buf = val;
}

static void write_sample_float32ne(char *ptr, double sample) {
    float *buf = (float *)ptr;
    *buf = sample;
}

static void write_sample_float64ne(char *ptr, double sample) {
    double *buf = (double *)ptr;
    *buf = sample;
}

///////////////////////////////////////////////////////////////////////////////

//#define AUDIO_SINE_TEST
#if defined(AUDIO_SINE_TEST)
static const double PI = 3.14159265358979323846264338328;
static double seconds_offset = 0.0;
#endif

/**
 * @brief write_callback
 *
 * @param[out] outstream_
 * @param[in]  frame_count_min
 * @param[in]  frame_count_max
 */
static void write_callback(struct SoundIoOutStream *outstream, int frame_count_min, int frame_count_max)
{
    AudioCtrl* audioctrl = (AudioCtrl*)outstream->userdata;

    double float_sample_rate = outstream->sample_rate;
    double seconds_per_frame = 1.0 / float_sample_rate;
    struct SoundIoChannelArea *areas;
    int err;

    int frames_left = frame_count_max;

    while (frames_left > 0) {
        int frame_count = frames_left;
        if ((err = soundio_outstream_begin_write(outstream, &areas, &frame_count))) {
            fprintf(stderr, "unrecoverable stream error: %s\n", soundio_strerror(err));
            exit(1);
        }

        if (!frame_count) {
            break;
        }

        const struct SoundIoChannelLayout *layout = &outstream->layout;

#if !defined(AUDIO_SINE_TEST)
        for (int frame = 0; frame < frame_count; frame += 1) {
            double sample = audioctrl->signal_callback_func_(audioctrl->signal_callback_userdata_);
            for (int channel = 0; channel < layout->channel_count; channel += 1) {
                audioctrl->write_sample_(areas[channel].ptr, sample);
                areas[channel].ptr += areas[channel].step;
            }
        }
        //seconds_offset = fmod(seconds_offset + seconds_per_frame * frame_count, 1.0);
#else
        double pitch = 440.0;
        double radians_per_second = pitch * 2.0 * PI;
        for (int frame = 0; frame < frame_count; frame += 1) {
            double sample = sin((seconds_offset + frame * seconds_per_frame) * radians_per_second);
            for (int channel = 0; channel < layout->channel_count; channel += 1) {
                audioctrl->write_sample_(areas[channel].ptr, sample);
                areas[channel].ptr += areas[channel].step;
            }
        }
        seconds_offset = fmod(seconds_offset + seconds_per_frame * frame_count, 1.0);
#endif

        if ((err = soundio_outstream_end_write(outstream))) {
            if (err == SoundIoErrorUnderflow)
                return;
            fprintf(stderr, "unrecoverable stream error: %s\n", soundio_strerror(err));
            exit(1);
        }

        frames_left -= frame_count;
    }
}

/**
 * @brief SampleRateGet
 */
int AudioCtrl::SampleRateGet()
{
    return outstream_->sample_rate;
}

/**
 * @brief SignalCallbackSet
 */
void AudioCtrl::SignalCallbackSet( SignalCallbackFunc func, void* userdata )
{
    signal_callback_func_     = func;
    signal_callback_userdata_ = userdata;
}

/**
 * @brief SignalCallbackSet
 */
void AudioCtrl::SignalCallbackUnset()
{
    signal_callback_func_     = nullptr;
    signal_callback_userdata_ = nullptr;
}
