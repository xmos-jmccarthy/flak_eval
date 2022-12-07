// Copyright (c) 2022 XMOS LIMITED. This Software is subject to the terms of the
// XMOS Public License: Version 1

#include <platform.h>
#include <xs1.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <xcore/hwtimer.h>

#include "share/compat.h"
#include "FLAC/metadata.h"
#include "FLAC/stream_encoder.h"
#include "FLAC/stream_decoder.h"

#define READSIZE 240
#define IN_FILE	 "test_in.wav"
#define OUT_FILE "out.flac"
#define ENCODER_FILE ""

static unsigned total_samples = 0; /* can use a 32-bit number due to WAVE size limitations */
static FLAC__byte buffer[READSIZE/*samples*/ * 2/*bytes_per_sample*/ * 2/*channels*/]; /* we read the WAVE data into here */
static FLAC__int32 pcm[READSIZE/*samples*/ * 2/*channels*/];

static void progress_callback(const FLAC__StreamEncoder *encoder, FLAC__uint64 bytes_written, FLAC__uint64 samples_written, unsigned frames_written, unsigned total_frames_estimate, void *client_data);

int main(int argc, char *argv[])
{
	FLAC__bool ok = true;
	FLAC__StreamEncoder *encoder = 0;
	FLAC__StreamEncoderInitStatus init_status;
#if 0
	FLAC__StreamMetadata *metadata[2];
	FLAC__StreamMetadata_VorbisComment_Entry entry;
#endif
	FILE *fin;
	unsigned sample_rate = 0;
	unsigned channels = 0;
	unsigned bps = 0;

	printf("Start\n");

#if 1
	if((fin = fopen(IN_FILE, "rb")) == NULL) {
		printf("ERROR: opening %s for input\n", IN_FILE);
		return 1;
	}
#endif

	printf("Checkpoint %d\n",__LINE__);
#if 1
	/* read wav header and validate it */
	if(
		fread(buffer, 1, 44, fin) != 44 ||
		memcmp(buffer, "RIFF", 4) //|| 
		// memcmp(buffer+8, "WAVEfmt \020\000\000\000\001\000\002\000", 16) ||
		// memcmp(buffer+32, "\004\000\020\000data", 8)
	) {
		printf("ERROR: invalid/unsupported WAVE file, only 16bps stereo WAVE in canonical form allowed\n");
		fclose(fin);
		return 1;
	}
	sample_rate = 16000; //((((((unsigned)buffer[27] << 8) | buffer[26]) << 8) | buffer[25]) << 8) | buffer[24];
	channels = 2;
	bps = 16;
	total_samples = 5*16*2*16000;//(((((((unsigned)buffer[43] << 8) | buffer[42]) << 8) | buffer[41]) << 8) | buffer[40]) / 4;

	/* allocate the encoder */
	if((encoder = FLAC__stream_encoder_new()) == NULL) {
		printf("ERROR: allocating encoder\n");
		fclose(fin);
		return 1;
	}
#endif

	printf("Checkpoint %d\n",__LINE__);
#if 1
	ok &= FLAC__stream_encoder_set_verify(encoder, false);
	ok &= FLAC__stream_encoder_set_compression_level(encoder, 5);
	ok &= FLAC__stream_encoder_set_channels(encoder, channels);
	ok &= FLAC__stream_encoder_set_bits_per_sample(encoder, bps);
	ok &= FLAC__stream_encoder_set_sample_rate(encoder, sample_rate);
	ok &= FLAC__stream_encoder_set_total_samples_estimate(encoder, total_samples);
#endif

	printf("Checkpoint %d\n",__LINE__);
#if 0
	/* now add some metadata; we'll add some tags and a padding block */
	if(ok) {
		if(
			(metadata[0] = FLAC__metadata_object_new(FLAC__METADATA_TYPE_VORBIS_COMMENT)) == NULL ||
			(metadata[1] = FLAC__metadata_object_new(FLAC__METADATA_TYPE_PADDING)) == NULL ||
			/* there are many tag (vorbiscomment) functions but these are convenient for this particular use: */
			!FLAC__metadata_object_vorbiscomment_entry_from_name_value_pair(&entry, "ARTIST", "Some Artist") ||
			!FLAC__metadata_object_vorbiscomment_append_comment(metadata[0], entry, /*copy=*/false) || /* copy=false: let metadata object take control of entry's allocated string */
			!FLAC__metadata_object_vorbiscomment_entry_from_name_value_pair(&entry, "YEAR", "1984") ||
			!FLAC__metadata_object_vorbiscomment_append_comment(metadata[0], entry, /*copy=*/false)
		) {
			printf("ERROR: out of memory or tag error\n");
			ok = false;
		} else {
			metadata[1]->length = 1234; /* set the padding length */
			ok = FLAC__stream_encoder_set_metadata(encoder, metadata, 2);
		}
	}
#endif

	printf("Checkpoint %d\n",__LINE__);
#if 1
	/* initialize encoder */
	if(ok) {
		init_status = FLAC__stream_encoder_init_file(encoder, OUT_FILE, progress_callback, /*client_data=*/NULL);
		if(init_status != FLAC__STREAM_ENCODER_INIT_STATUS_OK) {
			printf("ERROR: initializing encoder: %s\n", FLAC__StreamEncoderInitStatusString[init_status]);
			ok = false;
		}
	}
#endif

	printf("Checkpoint %d\n",__LINE__);
#if 1
	/* read blocks of samples from WAVE file and feed to encoder */
	if(ok) {
		size_t left = (size_t)total_samples;
		while(ok && left) {
			size_t need = (left>READSIZE? (size_t)READSIZE : (size_t)left);
			if(fread(buffer, channels*(bps/8), need, fin) != need) {
				printf("ERROR: reading from WAVE file\n");
				ok = false;
			}
			else {
				/* convert the packed little-endian 16-bit PCM samples from WAVE into an interleaved FLAC__int32 buffer for libFLAC */
				size_t i;
				for(i = 0; i < need*channels; i++) {
					/* inefficient but simple and works on big- or little-endian machines */
					pcm[i] = (FLAC__int32)(((FLAC__int16)(FLAC__int8)buffer[2*i+1] << 8) | (FLAC__int16)buffer[2*i]);
				}
				/* feed samples to encoder */
				uint32_t start = get_reference_time();
				ok = FLAC__stream_encoder_process_interleaved(encoder, pcm, need);
				uint32_t end = get_reference_time();
				printf("diff: %lu start: %lu end: %lu\n", end-start, start, end);
			}
			left -= need;
		}
	}
	printf("   state: %s\n", FLAC__StreamEncoderStateString[FLAC__stream_encoder_get_state(encoder)]);

	ok &= FLAC__stream_encoder_finish(encoder);

	printf("encoding: %s\n", ok? "succeeded" : "FAILED");
	printf("   state: %s\n", FLAC__StreamEncoderStateString[FLAC__stream_encoder_get_state(encoder)]);

	printf("   FLAC__stream_encoder_get_verify_decoder_state: %s\n", FLAC__StreamDecoderStateString[FLAC__stream_encoder_get_verify_decoder_state(encoder)]);
#endif

	printf("Checkpoint %d\n",__LINE__);
#if 0
	/* now that encoding is finished, the metadata can be freed */
	FLAC__metadata_object_delete(metadata[0]);
	FLAC__metadata_object_delete(metadata[1]);
#endif

	printf("Checkpoint %d\n",__LINE__);
#if 1
	FLAC__stream_encoder_delete(encoder);
	fclose(fin);
#endif

	printf("Done\n");
    return 0;
}

void progress_callback(const FLAC__StreamEncoder *encoder, FLAC__uint64 bytes_written, FLAC__uint64 samples_written, unsigned frames_written, unsigned total_frames_estimate, void *client_data)
{
	(void)encoder, (void)client_data;

	printf("wrote %llu bytes, %llu /%u samples, %u/%u frames\n", bytes_written, samples_written, total_samples, frames_written, total_frames_estimate);
}