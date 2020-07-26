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
//! @version	2020-07-22
//! @brief		Longan Nano Display Driver
//! @copyright	BSD 3-Clause License (c) 2019-2020, Samuli Laine
//! @copyright	BSD 3-Clause License (c) 2020, Orso Eric
//! @details
//!	Driver to use the embedded 160x80 0.96' LCD on the longan nano
//! SPI0 interface with physical screen
//!	DMA0: executes a number of SPI0 transfers in background
//!	Hardware display has a ST7735S controller
//!	The driver is meant to accelerate sprite based drawing
//!	Sprite buffer and draw are going to be handled by an higher level class
//!		2020-07-07
//!	Ported original LCD example to class. Basic operation. DMA doesn't work
//!		2020-07-08
//!	Skeletron for sprites
//!		2020-07-10
//!	Change architecture. S7735S_W160_H80_C16.hpp. Common name drive class: Display
//!	With another display controller, Screen class and Display class interface remains the same
//!	S7735S_W160_H80_C16.hpp is the specific lbrary for the physical display
//!	Clean up, partition in a more intelligent way calls. Test project for driver only without sprites
//!		2020-07-15
//!	Tested draw with HAL builtin
//!		2020-07-21
//!	Found working DMA example from Samuli Laine https://github.com/slmisc/gd32v-lcd
//!		2020-07-22
//!	Transplant working driver function
//!	Refactor function to use configuration and fit class form
//!	set_sprite and update_sprite provide a non blocking draw method that hide the wait time
//!	application can call update_sprite and immediately continue to do work while SPI and DMA are busy
//!	update_sprite will immediately return without doing work if SPI or DMA are busy or if there is nothing to draw
//!		2020-07-23
//!	Clean up example
//!	Prune excess methods from display class. Leave only the draw sprite primitives. Application is to be given control on resource utilization of the display
//!	Refactor all calls to use configuration enum for the hardware peripherals
//!		2020-07-24
//!	Fixed color conversion function
/************************************************************************************/

class Display
{
    //Visible to all
    public:
        /*********************************************************************************************************************************************************
        **********************************************************************************************************************************************************
        **	PUBLIC ENUM
        **********************************************************************************************************************************************************
        *********************************************************************************************************************************************************/
        
        //Configuration parameters of the LCD display
        typedef enum _Config
        {
            //Screen physical configuration
            WIDTH				= 160,				//WIdth of the LCD display
            HEIGHT				= 80,				//Height of the LCD display
            PIXEL_COUNT			= WIDTH *HEIGHT,	//Frame buffer
            COLOR_DEPTH			= 16,				//Color depth. Screen allows 12, 16 and 18
            ROW_ADDRESS_OFFSET	= 1,				//Offset to be applied to the row address (physical pixels do not begin in 0,0)
            COL_ADDRESS_OFFSET	= 26,				//Offset to be applied to the col address (physical pixels do not begin in 0,0)
            
            //Screen GPIO Configuration
            RS_GPIO			= GPIOB,			//RS pin of the LCD
            RS_PIN			= GPIO_PIN_0,		//RS pin of the LCD
            RST_GPIO		= GPIOB,			//Reset pin of the LCD
            RST_PIN			= GPIO_PIN_1,		//Reset pin of the LCD
            
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
            DMA_SPI_TX      = DMA0,             //DMA pheriperal used for the SPI transmit
            DMA_SPI_TX_CH   = (dma_channel_enum)DMA_CH2,          //DMA channel used for the SPI transmit
            //DMA_SPI_RX      = DMA0,             //DMA pheriperal used for the SPI receive
            //DMA_SPI_RX_CH   = DMA_CH1,          //DMA channel used for the SPI receive
        } Config;

