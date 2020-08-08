/**********************************************************************************
BSD 3-Clause License

Original Copyright (c) 2019-2020, Samuli Laine
Modified Copyright (c) 2020, Orso Eric
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

#ifndef ST7735S_W160_H80_C16_HPP_
    #define ST7735S_W160_H80_C16_HPP_

/**********************************************************************************
**	GLOBAL INCLUDES
**********************************************************************************/

//Standard bit size types 
#include <stdint.h>
//Longan Nano HAL
#include <gd32vf103.h>

#ifndef LONGAN_NANO_CHRONO_H_
    //Longan Nano Chrono functions
    #include "longan_nano_chrono.hpp"
#endif

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
//! @class 		Display
/************************************************************************************/
//!	@author		Orso Eric
//! @version	2020-08-08
//! @brief		Longan Nano Display Driver
//! @copyright	BSD 3-Clause License (c) 2019-2020, Samuli Laine
//! @copyright	BSD 3-Clause License (c) 2020, Orso Eric
//! @details
//!	\n Driver to use the embedded 160x80 0.96' LCD on the longan nano
//! \n SPI0 interface with physical screen
//!	\n DMA0: executes a number of SPI0 transfers in background
//!	\n Hardware display has a ST7735S controller
//!	\n The driver is meant to accelerate sprite based drawing
//!	\n Sprite buffer and draw are going to be handled by an higher level class
//!	\n 	2020-07-07
//!	\n Ported original LCD example to class. Basic operation. DMA doesn't work
//!	\n 	2020-07-08
//!	\n Skeletron for sprites
//!	\n 	2020-07-10
//!	\n Change architecture. S7735S_W160_H80_C16.hpp. Common name drive class: Display
//!	\n With another display controller, Screen class and Display class interface remains the same
//!	\n S7735S_W160_H80_C16.hpp is the specific lbrary for the physical display
//!	\n Clean up, partition in a more intelligent way calls. Test project for driver only without sprites
//!	\n 	2020-07-15
//!	\n Tested draw with HAL builtin
//!	\n 	2020-07-21
//!	\n Found working DMA example from Samuli Laine https://github.com/slmisc/gd32v-lcd
//!	\n 	2020-07-22
//!	\n Transplant working driver function
//!	\n Refactor function to use configuration and fit class form
//!	\n set_sprite and update_sprite provide a non blocking draw method that hide the wait time
//!	\n application can call update_sprite and immediately continue to do work while SPI and DMA are busy
//!	\n update_sprite will immediately return without doing work if SPI or DMA are busy or if there is nothing to draw
//!	\n 	2020-07-23
//!	\n Clean up example
//!	\n Prune excess methods from display class. Leave only the draw sprite primitives. Application is to be given control on resource utilization of the display
//!	\n Refactor all calls to use configuration enum for the hardware peripherals
//!	\n 	2020-07-24
//!	\n Fixed color conversion function
//!	\n      2020-08-06
//!	\n  Massive update of style
//!	\n  Refactor functions, especially initialization
//!	\n  Full support for USE_DMA=false. screen updates much slower at the same CPU use
//! \n      2020-08-07
//! \n  Bugfix: Solid color draw was bugged
/************************************************************************************/

class Display
{
    //Visible to all
    public:
        /*********************************************************************************************************************************************************
        **********************************************************************************************************************************************************
        **	CONSTRUCTORS
        **********************************************************************************************************************************************************
        *********************************************************************************************************************************************************/

        //Empty Constructor
        Display( void );
        
        /*********************************************************************************************************************************************************
        **********************************************************************************************************************************************************
        **	DESTRUCTORS
        **********************************************************************************************************************************************************
        *********************************************************************************************************************************************************/

        //Empty Destructor
        ~Display( void );

        /*********************************************************************************************************************************************************
        **********************************************************************************************************************************************************
        **	PUBLIC INIT
        **********************************************************************************************************************************************************
        *********************************************************************************************************************************************************/

        //Initialize the longan nano display and related peripherals
        bool init( void );

        /*********************************************************************************************************************************************************
        **********************************************************************************************************************************************************
        **	PUBLIC METHODS
        **********************************************************************************************************************************************************
        *********************************************************************************************************************************************************/

        //convert from 24b 8R8G8B space to 16bit 5R6G5B space
        static uint16_t color( uint8_t r, uint8_t g, uint8_t b );
        //Register a sprite for the driver to draw. Complex pixel map.
        int register_sprite( int origin_h, int origin_w, int size_h, int size_w, uint16_t* sprite_ptr );
        //Register a sprite for the driver to draw. Solid color.
        int register_sprite( int origin_h, int origin_w, int size_h, int size_w, uint16_t sprite_color );
        //Core method. FSM that physically updates the screen. Return: false = IDLE | true = BUSY
        bool update_sprite( void );
        //Draw a sprite. Complex color map. Blocking Method.
        int draw_sprite( int origin_h, int origin_w, int size_h, int size_w, uint16_t* sprite_ptr );
        //Draw a sprite. Solid color. Blocking Method.
        int draw_sprite( int origin_h, int origin_w, int size_h, int size_w, uint16_t sprite_color );
        //Clear the screen to black. Blocking method.
        int clear( void );
        //Clear the screen to a given color. Blocking method.
        int clear( uint16_t color );
    
    protected:
        /*********************************************************************************************************************************************************
        **********************************************************************************************************************************************************
        **	PROTECTED ENUM
        **********************************************************************************************************************************************************
        *********************************************************************************************************************************************************/
        
        //! @brief Configuration parameters of the LCD Display class
        typedef enum _Config
        {
            //Screen physical configuration
            WIDTH				= 160,				//WIdth of the LCD display
            HEIGHT				= 80,				//Height of the LCD display
            PIXEL_COUNT			= WIDTH *HEIGHT,	//Number of pixels
            COLOR_DEPTH			= 16,				//Color depth. Screen allows 12, 16 and 18
            ROW_ADDRESS_OFFSET	= 1,				//Offset to be applied to the row address (physical pixels do not begin in 0,0)
            COL_ADDRESS_OFFSET	= 26,				//Offset to be applied to the col address (physical pixels do not begin in 0,0)
            //Screen GPIO Configuration
            RS_GPIO			= GPIOB,			//RS pin of the LCD
            RS_PIN			= GPIO_PIN_0,		//RS pin of the LCD
            RST_GPIO		= GPIOB,			//Reset pin of the LCD
            RST_PIN			= GPIO_PIN_1,		//Reset pin of the LCD
            RESET_DELAY     = 1,                //TIme needed for the reset to take effect in milliseconds
            //SPI Configuration
            SPI_CH			= SPI0,				//SPI used for the ST7735S
            SPI_CS_GPIO		= GPIOB,			//SPI Chip Select pin of the LCD
            SPI_CS_PIN		= GPIO_PIN_2,		//SPI Chip Select pin of the LCD
            SPI_CLK_GPIO	= GPIOA,			//SPI Clock pin of the LCD
            SPI_CLK_PIN		= GPIO_PIN_5,		//SPI Clock pin of the LCD
            SPI_MISO_GPIO	= GPIOA,			//SPI MISO In pin of the LCD
            SPI_MISO_PIN	= GPIO_PIN_6,		//SPI MISO In pin of the LCD
            SPI_MOSI_GPIO	= GPIOA,			//SPI MOSI In pin of the LCD
            SPI_MOSI_PIN	= GPIO_PIN_7,		//SPI MOSI In pin of the LCD
            //DMA Configuration
            USE_DMA         = true,            	//With DMA acceleration disabled, CPU use is the same, but the screen takes a lot longer to refresh as there are 74 update/sprite instead of 8 update/sprite
            DMA_SPI_TX      = DMA0,             //DMA pheriperal used for the SPI transmit
            DMA_SPI_TX_CH   = (dma_channel_enum)DMA_CH2,          //DMA channel used for the SPI transmit
            //DMA_SPI_RX      = DMA0,             //DMA pheriperal used for the SPI receive
            //DMA_SPI_RX_CH   = (dma_channel_enum)DMA_CH1,          //DMA channel used for the SPI receive
        } Config;

