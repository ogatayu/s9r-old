// qmidiin.cpp
#include <iostream>
#include <cstdlib>
#include <signal.h>
#include <unistd.h>
#include <math.h>

#include <soundio/soundio.h>

#include "RtMidi.h"

float pitch = 440.0f;
float volume = 1.0;

/**
 * @brief chooseMidiPort
 * @param[in,out] rtmidi
 */
static bool chooseMidiPort( RtMidiIn *rtmidi )
{
    std::string portName;
    unsigned int ix = 0, nPorts = rtmidi->getPortCount();
    if ( nPorts == 0 ) {
        std::cout << "No input ports available!" << std::endl;
        return false;
    }

    if ( nPorts == 1 ) {
        std::cout << "\nOpening " << rtmidi->getPortName() << std::endl;
    }
    else {
        for ( ix=0; ix<nPorts; ix++ ) {
            portName = rtmidi->getPortName(ix);
            std::cout << "  Input port #" << ix << ": " << portName << '\n';
        }

        do {
            std::cout << "\nChoose a port number: ";
            std::cin >> ix;
        } while ( ix >= nPorts );
        std::string keyHit;
        std::getline( std::cin, keyHit );  // used to clear out stdin
    }

    rtmidi->openPort( ix );

  return true;
}

/**
 * @brief midiInCallback
 * @param[in]  deltatime
 * @param[in]  message
 * @param[in]  userData
 */
static void midiInCallback( double deltatime, std::vector< unsigned char > *message, void * userData )
{
    const unsigned int nBytes = message->size();
    if(nBytes <= 0) { return; }

    int const nKind = message->at(0) & 0xF0;

    switch( nKind ) {
        case 0x80:  // ノートオフ
            volume = 0.0f;
            break;
        case 0x90:  // ノートオン
            float notenum = message->at(1);
            pitch = 440.0 * pow(2.0, (notenum - 69.0) / 12.0);

            float velocity = message->at(2);
            volume = velocity / 127.0f;
            break;
    }

#if 1
    for ( unsigned int ix=0; ix<nBytes; ix++ ) {
        std::cout << "Byte " << ix << " = " << (int)message->at(ix) << ", ";
    }
    if ( nBytes > 0 ) {
        std::cout << "stamp = " << deltatime << std::endl;
    }
#endif
}


/**
 * @brief midiInit
 */
static bool midiInit( void )
{
    RtMidiIn *midiin = NULL;

    // initialize MIDI iunput
    try {
        midiin = new RtMidiIn();

        if ( chooseMidiPort( midiin ) == false ) {
            goto cleanup;
        }
        midiin->setCallback( &midiInCallback, NULL );

        // Don't ignore sysex, timing, or active sensing messages.
        midiin->ignoreTypes( false, false, false );
    } catch ( RtMidiError &error ) {
        error.printMessage();
        return false;
    }
    return true;

cleanup:
    delete midiin;
    return false;
}





/**
 * @brief write_callback
 *
 * @param[out] outstream
 * @param[in]  frame_count_min
 * @param[in]  frame_count_max
 */
static const float PI             = 3.1415926535f;
static float       seconds_offset = 0.0f;
static void write_callback(
    struct SoundIoOutStream *outstream,
    int                      frame_count_min,
    int                      frame_count_max
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

        float radians_per_second = pitch * 2.0f * PI;
        for (int frame = 0; frame < frame_count; frame += 1) {
            float sample = sin((seconds_offset + frame * seconds_per_frame) * radians_per_second) * volume;
            for (int ch = 0; ch < layout->channel_count; ch += 1) {
                float *ptr = (float*)(areas[ch].ptr + areas[ch].step * frame);
                *ptr = sample;
            }
        }
        seconds_offset = fmod(seconds_offset + seconds_per_frame * frame_count, 1.0);

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

    if (outstream->layout_error)
        fprintf(stderr, "unable to set channel layout: %s\n", soundio_strerror(outstream->layout_error));

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

