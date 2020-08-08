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

//Display driver. Physical interface and microcontroller peripherals.
#include "ST7735S_W160_H80_C16.hpp"
//Number -> string
#include "embedded_string.hpp"

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

//! @namespace Longan_nano specialized for the eval board Longan_nano with GD32VF103 RiscV MCU
namespace Longan_nano
{

/**********************************************************************************
**	TYPEDEFS
**********************************************************************************/

//Font size 10 has 8x20 sprites on screen
//Font size 16 has 5x20 sprites on screen
#define FONT_HEIGHT 10

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
**  Use the following build flags in platformio.ini
**	build_flags = -fno-exceptions
**********************************************************************************/

/*********************************************************************************/
//! @class 		Screen
/*********************************************************************************/
//!	@author		Orso Eric
//! @version	2020-08-08
//! @date		2020-07-09
//! @brief		Longan Nano Screen Class
//! @copyright	BSD 3-Clause License Copyright (c) 2020, Orso Eric
//! @details
//! \n Screen class: abstraction layer for low level LCD HAL for the ST7735S Longan Nano Display
//!	\n Screen provides a frame buffer and functions to write on it
//!	\n LCD class: low level driver for screen hardware. Inherited by screen
//!	\n 	2020-07-09
//!	\n Formulated architeture
//!	\n Focus on Display dirver first
//!	\n		2020-07-23
//!	\n	Display driver working
//!	\n	Core methods: register_sprite and update
//!	\n	Formulated palette, frame sprite and sprite pixel buffer architecture
//!	\n	Use 16x08 sprites. They already exists and 8 by 8 are too small
//!	\n	Structure of the Screen Update FSM Up
//!	\n	FSM is partitioned into the Display side FSM that handles the physical display and the Screen side FSM that handles the sprite based frame buffer
//!	\n	Screen class is designed to massively reduce memory usage and data sent to physical display. Most of the time I need fast updating characters and nothing more.
//!	\n		2020-07-24
//!	\n	Added through debug
//!	\n	BUG: Color decoding macro: FIXED
//!	\n Test clear black and white inherited from display: SUCCESS
//!	\n	clear function tested with background calls from update and periodic calls of clear triggered by the RTC interrupt
//!	\n	Test clear screen cycling colors from the palette: SUCCESS
//!	\n		2020-07-25
//!	\n Test random characters in random position: SUCCESS
//!	\n "is_same" check if two sprites are the same looking for all the nuances
//!	\n "paint" method, meant to draw special graphics sprites
//!	\n "Symbol" enum, meant to provide a few graphical sprites. Few in numbers and meant for light graphics, like progress bars.
//!	\n Test draw a random sprite with a random color:
//!	\n     2020-07-26
//!	\n Test print string functions: SUCCESS
//!	\n Change architecture: do not use RTC ISR but use chrono class for timings. Save a peripheral and achieve a mini hardwired scheduler. PA8 release button switches between demos: SUCCESS
//!	\n     2020-07-27
//!	\n print number with format
//!	\n User::String class handles number to string conversion up to 32b signed and unsigned
//!	\n     2020-07-28
//!	\n tested print numbers with format: SUCCESS
//!	\n bugfix: check that the number fits than the allowed size. Caused dot artifacts on screen
//! \n    2020-07-29
//!	\n "change_color" allow to change the color of all sprites from one to another. E.G. red background to blue background
//!	\n "set_default_colors" allow to set the default background and foreground colors to be used. Change all the sprites that use those colors as well
//!	\n "set_palette_color" change a color in the palette. Mark all the sprites that use those colors for update
//!	\n     2020-07-30
//!	\n color methods
//!	\n     2020-07-31
//!	\n change color demo
//!	\n with this release, I have a working screen library that does all I need it to do with 2.804% CPU usage at 100uS slice time
//!	\n I will add to this library as i work on the end applications and find bugs and shortcomings
//!	\n BUGFIX: print string had a botched same sprite detection 0.18% absolute performance improvement
//!	\n Now print number won't refresh sprites that have not changed
//!	\n Migrated print functions to return the number of updated sprites. -1 if error
//!	\n     2020-08-03
//!	\n g_pending_cnt was meant to skip scan if zero sprites need updating in the frame buffer
//!	\n it just increases CPU use overall, so was removed
//!	\n	Paint square method. Removed Symbols.
//!	\n		2020-08-04
//!	\n Unpack methods from class to make the header interface clear while keeping the implementation in the .hpp
//!	\n Complete doxygen documentation for all methods
//! \n		2020-08-06
//! \n Add pending_cnt. When counter is zero, the update method won't waste time scanning and print method will start seeking from the first valid sprite saving CPU
//! \n Adding error codes and print_err
//! \n Add rule. allow conditional formatting of number based on rules
//! \n Generated two fonts. 16 pixel height NSimSun and 10 pixel height Courier Now
//! \n      2020-08-07
//! \n  Bugfix: Driver wasn't taking advantage of the Display solid color register method to avoid computing a complex color map
//! \n  and was always computing a full color map. Now solid sprites just send one color.
//! \n      2020-08-08
//! \n  Bugfix: The draw string with colors and set color methods sometimes didn't update the sprites correctly
//! \n  either duplicating the previous char or leaving an artifact on pixel 0,0 of the sprite
//! \n  it was caused by the update FSM disagreeing on which sprites were solid colors or color maps
//! \n  now a register_sprite method combine the processing of color data and registering of sprites
//! \n  this reduces workload and solves the bug. Now Screen::register_sprite and Display::register_sprite nicely handle the hierarchy
//! \n  just like Screen::update and Display::update
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

        //! @brief Configuration parameters for the logical screen
        typedef enum _Config
        {
            PEDANTIC_CHECKS			= true,			//Pedantic check meant for debug
            //Screen Logical Configuration. The screen is divided in sprites, shrinking the frame buffer
            SPRITE_WIDTH			= 8,			//Width of a sprite
            SPRITE_HEIGHT			= FONT_HEIGHT,  //Height of a sprite
            SPRITE_PIXEL_COUNT		= SPRITE_HEIGHT *SPRITE_WIDTH,	//Number of pixels in a sprite
            SPRITE_SCAN_LIMIT		= 5,			//Scan at most # idle sprites before releasing control from update() anyway
            //Special sprite codes
            NUM_SPECIAL_SPRITES		= 5,			//Number of special sprites
            SPRITE_TRANSPARENT		= 0,			//Transparent sprite. Never updated. Ignore update flag.
            SPRITE_BLACK			= 1,			//Always mapped to a full black sprite, invariant to palette index and content
            SPRITE_WHITE			= 2,			//Always mapped to a full white sprite, invariant to palette index and content
            SPRITE_BACKGROUND		= 3,			//Sprite with the background color
            SPRITE_FOREGROUND		= 4,			//Sprite with the foreground color
            //ASCII Sprite Table Definitions
            ASCII_START				= ' ',			//First ASCII character defined in the sprite table
            ASCII_STOP				= '~',			//Last ASCII character defined in the sprite table
            //Colors are discretized in a palette
            PALETTE_SIZE			= 16,           //Size of the palette
            PALETTE_SIZE_BIT		= 4,			//Number of bit required to describe a color in the palette
            //Size of the frame buffer. Display phisical size comes from the Physical Display class
            FRAME_BUFFER_WIDTH		= Longan_nano::Display::Config::WIDTH /SPRITE_WIDTH,
            FRAME_BUFFER_HEIGHT		= Longan_nano::Display::Config::HEIGHT /SPRITE_HEIGHT,
            FRAME_BUFFER_SIZE		= FRAME_BUFFER_WIDTH *FRAME_BUFFER_HEIGHT,
            SPRITE_SIZE				= 128,			//Number of sprites in the sprite table
            SPRITE_SIZE_BIT			= 7				//Size of the sprite table
        } Config;