    private:
        /*********************************************************************************************************************************************************
        **********************************************************************************************************************************************************
        **	PRIVATE ENUM
        **********************************************************************************************************************************************************
        *********************************************************************************************************************************************************/
        
        //! @brief ST7735S Commands
        typedef enum _Command
        {           
            //  SETUP_NORMAL_MODE
            //Frame rate=850kHz/((RTNA x 2 + 40) x (LINE + FPA + BPA +2))
            //Frame Rate = 850Khz/( (5*2+40)*(80+56+56+2) )= 87.6[Hz]
            //RTNA | FPA | BPA
            SETUP_NORMAL_MODE           = 0xB1,
            //  SETUP_IDLE_MODE
            //Frame rate=850kHz/((RTNA x 2 + 40) x (LINE + FPA + BPA +2))
            //Frame Rate = 850Khz/( (5*2+40)*(80+56+56+2) )= 87.6[Hz]
            //RTNA | FPA | BPA
            SETUP_IDLE_MODE             = 0xB2,

            SETUP_PARTIAL_MODE          = 0xB3,
            //  DISPLAY_INVERSION_CONTROL
            //false  Dot Inversion | true = Normal Mode Column Inversion
            //Bit 0	| Normal Mode
            //Bit 1	| idle Mode
            //Bit 2 | partial mode/full colors
            DISPLAY_INVERSION_CONTROL   = 0xB4, 

            POWER_GVDD                  = 0xC0,
            POWER_VGH_VGL               = 0xC1,
            POWER_VCOM1                 = 0xC5,

            POWER_MODE_NORMAL           = 0xC2,
            POWER_MODE_IDLE             = 0xC3,
            POWER_MODE_PARTIAL          = 0xC4,

            ADJUST_GAMMA_PLUS           = 0xE0,
            ADJUST_GAMMA_MINUS          = 0xE1,
            
            //  COLOR_FORMAT
            //0x03	|	12b |4R4G|4B...4R|4G4B| 3 bytes transfer 2 color
            //0x05	|	16b |5R3G|3G5B|			2 bytes transfer 1 color
            //0x06	|	18b |6R..|6G..|6b..|	3 bytes transfer 1 color
            COLOR_FORMAT                = 0x3A,

            MEMORY_DATA_ACCESS_CONTROL  = 0x36,

            DISPLAY_ON                  = 0x29,
            SLEEP_OUT_BOOSTER_ON        = 0x11,

            ENABLE_DISPLAY_INVERSION	= 0x21,
            SEND_ROW_ADDRESS 			= 0x2A,		//Column address (ST7735S datasheet page 128/201)
            SEND_COL_ADDRESS 			= 0x2B,		//Row Address (ST7735S datasheet page 131/201)
            WRITE_MEM					= 0x2C,		//Memory Write. After this command, the display expects pixel data (ST7735S datasheet page 132/201)
            TERMINATOR					= 0xFF,		//Special terminator for the command parser FSM
        } Command;
    
        /*********************************************************************************************************************************************************
        **********************************************************************************************************************************************************
        **	PRIVATE STRUCT
        **********************************************************************************************************************************************************
        *********************************************************************************************************************************************************/
        
        //! @brief Sprite informations
        typedef struct _Sprite
        {
            //origin pixel of the sprite on the screen
            uint16_t origin_h;
            uint16_t origin_w;
            //Size of the sprite transfer
            uint16_t size_h;
            uint16_t size_w;
            uint32_t size;
            //false = sprite pixel buffer | true = solid color
            bool b_solid_color;
            //I do not use the sprite map and the solid color at the same time
            union
            {
                //pointer to the sprite pixel buffer
                uint16_t *sprite_ptr;
                //When in solid color mode, save the color to be applied to the full sprite
                uint16_t solid_color;
            };
        } Sprite;
    
        /*********************************************************************************************************************************************************
        **********************************************************************************************************************************************************
        **	PRIVATE INIT
        **********************************************************************************************************************************************************
        *********************************************************************************************************************************************************/

        //initialize GPIO configuration
        bool init_gpio( void );
        //Initialize SPI that communicates with the screen
        bool init_spi( void );
        //Initialize DMA that accelerates the SPI. Skipped if Config::USE_DMA is set to false
        bool init_dma( void );
        //Send ST7735 init sequence
        bool init_st7735( void );
        
        /*********************************************************************************************************************************************************
        **********************************************************************************************************************************************************
        **	PRIVATE HAL
        **********************************************************************************************************************************************************
        *********************************************************************************************************************************************************/

        //return true when the SPI is IDLE
        bool is_spi_idle( void );
        //Return true when the SPI is done TX
        bool is_spi_done_tx( void );
        //wait until SPI is idle. Blocking function.
        void spi_wait_idle( void );
        //wait until SPI is dont TX. Blocking function.
        void spi_wait_tbe( void );
        //select display
        void cs_active( void );
        //de-select display
        void cs_inactive( void );
        //Display in command mode
        void rs_mode_cmd( void );
        //Display in data mode
        void rs_mode_data( void );
        //Reset the display
        void rst_active( void );
        //Activate the display
        void rst_inactive( void );		
        //Configure the SPI to 8b
        void spi_set_8bit( void );
        //Configure the SPI to 16b
        void spi_set_16bit( void );
        //Use the DMA to send a 16b memory through the SPI
        void dma_send_map16( uint16_t *data_ptr, uint16_t data_size );
        //Use the DMA to send a 16b data through the SPI a number of times
        void dma_send_solid16( uint16_t *data_ptr, uint16_t data_size );

