#include "mp3audio.h"

void read_directory()
{
    FRESULT res;
    DIR dir;
    static FILINFO fno;
    char* path = "1:";
    FILE_COUNTER = 0;
    CURRENT_FILE = 0;
    res = f_opendir(&dir, path);
    if (res == F_OK)
    {
        for (;;) {
            res = f_readdir(&dir, &fno);                   
            if (res != FR_OK || fno.fname[0] == 0) break;                                
            for (int i = 0; fno.fname[i] != '\0'; i++){
                if ((fno.fname[i] == '.') && (fno.fname[i + 1] == 'm') && (fno.fname[i + 2] == 'p') && (fno.fname[i + 3] == '3'))
                {
                    memset(FILES[FILE_COUNTER], 0, 100);
                    snprintf(FILES[FILE_COUNTER], (i + 8), "0:/%s", fno.fname);
                    FILE_COUNTER++;
                    break;
                }
            }    
        }
        f_closedir(&dir);
    }
}

void process_callback(int dma_offset)
{
    while (processing_buff_offs < DMA_BUFFER_SIZE / 4)
    {
        int offset = MP3FindSyncWord((unsigned char *)file_buff_ptr, bytesLeft);
        bytesLeft -= offset;
        file_buff_ptr += offset;
        if (MP3Decode(hMP3Decoder, (unsigned char **)&file_buff_ptr, (int *)&bytesLeft, processing_buff_ptr, 0))
        {
            xprintf("ERROR");
        }
        MP3GetLastFrameInfo(hMP3Decoder, &mp3FrameInfo);
        processing_buff_offs += mp3FrameInfo.outputSamps;
        processing_buff_ptr = processing_buff + processing_buff_offs;
    }
    memcpy(dma_buff + dma_offset, processing_buff, DMA_BUFFER_SIZE / 2);
    memcpy(file_buff, file_buff_ptr, bytesLeft);
    memcpy(processing_buff, &processing_buff[DMA_BUFFER_SIZE / 4], (processing_buff_offs - DMA_BUFFER_SIZE / 4) * 2);
    file_buff_ptr = file_buff + bytesLeft;
    int br;
    if (f_read(&file, file_buff_ptr, (FILE_BUFFER_SIZE - bytesLeft), (void *)&br) != F_OK)
    {
            xprintf("ERROR");
    }
    file_buff_ptr = file_buff;
    bytesLeft += br;
    processing_buff_offs -= DMA_BUFFER_SIZE / 4;
    processing_buff_ptr = processing_buff + processing_buff_offs;
    dma_buff_offs = BUFFER_OFFSET_NONE;
}

void start_reading_file()
{  
    if (f_open(&file, FILES[CURRENT_FILE], FA_READ) != FR_OK)
    {
        xprintf("ERROR: %s\n", FILES[CURRENT_FILE]);
        while (1) {};
    }
    file_buff_ptr = file_buff;
    if (f_read(&file, file_buff_ptr, FILE_BUFFER_SIZE, (void *)&bytesLeft) != F_OK)
    {
        xprintf("ERROR\n");
    }
    bytesLeft = FILE_BUFFER_SIZE;

    processing_buff_ptr = processing_buff;
    file_buff_ptr = file_buff;
    processing_buff_ptr = processing_buff;
    dma_buff_offs = BUFFER_OFFSET_NONE;
    BSP_AUDIO_OUT_Play((uint16_t *)&dma_buff[0], DMA_BUFFER_SIZE);
}

