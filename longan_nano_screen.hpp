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

#ifndef SCREEN_HPP_
    #define SCREEN_HPP_

/**********************************************************************************
**	GLOBAL INCLUDES
**********************************************************************************/

//String manipulation
#include <cstring>
#include "ST7735S_W160_H80_C16.hpp"

/**********************************************************************************
**	DEBUG
**********************************************************************************/
/*
#ifndef DEBUG_H_
    #include <stdio.h>
    #define ENABLE_DEBUG
    #include "debug.h"
#endif
*/
//If DEBUG is not needed, blank out the implementations
#ifndef DEBUG_H_
    #define DEBUG_VARS_PROTOTYPES()
    #define DEBUG_VARS()
    #define DSHOW( ... )
    #define DSTART( ... )
    #define DSTOP()
    #define DTAB( ... )
    #define DPRINT( ... )
    #define DPRINT_NOTAB( ... )
    #define DENTER( ... )
    #define DRETURN( ... )
    #define DENTER_ARG( ... )
    #define DRETURN_ARG( ... )

    #define show_frame_sprite( ... )
    #define show_pixels( ... )
#endif

/**********************************************************************************
**	DEFINES
**********************************************************************************/

/**********************************************************************************
**	MACROS
**********************************************************************************/

/**********************************************************************************
**	NAMESPACE
**********************************************************************************/

//! @namespace Longan_Nano: specialized for the eval board Longan_nano with GD32VF103 RiscV MCU
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

/**********************************************************************************
**	DESCRIPTION
***********************************************************************************
**		SPRITE
**	Logically devide screen in blocks width*height
**
**		NULL COLOR vs. NULL SPRITE
**	I need at least one combination that forces a sprite not to be updated (transparent)
**
**		GLOBAL BACKGROUND/FOREGROUND
**	SCREEN
**		WIDTH
**	0,0 ---X-->	159,0
**	|
**	Y
**	|
**	v
**	0,79        159,79
**
**********************************************************************************/

/*********************************************************************************/
//! @class 		Dummy
/*********************************************************************************/
//!	@author		Orso Eric
//! @version	0.1 alpha
//! @date		2019/05
//! @brief		Longan Nano Screen Class
//! @warning	No warnings
//! @copyright	MIT 2020 Orso Eric
//! @todo       an ascii character with same background as foreground should be handled as a solid color sprite
//! @details
//!	Verbose description
//! Screen class: abstraction layer for low level LCD HAL for the ST7735S Longan Nano Display
//!	Screen provides a frame buffer and functions to write on it
//!	LCD class: low level driver for screen hardware. Inherited by screen
//!		2020-07-09
//!	Formulated architeture
//!	Focus on Display dirver first
//!		2020-07-23
//!	Display driver working
//!	Core methods: register_sprite and update
//!	Formulated palette, frame sprite and sprite pixel buffer architecture
//!	Use 16x08 sprites. They already exists and 8 by 8 are too small
//!	Structure of the Screen Update FSM Up
//!	FSM is partitioned into the Display side FSM that handles the physical display and the Screen side FSM that handles the sprite based frame buffer
//!	Screen class is designed to massively reduce memory usage and data sent to physical display. Most of the time I need fast updating characters and nothing more.
//!		2020-07-24
//!	Added through debug
//!	BUG: Color decoding macro: FIXED
//! Test clear black and white inherited from display: SUCCESS
//!	clear function tested with background calls from update and periodic calls of clear triggered by the RTC interrupt
//!	Test clear screen cycling colors from the palette: SUCCESS
//!		2020-07-25
//! Test random characters in random position: SUCCESS
//! "is_same" check if two sprites are the same looking for all the nuances
//! "paint" method, meant to draw special graphics sprites
//! "Symbol" enum, meant to provide a few graphical sprites. Few in numbers and meant for light graphics, like progress bars.
//! Test draw a random sprite with a random color:
//!     2020-07-26
//! Test print string functions: SUCCESS
//! Change architecture: do not use RTC ISR but use chrono class for timings. Save a peripheral and achieve a mini hardwired scheduler. PA8 release button switches between demos: SUCCESS
//! 
/*********************************************************************************/

class Screen : Longan_nano::Display
{
    //Visible to all
    public:
        /*********************************************************************************************************************************************************
        **********************************************************************************************************************************************************
        **	PUBLIC ENUM
        **********************************************************************************************************************************************************
        *********************************************************************************************************************************************************/

        //Configuration parameters for the logical screen
        typedef enum _Config
        {
            PEDANTIC_CHECKS			= true,			//Pedantic check meant for debug
            //Screen Logical Configuration. The screen is divided in sprites, shrinking the frame buffer
            SPRITE_WIDTH			= 8,			//Width of a sprite
            SPRITE_HEIGHT			= 16,			//Height of a sprite
            SPRITE_PIXEL_COUNT		= SPRITE_HEIGHT *SPRITE_WIDTH,	//Number of pixels in a sprite
            SPRITE_SCAN_LIMIT		= 5,			//Scan at most # idle sprites before releasing control from update() anyway
            //Special sprite codes
            NUM_SPECIAL_SPRITES		= 5,			//Number of special sprites
            SPRITE_TRANSPARENT		= 0,			//Transparent sprite. Never updated. Ignore update flag.
            SPRITE_BLACK			= 1,			//Always mapped to a full black sprite, invariant to palette index and content
            SPRITE_WHITE			= 2,			//Always mapped to a full white sprite, invariant to palette index and content
            SPRITE_BACKGROUND		= 3,			//Sprite with the background color
            SPRITE_FOREGROUND		= 4,			//Sprite with the foreground color
            //Colors are discretized in a palette
            PALETTE_SIZE			= 16,           //Size of the palette
            PALETTE_SIZE_BIT		= 4,			//Number of bit required to describe a color in the palette
            //Size of the frame buffer
            FRAME_BUFFER_WIDTH		= Longan_nano::Display::Config::WIDTH /SPRITE_WIDTH,
            FRAME_BUFFER_HEIGHT		= Longan_nano::Display::Config::HEIGHT /SPRITE_HEIGHT,
            FRAME_BUFFER_SIZE		= FRAME_BUFFER_WIDTH *FRAME_BUFFER_HEIGHT,
            SPRITE_SIZE				= 128,			//Number of sprites in the sprite table
            SPRITE_SIZE_BIT			= 7				//Size of the sprite table
        } Config;

        //Use the default Color palette. Short hand indexes for user. User can change the palette at will
        typedef enum _Color
        {
            BLACK,
            BLUE,
            GREEN,
            CYAN,
            RED,
            MAGENTA,
            BROWN,
            LGRAY,
            DGRAY,
            LBLUE,
            LGREEN,
            LCYAN,
            LMAGENTA,
            YELLOW,
            WHITE,
        } Color;

        //Enumerate graphical sprites that can be used by the user
        typedef enum _Symbol
        {
            SOLID_BACKGROUND,
            SOLID_FOREGROUND,
            CHECKER_BACKGROUND,
            CHECKER_FOREGROUND,
        } Symbol;

        /*********************************************************************************************************************************************************
        **********************************************************************************************************************************************************
        **	CONSTRUCTORS
        **********************************************************************************************************************************************************
        *********************************************************************************************************************************************************/

        /***************************************************************************/
        //!	@brief empty constructor
        //!	Screen | void
        /***************************************************************************/
        //! @details
        //!	dummy method to copy data
        /***************************************************************************/

        Screen( void ) : Display()
        {
            DENTER();
            //----------------------------------------------------------------
            //	VARS
            //----------------------------------------------------------------

            //----------------------------------------------------------------
            //	BODY
            //----------------------------------------------------------------

            this -> init_class_vars();

            //----------------------------------------------------------------
            //	RETURN
            //----------------------------------------------------------------
            DRETURN();
            return;
        }	//End constructor: Screen | void

        /*********************************************************************************************************************************************************
        **********************************************************************************************************************************************************
        **	DESTRUCTORS
        **********************************************************************************************************************************************************
        *********************************************************************************************************************************************************/

        /***************************************************************************/
        //!	@brief empty destructor
        /***************************************************************************/
        //!	~Screen | void
        //! @details
        //!	dummy method to copy data
        /***************************************************************************/

