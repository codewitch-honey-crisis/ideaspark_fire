// UIX can draw to one buffer while sending
// another for better performance but it requires
// twice the transfer buffer memory
#define TWO_BUFFERS
// spans in GFX allow direct access to backing
// bitmap memory so read and write ops can be
// done with pointers rather than function
// calls, again for increased performance
#define USE_SPANS
// make the flames blue
//#define BLUE_FLAME
// use a generated rather than static palette
//#define USE_GEN_PALETTE

// screen dimensions
#define LCD_WIDTH 320
#define LCD_HEIGHT 170
// screen connections
#define LCD_CLK 18
#define LCD_MOSI 23
#define LCD_DC 2
#define LCD_CS 15
#define LCD_RST 4
#define LCD_BL 32 // optional
#if __has_include(<Arduino.h>)
#include "Arduino.h"
#endif
#include "driver/gpio.h"
#include "driver/spi_master.h"
#include "esp_lcd_panel_io.h"
#include "esp_lcd_panel_ops.h"
#include "esp_lcd_panel_vendor.h"
#include "gfx.h" // htcw_gfx
#include "uix.h" // htcw_uix
// This is a Windows 3.1 .fon file, embedded in a header:
#define VGA_9X16_IMPLEMENTATION
#include "vga_9x16.h"

// import the namespaces
using namespace gfx;
using namespace uix;

// fonts load from streams, so wrap the font header array in a const stream
const_buffer_stream fps_stream(vga_9x16,sizeof(vga_9x16));
// now declare the font using the fps_stream
win_font fps_font(fps_stream);

