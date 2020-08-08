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
//! @version	2020-08-08
//! @brief		Deals with busy delays, elapsed time and accumulated time
//! @bug		None
//! @copyright	BSD 3-Clause License Copyright (c) 2020, Orso Eric
//! @details
//! \n	SysTick
//! \n	Use 64b 27MHz SysTick timer.
//! \n	History Version
//! \n 	2020-07-20
//! \n	Rework Utils library into Chrono library to add start/stop timer
//! \n	"start" snaps the start timestamp at full resolution
//! \n	"stop" snaps the stop timestamp at full resolution
//! \n	"elapsed" returns the elapsed time between stop and start if valid. zero otherwise
//! \n		2020-07-28
//! \n	I need a method to accumulate execution time and profile how long an activity has taken
//! \n	"accumulate" add stop-start to an internal accumulator at full resolution
//! \n	Add combined "stop" "elapsed" implementation with better performance and fewer calls needed
//! \n I can combine the stop and accumulator counters since i use one or the other
//! \n I use a flag to handle initialization and invalid when switching between modes and automatically reset the accumulator
//! \n		2020-08-04
//! \n	Refactor to separate header and implementation
/************************************************************************************/

class Chrono
{
    //Visible to all
    public:
        /*********************************************************************************************************************************************************
        **********************************************************************************************************************************************************
        **	PUBLIC ENUM
        **********************************************************************************************************************************************************
        *********************************************************************************************************************************************************/

        //! @brief Configurations of the SysTick
        typedef enum _Config
        {
            PEDANTIC_CHECKS	= false,				//Pedantic checks inside the functions
            SYSTICK_INVALID	= 0xFFFFFFFF,	//Invalid timer value
            SYSTICK_PRE 	= 4,					//Prescaler for the systick timer
            TIME_INVALID	= -1,					//Return time in case time is invalid
        } Config;
        //! @brief Possible time units. Same names as std::Chrono
        typedef enum _Unit
        {
            milliseconds,
            microseconds,
        } Unit;

        /*********************************************************************************************************************************************************
        **********************************************************************************************************************************************************
        **	CONSTRUCTORS
        **********************************************************************************************************************************************************
        *********************************************************************************************************************************************************/

        //Empty Constructor
        Chrono( void );

        /*********************************************************************************************************************************************************
        **********************************************************************************************************************************************************
        **	DESTRUCTORS
        **********************************************************************************************************************************************************
        *********************************************************************************************************************************************************/

        //Empty Destructor
        ~Chrono( void );

        /*********************************************************************************************************************************************************
        **********************************************************************************************************************************************************
        **	PUBLIC METHOD
        **********************************************************************************************************************************************************
        *********************************************************************************************************************************************************/
        
        //Snap the start time
        void start( void );
        //Snap the start time and invalidate the stop time and accumulator
        void restart( void );
        //Snap the stop time
        void stop( void );
        //Snap the stop time and return the elapsed time
        int32_t stop( Unit unit );
        //Accumulate DeltaT
        bool accumulate( void );
        //Accumulate DeltaT and return the accumulator time
        int32_t accumulate( Unit unit );
        //Return the elapsed time
        int32_t get_elapsed( Unit unit );
        //Return the accumulator time
        int32_t get_accumulator( Unit unit );
        
        /*********************************************************************************************************************************************************
        **********************************************************************************************************************************************************
        **	PUBLIC STATIC METHOD
        **********************************************************************************************************************************************************
        *********************************************************************************************************************************************************/

        //Systick timer frequency
        static unsigned int get_systick_freq( void );
        //Busy wait. Blocking function
        static bool delay( Unit unit, unsigned int delay_tmp );

    //Visible only inside the class
    private:
        /*********************************************************************************************************************************************************
        **********************************************************************************************************************************************************
        **	PRIVATE METHODS
        **********************************************************************************************************************************************************
        *********************************************************************************************************************************************************/

        //Compute the number of systick counts needed to count one time unit
        static uint32_t compute_tick_per_time_unit( Unit unit );
        //use start and stop timestamp to compute the elapsed time in a given time unit
        int32_t compute_elapsed( uint64_t start, uint64_t stop, Unit unit );
        //use start and stop timestamp to compute the elapsed time in a given time unit
        int32_t compute_accumulator( uint64_t accumulator, Unit unit );

        /*********************************************************************************************************************************************************
        **********************************************************************************************************************************************************
        **	PRIVATE VARS
        **********************************************************************************************************************************************************
        *********************************************************************************************************************************************************/

