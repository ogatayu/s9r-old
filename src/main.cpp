/**
 * @file main.cpp
 */

#include <iostream>
#include <cstdlib>
#include <unistd.h>
#include <math.h>

#include <soundio/soundio.h>

#include "common.h"
#include "synth.h"
#include "midi.h"

float pitch = 440.0f;
int NoteNumber = 1;

/**
 * @brief write_callback
 *
 * @param[out] outstream
 * @param[in]  frame_count_min
 * @param[in]  frame_count_max
 */
static void write_callback(
    struct SoundIoOutStream *outstream,
    int frame_count_min,
    int frame_count_max
    )
{
    int err;

    const struct SoundIoChannelLayout *layout = &outstream->layout;

    float seconds_per_frame = 1.0f / outstream->sample_rate;
    int   frames_left = frame_count_max;
    while (frames_left > 0) {
        int frame_count = frames_left;

        struct SoundIoChannelArea *areas;
        err = soundio_outstream_begin_write(outstream, &areas, &frame_count);
        if (err) {
            fprintf(stderr, "%s\n", soundio_strerror(err));
            exit(1);
        }

        if (!frame_count) {
            break;
        }

        for (int frame = 0; frame < frame_count; frame++) {
            // chごとに出力
            for (int ch = 0; ch < layout->channel_count; ch++) {
                float *ptr = (float*)(areas[ch].ptr + areas[ch].step * frame);
                *ptr = 0;
            }
        }

        // 押されたキーを検索
        for(int nn=0; nn<128; nn++) {
            if( midiKeyTable[nn].isPressed ) {
                // 波形作成
                int p = midiKeyTable[nn].m_p;
                float v = (float)midiKeyTable[nn].velocity / 127.0f;
                int w = synthGetW(nn, 0, 0);

                for (int frame = 0; frame < frame_count; frame++) {
                    float sample = synthGetWave( p ) * v;

                    // chごとに出力
                    for (int ch = 0; ch < layout->channel_count; ch++) {
                        float *ptr = (float*)(areas[ch].ptr + areas[ch].step * frame);
                        *ptr += sample;
                    }
                    p += w;
                }

                midiKeyTable[nn].m_p = p;
            }
        }

        err = soundio_outstream_end_write(outstream);
        if (err) {
            fprintf(stderr, "%s\n", soundio_strerror(err));
            exit(1);
        }

        frames_left -= frame_count;
    }
}


/**
 * @brief main
 * @param[in]  argc
 * @param[in]  argv
 */
int main(int argc, char *argv[])
{
    synthInit();
    midiInit();

    int err;
    struct SoundIo *soundio = soundio_create();
    if (!soundio) {
        fprintf(stderr, "out of memory\n");
        return 1;
    }

    if ((err = soundio_connect(soundio))) {
        fprintf(stderr, "error connecting: %s\n", soundio_strerror(err));
        return 1;
    }

    soundio_flush_events(soundio);

    int default_out_device_index = soundio_default_output_device_index(soundio);
    if (default_out_device_index < 0) {
        fprintf(stderr, "no output device found\n");
        return 1;
    }

    struct SoundIoDevice *device = soundio_get_output_device(soundio, default_out_device_index);
    if (!device) {
        fprintf(stderr, "out of memory\n");
        return 1;
    }

    fprintf(stderr, "Output device: %s\n", device->name);

    struct SoundIoOutStream *outstream = soundio_outstream_create(device);
    if (!outstream) {
        fprintf(stderr, "out of memory\n");
        return 1;
    }
    outstream->format         = SoundIoFormatFloat32NE;
    outstream->write_callback = write_callback;


    if ((err = soundio_outstream_open(outstream))) {
        fprintf(stderr, "unable to open device: %s", soundio_strerror(err));
        return 1;
    }

    if (outstream->layout_error) {
        fprintf(stderr, "unable to set channel layout: %s\n", soundio_strerror(outstream->layout_error));
    }

    fprintf(stderr, "outstream->sample_rate: %d\n", outstream->sample_rate);

    if ((err = soundio_outstream_start(outstream))) {
        fprintf(stderr, "unable to start device: %s\n", soundio_strerror(err));
        return 1;
    }

    for (;;) {
        soundio_wait_events(soundio);
    }

    soundio_outstream_destroy(outstream);
    soundio_device_unref(device);
    soundio_destroy(soundio);

    return 0;
}