// data for fire
// first the width and height
constexpr static const size_t fire_width = (LCD_WIDTH / 4);
constexpr static const size_t fire_height = ((LCD_HEIGHT / 4) + 6);
#ifdef USE_SPANS
using px_type = uint16_t;
// store preswapped uint16_ts for performance
#ifndef BLUE_FLAME
#define RGB(r,g,b) (rgb_pixel<16>(r,g,b).swapped())
#else
#define RGB(r,g,b) (rgb_pixel<16>(b,g,r).swapped())
#endif
#else
// store rgb_pixel<16> instances
using px_type = rgb_pixel<16>;
#ifndef BLUE_FLAME
#define RGB(r,g,b) rgb_pixel<16>(r,g,b)
#else
#define RGB(r,g,b) rgb_pixel<16>(b,g,r)
#endif
#endif
#ifndef USE_GEN_PALETTE
// RGB565 color palette for flames
static constexpr const px_type fire_palette[] = {
    RGB(0, 0, 0), RGB(0, 0, 3), RGB(0, 0, 3), RGB(0, 0, 3),
    RGB(0, 0, 4), RGB(0, 0, 4), RGB(0, 0, 4), RGB(0, 0, 5),
    RGB(1, 0, 5), RGB(2, 0, 4), RGB(3, 0, 4), RGB(4, 0, 4),
    RGB(5, 0, 3), RGB(6, 0, 3), RGB(7, 0, 3), RGB(8, 0, 2),
    RGB(9, 0, 2), RGB(10, 0, 2), RGB(11, 0, 2), RGB(12, 0, 1),
    RGB(13, 0, 1), RGB(14, 0, 1), RGB(15, 0, 0), RGB(16, 0, 0),
    RGB(16, 0, 0), RGB(16, 0, 0), RGB(17, 0, 0), RGB(17, 0, 0),
    RGB(18, 0, 0), RGB(18, 0, 0), RGB(18, 0, 0), RGB(19, 0, 0),
    RGB(19, 0, 0), RGB(20, 0, 0), RGB(20, 0, 0), RGB(20, 0, 0),
    RGB(21, 0, 0), RGB(21, 0, 0), RGB(22, 0, 0), RGB(22, 0, 0),
    RGB(23, 1, 0), RGB(23, 1, 0), RGB(24, 2, 0), RGB(24, 2, 0),
    RGB(25, 3, 0), RGB(25, 3, 0), RGB(26, 4, 0), RGB(26, 4, 0),
    RGB(27, 5, 0), RGB(27, 5, 0), RGB(28, 6, 0), RGB(28, 6, 0),
    RGB(29, 7, 0), RGB(29, 7, 0), RGB(30, 8, 0), RGB(30, 8, 0),
    RGB(31, 9, 0), RGB(31, 9, 0), RGB(31, 10, 0), RGB(31, 10, 0),
    RGB(31, 11, 0), RGB(31, 11, 0), RGB(31, 12, 0), RGB(31, 12, 0),
    RGB(31, 13, 0), RGB(31, 13, 0), RGB(31, 14, 0), RGB(31, 14, 0),
    RGB(31, 15, 0), RGB(31, 15, 0), RGB(31, 16, 0), RGB(31, 16, 0),
    RGB(31, 17, 0), RGB(31, 17, 0), RGB(31, 18, 0), RGB(31, 18, 0),
    RGB(31, 19, 0), RGB(31, 19, 0), RGB(31, 20, 0), RGB(31, 20, 0),
    RGB(31, 21, 0), RGB(31, 21, 0), RGB(31, 22, 0), RGB(31, 22, 0),
    RGB(31, 23, 0), RGB(31, 24, 0), RGB(31, 24, 0), RGB(31, 25, 0),
    RGB(31, 25, 0), RGB(31, 26, 0), RGB(31, 26, 0), RGB(31, 27, 0),
    RGB(31, 27, 0), RGB(31, 28, 0), RGB(31, 28, 0), RGB(31, 29, 0),
    RGB(31, 29, 0), RGB(31, 30, 0), RGB(31, 30, 0), RGB(31, 31, 0),
    RGB(31, 31, 0), RGB(31, 32, 0), RGB(31, 32, 0), RGB(31, 33, 0),
    RGB(31, 33, 0), RGB(31, 34, 0), RGB(31, 34, 0), RGB(31, 35, 0),
    RGB(31, 35, 0), RGB(31, 36, 0), RGB(31, 36, 0), RGB(31, 37, 0),
    RGB(31, 38, 0), RGB(31, 38, 0), RGB(31, 39, 0), RGB(31, 39, 0),
    RGB(31, 40, 0), RGB(31, 40, 0), RGB(31, 41, 0), RGB(31, 41, 0),
    RGB(31, 42, 0), RGB(31, 42, 0), RGB(31, 43, 0), RGB(31, 43, 0),
    RGB(31, 44, 0), RGB(31, 44, 0), RGB(31, 45, 0), RGB(31, 45, 0),
    RGB(31, 46, 0), RGB(31, 46, 0), RGB(31, 47, 0), RGB(31, 47, 0),
    RGB(31, 48, 0), RGB(31, 48, 0), RGB(31, 49, 0), RGB(31, 49, 0),
    RGB(31, 50, 0), RGB(31, 50, 0), RGB(31, 51, 0), RGB(31, 52, 0),
    RGB(31, 52, 0), RGB(31, 52, 0), RGB(31, 52, 0), RGB(31, 52, 0),
    RGB(31, 53, 0), RGB(31, 53, 0), RGB(31, 53, 0), RGB(31, 53, 0),
    RGB(31, 54, 0), RGB(31, 54, 0), RGB(31, 54, 0), RGB(31, 54, 0),
    RGB(31, 54, 0), RGB(31, 55, 0), RGB(31, 55, 0), RGB(31, 55, 0),
    RGB(31, 55, 0), RGB(31, 56, 0), RGB(31, 56, 0), RGB(31, 56, 0),
    RGB(31, 56, 0), RGB(31, 57, 0), RGB(31, 57, 0), RGB(31, 57, 0),
    RGB(31, 57, 0), RGB(31, 57, 0), RGB(31, 58, 0), RGB(31, 58, 0),
    RGB(31, 58, 0), RGB(31, 58, 0), RGB(31, 59, 0), RGB(31, 59, 0),
    RGB(31, 59, 0), RGB(31, 59, 0), RGB(31, 60, 0), RGB(31, 60, 0),
    RGB(31, 60, 0), RGB(31, 60, 0), RGB(31, 60, 0), RGB(31, 61, 0),
    RGB(31, 61, 0), RGB(31, 61, 0), RGB(31, 61, 0), RGB(31, 62, 0),
    RGB(31, 62, 0), RGB(31, 62, 0), RGB(31, 62, 0), RGB(31, 63, 0),
    RGB(31, 63, 0), RGB(31, 63, 1), RGB(31, 63, 1), RGB(31, 63, 2),
    RGB(31, 63, 2), RGB(31, 63, 3), RGB(31, 63, 3), RGB(31, 63, 4),
    RGB(31, 63, 4), RGB(31, 63, 5), RGB(31, 63, 5), RGB(31, 63, 5),
    RGB(31, 63, 6), RGB(31, 63, 6), RGB(31, 63, 7), RGB(31, 63, 7),
    RGB(31, 63, 8), RGB(31, 63, 8), RGB(31, 63, 9), RGB(31, 63, 9),
    RGB(31, 63, 10), RGB(31, 63, 10), RGB(31, 63, 10), RGB(31, 63, 11),
    RGB(31, 63, 11), RGB(31, 63, 12), RGB(31, 63, 12), RGB(31, 63, 13),
    RGB(31, 63, 13), RGB(31, 63, 14), RGB(31, 63, 14), RGB(31, 63, 15),
    RGB(31, 63, 15), RGB(31, 63, 15), RGB(31, 63, 16), RGB(31, 63, 16),
    RGB(31, 63, 17), RGB(31, 63, 17), RGB(31, 63, 18), RGB(31, 63, 18),
    RGB(31, 63, 19), RGB(31, 63, 19), RGB(31, 63, 20), RGB(31, 63, 20),
    RGB(31, 63, 21), RGB(31, 63, 21), RGB(31, 63, 21), RGB(31, 63, 22),
    RGB(31, 63, 22), RGB(31, 63, 23), RGB(31, 63, 23), RGB(31, 63, 24),
    RGB(31, 63, 24), RGB(31, 63, 25), RGB(31, 63, 25), RGB(31, 63, 26),
    RGB(31, 63, 26), RGB(31, 63, 26), RGB(31, 63, 27), RGB(31, 63, 27),
    RGB(31, 63, 28), RGB(31, 63, 28), RGB(31, 63, 29), RGB(31, 63, 29),
    RGB(31, 63, 30), RGB(31, 63, 30), RGB(31, 63, 31), RGB(31, 63, 31)};