        //Default colors for the LCD display 
        typedef enum _Color
        {
            //5R6G5B = 16b
            BLACK			= 0x0000,
            WHITE			= 0xFFFF,
            RED				= 0xF800,
            GREEN			= 0x07E0,
            BLUE			= 0x001F, 
            BRED			= 0XF81F,
            GRED			= 0XFFE0,
            GBLUE			= 0X07FF,
            MAGENTA			= 0xF81F,
            CYAN			= 0x7FFF,
            YELLOW			= 0xFFE0,
            BROWN			= 0XBC40,		//brown
            BRRED			= 0XFC07,		//maroon
            GRAY			= 0X8430,		//gray
            DARKBLUE		= 0X01CF,		//navy blue
            LIGHTBLUE		= 0X7D7C,		//light blue 
            GRAYBLUE		= 0X5458,		//gray blue
            LIGHTGREEN		= 0X841F,		//light green
            LGRAY			= 0XC618,		//Light gray (PANNEL), form background color
            LGRAYBLUE		= 0XA651,		//Light gray blue (middle layer color)
            LBBLUE			= 0X2B12,		//Light brown blue (inverted color of selected item)
        } Color;

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
        //!	initialize display class
        //! will NOT initialize the peripherals
        /***************************************************************************/

        Display( void )
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

        ~Display( void )
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
        //!	initialize the longan nano display and related peripherals
        /***************************************************************************/
        