        /*********************************************************************************************************************************************************
        **********************************************************************************************************************************************************
        **	PRIVATE VARS
        **********************************************************************************************************************************************************
        *********************************************************************************************************************************************************/

        //! @brief ST7735S initialization sequence. Stored in flash memory
        static constexpr uint8_t g_st7735s_init_sequence[] =
        {
            Command::ENABLE_DISPLAY_INVERSION,      Command::TERMINATOR,
            Command::SETUP_NORMAL_MODE,             0x05, 0x3a, 0x3a, Command::TERMINATOR,
            Command::SETUP_IDLE_MODE,               0x05, 0x3a, 0x3a, Command::TERMINATOR,
            Command::SETUP_PARTIAL_MODE,            0x05, 0x3a, 0x3a, 0x05, 0x3a, 0x3a, Command::TERMINATOR,
            Command::DISPLAY_INVERSION_CONTROL,     0x03, Command::TERMINATOR,
            Command::POWER_GVDD,                    0x62, 0x02, 0x04, Command::TERMINATOR,
            Command::POWER_VGH_VGL,                 0xc0, Command::TERMINATOR,
            Command::POWER_MODE_NORMAL,             0x0d, 0x00, Command::TERMINATOR,
            Command::POWER_MODE_IDLE,               0x8d, 0x6a, Command::TERMINATOR,
            Command::POWER_MODE_PARTIAL,            0x8d, 0xee, Command::TERMINATOR,
            Command::POWER_VCOM1,                   0x0e, Command::TERMINATOR,
            Command::ADJUST_GAMMA_PLUS,             0x10, 0x0e, 0x02, 0x03, 0x0e, 0x07, 0x02, 0x07, 0x0a, 0x12, 0x27, 0x37, 0x00, 0x0d, 0x0e, 0x10, Command::TERMINATOR,
            Command::ADJUST_GAMMA_MINUS,            0x10, 0x0e, 0x03, 0x03, 0x0f, 0x06, 0x02, 0x08, 0x0a, 0x13, 0x26, 0x36, 0x00, 0x0d, 0x0e, 0x10, Command::TERMINATOR,
            Command::COLOR_FORMAT,                  0x55, Command::TERMINATOR,
            Command::MEMORY_DATA_ACCESS_CONTROL,    0x78, Command::TERMINATOR,
            Command::DISPLAY_ON,                    Command::TERMINATOR,
            Command::SLEEP_OUT_BOOSTER_ON,          Command::TERMINATOR,
            Command::TERMINATOR,
        };
        //! @brief Runtime sprite informations
        Sprite g_sprite;
        //! @brief Buffer to send address data using DMA
        uint16_t g_address_buffer[2];
        //! @brief FSM status
        uint32_t g_sprite_status;

    //--------------------------------------------------------------------------
    //	End Private
    //--------------------------------------------------------------------------
};	//End class: Lcd

    /*********************************************************************************************************************************************************
    **********************************************************************************************************************************************************
    **	CONSTRUCTORS
    **********************************************************************************************************************************************************
    *********************************************************************************************************************************************************/

/***************************************************************************/
//!	@brief constructor
//!	Display | void |
/***************************************************************************/
//! @details
//!	\n	initialize display class
//!	\n will NOT initialize the peripherals
/***************************************************************************/

Display::Display( void )
{
    //----------------------------------------------------------------
    //	BODY
    //----------------------------------------------------------------
    //Initialize class vars
    
    //FSM to idle
    this -> g_sprite_status = 0;

    //----------------------------------------------------------------
    //	RETURN
    //----------------------------------------------------------------

    return;
}	//End Constructor: Display | void |

    /*********************************************************************************************************************************************************
    **********************************************************************************************************************************************************
    **	DESTRUCTORS
    **********************************************************************************************************************************************************
    *********************************************************************************************************************************************************/

/***************************************************************************/
//!	@brief destructor
//!	~Display | void |
/***************************************************************************/

Display::~Display( void )
{
    
    //----------------------------------------------------------------
    //	RETURN
    //----------------------------------------------------------------

    return;
}	//End Destructor: ~Display | void |

    /*********************************************************************************************************************************************************
    **********************************************************************************************************************************************************
    **	PUBLIC INIT
    **********************************************************************************************************************************************************
    *********************************************************************************************************************************************************/

/***************************************************************************/
//!	@brief Public init
//!	init | void |
/***************************************************************************/
//! @return bool | false = OK | true = ERR
//! @details
//!	\n initialize the longan nano display and related peripherals
/***************************************************************************/

bool Display::init( void )
{
    //----------------------------------------------------------------
    //	VARS
    //----------------------------------------------------------------

    //Retun flag
    bool f_ret = false;

    //----------------------------------------------------------------
    //	BODY
    //----------------------------------------------------------------
    
    //Initialize GPIO configuration
    f_ret |= this -> init_gpio();

    //Reset the ST7735 display
    this -> rs_mode_data();
    this -> rst_active();	
    this -> cs_inactive();
    Longan_nano::Chrono::delay( Longan_nano::Chrono::Unit::milliseconds, Config::RESET_DELAY );

    //Activate the ST7735 display
    this -> rst_inactive();
    Longan_nano::Chrono::delay( Longan_nano::Chrono::Unit::milliseconds, Config::RESET_DELAY );

    //Initialize SPI that communicates with the display
    f_ret |= this -> init_spi();
    //Initialize the DMA that accelerates the SPI
    f_ret |= this -> init_dma();
    //Send ST7735 Initialization sequence
    f_ret |= this -> init_st7735();
    
    //----------------------------------------------------------------
    //	RETURN
    //----------------------------------------------------------------
    
    return f_ret;
}	//End Public init: init | void |

    /*********************************************************************************************************************************************************
    **********************************************************************************************************************************************************
    **	PUBLIC METHODS
    **********************************************************************************************************************************************************
    *********************************************************************************************************************************************************/

/***************************************************************************/
//!	@brief Public Method
//!	color | uint8_t | uint8_t | uint8_t |
/***************************************************************************/
//! @param r | uint8_t | 8bit red color channel
//! @param g | uint8_t | 8bit green color channel
//! @param b | uint8_t | 8bit blue color channel
//! @return uint16_t | RGB565 color compatible with ST7735 display color space
//! @details
//!	\n	convert from 24b 8R8G8B space to 16bit 5R6G5B space
//!	\n          RRRRRRRR
//!	\n          GGGGGGGG
//!	\n          BBBBBBBB
//!	\n	RRRRRGGGGGGBBBBB
/***************************************************************************/

uint16_t Display::color( uint8_t r, uint8_t g, uint8_t b )
{
    //----------------------------------------------------------------
    //	RETURN
    //----------------------------------------------------------------
    
    return ( ((uint16_t)0) | ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | ((b & 0xF8) >> 3) );
}	//End Public Method: color | uint8_t | uint8_t | uint8_t |