#else
static px_type fire_palette[256];
static void gen_palette() {
    static constexpr const size_t pal_size = sizeof(fire_palette)/sizeof(px_type);
    for (int i = 0; i < pal_size; i++) {
        int N = pal_size - 1;

        // Red channel ramps up quickly to 31
        unsigned char r = (unsigned char)(31 * i / N);

        // Green channel starts increasing later, reaching max at 3/4 of the way
        unsigned char g = (i > N / 4) ? (unsigned char)(63 * (i - N / 4) / (3 * N / 4)) : 0;
        if (g > 63) g = 63;

        // Blue appears in the last quarter of the range
        unsigned char b = (i > 3 * N / 4) ? (unsigned char)(31 * (i - 3 * N / 4) / (N / 4)) : 0;
        if (b > 31) b = 31;

        fire_palette[i] = RGB(r, g, b);
    }
}
#endif
// the fire frame buffer
static uint8_t fire_buffer[fire_height][fire_width]; // fire buffer, quarter resolution w/extra lines

using screen_t = uix::screen<rgb_pixel<16>>;
using scr_color_t = color<screen_t::pixel_type>;
using uix_color_t = color<rgba_pixel<32>>;

// for UIX to manage the display
static uix::display lcd;
// the size of our transfer buffer(s)
static const constexpr size_t lcd_transfer_buffer_size = LCD_WIDTH*LCD_HEIGHT*2/2;
static uint8_t* lcd_transfer_buffer1 = nullptr;
// the second buffer, if indicated
#ifdef TWO_BUFFERS
static uint8_t* lcd_transfer_buffer2 = nullptr;
#endif