        //! @brief Use the default Color palette. Short hand indexes for user. User can change the palette at will
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
            LRED,
            LMAGENTA,
            YELLOW,
            WHITE,
        } Color;

        //! @brief Possible number configurations
        typedef enum _Format_format
        {
            NUM,        //Extended number
            ENG,        //Engineering format with four significant digits and SI suffix
        } Format_format;

        //! @brief Left or Right alignment for a number
        typedef enum _Format_align
        {
            ADJ_LEFT,
            ADJ_RIGHT,
        } Format_align;

        //! @brief Enumerate possible error of the Screen class
        typedef enum _Error
        {
            //Screen class working as intended
            OK,
            //Pending counter either underflow or overflow. Means an algorithmic error occurred in the screen class
            PENDING_OVERFLOW,
            PENDING_UNDERFLOW,
            REGISTER_SPRITE_FAIL,
            //Handle error of error. errorception!
            BAD_ERROR_CODE,
            NUM_ERROR_CODES,
        } Error;
        /*
        //Possible rules for conditional formatting of numbers
        typedef enum _Rule_condition
        {
            NEVER,
            ALWAYS,
            EQUAL,
            GREATER,
            GREATER_EQUAL,
            SMALLER,
            SMALLER_EQUAL,
        } Rule_condition;
        */
        /*********************************************************************************************************************************************************
        **********************************************************************************************************************************************************
        **	PUBLIC STRUCT
        **********************************************************************************************************************************************************
        *********************************************************************************************************************************************************/
        /*
        //Define a conditional formatting rule
        typedef struct _Rule
        {
            Rule_condition condition;
            int number;
            Color background;
            Color foreground;
            typedef struct _Rule next_rule;
        } Rule;
        */
        /*********************************************************************************************************************************************************
        **********************************************************************************************************************************************************
        **	CONSTRUCTORS
        **********************************************************************************************************************************************************
        *********************************************************************************************************************************************************/

        //Empty constructor
        Screen( void );

        /*********************************************************************************************************************************************************
        **********************************************************************************************************************************************************
        **	DESTRUCTORS
        **********************************************************************************************************************************************************
        *********************************************************************************************************************************************************/

       //Empty Destructor
       ~Screen( void );

        /*********************************************************************************************************************************************************
        **********************************************************************************************************************************************************
        **	PUBLIC INIT
        **********************************************************************************************************************************************************
        *********************************************************************************************************************************************************/

        //call the initializations for the driver
        bool init( void );
        //Reset the colors to default
        bool reset_colors( void );

        /*********************************************************************************************************************************************************
        **********************************************************************************************************************************************************
        **	PUBLIC SETTER
        **********************************************************************************************************************************************************
        *********************************************************************************************************************************************************/

        //Set the background and foreground color of a sprite in a given position without changing the content
        int set_color( int origin_h, int origin_w, Color background, Color foreground );
        //Set the default background and foreground color. Change all sprites that use those colors to the new ones
        int set_default_colors( Color new_background, Color new_foreground );
        //Change a color in the palette to another RGB color
        int set_palette_color( Color palette_index, uint8_t r, uint8_t g, uint8_t b );
        //Set the display format of the print number method
        bool set_format( int number_size, Format_align align, Format_format format );
        //Set the display format of the print number method. Include default exponent for ENG number
        bool set_format( int number_size, Format_align align, Format_format format, int exp );

        /*********************************************************************************************************************************************************
        **********************************************************************************************************************************************************
        **	PUBLIC GETTER
        **********************************************************************************************************************************************************
        *********************************************************************************************************************************************************/

        //Get the number of sprites that are pending for update
        int get_pending( void );
        //Get current error of the screen class
        Error get_error( void );
        
        /*********************************************************************************************************************************************************
        **********************************************************************************************************************************************************
        **	PUBLIC METHODS
        **********************************************************************************************************************************************************
        *********************************************************************************************************************************************************/

        //Expose color conversion method
        using Display::color;
        //Core method. FSM that synchronize the frame buffer with the display using the driver
        bool update( void );
        //Swap source color for dest color for each sprite
        int change_color( Color source, Color dest );
        //Clear the screen to solid black, even if there is no black in the palette
        int clear( void );
        //Clear the screen with given colors from the palette. Return number of sprites updated
        int clear( Color color_tmp );
        //Print a character on screen with given palette colors. Return number of sprites updated.
        int print( int origin_h, int origin_w, char c, Color background, Color foreground );
        //Print character with just foreground color. use default colors for background
        int print( int origin_h, int origin_w, char c, Color foreground );
        //Print character. Use default colors
        int print( int origin_h, int origin_w, char c );
        //Print a string with given colors from the palette. Return number of sprites updated
        int print( int origin_h, int origin_w, const char *str, Color background, Color foreground );
        //Print string with just foreground color. use default colors for background
        int print( int origin_h, int origin_w, const char *str, Color foreground );
        //Print string. Use default colors
        int print( int origin_h, int origin_w, const char *str );
        //Print number with given colors. Number format configuration is handled by set_format. Return number of sprites updated 
        int print( int origin_h, int origin_w, int num, Color background, Color foreground );
        //Print number with just foreground color. Number format configuration is handled by set_format.
        int print( int origin_h, int origin_w, int num, Color foreground );
        //Print number. Use default colors. Number format configuration is handled by set_format.
        int print( int origin_h, int origin_w, int num );
        //Draw a solid color sprite on the screen
        int paint( int origin_h, int origin_w, Color color );
        //Show the current error code on the screen. green foreground for ok. red foreground for error
        int print_err( int origin_h, int origin_w );
    
    //Visible only inside the class
    private:
        /*********************************************************************************************************************************************************
        **********************************************************************************************************************************************************
        **	PRIVATE ENUM
        **********************************************************************************************************************************************************
        *********************************************************************************************************************************************************/

        //! @brief States of the Screen side FSM. Display driver also has its own FSM states   
        typedef enum _Fsm_state
        {
            SCAN_SPRITE,    //Search for a sprite
            SEND_SPRITE,    //Ask the Display driver to send sprites to the physical display
        } Fsm_state;
    
        /*********************************************************************************************************************************************************
        **********************************************************************************************************************************************************
        **	PRIVATE STRUCT
        **********************************************************************************************************************************************************
        *********************************************************************************************************************************************************/

        //! @brief Structure that describes a sprite in the sprite frame buffer
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

        //! @brief Status of the update FSM
        typedef struct _Fsm_status
        {
            //width and height scan indexes
            uint16_t scan_w, scan_h;
            //Index withing the sprite sending sequence. Number of steps is defined inside the driver
            uint8_t cnt;
            //Status of the scan
            Fsm_state phase;
        } Fsm_status;
        
        //! @brief number format to be printed by the print number method
        typedef struct _Format_number
        {
            //Number of characters allowed for the number. 0 means no limit
            int size;
            //Number alignment
            Format_align align;
            //Number format
            Format_format format;
            //Base Exponent for the engineering number notation
            int8_t eng_exp;
        }   Format_number;

        /*********************************************************************************************************************************************************
        **********************************************************************************************************************************************************
        **	PRIVATE INIT
        **********************************************************************************************************************************************************
        *********************************************************************************************************************************************************/

        //Initialize class vars
        bool init_class_vars( void );
        //Initialize the sprite frame buffer
        bool init_frame_buffer( void );
        //Initialize the default colors
        bool init_default_colors( void );
        //Initialize the default color palette
        bool init_palette( void );
        //Initialize Screen FSM
        bool init_fsm( void );

        /*********************************************************************************************************************************************************
        **********************************************************************************************************************************************************
        **	PRIVATE TESTERS
        **********************************************************************************************************************************************************
        *********************************************************************************************************************************************************/
        
        //Return true if the character is in the ascii sprite table
        bool is_valid_char( char c );
        //return true if the sprite make use of the background palette color
        bool is_using_background( uint8_t sprite );
        //return true if the sprite make use of the foreground palette color
        bool is_using_foreground( uint8_t sprite );
        //true = sprite_a functionally the same as sprite_b
        bool is_same_sprite( Frame_buffer_sprite sprite_a, Frame_buffer_sprite sprite_b );

        /*********************************************************************************************************************************************************
        **********************************************************************************************************************************************************
        **	PRIVATE METHODS
        **********************************************************************************************************************************************************
        *********************************************************************************************************************************************************/

        //Register a sprite for draw in the display driver if possible. Sprite can be xomplex color map or solid color
        int8_t register_sprite( uint16_t index_h, uint16_t index_w );
        //Update a sprite in the frame buffer and mark it for update if required. Increase workload counter if required.
        int8_t update_sprite( uint16_t index_h, uint16_t index_w, Frame_buffer_sprite new_sprite );
        //Report an error in the Screen class
        void report_error( Error error_code );

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

        //! @brief Error code of the library. Default is Error::OK
        Error g_error_code;
        //! @brief Default background and foreground colors to be used
        Color g_default_background_color;
        Color g_default_foreground_color;
        //! @brief Color Palette. One special code for transparent. Two special indexes store global background and foreground
        uint16_t g_palette[ Config::PALETTE_SIZE ];
        //! @brief Frame Buffer
        Frame_buffer_sprite g_frame_buffer[ Config::FRAME_BUFFER_HEIGHT ][ Config::FRAME_BUFFER_WIDTH ];
        //! @brief Track the number of sprites that require update. At zero the update method quit without scanning and print methods will set the scan to the correct index
        uint16_t g_pending_cnt;
        //! @brief Sprite buffer that stores raw pixel data for a single sprite
        uint16_t g_pixel_data[ Config::SPRITE_PIXEL_COUNT ];
        //! @brief Status of the update FSM
        Fsm_status g_status;
        //! @brief Display format for print numeric values
        Format_number g_format_number;
    
        //Support for font with height of 10 pixels
        #if FONT_HEIGHT == 10
        //! @brief ASCII Sprites. Stored in the flash memory. 95 sprites from space ' ' code 32 to tilda '~' code 126 + special code 127. FONT_HEIGHT is used to change font
        //Font: Courier New 8 with two rows removed from bot and four rows removed from top
        static constexpr uint8_t g_ascii_sprites[96*10] =
        {
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 	//char:  32 ' '
            0x10, 0x10, 0x10, 0x10, 0x10, 0x00, 0x00, 0x10, 0x00, 0x00, 	//char:  33 '!'
            0x6C, 0x6C, 0x24, 0x24, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 	//char:  34 '"'
            0x48, 0x24, 0x7E, 0x24, 0x24, 0x7E, 0x24, 0x12, 0x12, 0x00, 	//char:  35 '#'
            0x38, 0x24, 0x04, 0x18, 0x20, 0x24, 0x1C, 0x10, 0x10, 0x00, 	//char:  36 '$'
            0x04, 0x0A, 0x04, 0x30, 0x0E, 0x10, 0x28, 0x10, 0x00, 0x00, 	//char:  37 '%'
            0x00, 0x70, 0x08, 0x08, 0x18, 0x54, 0x24, 0x78, 0x00, 0x00, 	//char:  38 '&'
            0x08, 0x08, 0x08, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 	//char:  39 '''
            0x20, 0x20, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x20, 0x20, 	//char:  40 '('
            0x04, 0x04, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x04, 0x04, 	//char:  41 ')'
            0x10, 0x7C, 0x10, 0x28, 0x28, 0x00, 0x00, 0x00, 0x00, 0x00, 	//char:  42 '*'
            0x00, 0x10, 0x10, 0x10, 0xFE, 0x10, 0x10, 0x10, 0x00, 0x00, 	//char:  43 '+'
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x18, 0x08, 0x0C, 0x04, 	//char:  44 ','
            0x00, 0x00, 0x00, 0x00, 0x7E, 0x00, 0x00, 0x00, 0x00, 0x00, 	//char:  45 '-'
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x18, 0x18, 0x00, 0x00, 	//char:  46 '.'
            0x20, 0x10, 0x10, 0x08, 0x08, 0x04, 0x04, 0x02, 0x02, 0x00, 	//char:  47 '/'
            0x3C, 0x42, 0x42, 0x42, 0x42, 0x42, 0x42, 0x3C, 0x00, 0x00, 	//char:  48 '0'
            0x10, 0x1C, 0x10, 0x10, 0x10, 0x10, 0x10, 0x7C, 0x00, 0x00, 	//char:  49 '1'
            0x1C, 0x22, 0x20, 0x10, 0x08, 0x04, 0x22, 0x3E, 0x00, 0x00, 	//char:  50 '2'
            0x1C, 0x22, 0x20, 0x18, 0x20, 0x20, 0x22, 0x1C, 0x00, 0x00, 	//char:  51 '3'
            0x30, 0x28, 0x24, 0x24, 0x7E, 0x20, 0x20, 0x70, 0x00, 0x00, 	//char:  52 '4'
            0x7C, 0x04, 0x04, 0x3C, 0x40, 0x40, 0x42, 0x3C, 0x00, 0x00, 	//char:  53 '5'
            0x70, 0x08, 0x04, 0x3C, 0x44, 0x44, 0x44, 0x38, 0x00, 0x00, 	//char:  54 '6'
            0x7E, 0x42, 0x40, 0x20, 0x20, 0x10, 0x10, 0x10, 0x00, 0x00, 	//char:  55 '7'
            0x3C, 0x42, 0x42, 0x3C, 0x42, 0x42, 0x42, 0x3C, 0x00, 0x00, 	//char:  56 '8'
            0x3C, 0x42, 0x42, 0x42, 0x7C, 0x40, 0x20, 0x1E, 0x00, 0x00, 	//char:  57 '9'
            0x00, 0x00, 0x18, 0x18, 0x00, 0x00, 0x18, 0x18, 0x00, 0x00, 	//char:  58 ':'
            0x00, 0x00, 0x18, 0x18, 0x00, 0x00, 0x18, 0x0C, 0x04, 0x00, 	//char:  59 ';'
            0x00, 0x40, 0x30, 0x08, 0x06, 0x08, 0x30, 0x40, 0x00, 0x00, 	//char:  60 '<'
            0x00, 0x00, 0x7E, 0x00, 0x7E, 0x00, 0x00, 0x00, 0x00, 0x00, 	//char:  61 '='
            0x00, 0x02, 0x0C, 0x10, 0x60, 0x10, 0x0C, 0x02, 0x00, 0x00, 	//char:  62 '>'
            0x38, 0x44, 0x40, 0x40, 0x20, 0x10, 0x00, 0x18, 0x00, 0x00, 	//char:  63 '?'
            0x22, 0x22, 0x32, 0x2A, 0x2A, 0x32, 0x02, 0x22, 0x1C, 0x00, 	//char:  64 '@'
            0x18, 0x10, 0x28, 0x28, 0x28, 0x38, 0x44, 0xEE, 0x00, 0x00, 	//char:  65 'A'
            0x3E, 0x44, 0x44, 0x3C, 0x44, 0x44, 0x44, 0x3E, 0x00, 0x00, 	//char:  66 'B'
            0x78, 0x44, 0x02, 0x02, 0x02, 0x02, 0x44, 0x38, 0x00, 0x00, 	//char:  67 'C'
            0x1E, 0x24, 0x44, 0x44, 0x44, 0x44, 0x24, 0x1E, 0x00, 0x00, 	//char:  68 'D'
            0x7E, 0x44, 0x14, 0x1C, 0x14, 0x04, 0x44, 0x7E, 0x00, 0x00, 	//char:  69 'E'
            0x7E, 0x44, 0x14, 0x1C, 0x14, 0x04, 0x04, 0x0E, 0x00, 0x00, 	//char:  70 'F'
            0x78, 0x44, 0x02, 0x02, 0xE2, 0x42, 0x44, 0x38, 0x00, 0x00, 	//char:  71 'G'
            0xEE, 0x44, 0x44, 0x7C, 0x44, 0x44, 0x44, 0xEE, 0x00, 0x00, 	//char:  72 'H'
            0x7C, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x7C, 0x00, 0x00, 	//char:  73 'I'
            0x78, 0x20, 0x20, 0x20, 0x22, 0x22, 0x22, 0x1C, 0x00, 0x00, 	//char:  74 'J'
            0xEE, 0x44, 0x24, 0x14, 0x1C, 0x24, 0x44, 0xCE, 0x00, 0x00, 	//char:  75 'K'
            0x0E, 0x04, 0x04, 0x04, 0x04, 0x44, 0x44, 0x7E, 0x00, 0x00, 	//char:  76 'L'
            0xEE, 0x6C, 0x6C, 0x54, 0x54, 0x44, 0x44, 0xEE, 0x00, 0x00, 	//char:  77 'M'
            0xE7, 0x46, 0x4A, 0x4A, 0x52, 0x52, 0x62, 0x67, 0x00, 0x00, 	//char:  78 'N'
            0x38, 0x44, 0x82, 0x82, 0x82, 0x82, 0x44, 0x38, 0x00, 0x00, 	//char:  79 'O'
            0x3E, 0x44, 0x44, 0x44, 0x3C, 0x04, 0x04, 0x0E, 0x00, 0x00, 	//char:  80 'P'
            0x38, 0x44, 0x82, 0x82, 0x82, 0x82, 0x44, 0x38, 0xF8, 0x00, 	//char:  81 'Q'
            0x3E, 0x44, 0x44, 0x44, 0x3C, 0x24, 0x44, 0x8E, 0x00, 0x00, 	//char:  82 'R'
            0x5C, 0x62, 0x02, 0x3C, 0x40, 0x40, 0x46, 0x3A, 0x00, 0x00, 	//char:  83 'S'
            0xFE, 0x92, 0x10, 0x10, 0x10, 0x10, 0x10, 0x38, 0x00, 0x00, 	//char:  84 'T'
            0xEE, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0x38, 0x00, 0x00, 	//char:  85 'U'
            0xE7, 0x42, 0x42, 0x24, 0x24, 0x24, 0x18, 0x18, 0x00, 0x00, 	//char:  86 'V'
            0xEE, 0x44, 0x44, 0x54, 0x54, 0x54, 0x54, 0x28, 0x00, 0x00, 	//char:  87 'W'
            0xEE, 0x44, 0x28, 0x10, 0x10, 0x28, 0x44, 0xEE, 0x00, 0x00, 	//char:  88 'X'
            0xEE, 0x44, 0x28, 0x28, 0x10, 0x10, 0x10, 0x38, 0x00, 0x00, 	//char:  89 'Y'
            0x7C, 0x44, 0x20, 0x10, 0x10, 0x08, 0x44, 0x7C, 0x00, 0x00, 	//char:  90 'Z'
            0x38, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x38, 	//char:  91 '['
            0x02, 0x04, 0x04, 0x08, 0x08, 0x08, 0x10, 0x10, 0x10, 0x00, 	//char:  92 '\'
            0x0E, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x0E, 	//char:  93 ']'
            0x08, 0x14, 0x22, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 	//char:  94 '^'
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 	//char:  95 '_'
            0x08, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 	//char:  96 '`'
            0x00, 0x00, 0x3C, 0x42, 0x7C, 0x42, 0x62, 0xDC, 0x00, 0x00, 	//char:  97 'a'
            0x03, 0x02, 0x3A, 0x46, 0x42, 0x42, 0x46, 0x3B, 0x00, 0x00, 	//char:  98 'b'
            0x00, 0x00, 0x5C, 0x62, 0x02, 0x02, 0x42, 0x3C, 0x00, 0x00, 	//char:  99 'c'
            0x60, 0x40, 0x5C, 0x62, 0x42, 0x42, 0x62, 0xDC, 0x00, 0x00, 	//char: 100 'd'
            0x00, 0x00, 0x3C, 0x42, 0x7E, 0x02, 0x02, 0x7C, 0x00, 0x00, 	//char: 101 'e'
            0x70, 0x08, 0x7E, 0x08, 0x08, 0x08, 0x08, 0x7E, 0x00, 0x00, 	//char: 102 'f'
            0x00, 0x00, 0xDC, 0x62, 0x42, 0x42, 0x62, 0x5C, 0x40, 0x3C, 	//char: 103 'g'
            0x06, 0x04, 0x34, 0x4C, 0x44, 0x44, 0x44, 0xEE, 0x00, 0x00, 	//char: 104 'h'
            0x10, 0x00, 0x1C, 0x10, 0x10, 0x10, 0x10, 0x7C, 0x00, 0x00, 	//char: 105 'i'
            0x10, 0x00, 0x3C, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x1E, 	//char: 106 'j'
            0x06, 0x04, 0xF4, 0x24, 0x1C, 0x14, 0x24, 0xE6, 0x00, 0x00, 	//char: 107 'k'
            0x18, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x7C, 0x00, 0x00, 	//char: 108 'l'
            0x00, 0x00, 0x4B, 0xB6, 0x92, 0x92, 0x92, 0xB7, 0x00, 0x00, 	//char: 109 'm'
            0x00, 0x00, 0x36, 0x4C, 0x44, 0x44, 0x44, 0xEE, 0x00, 0x00, 	//char: 110 'n'
            0x00, 0x00, 0x3C, 0x42, 0x42, 0x42, 0x42, 0x3C, 0x00, 0x00, 	//char: 111 'o'
            0x00, 0x00, 0x36, 0x4C, 0x44, 0x44, 0x44, 0x3C, 0x04, 0x0E, 	//char: 112 'p'
            0x00, 0x00, 0xDC, 0x62, 0x42, 0x42, 0x62, 0x5C, 0x40, 0xE0, 	//char: 113 'q'
            0x00, 0x00, 0x76, 0x0C, 0x04, 0x04, 0x04, 0x3E, 0x00, 0x00, 	//char: 114 'r'
            0x00, 0x00, 0x7C, 0x42, 0x3C, 0x40, 0x42, 0x3E, 0x00, 0x00, 	//char: 115 's'
            0x00, 0x04, 0x3E, 0x04, 0x04, 0x04, 0x44, 0x38, 0x00, 0x00, 	//char: 116 't'
            0x00, 0x00, 0x66, 0x44, 0x44, 0x44, 0x64, 0xD8, 0x00, 0x00, 	//char: 117 'u'
            0x00, 0x00, 0xE7, 0x42, 0x24, 0x24, 0x18, 0x18, 0x00, 0x00, 	//char: 118 'v'
            0x00, 0x00, 0xEE, 0x44, 0x54, 0x54, 0x54, 0x28, 0x00, 0x00, 	//char: 119 'w'
            0x00, 0x00, 0x66, 0x24, 0x18, 0x18, 0x24, 0x66, 0x00, 0x00, 	//char: 120 'x'
            0x00, 0x00, 0xEE, 0x44, 0x44, 0x28, 0x28, 0x10, 0x10, 0x1C, 	//char: 121 'y'
            0x00, 0x00, 0x7C, 0x24, 0x10, 0x08, 0x44, 0x7C, 0x00, 0x00, 	//char: 122 'z'
            0x10, 0x08, 0x08, 0x08, 0x04, 0x08, 0x08, 0x08, 0x10, 0x00, 	//char: 123 '{'
            0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 	//char: 124 '|'
            0x04, 0x08, 0x08, 0x08, 0x10, 0x08, 0x08, 0x08, 0x04, 0x00, 	//char: 125 '}'
            0x00, 0x00, 0x00, 0x4C, 0x32, 0x00, 0x00, 0x00, 0x00, 0x00, 	//char: 126 '~'
            0xC6, 0xBA, 0xBE, 0xBE, 0xDE, 0xEE, 0xFE, 0xEE, 0xEE, 0x7C, 	//char: 127 ' '
        };
        
        //Support for font with height of 16 pixels
        #elif FONT_HEIGHT == 16
        //! @brief ASCII Sprites. Stored in the flash memory. 95 sprites from space ' ' code 32 to tilda '~' code 126 + special code 127. FONT_HEIGHT is used to change font
        //Font: NSimSun 11 with one row added on top
        static constexpr uint8_t g_ascii_sprites[96*16] =
        {
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,     //char:  32 ' '
            0x00, 0x00, 0x00, 0x00, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x00, 0x08, 0x08, 0x00, 0x00,     //char:  33 '!'
            0x00, 0x00, 0x28, 0x14, 0x14, 0x0A, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,     //char:  34 '"'
            0x00, 0x00, 0x00, 0x00, 0x48, 0x48, 0x7E, 0x24, 0x24, 0x24, 0x7E, 0x24, 0x24, 0x24, 0x00, 0x00,     //char:  35 '#'
            0x00, 0x00, 0x00, 0x10, 0x3C, 0x52, 0x52, 0x14, 0x18, 0x30, 0x50, 0x52, 0x52, 0x3C, 0x10, 0x10,     //char:  36 '$'
            0x00, 0x00, 0x00, 0x00, 0x22, 0x25, 0x15, 0x15, 0x2D, 0x5A, 0x54, 0x54, 0x52, 0x22, 0x00, 0x00,     //char:  37 '%'
            0x00, 0x00, 0x00, 0x00, 0x0C, 0x12, 0x12, 0x0A, 0x66, 0x25, 0x25, 0x29, 0x91, 0x6E, 0x00, 0x00,     //char:  38 '&'
            0x00, 0x00, 0x06, 0x04, 0x04, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,     //char:  39 '''
            0x00, 0x00, 0x40, 0x20, 0x10, 0x10, 0x08, 0x08, 0x08, 0x08, 0x08, 0x10, 0x10, 0x20, 0x40, 0x00,     //char:  40 '('
            0x00, 0x00, 0x01, 0x02, 0x04, 0x04, 0x08, 0x08, 0x08, 0x08, 0x08, 0x04, 0x04, 0x02, 0x01, 0x00,     //char:  41 ')'
            0x00, 0x00, 0x00, 0x00, 0x00, 0x08, 0x08, 0x6B, 0x1C, 0x1C, 0x6B, 0x08, 0x08, 0x00, 0x00, 0x00,     //char:  42 '*'
            0x00, 0x00, 0x00, 0x00, 0x00, 0x10, 0x10, 0x10, 0xFE, 0x10, 0x10, 0x10, 0x00, 0x00, 0x00, 0x00,     //char:  43 '+'
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x06, 0x04, 0x04, 0x02,     //char:  44 ','
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,     //char:  45 '-'
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x06, 0x06, 0x00, 0x00,     //char:  46 '.'
            0x00, 0x00, 0x40, 0x20, 0x20, 0x20, 0x10, 0x10, 0x08, 0x08, 0x08, 0x04, 0x04, 0x02, 0x02, 0x00,     //char:  47 '/'
            0x00, 0x00, 0x00, 0x00, 0x18, 0x24, 0x42, 0x42, 0x42, 0x42, 0x42, 0x42, 0x24, 0x18, 0x00, 0x00,     //char:  48 '0'
            0x00, 0x00, 0x00, 0x00, 0x08, 0x0E, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x3E, 0x00, 0x00,     //char:  49 '1'
            0x00, 0x00, 0x00, 0x00, 0x3C, 0x42, 0x42, 0x40, 0x20, 0x10, 0x08, 0x04, 0x42, 0x7E, 0x00, 0x00,     //char:  50 '2'
            0x00, 0x00, 0x00, 0x00, 0x3C, 0x42, 0x42, 0x20, 0x18, 0x20, 0x40, 0x42, 0x42, 0x3C, 0x00, 0x00,     //char:  51 '3'
            0x00, 0x00, 0x00, 0x00, 0x20, 0x30, 0x30, 0x28, 0x24, 0x22, 0xFE, 0x20, 0x20, 0x78, 0x00, 0x00,     //char:  52 '4'
            0x00, 0x00, 0x00, 0x00, 0x7E, 0x02, 0x02, 0x3E, 0x42, 0x40, 0x40, 0x42, 0x42, 0x3C, 0x00, 0x00,     //char:  53 '5'
            0x00, 0x00, 0x00, 0x00, 0x18, 0x24, 0x02, 0x3A, 0x46, 0x42, 0x42, 0x42, 0x44, 0x38, 0x00, 0x00,     //char:  54 '6'
            0x00, 0x00, 0x00, 0x00, 0x7E, 0x42, 0x20, 0x10, 0x10, 0x10, 0x08, 0x08, 0x08, 0x08, 0x00, 0x00,     //char:  55 '7'
            0x00, 0x00, 0x00, 0x00, 0x3C, 0x42, 0x42, 0x42, 0x3C, 0x24, 0x42, 0x42, 0x42, 0x3C, 0x00, 0x00,     //char:  56 '8'
            0x00, 0x00, 0x00, 0x00, 0x1C, 0x22, 0x42, 0x42, 0x62, 0x5C, 0x40, 0x40, 0x24, 0x18, 0x00, 0x00,     //char:  57 '9'
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x18, 0x18, 0x00, 0x00, 0x00, 0x18, 0x18, 0x00, 0x00,     //char:  58 ':'
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x08, 0x08, 0x08,     //char:  59 ';'
            0x00, 0x00, 0x00, 0x00, 0x20, 0x10, 0x08, 0x04, 0x02, 0x02, 0x04, 0x08, 0x10, 0x20, 0x00, 0x00,     //char:  60 '<'
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7F, 0x00, 0x00, 0x7F, 0x00, 0x00, 0x00, 0x00, 0x00,     //char:  61 '='
            0x00, 0x00, 0x00, 0x00, 0x04, 0x08, 0x10, 0x20, 0x40, 0x40, 0x20, 0x10, 0x08, 0x04, 0x00, 0x00,     //char:  62 '>'
            0x00, 0x00, 0x00, 0x00, 0x3C, 0x42, 0x42, 0x42, 0x20, 0x10, 0x10, 0x00, 0x18, 0x18, 0x00, 0x00,     //char:  63 '?'
            0x00, 0x00, 0x00, 0x00, 0x1C, 0x22, 0x59, 0x55, 0x55, 0x55, 0x55, 0x39, 0x42, 0x3C, 0x00, 0x00,     //char:  64 '@'
            0x00, 0x00, 0x00, 0x00, 0x08, 0x08, 0x14, 0x14, 0x14, 0x14, 0x3E, 0x22, 0x22, 0x77, 0x00, 0x00,     //char:  65 'A'
            0x00, 0x00, 0x00, 0x00, 0x3F, 0x42, 0x42, 0x22, 0x1E, 0x22, 0x42, 0x42, 0x42, 0x3F, 0x00, 0x00,     //char:  66 'B'
            0x00, 0x00, 0x00, 0x00, 0x7C, 0x42, 0x41, 0x01, 0x01, 0x01, 0x01, 0x41, 0x22, 0x1C, 0x00, 0x00,     //char:  67 'C'
            0x00, 0x00, 0x00, 0x00, 0x1F, 0x22, 0x42, 0x42, 0x42, 0x42, 0x42, 0x42, 0x22, 0x1F, 0x00, 0x00,     //char:  68 'D'
            0x00, 0x00, 0x00, 0x00, 0x3F, 0x42, 0x12, 0x12, 0x1E, 0x12, 0x12, 0x02, 0x42, 0x3F, 0x00, 0x00,     //char:  69 'E'
            0x00, 0x00, 0x00, 0x00, 0x3F, 0x42, 0x12, 0x12, 0x1E, 0x12, 0x12, 0x02, 0x02, 0x07, 0x00, 0x00,     //char:  70 'F'
            0x00, 0x00, 0x00, 0x00, 0x3C, 0x22, 0x21, 0x01, 0x01, 0x01, 0x71, 0x21, 0x22, 0x1C, 0x00, 0x00,     //char:  71 'G'
            0x00, 0x00, 0x00, 0x00, 0x77, 0x22, 0x22, 0x22, 0x3E, 0x22, 0x22, 0x22, 0x22, 0x77, 0x00, 0x00,     //char:  72 'H'
            0x00, 0x00, 0x00, 0x00, 0x3E, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x3E, 0x00, 0x00,     //char:  73 'I'
            0x00, 0x00, 0x00, 0x00, 0x7C, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x11, 0x0F,     //char:  74 'J'
            0x00, 0x00, 0x00, 0x00, 0x77, 0x22, 0x12, 0x0A, 0x0E, 0x0A, 0x12, 0x12, 0x22, 0x77, 0x00, 0x00,     //char:  75 'K'
            0x00, 0x00, 0x00, 0x00, 0x07, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x42, 0x7F, 0x00, 0x00,     //char:  76 'L'
            0x00, 0x00, 0x00, 0x00, 0x77, 0x36, 0x36, 0x36, 0x36, 0x2A, 0x2A, 0x2A, 0x2A, 0x6B, 0x00, 0x00,     //char:  77 'M'
            0x00, 0x00, 0x00, 0x00, 0x77, 0x26, 0x26, 0x2A, 0x2A, 0x2A, 0x32, 0x32, 0x32, 0x27, 0x00, 0x00,     //char:  78 'N'
            0x00, 0x00, 0x00, 0x00, 0x1C, 0x22, 0x41, 0x41, 0x41, 0x41, 0x41, 0x41, 0x22, 0x1C, 0x00, 0x00,     //char:  79 'O'
            0x00, 0x00, 0x00, 0x00, 0x3F, 0x42, 0x42, 0x42, 0x3E, 0x02, 0x02, 0x02, 0x02, 0x07, 0x00, 0x00,     //char:  80 'P'
            0x00, 0x00, 0x00, 0x00, 0x1C, 0x22, 0x41, 0x41, 0x41, 0x41, 0x41, 0x4D, 0x32, 0x1C, 0x60, 0x00,     //char:  81 'Q'
            0x00, 0x00, 0x00, 0x00, 0x1F, 0x22, 0x22, 0x22, 0x1E, 0x0A, 0x12, 0x12, 0x22, 0x67, 0x00, 0x00,     //char:  82 'R'
            0x00, 0x00, 0x00, 0x00, 0x7C, 0x42, 0x42, 0x02, 0x0C, 0x30, 0x40, 0x42, 0x42, 0x3E, 0x00, 0x00,     //char:  83 'S'
            0x00, 0x00, 0x00, 0x00, 0x7F, 0x49, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x1C, 0x00, 0x00,     //char:  84 'T'
            0x00, 0x00, 0x00, 0x00, 0x77, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x1C, 0x00, 0x00,     //char:  85 'U'
            0x00, 0x00, 0x00, 0x00, 0x77, 0x22, 0x22, 0x22, 0x14, 0x14, 0x14, 0x14, 0x08, 0x08, 0x00, 0x00,     //char:  86 'V'
            0x00, 0x00, 0x00, 0x00, 0x6B, 0x2A, 0x2A, 0x2A, 0x2A, 0x36, 0x14, 0x14, 0x14, 0x14, 0x00, 0x00,     //char:  87 'W'
            0x00, 0x00, 0x00, 0x00, 0x77, 0x22, 0x14, 0x14, 0x08, 0x08, 0x14, 0x14, 0x22, 0x77, 0x00, 0x00,     //char:  88 'X'
            0x00, 0x00, 0x00, 0x00, 0x77, 0x22, 0x22, 0x14, 0x14, 0x08, 0x08, 0x08, 0x08, 0x1C, 0x00, 0x00,     //char:  89 'Y'
            0x00, 0x00, 0x00, 0x00, 0x7C, 0x22, 0x20, 0x10, 0x10, 0x08, 0x08, 0x04, 0x44, 0x7E, 0x00, 0x00,     //char:  90 'Z'
            0x00, 0x00, 0x38, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x38, 0x00,     //char:  91 '['
            0x00, 0x00, 0x00, 0x02, 0x02, 0x04, 0x04, 0x08, 0x08, 0x08, 0x10, 0x10, 0x20, 0x20, 0x20, 0x40,     //char:  92 '\'
            0x00, 0x00, 0x1E, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x1E, 0x00,     //char:  93 ']'
            0x00, 0x00, 0x18, 0x24, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,     //char:  94 '^'
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7F,     //char:  95 '_'
            0x00, 0x06, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,     //char:  96 '`'
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1C, 0x22, 0x30, 0x2C, 0x22, 0x32, 0x6C, 0x00, 0x00,     //char:  97 'a'
            0x00, 0x00, 0x00, 0x03, 0x02, 0x02, 0x02, 0x3A, 0x46, 0x42, 0x42, 0x42, 0x42, 0x3E, 0x00, 0x00,     //char:  98 'b'
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x38, 0x44, 0x02, 0x02, 0x02, 0x44, 0x38, 0x00, 0x00,     //char:  99 'c'
            0x00, 0x00, 0x00, 0x60, 0x40, 0x40, 0x40, 0x7C, 0x42, 0x42, 0x42, 0x42, 0x62, 0xDC, 0x00, 0x00,     //char: 100 'd'
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3C, 0x42, 0x42, 0x7E, 0x02, 0x42, 0x3C, 0x00, 0x00,     //char: 101 'e'
            0x00, 0x00, 0x00, 0x30, 0x48, 0x08, 0x08, 0x3E, 0x08, 0x08, 0x08, 0x08, 0x08, 0x3E, 0x00, 0x00,     //char: 102 'f'
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7C, 0x22, 0x22, 0x1C, 0x02, 0x3C, 0x42, 0x42, 0x3C,     //char: 103 'g'
            0x00, 0x00, 0x00, 0x03, 0x02, 0x02, 0x02, 0x3A, 0x46, 0x42, 0x42, 0x42, 0x42, 0xE7, 0x00, 0x00,     //char: 104 'h'
            0x00, 0x00, 0x00, 0x0C, 0x0C, 0x00, 0x00, 0x0E, 0x08, 0x08, 0x08, 0x08, 0x08, 0x3E, 0x00, 0x00,     //char: 105 'i'
            0x00, 0x00, 0x00, 0x30, 0x30, 0x00, 0x00, 0x38, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x22, 0x1E,     //char: 106 'j'
            0x00, 0x00, 0x00, 0x03, 0x02, 0x02, 0x02, 0x72, 0x12, 0x0A, 0x16, 0x12, 0x22, 0x77, 0x00, 0x00,     //char: 107 'k'
            0x00, 0x00, 0x00, 0x0E, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x3E, 0x00, 0x00,     //char: 108 'l'
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1F, 0x2A, 0x2A, 0x2A, 0x2A, 0x2A, 0x6B, 0x00, 0x00,     //char: 109 'm'
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3B, 0x46, 0x42, 0x42, 0x42, 0x42, 0xE7, 0x00, 0x00,     //char: 110 'n'
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x18, 0x24, 0x42, 0x42, 0x42, 0x24, 0x18, 0x00, 0x00,     //char: 111 'o'
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3B, 0x46, 0x42, 0x42, 0x42, 0x46, 0x3A, 0x02, 0x07,     //char: 112 'p'
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x5C, 0x62, 0x42, 0x42, 0x42, 0x62, 0x5C, 0x40, 0xE0,     //char: 113 'q'
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x77, 0x4C, 0x04, 0x04, 0x04, 0x04, 0x1F, 0x00, 0x00,     //char: 114 'r'
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3C, 0x22, 0x02, 0x1C, 0x20, 0x22, 0x1E, 0x00, 0x00,     //char: 115 's'
            0x00, 0x00, 0x00, 0x00, 0x00, 0x08, 0x08, 0x3E, 0x08, 0x08, 0x08, 0x08, 0x48, 0x30, 0x00, 0x00,     //char: 116 't'
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x63, 0x42, 0x42, 0x42, 0x42, 0x62, 0xDC, 0x00, 0x00,     //char: 117 'u'
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x77, 0x22, 0x22, 0x14, 0x14, 0x08, 0x08, 0x00, 0x00,     //char: 118 'v'
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x6B, 0x2A, 0x2A, 0x2A, 0x14, 0x14, 0x14, 0x00, 0x00,     //char: 119 'w'
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x6E, 0x24, 0x18, 0x18, 0x18, 0x24, 0x76, 0x00, 0x00,     //char: 120 'x'
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xE7, 0x42, 0x24, 0x24, 0x18, 0x18, 0x08, 0x08, 0x06,     //char: 121 'y'
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7E, 0x22, 0x10, 0x08, 0x08, 0x44, 0x7E, 0x00, 0x00,     //char: 122 'z'
            0x00, 0x00, 0x60, 0x20, 0x20, 0x20, 0x20, 0x20, 0x10, 0x20, 0x20, 0x20, 0x20, 0x20, 0x60, 0x00,     //char: 123 '{'
            0x00, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10,     //char: 124 '|'
            0x00, 0x00, 0x06, 0x04, 0x04, 0x04, 0x04, 0x04, 0x08, 0x04, 0x04, 0x04, 0x04, 0x04, 0x06, 0x00,     //char: 125 '}'
            0x00, 0x04, 0x5A, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,     //char: 126 '~'
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,     //char: 127 ' '
        };
        #else
            #error "ERR: Font size not supported"
        #endif
};	//End Class: Screen

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
//!	\n Empty constructor
/***************************************************************************/