        ~Screen( void )
        {
            DENTER();
            //----------------------------------------------------------------
            //	VARS
            //----------------------------------------------------------------

            //----------------------------------------------------------------
            //	BODY
            //----------------------------------------------------------------

            //----------------------------------------------------------------
            //	RETURN
            //----------------------------------------------------------------
            DRETURN();
            return;
        }

         /*********************************************************************************************************************************************************
        **********************************************************************************************************************************************************
        **	PUBLIC INIT
        **********************************************************************************************************************************************************
        *********************************************************************************************************************************************************/

        /***************************************************************************/
        //!	@brief public method
        //!	init | void
        /***************************************************************************/
        //! @return bool | false = OK | true = ERR
        //! @details
        //!	call the initializations for the driver
        /***************************************************************************/

        bool init( void )
        {
            DENTER();
            //----------------------------------------------------------------
            //	VARS
            //----------------------------------------------------------------

            //Return flag
            bool f_ret = false;

            //----------------------------------------------------------------
            //	BODY
            //----------------------------------------------------------------

            //
            f_ret = this -> Longan_nano::Display::init();
            //Initialize the frame buffer
            f_ret |= this -> init_frame_buffer();
            //Initialize default palette
            f_ret |= this -> init_palette();
            //Initialize update FSM
            f_ret |= this -> init_fsm();

            this -> Display::clear();

            //----------------------------------------------------------------
            //	RETURN
            //----------------------------------------------------------------
            DRETURN();
            return f_ret;	//OK
        }	//End public method: init | void

        /*********************************************************************************************************************************************************
        **********************************************************************************************************************************************************
        **	PUBLIC METHODS
        **********************************************************************************************************************************************************
        *********************************************************************************************************************************************************/

        /***************************************************************************/
        //!	@brief public method
        //!	update | void
        /***************************************************************************/
        //! @return bool | false = OK | true = ERR
        //! @details
        //!	FSM that synchronize the frame buffer with the display using the driver
        //!	The low level driver exposes control steps used by the high level frame buffer driver
        /***************************************************************************/

        bool update( void )
        {
            DENTER();
            //----------------------------------------------------------------
            //	VARS
            //----------------------------------------------------------------

            //Allows the FSM to run
            bool f_continue = true;
            //Temp FSM status
            Fsm_status status;

            //----------------------------------------------------------------
            //	BODY
            //----------------------------------------------------------------

            //Snap status of the FSM
            status = this -> g_status;
            //While: the Screen FSM is allowed to run
            while (f_continue == true)
            {
                DPRINT("exe: %3d | w: %5d | h: %5d | cnt: %3d\n", status.phase, status.scan_w, status.scan_h, status.cnt );
                ( this -> g_frame_buffer[ status.scan_h ][ status.scan_w ] );
                //If: I'm scanning for the next sprite to be updated
                if (status.phase == Fsm_state::SCAN_SPRITE)
                {
                    //If: the sprite indexed is to be updated
                    if (this -> g_frame_buffer[ status.scan_h ][ status.scan_w ].f_update == true)
                    {
                        //This sprite is not to be updated anymore
                        this -> g_frame_buffer[ status.scan_h ][ status.scan_w ].f_update = false;
                        DPRINT("REFRESH sprite h: %5d | w: %5d\n", status.scan_h, status.scan_w );
                        //If: the sprite is the special transparent sprite
                        if (this -> g_frame_buffer[ status.scan_h ][ status.scan_w ].sprite_index == Config::SPRITE_TRANSPARENT)
                        {
                            //I'm allowed to scan one less sprite
                            status.cnt++;
                        }
                        //If: This sprite is to be updated
                        else
                        {
                            //Register the sprite in the display driver
                            int ret = this -> Display::register_sprite( status.scan_h *Config::SPRITE_HEIGHT, status.scan_w *Config::SPRITE_WIDTH, Config::SPRITE_HEIGHT, Config::SPRITE_WIDTH, g_pixel_data  );
                            //Compute the content of the sprite pixel data to be sent to the display driver from the sprite index and the foreground and background palette indexes
                            this -> build_pixel_data( status.scan_h, status.scan_w );
                            //If: driver failed to register the sprite
                            if (ret <= 0)
                            {
                                DPRINT("ERR: Failed to register sprite\n");
                                //Reset the update FSM
                                this -> init_fsm();
                                f_continue = false;
                            }
                            //If: sprite was successfully registered inside the driver
                            else
                            {
                                //Begin execution of the driver FSM that sends pixel data to the screen
                                status.phase = Fsm_state::SEND_SPRITE;
                                //Count number of execution steps of the Display FSM
                                status.cnt = 0;
                            }
                        }	//End If: This sprite is to be updated
                            //Move on to next sprite
                        //if: space to advance in width
                        if (status.scan_w < Config::FRAME_BUFFER_WIDTH -1)
                        {
                            //Move cursor right
                            status.scan_w++;
                        }
                        //If: space to advance in height
                        else if (status.scan_h < Config::FRAME_BUFFER_HEIGHT -1)
                        {
                            //Get back left
                            status.scan_w = 0;
                            //Move down in height (2° rank of frame buffer vector)
                            status.scan_h++;
                        }

                        //if: scan limit
                        else
                        {
                            //Get back to the top left
                            status.scan_h = 0;
                            status.scan_w = 0;
                        }
                    }	//End If: the sprite indexed is to be updated
                    //If: I'm allowed to scan for more sprites
                    else if (status.cnt < Config::SPRITE_SCAN_LIMIT -1)
                    {
                            //Move on to next sprite
                        //if: space to advance in width
                        if (status.scan_w < Config::FRAME_BUFFER_WIDTH -1)
                        {
                            //Move cursor right
                            status.scan_w++;
                        }
                        //If: space to advance in height
                        else if (status.scan_h < Config::FRAME_BUFFER_HEIGHT -1)
                        {
                            //Get back left
                            status.scan_w = 0;
                            //Move down in height (2� rank of frame buffer vector)
                            status.scan_h++;
                        }

                        //if: scan limit
                        else
                        {
                            //Get back to the top left
                            status.scan_h = 0;
                            status.scan_w = 0;
                        }
                        //I scanned a sprite
                        status.cnt++;
                    }	//End if: I'm allowed to scan for more sprites
                    //If: I reached the scan limit
                    else
                    {
                        //Reset the scan sprite counter
                        status.cnt = 0;
                        //I'm not allowed to scan more sprite. Release execution of the FSM
                        f_continue = false;
                    }	//End If: I reached the scan limit
                }	//End If: I'm scanning for the next sprite to be updated
                //If: I'm in the process of sending a sprite
                else if (status.phase == Fsm_state::SEND_SPRITE)
                {
                    DPRINT("Execute display FSM: step: %d\n", status.cnt);
                    //Have the display drivere execute a step in its internal FSM. FSMs are partitioned in logical and physical for future expansion of the screen class.
                    bool f_ret = this -> Display::update_sprite();
                    //if: Display driver FSM is busy
                    if (f_ret == true)
                    {
                        //A FSM step was executed
                        status.cnt++;
                        //Driver still has work to do. Do not scan for more work.
                    }
                    //if: Display driver FSM is IDLE
                    else
                    {
                        DPRINT("Display FSM IDLE\n");
                        //reset status counter
                        status.cnt = 0;
                        //Driver FSM is done. I can scan for more work, if any is available.
                        status.phase = Fsm_state::SCAN_SPRITE;
                    }
                    //
                    f_continue = false;
                }	//End If: I'm in the process of sending a sprite
                //If: Algorithmic error
                else
                {
                    //Reset the update FSM
                    this -> init_fsm();
                    f_continue = false;
                }	//End If: Algorithmic error
            }	//End While: the Screen FSM is allowed to run
            //Write back FSM status
            this -> g_status = status;

            //----------------------------------------------------------------
            //	RETURN
            //----------------------------------------------------------------
            DRETURN_ARG("exe: %3d | w: %5d | h: %5d | cnt: %3d\n", status.phase, status.scan_w, status.scan_h, status.cnt );
            return false;	//OK
        }	//End public method: update | void