// tell UIX the DMA transfer is complete
static bool lcd_flush_ready(esp_lcd_panel_io_handle_t panel_io, 
    esp_lcd_panel_io_event_data_t* edata, 
    void* user_ctx) {
    lcd.flush_complete();
    return true;
}
// tell the lcd panel api to transfer data via DMA
static void uix_on_flush(const rect16& bounds, const void* bmp, void* state) {
    int x1 = bounds.x1, y1 = bounds.y1, x2 = bounds.x2 + 1, y2 = bounds.y2 + 1;
    esp_lcd_panel_draw_bitmap((esp_lcd_panel_handle_t)state, x1, y1, x2, y2, (void*)bmp);
}
// initialize the screen using the esp panel API
static void lcd_init() {
#ifdef LCD_BL
#if LCD_BL > -1
    gpio_set_direction((gpio_num_t)LCD_BL,GPIO_MODE_OUTPUT);
    gpio_set_level((gpio_num_t)LCD_BL,0);
#endif
#endif
    lcd_transfer_buffer1 = (uint8_t*)heap_caps_malloc(lcd_transfer_buffer_size,MALLOC_CAP_DMA);
#ifdef TWO_BUFFERS
    lcd_transfer_buffer2 = (uint8_t*)heap_caps_malloc(lcd_transfer_buffer_size,MALLOC_CAP_DMA);
#endif
    if(lcd_transfer_buffer1==nullptr
#ifdef TWO_BUFFERS
        ||lcd_transfer_buffer2==nullptr
#endif
    ) {
        puts("Out of memory allocating transfer buffer");
        while(1) vTaskDelay(5);
    }
    spi_bus_config_t buscfg;
    memset(&buscfg, 0, sizeof(buscfg));
    buscfg.sclk_io_num = LCD_CLK;
    buscfg.mosi_io_num = LCD_MOSI;
    buscfg.miso_io_num = -1;
    buscfg.quadwp_io_num = -1;
    buscfg.quadhd_io_num = -1;
    buscfg.max_transfer_sz = lcd_transfer_buffer_size + 8;

    // Initialize the SPI bus on VSPI (SPI3)
    spi_bus_initialize(SPI3_HOST, &buscfg, SPI_DMA_CH_AUTO);

    esp_lcd_panel_io_handle_t io_handle = NULL;
    esp_lcd_panel_io_spi_config_t io_config;
    memset(&io_config, 0, sizeof(io_config));
    io_config.dc_gpio_num = LCD_DC,
    io_config.cs_gpio_num = LCD_CS,
    io_config.pclk_hz = 40*1000*1000,
    io_config.lcd_cmd_bits = 8,
    io_config.lcd_param_bits = 8,
    io_config.spi_mode = 0,
    io_config.trans_queue_depth = 10,
    io_config.on_color_trans_done = lcd_flush_ready;
    // Attach the LCD to the SPI bus
    esp_lcd_new_panel_io_spi((esp_lcd_spi_bus_handle_t)SPI3_HOST, &io_config, &io_handle);
    esp_lcd_panel_dev_config_t lcd_config;
    memset(&lcd_config, 0, sizeof(lcd_config));
    lcd_config.reset_gpio_num = LCD_RST;
#if ESP_IDF_VERSION >= ESP_IDF_VERSION_VAL(5, 0, 0)
    lcd_config.rgb_endian = LCD_RGB_ENDIAN_RGB;
#else
    lcd_config.color_space = ESP_LCD_COLOR_SPACE_RGB;
#endif
    lcd_config.bits_per_pixel = 16;

    // Initialize the LCD configuration
    esp_lcd_panel_handle_t lcd_handle = nullptr;
    esp_lcd_new_panel_st7789(io_handle, &lcd_config, &lcd_handle);

    // Reset the display
    esp_lcd_panel_reset(lcd_handle);

    // Initialize LCD panel
    esp_lcd_panel_init(lcd_handle);
    //  Configure the panel (Different LCD screens may need different options)
    esp_lcd_panel_swap_xy(lcd_handle, false);
    esp_lcd_panel_set_gap(lcd_handle, 0, 35);
    esp_lcd_panel_swap_xy(lcd_handle,true);
    esp_lcd_panel_mirror(lcd_handle, false, true);
    esp_lcd_panel_invert_color(lcd_handle, true);
    // Turn on the screen
#if ESP_IDF_VERSION >= ESP_IDF_VERSION_VAL(5, 0, 0)
    esp_lcd_panel_disp_on_off(lcd_handle, true);
#else
    esp_lcd_panel_disp_off(lcd_handle, false);
#endif
#ifdef LCD_BL
#if LCD_BL > -1
    gpio_set_level((gpio_num_t)LCD_BL,1);
#endif
#endif
    lcd.on_flush_callback(uix_on_flush,lcd_handle);
    lcd.buffer_size(lcd_transfer_buffer_size);
    lcd.buffer1(lcd_transfer_buffer1);
#ifdef TWO_BUFFERS
    lcd.buffer2(lcd_transfer_buffer2);
#endif
}

