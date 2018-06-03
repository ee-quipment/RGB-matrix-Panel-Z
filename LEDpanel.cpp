/*******************************************************************************

    LEDpanel.cpp - Draws and refreshes the LED panel display.

    Panel size is defined at instantiation. Each pixel is one of 8 colors,
    stored as the lowest 3 bits in a byte. ex: 00000BGR. Total display
    buffer size is (width * height) bytes.

    Five panel address pins (A-E) are implemented, supporting up to a
    64 row panel. Panel heights of 16 and 32 have been tested.

    Panel pins are driven by direct writes to the I/O port, minimizing
    time spent refreshing the display. The interrupt executes in less
    than 24 uS for a 32 pixel wide display.

    Theoretical maximum refresh rate for a 32 x 32 matrix:
    = 1 / (24 uS x 16 rows) = 2.6 KHz

    Measured maximum refresh rate for a 32 x 32 matrix: 2.3 KHz

    COPYRIGHT NOTICE: (c) 2018 ee-quipment.com

 ******************************************************************************/


#include  <stdint.h>
#include  <Adafruit_GFX.h>
#include  <avdweb_SAMDtimer.h>  //http://www.avdweb.nl/arduino/libraries/samd21-timer.html
#include  "LEDpanel.h"


#define LED_PANEL_REFRESH_FREQ    100   // Hz

/*
 * Arduino IDE pin mapping
 */
#if defined (ARDUINO_SAMD_FEATHER_M0)
#define CLK    0
#define LAT   15
#define OE    16
#define A     14
#define B     17
#define C     18
#define D      9
#define E      1
#define R1     5
#define G1    11
#define B1    13
#define R2    10
#define G2    12
#define B2     6

#elif defined (ARDUINO_SAMD_MKRZERO)
#define CLK    6
#define LAT    3
#define OE     2
#define A     16
#define B     17
#define C      4
#define D      5
#define E     14
#define R1    19
#define G1    20
#define B1    21
#define R2     7
#define G2     0
#define B2     1

#endif


static uint8_t    g_width, g_height;
static uint8_t  * fb;

/* Row refresh timer interrupt forward declaration */
void ISR_LEDPanelRefresh(struct tc_module *const module_inst);


LEDpanel::LEDpanel(int16_t width, int16_t height) {
    if ((width < 0) || (height < 0)) { return; }
    g_width  = width;
    g_height = height;
}


void LEDpanel::begin(uint8_t * frame_buffer) {
    unsigned interrupt_interval_us = 1000000 / ((g_height / 2) * LED_PANEL_REFRESH_FREQ);
    static SAMDtimer timer(4, ISR_LEDPanelRefresh, interrupt_interval_us);

    fb = frame_buffer;

    pinMode(CLK,  OUTPUT);
    pinMode(LAT,  OUTPUT);
    pinMode(OE,   OUTPUT);
    pinMode(A,    OUTPUT);
    pinMode(B,    OUTPUT);
    pinMode(C,    OUTPUT);
    if(g_height > 16) { pinMode(D, OUTPUT); }
    if(g_height > 32) { pinMode(E, OUTPUT); }
    pinMode(R1,   OUTPUT);
    pinMode(G1,   OUTPUT);
    pinMode(B1,   OUTPUT);
    pinMode(R2,   OUTPUT);
    pinMode(G2,   OUTPUT);
    pinMode(B2,   OUTPUT);
}


/*******************************************************************************

    void ISR_LEDPanelRefresh() - Refresh one row of the panel.

    Shift the frame buffer out to the panel, refreshing rows n & (height/2).
    Port writes are performed directly to the PORT output set/clear registers
    as a performance optimization.

    The shift register contents are output to the drive pins only when LAT goes
    high. The row drive pins are not latched and any changes to them will
    be reflected in the panel immediately.

    The clock line is left low between interrupts, otherwise it seems to lose
    the first clock edge occasionally when connected with a ribbon cable.

        // shift out pixel data
        loop x width
            Set Clk low
            Drive R1, G1, B1; R2, G2, B2
            Set Clk high
        end loop

        Set OE high   // blank display
        Drive row address onto (A, B, C, D, E)
        Set LE high   // latch pixel data and address
        Set LE low
        Set OE low    // enable display
        Increment row address

    Adafruit FEATHER M0
    ----------
    Panel Pins:      OE   LAT        B2    G2    R2    B1    G1    R1       CLK    E         D         C     B         A
    Port Pins:      PB09  PB08  ..  PA20  PA19  PA18  PA17  PA16  PA15  ..  PA11  PA10  ..  PA07  ..  PA05  PA04  ..  PA02
    Arduino IDE:     16    15        6     12    10    13    11    5         0     1         9         18    17        14

    ARDUINO MKRZero
    ---------------
    Panel Pins:      E         D     C         B     A         B2    G2    R2   CLK        OE   LAT        B1    G1    R1
    Connector Pins:  8         12    11        10    9         7     6     5     13        15    14        3     2     1
    Port Pins:      PB22  ..  PB11  PB10  ..  PB03  PB02  ..  PA23  PA22  PA21  PA20  ..  PA11  PA10  ..  PA07  PA06  PA05
    Arduino IDE:     14        5     4         17    16        1     0     7     6         3     2         21    20    19
    Board Pin Name:  TX        D5    D4        A2    A1        D1    D0    D7    D6        D3    D2        A6    A5    A4


******************************************************************************/
// Pointers to PORT Output Set/Clear registers
volatile uint32_t *setPortA = &PORT->Group[PORTA].OUTSET.reg;
volatile uint32_t *clrPortA = &PORT->Group[PORTA].OUTCLR.reg;
volatile uint32_t *setPortB = &PORT->Group[PORTB].OUTSET.reg;
volatile uint32_t *clrPortB = &PORT->Group[PORTB].OUTCLR.reg;