        /***************************************************************************/
        //!	@brief public method
        //!	clear | uint8_t
        /***************************************************************************/
        //!	@param palette_index | uint8_t | index of the solid sprite in the palette
        //! @return bool | false = OK | true = ERR
        //! @details
        //!	Clear the screen by setting a solid color sprite to each element of the sprite frame buffer
        //! @todo: a foreground sprite with the right foreground color or solid black and white with right palette also don't need updating
        /***************************************************************************/

        bool clear( uint8_t palette_index )
        {
            DENTER_ARG("index: %d\n", palette_index);
            //----------------------------------------------------------------
            //	CHECK
            //----------------------------------------------------------------
            
            //If: palette index outside the palette
            if (palette_index >= Screen::Config::PALETTE_SIZE)
            {
                DRETURN_ARG("ERR: palette index outside the palette\n");
                return true;	//FAIL
            }

            //----------------------------------------------------------------
            //	VARS
            //----------------------------------------------------------------

            //Fast counters
            uint8_t th, tw;
            //Temp sprite
            Frame_buffer_sprite sprite_tmp;

            //----------------------------------------------------------------
            //	BODY
            //----------------------------------------------------------------

                //Construct sprite.
            //Special sprite with full background color
            sprite_tmp.sprite_index		= Config::SPRITE_BACKGROUND;
            //Set color. I don't care about background color
            sprite_tmp.background_color	= palette_index;
            sprite_tmp.foreground_color	= palette_index;
            sprite_tmp.f_update			= true;
            //For: each frame buffer row (height scan)
            for (th = 0;th < Screen::Config::FRAME_BUFFER_HEIGHT;th++)
            {
                //For: each frame buffer col (width scan)
                for (tw = 0;tw < Screen::Config::FRAME_BUFFER_WIDTH;tw++)
                {
                    //If: sprite needs updating
                    if ((this -> g_frame_buffer[th][tw].sprite_index != sprite_tmp.sprite_index) || (this -> g_frame_buffer[th][tw].background_color != sprite_tmp.background_color))
                    {
                        //Update sprite
                        this -> g_frame_buffer[th][tw] = sprite_tmp;	
                    }
                } //End For: each frame buffer col (width scan)
            } //End For: each frame buffer row (height scan)


            //----------------------------------------------------------------
            //	RETURN
            //----------------------------------------------------------------
            DRETURN();
            return false;	//OK
        }	//End public method: clear | uint8_t

        /***************************************************************************/
        //!	@brief public method
        //!	print | int | int | char | Color | Color
        /***************************************************************************/
        //! @return bool | false = OK | true = ERR
        //! @details
        //! Print a character on screen using user defined background and foreground colors
        //! Mark the sprite for update if needs to be
        /***************************************************************************/

        bool print( int origin_h, int origin_w, char c, Color background, Color foreground )
        {
            DENTER_ARG("h: %5d, w: %5d, c: %c %d\n", origin_h, origin_w, c, c);
            //----------------------------------------------------------------
            //	CHECK
            //----------------------------------------------------------------

            //If: character is outside the ascii sprite table
            if ((origin_h < 0) || (origin_h >= Config::FRAME_BUFFER_HEIGHT) || (origin_w < 0) || (origin_w >= Config::FRAME_BUFFER_WIDTH))
            {
                DRETURN_ARG("ERR: out of the sprite table\n");
                return true;    //FAIL
            }
            //If: colors are bad
            if ((background >= (Color)Config::PALETTE_SIZE) || (foreground >= (Color)Config::PALETTE_SIZE))
            {
                DRETURN_ARG("ERR: bad default colors | Back: %3d | Fore: %3d |\n", background, foreground );
                return true;    //FAIL
            }
            //If: character is not stored inside the ascii sprite table
            if (is_valid_char( c ) == false)
            {
                DRETURN_ARG("ERR: character not on the char table\n");
                return true;    //FAIL
            }

            //----------------------------------------------------------------
            //	VARS
            //----------------------------------------------------------------

            //Temp sprite
            Frame_buffer_sprite sprite_tmp;
                //Construct temp sprite
            //Sprite index. use its ascii code
            sprite_tmp.sprite_index = c;
            //Use default background and foreground colors
            sprite_tmp.background_color = background;
            sprite_tmp.foreground_color = foreground;
            //Mark this sprite for update
            sprite_tmp.f_update = true;            

            //----------------------------------------------------------------
            //	BODY
            //----------------------------------------------------------------

            //If: character is already inside the sprite frame buffer
            if (this -> is_same_sprite( this -> g_frame_buffer[origin_h][origin_w], sprite_tmp) == true )
            {
                DRETURN_ARG("INFO: nothing to do\n");
                return false; //Nothing to do
            }
            //Update the sprite frame buffer
            this -> g_frame_buffer[origin_h][origin_w] = sprite_tmp;
            //@debug
            show_frame_sprite(this -> g_frame_buffer[origin_h][origin_w]);

            //----------------------------------------------------------------
            //	RETURN
            //----------------------------------------------------------------
            DRETURN();
            return false;	//OK
        }	//End public method: print | int | int | char | Color | Color

        /***************************************************************************/
        //!	@brief public method
        //!	print | int | int | char | Color
        /***************************************************************************/
        //! @return bool | false = OK | true = ERR
        //! @details
        //! Print a character on screen using default background color but a user defined foreground color
        /***************************************************************************/

        inline bool print( int origin_h, int origin_w, char c, Color foreground )
        {
            DENTER();
            //----------------------------------------------------------------
            //	BODY
            //----------------------------------------------------------------

            //Call the more generic print char method and return its result
            bool f_ret = this -> print( origin_h, origin_w, c, this -> g_default_background_color, foreground );

            //----------------------------------------------------------------
            //	RETURN
            //----------------------------------------------------------------
            DRETURN();
            return f_ret;
        }	//End public method: print | int | int | char | Color

        /***************************************************************************/
        //!	@brief public method
        //!	print | int | int | char |
        /***************************************************************************/
        //! @return bool | false = OK | true = ERR
        //! @details
        //!	print a char inside the sprite frame buffer and mark it for update if needs to be
        //! uses default background and foreground colors
        /***************************************************************************/

        inline bool print( int origin_h, int origin_w, char c )
        {
            DENTER();
            //----------------------------------------------------------------
            //	BODY
            //----------------------------------------------------------------

            //Call the more generic print char method and return its result
            bool f_ret = this -> print( origin_h, origin_w, c, this -> g_default_background_color, this -> g_default_foreground_color );

            //----------------------------------------------------------------
            //	RETURN
            //----------------------------------------------------------------
            DRETURN();
            return f_ret;
        }	//End public method: print | int | int | char |
        
        /***************************************************************************/
        //!	@brief public method
        //!	print | int | int | char | Color | Color
        /***************************************************************************/
        //! @return bool | false = OK | true = ERR
        //! @details
        //! Print a string on screen using user defined background and foreground colors
        //! Strings will not wrap around the screen
        //! String must be \0 terminated
        /***************************************************************************/