void play_directory()
{
    volume = 30;
    start_reading_file();

    while(1){
//
        /* Infinite loop */
        for (;;)
        {
            if (touch_sound_communication == STOP_PRESSED)
            {
                xprintf("STOP_PRESSED\n");
                BSP_AUDIO_OUT_Stop(CODEC_PDWN_SW);
                touch_sound_communication = STOPPED;
                memset(dma_buff, 0, DMA_BUFFER_SIZE);            
                memset(file_buff, 0, FILE_BUFFER_SIZE);            
                memset(processing_buff, 0, DMA_BUFFER_SIZE);        
                f_close(&file);
            }
            else if (touch_sound_communication == PLAY_PRESSED)
            {
                xprintf("PLAY_PRESSED\n");
                start_reading_file();
                touch_sound_communication = PLAYING;
                dma_buff_offs = BUFFER_OFFSET_NONE;
                //vTaskDelay(1000);
            }
            else if (touch_sound_communication == PAUSE_PRESSED)
            {
                xprintf("PLAY_PRESSED\n");
                touch_sound_communication = PAUSED;
                BSP_AUDIO_OUT_Pause();
            }
            else if (touch_sound_communication == RESUME_PRESSED)
            {
                xprintf("RESUME_PRESSED\n");
                touch_sound_communication = PLAYING;
                BSP_AUDIO_OUT_Resume();
            }
            else if (touch_sound_communication == VOL_UP_PRESSED)
            {
                xprintf("VOL_UP_PRESSED\n");
                touch_sound_communication = PLAYING;
                BSP_AUDIO_OUT_SetVolume(volume<100 ? ++volume : volume);
            }
            else if (touch_sound_communication == VOL_DOWN_PRESSED)
            {
                xprintf("VOL_DOWN_PRESSED\n");
                touch_sound_communication = PLAYING;
                BSP_AUDIO_OUT_SetVolume(volume>0 ? --volume : volume);   
            }

            else if (touch_sound_communication == NEXT_PRESSED_PLAYING)
            {
                xprintf("NEXT_PRESSED_PLAYING\n");
                ///BSP_AUDIO_OUT_Pause();
                BSP_AUDIO_OUT_Stop(CODEC_PDWN_SW);
                memset(dma_buff, 0, DMA_BUFFER_SIZE);            
                memset(file_buff, 0, FILE_BUFFER_SIZE);            
                memset(processing_buff, 0, DMA_BUFFER_SIZE); 
                //vTaskDelay(100);
                //BSP_AUDIO_OUT_Stop(CODEC_PDWN_SW);
                //vTaskDelay(100);
                CURRENT_FILE = (CURRENT_FILE + 1) % FILE_COUNTER;
                f_close(&file);
                start_reading_file();
                BSP_AUDIO_OUT_Play((uint16_t *)&dma_buff[0], DMA_BUFFER_SIZE);
                //BSP_AUDIO_OUT_Resume();
                touch_sound_communication = PLAYING;
                //for (int i = 0; i < 10; i++){
                //    int a = 1;
                //}
                xprintf("TUTAJ");
                //continue;
            }

            else if (touch_sound_communication == NEXT_PRESSED_PAUSED)
            {
                xprintf("NEXT_PRESSED_PAUSED\n");
                memset(dma_buff, 0, DMA_BUFFER_SIZE);            
                memset(file_buff, 0, FILE_BUFFER_SIZE);            
                memset(processing_buff, 0, DMA_BUFFER_SIZE); 
                BSP_AUDIO_OUT_Stop(CODEC_PDWN_SW);
                CURRENT_FILE = (CURRENT_FILE + 1) % FILE_COUNTER;
                f_close(&file);
                start_reading_file();
                dma_buff_offs = BUFFER_OFFSET_NONE;
                BSP_AUDIO_OUT_Play((uint16_t *)&dma_buff[0], DMA_BUFFER_SIZE);
                touch_sound_communication = PAUSED;
                //continue;
            }

            else if (touch_sound_communication == NEXT_PRESSED_STOPPED)
            {
                xprintf("NEXT_PRESSED_STOPPED\n");
                touch_sound_communication = STOPPED;
                CURRENT_FILE = (CURRENT_FILE + 1) % FILE_COUNTER;
                //continue;
            }

            else if (touch_sound_communication == PREV_PRESSED_PLAYING)
            {
                xprintf("PREV_PRESSED_PLAYING\n");
                BSP_AUDIO_OUT_Pause();
                memset(dma_buff, 0, DMA_BUFFER_SIZE);            
                memset(file_buff, 0, FILE_BUFFER_SIZE);            
                memset(processing_buff, 0, DMA_BUFFER_SIZE); 
                BSP_AUDIO_OUT_Stop(CODEC_PDWN_SW);
                touch_sound_communication = PLAYING;
                CURRENT_FILE--;
                if (CURRENT_FILE < 0) CURRENT_FILE += FILE_COUNTER;
                f_close(&file);
                BSP_AUDIO_OUT_Resume();
                //continue;
            }

            else if (touch_sound_communication == PREV_PRESSED_PAUSED)
            {
                xprintf("PREV_PRESSED_PAUSED\n");
                memset(dma_buff, 0, DMA_BUFFER_SIZE);            
                memset(file_buff, 0, FILE_BUFFER_SIZE);            
                memset(processing_buff, 0, DMA_BUFFER_SIZE); 
                BSP_AUDIO_OUT_Stop(CODEC_PDWN_SW);
                touch_sound_communication = PLAY_PRESSED;
                CURRENT_FILE--;
                if (CURRENT_FILE < 0) CURRENT_FILE += FILE_COUNTER;
                f_close(&file);
                //continue;
            }

            else if (touch_sound_communication == PREV_PRESSED_STOPPED)
            {
                xprintf("PREV_PRESSED_STOPPED");
                touch_sound_communication = STOPPED;
                if (CURRENT_FILE < 0) CURRENT_FILE += FILE_COUNTER;
                //continue;
            }

            else if (touch_sound_communication == PLAYING)
            {
                if (dma_buff_offs == BUFFER_OFFSET_HALF)
                {
                    process_callback(0);
                }

                if (dma_buff_offs == BUFFER_OFFSET_FULL)
                {
                    process_callback(DMA_BUFFER_SIZE / 2);
                }

                if (bytesLeft == 0)
                {
                    BSP_AUDIO_OUT_Stop(CODEC_PDWN_SW);
                    memset(dma_buff, 0, DMA_BUFFER_SIZE);            
                    memset(file_buff, 0, FILE_BUFFER_SIZE);            
                    memset(processing_buff, 0, DMA_BUFFER_SIZE); 
                    touch_sound_communication = PLAY_PRESSED;
                    CURRENT_FILE = (CURRENT_FILE + 1) % 100;
                    //continue;               
                }
            }

            vTaskDelay(2);
        }
    }
}   