Screen::Screen( void ) : Longan_nano::Display::Display()
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
//!	~Screen | void |
/***************************************************************************/
//! @details
//!	\n Void destructor
/***************************************************************************/

Screen::~Screen( void )
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
} //End Destructor: ~Screen | void |

    /*********************************************************************************************************************************************************
    **********************************************************************************************************************************************************
    **	PUBLIC INIT
    **********************************************************************************************************************************************************
    *********************************************************************************************************************************************************/

/***************************************************************************/
//!	@brief public init
//!	init | void
/***************************************************************************/
//! @return bool | false = OK | true = ERR
//! @details
//!	\n call the initializations for the driver
/***************************************************************************/

bool Screen::init( void )
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

    //Initialize the display driver. It handles phisical communication with the display and provide methods to write sprites
    f_ret = this -> Longan_nano::Display::init();
    //Initialize colors
    this -> init_default_colors();
    //Initialize the frame buffer
    f_ret |= this -> init_frame_buffer();
    //Initialize default palette
    f_ret |= this -> init_palette();
    //Initialize update FSM
    f_ret |= this -> init_fsm();
    //Clear the display to black
    this -> Display::clear();

    //----------------------------------------------------------------
    //	RETURN
    //----------------------------------------------------------------
    DRETURN();
    return f_ret;
}	//End public init: init | void