        bool print( int origin_h, int origin_w, const char *str, Color background, Color foreground )
        {
            DENTER_ARG("h: %5d, w: %5d, c: %p %s\n", origin_h, origin_w, str, str);
            //----------------------------------------------------------------
            //	CHECK
            //----------------------------------------------------------------
            
            //If: first character is outside the ascii sprite table
            if ((origin_h < 0) || (origin_h >= Config::FRAME_BUFFER_HEIGHT) || (origin_w < 0) || (origin_w >= Config::FRAME_BUFFER_WIDTH))
            {
                DRETURN_ARG("ERR: out of the sprite table %5d %5d\n", origin_h, origin_w);
                return true;	//FAIL
            }
            //If: colors are bad
            if ((background >= (Color)Config::PALETTE_SIZE) || (foreground >= (Color)Config::PALETTE_SIZE))
            {
                DRETURN_ARG("ERR: bad default colors | Back: %3d | Fore: %3d |\n", background, foreground );
                return true;    //FAIL
            }
            //If: string is invalid
            if (str == nullptr)
            {
                DRETURN_ARG("ERR: null pointer string\n");
                return true;	//FAIL
            }
            //Compute size of the string
            uint8_t str_len = strlen( str );
            //If: zero size string
            if (str_len <= 0)
            {
                DRETURN_ARG("zero size string\n");
                return false; //OK Nothing to do
            }

            //----------------------------------------------------------------
            //	VARS
            //----------------------------------------------------------------

            //Temp sprite
            Frame_buffer_sprite sprite_tmp;
            //Initialize colors
            sprite_tmp.f_update = true;
            sprite_tmp.background_color = background;
            sprite_tmp.foreground_color = foreground;
            
            //----------------------------------------------------------------
            //	BODY
            //----------------------------------------------------------------

            //Fast counters
            uint8_t t = 0;
            uint8_t tw = origin_w;
            //While: I'm allowed to print, I didn't exceed the string size, and I didn't exceed the screen position
            while ((t < str_len) && (tw < FRAME_BUFFER_WIDTH))
            {
                //Compute sprite ascii char
                sprite_tmp.sprite_index = str[t];
                DPRINT("t: %5d | tw: %5d | ", t, tw);
                //If this is not a printable ascii character
                if (this -> is_valid_char(sprite_tmp.sprite_index) == false)
                {
                    //!@todo Use placeholder sprite (special '?')
                    DPRINT_NOTAB("H: %5d | W: %5d | non printable\n", origin_h, tw );
                }
                //If: character is already inside the sprite frame buffer
                else if (this -> is_same_sprite( this -> g_frame_buffer[origin_h][origin_w], sprite_tmp) == true )
                {
                    //This character doesn't need updating
                    DPRINT_NOTAB("H: %5d | W: %5d | C: %c | skip\n", origin_h, tw, sprite_tmp.sprite_index );
                }
                //If: character needs updating
                else
                {
                    DPRINT_NOTAB("H: %5d | W: %5d | C: %c | to be updated\n", origin_h, tw, str[t] );
                    //Update the sprite frame buffer
                    this -> g_frame_buffer[origin_h][tw] = sprite_tmp;
                    show_frame_sprite(this -> g_frame_buffer[origin_h][tw]);    //@debug
                }
                //Next character
                t++;
                tw++;
            }	//End While: I'm allowed to print, I didn't exceed the string size, and I didn't exceed the screen position

            //----------------------------------------------------------------
            //	RETURN
            //----------------------------------------------------------------
            DRETURN();
            return false;	//OK
        }	//End public method: print | int | int | char | Color | Color

        /***************************************************************************/
        //!	@brief public method
        //!	print | int | int | const char* | Color |
        /***************************************************************************/
        //! @return bool | false = OK | true = ERR
        //! @details
        //!	print a char inside the sprite frame buffer and mark it for update if needs to be
        //! use default background
        //! wrapper for more general print string function
        /***************************************************************************/

        inline bool print( int origin_h, int origin_w, const char *str, Color foreground )
        {
            DENTER();   //Trace enter
            //----------------------------------------------------------------
            //	BODY
            //----------------------------------------------------------------

            //more general print string function
            bool f_ret = print( origin_h, origin_w, str, this -> g_default_background_color,  foreground );

            //----------------------------------------------------------------
            //	RETURN
            //----------------------------------------------------------------
            DRETURN();  //Trace enter
            return f_ret;
        }	//End public method: print | int | int | const char* | Color |

        /***************************************************************************/
        //!	@brief public method
        //!	print | int | int | const char* |
        /***************************************************************************/
        //! @return bool | false = OK | true = ERR
        //! @details
        //!	print a char inside the sprite frame buffer and mark it for update if needs to be
        //! uses default background and foreground colors
        //! wrapper for more general print string function
        /***************************************************************************/

        bool print( int origin_h, int origin_w, const char *str )
        {
            DENTER();   //Trace enter
            //----------------------------------------------------------------
            //	BODY
            //----------------------------------------------------------------

            //more general print string function
            bool f_ret = print( origin_h, origin_w, str, this -> g_default_background_color,  this -> g_default_foreground_color );

            //----------------------------------------------------------------
            //	RETURN
            //----------------------------------------------------------------
            DRETURN();  //Trace enter
            return f_ret;
        }	//End public method: print | int | int | char |

        /***************************************************************************/
        //!	@brief public method
        //!	paint |
        /***************************************************************************/
        //! @return bool | false = OK | true = ERR
        //! @details
        //!	dummy method to copy data
        /***************************************************************************/

        bool paint( int origin_h, int origin_w, Symbol symbol )
        {
            //----------------------------------------------------------------
            //	VARS
            //----------------------------------------------------------------

            //----------------------------------------------------------------
            //	BODY
            //----------------------------------------------------------------

            //----------------------------------------------------------------
            //	RETURN
            //----------------------------------------------------------------

            return false;	//OK
        }	//End public method: dummy

    //Visible to derived classes
    protected:
        /*********************************************************************************************************************************************************
        **********************************************************************************************************************************************************
        **	PROTECTED METHODS
        **********************************************************************************************************************************************************
        *********************************************************************************************************************************************************/

        /*********************************************************************************************************************************************************
        **********************************************************************************************************************************************************
        **	PROTECTED VARS
        **********************************************************************************************************************************************************
        *********************************************************************************************************************************************************/
    
    //Visible only inside the class
    private:
        /*********************************************************************************************************************************************************
        **********************************************************************************************************************************************************
        **	PRIVATE ENUM
        **********************************************************************************************************************************************************
        *********************************************************************************************************************************************************/

        typedef enum _Fsm_state
        {
            SCAN_SPRITE,	//Search for a sprite
            SEND_SPRITE		//
        } Fsm_state;

        /*********************************************************************************************************************************************************
        **********************************************************************************************************************************************************
        **	PRIVATE STRUCT
        **********************************************************************************************************************************************************
        *********************************************************************************************************************************************************/

        //Structure that describes a sprite in the sprite frame buffer
        typedef struct _Frame_buffer_sprite
        {
            //true: this sprite was changed since last scan | false: this sprite does not need updating
            uint8_t f_update            : 1;
            //sprite index inside the sprite table. A number are special sprite. Not all sprites are mapped
            uint8_t sprite_index        : Screen::Config::SPRITE_SIZE_BIT;
            //foreground palette color index
            uint8_t foreground_color    : Screen::Config::PALETTE_SIZE_BIT;
            //background palette color index
            uint8_t background_color    : Screen::Config::PALETTE_SIZE_BIT;
        } Frame_buffer_sprite;

        //Status of the update FSM
        typedef struct _Fsm_status
        {
            //width and height scan indexes
            uint16_t scan_w, scan_h;
            //Index withing the sprite sending sequence. Number of steps is defined inside the driver
            uint8_t cnt;
            //Status of the scan
            Fsm_state phase;
        } Fsm_status;

         /*********************************************************************************************************************************************************
        **********************************************************************************************************************************************************
        **	PRIVATE INIT
        **********************************************************************************************************************************************************
        *********************************************************************************************************************************************************/

        /***************************************************************************/
        //!	@brief private init
        //!	init_class_vars | void |
        /***************************************************************************/
        //! @return bool | false = OK | true = ERR
        //! @details
        //!	set up the defaults
        /***************************************************************************/

        bool init_class_vars( void )
        {
            //----------------------------------------------------------------
            //	VARS
            //----------------------------------------------------------------

            //----------------------------------------------------------------
            //	BODY
            //----------------------------------------------------------------

            this -> g_default_background_color = Color::BLACK;
            this -> g_default_foreground_color = Color::WHITE;

            //----------------------------------------------------------------
            //	RETURN
            //----------------------------------------------------------------

            return false;	//OK
        }	//End private init: init_class_vars | void |

        /***************************************************************************/
        //!	@brief private init
        //!	init_frame_buffer | void
        /***************************************************************************/
        //! @return bool | false = OK | true = ERR
        //! @details
        //!	Initialize the sprite frame buffer
        /***************************************************************************/