        //true = accumulator mode | false = regular start stop mode
        bool g_f_accumulator_mode;
        //Systick Timestamps
        uint64_t g_systick_start;
        //Combined stop and accumulator counter
        uint64_t g_systick_stop;
};	//End Class: Chrono

    /*********************************************************************************************************************************************************
    **********************************************************************************************************************************************************
    **	CONSTRUCTORS
    **********************************************************************************************************************************************************
    *********************************************************************************************************************************************************/

/***************************************************************************/
//!	@brief Empty Constructor
//!	Chrono | void
/***************************************************************************/
//! @return void
//! @details
//!	\n Initialize timestamps to invalid
/***************************************************************************/

Chrono::Chrono( void )
{
    //Initialize timestamps to invalid
    this -> g_systick_start = Config::SYSTICK_INVALID;
    this -> g_systick_stop = Config::SYSTICK_INVALID;
    //Regular timer mode
    this -> g_f_accumulator_mode = false;
    
    //----------------------------------------------------------------
    //	RETURN
    //----------------------------------------------------------------
    
    return;
}	//End Constructor: Chrono | void

    /*********************************************************************************************************************************************************
    **********************************************************************************************************************************************************
    **	DESTRUCTORS
    **********************************************************************************************************************************************************
    *********************************************************************************************************************************************************/

/***************************************************************************/
//!	@brief Empty Destructor
/***************************************************************************/
//!	~Chrono | void
//! @return void
/***************************************************************************/

Chrono::~Chrono( void )
{
    //----------------------------------------------------------------
    //	RETURN
    //----------------------------------------------------------------

    return;
}

    /*********************************************************************************************************************************************************
    **********************************************************************************************************************************************************
    **	PUBLIC METHOD
    **********************************************************************************************************************************************************
    *********************************************************************************************************************************************************/

/***************************************************************************/
//!	@brief public method
//!	start | void
/***************************************************************************/
//! @return void
//! @details \n
//! \n	Start the timer
/***************************************************************************/

void Chrono::start( void )
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
}	//End method: start | void

/***************************************************************************/
//!	@brief public method
//!	start | void
/***************************************************************************/
//! @return void
//! @details \n
//! \n	Start the timer and invalidate the stop time and accumulator
/***************************************************************************/

void Chrono::restart( void )
{
    //----------------------------------------------------------------
    //	BODY
    //----------------------------------------------------------------		
    
    //Snap timer start
    this -> g_systick_start = get_timer_value();
    //Infalidate stop
    this -> g_systick_stop = Config::SYSTICK_INVALID;
    this -> g_f_accumulator_mode = false;

    //----------------------------------------------------------------
    //	RETURN
    //----------------------------------------------------------------
    
    return;
}	//End method: start | void

/***************************************************************************/
//!	@brief public method
//!	stop | void
/***************************************************************************/
//! @return void
//! @details \n
//! \n	Stop the timer. Snap the stop time
/***************************************************************************/

void Chrono::stop( void )
{
    //----------------------------------------------------------------
    //	BODY
    //----------------------------------------------------------------		
    
    //Snap timer start
    this -> g_systick_stop = get_timer_value();
    //Regular timer mode
    this -> g_f_accumulator_mode = false;
    
    //----------------------------------------------------------------
    //	RETURN
    //----------------------------------------------------------------
    
    return;
}	//End method: stop | void

/***************************************************************************/
//!	@brief public method
//!	stop | Unit |
/***************************************************************************/
//!	@param unit | Unit | result is given in this time unit
//! @return void
//! @details \n
//! \n	Stop the timer. Snap the stop time
//! \n	Return the elapsed time between stop and start in the given unit
/***************************************************************************/

int32_t Chrono::stop( Unit unit )
{
    //----------------------------------------------------------------
    //	CHECK
    //----------------------------------------------------------------
    
    //Get start time
    uint64_t start_tmp = this -> g_systick_start;
    //If: bad timestamp
    if ((Config::PEDANTIC_CHECKS == true) && (start_tmp == Config::SYSTICK_INVALID))
    {
        return Config::TIME_INVALID; //FAIL
    }
    
    //----------------------------------------------------------------
    //	BODY
    //----------------------------------------------------------------		
    
    //Snap the timestamp
    uint64_t stop_tmp = get_timer_value();
    //Record the stop timestamp inside the timer
    this -> g_systick_stop = stop_tmp;
    //Regular timer mode
    this -> g_f_accumulator_mode = false;

    //----------------------------------------------------------------
    //	RETURN
    //----------------------------------------------------------------
    //return elapsed time
    return this -> compute_elapsed( start_tmp, stop_tmp, unit );
}	//End method: stop | Unit |

