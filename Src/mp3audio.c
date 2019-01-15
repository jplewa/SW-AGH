#include "mp3audio.h"

int init_mp3()
{
    redraw_title = 0;

    volume = 30;

    if (BSP_AUDIO_OUT_Init(OUTPUT_DEVICE_HEADPHONE1, volume, AUDIO_FREQUENCY_44K) != AUDIO_OK)
    {
        xprintf("ERROR: Failed to configure the audio peripherals\n");
        return -1;
    }

    BSP_AUDIO_OUT_SetAudioFrameSlot(CODEC_AUDIOFRAME_SLOT_02);

    hMP3Decoder = MP3InitDecoder();

    if (hMP3Decoder == 0)
    {
        xprintf("ERROR: Failed to initialize the MP3 decoder\n");
        return -1;
    }

    return 0;
}

int read_directory(char *path)
{
    FILE_COUNTER = 0;
    CURRENT_FILE = 0;

    DIR dir;
    static FILINFO fno;

    FRESULT res = f_opendir(&dir, path);
    if (res != F_OK)
    {
        xprintf("ERROR: Failed to open directory %s\n", path);
        return -1;
    }
    res = f_readdir(&dir, &fno);
    if (res != F_OK)
    {
        xprintf("ERROR: Failed to read directory %s\n", path);
        return -1;
    }
    while (res == FR_OK && fno.fname[0] != 0 && FILE_COUNTER < NUMBER_OF_FILES)
    {
        for (int i = 0; fno.fname[i] != '\0'; i++)
        {
            if ((fno.fname[i] == '.') && (fno.fname[i + 1] == 'm') && (fno.fname[i + 2] == 'p') && (fno.fname[i + 3] == '3'))
            {
                if (f_open(&file, fno.fname, FA_READ) == FR_OK)
                {
                    memset(FILES[FILE_COUNTER], 0, FILE_NAME_LENGTH);
                    snprintf(FILES[FILE_COUNTER], (i + 8), "%s%s", path, fno.fname);
                    FILE_COUNTER++;
                    f_close(&file);
                }
            }
        }
        res = f_readdir(&dir, &fno);
    }
    if (f_closedir(&dir) != F_OK)
    {
        xprintf("ERROR: Failed to close directory %s\n", path);
        return -1;
    }
    return 0;
}

int process_callback(int dma_offset)
{
    int bytes_read, offset;

    while (processing_buff_offs < DMA_BUFFER_SIZE / 4)
    {
        offset = MP3FindSyncWord((unsigned char *)file_buff_ptr, bytes_left);
        if (offset == -1)
        {
            bytes_left = 0;
            return 0;
        }
        bytes_left -= offset;
        file_buff_ptr += offset;
        if (MP3Decode(hMP3Decoder, (unsigned char **)&file_buff_ptr, (int *)&bytes_left, processing_buff_ptr, 0))
        {
            xprintf("ERROR: Failed to decode the next frame\n");
            return -1;
        }
        MP3GetLastFrameInfo(hMP3Decoder, &mp3FrameInfo);
        processing_buff_offs += mp3FrameInfo.outputSamps;
        processing_buff_ptr = processing_buff + processing_buff_offs;
    }

    memcpy(dma_buff + dma_offset, processing_buff, DMA_BUFFER_SIZE / 2);
    memcpy(file_buff, file_buff_ptr, bytes_left);
    memcpy(processing_buff, &processing_buff[DMA_BUFFER_SIZE / 4], (processing_buff_offs - DMA_BUFFER_SIZE / 4) * 2);

    file_buff_ptr = file_buff + bytes_left;

    if (f_read(&file, file_buff_ptr, (FILE_BUFFER_SIZE - bytes_left), (void *)&bytes_read) != F_OK)
    {
        xprintf("ERROR: Failed to read from file\n");
        return -1;
    }

    file_buff_ptr = file_buff;
    bytes_left += bytes_read;
    processing_buff_offs -= DMA_BUFFER_SIZE / 4;
    processing_buff_ptr = processing_buff + processing_buff_offs;
    dma_buff_offs = BUFFER_OFFSET_NONE;

    return 0;
}

int next_file()
{
    return (CURRENT_FILE + 1) % FILE_COUNTER;
}

int prev_file()
{
    int file_number = CURRENT_FILE - 1;
    if (file_number < 0) 
    {
        file_number += FILE_COUNTER;
    }
    return file_number;
}

int start_reading_file()
{
    if (f_open(&file, FILES[CURRENT_FILE], FA_READ) != FR_OK)
    {
        xprintf("ERROR: Failed to open file %s\n", FILES[CURRENT_FILE]);
        return -1;
    }

    file_buff_ptr = file_buff;

    if (f_read(&file, file_buff_ptr, FILE_BUFFER_SIZE, (void *)&bytes_left) != F_OK)
    {
        xprintf("ERROR: Failed to read from file %s\n", FILES[CURRENT_FILE]);
        return -1;
    }

    processing_buff_ptr = processing_buff;
    processing_buff_offs = 0;

    dma_buff_offs = BUFFER_OFFSET_NONE;

    if (BSP_AUDIO_OUT_Play((uint16_t *)&dma_buff[0], DMA_BUFFER_SIZE) != AUDIO_OK)
    {
        xprintf("ERROR: Failed to start the audio stream\n");
        return -1;
    }

    return 0;
}