/***************************************************************************/
//!	@brief public init
//!	reset_colors | 
/***************************************************************************/
//! @return bool | false = OK | true = ERR
//! @details
//!	\n Reset the colors to default
/***************************************************************************/

bool Screen::reset_colors( void )
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
    
    this -> init_default_colors();
    f_ret |= this -> init_palette();
    this -> Display::clear();

    //----------------------------------------------------------------
    //	RETURN
    //----------------------------------------------------------------
    DRETURN();
    return f_ret;	//OK
}	//End public init: reset_colors | void

    /*********************************************************************************************************************************************************
    **********************************************************************************************************************************************************
    **	PUBLIC SETTER
    **********************************************************************************************************************************************************
    *********************************************************************************************************************************************************/

/***************************************************************************/
//!	@brief public setter
//!	set_color | int | int | Color | Color
/***************************************************************************/
//!	@param origin_h | int | height position of the sprite
//!	@param origin_w | int | width position of the sprite
//!	@param background | Color | background color of ths sprite as index from the palette
//!	@param foreground | Color | foreground color of ths sprite as index from the palette
//! @return int | <0 ERR | >= 0 number of updated sprites
//! @details
//! \n Set background and foreground color of a given sprite
/***************************************************************************/

inline int Screen::set_color( int origin_h, int origin_w, Color background, Color foreground )
{
    DENTER();
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

    //----------------------------------------------------------------
    //	BODY
    //----------------------------------------------------------------

    //Has the sprite changed?
    bool f_changed = false;
    //Fetch sprite
    Frame_buffer_sprite sprite_tmp = g_frame_buffer[origin_h][origin_w];
    //If: sprite uses color and color has changed
    if ((this -> is_using_background(sprite_tmp.sprite_index) == true) && (sprite_tmp.background_color != background))
    {
        //Update colors
        sprite_tmp.background_color = background;
        f_changed = true;
    }
    //If: sprite uses color and color has changed
    if ((this -> is_using_foreground(sprite_tmp.sprite_index) == true) && (sprite_tmp.foreground_color != foreground))
    {
        //Update colors
        sprite_tmp.foreground_color = foreground;
        f_changed = true;
    }
    //Number of sprites changed
    int ret;
    //If the sprite has changed
    if (f_changed == true)
    {
        //Update the frame buffer with the new sprite if needed
        ret = this -> update_sprite( origin_h, origin_w, sprite_tmp );
    }

    //----------------------------------------------------------------
    //	RETURN
    //----------------------------------------------------------------
    DRETURN();
    return ret;   //OK
}	//End public setter: set_color | int | int | Color | Color

/***************************************************************************/
//!	@brief public setter
//!	set_default_colors | Color | Color
/***************************************************************************/
//!	@param new_background | Color | background color of ths sprite as index from the palette
//!	@param new_foreground | Color | foreground color of ths sprite as index from the palette
//! @return int16_t | >=0 Number of sprites changed | < 0 error |
//! @details
//! \n Change the default background and foreground colors
//! \n All existing sprites are automatically updated as needed
/***************************************************************************/

int Screen::set_default_colors( Color new_background, Color new_foreground )
{
    DENTER();
    //----------------------------------------------------------------
    //	CHECK
    //----------------------------------------------------------------

    //If: colors are bad
    if ((new_background >= (Color)Config::PALETTE_SIZE) || (new_foreground >= (Color)Config::PALETTE_SIZE))
    {
        DRETURN_ARG("ERR: bad default colors | Back: %3d | Fore: %3d |\n", new_background, new_foreground );
        return -1;    //FAIL
    }            

    //----------------------------------------------------------------
    //	VARS
    //----------------------------------------------------------------

    //Temp sprite
    Frame_buffer_sprite sprite_tmp;
    //Num of updated sprites
    int ret;
    //Compute branch flags
    Color old_background = this -> g_default_background_color;
    Color old_foreground = this -> g_default_foreground_color;
    bool f_background_change = (this -> g_default_background_color != new_background);
    bool f_foreground_change = (this -> g_default_foreground_color != new_foreground);
    //Number of sprites changed
    int num_changed_sprites = 0;

    //----------------------------------------------------------------
    //	BODY
    //----------------------------------------------------------------

    //If: nothing to do
    if ( (f_background_change == false) && (f_foreground_change == false))
    {
        //Do nothing
    }
    //If: only background change
    if ( (f_background_change == true) && (f_foreground_change == false))
    {
        //Update defaults
        this -> g_default_background_color = new_background;
        //For: scan height
        for (uint8_t th = 0;th < Config::FRAME_BUFFER_HEIGHT;th++)
        {
            //For: scan width
            for (uint8_t tw = 0;tw < Config::FRAME_BUFFER_WIDTH;tw++)
            {
                //Fetch sprite
                sprite_tmp = this -> g_frame_buffer[ th ][ tw ];
                //If: the background is a color to be swapped and the character uses the background
                if ( (sprite_tmp.background_color == old_background) && (this -> is_using_background( sprite_tmp.sprite_index )) )
                {
                    //Change the color
                    sprite_tmp.background_color = new_background;
                    //Update the sprite
                    ret = this -> update_sprite( th, tw, sprite_tmp );
                    //If: failed to update sprite
                    if ((Config::PEDANTIC_CHECKS == true) && (ret < 0))
                    {
                        DRETURN_ARG("ERR: Failed to update sprite\n");
                        return -1;
                    }
                    //If: either zero or one sprite was changed
                    else
                    {
                        //Accumulate number of sprites changed
                        num_changed_sprites += ret;
                    }
                }
            } //End For: scan width
        } //For: scan height
    } //End If: only background change
    //If: only foreground change
    else if ( (f_background_change == false) && (f_foreground_change == true))
    {
        //Update defaults
        this -> g_default_foreground_color = new_foreground;
        //For: scan height
        for (uint8_t th = 0;th < Config::FRAME_BUFFER_HEIGHT;th++)
        {
            //For: scan width
            for (uint8_t tw = 0;tw < Config::FRAME_BUFFER_WIDTH;tw++)
            {
                //Fetch sprite
                sprite_tmp = this -> g_frame_buffer[ th ][ tw ];
                //If: the background is a color to be swapped and the character uses the background
                if ( (sprite_tmp.foreground_color == old_foreground) && (this -> is_using_foreground( sprite_tmp.sprite_index )) )
                {
                    //Change the color
                    sprite_tmp.foreground_color = new_foreground;
                    //Update the sprite
                    ret = this -> update_sprite( th, tw, sprite_tmp );
                    //If: failed to update sprite
                    if ((Config::PEDANTIC_CHECKS == true) && (ret < 0))
                    {
                        DRETURN_ARG("ERR: Failed to update sprite\n");
                        return -1;
                    }
                    //If: either zero or one sprite was changed
                    else
                    {
                        //Accumulate number of sprites changed
                        num_changed_sprites += ret;
                    }
                }
            } //End For: scan width
        } //For: scan height
    } //End If: only foreground change
    //If: both colors have changed
    else //if ( (f_background_change == true) && (f_foreground_change == true))
    {
        //Has sprite changed
        bool f_changed = false;
        //Update defaults
        this -> g_default_background_color = new_background;
        this -> g_default_foreground_color = new_foreground;
        //For: scan height
        for (uint8_t th = 0;th < Config::FRAME_BUFFER_HEIGHT;th++)
        {
            //For: scan width
            for (uint8_t tw = 0;tw < Config::FRAME_BUFFER_WIDTH;tw++)
            {
                //Fetch sprite
                sprite_tmp = this -> g_frame_buffer[ th ][ tw ];
                //If: the background is a color to be swapped and the character uses the background
                if ( (sprite_tmp.background_color == old_background) && (this -> is_using_background( sprite_tmp.sprite_index )) )
                {
                    //Change the color
                    f_changed = true;
                    sprite_tmp.background_color = new_background;
                }
                //If: the background is a color to be swapped and the character uses the background
                if ( (sprite_tmp.foreground_color == old_foreground) && (this -> is_using_foreground( sprite_tmp.sprite_index )) )
                {
                    //Change the color
                    f_changed = true;
                    sprite_tmp.foreground_color = new_foreground;
                }
                //If: Sprite has changed
                if (f_changed == true)
                {
                    //Mark sprite for update
                    sprite_tmp.f_update = true;
                    //Update the sprite
                    ret = this -> update_sprite( th, tw, sprite_tmp );
                    //If: failed to update sprite
                    if ((Config::PEDANTIC_CHECKS == true) && (ret < 0))
                    {
                        DRETURN_ARG("ERR: Failed to update sprite\n");
                        return -1;
                    }
                    //If: either zero or one sprite was changed
                    else
                    {
                        //Accumulate number of sprites changed
                        num_changed_sprites += ret;
                    }
                }
            } //End For: scan width
        } //For: scan height
    } //End If: both colors have changed

    //----------------------------------------------------------------
    //	RETURN
    //----------------------------------------------------------------
    DRETURN();
    return num_changed_sprites;   //Number of sprites changed
}	//End public setter: set_default_colors | Color | Color