/***************************************************************************/
//!	@brief public method
//!	accumulate | void |
/***************************************************************************/
//! @return unsigned int | frequency of the SysTick timer
//! @details \n
//! \n Snap the stop time
//! \n	Accumulate the difference between stop and start inside the accumulator
//! \n Swap the stop and start, invalidate the stop. Prepare for next cycle
//! \n Use stop counter as accumulator
//! \n If timer was in timer mode, reset the accumulator
/***************************************************************************/

bool Chrono::accumulate( void )
{
    //----------------------------------------------------------------
    //	CHECK
    //----------------------------------------------------------------
    
    //Get start time
    uint64_t start_tmp = this -> g_systick_start;
    //If: bad timestamp
    if ((Config::PEDANTIC_CHECKS == true) && (start_tmp == Config::SYSTICK_INVALID))
    {
        return true; //FAIL
    }
    //Temp accumulator
    uint64_t accumulator_tmp;
    //if: Regular timer mode
    if (this -> g_f_accumulator_mode == false)
    {
        //reset the accumulator
        accumulator_tmp = 0;
        //go into accumulator mode
        this -> g_f_accumulator_mode = true;
    }
    //If: accumulator mode
    else
    {
        //Fetch the current accumulator count
        accumulator_tmp = this -> g_systick_stop;
    }

    //----------------------------------------------------------------
    //	BODY
    //----------------------------------------------------------------

    //Snap the timestamp
    uint64_t stop_tmp = get_timer_value();
    //Record the stop timestamp inside the start timestamp and invalidate the stop timestamp
    this -> g_systick_start = stop_tmp;
    //Accumulate the DeltaT inside the accumulator at full resolution
    accumulator_tmp += stop_tmp -start_tmp;
    //Store the accumulator value
    this -> g_systick_stop = accumulator_tmp;

    //----------------------------------------------------------------
    //	RETURN
    //----------------------------------------------------------------
    
    return false; //OK
}	//End public method: accumulate | void |

/***************************************************************************/
//!	@brief public method
//!	accumulate | Unit unit |
/***************************************************************************/
//!	@param unit | Unit | result is given in this time unit
//! @return unsigned int | frequency of the SysTick timer
//! @details \n
//! \n Snap the stop time
//! \n	Accumulate the difference between stop and start inside the accumulator
//! \n Swap the stop and start, invalidate the stop. Prepare for next cycle
/***************************************************************************/

int32_t Chrono::accumulate( Unit unit )
{
    //----------------------------------------------------------------
    //	CHECK
    //----------------------------------------------------------------
    
    //Get start time
    uint64_t start_tmp = this -> g_systick_start;
    //If: bad timestamp
    if ((Config::PEDANTIC_CHECKS == true) && (start_tmp == Config::SYSTICK_INVALID))
    {
        return true; //FAIL
    }
    //Temp accumulator
    uint64_t accumulator_tmp;
    //if: Regular timer mode
    if (this -> g_f_accumulator_mode == false)
    {
        //reset the accumulator
        accumulator_tmp = 0;
        //go into accumulator mode
        this -> g_f_accumulator_mode = true;
    }
    //If: accumulator mode
    else
    {
        //Fetch the current accumulator count
        accumulator_tmp = this -> g_systick_stop;
    }

    //----------------------------------------------------------------
    //	BODY
    //----------------------------------------------------------------

    //Snap the timestamp
    uint64_t stop_tmp = get_timer_value();
    //Record the stop timestamp inside the start timestamp and invalidate the stop timestamp
    this -> g_systick_start = stop_tmp;
    //Accumulate the DeltaT inside the accumulator at full resolution
    accumulator_tmp += stop_tmp -start_tmp;
    //Store the accumulator value
    this -> g_systick_stop = accumulator_tmp;

    //----------------------------------------------------------------
    //	RETURN
    //----------------------------------------------------------------
    
    return this -> compute_accumulator( accumulator_tmp, unit );
}	//End public method: accumulate | void |

/***************************************************************************/
//!	@brief public getter
//!	get_elapsed | Unit |
/***************************************************************************/
//! @param unit | Unit | result is given in this time unit
//! @return int | elapsed time. negative mean the timer was uninitialized
//! @details \n
//! \n	Time elapsed between start and stop
/***************************************************************************/