// create a UIX control class to handle drawing the fire
class fire_box : public control<screen_t::control_surface_type> {
    using base_type = control<screen_t::control_surface_type>;
public:
    fire_box() : base_type()
    {}
    // gets called once per update
    void on_before_paint() override {
        unsigned int i, j, delta;    // looping variables, counters, and data
        for (i = 1; i < fire_height; ++i)
        {
            for (j = 0; j < fire_width; ++j)
            {
                if (j == 0)
                    fire_buffer[i - 1][j] = (fire_buffer[i][j] +
                                    fire_buffer[i - 1][fire_width - 1] +
                                    fire_buffer[i][j + 1] +
                                    fire_buffer[i + 1][j]) >>
                                    2;
                else if (j == LCD_WIDTH/4-1)
                    fire_buffer[i - 1][j] = (fire_buffer[i][j] +
                                    fire_buffer[i][j - 1] +
                                    fire_buffer[i + 1][0] +
                                    fire_buffer[i + 1][j]) >>
                                    2;
                else
                    fire_buffer[i - 1][j] = (fire_buffer[i][j] +
                                    fire_buffer[i][j - 1] +
                                    fire_buffer[i][j + 1] +
                                    fire_buffer[i + 1][j]) >>
                                    2;
    
                if (fire_buffer[i][j] > 11)
                    fire_buffer[i][j] = fire_buffer[i][j] - 12;
                else if (fire_buffer[i][j] > 3)
                    fire_buffer[i][j] = fire_buffer[i][j] - 4;
                else
                {
                    if (fire_buffer[i][j] > 0)
                        fire_buffer[i][j]--;
                    if (fire_buffer[i][j] > 0)
                        fire_buffer[i][j]--;
                    if (fire_buffer[i][j] > 0)
                        fire_buffer[i][j]--;
                }
            }
        }
        delta = 0;
        for (j = 0; j < fire_width; j++)
        {
            if (rand() % 10 < 5)
            {
                delta = (rand() & 1) * 255;
            }
            fire_buffer[fire_height - 2][j] = delta;
            fire_buffer[fire_height - 1][j] = delta;
        }
    }
    // may get called multipe times per update
    void on_paint(control_surface_type &destination, const srect16 &clip) override {
    #ifdef USE_SPANS
        static_assert(gfx::helpers::is_same<rgb_pixel<16>,typename screen_t::pixel_type>::value,"USE_SPANS only works with RGB565");
        // while clipping is done automatically, it's faster to only draw 
        // the parts we need, so use the clipping rectangle.
        for (int y = clip.y1; y <= clip.y2; y+=2) {
            // must use rgb_pixel<16>
            // get the spans for the current partial rows (starting at clip.x1)
            // note that we're getting two, because we draw 2x2 squares
            // of all the same color.
            gfx_span row = destination.span(point16(clip.x1,y));
            gfx_span row2 = destination.span(point16(clip.x1,y+1));
            // get the pointers to the partial row data
            uint16_t *prow = (uint16_t*)row.data;
            uint16_t *prow2 = (uint16_t*)row2.data;
            for (int x = clip.x1; x <= clip.x2; x+=2) {
                int i = y >> 2;
                int j = x >> 2;
                px_type px = fire_palette[fire_buffer[i][j]];
                // set the pixels
                *(prow++)=px;
                // if the clip x ends on an odd value, we need to not set the pointer
                // so check here
                if(x-clip.x1+1<row.length) {
                    *(prow++)=px;
                }
                // the clip y ends on an odd value prow2 will be null
                if(prow2!=nullptr) {
                    *(prow2++)=px;
                    // another check for x if clip ends on an odd value
                    if(x-clip.x1+1<row2.length) {
                        *(prow2++)=px;
                    }
                }                
            }
        }
    #else 
        for (int y = clip.y1; y <= clip.y2; ++y) {
            for (int x = clip.x1; x <= clip.x2; ++x) {
                int i = y >> 2;
                int j = x >> 2;
                px_type px = fire_palette[fire_buffer[i][j]];
                // set the pixel
                destination.point(point16(x,y),px);
            }
        }
    #endif               
    }

};