/***************************************************************************/
//!	@brief public setter
//!	set_palette_color | Color | uint8_t | uint8_t | uint8_t |
/***************************************************************************/
//! @param palette_index | Color | color index of the palette to be changed
//! @param r | uint8_t | red color channel
//! @param g | uint8_t | red color channel
//! @param b | uint8_t | red color channel
//! @return int | >=0 Number of sprites changed | < 0 error |
//! @details
//! \n Change a color in the palette to another RGB color
//!	\n Change all sprites that use that color and mark them for update
//! \n Use the conversion function provided by the Display driver to compute the right color space
/***************************************************************************/

int Screen::set_palette_color( Color palette_index, uint8_t r, uint8_t g, uint8_t b )
{
    DENTER_ARG("source: %d |\n", palette_index);
    //----------------------------------------------------------------
    //	CHECK
    //----------------------------------------------------------------
    //If: bad colors
    if ((Config::PEDANTIC_CHECKS == true) && (palette_index >= (Color)Config::PALETTE_SIZE))
    {
        DRETURN_ARG("ERR: bad palette index | source: %d |\n", palette_index );
        return -1;
    }

    //----------------------------------------------------------------
    //	CHANGE PALETTE
    //----------------------------------------------------------------

    //Ask the display driver to compute the RGB color to the color supported by the screen
    uint16_t new_color = Display::color( r, g, b );
    //If: desired color is already in the palette
    if (this -> g_palette[ palette_index ] == new_color)
    {
        DRETURN();    
        return 0;
    }
    //Set the color in the palette
    this -> g_palette[ palette_index ] = new_color;

    //----------------------------------------------------------------
    //	MARK CHANGED SPRITES FOR UPDATE
    //----------------------------------------------------------------

    //Temp sprite
    Frame_buffer_sprite sprite_tmp;
    //Num of updated sprites
    int ret;
    bool f_sprite_changed;
    int num_changed_sprites = 0;
    //For: scan height
    for (uint8_t th = 0;th < Config::FRAME_BUFFER_HEIGHT;th++)
    {
        //For: scan width
        for (uint8_t tw = 0;tw < Config::FRAME_BUFFER_WIDTH;tw++)
        {
            //Fetch sprite
            sprite_tmp = this -> g_frame_buffer[ th ][ tw ];
            //Was sprite changed?
            f_sprite_changed = false;
            //If: the background is a color to be swapped and the character uses the background
            if ( (sprite_tmp.background_color == palette_index) && (this -> is_using_background( sprite_tmp.sprite_index )) )
            {
                f_sprite_changed = true;
            }
            //If: the foreground is a color to be swapped and the character uses the foreground
            if ( (sprite_tmp.foreground_color == palette_index) && (this -> is_using_foreground( sprite_tmp.sprite_index )) )
            {
                f_sprite_changed = true;                        
            }
            //If: sprite was changed
            if (f_sprite_changed == true)
            {
                //Update the sprite
                ret = this -> update_sprite( th, tw, sprite_tmp );
                //If: failed to update sprite
                if ((Config::PEDANTIC_CHECKS == true) && (ret < 0))
                {
                    DRETURN_ARG("ERR: Failed to update sprite\n");
                    return -1;
                }
                //If: either zero or one sprite was changed
                else
                {
                    //Accumulate number of sprites changed
                    num_changed_sprites += ret;
                }
            }
        } //End For: scan width
    } //For: scan height

    //----------------------------------------------------------------
    //	RETURN
    //----------------------------------------------------------------
    DRETURN();
    return num_changed_sprites;
}	//End public setter: set_palette_color | Color | uint8_t |  uint8_t |  uint8_t |

/***************************************************************************/
//!	@brief public setter
//!	set_format | int | Format_align | Format_format |
/***************************************************************************/
//! @param number_size | int | sprites reserved for the number
//! @param align | Format_align | set alignment of the number
//! @param format | Format_format | set display format of the number
//! @return bool | false = OK | true = ERR
//! @details
//!	\n With left adjust, the number is print starting from left an origin is the position of the most significant number
//!	\n With right adjust, the origin is the position of the least significant number        
//!	\n LEFT        78901       12.71m
//!	\n RIGHT   78901      12.71m 
//!	\n ORIGIN      ^           ^
//!	\n The format also specifies full number or enginnering format
/***************************************************************************/

inline bool Screen::set_format( int number_size, Format_align align, Format_format format )
{
    DENTER_ARG("number_size: %d |align: %d | format: %d\n", number_size, (int)align, (int)format );
    //----------------------------------------------------------------
    //	CHECK
    //----------------------------------------------------------------
    //If: bad number size
    if (number_size <= 0)
    {
        DENTER_ARG("ERR: bad number_size: %d\n", number_size );
        return true; //ERR
    }
    
    //----------------------------------------------------------------
    //	BODY
    //----------------------------------------------------------------

    //Configure number format for the next print
    this -> g_format_number.size = number_size;
    this -> g_format_number.align = align;
    this -> g_format_number.format = format;

    //----------------------------------------------------------------
    //	RETURN
    //----------------------------------------------------------------
    DRETURN();
    return false; //OK
}	//End public setter: set_format | int | Format_align | Format_format |

/***************************************************************************/
//!	@brief public setter
//!	set_format | int | Format_align | Format_format | int |
/***************************************************************************/
//! @param number_size | int | sprites reserved for the number
//! @param align | Format_align | set alignment of the number
//! @param format | Format_format | set display format of the number
//! @param exp | int | ENG number exponent
//! @return bool | false = OK | true = ERR
//! @details
//!	\n With left adjust, the number is print starting from left an origin is the position of the most significant number
//!	\n With right adjust, the origin is the position of the least significant number        
//!	\n LEFT        78901       12.71m
//!	\n RIGHT   78901      12.71m 
//!	\n ORIGIN      ^           ^
//!	\n The format also specifies full number or enginnering format
/***************************************************************************/

inline bool Screen::set_format( int number_size, Format_align align, Format_format format, int exp )
{
    DENTER_ARG("number_size: %d |align: %d | format: %d | ENG exponent: %d\n", number_size, (int)align, (int)format, exp );
    //----------------------------------------------------------------
    //	CHECK
    //----------------------------------------------------------------
    //If: bad number size
    if (number_size <= 0)
    {
        DENTER_ARG("ERR: bad number_size: %d\n", number_size );
        return true; //ERR
    }
    //If: bad exponent
    if ((exp < -6) || (exp > 6))
    {
        DENTER_ARG("ERR: bad ENG exponent: %d\n", exp );
        return true; //ERR
    }
    
    //----------------------------------------------------------------
    //	BODY
    //----------------------------------------------------------------

    //Configure number format for the next print
    this -> g_format_number.size = number_size;
    this -> g_format_number.align = align;
    this -> g_format_number.format = format;
    //Configure ENG number exponent
    this -> g_format_number.eng_exp = exp;

    //----------------------------------------------------------------
    //	RETURN
    //----------------------------------------------------------------
    DRETURN();
    return false; //OK
}	//End public setter: set_format | int | Format_align | Format_format | int |

    /*********************************************************************************************************************************************************
    **********************************************************************************************************************************************************
    **	PUBLIC GETTERS
    **********************************************************************************************************************************************************
    *********************************************************************************************************************************************************/

/***************************************************************************/
//!	@brief public getter
//!	get_pending | void |
/***************************************************************************/
//! @return int | number of sprites pending for update in the frame buffer
//!	@details
//! \n return the number of sprites pending for update in the frame buffer
/***************************************************************************/

int Screen::get_pending( void )
{
    DENTER(); //Trace Enter
    ///--------------------------------------------------------------------------
    ///	RETURN
    ///--------------------------------------------------------------------------
    DRETURN_ARG("Pending: %d", this -> g_pending_cnt ); //Trace Return
    return this -> g_pending_cnt ;	//OK
}	//end public getter: get_pending | void |

/***************************************************************************/
//!	@brief public getter
//!	get_pending | void |
/***************************************************************************/
//! @return int | number of sprites pending for update in the frame buffer
//!	@details
//! \n return the number of sprites pending for update in the frame buffer
/***************************************************************************/

Screen::Error Screen::get_error( void )
{
    DENTER(); //Trace Enter
    ///--------------------------------------------------------------------------
    ///	RETURN
    ///--------------------------------------------------------------------------
    DRETURN_ARG("ERR%d", (int)this -> g_error_code ); //Trace Return
    return this -> g_error_code ;	//OK
}	//end public getter: get_pending | void |

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

