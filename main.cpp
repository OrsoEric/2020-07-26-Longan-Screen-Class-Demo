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
std::uniform_int_distribution<uint8_t> g_rng_char(' ','~');
std::uniform_int_distribution<int> g_rng_height(0,Longan_nano::Screen::FRAME_BUFFER_HEIGHT-1);
std::uniform_int_distribution<int> g_rng_width(0,Longan_nano::Screen::FRAME_BUFFER_WIDTH-1);
std::uniform_int_distribution<uint8_t> g_rng_color(0, Longan_nano::Screen::PALETTE_SIZE);
//String length reroll
std::uniform_int_distribution<uint8_t> g_rng_length(0, MAX_STR_LEN);

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
    //Display Driver
    Longan_nano::Screen g_screen;
    //elapsed time
    int elapsed_us;
    //Demo scheduler prescaler
    uint16_t scheduler_cnt = 0;
    uint16_t demo_pre = 100;

    //Demo to be executed
    Demo demo_index = Demo::TEST_CLEAR_BLINK;
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

        //Snap stop
        my_timer.stop();
        elapsed_us = my_timer.elapsed_us();
        //If: for some reason, the timing is invalid. Algorithmic error
        if (elapsed_us < 0)
        {
            Longan_nano::Leds::toggle( Longan_nano::Leds::Color::BLUE );
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
            //Execute a step in the screen update
            g_screen.update();
        }

        //----------------------------------------------------------------
        //  DEMO
        //----------------------------------------------------------------
        
        //If: demo is authorized to execute
        if (g_scheduler.f_demo == true)
        {
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
                        g_screen.print( height_tmp, width_tmp, char_tmp,background_tmp, foreground_tmp );
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
                //Unhandled demo
                default:
                {
                    //Do nothing

                }
            }   //End Switch: for each demo

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