/***************************************************************************/
//!	@brief public method
//!	register_sprite | int | int | int | int | uint16_t * |
/***************************************************************************/
//! @param origin_h | int | starting top left corner height of the sprite
//! @param origin_w | int | starting top left corner height of the sprite
//! @param size_h | int | height size of the sprite
//! @param size_w | int | width size of the sprite
//! @param sprite_ptr | uint16_t * | pointer to RGB565 pixel color map
//! @return int | number of pixels queued for draw
//! @details
//!	\n	Ask the driver to draw a sprite
//!	\n	The draw method handles sprites that fill only part of the screen
//!	\n	If reworked, the buffer stays the same or is smaller than user buffer
//!	\n	1) Sprite fully inside screen area: the sprite is queued for draw
//!	\n	2) Sprite is fully outside screen area: no pixels are queued for draw
//!	\n	3) sprite is partially outside screen area: the method rework the buffer to include only pixel that can be drawn
/***************************************************************************/

int Display::register_sprite( int origin_h, int origin_w, int size_h, int size_w, uint16_t* sprite_ptr )
{
    //----------------------------------------------------------------
    //	BODY
    //----------------------------------------------------------------
    //! @todo handle partial sprite draw
    
    this -> g_sprite.origin_w		= origin_w;
    this -> g_sprite.origin_h		= origin_h;
    this -> g_sprite.size_w			= size_w;
    this -> g_sprite.size_h			= size_h;
    this -> g_sprite.size			= size_w *size_h;
    this -> g_sprite.b_solid_color	= false;
    this -> g_sprite.sprite_ptr		= sprite_ptr;
    //Start the FSM
    this -> g_sprite_status		= 1;

    //----------------------------------------------------------------
    //	RETURN
    //----------------------------------------------------------------
    
    return (size_w *size_h);
}	//End Public Method: register_sprite | int | int | int | int | uint16_t * |

/***************************************************************************/
//!	@brief public method
//!	register_sprite | int | int | int | int | uint16_t |
/***************************************************************************/
//! @param origin_h | int | starting top left corner height of the sprite
//! @param origin_w | int | starting top left corner height of the sprite
//! @param size_h | int | height size of the sprite
//! @param size_w | int | width size of the sprite
//! @param sprite_color | uint16_t | RGB565 pixel solid color for the full sprite
//! @return int | number of pixels queued for draw
//! @details
//!	\n	Ask the driver to draw a sprite with a solid color
//!	\n	The draw method handles sprites that fill only part of the screen
//!	\n	If reworked, the buffer stays the same or is smaller than user buffer
//!	\n	1) Sprite fully inside screen area: the sprite is queued for draw
//!	\n	2) Sprite is fully outside screen area: no pixels are queued for draw
//!	\n	3) sprite is partially outside screen area: the method rework the buffer to include only pixel that can be drawn
/***************************************************************************/

int Display::register_sprite( int origin_h, int origin_w, int size_h, int size_w, uint16_t sprite_color )
{
    //----------------------------------------------------------------
    //	BODY
    //----------------------------------------------------------------
    //! @todo handle partial sprite draw
    
    //Size of the sprite
    this -> g_sprite.origin_w		= origin_w;
    this -> g_sprite.origin_h		= origin_h;
    this -> g_sprite.size_w			= size_w;
    this -> g_sprite.size_h			= size_h;
    this -> g_sprite.size			= size_w *size_h;
    //Draw a solid color
    this -> g_sprite.b_solid_color	= true;
    this -> g_sprite.solid_color	= sprite_color;
    //Start the FSM
    this -> g_sprite_status			= 1;

    //----------------------------------------------------------------
    //	RETURN
    //----------------------------------------------------------------
    
    return (this -> g_sprite.size);
}	//End Public Method: register_sprite | int | int | int | int | uint16_t * |

/***************************************************************************/
//!	@brief public method
//!	update_sprite | void |
/***************************************************************************/
//! @return bool | false = IDLE | true = BUSY
//! @details
//!	\n	Execute a step of the FSM that interfaces with the physical display
//!	\n	Handle both solid color and color map
//!	\n	sprite data must already be valid before execution
/***************************************************************************/