int32_t Chrono::get_elapsed( Unit unit )
{
    //----------------------------------------------------------------
    //	VARS
    //----------------------------------------------------------------

    //Fetch timestamps
    uint64_t start_tmp = this -> g_systick_start;
    uint64_t stop_tmp = this -> g_systick_stop;

    //----------------------------------------------------------------
    //	CHECKS
    //----------------------------------------------------------------
    
    //If: a timetamp is invalid
    if ((start_tmp == Config::SYSTICK_INVALID) || (stop_tmp == Config::SYSTICK_INVALID))
    {
        return Config::TIME_INVALID;	//Invalid
    }
    //If: accumulator mode
    if (this -> g_f_accumulator_mode == true)
    {
        return Config::TIME_INVALID;	//Invalid
    }

    //----------------------------------------------------------------
    //	RETURN
    //----------------------------------------------------------------
    //return elapsed time
    return compute_elapsed( start_tmp, start_tmp, unit );
}	//End public getter: get_elapsed |  Unit |

/***************************************************************************/
//!	@brief public getter
//!	get_accumulator | Unit |
/***************************************************************************/
//!	@param unit | Unit | result is given in this time unit
//! @return int | accumulators. negative mean the timer was uninitialized
//! @details \n
//! \n return the DeltaT accumulated by the accumulate function in the given time unit
/***************************************************************************/

inline int32_t Chrono::get_accumulator( Unit unit )
{
    //----------------------------------------------------------------
    //	CHECK
    //----------------------------------------------------------------
    //If: accumulator mode
    if (this -> g_f_accumulator_mode == false)
    {
        return Config::TIME_INVALID;	//Invalid
    }

    //----------------------------------------------------------------
    //	RETURN
    //----------------------------------------------------------------
    //return accumulated time
    return this -> compute_accumulator( this -> g_systick_stop, unit );
}	//End public getter: get_elapsed |  Unit |

    /*********************************************************************************************************************************************************
    **********************************************************************************************************************************************************
    **	PUBLIC STATIC METHOD
    **********************************************************************************************************************************************************
    *********************************************************************************************************************************************************/

/***************************************************************************/
//!	@brief public static method
//!	get_systick_freq | void
/***************************************************************************/
//! @return unsigned int | frequency of the SysTick timer
//! @details \n
//! \n The SysTick timer is tied to the CPU clock prescaled by four
/***************************************************************************/

unsigned int Chrono::get_systick_freq( void )
{
    //----------------------------------------------------------------
    //	RETURN
    //----------------------------------------------------------------
    
    return SystemCoreClock /Config::SYSTICK_PRE;
}	//End static method: get_systick_freq | void

/***************************************************************************/
//!	@brief public static method
//!	delay | Unit | unsigned int |
/***************************************************************************/
//!	@param unit | Unit | timeunit of the delay
//!	@param delay_tmp | unsigned int | how long to wait for
//! @return bool | false = OK | true = fail
//! @details \n
//! \n	Use the SysTick timer counter to busy wait for the correct number of microseconds
//! \n	The CPU SysTick timer is clocked by the ABH clock/4 = 27MHz
//! \n	SystemCoreClock defines the frequency of the CPU in Hz
/***************************************************************************/

bool Chrono::delay( Unit unit, unsigned int delay_tmp )
{
    //----------------------------------------------------------------
    //	VARS
    //----------------------------------------------------------------

    //Temp timestamp
    uint64_t systick_tmp;
    //Compute final timestamp
    uint64_t systick_stop;
    //Ticks required to count 1mS
    uint32_t numticks = compute_tick_per_time_unit( unit );
    //If: bad unit
    if (numticks == 0)
    {
        return true; //fail
    }

    //----------------------------------------------------------------
    //	BODY
    //----------------------------------------------------------------
    //	Wait for the correct number of ticks

    //Snap start
    systick_stop = get_timer_value();
    //Compute stop time. 
    systick_stop += numticks *delay_tmp;
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

    return false; //OK
}	//End public static method: delay | Unit | unsigned int |

    /*********************************************************************************************************************************************************
    **********************************************************************************************************************************************************
    **	PRIVATE METHODS
    **********************************************************************************************************************************************************
    *********************************************************************************************************************************************************/

/***************************************************************************/
//!	@brief private method
//!	compute_tick_per_time_unit | Unit |
/***************************************************************************/
//! @param unit | Unit | result is given in this time unit
//! @return uint32_t | number of systick counts needed to count one time unit
//! @details
//!	\n Compute the number of systick counts needed to count one time unit
/***************************************************************************/

