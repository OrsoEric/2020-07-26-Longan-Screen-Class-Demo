/****************************************************************************
**	OrangeBot Project
*****************************************************************************
**        /
**       /
**      /
** ______ \
**         \
**          \
*****************************************************************************
**	Longan Nano Display Driver
*****************************************************************************
**  Development of the display class for the ST7735S LCD controller
**  interfaced to the SPI0 of the Risc-V Longan Nano board
**  Codesize exploded. -fno-exceptions as compiler option brought it back under control
****************************************************************************/

/****************************************************************************
**	INCLUDES
****************************************************************************/

//C++ std random number generators
#include <random>
//Longan Nano HAL
#include <gd32vf103.h>
//LED class
#include "longan_nano_led.hpp"
//Time class
#include "longan_nano_chrono.hpp"
//Higher level abstraction layer to base Display Class. Provides character sprites and print methods with color
#include "longan_nano_screen.hpp"

/****************************************************************************
**	NAMESPACES
****************************************************************************/

/****************************************************************************
**	DEFINES
****************************************************************************/

//forever
#define EVER (;;)

/****************************************************************************
**	MACROS
****************************************************************************/

/****************************************************************************
**	ENUM
****************************************************************************/

//Configurations
typedef enum _Config
{
    //Microseconds between calls of the screen update method. User decides how much CPU to allocate to the screen by tuning this time.
    //Longer time will mean fewer sprites rendered per second, but no matter what, the screen class will not crash as only the most recent sprite is drawn
    SCREEN_US           = 100,
    //Microseconds between led toggles
    LED_BLINK_US        = 250000,
    //Microseconds between calls of the demos. Various demos can be executed at differing rates depending on how many sprites they update
    SLOW_DEMO_US        = 500000,
    MEDIUM_DEMO_US      = 25000,
    FAST_DEMO_US        = 1000,
} Config;

//List of DEMOS
typedef enum _Demo
{
    //Periodically clear the full screen with a new color
    TEST_CLEAR_BLINK,
    //Periodically write a random ascii character in a random position in the screen
    TEST_CHAR_CONSOLE,
    //Periodically write a random ascii character in a random position with random colors on the screen
    TEST_CHAR_CONSOLE_COLOR,
    //Periodically write a random string with random length in a random position 
    TEST_STRING_CONSOLE,
    //Periodically write a random string with random length in a random position with random colors on the screen
    TEST_STRING_CONSOLE_COLOR,
    //Show numbers updating on the screen
    TEST_NUMBERS,
    //Engineering format strings
    TEST_ENG_NUMBERS,
    //Test the change_color 
    TEST_CHANGE_COLORS,
	//Profile execution time with constant workload
	TEST_WORKLOAD,
    //Total number of demos installed
    NUM_DEMOS,
    //Maximum length of a demo string
    MAX_STR_LEN = 25, 
} Demo;

/****************************************************************************
**	STRUCT
****************************************************************************/

typedef struct _Scheduler
{
    bool f_screen       : 1;
    bool f_demo         : 1;
    bool f_overrun      : 1;
} Scheduler;

/****************************************************************************
**	PROTOTYPES
****************************************************************************/

extern void init_pa8_button_interrupt( void );

/****************************************************************************
**	GLOBAL VARIABILES
****************************************************************************/

volatile bool g_rtc_flag = false;

//C++ Standard Random Number Generator
std::default_random_engine g_rng_engine;
//C++ standard random number distributions
std::uniform_int_distribution<uint8_t> g_rng_char( ' ', '~' );
std::uniform_int_distribution<int> g_rng_height( 0, Longan_nano::Screen::Config::FRAME_BUFFER_HEIGHT -1 );
std::uniform_int_distribution<int> g_rng_width( 0, Longan_nano::Screen::Config::FRAME_BUFFER_WIDTH -1 );
std::uniform_int_distribution<uint8_t> g_rng_color( 0, Longan_nano::Screen::Config::PALETTE_SIZE -1 );
//String length reroll
std::uniform_int_distribution<uint8_t> g_rng_length( 0, MAX_STR_LEN );