bool Screen::update( void )
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
        //If: I'm scanning for the next sprite to be updated
        if (status.phase == Fsm_state::SCAN_SPRITE)
        {
            //Fetch the sprite
            Frame_buffer_sprite sprite_tmp = this -> g_frame_buffer[ status.scan_h ][ status.scan_w ];

            //If: there are no sprites to be updated in the frame buffer
            if (this -> g_pending_cnt == 0)
            {
                //I'm done. Don't wasete time scanning
                f_continue = false;
            }
            //If: the sprite indexed is to be updated
            else if (sprite_tmp.f_update == true)
            {
                //This sprite is not to be updated anymore
                this -> g_frame_buffer[ status.scan_h ][ status.scan_w ].f_update = false;                
                //If: pending counter is already zero
                if ((Config::PEDANTIC_CHECKS == true) && (this -> g_pending_cnt == 0))
                {
                    //Algorithmic error
                    this -> report_error( Screen::Error::PENDING_UNDERFLOW );
                    DRETURN_ARG("ERR: %d\n", (int)this -> get_error() );
                    return true;
                }
                //If: pending ccounter is valid
                else
                {
                    //A pending sprite has been processed. This is the only code allowed to reduce workload
                    this -> g_pending_cnt--;
                }
                DPRINT("REFRESH sprite h: %5d | w: %5d\n", status.scan_h, status.scan_w );
                //Compute the pixel data and try to register the sprite for draw inside the display driver
                int ret = this -> register_sprite( status.scan_h, status.scan_w );
                //If: no sprites were registered but no errors occurred
                if (ret == 0)
                {
                    //Maybe a transparent sprite. Keep scanning for sprites
                }
                //If: sprite has been registered for draw
                else if (ret > 0)
                {
                    //Begin execution of the driver FSM that sends pixel data to the screen
                    status.phase = Fsm_state::SEND_SPRITE;
                    //Count number of execution steps of the Display FSM
                    status.cnt = 0;
                }
                //If: failed to register sprite
                else
                {
                    DPRINT("ERR: Failed to register sprite\n");
                    //Reset the update FSM
                    this -> init_fsm();
                    f_continue = false;
                }

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
                    //Move down in height (2nd rank of frame buffer vector)
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
//!	change_color | Color | Color |
/***************************************************************************/
//! @param source | Color | color to be changed
//! @param dest | Color | replacement color
//! @return int | >=0 Number of sprites changed | < 0 error |
//! @details
//!	Every sprite that use the "source" palette color as either background or foreground
//! has it swapped for the "dest" palette color. All changed sprites are marked for update
/***************************************************************************/

int Screen::change_color( Color source, Color dest )
{
    DENTER_ARG("source: %d | dest: %d |\n", source, dest);
    //----------------------------------------------------------------
    //	CHECK
    //----------------------------------------------------------------
    //If: bad colors
    if ((Config::PEDANTIC_CHECKS == true) && ((source >= (Color)Config::PALETTE_SIZE) || (dest >= (Color)Config::PALETTE_SIZE)) )
    {
        DRETURN_ARG("ERR: bad colors | source: %d | dest: %d |\n", source, dest );
        return -1;
    }

    //----------------------------------------------------------------
    //	BODY
    //----------------------------------------------------------------
    
    //Temp sprite
    Frame_buffer_sprite sprite_tmp;
    //Num of updated sprites
    int ret = 0;
    bool f_sprite_changed;
    //For: scan height
    for (uint8_t th = 0;th < Config::FRAME_BUFFER_HEIGHT;th++)
    {
        //For: scan width
        for (uint8_t tw = 0;tw < Config::FRAME_BUFFER_WIDTH;tw++)
        {
            //Fetch sprite
            sprite_tmp = this -> g_frame_buffer[ th ][ tw ];
            //Was sprite changed?
            f_sprite_changed = false;
            //If: the background is a color to be swapped and the character uses the background
            if ( (sprite_tmp.background_color == source) && (this -> is_using_background( sprite_tmp.sprite_index )) )
            {
                //Swap the color
                sprite_tmp.background_color = dest;
                f_sprite_changed = true;
            }
            //If: the foreground is a color to be swapped and the character uses the foreground
            if ( (sprite_tmp.foreground_color == source) && (this -> is_using_foreground( sprite_tmp.sprite_index )) )
            {
                //Swap the color
                sprite_tmp.foreground_color = dest;
                f_sprite_changed = true;                        
            }
            //If: sprite was changed
            if (f_sprite_changed == true)
            {
                
                this -> update_sprite( th, tw, sprite_tmp );

            }
        } //End For: scan width
    } //For: scan height

    //----------------------------------------------------------------
    //	RETURN
    //----------------------------------------------------------------
    DENTER_ARG("sprites updated %d |\n", ret);
    return ret;	//fail
}	//End public method: change_color | Color | Color |

/***************************************************************************/
//!	@brief public method
//!	clear | void |
/***************************************************************************/
//! @return int | >=0 Number of sprites changed | < 0 error |
//! @details
//!	\n Clear the screen by setting a solid color sprite to each element of the sprite frame buffer
/***************************************************************************/

int Screen::clear( void )
{
    DENTER();

    //----------------------------------------------------------------
    //	VARS
    //----------------------------------------------------------------

    //Fast counters
    uint8_t th, tw;
    //Temp sprite
    Frame_buffer_sprite sprite_tmp;
    //Number of sprites updated
    int num_sprites_updated = 0;
    int ret;

    //----------------------------------------------------------------
    //	BODY
    //----------------------------------------------------------------

        //Construct sprite.
    //Special sprite with full background color
    sprite_tmp.sprite_index		= Config::SPRITE_BLACK;
    //Set color. I don't care about background color
    sprite_tmp.background_color	= Color::BLACK;
    sprite_tmp.foreground_color	= Color::BLACK;
    sprite_tmp.f_update			= true;
    //For: each frame buffer row (height scan)
    for (th = 0;th < Screen::Config::FRAME_BUFFER_HEIGHT;th++)
    {
        //For: each frame buffer col (width scan)
        for (tw = 0;tw < Screen::Config::FRAME_BUFFER_WIDTH;tw++)
        {
            //Update the frame buffer with the new sprite if needed
            ret = this -> update_sprite( th, tw, sprite_tmp );
            //If: an error occurred
            if ((Config::PEDANTIC_CHECKS == true) && (ret < 0))
            {
                DRETURN_ARG("ERR: Failed to update sprite\n");
                return ret;
            }
            else
            {
                //Compute number of updated sprites	
                num_sprites_updated += ret;
            }
        } //End For: each frame buffer col (width scan)
    } //End For: each frame buffer row (height scan)

    //----------------------------------------------------------------
    //	RETURN
    //----------------------------------------------------------------
    DRETURN();
    return num_sprites_updated;
}	//End public method: clear | void |

/***************************************************************************/
//!	@brief public method
//!	clear | Color |
/***************************************************************************/
//!	@param color_tmp | Color | index of the solid sprite in the palette
//! @return int | >=0 Number of sprites changed | < 0 error |
//! @details
//!	\n Clear the screen by setting a solid color sprite to each element of the sprite frame buffer
/***************************************************************************/

int Screen::clear( Color color_tmp )
{
    DENTER_ARG("color: %d\n", (int)color_tmp);
    //----------------------------------------------------------------
    //	CHECK
    //----------------------------------------------------------------
    
    //If: palette index outside the palette
    if (color_tmp >= (Color)Screen::Config::PALETTE_SIZE)
    {
        DRETURN_ARG("ERR: palette index outside the palette\n");
        return -1;
    }

    //----------------------------------------------------------------
    //	VARS
    //----------------------------------------------------------------

    //Fast counters
    uint8_t th, tw;
    //Temp sprite
    Frame_buffer_sprite sprite_tmp;
    //Number of sprites updated
    int num_sprites_updated = 0;
    //Temp return
    int ret;

    //----------------------------------------------------------------
    //	BODY
    //----------------------------------------------------------------

        //Construct sprite.
    //Special sprite with full background color
    sprite_tmp.sprite_index		= Config::SPRITE_BACKGROUND;
    //Set color. I don't care about background color
    sprite_tmp.background_color	= color_tmp;
    sprite_tmp.foreground_color	= color_tmp;
    sprite_tmp.f_update			= true;
    //For: each frame buffer row (height scan)
    for (th = 0;th < Screen::Config::FRAME_BUFFER_HEIGHT;th++)
    {
        //For: each frame buffer col (width scan)
        for (tw = 0;tw < Screen::Config::FRAME_BUFFER_WIDTH;tw++)
        {
            //Update the frame buffer with the new sprite if needed
            ret = this -> update_sprite( th, tw, sprite_tmp );
            //If: an error occurred
            if ((Config::PEDANTIC_CHECKS == true) && (ret < 0))
            {
                DRETURN_ARG("ERR: Failed to update sprite\n");
                return ret;
            }
            else
            {
                //Compute number of updated sprites	
                num_sprites_updated += ret;
            }
        } //End For: each frame buffer col (width scan)
    } //End For: each frame buffer row (height scan)

    //----------------------------------------------------------------
    //	RETURN
    //----------------------------------------------------------------
    DRETURN();
    return num_sprites_updated;
}	//End public method: clear | Color |

/***************************************************************************/
//!	@brief public method
//!	print | int | int | char | Color | Color
/***************************************************************************/
//!	@param origin_h | int | height position of the sprite
//!	@param origin_w | int | width position of the sprite
//!	@param c | char | ascii char to be drawn
//!	@param background | Color | background color of ths sprite as index from the palette
//!	@param foreground | Color | foreground color of ths sprite as index from the palette
//! @return int | >=0 Number of sprites changed | < 0 error |
//!	@todo add special non drawable sprite
//! @details
//! \n Print a character on screen using user defined background and foreground colors
//! \n Mark the sprite for update if needs to be
/***************************************************************************/

int Screen::print( int origin_h, int origin_w, char c, Color background, Color foreground )
{
    DENTER_ARG("h: %5d, w: %5d, c: %d %d\n", origin_h, origin_w, (int)background, (int)foreground);
    //----------------------------------------------------------------
    //	CHECK
    //----------------------------------------------------------------

    //If: character is outside the ascii sprite table
    if ((origin_h < 0) || (origin_h >= Config::FRAME_BUFFER_HEIGHT) || (origin_w < 0) || (origin_w >= Config::FRAME_BUFFER_WIDTH))
    {
        DRETURN_ARG("ERR: out of the sprite table\n");
        return -1;    //FAIL
    }
    //If: colors are bad
    if ((background >= (Color)Config::PALETTE_SIZE) || (foreground >= (Color)Config::PALETTE_SIZE))
    {
        DRETURN_ARG("ERR: bad default colors | Back: %3d | Fore: %3d |\n", background, foreground );
        return -1;    //FAIL
    }
    //If: character is not stored inside the ascii sprite table
    if (is_valid_char( c ) == false)
    {
        DRETURN_ARG("ERR: character not on the char table\n");
        return -1;    //FAIL
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

    //Update the frame buffer with the new sprite if needed
    int ret = this -> update_sprite( origin_h, origin_w, sprite_tmp );
    //@debug
    show_frame_sprite(this -> g_frame_buffer[origin_h][origin_w]);

    //----------------------------------------------------------------
    //	RETURN
    //----------------------------------------------------------------
    DRETURN();
    return ret;
}	//End public method: print | int | int | char | Color | Color

/***************************************************************************/
//!	@brief public method
//!	print | int | int | char | Color
/***************************************************************************/
//!	@param origin_h | int | height position of the sprite
//!	@param origin_w | int | width position of the sprite
//!	@param c | char | ascii char to be drawn
//!	@param foreground | Color | foreground color of ths sprite as index from the palette
//! @return int | >=0 Number of sprites changed | < 0 error |
//! @details
//! \n Print a character on screen using default background color but a user defined foreground color
/***************************************************************************/

inline int Screen::print( int origin_h, int origin_w, char c, Color foreground )
{
    DENTER();
    //----------------------------------------------------------------
    //	BODY
    //----------------------------------------------------------------

    //Call the more generic print char method and return its result
    int num_changed_sprites = this -> print( origin_h, origin_w, c, this -> g_default_background_color, foreground );

    //----------------------------------------------------------------
    //	RETURN
    //----------------------------------------------------------------
    DRETURN();
    return num_changed_sprites;
}	//End public method: print | int | int | char | Color

/***************************************************************************/
//!	@brief public method
//!	print | int | int | char |
/***************************************************************************/
//!	@param origin_h | int | height position of the sprite
//!	@param origin_w | int | width position of the sprite
//!	@param c | char | ascii char to be drawn
//! @return int16_t | >=0 Number of sprites changed | < 0 error |
//! @details
//!	\n print a char inside the sprite frame buffer and mark it for update if needs to be
//! \n uses default background and foreground colors
/***************************************************************************/

inline int Screen::print( int origin_h, int origin_w, char c )
{
    DENTER();
    //----------------------------------------------------------------
    //	BODY
    //----------------------------------------------------------------

    //Call the more generic print char method and return its result
    int num_changed_sprites = this -> print( origin_h, origin_w, c, this -> g_default_background_color, this -> g_default_foreground_color );

    //----------------------------------------------------------------
    //	RETURN
    //----------------------------------------------------------------
    DRETURN();
    return num_changed_sprites;
}	//End public method: print | int | int | char |

/***************************************************************************/
//!	@brief public method
//!	print | int | int | const char * | Color | Color |
/***************************************************************************/
//!	@param origin_h | int | height position of the sprite
//!	@param origin_w | int | width position of the sprite
//!	@param str | const char * | string to be drawn. Must be null terminated
//!	@param background | Color | background color of ths sprite as index from the palette
//!	@param foreground | Color | foreground color of ths sprite as index from the palette
//! @return int16_t | >=0 Number of sprites changed | < 0 error |
//! @details
//! \n Print a string on screen using user defined background and foreground colors
//! \n Strings will not wrap around the screen
//! \n String must be \0 terminated
/***************************************************************************/

int Screen::print( int origin_h, int origin_w, const char *str, Color background, Color foreground )
{
    DENTER_ARG("h: %5d, w: %5d, c: %p %s\n", origin_h, origin_w, str, str);
    //----------------------------------------------------------------
    //	CHECK
    //----------------------------------------------------------------
    
    //If: first character is outside the ascii sprite table
    if ((origin_h < 0) || (origin_h >= Config::FRAME_BUFFER_HEIGHT) || (origin_w < 0) || (origin_w >= Config::FRAME_BUFFER_WIDTH))
    {
        DRETURN_ARG("ERR: out of the sprite table %5d %5d\n", origin_h, origin_w);
        return -1;	//FAIL
    }
    //If: colors are bad
    if ((background >= (Color)Config::PALETTE_SIZE) || (foreground >= (Color)Config::PALETTE_SIZE))
    {
        DRETURN_ARG("ERR: bad default colors | Back: %3d | Fore: %3d |\n", background, foreground );
        return -1;    //FAIL
    }
    //If: string is invalid
    if (str == nullptr)
    {
        DRETURN_ARG("ERR: null pointer string\n");
        return -1;	//FAIL
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

    int num_changed_sprites = 0;
    //Fast counters
    uint8_t t = 0;
    uint8_t tw = origin_w;
    int ret;
    //While: I'm allowed to print, I didn't exceed the string size, and I didn't exceed the screen position
    while ((str[t] != '\0') && (tw < Config::FRAME_BUFFER_WIDTH))
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
        else
        {
            //Update the frame buffer with the new sprite if needed
            ret = this -> update_sprite( origin_h, tw, sprite_tmp );
            //If: an error occurred
            if ((Config::PEDANTIC_CHECKS == true) && (ret < 0))
            {
                DRETURN_ARG("ERR: Failed to update sprite\n");
                return ret;
            }
            else
            {
                //Compute number of updated sprites	
                num_changed_sprites += ret;
                show_frame_sprite(this -> g_frame_buffer[origin_h][tw]);
            }
        }
        //Next character
        t++;
        tw++;
    }	//End While: I'm allowed to print, I didn't exceed the string size, and I didn't exceed the screen position

    //----------------------------------------------------------------
    //	RETURN
    //----------------------------------------------------------------
    DRETURN();
    return num_changed_sprites;
}	//End public method: print | int | int | const char * | Color | Color |

/***************************************************************************/
//!	@brief public method
//!	print | int | int | const char* | Color |
/***************************************************************************/
//!	@param origin_h | int | height position of the sprite
//!	@param origin_w | int | width position of the sprite
//!	@param str | const char * | string to be drawn. Must be null terminated
//!	@param foreground | Color | foreground color of ths sprite as index from the palette
//! @return int16_t | >=0 Number of sprites changed | < 0 error |
//! @details
//!	\n print a char inside the sprite frame buffer and mark it for update if needs to be
//! \n use default background
//! \n wrapper for more general print string function
/***************************************************************************/

inline int Screen::print( int origin_h, int origin_w, const char *str, Color foreground )
{
    DENTER();   //Trace enter
    //----------------------------------------------------------------
    //	BODY
    //----------------------------------------------------------------

    //more general print string function
    int num_changed_sprites = print( origin_h, origin_w, str, this -> g_default_background_color,  foreground );

    //----------------------------------------------------------------
    //	RETURN
    //----------------------------------------------------------------
    DRETURN();  //Trace enter
    return num_changed_sprites;
}	//End public method: print | int | int | const char* | Color |

/***************************************************************************/
//!	@brief public method
//!	print | int | int | const char* |
/***************************************************************************/
//!	@param origin_h | int | height position of the sprite
//!	@param origin_w | int | width position of the sprite
//!	@param str | const char * | string to be drawn. Must be null terminated
//! @return int16_t | >=0 Number of sprites changed | < 0 error |
//! @details
//!	\n print a char inside the sprite frame buffer and mark it for update if needs to be
//! \n uses default background and foreground colors
//! \n wrapper for more general print string function
/***************************************************************************/

inline int Screen::print( int origin_h, int origin_w, const char *str )
{
    DENTER();   //Trace enter
    //----------------------------------------------------------------
    //	BODY
    //----------------------------------------------------------------

    //more general print string function
    int num_changed_sprites = print( origin_h, origin_w, str, this -> g_default_background_color,  this -> g_default_foreground_color );

    //----------------------------------------------------------------
    //	RETURN
    //----------------------------------------------------------------
    DRETURN();  //Trace enter
    return num_changed_sprites;
}	//End public method: print | int | int | char |

/***************************************************************************/
//!	@brief public method
//!	print | int | int | int | Color | Color |
/***************************************************************************/
//!	@param origin_h | int | height position of the first character
//!	@param origin_w | int | width position of the first character
//!	@param num | int | number to be shown on screen
//!	@param background | Color | background color of ths sprite as index from the palette
//!	@param foreground | Color | foreground color of ths sprite as index from the palette
//! @return int16_t | >=0 Number of sprites changed | < 0 error |
//! @details
//!	\n Print a number on screen. Format configuration is handled by format
//! \n With left asjust, the number is print starting from left an origin is the position of the most significant number
//! \n With right adjust, the origin is the position of the least significant number        
//! \n LEFT        78901       12.71m
//! \n RIGHT   78901      12.71m 
//! \n ORIGIN      ^           ^
//! \n The format also specifies full number or enginnering format
/***************************************************************************/

int Screen::print( int origin_h, int origin_w, int num, Color background, Color foreground )
{
    DENTER_ARG("NUM: %d\n", num);   //Trace enter

    //----------------------------------------------------------------
    //	CHECK
    //----------------------------------------------------------------
    // Quit if the number is fully outside the screen for sure
    // Avoid wasting CPU time for the conversion

    //If: colors are bad
    if ((background >= (Color)Config::PALETTE_SIZE) || (foreground >= (Color)Config::PALETTE_SIZE))
    {
        DRETURN_ARG("ERR: bad default colors | Back: %3d | Fore: %3d |\n", background, foreground );
        return -1;    //FAIL
    }
    //If: height fully outside screen
    if ((origin_h < 0) || (origin_h >= Config::FRAME_BUFFER_HEIGHT))
    {
        DRETURN_ARG("ERR: Height out of range: %d\n", origin_h);
        return -1;
    }
    //Fetch the format of the number locally
    Format_number format_tmp = this -> g_format_number;
    //Compute first sprite and last sprite that can be occupied by the number
    uint8_t start_w = ((format_tmp.align == Format_align::ADJ_LEFT)?(origin_w):(origin_w -format_tmp.size +1));
    uint8_t stop_w = ((format_tmp.align == Format_align::ADJ_LEFT)?(origin_w +format_tmp.size -1):(origin_w));
    DPRINT("start_w: %3d | stop_w: %3d\n", start_w, stop_w );
    //If: width fully outside screen in right adjust
    if ( (stop_w < 0) || (start_w >= Config::FRAME_BUFFER_WIDTH) )
    {
        DRETURN_ARG("ERR: Width out of range\n");
        return -1;
    }
    //Clip the start and stop position to be inside the screen
    start_w = ((start_w < 0)?(0):(start_w));
    stop_w = ((stop_w >= Config::FRAME_BUFFER_WIDTH)?(Config::FRAME_BUFFER_WIDTH -1):(stop_w));

    //----------------------------------------------------------------
    //	NUM -> STRING
    //----------------------------------------------------------------
    //	Execute the conversion
    
    int num_changed_sprites = 0;
    //Prepare a string to hold the number
    char str[User::String::Config::STRING_SIZE_S32];
    //Temp return
    uint8_t num_digit;
    //Fast counter
    uint8_t t;
    //if: number format is extended
    if (format_tmp.format == Format_format::NUM)
    {
        //Try to convert the number into a string
        num_digit = User::String::num_to_str( (int32_t)num, User::String::Config::STRING_SIZE_S32, str );
        //If: fail
        if (num_digit == 0)
        {
            DRETURN_ARG("ERR: Failed to convert NUM number: %d", num);
            return -1;
        }
    }
    //If: number format is enginnering
    else //if (format_tmp.format = Format_format::ENG)
    {
        //Try to convert the number into a string
        num_digit = User::String::num_to_eng( (int32_t)num, this -> g_format_number.eng_exp, User::String::Config::STRING_SIZE_S32, str );
        //If: fail
        if (num_digit == 0)
        {
            DRETURN_ARG("ERR: Failed to convert ENG number: %d", num);
            return -1;
        }
    }
    
    //----------------------------------------------------------------
    //	NUMBER PARTIALLY OUT OF SCREEN
    //----------------------------------------------------------------
    //	Having just a piece of a number on screen is dangerous
    //	partially blanked digits need to invalidate the full number and show '#'
    //	Condition for partially out
    //			START			STOP
    //	LEFT	OW<0			OW+NUM-1>=W
    //	RIGHT	OW-NUM+1<0		OW>=W

    int ret;
    //Temporary sprite
    Frame_buffer_sprite sprite_tmp;
    //Build temporary sprite
    sprite_tmp.background_color = background;
    sprite_tmp.foreground_color = foreground;
    sprite_tmp.f_update = true;
    //If: number is too big
    if (num_digit > format_tmp.size)
    {
        //Print the invalid number character to show the user the number has no meaning
        sprite_tmp.sprite_index = '#';
        //For: each number character that can be displayed
        for (t = start_w;t <= stop_w;t++)
        {
            //Update the frame buffer with the new sprite if needed
            ret = this -> update_sprite( origin_h, t, sprite_tmp );
            //If: an error occurred
            if ((Config::PEDANTIC_CHECKS == true) && (ret < 0))
            {
                DRETURN_ARG("ERR: Failed to update sprite\n");
                return ret;
            }
            else
            {
                //Compute number of updated sprites	
                num_changed_sprites += ret;
            }
        }
        DRETURN_ARG("ERR: Width partially out of range | LEFT | start %d | stop %d |\n", start_w, stop_w );
        return num_changed_sprites;
    }
    //If: number partially outside screen in left adjust
    else if ( (format_tmp.align == Format_align::ADJ_LEFT) && ((origin_w < 0) || ((origin_w +num_digit -1) >= Config::FRAME_BUFFER_WIDTH)) )
    {
        //Print the invalid number character to show the user the number has no meaning
        sprite_tmp.sprite_index = '#';
        //For: each number character that can be displayed
        for (t = start_w;t <= stop_w;t++)
        {
            //Update the frame buffer with the new sprite if needed
            ret = this -> update_sprite( origin_h, t, sprite_tmp );
            //If: an error occurred
            if ((Config::PEDANTIC_CHECKS == true) && (ret < 0))
            {
                DRETURN_ARG("ERR: Failed to update sprite\n");
                return ret;
            }
            else
            {
                //Compute number of updated sprites	
                num_changed_sprites += ret;
            }
        }
        DRETURN_ARG("ERR: Width partially out of range | LEFT | start %d | stop %d |\n", start_w, stop_w );
        return num_changed_sprites;
    }
    //If: number partially outside screen in right adjust
    else if ( (format_tmp.align == Format_align::ADJ_RIGHT) && ((origin_w -num_digit +1 < 0) || (origin_w >= Config::FRAME_BUFFER_WIDTH)) )
    {
        //Print the invalid number character to show the user the number has no meaning
        sprite_tmp.sprite_index = '#';
        //For: each number character that can be displayed
        for (t = start_w;t <= stop_w;t++)
        {
            //Update the frame buffer with the new sprite if needed
            ret = this -> update_sprite( origin_h, t, sprite_tmp );
            //If: an error occurred
            if ((Config::PEDANTIC_CHECKS == true) && (ret < 0))
            {
                DRETURN_ARG("ERR: Failed to update sprite\n");
                return ret;
            }
            else
            {
                //Compute number of updated sprites	
                num_changed_sprites += ret;
            }
        }
        DRETURN_ARG("ERR: Width partially out of range | RIGHT | start %d | stop %d |\n", start_w, stop_w );
        return num_changed_sprites;
    }
    
    //----------------------------------------------------------------
    //	NUMBER FULLY INSIDE SCREEN
    //----------------------------------------------------------------
    //	Number can be shown fully on screen, with at most spaces left out

    //If: left alignment
    if (format_tmp.align == Format_align::ADJ_LEFT)
    {
            //Print the number
        //For: every number digit
        for (t = 0;t < num_digit; t++)
        {
            //Digit -> Sprite
            sprite_tmp.sprite_index = str[t];
            //Update the frame buffer with the new sprite if needed
            ret = this -> update_sprite( origin_h, t, sprite_tmp );
            //If: an error occurred
            if ((Config::PEDANTIC_CHECKS == true) && (ret < 0))
            {
                DRETURN_ARG("ERR: Failed to update sprite\n");
                return ret;
            }
            else
            {
                //Compute number of updated sprites	
                num_changed_sprites += ret;
            }
        }	//End For: every digit
            //Print spaces to clear the previously printed character
        //space sprite
        sprite_tmp.sprite_index = ' ';
        //For: every number digit
        for (;t <= stop_w; t++)
        {
            //Update the frame buffer with the new sprite if needed
            ret = this -> update_sprite( origin_h, t, sprite_tmp );
            //If: an error occurred
            if ((Config::PEDANTIC_CHECKS == true) && (ret < 0))
            {
                DRETURN_ARG("ERR: Failed to update sprite\n");
                return ret;
            }
            else
            {
                //Compute number of updated sprites	
                num_changed_sprites += ret;
            }
        }
    }   //End If: left alignment
    //If: right alignment
    else //if (format_tmp.align == Format_align::ADJ_RIGHT)
    {
        //Print the correct number of spaces
        //space sprite
        sprite_tmp.sprite_index = ' ';
        //For: every number digit
        for (t = start_w;t < stop_w -num_digit +1; t++)
        {
            //Update the frame buffer with the new sprite if needed
            ret = this -> update_sprite( origin_h, t, sprite_tmp );
            //If: an error occurred
            if ((Config::PEDANTIC_CHECKS == true) && (ret < 0))
            {
                DRETURN_ARG("ERR: Failed to update sprite\n");
                return ret;
            }
            else
            {
                //Compute number of updated sprites	
                num_changed_sprites += ret;
            }
        }
        //Reset start position
        start_w = t;
        //For: every number digit
        for (t = 0;t < num_digit; t++)
        {
            //Digit -> Sprite
            sprite_tmp.sprite_index = str[t];
            //Update the frame buffer with the new sprite if needed
            ret = this -> update_sprite( origin_h, start_w +t, sprite_tmp );
            //If: an error occurred
            if ((Config::PEDANTIC_CHECKS == true) && (ret < 0))
            {
                DRETURN_ARG("ERR: Failed to update sprite\n");
                return ret;
            }
            else
            {
                //Compute number of updated sprites	
                num_changed_sprites += ret;
            }
        }	//End For: every digit	
    }   //End If: right alignment

    //----------------------------------------------------------------
    //	RETURN
    //----------------------------------------------------------------
    DRETURN();
    return num_changed_sprites;	//OK
}	//End public method: print | int | int | int | Color | Color |

/***************************************************************************/
//!	@brief public method
//!	print | int | int | int | Color | 
/***************************************************************************/
//!	@param origin_h | int | height position of the first character
//!	@param origin_w | int | width position of the first character
//!	@param num | int | number to be shown on screen
//!	@param foreground | Color | foreground color of ths sprite as index from the palette
//! @return int16_t | >=0 Number of sprites changed | < 0 error |
//! @details
//!	\n Print a number on screen. Overload with default background.
/***************************************************************************/

inline int Screen::print( int origin_h, int origin_w, int num, Color foreground )
{
    DENTER_ARG("Num: %d\n", num );
    //----------------------------------------------------------------
    //	RETURN
    //----------------------------------------------------------------
    //
    int num_changed_sprites = print( origin_h, origin_w, num, this -> g_default_background_color, foreground );
    DRETURN_ARG("Success: %d\n", num_changed_sprites );
    return num_changed_sprites;
}   //End public method: print | int | int | int |

/***************************************************************************/
//!	@brief public method
//!	print | int | int | int |
/***************************************************************************/
//!	@param origin_h | int | height position of the first character
//!	@param origin_w | int | width position of the first character
//!	@param num | int | number to be shown on screen
//! @return int16_t | >=0 Number of sprites changed | < 0 error |
//! @details
//!	\n Print a number on screen. Overload with default background and foreground
/***************************************************************************/

inline int Screen::print( int origin_h, int origin_w, int num )
{
    DENTER_ARG("Num: %d\n", num );
    //----------------------------------------------------------------
    //	RETURN
    //----------------------------------------------------------------
    //
    int ret = print( origin_h, origin_w, num, this -> g_default_background_color, this -> g_default_foreground_color );
    DRETURN_ARG("Success: %d\n", f_ret );
    return ret;
}   //End public method: print | int | int | int |

/***************************************************************************/
//!	@brief public method
//!	paint | int | int | Color |
/***************************************************************************/
//! @return int16_t | >=0 Number of sprites changed | < 0 error |
//! @details
//!	\n Draw a solid color on the screen
//! \n There are ery few graphics sprite as they ar meant for progress bars and little more
/***************************************************************************/

int Screen::paint( int origin_h, int origin_w, Color color )
{
    DENTER_ARG("H: %d, W: %d, color index: %d\n", origin_h, origin_w, (int)color );
    //----------------------------------------------------------------
    //	CHECK
    //----------------------------------------------------------------

    //If: colors are bad
    if (color >= (Color)Config::PALETTE_SIZE)
    {
        DRETURN_ARG("ERR: bad default colors | Color: %3d |\n", color );
        return -1;    //FAIL
    }
    //If: height fully outside screen
    if ((origin_h < 0) || (origin_h >= Config::FRAME_BUFFER_HEIGHT))
    {
        DRETURN_ARG("ERR: Height out of range: %d\n", origin_h);
        return -1;
    }
    //If: width fully outside screen
    if ((origin_w < 0) || (origin_w >= Config::FRAME_BUFFER_WIDTH))
    {
        DRETURN_ARG("ERR: Height out of range: %d\n", origin_w);
        return -1;
    }

    //----------------------------------------------------------------
    //	Construct Sprite
    //----------------------------------------------------------------

    //Temporary sprite
    Frame_buffer_sprite sprite_tmp;
    //Build temporary sprite
    sprite_tmp.sprite_index = Config::SPRITE_BACKGROUND;
    sprite_tmp.background_color = color;
    sprite_tmp.foreground_color = color;
    sprite_tmp.f_update = true;

    //----------------------------------------------------------------
    //	Update Frame Buffer
    //----------------------------------------------------------------

    //Update the frame buffer with the new sprite if needed
    int ret = this -> update_sprite( origin_h, origin_w, sprite_tmp );
    
    //----------------------------------------------------------------
    //	RETURN
    //----------------------------------------------------------------
    DRETURN();
    return ret;	//No sprites have been drawn
}	//End public method: paint | int | int | Color |

/***************************************************************************/
//!	@brief public method
//!	print_err | int | int |
/***************************************************************************/
//!	@param origin_h | int | height position of the sprite
//!	@param origin_w | int | width position of the sprite
//! @return int16_t | >=0 Number of sprites changed | < 0 error |
//! @details
//!	\n print a char inside the sprite frame buffer and mark it for update if needs to be
//! \n use default background
//! \n wrapper for more general print string function
/***************************************************************************/

int Screen::print_err( int origin_h, int origin_w )
{
    DENTER();   //Trace enter
    //----------------------------------------------------------------
    //	BODY
    //----------------------------------------------------------------

    //more general print string function
    int num_changed_sprites;
    //If: OK
    if (this -> g_error_code == 0)
    {
        num_changed_sprites = print( origin_h, origin_w, "OK", Color::GREEN );
    }
    //If: ERR
    else
    {
        num_changed_sprites = print( origin_h, origin_w, "ERR", Color::RED );
        this -> set_format(2, Format_align::ADJ_LEFT, Format_format::NUM );
        num_changed_sprites += print( origin_h, origin_w +3, (int)this -> g_error_code, Color::RED );
    }
    
    //----------------------------------------------------------------
    //	RETURN
    //----------------------------------------------------------------
    DRETURN();  //Trace enter
    return num_changed_sprites;
}	//End public method: print_err | int | int |

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
//!	\n Initialize the class vars
/***************************************************************************/

bool Screen::init_class_vars( void )
{
    //----------------------------------------------------------------
    //	VARS
    //----------------------------------------------------------------

    //----------------------------------------------------------------
    //	BODY
    //----------------------------------------------------------------

    //Initialize error code
    this -> g_error_code = Screen::Error::OK;
    //Initialize default number format
    this -> set_format( Screen::Config::FRAME_BUFFER_WIDTH, Format_align::ADJ_LEFT, Format_format::NUM, 0 );

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
//!	\n Initialize the sprite frame buffer
/***************************************************************************/

bool Screen::init_frame_buffer()
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
    //All sprites require update at the initialization
    this -> g_pending_cnt = Config::FRAME_BUFFER_SIZE;

    //----------------------------------------------------------------
    //	RETURN
    //----------------------------------------------------------------
    DRETURN();
    return false;	//OK
}	//End private init: init_frame_buffer | void

/***************************************************************************/
//!	@brief private init
//!	init_default_colors | void |
/***************************************************************************/
//! @return void
//! @details
//!	\n Initialize the default background and foreground colors
/***************************************************************************/

inline bool Screen::init_default_colors( void )
{
    DENTER();
    //----------------------------------------------------------------
    //	BODY
    //----------------------------------------------------------------

    //Initialize default colors
    this -> g_default_background_color = Color::BLACK;
    this -> g_default_foreground_color = Color::WHITE;

    //----------------------------------------------------------------
    //	RETURN
    //----------------------------------------------------------------
    DRETURN();
    return false;	//OK
}	//End private init: init_default_colors | void |

/***************************************************************************/
//!	@brief private init
//!	init_palette | void
/***************************************************************************/
//! @return bool | false = OK | true = ERR
//! @details
//!	\n initialize palette to default values
/***************************************************************************/

bool Screen::init_palette( void )
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
    this -> g_palette[Screen::Color::LRED]      = Longan_nano::Display::color( 0xFF, 0x55, 0x55 );
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
//!	\n Initialize the update FSM machine
/***************************************************************************/

bool Screen::init_fsm( void )
{
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
    **	PRIVATE SETTER
    **********************************************************************************************************************************************************
    *********************************************************************************************************************************************************/

/***************************************************************************/
//!	@brief private setter
//!	report_error | Error |
/***************************************************************************/
//! @param error_code | Error | error code of the Screen Class
//! @return no return
//!	@details
//! Method
/***************************************************************************/

void Screen::report_error( Error error_code )
{
    DENTER_ARG("ERR%d\n", (int)error_code ); //Trace Enter
    ///--------------------------------------------------------------------------
    ///	CHECK
    ///--------------------------------------------------------------------------

    if ((Config::PEDANTIC_CHECKS == true) && (error_code >= Screen::Error::NUM_ERROR_CODES))
    {
        this -> g_error_code = Screen::Error::BAD_ERROR_CODE;
    }
    else
    {
        this -> g_error_code = error_code;
    }
    
    ///--------------------------------------------------------------------------
    ///	RETURN
    ///--------------------------------------------------------------------------
    DRETURN(); //Trace Return
    return;
}	//end private setter: report_error | Error |

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

inline bool Screen::is_valid_char( char c )
{
    //----------------------------------------------------------------
    //	RETURN
    //----------------------------------------------------------------

    return ((c >= Config::ASCII_START) && (c <= Config::ASCII_STOP));
}	//End private tester: is_valid_char | char |

/***************************************************************************/
//!	@brief private tester
//!	is_using_background | uint8_t  |
/***************************************************************************/
//! @return bool | false = sprite do not use background color | true = sprite use background color
//! @details
//!	return true if the sprite make use of the background palette color
/***************************************************************************/

inline bool Screen::is_using_background( uint8_t sprite )
{
    //----------------------------------------------------------------
    //	BODY
    //----------------------------------------------------------------
    //If: special sprite
    if (sprite < Config::NUM_SPECIAL_SPRITES)
    {
        //If: special sprite that use the background
        if (sprite == Config::SPRITE_BACKGROUND)
        {
            //Use background
            return true;
        }
        else
        {
            //Do not use the background
            return false;
        }
    }
    //If: sprite is an ascii character
    else if (this -> is_valid_char(sprite) == true)
    {
        //An ascii character use the background
        return true;
    }

    //----------------------------------------------------------------
    //	RETURN
    //----------------------------------------------------------------
    
    //Non drawable sprite
    return false;
}	//End private tester: is_using_background | uint8_t  |

/***************************************************************************/
//!	@brief private tester
//!	is_using_foreground | uint8_t  |
/***************************************************************************/
//! @return bool | false = sprite do not use foreground color | true = sprite use foreground color
//! @details
//!	return true if the sprite make use of the foreground palette color
/***************************************************************************/

inline bool Screen::is_using_foreground( uint8_t sprite )
{
    //----------------------------------------------------------------
    //	BODY
    //----------------------------------------------------------------
    //If: special sprite
    if (sprite < Config::NUM_SPECIAL_SPRITES)
    {
        //If: special sprite that use the color
        if (sprite == Config::SPRITE_FOREGROUND)
        {
            //Using color
            return true;
        }
        else
        {
            //Not Using color
            return false;
        }
    }
    //If: sprite is an ascii character
    else if (this -> is_valid_char(sprite) == true)
    {
        //An ascii character use the color
        return true;
    }

    //----------------------------------------------------------------
    //	RETURN
    //----------------------------------------------------------------
    
    //Non drawable sprite
    return false;
}	//End private tester: is_using_foreground | uint8_t  |

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

bool Screen::is_same_sprite( Frame_buffer_sprite sprite_a, Frame_buffer_sprite sprite_b )
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
//!	register_sprite | uint16_t | uint16_t |
/***************************************************************************/
//! @param index_h | uint16_t | index of the sprite in the frame buffer to build pixel from
//! @param index_w | uint16_t | index of the sprite in the frame buffer to build pixel from
//! @return int8_t | -1 error | >= 0 sprites registered for draw
//! @details
//!	\n Register a sprite for draw in the display driver if possible
//! \n Sprite can be xomplex color map or solid color
/***************************************************************************/

int8_t Screen::register_sprite( uint16_t index_h, uint16_t index_w )
{
    DENTER_ARG("index_h : %5d | index_w %5d\n", index_h, index_w);
    //----------------------------------------------------------------
    //	VARS
    //----------------------------------------------------------------

    //Temp sprite data
    uint16_t foreground_color, background_color;
    //Temp color
    uint16_t color;
    //false = complex color map | true = solid color
    bool f_solid_color;

    //----------------------------------------------------------------
    //	INIT
    //----------------------------------------------------------------

    //If: bad parameters
    if ((Config::PEDANTIC_CHECKS == true) && ((index_w >= Config::FRAME_BUFFER_WIDTH) || (index_h >= Config::FRAME_BUFFER_HEIGHT)) )
    {
        DRETURN_ARG("ERR: bad parameters\n");
        return -1; //FAIL
    }

    //----------------------------------------------------------------
    //	DECODE SPRITE
    //----------------------------------------------------------------

    //Fetch a frame sprite
    Frame_buffer_sprite sprite_tmp = g_frame_buffer[index_h][index_w];
    show_frame_sprite( sprite_tmp );
    //Decode background and foreground colors
    background_color = g_palette[ sprite_tmp.background_color ];
    foreground_color = g_palette[ sprite_tmp.foreground_color ];
    DPRINT("sprite: %d | background color: %6x | foreground_color: %6x |\n", sprite_tmp.sprite_index, background_color, foreground_color );
    //Pointer to sprite data
    const uint8_t *sprite_ptr = nullptr;

    //If: special sprite
    if (sprite_tmp.sprite_index < Config::NUM_SPECIAL_SPRITES)
    {
        //If: Solid black
        if (sprite_tmp.sprite_index == Config::SPRITE_BLACK)
        {
            f_solid_color = true;
            color = Display::color(0x00,0x00,0x00);
        }
        //If: Solid white
        else if (sprite_tmp.sprite_index == Config::SPRITE_WHITE)
        {
            f_solid_color = true;
            color = Display::color(0xFF,0xFF,0xFF);
        }
        //If: Solid background
        else if (sprite_tmp.sprite_index == Config::SPRITE_BACKGROUND)
        {
            f_solid_color = true;
            color = background_color;
        }
        //If: Solid foreground
        else if (sprite_tmp.sprite_index == Config::SPRITE_FOREGROUND)
        {
            f_solid_color = true;
            color = foreground_color;
        }
        //If: Transparent sprite
        else if (sprite_tmp.sprite_index == Config::SPRITE_TRANSPARENT)
        {
            DRETURN_ARG("Transparent Sprite\n");
            return 0;
        }
        //If: unhandled special sprite
        else
        {
            //Signal the error
            this -> report_error( Error::REGISTER_SPRITE_FAIL );
            DRETURN_ARG("ERR%d: unhandled special sprit\n", this -> get_error() );
            return -1;
        }
    }   //End If: special sprite
    //If: Handled Ascii Character in the character table
    else if (this -> is_valid_char( sprite_tmp.sprite_index ) == true)
    {
        //If: background and foreground are different
        if (background_color != foreground_color)
        {
            //Full pixel color map
            f_solid_color = false;
            //Point to the first byte of the ascii sprite
            sprite_ptr = &g_ascii_sprites[ (sprite_tmp.sprite_index -Config::ASCII_START) *Config::SPRITE_HEIGHT ];
        }
        //If: background and foreground are the same
        else //if (background_color == foreground_color)
        {
            //Draw a solid color sprite. Don't bother with computing a redundant pixel color map
            f_solid_color = true;
            color = background_color;
        }
    }
    //If: Unhandled sprite
    else
    {
        //Signal the error
        this -> report_error( Error::REGISTER_SPRITE_FAIL );
        DRETURN_ARG("ERR%d: unhandled special sprit\n", this -> get_error() );
        return -1;
    }

    //----------------------------------------------------------------
    //	BUILD PIXEL MAP
    //----------------------------------------------------------------

    //Temp return
    int ret;
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
        //Register the sprite for draw in the Display driver
        ret = this -> Display::register_sprite( index_h *Config::SPRITE_HEIGHT, index_w *Config::SPRITE_WIDTH, Config::SPRITE_HEIGHT, Config::SPRITE_WIDTH, g_pixel_data );
        //If: failed to register. the register sprite in future can be smaller than the sprite size if trying to register a sprite partially out of screen
        if (ret <= 0)
        {
            //Signal the error
            this -> report_error( Error::REGISTER_SPRITE_FAIL );
            //Failed to register sprite
            ret = -1;
        }
        //If: success
        else
        {
            //One sprite has been drawn
            ret = 1;
        }
    }   //End If: sprite is a complex color map
    //If: sprite is a solid color
    else //if (f_solid_color == true)
    {
        //Register the sprite for draw in the Display driver
        ret = this -> Display::register_sprite( index_h *Config::SPRITE_HEIGHT, index_w *Config::SPRITE_WIDTH, Config::SPRITE_HEIGHT, Config::SPRITE_WIDTH, color );
        //If: failed to register. the register sprite in future can be smaller than the sprite size if trying to register a sprite partially out of screen
        if (ret <= 0)
        {
            //Signal the error
            this -> report_error( Error::REGISTER_SPRITE_FAIL );
            //Failed to register sprite
            ret = -1;
        }
        //If: success
        else
        {
            //One sprite has been drawn
            ret = 1;
        }
    }	//End If: sprite is a solid color

    //DEBUG
    show_pixels();

    //----------------------------------------------------------------
    //	RETURN
    //----------------------------------------------------------------
    DRETURN();
    return ret;
}	//End private method: register_sprite | uint16_t | uint16_t |

/***************************************************************************/
//!	@brief private tester
//!	update_sprite | uint16_t | uint16_t | Frame_buffer_sprite |
/***************************************************************************/
//! @return ont | <0 = error coccurred | 0 = frame buffer wasn't updated | 1 = frame buffer was updated
//! @details
//!	Update a sprite in the frame buffer and mark it for update if required
//!	Increase the workload counter if applicable
//!	If workload counter is zero, set the scan to the current sprite to quicken the seek
/***************************************************************************/

int8_t Screen::update_sprite( uint16_t index_h, uint16_t index_w, Frame_buffer_sprite new_sprite )
{
    DENTER_ARG("H: %d | W: %d |\n", index_h, index_w );
    //----------------------------------------------------------------
    //	CHECK
    //----------------------------------------------------------------

    //If: invalid coordinates
    if ((Config::PEDANTIC_CHECKS == true) && ((index_h >= Config::FRAME_BUFFER_HEIGHT) || (index_w >= Config::FRAME_BUFFER_WIDTH)) )
    {
        DRETURN_ARG("ERR: bad index H: %d | W: %d |\n", index_h, index_w);
        return -1;
    }

    //----------------------------------------------------------------
    //	BODY
    //----------------------------------------------------------------

    int8_t num_updated_sprites;
    //Fetch sprite
    Frame_buffer_sprite old_sprite = this -> g_frame_buffer[index_h][index_w];
    //If: the sprites are the same
    if (this -> is_same_sprite(old_sprite, new_sprite) == true)
    {
        //Do nothing
        num_updated_sprites = 0;
    }
    //If: the sprites are not the same
    else //if (old_sprite.f_update == true)
    {
        //If: old sprite was not marked for update
        if (old_sprite.f_update == false)
        {
            //If: the library workload is full
            if ((Config::PEDANTIC_CHECKS == true) && (this -> g_pending_cnt >= Config::FRAME_BUFFER_SIZE))
            {
                this -> report_error( Screen::Error::PENDING_OVERFLOW );
            }
            //If: the screen class was IDLE before this call
            else if (this -> g_pending_cnt == 0)
            {
                //Set the scan to this sprite so that the seek is quick
                this -> g_status.scan_h = index_h;
                this -> g_status.scan_w = index_w;
                //Increase workload of the Screen class
                this -> g_pending_cnt = 1;
            }
            //If: Screen class is already busy
            else
            {
                //Increase workload of the Screen class
                this -> g_pending_cnt++;
            }
        }
    
        //Mark for update
        new_sprite.f_update = true;
        //Update the sprite
        this -> g_frame_buffer[index_h][index_w] = new_sprite;
        //A sprite was updated
        num_updated_sprites = 1;
    }

    //----------------------------------------------------------------
    //	RETURN
    //----------------------------------------------------------------
    DRETURN();
    return num_updated_sprites;
}	//End private method: update_sprite | uint16_t | uint16_t | Frame_buffer_sprite |

/**********************************************************************************
**	NAMESPACE
**********************************************************************************/

} //End Namespace: Longan_nano

#else
    #warning "Multiple inclusion of hader file"
#endif