using label_t = uix::label<screen_t::control_surface_type>;

// the screen
static screen_t scr;
// the controls
static fire_box fire;
static label_t fps;

// initialize the screens and controls
static void screen_init()
{
    // make sure to set the dimensions of the screen
    scr.dimensions(ssize16(LCD_WIDTH,LCD_HEIGHT));
    // make the fire control the size of the screen
    fire.bounds(scr.bounds());
    // register it with the screen
    scr.register_control(fire);
    // put the font text at the top of the screen
    fps.bounds(srect16(0,0,scr.bounds().x2,fps_font.line_height()-1));
    fps.font(fps_font);
    fps.color(uix_color_t::green);
    fps.text_justify(uix_justify::top_right);
    fps.text("");
    fps.padding({0,0});
    scr.register_control(fps);
}
#ifdef ARDUINO
void setup() {
    // enable batt power:
    pinMode(15,OUTPUT);
    digitalWrite(15,HIGH);
    Serial.begin(115200);
#else
void loop();
void loop_task(void* arg) {
    uint32_t ts = pdTICKS_TO_MS(xTaskGetTickCount());
    while(1) {
        loop();
        if(pdTICKS_TO_MS(xTaskGetTickCount())>=ts+250) {
            ts=pdTICKS_TO_MS(xTaskGetTickCount());
            vTaskDelay(1);
        }
    }
}
extern "C" void app_main() {
    gpio_set_direction((gpio_num_t)15,GPIO_MODE_OUTPUT);
    gpio_set_level((gpio_num_t)15,1);
#endif
#ifdef USE_GEN_PALETTE
    gen_palette();
#endif
    lcd_init();
    fps_font.initialize();
    screen_init();
    lcd.active_screen(scr);
#ifndef ARDUINO
    TaskHandle_t loop_handle;
    xTaskCreate(loop_task,"loop_task",4096,nullptr,10,&loop_handle);
#endif
}

void loop() {
    // statistics
    static int frames = 0;
    static char szfps[64];
    static uint32_t fps_ts = 0;
    static int old_frames = 0;
    uint32_t ms = pdTICKS_TO_MS(xTaskGetTickCount());
    static uint32_t total_ms = 0;
    // UIX will return from update() immediately if 
    // a transfer is in progress and it can't send
    // the currently generated frame
    // in that case, we don't want to count the update
    // call as another frame nor actually update the frame.
    // We can check flush pending to see if it's 
    // actually ready for a new frame:
    if(!lcd.flush_pending()) {
        ++frames;
        // generate the next fire frame on lcd.update():
        fire.invalidate();
    }
    
    if (ms > fps_ts + 1000)
    {
        fps_ts = ms;
        if(old_frames!=frames) {
            old_frames = frames;
            if(frames==0) {
                snprintf(szfps, sizeof(szfps), "fps: < 1, total: %d ms",(int)total_ms);
            } else {
                snprintf(szfps, sizeof(szfps), "fps: %d, avg: %d ms", (int)frames,(int)total_ms/frames);
            }
        }
        fps.text(szfps);
        puts(szfps);
        frames = 0;
        total_ms = 0;
    }
    // update the display
    lcd.update();
    total_ms+=(pdTICKS_TO_MS(xTaskGetTickCount())-ms);
    ms = pdTICKS_TO_MS(xTaskGetTickCount());
}
