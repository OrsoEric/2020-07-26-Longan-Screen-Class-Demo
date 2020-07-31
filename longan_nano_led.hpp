/**********************************************************************************
BSD 3-Clause License

Copyright (c) 2020, Orso Eric
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this
   list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its
   contributors may be used to endorse or promote products derived from
   this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
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
//! @class 		Leds
/************************************************************************************/
//!	@author		Orso Eric
//! @version	2020-07-15
//! @date		2020-07-15
//! @brief		Leds
//! @copyright	BSD 3-Clause License Copyright (c) 2020, Orso Eric
//! @todo		todo list
//! @details
//!	Class to control the LEDs on the Longan Nano board
//! Experiment with scoping of enum and typedef
//! Can do better...
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
