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

#ifndef LONGAN_NANO_CHRONO_H_
    #define LONGAN_NANO_CHRONO_H_

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

//! @namespace Longan_nano namespace encapsulating all related drivers and HAL
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
//! @class 		Chrono
/************************************************************************************/
//!	@author		Orso Eric
//! @version	2020-07-20
//! @brief		Chrono. Deals with busy delays and timestamps. Use SysTick timer.
//! @bug		None
//! @copyright	MIT 2020 Orso Eric
//! @details
//!	SysTick
//!	History Version \n
//! 	2020-07-20\n
//!	Rework Utils library into Chrono library to add start/stop timer
/************************************************************************************/

class Chrono
{
    //Visible to all
    public:
        //--------------------------------------------------------------------------
        //	ENUM
        //--------------------------------------------------------------------------

        //Configurations of the SysTick
        typedef enum _Config
        {
            systick_invalid	= (uint64_t)0xffffffff,				//Invalid timer value
            pre 			= 4,								//Prescaler for the systick timer
            pedantic_checks	= false,							//Pedantic checks inside the functions
        } Config;

        //--------------------------------------------------------------------------
        //	CONSTRUCTORS
        //--------------------------------------------------------------------------

        /***************************************************************************/
        //!	@brief Empty Constructor
        //!	Chrono | void
        /***************************************************************************/
        //! @return void
        //! @details \n
        //!	Initialize timer to invalid
        /***************************************************************************/
        
        Chrono( void )
        {
            //Initialize timer to invalid
            this -> g_systick_start	= Config::systick_invalid;
            this -> g_systick_stop	= Config::systick_invalid;
            
            //----------------------------------------------------------------
            //	RETURN
            //----------------------------------------------------------------
            
            return;
        }	//End Constructor: Chrono | void

        //--------------------------------------------------------------------------
        //	DESTRUCTORS
        //--------------------------------------------------------------------------

        /***************************************************************************/
        //!	@brief Empty Destructor
        /***************************************************************************/
        //!	~Chrono | void
        //! @return void
        /***************************************************************************/

        ~Chrono( void )
        {
            //----------------------------------------------------------------
            //	RETURN
            //----------------------------------------------------------------

            return;
        }

        //--------------------------------------------------------------------------
        //	OPERATORS
        //--------------------------------------------------------------------------

        //--------------------------------------------------------------------------
        //	SETTERS
        //--------------------------------------------------------------------------
        
        /***************************************************************************/
        //!	@brief public setter
        //!	start | void
        /***************************************************************************/
        //! @return void
        //! @details \n
        //!	Start the timer
        /***************************************************************************/

        void start( void )
        {
            //----------------------------------------------------------------
            //	BODY
            //----------------------------------------------------------------		
            
            //Snap timer start
            this -> g_systick_start = get_timer_value();
            
            //----------------------------------------------------------------
            //	RETURN
            //----------------------------------------------------------------
            
            return;
        }	//End setter: start | void
        
        /***************************************************************************/
        //!	@brief public setter
        //!	stop | void
        /***************************************************************************/
        //! @return void
        //! @details \n
        //!	Stop the timer. Snap the stop time
        /***************************************************************************/

        void stop( void )
        {
            //----------------------------------------------------------------
            //	BODY
            //----------------------------------------------------------------		
            
            //Snap timer start
            this -> g_systick_stop = get_timer_value();
            
            //----------------------------------------------------------------
            //	RETURN
            //----------------------------------------------------------------
            
            return;
        }	//End setter: stop | void

        //--------------------------------------------------------------------------
        //	GETTERS
        //--------------------------------------------------------------------------

        /***************************************************************************/
        //!	@brief public setter
        //!	elapsed_ms | void
        /***************************************************************************/
        //! @return int | elapsed milliseconds. negative mean the timer was uninitialized
        //! @details \n
        //!	Time elapsed between start and stop
        /***************************************************************************/

        int elapsed_ms( void )
        {
            //----------------------------------------------------------------
            //	VARS
            //----------------------------------------------------------------		
            
            uint64_t systick_tmp;
            int ret;
            //Ticks required to count 1uS
            int numticks_ms		= SystemCoreClock /1000 /pre;	
            
            //----------------------------------------------------------------
            //	BODY
            //----------------------------------------------------------------		
            
            //If: a timetamp is invalid
            if ((this -> g_systick_start == Config::systick_invalid) || (this -> g_systick_stop == Config::systick_invalid))
            {
                return -1;	//Invalid
            }
            
            //Compute DeltaT
            systick_tmp = this -> g_systick_stop -this -> g_systick_start;
            //@todo: add rounding
            //Compute elapsed time in ms
            systick_tmp /= numticks_ms;
            ret = systick_tmp;
            
            //----------------------------------------------------------------
            //	RETURN
            //----------------------------------------------------------------
            
            return ret;
        }	//End setter: elapsed_ms | void

        /***************************************************************************/
        //!	@brief public setter
        //!	elapsed_us | void
        /***************************************************************************/
        //! @return int | elapsed microseconds. negative mean the timer was uninitialized
        //! @details \n
        //!	Time elapsed between start and stop
        /***************************************************************************/