bool Display::update_sprite( void )
{
    //----------------------------------------------------------------
    //	BODY
    //----------------------------------------------------------------
    
    //Switch: FSM status
    switch (this -> g_sprite_status)
    {
        //IDLE
        case 0:
        {
            //Do Nothing
            
            break;
        }
        //Row address
        case 1:
        {
            if (this -> is_spi_idle() == true)
            {
                //Commands are 8b
                this -> spi_set_8bit();
                this -> rs_mode_cmd();
                spi_i2s_data_transmit(Config::SPI_CH, Command::SEND_ROW_ADDRESS);
                //Next state
                this -> g_sprite_status++;
            }
            break;
        }
        //Row Address start
        case 2:
        {
            //If: user wants to use the DMA
            if (Config::USE_DMA == true)
            {
                if (this -> is_spi_idle() == true)
                {
                    //Configure Data are 16b
                    this -> rs_mode_data();
                    this -> spi_set_16bit();
                    //Load addresses on the address buffer
                    this -> g_address_buffer[ 0 ] = this -> g_sprite.origin_w +Config::ROW_ADDRESS_OFFSET;
                    this -> g_address_buffer[ 1 ] = this -> g_sprite.origin_w +Config::ROW_ADDRESS_OFFSET +this -> g_sprite.size_w -1;
                    //Program the DMA to send the address
                    this -> dma_send_map16( this -> g_address_buffer, 2 );
                    //Next state. Skip second SPI transfer
                    this -> g_sprite_status += 2;
                }
            }
            //End If: user doesn't want to use the DMA
            else
            {
                if (this -> is_spi_idle() == true)
                {
                    this -> spi_set_16bit();
                    this -> rs_mode_data();
                    spi_i2s_data_transmit(Config::SPI_CH, this -> g_sprite.origin_w +Config::ROW_ADDRESS_OFFSET);
                    //Next state 
                    this -> g_sprite_status++;
                }
            }
            break;
        }
        //Row Address stop
        case 3:
        {
            if (this -> is_spi_done_tx() == true)
            {
                spi_i2s_data_transmit(Config::SPI_CH, this -> g_sprite.origin_w +Config::ROW_ADDRESS_OFFSET +this -> g_sprite.size_w -1);	
                //Next state
                this -> g_sprite_status++;
            }
            break;
        }	
        //Col Address
        case 4:
        {
            if (this -> is_spi_idle() == true)
            {
                //Commands are 8b
                this -> spi_set_8bit();
                this -> rs_mode_cmd();
                spi_i2s_data_transmit(Config::SPI_CH, Command::SEND_COL_ADDRESS);
                //Next state
                this -> g_sprite_status++;
            }
            break;
        }	
        //Col Address Start
        case 5:
        {
            //If: user wants to use the DMA
            if (Config::USE_DMA == true)
            {
                if (this -> is_spi_idle() == true)
                {
                    //Configure HW
                    this -> rs_mode_data();
                    this -> spi_set_16bit();
                    //Load addresses on the address buffer
                    this -> g_address_buffer[ 0 ] = this -> g_sprite.origin_h +Config::COL_ADDRESS_OFFSET;
                    this -> g_address_buffer[ 1 ] = this -> g_sprite.origin_h +Config::COL_ADDRESS_OFFSET +this -> g_sprite.size_h -1;
                    //Program the DMA to send the address
                    this -> dma_send_map16( this -> g_address_buffer, 2 );
                    //Next state. Skip second SPI transfer
                    this -> g_sprite_status += 2;
                }
            }
            //End If: user doesn't want to use the DMA
            else
            {
                if (this -> is_spi_idle() == true)
                {
                    this -> spi_set_16bit();
                    this -> rs_mode_data();
                    spi_i2s_data_transmit(Config::SPI_CH, this -> g_sprite.origin_h +Config::COL_ADDRESS_OFFSET);
                    //Next state 
                    this -> g_sprite_status++;
                }
            }
            break;
        }
        //Col Address Stop
        case 6:
        {
            if (this -> is_spi_done_tx() == true)
            {
                spi_i2s_data_transmit( Config::SPI_CH, this -> g_sprite.origin_h +Config::COL_ADDRESS_OFFSET +this -> g_sprite.size_h -1 );
                //Next state
                this -> g_sprite_status++;
            }
            break;
        }
        //Write Memory
        case 7:
        {
            if (this -> is_spi_idle() == true)
            {
                this -> spi_set_8bit();
                this -> rs_mode_cmd();
                spi_i2s_data_transmit( Config::SPI_CH, Command::WRITE_MEM );
                //Next state. DMA uses a single state, SPI use one state per pixel
                this -> g_sprite_status = ((Config::USE_DMA == true)?(8):(10));
            }
            break;
        }
        //DMA Send
        case 8:
        {
            if (this -> is_spi_idle() == true)
            {
                this -> rs_mode_data();
                this -> spi_set_16bit();
                //If: the sprite uses a color map
                if (this -> g_sprite.b_solid_color == false)
                {
                    //Program the DMA to send the pixel map
                    this -> dma_send_map16( this -> g_sprite.sprite_ptr, this -> g_sprite.size );
                }	//End If: the sprite uses a color map
                //If: the sprite is solid color
                else //if (this -> g_sprite.b_solid_color == true)
                {
                    //Program the DMA to send the same pixel a number of times
                    this -> dma_send_solid16( &this -> g_sprite.solid_color, this -> g_sprite.size );    
                }	//End If: the sprite is solid color
                                
                //STOP
                this -> g_sprite_status = 9;
            }
            break;
        }			
        //STOP
        case 9:
        {
            if (this -> is_spi_idle() == true)
            {
                //Return to IDLE
                this -> g_sprite_status = 0;
            }
            break;
        }
        //SPI SEND FIRST
        case 10:
        {
            if (this -> is_spi_idle() == true)
            {
                this -> rs_mode_data();
                this -> spi_set_16bit();
                spi_i2s_data_transmit( Config::SPI_CH, (this -> g_sprite.b_solid_color == false)?(this -> g_sprite.sprite_ptr[this -> g_sprite_status -10]):(this -> g_sprite.solid_color) );
                //If: all pixels have been transfered
                if ((this -> g_sprite_status -10) >= (this -> g_sprite.size -1))
                {
                    //STOP
                    this -> g_sprite_status = 9;
                }
                //If: there are pixels to be transfered
                else
                {
                    //Next pixel
                    this -> g_sprite_status++;
                }	
            }
            break;
        }
        //SPI SEND
        default:
        { 
            if (this -> is_spi_done_tx() == true)
            {
                spi_i2s_data_transmit( Config::SPI_CH, (this -> g_sprite.b_solid_color == false)?(this -> g_sprite.sprite_ptr[this -> g_sprite_status -10]):(this -> g_sprite.solid_color) );
                //If: all pixels have been transfered
                if ((this -> g_sprite_status -10) >= (this -> g_sprite.size -1))
                {
                    //STOP
                    this -> g_sprite_status = 9;
                }
                //If: there are pixels to be transfered
                else
                {
                    //Next pixel
                    this -> g_sprite_status++;
                }	
            }
        }
    }	//End Switch: FSM Status
    
    //----------------------------------------------------------------
    //	RETURN
    //----------------------------------------------------------------
    
    return (this -> g_sprite_status != 0);
}	//End public method: update_sprite | void |

/***************************************************************************/
//!	@brief public method
//!	draw_sprite | int | int | int | int | uint16_t * |
/***************************************************************************/
//! @param origin_h | int | starting top left corner height of the sprite
//! @param origin_w | int | starting top left corner height of the sprite
//! @param size_h | int | height size of the sprite
//! @param size_w | int | width size of the sprite
//! @param sprite_ptr | uint16_t * | pointer to RGB565 pixel color map
//! @return int | number of pixels drawn
//! @details
//!	\n	Draw a sprite
//!	\n Blocking method
//!	\n Draw a color map, requires a user buffer of the proper size
/***************************************************************************/

int Display::draw_sprite( int origin_h, int origin_w, int size_h, int size_w, uint16_t* sprite_ptr )
{
    //----------------------------------------------------------------
    //	VARS
    //----------------------------------------------------------------
    
    //temp return
    bool f_ret;
    //Number of pixels drawn by the method
    int pixel_count;
    
    //----------------------------------------------------------------
    //	BODY
    //----------------------------------------------------------------

    //Register the sprite to be drawn. Authorize the update FSM to draw the sprite through the "update_sprite" non blocking method
    pixel_count = this -> register_sprite( origin_h, origin_w, size_h, size_w, sprite_ptr );
    //Allow FSM to run if I have at least one pixel to draw
    f_ret = (pixel_count > 0);
    //While: the driver FSM is busy
    while (f_ret == true)
    {
        //Execute a step of the FSM
        f_ret = this -> update_sprite();
    }

    //----------------------------------------------------------------
    //	RETURN
    //----------------------------------------------------------------

    return pixel_count;
}	//End Public Method: draw_sprite | int | int | int | int | uint16_t * |

/***************************************************************************/
//!	@brief public method
//!	draw_sprite | int | int | int | int | uint16_t |
/***************************************************************************/
//! @param origin_h | int | starting top left corner height of the sprite
//! @param origin_w | int | starting top left corner height of the sprite
//! @param size_h | int | height size of the sprite
//! @param size_w | int | width size of the sprite
//! @param sprite_color | uint16_t | RGB565 pixel solid color for the full sprite
//! @return int | number of pixels drawn
//! @details
//!	\n Draw a sprite
//!	\n Blocking method
//!	\n Draw a solid color sprite and doesn't require a color map
/***************************************************************************/

