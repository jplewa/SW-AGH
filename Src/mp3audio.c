#include "mp3audio.h"

void process_callback(int dma_offset)
{
    while (processing_buff_offs < DMA_BUFFER_SIZE / 4)
    {
        int offset = MP3FindSyncWord((unsigned char *)file_buff_ptr, bytesLeft);
        bytesLeft -= offset;
        file_buff_ptr += offset;
        int err = MP3Decode(hMP3Decoder, (unsigned char **)&file_buff_ptr, (int *)&bytesLeft, processing_buff_ptr, 0);
        //if (err)
            ////xprintf("BLAD: %d\n", err);
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
        ////xprintf("ups\n");
    }
    file_buff_ptr = file_buff;
    bytesLeft += br;
    processing_buff_offs -= DMA_BUFFER_SIZE / 4;
    processing_buff_ptr = processing_buff + processing_buff_offs;
    dma_buff_offs = BUFFER_OFFSET_NONE;
}

void play_file(char *file_name)
{
    volume = 30;
    dma_buff_offs = BUFFER_OFFSET_NONE;
    FRESULT res;

    res = f_open(&file, file_name, FA_READ);

    if (res == FR_OK)
    {
        xprintf("mp3 file open OK\n");
    }
    else
    {
        //xprintf("mp3 file open ERROR, res = %d\n", res);
        while (1)
            ;
    }

    file_buff_ptr = file_buff;
    if (f_read(&file, file_buff_ptr, FILE_BUFFER_SIZE, (void *)&bytesLeft) != F_OK)
    {
        ////xprintf("ups\n");
    }
    processing_buff_ptr = processing_buff;
    /* Infinite loop */
    for (;;)
    {
        /*
        char key = inkey();
        switch (key)
        {
        case 'a':
            BSP_AUDIO_OUT_SetVolume(volume < 100 ? ++volume : volume);
            break;
        case 'z':
            BSP_AUDIO_OUT_SetVolume(volume > 0 ? --volume : volume);
            break;
        case 'r':
            BSP_AUDIO_OUT_Resume();
            break;
        case 'p':
            BSP_AUDIO_OUT_Pause();
            break;
        }*/

        BSP_TS_GetState(&TS_State);
        if (TS_State.touchDetected)
        {
            if ((TS_State.touchX[0] < 290) && (TS_State.touchX[0] > 190) && (TS_State.touchY[0] < 186) && (TS_State.touchY[0] > 86))
                {
                //xprintf("play command...\n");
                if (player_state)
                {
                    //xprintf("already playing\n");
                }
                else
                {
                    player_state = 1;
                    BSP_AUDIO_OUT_Play((uint16_t *)&dma_buff[0], DMA_BUFFER_SIZE);
                    dma_buff_offs = BUFFER_OFFSET_NONE;
                    vTaskDelay(1000);
                }
            }
        }
        if (player_state)
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
                //xprintf("stop at eof\n");
                BSP_AUDIO_OUT_Stop(CODEC_PDWN_SW);
                player_state = 0;
            }
        } //if(player_state)

        vTaskDelay(2);
    }
}