        int elapsed_us( void )
        {
            //----------------------------------------------------------------
            //	VARS
            //----------------------------------------------------------------		
            
            uint64_t systick_tmp;
            int ret;
            //Ticks required to count 1uS
            int numticks_us		= SystemCoreClock /1000000 /pre;	
            
            //----------------------------------------------------------------
            //	BODY
            //----------------------------------------------------------------		
            
            //If: a timetamp is invalid
            if ((this -> g_systick_start == Config::systick_invalid) || (this -> g_systick_stop == Config::systick_invalid))
            {
                return -1;	//Invalid
            }
            
            //Compute DeltaT
            systick_tmp = this -> g_systick_stop -this -> g_systick_start;
            //@todo: add rounding
            //Compute elapsed time in ms
            systick_tmp /= numticks_us;
            ret = systick_tmp;
            
            //----------------------------------------------------------------
            //	RETURN
            //----------------------------------------------------------------
            
            return ret;
        }	//End setter: elapsed_us | void

        /***************************************************************************/
        //!	@brief public static method
        //!	get_systick_freq | void
        /***************************************************************************/
        //! @return unsigned int | frequency of the SysTick timer
        //! @details \n
        //!	The SysTick timer is tied to the CPU clock prescaled by four
        /***************************************************************************/

        static unsigned int get_systick_freq( void )
        {
            //----------------------------------------------------------------
            //	RETURN
            //----------------------------------------------------------------
            
            return SystemCoreClock /Config::pre;
        }	//End static method: get_systick_freq | void

        //--------------------------------------------------------------------------
        //	TESTERS
        //--------------------------------------------------------------------------

        //--------------------------------------------------------------------------
        //	PUBLIC METHODS
        //--------------------------------------------------------------------------

        //--------------------------------------------------------------------------
        //	PUBLIC STATIC METHODS
        //--------------------------------------------------------------------------

        /***************************************************************************/
        //!	@brief public static method
        //!	delay_ms | unsigned int
        /***************************************************************************/
        //!	@param delay_ms | unsigned int | how long to wait for in milliseconds
        //! @return void |
        //! @details \n
        //!	Use the SysTick timer counter to busy wait for the correct number of microseconds
        //!	The CPU SysTick timer is clocked by the ABH clock/4 = 27MHz
        //!	SystemCoreClock defines the frequency of the CPU in Hz
        /***************************************************************************/

        static void delay_ms( unsigned int delay_ms )
        {
            //----------------------------------------------------------------
            //	VARS
            //----------------------------------------------------------------

            //Temp timestamp
            uint64_t systick_tmp;
            //Compute final timestamp
            uint64_t systick_stop;
            //Ticks required to count 1mS
            int numticks_ms		= SystemCoreClock /1000 /pre;	

            //----------------------------------------------------------------
            //	BODY
            //----------------------------------------------------------------
            //	Wait for the correct number of ticks

            //Snap start
            systick_stop = get_timer_value();
            //Compute stop time. 
            systick_stop += (uint64_t)numticks_ms *delay_ms;
            //Wait an additional tick for current tick
            systick_stop++;
            //Busy wait for time to pass
            do
            {
                //Snap timestamp
                systick_tmp = get_timer_value();
            }
            while( systick_tmp < systick_stop );

            //----------------------------------------------------------------
            //	RETURN
            //----------------------------------------------------------------

            return;
        }	//End static method: delay_ms | unsigned int

        /***************************************************************************/
        //!	@brief public static method
        //!	delay_us | unsigned int
        /***************************************************************************/
        //!	@param delay_us | unsigned int | how long to wait for in microseconds
        //! @return void |
        //! @details \n
        //!	Use the SysTick timer counter to busy wait for the correct number of microseconds
        //!	The CPU SysTick timer is clocked by the ABH clock/4 = 27MHz
        //!	SystemCoreClock defines the frequency of the CPU in Hz
        /***************************************************************************/

        static void delay_us( unsigned int delay_us )
        {
            //----------------------------------------------------------------
            //	VARS
            //----------------------------------------------------------------

            //Temp timestamp
            uint64_t systick_tmp;
            //Compute final timestamp
            uint64_t systick_stop;
            //Ticks required to count 1uS
            int numticks_us		= SystemCoreClock /1000000 /pre;	

            //----------------------------------------------------------------
            //	BODY
            //----------------------------------------------------------------
            //	Wait for the correct number of ticks

            //Snap start
            systick_stop = get_timer_value();
            //Compute stop time. 
            systick_stop += (uint64_t)numticks_us *delay_us;
            //Wait an additional tick for current tick
            systick_stop++;
            //Busy wait for time to pass
            do
            {
                //Snap timestamp
                systick_tmp = get_timer_value();
            }
            while( systick_tmp < systick_stop );

            //----------------------------------------------------------------
            //	RETURN
            //----------------------------------------------------------------

            return;
        }	//End static method: delay_us | unsigned int

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

        //Systick Timestamps
        uint64_t g_systick_start;
        uint64_t g_systick_stop;
};	//End Class: Chrono

/**********************************************************************************
**	NAMESPACE
**********************************************************************************/

} //End Namespace: Longan_nano

#else
    #warning "Multiple inclusion of hader file LONGAN_NANO_CHRONO_H_"
#endif