#if defined (ARDUINO_SAMD_FEATHER_M0)
static const uint32_t  RGB_MASK       = 0x07;           // port A
static const uint32_t  PIXEL_1_SHIFT  = 15;
static const uint32_t  PIXEL_2_SHIFT  = 18;
static const uint32_t  PIXEL_MASK     = (RGB_MASK << PIXEL_1_SHIFT) + (RGB_MASK << PIXEL_2_SHIFT);
static const uint32_t  CLK_MASK       = 0x01  << 11;
static const uint32_t  ABCD_MASK      = 0x12d << 2;
static const uint32_t  A_MASK         = 0x01;
static const uint32_t  BC_MASK        = 0x06;
static const uint32_t  D_MASK         = 0x08;
static const uint32_t  E_MASK         = 0x10;
static const uint32_t  LAT_MASK       = 0x01  << 8;     // port B
static const uint32_t  OE_MASK        = 0x01  << 9;

#elif defined (ARDUINO_SAMD_MKRZERO)
static const uint32_t  RGB_MASK       = 0x07;           // port A
static const uint32_t  PIXEL_1_SHIFT  = 5;
static const uint32_t  PIXEL_2_SHIFT  = 21;
static const uint32_t  PIXEL_MASK     = (RGB_MASK << PIXEL_1_SHIFT) + (RGB_MASK << PIXEL_2_SHIFT);
static const uint32_t  LAT_MASK       = 0x01  << 10;
static const uint32_t  OE_MASK        = 0x01  << 11;
static const uint32_t  CLK_MASK       = 0x01  << 20;
static const uint32_t  ABCD_MASK      = 0x300303 << 2;  // port B
static const uint32_t  AB_MASK        = 0x03;
static const uint32_t  CD_MASK        = 0x0c;
static const uint32_t  E_MASK         = 0x10;

#endif


void ISR_LEDPanelRefresh(struct tc_module *const module_inst) {
    static uint32_t   row=0;

    // clock in pixel data. loop unrolled x4
    uint8_t * ptr1 = &fb[row * g_width];
    uint8_t * ptr2 = &fb[(row + (g_height/2)) * g_width];
    for (int col=0; col<g_width; col+=4) {
        *setPortA = *ptr1++ << PIXEL_1_SHIFT;   // write pixel to top half of panel
        *setPortA = *ptr2++ << PIXEL_2_SHIFT;   // write pixel to bottom half of panel
        *setPortA = CLK_MASK;                   // clk high
        *clrPortA = CLK_MASK + PIXEL_MASK;      // clk low, pixel data zeroed
        *setPortA = *ptr1++ << PIXEL_1_SHIFT;
        *setPortA = *ptr2++ << PIXEL_2_SHIFT;
        *setPortA = CLK_MASK;
        *clrPortA = CLK_MASK + PIXEL_MASK;
        *setPortA = *ptr1++ << PIXEL_1_SHIFT;
        *setPortA = *ptr2++ << PIXEL_2_SHIFT;
        *setPortA = CLK_MASK;
        *clrPortA = CLK_MASK + PIXEL_MASK;
        *setPortA = *ptr1++ << PIXEL_1_SHIFT;
        *setPortA = *ptr2++ << PIXEL_2_SHIFT;
        *setPortA = CLK_MASK;
        *clrPortA = CLK_MASK + PIXEL_MASK;
    }

#if defined (ARDUINO_SAMD_FEATHER_M0)

    *setPortB = OE_MASK;            // oe high, turn off display

    // Set the ABCDE pins to the row address. Sadly, the port address pins are not contiguous
    *clrPortA = ABCD_MASK;
    *setPortA = ((row & E_MASK) << 6) | ((row & D_MASK) << 4) | ((row & BC_MASK) << 3) | ((row & A_MASK) << 2);

    // latch pixel data and address
    *setPortB = LAT_MASK;           // pulse latch to drive new data
    *clrPortB = LAT_MASK | OE_MASK; // turn display on again

#elif defined (ARDUINO_SAMD_MKRZERO)

    *setPortA = OE_MASK;            // oe high, turn off display

    // Set the ABCDE pins to the row address. Sadly, the port address pins are not contiguous
    *clrPortB = ABCD_MASK;
    *setPortB = ((row & E_MASK) << 18) | ((row & CD_MASK) << 8) | ((row & AB_MASK) << 2);

    // latch pixel data and address
    *setPortA = LAT_MASK;           // pulse latch to drive new data
    *clrPortA = LAT_MASK | OE_MASK; // turn display on again

#endif

    row = ++row & ((g_height/2) - 1);   // ++row % (g_height/2)
}




