#ifndef __MP3AUDIO_H
#define __MP3AUDIO_H

#include "stm32746g_discovery_ts.h"
#include "term_io.h"
#include "stm32746g_discovery_audio.h"
#include "pub/mp3dec.h"
#include "pub/mp3common.h"
#include "fatfs.h"
#include <sys/unistd.h>
#include "usb_host.h"
#include "wm8994/wm8994.h"
#include "stm32f7xx_hal.h"
#include "screen_refresh.h"

#define FILE_BUFFER_SIZE 8192
#define DMA_BUFFER_SIZE 8192

uint8_t volume;

char file_buff[FILE_BUFFER_SIZE];
short processing_buff[DMA_BUFFER_SIZE / 2];
char dma_buff[DMA_BUFFER_SIZE];

int bytesLeft;
char *file_buff_ptr;
short *processing_buff_ptr;
int processing_buff_offs;

HMP3Decoder hMP3Decoder;
MP3FrameInfo mp3FrameInfo;
char FILES[20][100];
int FILE_COUNTER;
int CURRENT_FILE;
FIL file;

enum
{
  BUFFER_OFFSET_NONE = 0,
  BUFFER_OFFSET_HALF,
  BUFFER_OFFSET_FULL,
};

extern ApplicationTypeDef Appli_state;
uint8_t player_state;
uint8_t dma_buff_offs;

void read_directory();
void process_callback(int dma_offset);
void play_directory();

#endif