        bool init_frame_buffer()
        {
            DENTER();
            //----------------------------------------------------------------
            //	VARS
            //----------------------------------------------------------------

            //Fast counter
            uint16_t tw, th;
            //Temp sprite
            Frame_buffer_sprite sprite_tmp;

            //----------------------------------------------------------------
            //	BODY
            //----------------------------------------------------------------

            //Force update of this sprite
            sprite_tmp.f_update = true;
            //Initialize sprite code to FULL BACKGROUND sprite
            sprite_tmp.sprite_index = Config::SPRITE_BLACK;
            //Initialize colors to defaults Color black and white
            sprite_tmp.background_color = Screen::Color::BLACK;
            sprite_tmp.foreground_color = Screen::Color::WHITE;
            show_frame_sprite( sprite_tmp );
            //For: each frame buffer row (height scan)
            for (th = 0;th < Screen::Config::FRAME_BUFFER_HEIGHT;th++)
            {
                //For: each frame buffer col (width scan)
                for (tw = 0;tw < Screen::Config::FRAME_BUFFER_WIDTH;tw++)
                {
                    //Save default sprite in the frame buffer
                    this -> g_frame_buffer[th][tw] = sprite_tmp;
                } //End For: each frame buffer col (width scan)
            } //End For: each frame buffer row (height scan)

            //----------------------------------------------------------------
            //	RETURN
            //----------------------------------------------------------------
            DRETURN();
            return false;	//OK
        }	//End private init: init_frame_buffer | void

        /***************************************************************************/
        //!	@brief private init
        //!	init_palette | void
        /***************************************************************************/
        //! @return bool | false = OK | true = ERR
        //! @details
        //!	initialize palette to default values
        /***************************************************************************/

        bool init_palette( void )
        {
            DENTER();
            //----------------------------------------------------------------
            //	BODY
            //----------------------------------------------------------------

            //Use the CGA default palette (https://en.wikipedia.org/wiki/Color_Graphics_Adapter)
            this -> g_palette[Screen::Color::BLACK]		= Longan_nano::Display::color( 0x00, 0x00, 0x00 );
            this -> g_palette[Screen::Color::BLUE]		= Longan_nano::Display::color( 0x00, 0x00, 0xAA );
            this -> g_palette[Screen::Color::GREEN] 	= Longan_nano::Display::color( 0x00, 0xAA, 0x00 );
            this -> g_palette[Screen::Color::CYAN]		= Longan_nano::Display::color( 0x00, 0xAA, 0xAA );
            this -> g_palette[Screen::Color::RED]		= Longan_nano::Display::color( 0xAA, 0x00, 0x00 );
            this -> g_palette[Screen::Color::MAGENTA]	= Longan_nano::Display::color( 0xAA, 0x00, 0xAA );
            this -> g_palette[Screen::Color::BROWN]		= Longan_nano::Display::color( 0xAA, 0x55, 0x00 );
            this -> g_palette[Screen::Color::LGRAY]		= Longan_nano::Display::color( 0xAA, 0xAA, 0xAA );
            this -> g_palette[Screen::Color::DGRAY]		= Longan_nano::Display::color( 0x55, 0x55, 0x55 );
            this -> g_palette[Screen::Color::LBLUE]		= Longan_nano::Display::color( 0x55, 0x55, 0xFF );
            this -> g_palette[Screen::Color::LGREEN]	= Longan_nano::Display::color( 0x55, 0xFF, 0x55 );
            this -> g_palette[Screen::Color::LCYAN]		= Longan_nano::Display::color( 0x55, 0xFF, 0xFF );
            this -> g_palette[Screen::Color::LMAGENTA]	= Longan_nano::Display::color( 0xFF, 0x55, 0xFF );
            this -> g_palette[Screen::Color::YELLOW]	= Longan_nano::Display::color( 0xFF, 0xFF, 0x55 );
            this -> g_palette[Screen::Color::WHITE]		= Longan_nano::Display::color( 0xFF, 0xFF, 0xFF );
            #ifdef DEBUG_ENABLE
            {
                DPRINT("TEST: %6x\n", Longan_nano::Display::color( 0xFF, 0xFF, 0xFF ));
                this -> show_palette();
            }
            #endif
            //----------------------------------------------------------------
            //	RETURN
            //----------------------------------------------------------------
            DRETURN();
            return false;	//OK
        }	//End private init: init_palette | void

        /***************************************************************************/
        //!	@brief private init
        //!	init_fsm | void |
        /***************************************************************************/
        //! @return bool | false = OK | true = ERR
        //! @details
        //!	Initialize the update FSM machine
        /***************************************************************************/

        bool init_fsm( void )
        {
            //----------------------------------------------------------------
            //	VARS
            //----------------------------------------------------------------

            //----------------------------------------------------------------
            //	BODY
            //----------------------------------------------------------------

            //Start from top left
            this -> g_status.scan_h = 0;
            this -> g_status.scan_w = 0;
            //IDLE state
            this -> g_status.cnt = 0;
            //Update should scan for the next sprite to be updated
            this -> g_status.phase = Fsm_state::SCAN_SPRITE;

            //----------------------------------------------------------------
            //	RETURN
            //----------------------------------------------------------------

            return false;	//OK
        }	//End private init: init_fsm | void |

         /*********************************************************************************************************************************************************
        **********************************************************************************************************************************************************
        **	PRIVATE TESTERS
        **********************************************************************************************************************************************************
        *********************************************************************************************************************************************************/
        
        /***************************************************************************/
        //!	@brief private tester
        //!	is_valid_char | char |
        /***************************************************************************/
        //! @return bool | false = INVALID | true =VALID
        //! @details
        //!	return true if the char is stored inside the ascii sprite table
        /***************************************************************************/

        inline bool is_valid_char( char c )
        {
            //----------------------------------------------------------------
            //	RETURN
            //----------------------------------------------------------------

            return ((c >= ' ') && (c <= '~'));
        }	//End private tester: is_valid_char | char |

        /***************************************************************************/
        //!	@brief private tester
        //!	is_same_sprite | Frame_buffer_sprite | Frame_buffer_sprite |
        /***************************************************************************/
        //! @return bool | false = different | true = same
        //! @details
        //! check if sprite_a is functionally the same as sprite_b
        //! this is used to decide if mark a sprite for update or leave it as is
        //! this function is more nuanced that it appears
        //! this function is inside the drivers and assumes all relevant checks have been made by interface functions
        //! E.G. >a foreground sprite with with foreground is the same as a background sprite with white background
        //! E.G. >writing 'a' red on black is differet from writing 'a' green on black
        //! A change of a color in the palette will automatically mark for update all characters that use that palette color, but its taken care by the relevant function
        //! Because of that, no function but the update FSM is allowed to deactivate the update flag, so don't be overly aggressive with the result of this function
        /***************************************************************************/

