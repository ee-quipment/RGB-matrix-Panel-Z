/*******************************************************************************

    led_panel_demo.ino - Test different configurations of Feathers, Panels, and drivers.

    This is a collection of tests for different configuration of Arduino
    Zero type boards and LED RGB matrix panels.

    To configure a specific test, #define the desired panel and library.
    The Adafruit RGBmatrixPanel library only supports the Feather M0.
    Compile and run.

    COPYRIGHT NOTICE: (c) 2018 ee-quipment.com
        
******************************************************************************/


/*******************************************************************************

    Define which tests to run

******************************************************************************/
//#define PANEL_16x32
#define PANEL_32x32
//#define PANEL_64x32
//#define PANEL_96x32

#define LIB_LEDPANEL
//#define LIB_RGBMATRIXPANEL

#if defined (LIB_RGBMATRIXPANEL) && defined (ARDUINO_SAMD_MKRZERO)
#error("RGBmatrixPanel does not support the Arduino MKRZero")
#endif


#ifdef PANEL_16x32
#define PANEL_WIDTH   32
#define PANEL_HEIGHT  16
#endif

#ifdef PANEL_32x32
#define PANEL_WIDTH   32
#define PANEL_HEIGHT  32
#endif

#ifdef PANEL_64x32
#define PANEL_WIDTH   64
#define PANEL_HEIGHT  32
#endif

#ifdef PANEL_96x32
#define PANEL_WIDTH   96
#define PANEL_HEIGHT  32
#endif


const unsigned char splash_screen_32x32 [] = {
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0F, 0xFF, 0xFF, 0xF0, 0x3F, 0xFF, 0xFF, 0xFC,
0x78, 0x00, 0x00, 0x1E, 0x60, 0x00, 0x00, 0x06, 0xE0, 0x00, 0x00, 0x07, 0xC7, 0xFE, 0x00, 0x03,
0xCF, 0xFF, 0x38, 0xE3, 0xCF, 0xFF, 0x45, 0x13, 0xCF, 0xFF, 0x79, 0xE3, 0xCB, 0x9D, 0x41, 0x03,
0xCD, 0x6B, 0x38, 0xE3, 0xCE, 0xF7, 0x00, 0x03, 0xCF, 0xFF, 0x00, 0x03, 0xCF, 0xFF, 0x22, 0x23,
0xC7, 0xFE, 0x00, 0x03, 0xE0, 0x00, 0x00, 0x07, 0x60, 0x00, 0x00, 0x06, 0x78, 0x00, 0x00, 0x1E,
0x3F, 0xFF, 0xFF, 0xFC, 0x0F, 0xFF, 0xFF, 0xF0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

#include <Arduino.h>
#include <Adafruit_GFX.h>

#ifdef LIB_LEDPANEL
#include <LEDpanel.h>
LEDpanel    panel_hdwe(PANEL_WIDTH, PANEL_HEIGHT);
GFXcanvas8  panel(PANEL_WIDTH, PANEL_HEIGHT);
#endif

#ifdef LIB_RGBMATRIXPANEL
#include <RGBmatrixPanel.h>
// map colors to LEDpanel palette
#define RED       panel.Color333(7, 0, 0)
#define GREEN     panel.Color333(0, 7, 0)
#define BLUE      panel.Color333(0, 0, 7)
#define YELLOW    panel.Color333(7, 7, 0)
#define CYAN      panel.Color333(0, 7, 7)
#define MAGENTA   panel.Color333(7, 0, 7)
#define WHITE     panel.Color333(7, 7, 7)
#define BLACK     panel.Color333(0, 0, 0)
RGBmatrixPanel panel(0, 0, 0, 0, 0, 0, 0, false);    // pin assignments are hard-coded in driver
#endif


void setup() {

    #ifdef LIB_RGBMATRIXPANEL
        panel.begin();
    #endif

    #ifdef LIB_LEDPANEL
        panel_hdwe.begin(panel.getBuffer());
    #endif
        
    panel.drawBitmap(0, 0, splash_screen_32x32, 32, 32, BLUE);
    // allow some time to assess image
    delay(10000);
    
    // fill screen with green
    panel.fillScreen(GREEN);
    
    // outline display in yellow
    panel.drawRect(0, 0, PANEL_WIDTH, PANEL_HEIGHT, YELLOW);
    
    // draw a red 'x' across the panel
    panel.drawLine(1, 1, PANEL_WIDTH-2, PANEL_HEIGHT-2, RED);
    panel.drawLine(PANEL_WIDTH-2, 1, 1, PANEL_HEIGHT-2, RED);
    
    // put blue circles in the X quadrants
    panel.drawCircle(PANEL_WIDTH/2, PANEL_HEIGHT/4, PANEL_HEIGHT/8, BLUE);
    panel.drawCircle(PANEL_WIDTH/2, (3*PANEL_HEIGHT)/4, PANEL_HEIGHT/8, BLUE);
    panel.drawCircle(PANEL_WIDTH/4, PANEL_HEIGHT/2, PANEL_HEIGHT/8, BLUE);
    panel.drawCircle((3*PANEL_WIDTH)/4, PANEL_HEIGHT/2, PANEL_HEIGHT/8, BLUE);
    
    // 2 pixel high rainbow stripe along bottom
    int x = 1;
    int y = PANEL_HEIGHT-3; // sits above yellow outline pixel
    int bar_width = (PANEL_WIDTH - 2) / 6; // 6 bars allowing for 2 yellow pixels from outline
    panel.fillRect(x, y, bar_width, 2, RED);
    panel.fillRect(x += bar_width, y, bar_width, 2, YELLOW);
    panel.fillRect(x += bar_width, y, bar_width, 2, GREEN);
    panel.fillRect(x += bar_width, y, bar_width, 2, CYAN);
    panel.fillRect(x += bar_width, y, bar_width, 2, BLUE);
    panel.fillRect(x += bar_width, y, bar_width, 2, MAGENTA);
    panel.fillRect(x += bar_width, y, PANEL_WIDTH-x-1, 2, WHITE);
    
    // allow some time to assess image
    delay(10000);
}

void loop() {
    /*
     * Font is 6 x 8
     */
    static uint32_t min, sec, last_second = 0;
    
    sec = (millis() / 1000) % 60;
    
    if (sec != last_second) {
        last_second = sec;
        min = min((millis() / 1000) / 60, 99);
        
        panel.fillScreen(BLACK);
        panel.setTextColor(WHITE); 
        
        panel.setCursor( PANEL_WIDTH/2 - 12, PANEL_HEIGHT/2 - 8);
        panel.print("Time");
        
        panel.setCursor( PANEL_WIDTH/2 - 15, PANEL_HEIGHT/2 + 1);
        if (min < 10) { panel.print('0'); }
        panel.print(min);
        panel.print(':');
        if (sec < 10) { panel.print('0'); }
        panel.print(sec);
    }
}
