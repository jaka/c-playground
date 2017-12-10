
/*
   apt-get install gcc libc6-dev libasound2-dev alsa-utils
   gcc -lasound -o playwav playwav.c
*/

#include <alsa/asoundlib.h>
#include <alsa/pcm.h>

#include <opus.h>

#define PCM_DEVICE "default"

#include <stdio.h>
#include <stdlib.h>
#include <string.h> // for memcmp
#include <stdint.h> // for int16_t and int32_t

#define SAMPLE_RATE 48000
#define CHANNELS 2
#define BITRATE 64000

struct wavfile {
   char    id[4];        // should always contain "RIFF"
   int32_t totallength;  // total file length minus 8
   char    wavefmt[8];   // should be "WAVEfmt "
   int32_t format;       // 16 for PCM format
   int16_t pcm;          // 1 for PCM format
   int16_t channels;     // channels
   int32_t samples_per_second;  // sampling frequency
   int32_t bytes_per_second;
   int16_t bytes_by_capture;
   int16_t bits_per_sample;
   char    data[4];      // should always contain "data"
   int32_t bytes_in_data;
};

int is_big_endian(void) {
  union {
    uint32_t i;
    char c[4];
  } bint = {0x01000000};
  return bint.c[0]==1;
}

int wav_read_header(FILE *wav, struct wavfile *header) {

  /* Read header */
  if ( fread(header, sizeof(struct wavfile), 1, wav) < 1 ) {
    fprintf(stderr, "Can't read the file header\n");
    return -1;
  }

  /* If WAV file isn't the same endianness than the current environment we quit */
  if ( is_big_endian() ) {
    if ( memcmp(header->id, "RIFX", 4) != 0 ) {
      fprintf(stderr, "ERROR: This is not a big endian WAV file\n");
      return -1;
    }
  }
  else {
    if ( memcmp(header->id, "RIFF", 4) != 0 ) {
      fprintf(stderr, "ERROR: This is not a little endian WAV file\n");
      return -1;
    }
  }

  if ( memcmp(header->wavefmt, "WAVEfmt ", 8) != 0 || memcmp(header->data, "data", 4) != 0 ) {
    fprintf(stderr,"ERROR: Not WAV format\n");
    return -1;
  }

  if ( header->format != 16 ) {
    fprintf(stderr, "\nERROR: not 16 bit WAV format.");
    return -1;
  }

  if ( memcmp( header->data, "data", 4) != 0 ) {
    fprintf(stderr, "ERROR: Prrroblem?\n");
    return -1;
  }

  return 0;
}

int wav_read_data(FILE *wav, char* buff, const int buff_size) {

  return fread(buff, buff_size, 1, wav);

}

int main(int argc, char **argv) {

  unsigned int pcm, tmp;
  snd_pcm_t *pcm_handle;
  snd_pcm_hw_params_t *params;
  snd_pcm_uframes_t frames;

  char *filename;
  FILE *wav;
  
  struct wavfile header;
  unsigned int rate, channels, seconds;
  
  char *buff;
  int buff_size;
  
  int loops;

  if ( argc < 2 ) {
    fprintf(stderr, "Usage: %s <filename>\n", argv[0]);
    return -1;
  }

  filename = strdup(argv[1]);
  printf("Filename: %s\n", filename);

  wav = fopen(filename, "rb");

  if ( !wav ) {
    fprintf(stderr, "Can't open input file %s\n", filename);
    return 1;
  }

  if ( wav_read_header(wav, &header) < 0 ) {
    fprintf(stderr, "Cannot read file header");
    return 1;
  }

  fprintf(stderr, "%d bytes per sec \n", header.bytes_per_second );

  printf("bpd %d\n", header.bytes_in_data);

  /* */

  rate = header.samples_per_second;
  channels = header.channels;
  seconds = (int)(header.bytes_in_data / header.bytes_per_second);
  
  /*Create a new encoder state */
  /* encoder = opus_encoder_create(SAMPLE_RATE, 2, OPUS_APPLICATION_AUDIO, &err); */
  
  if (err<0)
   {
      fprintf(stderr, "failed to create an encoder: %s\n", opus_strerror(err));
      return EXIT_FAILURE;
   }

  if ( (pcm = snd_pcm_open(&pcm_handle, PCM_DEVICE, SND_PCM_STREAM_PLAYBACK, 0)) < 0 ) {
    printf("ERROR: Can't open \"%s\" PCM device. %s\n", PCM_DEVICE, snd_strerror(pcm));
    return 1;
  }

  /* Set parameters */
  snd_pcm_hw_params_alloca(&params);
  snd_pcm_hw_params_any(pcm_handle, params);

  if ( (pcm = snd_pcm_hw_params_set_access(pcm_handle, params, SND_PCM_ACCESS_RW_INTERLEAVED)) < 0 )
    printf("ERROR: Can't set interleaved mode. %s\n", snd_strerror(pcm));

  if ( (pcm = snd_pcm_hw_params_set_format(pcm_handle, params, SND_PCM_FORMAT_S16_LE)) < 0)
    printf("ERROR: Can't set format. %s\n", snd_strerror(pcm));

  if ( (pcm = snd_pcm_hw_params_set_channels(pcm_handle, params, channels)) < 0 )
    printf("ERROR: Can't set channels number. %s\n", snd_strerror(pcm));

  if ( (pcm = snd_pcm_hw_params_set_rate_near(pcm_handle, params, &rate, 0)) < 0 )
    printf("ERROR: Can't set rate. %s\n", snd_strerror(pcm));

  /* Write parameters */
  if ( (pcm = snd_pcm_hw_params(pcm_handle, params)) < 0 )
    printf("ERROR: Can't set harware parameters. %s\n", snd_strerror(pcm));

  /* Read state */
  printf("PCM name: '%s'\n", snd_pcm_name(pcm_handle));
  printf("PCM state: %s\n", snd_pcm_state_name(snd_pcm_state(pcm_handle)));

  snd_pcm_hw_params_get_channels(params, &tmp);
  printf("PCM channels: %i ", tmp);

  if (tmp == 1)
    printf("(mono)\n");
  else if (tmp == 2)
    printf("(stereo)\n");

  snd_pcm_hw_params_get_rate(params, &tmp, 0);
  printf("rate: %d bps\n", tmp);

  printf("seconds: %d\n", seconds);

  /* Allocate buffer to hold single period */
  snd_pcm_hw_params_get_period_size(params, &frames, 0);

  printf("frames: %d\n", frames);

  buff_size = frames * channels * 2 /* 2 -> sample size */;
  buff = (char *) malloc(buff_size);

  snd_pcm_hw_params_get_period_time(params, &tmp, NULL);

  printf("tmp: %d\n", tmp);

  for (loops = header.bytes_in_data / buff_size; loops > 0; loops--) {

    if ( !(pcm = wav_read_data(wav, buff, buff_size)) ) {

	  printf("Early end of file.\n");
	  break;
    }

	if ( (pcm = snd_pcm_writei(pcm_handle, buff, frames)) == -EPIPE ) {
	  printf("XRUN.\n");
	  snd_pcm_prepare(pcm_handle);
	} else if (pcm < 0) {
	  printf("ERROR. Can't write to PCM device. %s\n", snd_strerror(pcm));
	}

  }

  printf("%f\n", header.bytes_in_data / header.bytes_per_second);

  snd_pcm_drain(pcm_handle);
  snd_pcm_close(pcm_handle);
  free(buff);

  fclose(wav);
  free(filename);

  return 0;

}