        bool is_same_sprite( Frame_buffer_sprite sprite_a, Frame_buffer_sprite sprite_b )
        {
            DENTER();
            //----------------------------------------------------------------
            //	VARS
            //----------------------------------------------------------------
            
            bool f_ascii_a = is_valid_char( sprite_a.sprite_index );
            bool f_special_a = (sprite_a.sprite_index < Config::NUM_SPECIAL_SPRITES);
            bool f_ascii_b = is_valid_char( sprite_b.sprite_index );
            bool f_special_b = (sprite_b.sprite_index < Config::NUM_SPECIAL_SPRITES);

            //----------------------------------------------------------------
            //	BODY
            //----------------------------------------------------------------

            //If: both characters are special
            if ((f_special_a == true) && (f_special_b == true))
            {
                //If: sprite indexes are the same
                if (sprite_a.sprite_index == sprite_b.sprite_index)
                {
                    //If: fixed color sprites (do not use palette)
                    if ((sprite_a.sprite_index == Config::SPRITE_BLACK) || (sprite_a.sprite_index == Config::SPRITE_WHITE))
                    {
                        DRETURN_ARG("Same fixed solid sprites: %d %d\n", sprite_a.sprite_index, sprite_b.sprite_index );
                        return true;    //Same
                    }
                    //If: character that use background only
                    else if (sprite_a.sprite_index == Config::SPRITE_BACKGROUND)
                    {
                        //If: background are the same
                        if (sprite_a.background_color == sprite_b.background_color)
                        {
                            DRETURN_ARG("Same solid background sprites: %d\n", sprite_a.background_color );
                            return true;    //Same
                        }
                        //If: background are the different
                        else
                        {
                            DRETURN_ARG("Different solid background sprites: %d %d\n", sprite_a.background_color, sprite_b.background_color );
                            return false;   //Different
                        }
                    }
                    //If: character that use foreground only
                    else if (sprite_a.sprite_index == Config::SPRITE_FOREGROUND)
                    {
                        //If: background are the same
                        if (sprite_a.foreground_color == sprite_b.foreground_color)
                        {
                            DRETURN_ARG("Same solid foreground sprites: %d\n", sprite_a.foreground_color );
                            return true;    //Same
                        }
                        //If: background are the different
                        else
                        {
                            DRETURN_ARG("Different solid foreground sprites: %d %d\n", sprite_a.foreground_color, sprite_b.foreground_color );
                            return false;   //Different
                        }
                    }
                    //Default:
                    else
                    {
                        //Any other combination is to be treated as different sprites 
                        DRETURN_ARG("Different sprites: %d %d\n", sprite_a.sprite_index, sprite_b.sprite_index );
                        return false; //Different
                    }
                }   //End If: sprite indexes are the same
                //End If: sprite indexes are the different
                else
                {
                    //@todo: there are combinations of different indexes that yield the same color
                    DRETURN_ARG("Different sprites: %d %d\n", sprite_a.sprite_index, sprite_b.sprite_index );
                    return false; //Different
                }
                
            }   //End If: both characters are special
            //If: both are validascii characters
            else if ((f_ascii_a == true) && (f_ascii_b == true))
            {
                //If: sprites have the same index, the same background and the same foreground
                if ((sprite_a.sprite_index == sprite_b.sprite_index) && (sprite_a.background_color == sprite_b.background_color) && (sprite_a.foreground_color == sprite_b.foreground_color))
                {
                    DRETURN_ARG("Same ascii characters: %c %c\n", sprite_a.sprite_index, sprite_b.sprite_index );
                    return true;    //Same ascii
                }
                //If: any property is different
                else
                {
                    DRETURN_ARG("Different ascii characters: %c %c\n", sprite_a.sprite_index, sprite_b.sprite_index );
                    return false; //Different ascii
                }
            }
            //@todo: graphics sprites (Symbols)
            //If: every other combination is an invalid sprite
            else
            {
                //Do nothing
            }
            
            //----------------------------------------------------------------
            //	RETURN
            //----------------------------------------------------------------
            DRETURN();
            return false;	//Sprites are different
        }	//End private tester: is_same_sprite | Frame_buffer_sprite | Frame_buffer_sprite |

         /*********************************************************************************************************************************************************
        **********************************************************************************************************************************************************
        **	PRIVATE METHODS
        **********************************************************************************************************************************************************
        *********************************************************************************************************************************************************/

        /***************************************************************************/
        //!	@brief private method
        //!	build_pixel_data | uint16_t | uint16_t |
        /***************************************************************************/
        //! @return bool | false = OK | true = ERR
        //! @details
        //!	Combine the sprite data to fill the pixel data to be feed to the display driver
        /***************************************************************************/

        bool build_pixel_data( uint16_t index_h, uint16_t index_w )
        {
            DENTER_ARG("index_w : %5d | index_h %5d\n", index_w, index_h);
            //----------------------------------------------------------------
            //	VARS
            //----------------------------------------------------------------


            //
            uint8_t sprite_index;

            uint16_t foreground_color, background_color;
            //Temp color
            uint16_t color;

            bool f_solid_color;

            //----------------------------------------------------------------
            //	INIT
            //----------------------------------------------------------------

            //If: bad parameters
            if ((PEDANTIC_CHECKS == true) && ((index_w >= Config::FRAME_BUFFER_WIDTH) || (index_h >= Config::FRAME_BUFFER_HEIGHT)) )
            {
                DRETURN_ARG("ERR: bad parameters\n");
                return true; //FAIL
            }

            //----------------------------------------------------------------
            //	DECODE SPRITE
            //----------------------------------------------------------------

            //Fetch a frame sprite
            Frame_buffer_sprite frame_sprite_tmp = g_frame_buffer[index_h][index_w];
            show_frame_sprite( frame_sprite_tmp );
            //Decode background and foreground colors
            background_color = g_palette[ frame_sprite_tmp.background_color ];
            foreground_color = g_palette[ frame_sprite_tmp.foreground_color ];
            //Get sprite index from sprite frame buffer
            sprite_index = g_frame_buffer[index_h][index_w].sprite_index;
            DPRINT("sprite: %d | background color: %6x | foreground_color: %6x |\n", sprite_index, background_color, foreground_color );
            //Pointer to sprite data
            const uint8_t *sprite_ptr = nullptr;
                //Decode the sprite index into a sprite monochrome map
            //If: Transparent sprite
            if (sprite_index == Config::SPRITE_TRANSPARENT)
            {
                //Algorithmic error. i should have not gotten to the point of decoding a transparent sprite.
                DRETURN_ARG("ERR: algorithm\n");
                return true;
            }
            //If: Solid black
            else if (sprite_index == Config::SPRITE_BLACK)
            {
                f_solid_color = true;
                color = Display::color(0x00,0x00,0x00);
            }
            //If: Solid white
            else if (sprite_index == Config::SPRITE_WHITE)
            {
                f_solid_color = true;
                color = Display::color(0xFF,0xFF,0xFF);
            }
            //If: Solid background
            else if (sprite_index == Config::SPRITE_BACKGROUND)
            {
                f_solid_color = true;
                color = background_color;
            }
            //If: Solid foreground
            else if (sprite_index == Config::SPRITE_FOREGROUND)
            {
                f_solid_color = true;
                color = foreground_color;
            }
            //If: Handled Ascii Character in the character table
            else if (this -> is_valid_char( sprite_index ))
            {
                //Full pixel color map
                f_solid_color = false;
                //Point to the first byte of the ascii sprite
                sprite_ptr = &g_ascii_sprites_1608[ (sprite_index -' ') *Config::SPRITE_HEIGHT ];
            }
            //If: Unhandled sprite code
            else
            {
                //@todo: all unhandled sprites should be mapped to a default placeholder sprite
                DRETURN_ARG("ERR: Unhandled sprite\n");
                return true;
            }

            //----------------------------------------------------------------
            //	BUILD PIXEL MAP
            //----------------------------------------------------------------

            //If: sprite is a complex color map
            if (f_solid_color == false)
            {
                //Fast counter
                int tw, th;
                //Store a fullbinary width slice (row)
                uint32_t sprite_width_slice;
                DPRINT("sprite table index: %c %5d | width slice | ", sprite_index, sprite_index-' ' );
                //For: Scan height
                for (th = 0;th < Config::SPRITE_HEIGHT;th++)
                {
                    //Grab full width slice of data
                    sprite_width_slice = sprite_ptr[ th ];
                    DPRINT_NOTAB(" %x |",sprite_width_slice);
                    //For: Scan width
                    for (tw = 0;tw < Config::SPRITE_WIDTH;tw++)
                    {
                        //Compute color from the binary sprite map | false = background | true = foreground
                        color = ((sprite_width_slice & 0x01) == 0x00)?(background_color):(foreground_color);
                        //Shift away the decoded bit
                        sprite_width_slice = sprite_width_slice >> 1;
                        //Save pixel
                        this -> g_pixel_data[((th *Config::SPRITE_WIDTH) +tw)] = color;
                    }	//End For: Scan width
                }	//End For: Scan height
                DPRINT_NOTAB("\n");
            }
            //If: sprite is a solid color
            else
            {
                //Fast counter
                int t;
                //For: Each Pixel
                for (t = 0;t < Config::SPRITE_PIXEL_COUNT;t++)
                {
                    //Save pixel
                    this -> g_pixel_data[ t ] = color;
                }	//End For: Each Pixel
            }	//End If: sprite is a solid color

            //DEBUG
            show_pixels();

            //----------------------------------------------------------------
            //	RETURN
            //----------------------------------------------------------------
            DRETURN();
            return false;
        }	//End private method: build_pixel_data | uint16_t | uint16_t |

        /***************************************************************************/
        //!	@brief public method
        //!	dummy |
        /***************************************************************************/
        //! @return bool | false = OK | true = ERR
        //! @details
        //!	dummy method to copy data
        /***************************************************************************/

