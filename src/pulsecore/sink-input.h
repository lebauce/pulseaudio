#ifndef foosinkinputhfoo
#define foosinkinputhfoo

/* $Id$ */

/***
  This file is part of PulseAudio.
 
  PulseAudio is free software; you can redistribute it and/or modify
  it under the terms of the GNU Lesser General Public License as published
  by the Free Software Foundation; either version 2 of the License,
  or (at your option) any later version.
 
  PulseAudio is distributed in the hope that it will be useful, but
  WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
  General Public License for more details.
 
  You should have received a copy of the GNU Lesser General Public License
  along with PulseAudio; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
  USA.
***/

#include <inttypes.h>

typedef struct pa_sink_input pa_sink_input;

#include <pulse/sample.h>
#include <pulsecore/sink.h>
#include <pulsecore/memblockq.h>
#include <pulsecore/resampler.h>
#include <pulsecore/module.h>
#include <pulsecore/client.h>

typedef enum pa_sink_input_state {
    PA_SINK_INPUT_RUNNING,      /*< The stream is alive and kicking */
    PA_SINK_INPUT_DRAINED,      /*< The stream stopped playing because there was no data to play */
    PA_SINK_INPUT_CORKED,       /*< The stream was corked on user request */
    PA_SINK_INPUT_DISCONNECTED  /*< The stream is dead */
} pa_sink_input_state_t;

struct pa_sink_input {
    int ref;
    uint32_t index;
    pa_sink_input_state_t state;
    
    char *name, *driver;                /* may be NULL */
    pa_module *owner;                   /* may be NULL */  

    pa_sink *sink;
    pa_client *client;                  /* may be NULL */
    
    pa_sample_spec sample_spec;
    pa_channel_map channel_map;

    pa_cvolume volume;

    /* Some silence to play before the actual data. This is used to
     * compensate for latency differences when moving a sink input
     * "hot" between sinks. */
    size_t move_silence;
    
    int (*peek) (pa_sink_input *i, pa_memchunk *chunk);
    void (*drop) (pa_sink_input *i, const pa_memchunk *chunk, size_t length);
    void (*kill) (pa_sink_input *i);             /* may be NULL */
    pa_usec_t (*get_latency) (pa_sink_input *i); /* may be NULL */
    void (*underrun) (pa_sink_input *i);         /* may be NULL */

    void *userdata;

    pa_memchunk resampled_chunk;
    pa_resampler *resampler;                     /* may be NULL */

    int variable_rate;
    pa_resample_method_t resample_method;

    pa_memblock *silence_memblock;               /* may be NULL */
};

pa_sink_input* pa_sink_input_new(
    pa_sink *s,
    const char *driver,
    const char *name,
    const pa_sample_spec *spec,
    const pa_channel_map *map,
    const pa_cvolume *volume,
    int variable_rate,
    pa_resample_method_t resample_method);

void pa_sink_input_unref(pa_sink_input* i);
pa_sink_input* pa_sink_input_ref(pa_sink_input* i);

/* To be called by the implementing module only */
void pa_sink_input_disconnect(pa_sink_input* i);

/* External code may request disconnection with this funcion */
void pa_sink_input_kill(pa_sink_input*i);

pa_usec_t pa_sink_input_get_latency(pa_sink_input *i);

int pa_sink_input_peek(pa_sink_input *i, pa_memchunk *chunk, pa_cvolume *volume);
void pa_sink_input_drop(pa_sink_input *i, const pa_memchunk *chunk, size_t length);

void pa_sink_input_set_volume(pa_sink_input *i, const pa_cvolume *volume);
const pa_cvolume * pa_sink_input_get_volume(pa_sink_input *i);

void pa_sink_input_cork(pa_sink_input *i, int b);

void pa_sink_input_set_rate(pa_sink_input *i, uint32_t rate);

void pa_sink_input_set_name(pa_sink_input *i, const char *name);

pa_resample_method_t pa_sink_input_get_resample_method(pa_sink_input *i);

int pa_sink_input_move_to(pa_sink_input *i, pa_sink *dest, int immediately);

#endif