inline uint32_t Chrono::compute_tick_per_time_unit( Unit unit )
{
    //----------------------------------------------------------------
    //	BODY
    //----------------------------------------------------------------
    //	Compute del

    //Switch: Time unit
    switch( unit )
    {
        case Unit::milliseconds:
        {
            return SystemCoreClock /1000 /Config::SYSTICK_PRE;
            break;
        }
        case Unit::microseconds:
        {
            return SystemCoreClock /1000000 /Config::SYSTICK_PRE;
            break;
        }
        //Unhandled time unit
        default:
        {
            return 0;   //Invalid number of systick counts. Using it will yield infinite time
        }
    };	//End switch: Time unit

    //----------------------------------------------------------------
    //	RETURN
    //----------------------------------------------------------------

    return 0;   //Invalid number of systick counts. Using it will yield infinite time
}	//End private method: compute_tick_per_time_unit | Unit |

/***************************************************************************/
//!	@brief private method
//!	compute_elapsed | uint64_t | uint64_t | Unit |
/***************************************************************************/
//! @param start | uint64_t | timestamp
//! @param stop | uint64_t | timestamp
//! @param unit | Unit | result is given in this time unit
//! @return int32_t | negative = invalid | zero or positive = elapsed time in the given time unit
//! @details \n
//!	\n use start and stop timestamp to compute the elapsed time in a given time unit
/***************************************************************************/

inline int32_t Chrono::compute_elapsed( uint64_t start, uint64_t stop, Unit unit )
{
    //----------------------------------------------------------------
    //	VARS
    //----------------------------------------------------------------

    //If: causality violation
    if ((Config::PEDANTIC_CHECKS == true) && (start > stop))
    {
        //Hopefully the timestamps are wrong and the universe still works as intended
        return Config::TIME_INVALID; //FAIL
    }
    
    //----------------------------------------------------------------
    //	BODY
    //----------------------------------------------------------------		
    
    //SysTick counts in one time unit
    uint32_t numticks_time_unit = this -> compute_tick_per_time_unit( unit );
    //If: bad unit was provided
    if ((Config::PEDANTIC_CHECKS == true) && (numticks_time_unit == 0))
    {
        return TIME_INVALID;
    }
    //Compute DeltaT in system ticks as stop-start
    uint64_t deltat = stop -start;
    //Compute DeltaT in time units
    deltat /= numticks_time_unit;
    //Demote
    int32_t ret = deltat;

    //----------------------------------------------------------------
    //	RETURN
    //----------------------------------------------------------------

    return ret;
}	//End private method: compute_elapsed | uint64_t | uint64_t | Unit |

/***************************************************************************/
//!	@brief private method
//!	compute_accumulator | uint64_t | Unit |
/***************************************************************************/
//! @param accumulator | uint64_t | timestamp DeltaT
//! @param unit | Unit | result is given in this time unit
//! @return int32_t | negative = invalid | zero or positive = elapsed time in the given time unit
//! @details \n
//!	use start and stop timestamp to compute the elapsed time in a given time unit
/***************************************************************************/

int32_t Chrono::compute_accumulator( uint64_t accumulator, Unit unit )
{
    //----------------------------------------------------------------
    //	VARS
    //----------------------------------------------------------------

    //If: accumulator
    if ((Config::PEDANTIC_CHECKS == true) && (accumulator == Config::SYSTICK_INVALID))
    {
        return TIME_INVALID;
    }
    
    //----------------------------------------------------------------
    //	BODY
    //----------------------------------------------------------------		
    
    //SysTick counts in one time unit
    uint32_t numticks_time_unit = this -> compute_tick_per_time_unit( unit );
    //If: bad unit was provided
    if ((Config::PEDANTIC_CHECKS == true) && (numticks_time_unit == 0))
    {
        return TIME_INVALID;
    }
    //Compute DeltaT in time units
    accumulator /= numticks_time_unit;
    //Demote
    int32_t ret = accumulator;

    //----------------------------------------------------------------
    //	RETURN
    //----------------------------------------------------------------

    return ret;
}	//End private method: compute_accumulator | uint64_t | Unit |

/**********************************************************************************
**	NAMESPACE
**********************************************************************************/

} //End Namespace: Longan_nano

#else
    #warning "Multiple inclusion of hader file LONGAN_NANO_CHRONO_H_"
#endif