//Scheduler for the tasks
Scheduler g_scheduler = { 0 };

//True when the PA8 button is released
volatile bool g_f_pa8_button_up = false;

/****************************************************************************
**	FUNCTIONS
****************************************************************************/

/****************************************************************************
**	@brief main
**	main | void
****************************************************************************/
//! @return int |
//! @details Entry point of program
/***************************************************************************/

int main( void )
{
    //----------------------------------------------------------------
    //	VARS
    //----------------------------------------------------------------

    //Systick timer used to schedule activities
    Longan_nano::Chrono my_timer;
    //Systick timers to profile resource use
    Longan_nano::Chrono timer_uptime;
    Longan_nano::Chrono timer_screen;
    Longan_nano::Chrono timer_demo;

    
    //Display Driver
    Longan_nano::Screen g_screen;
    //elapsed time
    int elapsed_us;
    //Demo scheduler prescaler
    uint16_t scheduler_cnt = 0;
    uint16_t demo_pre = 100;

    //Default Demo to be executed
    Demo demo_index = Demo::TEST_WORKLOAD;
    //Demo is not initialized
    bool f_demo_init = false;

    //----------------------------------------------------------------
    //	INIT
    //----------------------------------------------------------------

    //Initialize LEDs
    Longan_nano::Leds::init();
    Longan_nano::Leds::set_color( Longan_nano::Leds::Color::BLACK );

    //Initialize the PA8 Boot button with interrupt. Used to switch between demos
    init_pa8_button_interrupt();

    //Initialize the Display
    g_screen.init();

    //Snap start
    my_timer.start();
    timer_uptime.start();
    timer_screen.start();

    //----------------------------------------------------------------
    //	BODY
    //----------------------------------------------------------------

    for EVER
    {
        //----------------------------------------------------------------
        //	Scheduler
        //----------------------------------------------------------------
        //  Hardwired scheduler to release tasks
        //  Thanks to the Longan Nano SysTick timer there is no need to use peripherals for timekeeping

        //Snap stop and get time since last start in microseconds
        elapsed_us = my_timer.stop( Longan_nano::Chrono::Unit::microseconds );
        //If: for some reason, the timing is invalid. Algorithmic error
        if (elapsed_us < 0)
        {
            Longan_nano::Leds::set( Longan_nano::Leds::Color::BLUE );
        }
        //If: enough time has passed between screen executions
        else if (elapsed_us >= Config::SCREEN_US)
        {
            //----------------------------------------------------------------
            //	Screen
            //----------------------------------------------------------------
            //  The screen is the fastest task

            //If: the previous task was not cleared
            if (g_scheduler.f_screen == true)
            {
                //There was an overrun. Not enough CPU to keep up
                g_scheduler.f_overrun = true;
            }
            else
            {
                //Issue a screen update
                g_scheduler.f_screen = true;
            }
            //Snap start. Restart the timer
            my_timer.start();

            //----------------------------------------------------------------
            //	Prescaler
            //----------------------------------------------------------------
            //  A prescaler is used to schedule the execution of slow tasks without the need of additional timers

            //Prescaler counter
            scheduler_cnt++;      

            //----------------------------------------------------------------
            //	Demo
            //----------------------------------------------------------------
            
            //If: enough ticks of the prescaler counter have elapsed
            if (scheduler_cnt%demo_pre == 0)
            {
                //Issue the execution of the demo
                g_scheduler.f_demo = true;
            }

            //----------------------------------------------------------------
            //	LED Blink
            //----------------------------------------------------------------

            //If: enough ticks of the prescaler counter have elapsed
            if (scheduler_cnt%(Config::LED_BLINK_US/Config::SCREEN_US) == 0)
            {
                Longan_nano::Leds::toggle( Longan_nano::Leds::Color::RED );
                
                //----------------------------------------------------------------
                //	Demo Switch
                //----------------------------------------------------------------
                //  PA8 boot button is used to switch between demos. I read it slowly to avoid bouncing

                //If: button released
                if (g_f_pa8_button_up == true)
                {
                    //Clear flag
                    g_f_pa8_button_up = false;
                    //Next demo
                    demo_index = (Demo)( ((uint8_t)demo_index < (uint8_t)Demo::NUM_DEMOS-1)?((uint8_t)demo_index +1):(0));
                    //Initialize the demo
                    f_demo_init = false;
                }      
            }
        }   //If: enough time has passed between screen executions
        //Default
        else
        {
            //Nothing to do    
        }
        
        //----------------------------------------------------------------
        //	OVERRUN
        //----------------------------------------------------------------
        //  Triggered when a task is not completed before the next issue

        //If: overrun
        if (g_scheduler.f_overrun == true)
        {
            //Clear error
            g_scheduler.f_overrun = false;
            //Signal overrun condition
            Longan_nano::Leds::toggle( Longan_nano::Leds::Color::BLUE );
        }

        //----------------------------------------------------------------
        //	Screen Update
        //----------------------------------------------------------------
        //	Ask the driver to sync with the display
        //	The driver will do nothing if its not time to update
        //	Use all the spare CPU time to do so
        
        //If: screen is authorized to update. User controls the CPU dedicated to this task
        if (g_scheduler.f_screen == true)
        {
            //Reset flag
            g_scheduler.f_screen = false;
            //Snap start
            timer_screen.start();
            //Execute a step in the screen update
            g_screen.update();
            //Accumulate DeltaT into timer accumulator
            timer_screen.accumulate();
        }

        //----------------------------------------------------------------
        //  DEMO
        //----------------------------------------------------------------
        
        //If: demo is authorized to execute
        if (g_scheduler.f_demo == true)
        {
            //Profile time spent running the DEMO
            timer_demo.start();

            //Clear execution flag
            g_scheduler.f_demo = false;
            
            //Switch: for each demo
            switch (demo_index)
            {
                //----------------------------------------------------------------
                //	TEST_CLEAR_BLINK
                //----------------------------------------------------------------
                //	Periodically clear the full screen with a new color

                case Demo::TEST_CLEAR_BLINK:
                {
                    //If: demo is yet to be initialized
                    if (f_demo_init == false)
                    {
                        g_screen.reset_colors();
                        //Clear the screen
                        g_screen.clear( Longan_nano::Screen::Color::BLACK );
                        //Configure prescaler to achieve the correct execution time    
                        demo_pre = Config::SLOW_DEMO_US/Config::SCREEN_US;
                        //Demo is now initialized
                        f_demo_init = true;
                    }
                    //If: demo is initialized and can be run
                    else
                    {
                        //Clear the screen to a random color
                        g_screen.clear( (Longan_nano::Screen::Color)g_rng_color( g_rng_engine ) );     
                    }

                    break;
                }
                
                //----------------------------------------------------------------
                //	TEST_CHAR_CONSOLE
                //----------------------------------------------------------------
                //	Periodically write a random ascii character in a random position in the screen

                case Demo::TEST_CHAR_CONSOLE:
                {
                    //If: demo is yet to be initialized
                    if (f_demo_init == false)
                    {
                        g_screen.reset_colors();
                        //Clear the screen
                        g_screen.clear( Longan_nano::Screen::Color::BLACK );
                        //Configure prescaler to achieve the correct execution time    
                        demo_pre = Config::FAST_DEMO_US/Config::SCREEN_US;
                        //Demo is now initialized
                        f_demo_init = true;
                    }
                    //If: demo is initialized and can be run
                    else
                    {
                        //Randomly generate character
                        char char_tmp = g_rng_char( g_rng_engine );
                        int height_tmp = g_rng_height( g_rng_engine );
                        int width_tmp = g_rng_width( g_rng_engine );
                        //Ask the screen driver to print the character
                        g_screen.print( height_tmp, width_tmp, char_tmp );  
                    }
                        
                    break;
                }

                //----------------------------------------------------------------
                //	TEST_CHAR_CONSOLE_COLOR
                //----------------------------------------------------------------
                //	Periodically write a random ascii character in a random position with random colors on the screen
                
                case Demo::TEST_CHAR_CONSOLE_COLOR:
                {
                    //If: demo is yet to be initialized
                    if (f_demo_init == false)
                    {
                        g_screen.reset_colors();
                        //Clear the screen
                        g_screen.clear( Longan_nano::Screen::Color::BLACK );
                        //Configure prescaler to achieve the correct execution time    
                        demo_pre = Config::FAST_DEMO_US/Config::SCREEN_US;
                        //Demo is now initialized
                        f_demo_init = true;
                    }
                    //If: demo is initialized and can be run
                    else
                    {
                        //Randomly generate character
                        uint8_t char_tmp = g_rng_char( g_rng_engine );
                        int height_tmp = g_rng_height( g_rng_engine );
                        int width_tmp = g_rng_width( g_rng_engine );
                        Longan_nano::Screen::Color background_tmp = (Longan_nano::Screen::Color)g_rng_color( g_rng_engine );
                        Longan_nano::Screen::Color foreground_tmp = (Longan_nano::Screen::Color)g_rng_color( g_rng_engine );
                        //Ask the screen driver to print the character
                        g_screen.print( height_tmp, width_tmp, (char)char_tmp,background_tmp, foreground_tmp );
                    }
                        
                    break;
                }

                //----------------------------------------------------------------
                //	TEST_STRING_CONSOLE
                //----------------------------------------------------------------
                //	Periodically write a random string with random length in a random position
                
                case Demo::TEST_STRING_CONSOLE:
                {
                    //If: demo is yet to be initialized
                    if (f_demo_init == false)
                    {
                        g_screen.reset_colors();
                        //Clear the screen
                        g_screen.clear( Longan_nano::Screen::Color::BLACK );
                        //Configure prescaler to achieve the correct execution time    
                        demo_pre = Config::MEDIUM_DEMO_US/Config::SCREEN_US;
                        //Demo is now initialized
                        f_demo_init = true;
                    }
                    //If: demo is initialized and can be run
                    else
                    {
                        //Randomly generate character
                        uint8_t len_tmp = g_rng_length( g_rng_engine );
                        //Temp string
                        char str[MAX_STR_LEN +1];
                        //Construct string
                        for (uint8_t t = 0; t < len_tmp;t++)
                        {
                            str[t] = g_rng_char( g_rng_engine );
                        }
                        //append terminator
                        str[len_tmp] = '\0';
                        //Random position
                        int height_tmp = g_rng_height( g_rng_engine );
                        int width_tmp = g_rng_width( g_rng_engine );
                        //Ask the screen driver to print the character
                        g_screen.print( height_tmp, width_tmp, str );
                    }
                    break;
                }

                //----------------------------------------------------------------
                //	TEST_STRING_CONSOLE_COLOR
                //----------------------------------------------------------------
                //	Periodically write a random string with random length in a random position with random colors
                
                case Demo::TEST_STRING_CONSOLE_COLOR:
                {
                    //If: demo is yet to be initialized
                    if (f_demo_init == false)
                    {
                        g_screen.reset_colors();
                        //Clear the screen
                        g_screen.clear( Longan_nano::Screen::Color::BLACK );
                        //Configure prescaler to achieve the correct execution time    
                        demo_pre = Config::MEDIUM_DEMO_US/Config::SCREEN_US;
                        //Demo is now initialized
                        f_demo_init = true;
                    }
                    //If: demo is initialized and can be run
                    else
                    {
                        //Randomly generate character
                        uint8_t len_tmp = g_rng_length( g_rng_engine );
                        //Temp string
                        char str[MAX_STR_LEN +1];
                        //Construct string
                        for (uint8_t t = 0; t < len_tmp;t++)
                        {
                            str[t] = g_rng_char( g_rng_engine );
                        }
                        //append terminator
                        str[len_tmp] = '\0';

                        int height_tmp = g_rng_height( g_rng_engine );
                        int width_tmp = g_rng_width( g_rng_engine );
                        Longan_nano::Screen::Color background_tmp = (Longan_nano::Screen::Color)g_rng_color( g_rng_engine );
                        Longan_nano::Screen::Color foreground_tmp = (Longan_nano::Screen::Color)g_rng_color( g_rng_engine );

                        //Ask the screen driver to print the character
                        g_screen.print( height_tmp, width_tmp, str, background_tmp, foreground_tmp );
                    }
                    break;
                }

                //----------------------------------------------------------------
                //	TEST_NUMBERS
                //----------------------------------------------------------------
                //	Test writing numbers on screen using the number to string screen print
                //  Profile execution times as well using the Chrono class
                
                case Demo::TEST_NUMBERS:
                {
                    //If: demo is yet to be initialized
                    if (f_demo_init == false)
                    {
                        g_screen.reset_colors();
                        //Clear the screen
                        g_screen.clear( Longan_nano::Screen::Color::BLACK );
                        //Configure prescaler to achieve the correct execution time    
                        demo_pre = Config::MEDIUM_DEMO_US/Config::SCREEN_US;
                        //Demo is now initialized
                        f_demo_init = true;
                    }
                    //If: demo is initialized and can be run
                    else
                    {
                        //Header
                        g_screen.print( 0, 0, "DEMO: Numeric String" );
                        //Demo counter
                        static int demo_cnt = 0;
                        demo_cnt++;
                        //Show a counter left aligned
                        g_screen.print( 1, 1, "Counter: " );
                        g_screen.set_format( 8, Longan_nano::Screen::Format_align::ADJ_LEFT, Longan_nano::Screen::Format_format::NUM );
                        g_screen.print( 1, 11, demo_cnt );
                        //Show a counter right aligned
                        g_screen.print( 2, 1, "Counter: " );
                        g_screen.set_format( 8, Longan_nano::Screen::Format_align::ADJ_RIGHT, Longan_nano::Screen::Format_format::NUM );
                        g_screen.print( 2, 19, demo_cnt );
                        //Temp
                        int tmp;
                        //Show uptime in microseconds
                        g_screen.print( 3, 1, "Uptime:" );
                        g_screen.print( 3, 18, "mS" );
                        g_screen.set_format( 10, Longan_nano::Screen::Format_align::ADJ_RIGHT, Longan_nano::Screen::Format_format::NUM );
                        tmp = timer_uptime.stop( Longan_nano::Chrono::Unit::milliseconds );
                        g_screen.print( 3, 17, tmp );
                        //Show cpu time spent updating the screen
                        g_screen.print( 4, 1, "Screen:" );
                        g_screen.print( 4, 18, "mS" );
                        tmp = timer_screen.get_accumulator( Longan_nano::Chrono::Unit::milliseconds );
                        g_screen.print( 4, 17, tmp );
                    }
                    break;
                }
                
                //----------------------------------------------------------------
                //	TEST_ENG_NUMBERS
                //----------------------------------------------------------------
                //	Test writing numbers on screen using the number to string screen print
                //  Profile execution times as well using the Chrono class
                
                case Demo::TEST_ENG_NUMBERS:
                {
                    //If: demo is yet to be initialized
                    if (f_demo_init == false)
                    {
                        g_screen.reset_colors();
                        //Clear the screen
                        g_screen.clear( Longan_nano::Screen::Color::BLACK );
                        //Configure prescaler to achieve the correct execution time    
                        demo_pre = Config::MEDIUM_DEMO_US/Config::SCREEN_US;
                        //Demo is now initialized
                        f_demo_init = true;
                    }
                    //If: demo is initialized and can be run
                    else
                    {     
                        int16_t num_sprites_changed = 0;
                        //Header
                        num_sprites_changed += g_screen.print( 0, 0, "DEMO: Profile eng" );
                        num_sprites_changed += g_screen.print( 1, 4, "|Time[s]|CPU [%]" );
                        //Show uptime in microseconds
                        int tmp_uptime;
                        num_sprites_changed += g_screen.print( 2, 0, "Time|" );
                        num_sprites_changed += g_screen.print( 2, 12, '|' );
                        
                        g_screen.set_format( User::String::STRING_SIZE_SENG -1, Longan_nano::Screen::Format_align::ADJ_RIGHT, Longan_nano::Screen::Format_format::ENG, -3 );
                        tmp_uptime = timer_uptime.stop( Longan_nano::Chrono::Unit::milliseconds );
                        num_sprites_changed += g_screen.print( 2, 11, tmp_uptime );
                        
                        //Show cpu time spent updating the screen
                        int tmp_screen;
                        num_sprites_changed += g_screen.print( 3, 0, "LCD |" );
                        num_sprites_changed += g_screen.print( 3, 12, '|' );
                        tmp_screen = timer_screen.get_accumulator( Longan_nano::Chrono::Unit::milliseconds );
                        num_sprites_changed += g_screen.print( 3, 11, tmp_screen );
                        //Compute CPU usage for the screen
                        int64_t cpu_tmp = (int64_t)1 *tmp_screen *100000 /tmp_uptime;
                        g_screen.set_format( User::String::STRING_SIZE_SENG -1, Longan_nano::Screen::Format_align::ADJ_RIGHT, Longan_nano::Screen::Format_format::ENG, -3 );
                        num_sprites_changed += g_screen.print( 3, 19, (int)cpu_tmp );

                        //Show CPU time spent running the DEMO
                        num_sprites_changed += g_screen.print( 4, 0, "DEMO|" );
                        num_sprites_changed += g_screen.print( 4, 12, '|' );
                        tmp_screen = timer_demo.get_accumulator( Longan_nano::Chrono::Unit::milliseconds );
                        g_screen.print( 4, 11, tmp_screen );
                        //Compute CPU usage for the DEMO
                        cpu_tmp = (int64_t)1 *tmp_screen *100000 /tmp_uptime;
                        g_screen.set_format( User::String::STRING_SIZE_SENG -1, Longan_nano::Screen::Format_align::ADJ_RIGHT, Longan_nano::Screen::Format_format::ENG, -3 );
                        num_sprites_changed += g_screen.print( 4, 19, (int)cpu_tmp );

                        //Profile the number of sprites updated by the previous functions
                        g_screen.set_format( 4, Longan_nano::Screen::Format_align::ADJ_LEFT, Longan_nano::Screen::Format_format::NUM );
                        g_screen.print( 1, 0, num_sprites_changed);
                    }
                    break;
                }

                //----------------------------------------------------------------
                //	TEST_CHANGE_COLORS
                //----------------------------------------------------------------
                //	Test the methods that handles the colors of the sprites without changing the content
                //  "change_color" change a palette color for another
                //  "set_palette_color"
                
                case Demo::TEST_CHANGE_COLORS:
                {
                    static uint8_t background_change_cnt;

                    //Randomly change the defaults background and foreground colors
                    Longan_nano::Screen::Color background_tmp = (Longan_nano::Screen::Color)g_rng_color( g_rng_engine );
                    Longan_nano::Screen::Color foreground_tmp = (Longan_nano::Screen::Color)g_rng_color( g_rng_engine );

                    //If: demo is yet to be initialized
                    if (f_demo_init == false)
                    {
                        g_screen.reset_colors();
                        //Clear the screen
                        g_screen.clear( Longan_nano::Screen::Color::BLACK );
                        //Configure prescaler to achieve the correct execution time    
                        demo_pre = Config::MEDIUM_DEMO_US/Config::SCREEN_US;
                        //Demo is now initialized
                        f_demo_init = true;
                    }
                    //If: demo is initialized and can be run
                    else
                    {
                        int16_t num_sprites_changed = 0;
                        //Header
                        num_sprites_changed += g_screen.print( 0, 0, "DEMO: Eng Num Color" );
                        num_sprites_changed += g_screen.print( 1, 4, "|Time[s]|CPU [%]" );
                        //Show uptime in microseconds
                        int tmp_uptime;
                        num_sprites_changed += g_screen.print( 2, 0, "Time|" );
                        num_sprites_changed += g_screen.print( 2, 12, '|' );
                        g_screen.set_format( User::String::STRING_SIZE_SENG -1, Longan_nano::Screen::Format_align::ADJ_RIGHT, Longan_nano::Screen::Format_format::ENG, -3 );
                        tmp_uptime = timer_uptime.stop( Longan_nano::Chrono::Unit::milliseconds );
                        num_sprites_changed += g_screen.print( 2, 11, tmp_uptime );
                        
                        //Show cpu time spent updating the screen
                        int tmp_screen;
                        num_sprites_changed += g_screen.print( 3, 0, "LCD |" );
                        num_sprites_changed += g_screen.print( 3, 12, '|' );
                        tmp_screen = timer_screen.get_accumulator( Longan_nano::Chrono::Unit::milliseconds );
                        num_sprites_changed += g_screen.print( 3, 11, tmp_screen );
                        //Compute CPU usage for the screen
                        int64_t cpu_tmp = (int64_t)1 *tmp_screen *100000 /tmp_uptime;
                        g_screen.set_format( User::String::STRING_SIZE_SENG -1, Longan_nano::Screen::Format_align::ADJ_RIGHT, Longan_nano::Screen::Format_format::ENG, -3 );
                        num_sprites_changed += g_screen.print( 3, 19, (int)cpu_tmp );

                        //Show CPU time spent running the DEMO
                        num_sprites_changed += g_screen.print( 4, 0, "DEMO|" );
                        num_sprites_changed += g_screen.print( 4, 12, '|' );
                        tmp_screen = timer_demo.get_accumulator( Longan_nano::Chrono::Unit::milliseconds );
                        g_screen.print( 4, 11, tmp_screen );
                        //Compute CPU usage for the DEMO
                        cpu_tmp = (int64_t)1 *tmp_screen *100000 /tmp_uptime;
                        g_screen.set_format( User::String::STRING_SIZE_SENG -1, Longan_nano::Screen::Format_align::ADJ_RIGHT, Longan_nano::Screen::Format_format::ENG, -3 );
                        num_sprites_changed += g_screen.print( 4, 19, (int)cpu_tmp );


                        //Randomly color a sprite
                        background_tmp = (Longan_nano::Screen::Color)g_rng_color( g_rng_engine );
                        foreground_tmp = (Longan_nano::Screen::Color)g_rng_color( g_rng_engine );

                        background_change_cnt++;
                        if (background_change_cnt > 15)
                        {
                            background_change_cnt = 0;
                            num_sprites_changed += g_screen.set_default_colors( background_tmp, foreground_tmp );                            
                        }

                        //Profile the number of sprites updated by the previous functions
                        g_screen.set_format( 4, Longan_nano::Screen::Format_align::ADJ_LEFT, Longan_nano::Screen::Format_format::NUM );
                        g_screen.print( 1, 0, num_sprites_changed);

                    }
                    break;
                }

                //----------------------------------------------------------------
                //	TEST_WORKLOAD
                //----------------------------------------------------------------
                //	Profile execution time with constant workload
				//	Allow to test improvements in the Driver Class and Screen Class
                
                case Demo::TEST_WORKLOAD:
                {
                    //If: demo is yet to be initialized
                    if (f_demo_init == false)
                    {
                        g_screen.reset_colors();
                        //Clear the screen
                        g_screen.clear( Longan_nano::Screen::Color::BLACK );
                        //Configure prescaler to achieve the correct execution time    
                        demo_pre = Config::MEDIUM_DEMO_US/Config::SCREEN_US;
                        //Demo is now initialized
                        f_demo_init = true;
                    }
                    //If: demo is initialized and can be run
                    else
                    {
						//Print random color squares on the screen
                        for (uint8_t t = 0;t < 10;t++)
                        {	
                            int th = g_rng_height( g_rng_engine );
                            int tw = g_rng_width( g_rng_engine );
                            Longan_nano::Screen::Color color_tmp = (Longan_nano::Screen::Color)g_rng_color( g_rng_engine );
                            //Ask the screen driver to print the character
                            g_screen.paint( th, tw, color_tmp );
                        }
						
                        //Header
                        g_screen.print( 0, 0, "CPU |" );
						g_screen.print( 0, 12, '|' );
						
                        
						//Compute CPU usage for the screen
						int tmp_uptime = timer_uptime.stop( Longan_nano::Chrono::Unit::milliseconds );
						int tmp_deltat = timer_screen.get_accumulator( Longan_nano::Chrono::Unit::milliseconds );
                        int cpu_tmp = (int64_t)1 *tmp_deltat *100000 /tmp_uptime;
                        g_screen.set_format( User::String::STRING_SIZE_SENG -1, Longan_nano::Screen::Format_align::ADJ_RIGHT, Longan_nano::Screen::Format_format::ENG, -3 );
                        g_screen.print( 0, 11, (int)cpu_tmp );
                        //Compute CPU usage for the DEMO
						tmp_deltat = timer_demo.get_accumulator( Longan_nano::Chrono::Unit::milliseconds );
                        cpu_tmp = (int64_t)1 *tmp_deltat *100000 /tmp_uptime;
						g_screen.set_format( User::String::STRING_SIZE_SENG -1, Longan_nano::Screen::Format_align::ADJ_RIGHT, Longan_nano::Screen::Format_format::ENG, -3 );
                        g_screen.print( 0, 19, (int)cpu_tmp );
                    }
                    break;
                }				
                //Unhandled demo
                default:
                {
                    //Do nothing

                }
            }   //End Switch: for each demo
            //Profile time spent running the DEMO
            timer_demo.accumulate();
        }	//End If: demo is authorized to execute

    } //End forever

    //----------------------------------------------------------------
    //	RETURN
    //----------------------------------------------------------------

    return 0;
}	//end function: main