        bool dummy( void )
        {
            //----------------------------------------------------------------
            //	VARS
            //----------------------------------------------------------------

            //----------------------------------------------------------------
            //	BODY
            //----------------------------------------------------------------

            //----------------------------------------------------------------
            //	RETURN
            //----------------------------------------------------------------

            return false;	//OK
        }	//End public method: dummy

        /*********************************************************************************************************************************************************
        **********************************************************************************************************************************************************
        **	PRIVATE DEBUGGERS
        **********************************************************************************************************************************************************
        *********************************************************************************************************************************************************/

        #ifdef ENABLE_DEBUG

        void show_frame_sprite( Frame_buffer_sprite sprite_tmp )
        {
            DPRINT("update: %d | index: %5d | background: %5d | foreground: %5d |\n", sprite_tmp.f_update, sprite_tmp.sprite_index, sprite_tmp.background_color, sprite_tmp.foreground_color );
            DPRINT("decoded background color: %6x | decoded foreground color: %6x |\n", g_palette[sprite_tmp.background_color], sprite_tmp.foreground_color );
            return;
        }

        void show_pixels( void )
        {
            DENTER_ARG("oh: %5d, ow: %5d, sh: %5d, sw: %5d\n", g_sprite.origin_h, g_sprite.origin_w, g_sprite.size_h, g_sprite.size_w);

            for (int th = 0;th < g_sprite.size_h;th++)
            {
                DPRINT("");
                for (int tw = 0;tw < g_sprite.size_w;tw++)
                {
                    DPRINT_NOTAB("%6x | ", g_pixel_data[th *g_sprite.size_w +tw]);
                }
                DPRINT_NOTAB("\n");
            }

            DRETURN();
            return;
        }

        void show_palette( void )
        {
            DENTER();
            for (int t = 0;t < Config::PALETTE_SIZE;t++)
            {
                DPRINT("%d -> %6x\n", t, g_palette[t] );

            }

            DRETURN();
            return;
        }

        #endif

         /*********************************************************************************************************************************************************
        **********************************************************************************************************************************************************
        **	PRIVATE VARS
        **********************************************************************************************************************************************************
        *********************************************************************************************************************************************************/

