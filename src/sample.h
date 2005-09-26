/* wavbreaker - A tool to split a wave file up into multiple wave.
 * Copyright (C) 2002-2005 Timothy Robinson
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#ifndef SAMPLE_H
#define SAMPLE_H

#include "wavbreaker.h"

#define DEFAULT_BUF_SIZE 4096
#define BLOCK_SIZE 2352

#define CD_BLOCK_SIZE                   (2352)
#define CD_BLOCKS_PER_SEC               (75)

typedef struct SampleInfo_ SampleInfo;

struct SampleInfo_ {
    unsigned short  channels;
    unsigned int    samplesPerSec;
    unsigned int    avgBytesPerSec;
    unsigned short  blockAlign;
    unsigned short  bitsPerSample;
    unsigned int    numBytes;
    unsigned int    bufferSize;
};

char *wav_get_error_message();
char *sample_get_error_message();

void sample_init();
void sample_set_audio_dev(char *str);
char * sample_get_audio_dev();

char * sample_get_sample_file();

int play_sample(gulong, gulong *);
void stop_sample();
int sample_open_file(const char *, GraphData *, double *);
void sample_close_file();
void sample_write_files(GList *, WriteInfo *, char *);
gint sample_get_playing();

#endif /* SAMPLE_H*/