int Display::draw_sprite( int origin_h, int origin_w, int size_h, int size_w, uint16_t sprite_color )
{
    //----------------------------------------------------------------
    //	VARS
    //----------------------------------------------------------------
    
    //temp return
    bool f_ret;
    //Number of pixels drawn by the method
    int pixel_count;
    
    //----------------------------------------------------------------
    //	BODY
    //----------------------------------------------------------------

    //Register the sprite to be drawn. Authorize the update FSM to draw the sprite through the "update_sprite" non blocking method
    pixel_count = this -> register_sprite( origin_h, origin_w, size_h, size_w, sprite_color );
    //Allow FSM to run if I have at least one pixel to draw
    f_ret = (pixel_count > 0);
    //While: the driver FSM is busy
    while (f_ret == true)
    {
        //Execute a step of the FSM
        f_ret = this -> update_sprite();
    }

    //----------------------------------------------------------------
    //	RETURN
    //----------------------------------------------------------------

    return pixel_count;
}	//End Public Method: draw_sprite | int | int | int | int | uint16_t |

/***************************************************************************/
//!	@brief public method
//!	clear | void |
/***************************************************************************/
//! @return int | number of pixels drawn
//! @details
//!	\n Clear the screen. Blocking method.
//!	\n Draw a black sprite the size of the screen
/***************************************************************************/

inline int Display::clear( void )
{
    //----------------------------------------------------------------
    //	BODY
    //----------------------------------------------------------------

    //blocking draw sprite
    int pixel_count = this -> draw_sprite( 0, 0, Config::HEIGHT, Config::WIDTH, Display::color( 0, 0, 0 ) );

    //----------------------------------------------------------------
    //	RETURN
    //----------------------------------------------------------------

    return pixel_count;
}	//End public method: clear | void |

/***************************************************************************/
//!	@brief public method
//!	clear | void |
/***************************************************************************/
//! @param color | uint16_t | clear the display to a solid color
//! @return int | number of pixels drawn
//! @details
//!	\n Clear the screen. Blocking method.
//!	\n Draw a black sprite the size of the screen
/***************************************************************************/

inline int Display::clear( uint16_t color )
{
    //----------------------------------------------------------------
    //	BODY
    //----------------------------------------------------------------

    int pixel_count = this -> draw_sprite( 0, 0, Config::HEIGHT, Config::WIDTH, color );

    //----------------------------------------------------------------
    //	RETURN
    //----------------------------------------------------------------

    return pixel_count;
}	//End public method: clear | void |

    /*********************************************************************************************************************************************************
    **********************************************************************************************************************************************************
    **	PRIVATE INIT
    **********************************************************************************************************************************************************
    *********************************************************************************************************************************************************/

/***************************************************************************/
//!	@brief Private init
//!	init_gpio | void |
/***************************************************************************/
//! @return bool | false = OK | true = ERR
//! @details
//!	\n HAL method
//!	\n initialize GPIO configuration
/***************************************************************************/

inline bool Display::init_gpio( void )
{
    //----------------------------------------------------------------
    //	BODY
    //----------------------------------------------------------------
    
    //Clock the GPIOs
    rcu_periph_clock_enable( RCU_GPIOA );
    rcu_periph_clock_enable( RCU_GPIOB );
    rcu_periph_clock_enable( RCU_AF );
        
    //LCD RS command pin
    gpio_init( Config::RS_GPIO, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, Config::RS_PIN );
    //LCD RST reset pin
    gpio_init( Config::RST_GPIO, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, Config::RST_PIN );
    //LCD CS chip select pin
    gpio_init( Config::SPI_CS_GPIO, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, Config::SPI_CS_PIN );
    //Set the SPI0 pins to Alternate Functions
    gpio_init( Config::SPI_CLK_GPIO, GPIO_MODE_AF_PP, GPIO_OSPEED_50MHZ, Config::SPI_CLK_PIN );
    gpio_init( Config::SPI_MISO_GPIO, GPIO_MODE_AF_PP, GPIO_OSPEED_50MHZ, Config::SPI_MISO_PIN );
    gpio_init( Config::SPI_MOSI_GPIO, GPIO_MODE_AF_PP, GPIO_OSPEED_50MHZ, Config::SPI_MOSI_PIN );

    //----------------------------------------------------------------
    //	RETURN
    //----------------------------------------------------------------
    
    return false; //OK
}	//End Private init: init_gpio | void |

/***************************************************************************/
//!	@brief Private init
//!	init_spi | void |
/***************************************************************************/
//! @return bool | false = OK | true = ERR
//! @details
//!	\n HAL method
//!	\n Initialize SPI that communicates with the Display
/***************************************************************************/

inline bool Display::init_spi( void )
{
    //----------------------------------------------------------------
    //	BODY
    //----------------------------------------------------------------

    //Clock the SPI
    rcu_periph_clock_enable( RCU_SPI0 );
    spi_i2s_deinit( Config::SPI_CH );
    SPI_CTL0( Config::SPI_CH ) = (uint32_t)(SPI_MASTER | SPI_TRANSMODE_FULLDUPLEX | SPI_FRAMESIZE_8BIT | SPI_NSS_SOFT | SPI_ENDIAN_MSB | SPI_CK_PL_LOW_PH_1EDGE | SPI_PSC_8);
    //If: DMA is accelerating the SPI
    if (Config::USE_DMA == true)
    {
        SPI_CTL1( Config::SPI_CH ) = (uint32_t)(SPI_CTL1_DMATEN);
    }
    spi_enable( Config::SPI_CH );

    //----------------------------------------------------------------
    //	RETURN
    //----------------------------------------------------------------
    
    return false; //OK
}	//End Private init: init_spi | void |

/***************************************************************************/
//!	@brief Private init
//!	init_dma | void |
/***************************************************************************/
//! @return bool | false = OK | true = ERR
//! @details
//!	\n Initialize DMA that accelerates the SPI
/***************************************************************************/

inline bool Display::init_dma( void )
{
    //----------------------------------------------------------------
    //	BODY
    //----------------------------------------------------------------
    
    //CLock the DMA
    rcu_periph_clock_enable( RCU_DMA0 );
    dma_deinit( Config::DMA_SPI_TX, (dma_channel_enum)Config::DMA_SPI_TX_CH );
    DMA_CHCTL( Config::DMA_SPI_TX, Config::DMA_SPI_TX_CH ) = (uint32_t)(DMA_PRIORITY_ULTRA_HIGH | DMA_CHXCTL_DIR);
    DMA_CHPADDR( Config::DMA_SPI_TX, Config::DMA_SPI_TX_CH ) = (uint32_t)&SPI_DATA(Config::SPI_CH);

    //----------------------------------------------------------------
    //	RETURN
    //----------------------------------------------------------------
    
    return false; //OK
}	//End Private init: init_dma | void |