        //Default background and foreground colors to be used
        Color g_default_background_color;
        Color g_default_foreground_color;
        //Color Palette. One special code for transparent. Two special indexes store global background and foreground
        uint16_t g_palette[ Config::PALETTE_SIZE ];
        //Frame Buffer
        Frame_buffer_sprite g_frame_buffer[ Config::FRAME_BUFFER_HEIGHT ][ Config::FRAME_BUFFER_WIDTH ];
        //Sprite buffer that stores raw pixel data for a single sprite
        uint16_t g_pixel_data[ Config::SPRITE_PIXEL_COUNT ];
        //Status of the update FSM
        Fsm_status g_status;
        //H16W8 ASCII Sprites. Stored in the flash memory. 95 sprites from space ' ' code 32 to tilda '~' code 126
        static constexpr uint8_t g_ascii_sprites_1608[ 1520 ] =
        {
            0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,	// ' '
            0x00,0x00,0x00,0x08,0x08,0x08,0x08,0x08,0x08,0x08,0x00,0x00,0x18,0x18,0x00,0x00,	// '!'
            0x00,0x48,0x6C,0x24,0x12,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
            0x00,0x00,0x00,0x24,0x24,0x24,0x7F,0x12,0x12,0x12,0x7F,0x12,0x12,0x12,0x00,0x00,
            0x00,0x00,0x08,0x1C,0x2A,0x2A,0x0A,0x0C,0x18,0x28,0x28,0x2A,0x2A,0x1C,0x08,0x08,
            0x00,0x00,0x00,0x22,0x25,0x15,0x15,0x15,0x2A,0x58,0x54,0x54,0x54,0x22,0x00,0x00,
            0x00,0x00,0x00,0x0C,0x12,0x12,0x12,0x0A,0x76,0x25,0x29,0x11,0x91,0x6E,0x00,0x00,
            0x00,0x06,0x06,0x04,0x03,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
            0x00,0x40,0x20,0x10,0x10,0x08,0x08,0x08,0x08,0x08,0x08,0x10,0x10,0x20,0x40,0x00,
            0x00,0x02,0x04,0x08,0x08,0x10,0x10,0x10,0x10,0x10,0x10,0x08,0x08,0x04,0x02,0x00,
            0x00,0x00,0x00,0x00,0x08,0x08,0x6B,0x1C,0x1C,0x6B,0x08,0x08,0x00,0x00,0x00,0x00,
            0x00,0x00,0x00,0x00,0x08,0x08,0x08,0x08,0x7F,0x08,0x08,0x08,0x08,0x00,0x00,0x00,
            0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x06,0x06,0x04,0x03,
            0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFE,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
            0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x06,0x06,0x00,0x00,
            0x00,0x00,0x80,0x40,0x40,0x20,0x20,0x10,0x10,0x08,0x08,0x04,0x04,0x02,0x02,0x00,
            0x00,0x00,0x00,0x18,0x24,0x42,0x42,0x42,0x42,0x42,0x42,0x42,0x24,0x18,0x00,0x00,
            0x00,0x00,0x00,0x08,0x0E,0x08,0x08,0x08,0x08,0x08,0x08,0x08,0x08,0x3E,0x00,0x00,
            0x00,0x00,0x00,0x3C,0x42,0x42,0x42,0x20,0x20,0x10,0x08,0x04,0x42,0x7E,0x00,0x00,
            0x00,0x00,0x00,0x3C,0x42,0x42,0x20,0x18,0x20,0x40,0x40,0x42,0x22,0x1C,0x00,0x00,
            0x00,0x00,0x00,0x20,0x30,0x28,0x24,0x24,0x22,0x22,0x7E,0x20,0x20,0x78,0x00,0x00,
            0x00,0x00,0x00,0x7E,0x02,0x02,0x02,0x1A,0x26,0x40,0x40,0x42,0x22,0x1C,0x00,0x00,
            0x00,0x00,0x00,0x38,0x24,0x02,0x02,0x1A,0x26,0x42,0x42,0x42,0x24,0x18,0x00,0x00,
            0x00,0x00,0x00,0x7E,0x22,0x22,0x10,0x10,0x08,0x08,0x08,0x08,0x08,0x08,0x00,0x00,
            0x00,0x00,0x00,0x3C,0x42,0x42,0x42,0x24,0x18,0x24,0x42,0x42,0x42,0x3C,0x00,0x00,
            0x00,0x00,0x00,0x18,0x24,0x42,0x42,0x42,0x64,0x58,0x40,0x40,0x24,0x1C,0x00,0x00,
            0x00,0x00,0x00,0x00,0x00,0x00,0x18,0x18,0x00,0x00,0x00,0x00,0x18,0x18,0x00,0x00,
            0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x08,0x00,0x00,0x00,0x00,0x00,0x08,0x08,0x04,
            0x00,0x00,0x00,0x40,0x20,0x10,0x08,0x04,0x02,0x04,0x08,0x10,0x20,0x40,0x00,0x00,
            0x00,0x00,0x00,0x00,0x00,0x00,0x7F,0x00,0x00,0x00,0x7F,0x00,0x00,0x00,0x00,0x00,
            0x00,0x00,0x00,0x02,0x04,0x08,0x10,0x20,0x40,0x20,0x10,0x08,0x04,0x02,0x00,0x00,
            0x00,0x00,0x00,0x3C,0x42,0x42,0x46,0x40,0x20,0x10,0x10,0x00,0x18,0x18,0x00,0x00,
            0x00,0x00,0x00,0x1C,0x22,0x5A,0x55,0x55,0x55,0x55,0x2D,0x42,0x22,0x1C,0x00,0x00,
            0x00,0x00,0x00,0x08,0x08,0x18,0x14,0x14,0x24,0x3C,0x22,0x42,0x42,0xE7,0x00,0x00,
            0x00,0x00,0x00,0x1F,0x22,0x22,0x22,0x1E,0x22,0x42,0x42,0x42,0x22,0x1F,0x00,0x00,
            0x00,0x00,0x00,0x7C,0x42,0x42,0x01,0x01,0x01,0x01,0x01,0x42,0x22,0x1C,0x00,0x00,
            0x00,0x00,0x00,0x1F,0x22,0x42,0x42,0x42,0x42,0x42,0x42,0x42,0x22,0x1F,0x00,0x00,
            0x00,0x00,0x00,0x3F,0x42,0x12,0x12,0x1E,0x12,0x12,0x02,0x42,0x42,0x3F,0x00,0x00,
            0x00,0x00,0x00,0x3F,0x42,0x12,0x12,0x1E,0x12,0x12,0x02,0x02,0x02,0x07,0x00,0x00,
            0x00,0x00,0x00,0x3C,0x22,0x22,0x01,0x01,0x01,0x71,0x21,0x22,0x22,0x1C,0x00,0x00,
            0x00,0x00,0x00,0xE7,0x42,0x42,0x42,0x42,0x7E,0x42,0x42,0x42,0x42,0xE7,0x00,0x00,
            0x00,0x00,0x00,0x3E,0x08,0x08,0x08,0x08,0x08,0x08,0x08,0x08,0x08,0x3E,0x00,0x00,
            0x00,0x00,0x00,0x7C,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x11,0x0F,
            0x00,0x00,0x00,0x77,0x22,0x12,0x0A,0x0E,0x0A,0x12,0x12,0x22,0x22,0x77,0x00,0x00,
            0x00,0x00,0x00,0x07,0x02,0x02,0x02,0x02,0x02,0x02,0x02,0x02,0x42,0x7F,0x00,0x00,
            0x00,0x00,0x00,0x77,0x36,0x36,0x36,0x36,0x2A,0x2A,0x2A,0x2A,0x2A,0x6B,0x00,0x00,
            0x00,0x00,0x00,0xE3,0x46,0x46,0x4A,0x4A,0x52,0x52,0x52,0x62,0x62,0x47,0x00,0x00,
            0x00,0x00,0x00,0x1C,0x22,0x41,0x41,0x41,0x41,0x41,0x41,0x41,0x22,0x1C,0x00,0x00,
            0x00,0x00,0x00,0x3F,0x42,0x42,0x42,0x42,0x3E,0x02,0x02,0x02,0x02,0x07,0x00,0x00,
            0x00,0x00,0x00,0x1C,0x22,0x41,0x41,0x41,0x41,0x41,0x4D,0x53,0x32,0x1C,0x60,0x00,
            0x00,0x00,0x00,0x3F,0x42,0x42,0x42,0x3E,0x12,0x12,0x22,0x22,0x42,0xC7,0x00,0x00,
            0x00,0x00,0x00,0x7C,0x42,0x42,0x02,0x04,0x18,0x20,0x40,0x42,0x42,0x3E,0x00,0x00,
            0x00,0x00,0x00,0x7F,0x49,0x08,0x08,0x08,0x08,0x08,0x08,0x08,0x08,0x1C,0x00,0x00,
            0x00,0x00,0x00,0xE7,0x42,0x42,0x42,0x42,0x42,0x42,0x42,0x42,0x42,0x3C,0x00,0x00,
            0x00,0x00,0x00,0xE7,0x42,0x42,0x22,0x24,0x24,0x14,0x14,0x18,0x08,0x08,0x00,0x00,
            0x00,0x00,0x00,0x6B,0x49,0x49,0x49,0x49,0x55,0x55,0x36,0x22,0x22,0x22,0x00,0x00,
            0x00,0x00,0x00,0xE7,0x42,0x24,0x24,0x18,0x18,0x18,0x24,0x24,0x42,0xE7,0x00,0x00,
            0x00,0x00,0x00,0x77,0x22,0x22,0x14,0x14,0x08,0x08,0x08,0x08,0x08,0x1C,0x00,0x00,
            0x00,0x00,0x00,0x7E,0x21,0x20,0x10,0x10,0x08,0x04,0x04,0x42,0x42,0x3F,0x00,0x00,
            0x00,0x78,0x08,0x08,0x08,0x08,0x08,0x08,0x08,0x08,0x08,0x08,0x08,0x08,0x78,0x00,
            0x00,0x00,0x02,0x02,0x04,0x04,0x08,0x08,0x08,0x10,0x10,0x20,0x20,0x20,0x40,0x40,
            0x00,0x1E,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x1E,0x00,
            0x00,0x38,0x44,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
            0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,
            0x00,0x06,0x08,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
            0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x3C,0x42,0x78,0x44,0x42,0x42,0xFC,0x00,0x00,
            0x00,0x00,0x00,0x03,0x02,0x02,0x02,0x1A,0x26,0x42,0x42,0x42,0x26,0x1A,0x00,0x00,
            0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x38,0x44,0x02,0x02,0x02,0x44,0x38,0x00,0x00,
            0x00,0x00,0x00,0x60,0x40,0x40,0x40,0x78,0x44,0x42,0x42,0x42,0x64,0xD8,0x00,0x00,
            0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x3C,0x42,0x7E,0x02,0x02,0x42,0x3C,0x00,0x00,
            0x00,0x00,0x00,0xF0,0x88,0x08,0x08,0x7E,0x08,0x08,0x08,0x08,0x08,0x3E,0x00,0x00,
            0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x7C,0x22,0x22,0x1C,0x02,0x3C,0x42,0x42,0x3C,
            0x00,0x00,0x00,0x03,0x02,0x02,0x02,0x3A,0x46,0x42,0x42,0x42,0x42,0xE7,0x00,0x00,
            0x00,0x00,0x00,0x0C,0x0C,0x00,0x00,0x0E,0x08,0x08,0x08,0x08,0x08,0x3E,0x00,0x00,
            0x00,0x00,0x00,0x30,0x30,0x00,0x00,0x38,0x20,0x20,0x20,0x20,0x20,0x20,0x22,0x1E,
            0x00,0x00,0x00,0x03,0x02,0x02,0x02,0x72,0x12,0x0A,0x16,0x12,0x22,0x77,0x00,0x00,
            0x00,0x00,0x00,0x0E,0x08,0x08,0x08,0x08,0x08,0x08,0x08,0x08,0x08,0x3E,0x00,0x00,
            0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x7F,0x92,0x92,0x92,0x92,0x92,0xB7,0x00,0x00,
            0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x3B,0x46,0x42,0x42,0x42,0x42,0xE7,0x00,0x00,
            0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x3C,0x42,0x42,0x42,0x42,0x42,0x3C,0x00,0x00,
            0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x1B,0x26,0x42,0x42,0x42,0x22,0x1E,0x02,0x07,
            0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x78,0x44,0x42,0x42,0x42,0x44,0x78,0x40,0xE0,
            0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x77,0x4C,0x04,0x04,0x04,0x04,0x1F,0x00,0x00,
            0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x7C,0x42,0x02,0x3C,0x40,0x42,0x3E,0x00,0x00,
            0x00,0x00,0x00,0x00,0x00,0x08,0x08,0x3E,0x08,0x08,0x08,0x08,0x08,0x30,0x00,0x00,
            0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x63,0x42,0x42,0x42,0x42,0x62,0xDC,0x00,0x00,
            0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xE7,0x42,0x24,0x24,0x14,0x08,0x08,0x00,0x00,
            0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xEB,0x49,0x49,0x55,0x55,0x22,0x22,0x00,0x00,
            0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x76,0x24,0x18,0x18,0x18,0x24,0x6E,0x00,0x00,
            0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xE7,0x42,0x24,0x24,0x14,0x18,0x08,0x08,0x07,
            0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x7E,0x22,0x10,0x08,0x08,0x44,0x7E,0x00,0x00,
            0x00,0xC0,0x20,0x20,0x20,0x20,0x20,0x10,0x20,0x20,0x20,0x20,0x20,0x20,0xC0,0x00,
            0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10,	// '|'
            0x00,0x06,0x08,0x08,0x08,0x08,0x08,0x10,0x08,0x08,0x08,0x08,0x08,0x08,0x06,0x00,	// '}'
            0x0C,0x32,0xC2,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,	// '~'
        };
};	//End Class: Screen

/**********************************************************************************
**	NAMESPACE
**********************************************************************************/

} //End Namespace

#else
    #warning "Multiple inclusion of hader file"
#endif
