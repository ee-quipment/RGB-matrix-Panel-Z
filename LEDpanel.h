/*******************************************************************************

    LEDpanel.h - Draws and refreshes an LED RGB matrix panel display.
                 16, 32, and 64 row panels are supported.

    COPYRIGHT NOTICE: (c) 2018 ee-quipment.com

 ******************************************************************************/


#ifndef LEDpanel_H_
#define LEDpanel_H_

#include  <stdint.h>

#define RED     1
#define GREEN   2
#define BLUE    4
#define YELLOW  (RED + GREEN)
#define CYAN    (GREEN + BLUE)
#define MAGENTA (RED + BLUE)
#define WHITE   (RED + GREEN + BLUE)
#define BLACK   0


class LEDpanel {
  public:
    LEDpanel(int16_t width, int16_t height);

    void      begin(uint8_t * frame_buffer);
};

#endif /* LEDpanel_H_ */