/****************************************************************************
**	@brief init
**	init_pa8_button_interrupt | void
****************************************************************************/
//! @details 
//! User clicked the PA8 boot button
//! This button is used to switch between demos
/***************************************************************************/

void init_pa8_button_interrupt( void )
{
    //Clock the GPIO banks
    rcu_periph_clock_enable(RCU_GPIOA);
    //Setup the boot button
    gpio_init( GPIOA, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_50MHZ, GPIO_PIN_8 );

    //Initialize the ECLIC IRQ lines
    eclic_priority_group_set(ECLIC_PRIGROUP_LEVEL3_PRIO1);
    eclic_irq_enable(EXTI5_9_IRQn, 1, 1);

    //Initialize the EXTI. IRQ can be generated from GPIO edge detectors
    gpio_exti_source_select(GPIO_PORT_SOURCE_GPIOA, GPIO_PIN_SOURCE_8);
    exti_init(EXTI_8, EXTI_INTERRUPT, EXTI_TRIG_BOTH);

    //Clear interrupt flag. Ensure no spurious execution at start
    exti_interrupt_flag_clear(EXTI_8);

    //Enable the interrupts. From now on interrupt handlers can be executed
    eclic_global_interrupt_enable();	

    return;
}   //End init: init_pa8_button_interrupt | void

/****************************************************************************
**	@brief isr
**	EXTI5_9_IRQHandler | void
****************************************************************************/
//! @details 
//! User clicked the PA8 boot button
//! This button is used to switch between demos
/***************************************************************************/

extern "C"
void EXTI5_9_IRQHandler( void )
{
    //If: interrupt from PA8 boot button
    if (exti_interrupt_flag_get(EXTI_8) != RESET)
    {
        //Clear the interrupt from PA8 boot button
        exti_interrupt_flag_clear(EXTI_8);
        //If: the button is released after ISR (UP)
        if (gpio_input_bit_get( GPIOA, GPIO_PIN_8 ) == RESET)
        {
            //Signal event
            g_f_pa8_button_up = true;
        }
    }
    //Default: interrupt from an unhandled GPIO
    else
    {
        //Do nothing (should clear the interrupt flags)
    }
}   //End isr: EXTI5_9_IRQHandler | void