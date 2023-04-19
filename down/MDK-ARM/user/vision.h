#ifndef _VISION_H_
#define _VISION_H_


#define FRAME_HEADER 0x80
#define FRAME_TAIL 0x7F


void vision_Send(void);
void vision_handle(void);

#endif

