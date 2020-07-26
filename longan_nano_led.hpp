/**********************************************************************************
MIT License

Copyright (c) 2020 Orso Eric

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
**********************************************************************************/

/**********************************************************************************
**	ENVIROMENT VARIABILE
**********************************************************************************/

#ifndef LONGAN_NANO_LED_H_
    #define LONGAN_NANO_LED_H_

/**********************************************************************************
**	GLOBAL INCLUDES
**********************************************************************************/

#include <gd32vf103.h>

/**********************************************************************************
**	DEFINES
**********************************************************************************/

/**********************************************************************************
**	MACROS
**********************************************************************************/

/**********************************************************************************
**	NAMESPACE
**********************************************************************************/

//! @namespace User My custom namespace
namespace Longan_nano
{
 
/**********************************************************************************
**	TYPEDEFS
**********************************************************************************/

/**********************************************************************************
**	PROTOTYPE: STRUCTURES
**********************************************************************************/

/**********************************************************************************
**	PROTOTYPE: GLOBAL VARIABILES
**********************************************************************************/

/**********************************************************************************
**	PROTOTYPE: CLASS
**********************************************************************************/

/************************************************************************************/
//! @class 		Longan_nano_led
/************************************************************************************/
//!	@author		Orso Eric
//! @version	0.1 alpha
//! @date		2019/05
//! @brief		Dummy Library
//! @details
//!	Verbose description \n
//! xxx
//! @pre		No prerequisites
//! @bug		None
//! @warning	No warnings
//! @copyright	License ?
//! @todo		todo list
/************************************************************************************/

class Leds
{
    //Visible to all
    public:
        //--------------------------------------------------------------------------
        //	ENUM
        //--------------------------------------------------------------------------
        //	Scope enums inside class in order to encapsulate them where they logically belong

        typedef enum _Color
        {
            BLACK,
            RED,
            GREEN,
            BLUE,
            WHITE
        } Color;

        typedef enum _Led_gpio
        {
            RED_GPIO = GPIOC,
            GREEN_GPIO = GPIOA,
            BLUE_GPIO = GPIOA,
        } Led_gpio;

        typedef enum _Led_pin
        {
            RED_PIN = GPIO_PIN_13,
            GREEN_PIN = GPIO_PIN_1,
            BLUE_PIN = GPIO_PIN_2,
        } Led_pin;

        //--------------------------------------------------------------------------
        //	CONSTRUCTORS
        //--------------------------------------------------------------------------

        //! Default constructor
        Leds( void );

        //--------------------------------------------------------------------------
        //	DESTRUCTORS
        //--------------------------------------------------------------------------

        //!Default destructor
        ~Leds( void );

        //--------------------------------------------------------------------------
        //	OPERATORS
        //--------------------------------------------------------------------------

        //--------------------------------------------------------------------------
        //	SETTERS
        //--------------------------------------------------------------------------

        //--------------------------------------------------------------------------
        //	GETTERS
        //--------------------------------------------------------------------------

        //--------------------------------------------------------------------------
        //	TESTERS
        //--------------------------------------------------------------------------

        //--------------------------------------------------------------------------
        //	PUBLIC METHODS
        //--------------------------------------------------------------------------

        /***************************************************************************/
        //!	@brief public static method
        //!	init |
        /***************************************************************************/
        //! @return bool | false = OK | true = ERR
        //! @details
        //!	initialize the longan nano GPIO LEDs
        /***************************************************************************/

        static inline bool init( void )
        {
            //----------------------------------------------------------------
            //	VARS
            //----------------------------------------------------------------

            //----------------------------------------------------------------
            //	BODY
            //----------------------------------------------------------------

            //Clock the GPIO banks
            rcu_periph_clock_enable(RCU_GPIOA);
            rcu_periph_clock_enable(RCU_GPIOC);
            //Setup the R (PC13), G (PA1) and B (PA2) LEDs
            gpio_init(Led_gpio::RED_GPIO, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ,Led_pin::RED_PIN);
            gpio_init(Led_gpio::GREEN_GPIO, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ,Led_pin::GREEN_PIN);
            gpio_init(Led_gpio::BLUE_GPIO, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ,Led_pin::BLUE_PIN);

            //----------------------------------------------------------------
            //	RETURN
            //----------------------------------------------------------------

            return false;	//OK
        }	//End public static method: init

        /***************************************************************************/
        //!	@brief public static method
        //!	set_color | Led_color
        /***************************************************************************/
        //!	@param Led_color | color of the LED
        //! @return bool | false = OK | true = ERR
        //! @details
        //!	initialize the longan nano GPIO LEDs
        /***************************************************************************/

        static inline bool set_color( Leds::Color color )
        {
            //----------------------------------------------------------------
            //	VARS
            //----------------------------------------------------------------

            //LED are inverted. SET turn them OFF
            bit_status led_r = FlagStatus::SET;
            bit_status led_g = FlagStatus::SET;
            bit_status led_b = FlagStatus::SET;
            
            //----------------------------------------------------------------
            //	BODY
            //----------------------------------------------------------------

            //Switch: Color
            switch(color)
            {
                case (Leds::Color::BLACK):
                {

                    break;
                }
                case (Leds::Color::WHITE):
                {
                    led_r = FlagStatus::RESET;
                    led_g = FlagStatus::RESET;
                    led_b = FlagStatus::RESET;

                    break;
                }
                default:
                {
                    return true;	//FAIL
                }
            }	//End Switch: Color

            //Apply the color setting
            gpio_bit_write( Led_gpio::RED_GPIO, Led_pin::RED_PIN, (bit_status)led_r );
            gpio_bit_write( Led_gpio::GREEN_GPIO, Led_pin::GREEN_PIN, (bit_status)led_g );
            gpio_bit_write( Led_gpio::BLUE_GPIO, Led_pin::BLUE_PIN, (bit_status)led_b );

            //----------------------------------------------------------------
            //	RETURN
            //----------------------------------------------------------------

            return false;	//OK
        }	//End public static method: set_color