/***************************************************************************/
//!	@brief Private init
//!	init_st7735 | void |
/***************************************************************************/
//! @return bool | false = OK | true = ERR
//! @details
//!	\n Send initialization sequence to the ST7735 display controller
//!	\n First byte in the sequence is command
//!	\n Next bytes in the sequence are terminators
//!	\n If the command is a terminator, it means the initialization sequence is complete
/***************************************************************************/

bool Display::init_st7735( void )
{
    //----------------------------------------------------------------
    //	BODY
    //----------------------------------------------------------------
    
    //Configure SPI and select display
    this -> cs_active();
    this -> spi_set_8bit();
    //For: each command sequence
    for (const uint8_t *str_p = this -> g_st7735s_init_sequence; *str_p != Command::TERMINATOR; str_p++)
    {
        //Wait SPI
        this -> spi_wait_idle();
        //Send command
        this -> rs_mode_cmd();
        spi_i2s_data_transmit(Config::SPI_CH, *str_p++);
        //Wait SPI
        this -> spi_wait_idle();
        //Enter data mode
        this -> rs_mode_data();
        //While: I'm not done sendinga data
        while(*str_p != Command::TERMINATOR)
        {
            //Wait SPI
            this -> spi_wait_tbe();
            //Send DATA
            spi_i2s_data_transmit(Config::SPI_CH, *str_p++);
        }
    }	//End For: each command sequence

    //----------------------------------------------------------------
    //	RETURN
    //----------------------------------------------------------------
    
    return false; //OK
}	//End Private init: init_st7735 | void |

    /*********************************************************************************************************************************************************
    **********************************************************************************************************************************************************
    **	PRIVATE HAL
    **********************************************************************************************************************************************************
    *********************************************************************************************************************************************************/

/***************************************************************************/
//!	@brief Private HAL Method
//!	is_spi_idle | void
/***************************************************************************/
//! @return bool | false = BUSY | true = IDLE
//! @details
//!	\n return true when the SPI is IDLE
/***************************************************************************/

inline bool Display::is_spi_idle( void )
{
    //----------------------------------------------------------------
    //	RETURN
    //----------------------------------------------------------------
    
    return ((SPI_STAT( Config::SPI_CH ) &SPI_STAT_TRANS) == 0);
}	//End Private HAL Method: is_spi_idle | void

/***************************************************************************/
//!	@brief Private HAL Method
//!	spi_wait_idle | void
/***************************************************************************/
//! @details
//!	\n Block execution until SPI is IDLE
/***************************************************************************/

inline void Display::spi_wait_idle( void )
{
    //----------------------------------------------------------------
    //	BODY
    //----------------------------------------------------------------
    
    //While: SPI is BUSY
    while( (SPI_STAT( Config::SPI_CH ) &SPI_STAT_TRANS) != 0 )
    {
        //Block Execution
    }
    
    //----------------------------------------------------------------
    //	RETURN
    //----------------------------------------------------------------
    
    return;
}	//End Private HAL Method: is_spi_idle | void

/***************************************************************************/
//!	@brief Private HAL Method
//!	is_spi_done_tx | void
/***************************************************************************/
//! @return bool | false = BUSY | true = IDLE
//! @details
//!	\n return true when the SPI is done TX
/***************************************************************************/

inline bool Display::is_spi_done_tx( void )
{
    //----------------------------------------------------------------
    //	RETURN
    //----------------------------------------------------------------
    
    return ((SPI_STAT( Config::SPI_CH ) &SPI_STAT_TBE ) != 0);
}	//End Private HAL Method: is_spi_idle | void

/***************************************************************************/
//!	@brief Private HAL Method
//!	spi_wait_tbe | void |
/***************************************************************************/
//! @details
//!	\n Block execution until SPI is done TX
/***************************************************************************/

inline void Display::spi_wait_tbe( void )
{
    //----------------------------------------------------------------
    //	BODY
    //----------------------------------------------------------------

    while( ( SPI_STAT( Config::SPI_CH ) &SPI_STAT_TBE ) == 0 )
    {
        //Block Execution
    }

    //----------------------------------------------------------------
    //	RETURN
    //----------------------------------------------------------------
    
    return;
}	//End Private HAL Method: spi_wait_tbe | void |

/***************************************************************************/
//!	@brief Private HAL Method
//!	cs_active | void |
/***************************************************************************/
//! @details
//!	\n Select the ST7735 Display
/***************************************************************************/

inline void Display::cs_active( void )
{
    //----------------------------------------------------------------
    //	BODY
    //----------------------------------------------------------------

    gpio_bit_reset( Config::SPI_CS_GPIO, Config::SPI_CS_PIN );

    //----------------------------------------------------------------
    //	RETURN
    //----------------------------------------------------------------

    return;
}	//End Private HAL Method: cs_active | void |

/***************************************************************************/
//!	@brief Private HAL Method
//!	cs_inactive | void |
/***************************************************************************/
//! @details
//!	\n Deselect the ST7735 Display
/***************************************************************************/

inline void Display::cs_inactive( void )
{
    //----------------------------------------------------------------
    //	BODY
    //----------------------------------------------------------------

    gpio_bit_set( Config::SPI_CS_GPIO, Config::SPI_CS_PIN );
    
    //----------------------------------------------------------------
    //	RETURN
    //----------------------------------------------------------------

    return;
}	//End Private HAL Method: cs_inactive | void |

/***************************************************************************/
//!	@brief Private HAL Method
//!	rs_mode_cmd | void |
/***************************************************************************/
//! @details
//!	\n Send a command to the display
/***************************************************************************/

inline void Display::rs_mode_cmd( void )
{
    //----------------------------------------------------------------
    //	BODY
    //----------------------------------------------------------------

    gpio_bit_reset( Config::RS_GPIO, Config::RS_PIN );
    
    //----------------------------------------------------------------
    //	RETURN
    //----------------------------------------------------------------

    return;
}	//End Private HAL Method: rs_mode_cmd | void |

/***************************************************************************/
//!	@brief Private HAL Method
//!	rs_mode_data | void |
/***************************************************************************/
//! @details
//!	\n Send data to the display
/***************************************************************************/

inline void Display::rs_mode_data( void )
{
    //----------------------------------------------------------------
    //	BODY
    //----------------------------------------------------------------

    gpio_bit_set( Config::RS_GPIO, Config::RS_PIN );
    
    //----------------------------------------------------------------
    //	RETURN
    //----------------------------------------------------------------

    return;
}	//End Private HAL Method: rs_mode_data | void |

/***************************************************************************/
//!	@brief Private HAL Method
//!	rst_active | void |
/***************************************************************************/
//! @details
//!	\n Assert the reset pin of the physical display
//!	\n The display will need time to become ready after an assert
/***************************************************************************/