        bool init( void )
        {
            //----------------------------------------------------------------
            //	BODY
            //----------------------------------------------------------------
            
            rcu_periph_clock_enable( RCU_GPIOA );
            rcu_periph_clock_enable( RCU_GPIOB );
            rcu_periph_clock_enable( RCU_AF );
            rcu_periph_clock_enable( RCU_DMA0 );
            rcu_periph_clock_enable( RCU_SPI0 );

            gpio_init(GPIOA, GPIO_MODE_AF_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_5 | GPIO_PIN_7);
            gpio_init(GPIOB, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2);
            
            this -> rs_mode_data();
            this -> rst_active();	
            this -> cs_inactive();

            Longan_nano::Chrono::delay_ms( 1 );
            this -> rst_inactive();
            
            Longan_nano::Chrono::delay_ms( 5 );

            // Deinit SPI and DMA.
            spi_i2s_deinit(Config::SPI_CH);
            dma_deinit(DMA0, DMA_CH1);
            dma_deinit(DMA0, DMA_CH2);

            // Configure DMA, do not enable.
            DMA_CHCTL(DMA0, DMA_CH1) = (uint32_t)(DMA_PRIORITY_ULTRA_HIGH | DMA_CHXCTL_MNAGA);  // Receive.
            DMA_CHCTL(DMA0, DMA_CH2) = (uint32_t)(DMA_PRIORITY_ULTRA_HIGH | DMA_CHXCTL_DIR);    // Transmit.
            DMA_CHPADDR(DMA0, DMA_CH1) = (uint32_t)&SPI_DATA(Config::SPI_CH);
            DMA_CHPADDR(DMA0, DMA_CH2) = (uint32_t)&SPI_DATA(Config::SPI_CH);

            // Configure and enable SPI.
            SPI_CTL0(Config::SPI_CH) = (uint32_t)(SPI_MASTER | SPI_TRANSMODE_FULLDUPLEX | SPI_FRAMESIZE_8BIT | SPI_NSS_SOFT | SPI_ENDIAN_MSB | SPI_CK_PL_LOW_PH_1EDGE | SPI_PSC_8);
            SPI_CTL1(Config::SPI_CH) = (uint32_t)(SPI_CTL1_DMATEN);
            spi_enable(Config::SPI_CH);

            // Enable lcd controller.
            this -> cs_active();

            // Initialize the display.
            for (const uint8_t* p = this -> g_st7735s_init_sequence; *p != 0xff; p++)
            {
                this -> spi_wait_idle();
                this -> spi_set_8bit();
                this -> rs_mode_cmd();
                spi_i2s_data_transmit(Config::SPI_CH, *p++);
                
                if (*p == 0xff)
                {
                    continue;
                }
                
                this -> spi_wait_idle();
                this -> rs_mode_data();
                
                while(*p != 0xff)
                {
                    this -> spi_wait_tbe();
                    spi_i2s_data_transmit(Config::SPI_CH, *p++);
                }
            }

            //----------------------------------------------------------------
            //	RETURN
            //----------------------------------------------------------------
            
            return false;
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
        //! @return bool | false = OK | true = ERR
        //! @details
        //!	convert from 24b 8R8G8B space to 16bit 5R6G5B space
        //!         RRRRRRRR
        //!         GGGGGGGG
        //!         BBBBBBBB
        //!	RRRRRGGGGGGBBBBB
        /***************************************************************************/
        
        static uint16_t color( uint8_t r, uint8_t g, uint8_t b )
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
        //! @return int | number of pixels queued for draw
        //! @details
        //!	Ask the driver to draw a sprite
        //!	The draw method handles sprites that fill only part of the screen
        //!	If reworked, the buffer stays the same or is smaller than user buffer
        //!	1) Sprite fully inside screen area: the sprite is queued for draw
        //!	2) Sprite is fully outside screen area: no pixels are queued for draw
        //!	3) sprite is partially outside screen area: the method rework the buffer to include only pixel that can be drawn
        /***************************************************************************/
        
        int register_sprite( int origin_h, int origin_w, int size_h, int size_w, uint16_t* sprite_ptr )
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
        //! @return int | number of pixels queued for draw
        //! @details
        //!	Ask the driver to draw a sprite with a solid color
        //!	The draw method handles sprites that fill only part of the screen
        //!	If reworked, the buffer stays the same or is smaller than user buffer
        //!	1) Sprite fully inside screen area: the sprite is queued for draw
        //!	2) Sprite is fully outside screen area: no pixels are queued for draw
        //!	3) sprite is partially outside screen area: the method rework the buffer to include only pixel that can be drawn
        /***************************************************************************/
        
        int register_sprite( int origin_h, int origin_w, int size_h, int size_w, uint16_t sprite_color )
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
        //!	Execute a step of the FSM that interfaces with the physical display
        //!	Handle both solid color and color map
        //!	sprite data must already be valid before execution
        /***************************************************************************/

        bool update_sprite( void )
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
                    if ((SPI_STAT( Config::SPI_CH ) &SPI_STAT_TRANS) == 0)
                    {
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
                    if ((SPI_STAT( Config::SPI_CH ) &SPI_STAT_TRANS) == 0)
                    {
                        this -> spi_set_16bit();
                        this -> rs_mode_data();
                        spi_i2s_data_transmit(Config::SPI_CH, this -> g_sprite.origin_w +Config::ROW_ADDRESS_OFFSET);
                        //Next state
                        this -> g_sprite_status++;
                    }
                    break;
                }
                //Row Address stop
                case 3:
                {
                    if ((SPI_STAT( Config::SPI_CH ) &SPI_STAT_TBE) != 0)
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
                    if ((SPI_STAT( Config::SPI_CH ) &SPI_STAT_TRANS) == 0)
                    {
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
                    if ((SPI_STAT( Config::SPI_CH ) &SPI_STAT_TRANS) == 0)
                    {
                        this -> spi_set_16bit();
                        this -> rs_mode_data();
                        spi_i2s_data_transmit(Config::SPI_CH, this -> g_sprite.origin_h +Config::COL_ADDRESS_OFFSET);
                        //Next state
                        this -> g_sprite_status++;
                    }
                    break;
                }
                //Col Address Stop
                case 6:
                {
                    if ((SPI_STAT( Config::SPI_CH ) &SPI_STAT_TBE) != 0)
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
                    if ((SPI_STAT( Config::SPI_CH ) &SPI_STAT_TRANS) == 0)
                    {
                        this -> spi_set_8bit();
                        this -> rs_mode_cmd();
                        spi_i2s_data_transmit( Config::SPI_CH, Command::WRITE_MEM );
                        //Next state
                        this -> g_sprite_status++;
                    }
                    break;
                }
                //DMA Send
                case 8:
                {
                    if ((SPI_STAT( Config::SPI_CH ) &SPI_STAT_TRANS) == 0)
                    {
                        this -> rs_mode_data();
                        this -> spi_set_16bit();
                        
                        dma_channel_disable( Config::DMA_SPI_TX, (dma_channel_enum)Config::DMA_SPI_TX_CH );
                        dma_memory_width_config( Config::DMA_SPI_TX, (dma_channel_enum)Config::DMA_SPI_TX_CH, DMA_MEMORY_WIDTH_16BIT );
                        dma_periph_width_config( Config::DMA_SPI_TX, (dma_channel_enum)Config::DMA_SPI_TX_CH, DMA_PERIPHERAL_WIDTH_16BIT );
                        
                        //If: the sprite uses a color map
                        if (this -> g_sprite.b_solid_color == false)
                        {
                            dma_memory_address_config( Config::DMA_SPI_TX, (dma_channel_enum)Config::DMA_SPI_TX_CH, (uint32_t)(this -> g_sprite.sprite_ptr) );
                            dma_memory_increase_enable( Config::DMA_SPI_TX, (dma_channel_enum)Config::DMA_SPI_TX_CH );
                        }	//End If: the sprite uses a color map
                        //If: the sprite is solid color
                        else //if (this -> g_sprite.b_solid_color == true)
                        {
                            dma_memory_address_config( Config::DMA_SPI_TX, (dma_channel_enum)Config::DMA_SPI_TX_CH, (uint32_t)(&g_sprite.solid_color) );
                            dma_memory_increase_disable( Config::DMA_SPI_TX, (dma_channel_enum)Config::DMA_SPI_TX_CH );
                        }	//End If: the sprite is solid color
                        
                        dma_transfer_number_config( Config::DMA_SPI_TX, (dma_channel_enum)Config::DMA_SPI_TX_CH, this -> g_sprite.size );
                        dma_channel_enable( Config::DMA_SPI_TX, (dma_channel_enum)Config::DMA_SPI_TX_CH );
                        
                        //Next state
                        this -> g_sprite_status++;
                    }
                    break;
                }			
                //STOP
                case 9:
                {
                    if ((SPI_STAT( Config::SPI_CH ) &SPI_STAT_TRANS) == 0)
                    {
                        //Return to IDLE
                        this -> g_sprite_status = 0;
                    }
                    break;
                }
                //ERR
                default:
                {
                    //Return to IDLE
                    this -> g_sprite_status = 0;
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
        //! @return int | number of pixels drawn
        //! @details
        //!	Draw a sprite
        //! Blocking method
        //! Draw a color map, requires a user buffer of the proper size
        /***************************************************************************/

        int draw_sprite( int origin_h, int origin_w, int size_h, int size_w, uint16_t* sprite_ptr )
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
        //! @return int | number of pixels drawn
        //! @details
        //!	Draw a sprite
        //! Blocking method
        //! Draw a solid color sprite and doesn't require a color map
        /***************************************************************************/

        int draw_sprite( int origin_h, int origin_w, int size_h, int size_w, uint16_t sprite_color )
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
        //!	Clear the screen. Blocking method.
        //!	Draw a black sprite the size of the screen
        /***************************************************************************/

        int clear( void )
        {
            //----------------------------------------------------------------
            //	BODY
            //----------------------------------------------------------------

            int pixel_count = this -> draw_sprite( 0, 0, Config::HEIGHT, Config::WIDTH, Color::BLACK );

            //----------------------------------------------------------------
            //	RETURN
            //----------------------------------------------------------------

            return pixel_count;
        }	//End public method: clear | void |

        /***************************************************************************/
        //!	@brief public method
        //!	clear | void |
        /***************************************************************************/
        //! @return int | number of pixels drawn
        //! @details
        //!	Clear the screen. Blocking method.
        //!	Draw a black sprite the size of the screen
        /***************************************************************************/

        int clear( uint16_t color )
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


    private:
        /*********************************************************************************************************************************************************
        **********************************************************************************************************************************************************
        **	PRIVATE ENUM
        **********************************************************************************************************************************************************
        *********************************************************************************************************************************************************/
        
        //ST7735S Commands
        typedef enum _Command
        {
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
        
        //Sprite informations
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

        /***************************************************************************/
        //!	@brief Private HAL Method
        //!	init_gpio | void
        /***************************************************************************/
        //! @return bool | false = BUSY | true = IDLE
        //! @details
        //!	HAL method
        //!	initialize GPIO configuration
        /***************************************************************************/
        
        void init_gpio( void )
        {
            //----------------------------------------------------------------
            //	RETURN
            //----------------------------------------------------------------
            
            return;
        }	//End Private HAL Method: init_gpio | void
        
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
        //!	HAL method
        //!	return true when the SPI is IDLE
        /***************************************************************************/
        
        inline bool is_spi_idle( void )
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
        //!	HAL method
        //!	Block execution until SPI is IDLE
        /***************************************************************************/
        
        inline void spi_wait_idle( void )
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

        inline void spi_wait_tbe( void )
        {
            while( ( SPI_STAT( Config::SPI_CH ) &SPI_STAT_TBE ) == 0 )
            {
                //Block Execution
            }
            return;
        }

        inline void cs_active( void )
        {
            gpio_bit_reset(Config::SPI_CS_GPIO,Config::SPI_CS_PIN);
            return;
        }

        inline void cs_inactive( void )
        {
            gpio_bit_set(Config::SPI_CS_GPIO,Config::SPI_CS_PIN);
            return;
        }

        inline void rs_mode_cmd( void )
        {
            gpio_bit_reset( Config::RS_GPIO, Config::RS_PIN );
            return;
        }

        inline void rs_mode_data( void )
        {
            gpio_bit_set( Config::RS_GPIO, Config::RS_PIN );
            return;
        }
        
        /***************************************************************************/
        //!	@brief Private HAL Method
        //!	rst_active | void |
        /***************************************************************************/
        //! @details
        //!	HAL method
        //!	Assert the reset pin of the physical display
        //!	The display will need time to become ready after an assert
        /***************************************************************************/
        
        inline void rst_active( void )
        {
            //----------------------------------------------------------------
            //	BODY
            //----------------------------------------------------------------
            
            gpio_bit_reset(Config::RST_GPIO, Config::RST_PIN);
            
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
        //!	HAL method
        //!	Deassert the reset pin of the physical display
        /***************************************************************************/
        
        inline void rst_inactive( void )
        {
            //----------------------------------------------------------------
            //	BODY
            //----------------------------------------------------------------
            
            gpio_bit_set(Config::RST_GPIO, Config::RST_PIN);
            
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
        //!	HAL method
        //!	Configure the SPI to 8b
        /***************************************************************************/		
        
        void spi_set_8bit()
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
        //!	HAL method
        //!	Configure the SPI to 8b
        /***************************************************************************/

        void spi_set_16bit()
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

        /*********************************************************************************************************************************************************
        **********************************************************************************************************************************************************
        **	PRIVATE METHODS
        **********************************************************************************************************************************************************
        *********************************************************************************************************************************************************/

        /*********************************************************************************************************************************************************
        **********************************************************************************************************************************************************
        **	PRIVATE VARS
        **********************************************************************************************************************************************************
        *********************************************************************************************************************************************************/

        //ST7735S initialization sequence. Stored in flash memory
        static constexpr uint8_t g_st7735s_init_sequence[] =
        {
            Command::ENABLE_DISPLAY_INVERSION, Command::TERMINATOR,
            0xb1, 0x05, 0x3a, 0x3a, Command::TERMINATOR,
            0xb2, 0x05, 0x3a, 0x3a, Command::TERMINATOR,
            0xb3, 0x05, 0x3a, 0x3a, 0x05, 0x3a, 0x3a, Command::TERMINATOR,
            0xb4, 0x03, Command::TERMINATOR,
            0xc0, 0x62, 0x02, 0x04, Command::TERMINATOR,
            0xc1, 0xc0, Command::TERMINATOR,
            0xc2, 0x0d, 0x00, Command::TERMINATOR,
            0xc3, 0x8d, 0x6a, Command::TERMINATOR,
            0xc4, 0x8d, 0xee, Command::TERMINATOR,
            0xc5, 0x0e, Command::TERMINATOR,
            0xe0, 0x10, 0x0e, 0x02, 0x03, 0x0e, 0x07, 0x02, 0x07, 0x0a, 0x12, 0x27, 0x37, 0x00, 0x0d, 0x0e, 0x10, Command::TERMINATOR,
            0xe1, 0x10, 0x0e, 0x03, 0x03, 0x0f, 0x06, 0x02, 0x08, 0x0a, 0x13, 0x26, 0x36, 0x00, 0x0d, 0x0e, 0x10, Command::TERMINATOR,
            0x3a, 0x55, Command::TERMINATOR,
            0x36, 0x78, Command::TERMINATOR,
            0x29, Command::TERMINATOR,
            0x11, Command::TERMINATOR,
            Command::TERMINATOR,
        };

        //Sprite
        Sprite g_sprite;
        //FSM status
        uint8_t g_sprite_status;
        //Record errors | false = OK | true = ERR |
        //bool g_err;

    //--------------------------------------------------------------------------
    //	End Private
    //--------------------------------------------------------------------------
};	//End class: Lcd

/**********************************************************************************
**	NAMESPACE
**********************************************************************************/

} //End namespace: Longan_nano

#else
    #warning "Multiple inclusion of hader file ST7735S_W160_H80_C16_HPP_"
#endif