        /***************************************************************************/
        //!	@brief public static method
        //!	toggle |
        /***************************************************************************/
        //! @return bool | false = OK | true = ERR
        //! @details
        //!	initialize the longan nano GPIO LEDs
        /***************************************************************************/

        static inline bool toggle( Leds::Color color )
        {
            //----------------------------------------------------------------
            //	VARS
            //----------------------------------------------------------------

            //----------------------------------------------------------------
            //	BODY
            //----------------------------------------------------------------

            //Switch: Color
            switch(color)
            {
                case (Leds::Color::RED):
                {
                    gpio_bit_write(Led_gpio::RED_GPIO, Led_pin::RED_PIN, (bit_status)(1-gpio_input_bit_get(Led_gpio::RED_GPIO, Led_pin::RED_PIN)));
                    break;
                }
                case (Leds::Color::GREEN):
                {
                    gpio_bit_write(Led_gpio::GREEN_GPIO, Led_pin::GREEN_PIN, (bit_status)(1-gpio_input_bit_get(Led_gpio::GREEN_GPIO, Led_pin::GREEN_PIN)));
                    break;
                }
                case (Leds::Color::BLUE):
                {
                    gpio_bit_write(Led_gpio::BLUE_GPIO, Led_pin::BLUE_PIN, (bit_status)(1-gpio_input_bit_get(Led_gpio::BLUE_GPIO, Led_pin::BLUE_PIN)));
                    break;
                }
                default:
                {
                    return true;	//FAIL
                }
            }	//End Switch: Color

            //----------------------------------------------------------------
            //	RETURN
            //----------------------------------------------------------------

            return false;	//OK
        }	//End public static method: toggle

        /***************************************************************************/
        //!	@brief public static method
        //!	clear |
        /***************************************************************************/
        //! @return bool | false = OK | true = ERR
        //! @details
        //!	initialize the longan nano GPIO LEDs
        /***************************************************************************/

        static inline bool clear( Leds::Color color )
        {
            //----------------------------------------------------------------
            //	VARS
            //----------------------------------------------------------------

            //----------------------------------------------------------------
            //	BODY
            //----------------------------------------------------------------

            //Switch: Color
            switch(color)
            {
                case (Leds::Color::RED):
                {
                    gpio_bit_set(Led_gpio::RED_GPIO, Led_pin::RED_PIN);
                    break;
                }
                case (Leds::Color::GREEN):
                {
                    gpio_bit_set(Led_gpio::GREEN_GPIO, Led_pin::GREEN_PIN);
                    break;
                }
                case (Leds::Color::BLUE):
                {
                    gpio_bit_set(Led_gpio::BLUE_GPIO, Led_pin::BLUE_PIN);
                    break;
                }
                default:
                {
                    return true;	//FAIL
                }
            }	//End Switch: Color

            //----------------------------------------------------------------
            //	RETURN
            //----------------------------------------------------------------

            return false;	//OK
        }	//End public static method: clear

        /***************************************************************************/
        //!	@brief public static method
        //!	set |
        /***************************************************************************/
        //! @return bool | false = OK | true = ERR
        //! @details
        //!	initialize the longan nano GPIO LEDs
        /***************************************************************************/

        static inline bool set( Leds::Color color )
        {
            //----------------------------------------------------------------
            //	VARS
            //----------------------------------------------------------------

            //----------------------------------------------------------------
            //	BODY
            //----------------------------------------------------------------

            //Switch: Color
            switch(color)
            {
                case (Leds::Color::RED):
                {
                    gpio_bit_reset(Led_gpio::RED_GPIO, Led_pin::RED_PIN);
                    break;
                }
                case (Leds::Color::GREEN):
                {
                    gpio_bit_reset(Led_gpio::GREEN_GPIO, Led_pin::GREEN_PIN);
                    break;
                }
                case (Leds::Color::BLUE):
                {
                    gpio_bit_reset(Led_gpio::BLUE_GPIO, Led_pin::BLUE_PIN);
                    break;
                }
                default:
                {
                    return true;	//FAIL
                }
            }	//End Switch: Color

            //----------------------------------------------------------------
            //	RETURN
            //----------------------------------------------------------------

            return false;	//OK
        }	//End public static method: set

        //--------------------------------------------------------------------------
        //	PUBLIC STATIC METHODS
        //--------------------------------------------------------------------------

        //--------------------------------------------------------------------------
        //	PUBLIC VARS
        //--------------------------------------------------------------------------

    //Visible to derived classes
    protected:
        //--------------------------------------------------------------------------
        //	PROTECTED METHODS
        //--------------------------------------------------------------------------

        //--------------------------------------------------------------------------
        //	PROTECTED VARS
        //--------------------------------------------------------------------------

    //Visible only inside the class
    private:
        //--------------------------------------------------------------------------
        //	PRIVATE METHODS
        //--------------------------------------------------------------------------

        //--------------------------------------------------------------------------
        //	PRIVATE VARS
        //--------------------------------------------------------------------------

};	//End Class: Longan_nano_led

/**********************************************************************************
**	NAMESPACE
**********************************************************************************/

} //End Namespace: Longan_nano

#else
    #warning "Multiple inclusion of hader file LONGAN_NANO_LED_H_"
#endif