int mp3_stop()
{
    if (BSP_AUDIO_OUT_Stop(CODEC_PDWN_SW) != AUDIO_OK)
    {
        xprintf("ERROR: Failed to stop the audio stream\n");
        return -1;
    }

    memset(dma_buff, 0, DMA_BUFFER_SIZE);
    memset(file_buff, 0, FILE_BUFFER_SIZE);
    memset(processing_buff, 0, DMA_BUFFER_SIZE);

    if (f_close(&file) != F_OK)
    {
        xprintf("ERROR: Failed to close audio file\n");
        return -1;
    }

    player_state = STOPPED;

    return 0;
}

int mp3_play()
{
    if (start_reading_file())
    {
        return -1;
    }
    player_state = PLAYING;
    return 0;
}

int mp3_pause()
{
    if (BSP_AUDIO_OUT_Pause() != AUDIO_OK)
    {
        xprintf("ERROR: Failed to pause the audio stream\n");
        return -1;
    }
    player_state = PAUSED;
    return 0;
}

int mp3_resume()
{
    if (BSP_AUDIO_OUT_Resume() != AUDIO_OK)
    {
        xprintf("ERROR: Failed to resume the audio stream\n");
        return -1;
    }
    player_state = PLAYING;
    return 0;
}

int mp3_next()
{
    if (mp3_stop())
    {
        return -1;
    }

    CURRENT_FILE = next_file();

    if (start_reading_file())
    {
        return -1;
    }

    return 0;
}

int next_playing()
{
    if (mp3_next())
    {
        return -1;
    }

    player_state = PLAYING;

    return 0;
}

int next_paused()
{
    if (mp3_next())
    {
        return -1;
    }

    player_state = PAUSED;

    return 0;
}

int mp3_prev()
{
    if (mp3_stop())
    {
        return -1;
    }

    CURRENT_FILE = prev_file();

    if (start_reading_file())
    {
        return -1;
    }

    return 0;
}

int prev_playing()
{
    if (mp3_prev())
    {
        return -1;
    }

    player_state = PLAYING;

    return 0;
}

int prev_paused()
{
    if (mp3_prev())
    {
        return -1;
    }

    player_state = PAUSED;

    return 0;
}

int volume_up()
{
    if (BSP_AUDIO_OUT_SetVolume(volume < 100 ? ++volume : volume) != AUDIO_OK)
    {
        xprintf("ERROR: Failed to set audio volume\n");
        return -1;
    }
    player_state = PLAYING;
    return 0;
}

int volume_down()
{
    if (BSP_AUDIO_OUT_SetVolume(volume > 0 ? --volume : volume) != AUDIO_OK)
    {
        xprintf("ERROR: Failed to set audio volume\n");
        return -1;
    }
    player_state = PLAYING;
    return 0;
}

void play_directory()
{
    volume = 30;

    int err = 0;
    
    err = start_reading_file();
    
    if (err)
    {
        return;
    }
    
    redraw_title = 1;

    vTaskDelay(2);

    while (1)
    {
        switch (player_state)
        {
            case PAUSED:
            case STOPPED:
                break;

            case STOP_PRESSED:
                err = mp3_stop();
                break;

            case PLAY_PRESSED:
                err = mp3_play();
                redraw_title = 1;
                break;

            case PAUSE_PRESSED:
                err = mp3_pause();
                break;

            case RESUME_PRESSED:
                err = mp3_resume();
                break;

            case VOL_UP_PRESSED:
                err = volume_up();
                break;

            case VOL_DOWN_PRESSED:
                err = volume_down();
                break;

            case NEXT_PRESSED_PLAYING:
                err = next_playing();
                redraw_title = 1;
                break;

            case NEXT_PRESSED_PAUSED:
                err = next_paused();
                redraw_title = 1;
                break;

            case NEXT_PRESSED_STOPPED:
                CURRENT_FILE = next_file();
                player_state = STOPPED;
                redraw_title = 1;
                break;

            case PREV_PRESSED_PLAYING:
                err = prev_playing();
                redraw_title = 1;
                break;

            case PREV_PRESSED_PAUSED:
                err = prev_paused();
                redraw_title = 1;
                break;

            case PREV_PRESSED_STOPPED:
                FILE_COUNTER = prev_file();
                player_state = STOPPED;
                redraw_title = 1;
                break;

            case PLAYING:
                if (dma_buff_offs == BUFFER_OFFSET_HALF)
                {
                    err = process_callback(0);
                }

                if (dma_buff_offs == BUFFER_OFFSET_FULL)
                {
                    err = process_callback(DMA_BUFFER_SIZE / 2);
                }

                if (bytes_left == 0)
                {
                    err = mp3_stop();
                    if (!err)
                    {
                        CURRENT_FILE = next_file();
                        err = mp3_play();
                        redraw_title = 1;
                    }
                }
                break;

            default:
                break;
        }
        if (err)
        {
            break;
        }
        vTaskDelay(2);
    }
}