inline void Display::rst_active( void )
{
    //----------------------------------------------------------------
    //	BODY
    //----------------------------------------------------------------
    
    gpio_bit_reset( Config::RST_GPIO, Config::RST_PIN );
    
    //----------------------------------------------------------------
    //	RETURN
    //----------------------------------------------------------------
    
    return;
}	//End Private HAL Method: rst_active | void |

/***************************************************************************/
//!	@brief Private HAL Method
//!	rst_inactive | void |
/***************************************************************************/
//! @details
//!	\n Deassert the reset pin of the physical display
/***************************************************************************/

inline void Display::rst_inactive( void )
{
    //----------------------------------------------------------------
    //	BODY
    //----------------------------------------------------------------
    
    gpio_bit_set( Config::RST_GPIO, Config::RST_PIN );
    
    //----------------------------------------------------------------
    //	RETURN
    //----------------------------------------------------------------
    
    return;
}	//End Private HAL Method: rst_inactive | void |

/***************************************************************************/
//!	@brief Private HAL Method
//!	spi_set_8bit | void |
/***************************************************************************/
//! @details
//!	\n Configure the SPI to 8b
/***************************************************************************/		

inline void Display::spi_set_8bit( void )
{
    //----------------------------------------------------------------
    //	BODY
    //----------------------------------------------------------------
    
    if (SPI_CTL0(Config::SPI_CH) & (uint32_t)(SPI_CTL0_FF16))
    {
        SPI_CTL0(Config::SPI_CH) &= ~(uint32_t)(SPI_CTL0_SPIEN);
        SPI_CTL0(Config::SPI_CH) &= ~(uint32_t)(SPI_CTL0_FF16);
        SPI_CTL0(Config::SPI_CH) |= (uint32_t)(SPI_CTL0_SPIEN);
    }
    
    //----------------------------------------------------------------
    //	RETURN
    //----------------------------------------------------------------
    
    return;
}	//End Private HAL Method: spi_set_8bit | void |

/***************************************************************************/
//!	@brief Private HAL Method
//!	spi_set_16bit | void |
/***************************************************************************/
//! @details
//!	\n Configure the SPI to 16b
/***************************************************************************/

inline void Display::spi_set_16bit( void )
{
    //----------------------------------------------------------------
    //	BODY
    //----------------------------------------------------------------
    
    if (!(SPI_CTL0(Config::SPI_CH) & (uint32_t)(SPI_CTL0_FF16)))
    {
        SPI_CTL0(Config::SPI_CH) &= ~(uint32_t)(SPI_CTL0_SPIEN);
        SPI_CTL0(Config::SPI_CH) |= (uint32_t)(SPI_CTL0_FF16);
        SPI_CTL0(Config::SPI_CH) |= (uint32_t)(SPI_CTL0_SPIEN);
    }
    
    //----------------------------------------------------------------
    //	RETURN
    //----------------------------------------------------------------
    
    return;
}	//End Private HAL Method: spi_set_16bit | void |

/***************************************************************************/
//!	@brief Private HAL Method
//!	dma_send_map16 | uint16_t * | uint16_t |
/***************************************************************************/
//! @param data_ptr | uint16_t * | pointer to RGB565 pixel color map
//! @param data_size | uint16_t | size of the pixel color map in pixels
//! @return void
//! @details
//!	\n Use the DMA to send a 16b memory through the SPI
/***************************************************************************/		

inline void Display::dma_send_map16( uint16_t *data_ptr, uint16_t data_size )
{
    //----------------------------------------------------------------
    //	BODY
    //----------------------------------------------------------------
    
    dma_channel_disable( Config::DMA_SPI_TX, (dma_channel_enum)Config::DMA_SPI_TX_CH );
    dma_memory_width_config( Config::DMA_SPI_TX, (dma_channel_enum)Config::DMA_SPI_TX_CH, DMA_MEMORY_WIDTH_16BIT );
    dma_periph_width_config( Config::DMA_SPI_TX, (dma_channel_enum)Config::DMA_SPI_TX_CH, DMA_PERIPHERAL_WIDTH_16BIT );
    dma_memory_address_config( Config::DMA_SPI_TX, (dma_channel_enum)Config::DMA_SPI_TX_CH, (uint32_t)(data_ptr) );
    dma_memory_increase_enable( Config::DMA_SPI_TX, (dma_channel_enum)Config::DMA_SPI_TX_CH );
    dma_transfer_number_config( Config::DMA_SPI_TX, (dma_channel_enum)Config::DMA_SPI_TX_CH, data_size );
    //Begin the DMA transfer
    dma_channel_enable( Config::DMA_SPI_TX, (dma_channel_enum)Config::DMA_SPI_TX_CH );
    
    //----------------------------------------------------------------
    //	RETURN
    //----------------------------------------------------------------
    
    return;
}	//End Private HAL Method: dma_send_map16 | uint16_t * | uint16_t |


/***************************************************************************/
//!	@brief Private HAL Method
//!	dma_send_map16 | uint16_t * | uint16_t |
/***************************************************************************/
//! @param data_ptr | uint16_t * | pointer to RGB565 solid color for the full sprite
//! @param data_size | uint16_t | size of the pixel color map in pixels
//! @return void
//! @details
//!	\n Use the DMA to send a 16b data through the SPI a number of times
/***************************************************************************/		

inline void Display::dma_send_solid16( uint16_t* data_ptr, uint16_t data_size )
{
    //----------------------------------------------------------------
    //	BODY
    //----------------------------------------------------------------
    
    dma_channel_disable( Config::DMA_SPI_TX, (dma_channel_enum)Config::DMA_SPI_TX_CH );
    dma_memory_width_config( Config::DMA_SPI_TX, (dma_channel_enum)Config::DMA_SPI_TX_CH, DMA_MEMORY_WIDTH_16BIT );
    dma_periph_width_config( Config::DMA_SPI_TX, (dma_channel_enum)Config::DMA_SPI_TX_CH, DMA_PERIPHERAL_WIDTH_16BIT );
    dma_memory_address_config( Config::DMA_SPI_TX, (dma_channel_enum)Config::DMA_SPI_TX_CH, (uint32_t)(data_ptr) );
    dma_memory_increase_disable( Config::DMA_SPI_TX, (dma_channel_enum)Config::DMA_SPI_TX_CH );
    dma_transfer_number_config( Config::DMA_SPI_TX, (dma_channel_enum)Config::DMA_SPI_TX_CH, data_size );
    //Begin the DMA transfer
    dma_channel_enable( Config::DMA_SPI_TX, (dma_channel_enum)Config::DMA_SPI_TX_CH );
    
    //----------------------------------------------------------------
    //	RETURN
    //----------------------------------------------------------------
    
    return;
}	//End Private HAL Method: dma_send_map16 | uint16_t * | uint16_t |

/**********************************************************************************
**	NAMESPACE
**********************************************************************************/

} //End namespace: Longan_nano

#else
    #warning "Multiple inclusion of hader file ST7735S_W160_H80_C16_HPP_"
#endif
