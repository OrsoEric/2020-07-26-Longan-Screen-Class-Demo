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

#ifndef EMBEDDED_STRING_
    #define EMBEDDED_STRING_

/**********************************************************************************
**	GLOBAL INCLUDES
**********************************************************************************/

#include <stdint.h>

/**********************************************************************************
**	DEFINES
**********************************************************************************/

//Enable the file trace debugger
//#define ENABLE_DEBUG
//file trace debugger
#ifdef ENABLE_DEBUG
    #include <cstdio>
    #include "debug.h"
#endif
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
#endif

/**********************************************************************************
**	MACROS
**********************************************************************************/

/**********************************************************************************
**	NAMESPACE
**********************************************************************************/

//! @namespace User utilities
namespace User
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
//! @class 		String
/************************************************************************************/
//!	@author		Orso Eric
//! @version	2020-07-27
//! @brief		String <-> Number conversion using standard c strings to avoid std::string under the hood
//! @pre		None
//! @bug		None
//! @warning	None
//! @todo		todo list
//! @copyright	BSD 3-Clause License Copyright (c) 2020, Orso Eric
//! @details
//!	    2020-07-27
//! Encapsulate in class form
//! Transplant u8, s8, u16, s16, u32 and s32 methods from the 2019-11-07 At_string V2 library
//!	Added safety checks and option to disable them in release
//!	Use C++ overloading and make use of num_to_str method. No longer calling the wrong conversion method
//!	Tested U8 and S8 methods. Test safety by providing shorter string: SUCCESS
//!	U16 and S16 methods can call the U8 conversion when number is small enough
//! Tested U16 and S16 methods: SUCCESS
//!	Tested U32 and S32. The test example only test S16 as there are problems with %d
//!	Engineering format string
//!		2020-07-29
//!	Tested signed and unsigned integer engineering format string generation
//! Added base 10^X exponent to eng conversion. It's meant to allow user to specify a multiplier when using inegers. e.g. 1000 means 100.0%
/************************************************************************************/

class String
{
    //Visible to all
    public:
        /*********************************************************************************************************************************************************
        **********************************************************************************************************************************************************
        **	CONSTRUCTORS
        **********************************************************************************************************************************************************
        *********************************************************************************************************************************************************/

        /***************************************************************************/
        //!	@brief Constructor
        //!	String | void
        /***************************************************************************/
        // @param
        //! @return no return
        //!	@details
        //! Empty constructor
        /***************************************************************************/

        String( void )
        {
            DENTER();		//Trace Enter
            ///--------------------------------------------------------------------------
            ///	RETURN
            ///--------------------------------------------------------------------------
            DRETURN();		//Trace Return
            return;			//OK
        }	//end constructor: String | void

        /*********************************************************************************************************************************************************
        **********************************************************************************************************************************************************
        **	DESTRUCTORS
        **********************************************************************************************************************************************************
        *********************************************************************************************************************************************************/

        /***************************************************************************/
        //!	@brief Destructor
        //!	String | void
        /***************************************************************************/
        // @param
        //! @return no return
        //!	@details
        //! Empty destructor
        /***************************************************************************/

        ~String( void )
        {
            DENTER();		//Trace Enter
            ///--------------------------------------------------------------------------
            ///	RETURN
            ///--------------------------------------------------------------------------
            DRETURN();		//Trace Return
            return;			//OK
        }	//end destructor:	String | void

        /*********************************************************************************************************************************************************
        **********************************************************************************************************************************************************
        **	PUBLIC ENUM
        **********************************************************************************************************************************************************
        *********************************************************************************************************************************************************/

        typedef enum _Config
        {
            //Safety checks. Can be disabled for performance in production
            SAFETY_CHECKS = true,
            //Maximum digits for the various signed and unsigned types
            DIGIT8 = 3,
            DIGIT16 = 5,
            DIGIT32 = 10,
            //DIGIT64 = 20,
            //Engineering format has four meaningful digits
            DIGIT_ENG = 4,
            //Size of special characters
            STRING_SIGN_SIZE = 1,
            STRING_TERMINATOR_SIZE = 1,
            //Special characters
            TERMINATOR = '\0',
            //Maximum size of strings of various types
            STRING_SIZE_U8 = DIGIT8 +STRING_TERMINATOR_SIZE,
            STRING_SIZE_S8 = DIGIT8 +STRING_SIGN_SIZE +STRING_TERMINATOR_SIZE,
            STRING_SIZE_U16 = DIGIT16 +STRING_TERMINATOR_SIZE,
            STRING_SIZE_S16 = DIGIT16 +STRING_SIGN_SIZE +STRING_TERMINATOR_SIZE,
            STRING_SIZE_U32 = DIGIT32 +STRING_TERMINATOR_SIZE,
            STRING_SIZE_S32 = DIGIT32 +STRING_SIGN_SIZE +STRING_TERMINATOR_SIZE,
            //STRING_SIZE_U64 = DIGIT64 +STRING_TERMINATOR_SIZE,
            //STRING_SIZE_S64 = DIGIT64 +STRING_SIGN_SIZE +STRING_TERMINATOR_SIZE,
            //Size of an enginnering format string
            STRING_SIZE_UENG = 7,
            STRING_SIZE_SENG = 8,
        } Config;

        /*********************************************************************************************************************************************************
        **********************************************************************************************************************************************************
        **	PUBLIC STATIC METHODS
        **********************************************************************************************************************************************************
        *********************************************************************************************************************************************************/

        /***************************************************************************/
        //!	@brief public static method
        //!	num_to_str | uint8_t | uint8_t | char * |
        /***************************************************************************/
        //! @param num		| uint8_t | number to be converted
        //!	@param str_len	| uint8_t | length of the provided string. Avoids overflow. Include terminator. "XXX\0" str_len = 4
        //! @param str		| const char * | return string provied by the caller
        //! @return uint8_t	| number of digits written in the string E.g. writing "255" returns 3. writing "0" returns 1 | 0 means no digits were written: fail
        //! @details
        //! convert a U8 into a string
        /***************************************************************************/

        static uint8_t num_to_str( uint8_t num, uint8_t str_len, char *str )
        {
            DENTER_ARG("U8 num: %d\n", num);
            //----------------------------------------------------------------
            //	CHECKS
            //----------------------------------------------------------------

            //If: string is invalid
            if ((SAFETY_CHECKS == true) && ((str == nullptr) || (str_len == 0)))
            {
                //Bad parameters
                return 0;
            }

            //----------------------------------------------------------------
            //	VARS
            //----------------------------------------------------------------

            //decimal base
            const uint8_t base[Config::DIGIT8] =
            {
                100,
                10,
                1
            };
            //Fast counter
            uint8_t t, tmp;
            //index to the string
            uint8_t index = 0;
            //flag used to blank non meaningful zeros
            bool flag = true;

            //----------------------------------------------------------------
            //	BODY
            //----------------------------------------------------------------

            //For all bases
            for (t = 0;t < Config::DIGIT8; t++)
            {
                //If the base is bigger or equal than the number (division is meaningful)
                if (base[t] <= num)
                {
                    //Divide number by base, get the digit. Bounded to 0 to 9.
                    tmp = num / base[t];
                    //If: index would overflow. Need an extra slot for the final teminator
                    if ( (SAFETY_CHECKS == true) && (index >= (str_len -Config::STRING_TERMINATOR_SIZE)) )
                    {
                        //Put a terminator in first position for safety
                        str[0] = Config::TERMINATOR;
                        //Overflow occurred. Return 0 digits written
                        DRETURN_ARG("ERR: Not enough space: %d\n", str_len);
                        return 0;
                    }
                    //Write the digit
                    str[ index ] = '0' +tmp;
                    //Update the number
                    num = num - base[t] * tmp;
                    //I have found a meaningful digit
                    flag = false;
                    //Jump to the next digit
                    index++;
                }
                //If: The base is smaller then the number, and I have yet to find a non zero digit, and I'm not to the last digit
                else if ( (flag == true) && (t != (Config::DIGIT8 -1)) )
                {
                    //do nothing
                }
                //If: I have a meaningful zero
                else
                {
                    //If: index would overflow. Need an extra slot for the final teminator
                    if ( (SAFETY_CHECKS == true) && (index >= (str_len -Config::STRING_TERMINATOR_SIZE)) )
                    {
                        //Put a terminator in first position for safety
                        str[0] = Config::TERMINATOR;
                        //Overflow occurred. Return 0 digits written
                        DRETURN_ARG("ERR: Not enough space: %d\n", str_len);
                        return 0;
                    }
                    //It's a zero
                    str[ index ] = '0';
                    //Jump to the next digit
                    index++;
                }
            }	//End for: all bases

            //----------------------------------------------------------------
            //	RETURN
            //----------------------------------------------------------------

            //Append the terminator
            str[ index ] = Config::TERMINATOR;
            DRETURN_ARG("digits written: %d | output >%s<\n", index, str);
            return index;
        }   //End public static method: num_to_str | uint8_t | uint8_t | char * |

        /***************************************************************************/
        //!	@brief public static method
        //!	num_to_str | int8_t | uint8_t | char * |
        /***************************************************************************/
        //! @param num		| int8_t | number to be converted
        //!	@param str_len	| uint8_t | length of the provided string. Avoids overflow. Include terminator. "XXX\0" str_len = 4
        //! @param str		| const char * | return string provied by the caller
        //! @return uint8_t	| number of digits written in the string
        //! @details
        //!	convert a S8 into a string. Mostly does sign correction. Make use of the U8 method for the actual conversion.
        /***************************************************************************/

        static uint8_t num_to_str( int8_t num, uint8_t str_len, char *str )
        {
            DENTER_ARG("S8 num: %d\n", num);
            //----------------------------------------------------------------
            //	VARS
            //----------------------------------------------------------------

            //number of character written
            uint8_t ret;
            uint8_t positive;

            //----------------------------------------------------------------
            //	INIT
            //----------------------------------------------------------------

            //If: string is invalid
            if ((SAFETY_CHECKS == true) && ((str == nullptr) || (str_len == 0)))
            {
                //Bad parameters
                DRETURN_ARG("ERR: bad parameters\n");
                return 0;
            }

            //----------------------------------------------------------------
            //	BODY
            //----------------------------------------------------------------

            //If: negative
            if (num < 0)
            {
                //Write sign '-'
                str[ 0 ] = '-';
                //Correct sign
                positive = -num;

            }
            //If: zero or positive
            else
            {
                //Write sign '+'
                str[ 0 ] = '+';
                positive = num;
            }
            //launch the num_to_str to the corrected num, but feed the vector shifted by 1 to make room for the sign. save the return value
            ret = num_to_str( (uint8_t)positive, str_len -Config::STRING_SIGN_SIZE, &str[Config::STRING_SIGN_SIZE] );
            //If: no digits were written
            if (ret == 0)
            {
                //Add a terminator for safety
                str[0] = '\0';
                DRETURN_ARG("ERR: no digits were written\n");
                return 0;   //No point in adding a sign to an empty string: fail
            }

            //----------------------------------------------------------------
            //	RETURN
            //----------------------------------------------------------------
            DRETURN_ARG("digits written: %d | output >%s<\n", ret +Config::STRING_SIGN_SIZE, str);
            return (ret +Config::STRING_SIGN_SIZE);
        } //End public static method: num_to_str | int8_t | uint8_t | const char * |

        /***************************************************************************/
        //!	@brief public static method
        //!	num_to_str | uint16_t | uint8_t | char * |
        /***************************************************************************/
        //! @param num		| uint16_t | number to be converted
        //!	@param str_len	| uint8_t | length of the provided string. Avoids overflow. Include terminator. "XXX\0" str_len = 4
        //! @param str		| const char * | return string provied by the caller
        //! @return uint8_t	| number of digits written in the string E.g. writing "255" returns 3. writing "0" returns 1 | 0 means no digits were written: fail
        //! @details
        //!	convert a U16 into a string. Make use of the U8 method for small numbers
        /***************************************************************************/

        static uint8_t num_to_str( uint16_t num, uint8_t str_len, char *str )
        {
            DENTER_ARG("U16 num: %d\n", num);
            //----------------------------------------------------------------
            //	CHECKS
            //----------------------------------------------------------------

            //If: string is invalid
            if ((SAFETY_CHECKS == true) && ((str == nullptr) || (str_len == 0)))
            {
                //Bad parameters
                return 0;
            }
            //If: number is small enough
            if (num <= UINT8_MAX)
            {
                //Call the faster U8 method
                uint8_t ret = num_to_str( (uint8_t)num, str_len, str );
                DRETURN_ARG("digits written: %d\n", ret);
                return ret;
            }

            //----------------------------------------------------------------
            //	VARS
            //----------------------------------------------------------------

            //decimal base
            const uint16_t base[Config::DIGIT16] =
            {
                10000,
                1000,
                100,
                10,
                1
            };
            //Fast counter
            uint8_t t, tmp;
            //index to the string
            uint8_t index = 0;
            //flag used to blank non meaningful zeros
            bool flag = true;

            //----------------------------------------------------------------
            //	BODY
            //----------------------------------------------------------------

            //For all bases
            for (t = 0;t < Config::DIGIT16; t++)
            {
                //If the base is bigger or equal than the number (division is meaningful)
                if (base[t] <= num)
                {
                    //Divide number by base, get the digit. Bounded to 0 to 9.
                    tmp = num / base[t];
                    //If: index would overflow. Need an extra slot for the final teminator
                    if ( (SAFETY_CHECKS == true) && (index >= (str_len -Config::STRING_TERMINATOR_SIZE)) )
                    {
                        //Put a terminator in first position for safety
                        str[0] = Config::TERMINATOR;
                        //Overflow occurred. Return 0 digits written
                        DRETURN_ARG("ERR: Not enough space: %d\n", str_len);
                        return 0;
                    }
                    //Write the digit
                    str[ index ] = '0' +tmp;
                    //Update the number
                    num = num - base[t] * tmp;
                    //I have found a meaningful digit
                    flag = false;
                    //Jump to the next digit
                    index++;
                }
                //If: The base is smaller then the number, and I have yet to find a non zero digit, and I'm not to the last digit
                else if ( (flag == true) && (t != (Config::DIGIT16 -1)) )
                {
                    //do nothing
                }
                //If: I have a meaningful zero
                else
                {
                    //If: index would overflow. Need an extra slot for the final teminator
                    if ( (SAFETY_CHECKS == true) && (index >= (str_len -Config::STRING_TERMINATOR_SIZE)) )
                    {
                        //Put a terminator in first position for safety
                        str[0] = Config::TERMINATOR;
                        //Overflow occurred. Return 0 digits written
                        DRETURN_ARG("ERR: Not enough space: %d\n", str_len);
                        return 0;
                    }
                    //It's a zero
                    str[ index ] = '0';
                    //Jump to the next digit
                    index++;
                }
            }	//End for: all bases

            //----------------------------------------------------------------
            //	RETURN
            //----------------------------------------------------------------

            //Append the terminator
            str[ index ] = Config::TERMINATOR;
            DRETURN_ARG("digits written: %d | output >%s<\n", index, str);
            return index;
        }   //End public static method: num_to_str | uint16_t | uint8_t | char * |

        /***************************************************************************/
        //!	@brief public static method
        //!	num_to_str | int16_t | uint8_t | const char * |
        /***************************************************************************/
        //! @param num		| int16_t | number to be converted
        //!	@param str_len	| uint8_t | length of the provided string. Avoids overflow. Include terminator. "XXX\0" str_len = 4
        //! @param str		| const char * | return string provied by the caller
        //! @return uint8_t	| number of digits written in the string
        //! @brief Convert an int16_t to string
        //! @details
        //!	convert a S16 into a string. Does mostly sign conversion. Make use of the U8 and U16 methods for the actual conversion.
        /***************************************************************************/

        static uint8_t num_to_str( int16_t num, uint8_t str_len, char *str )
        {
            DENTER_ARG("S16 num: %d\n", num);
            //----------------------------------------------------------------
            //	VARS
            //----------------------------------------------------------------

            //number of character written
            uint8_t ret;
            uint16_t positive;

            //----------------------------------------------------------------
            //	INIT
            //----------------------------------------------------------------

            //If: string is invalid
            if ((SAFETY_CHECKS == true) && ((str == nullptr) || (str_len == 0)))
            {
                //Bad parameters
                DRETURN_ARG("ERR: bad parameters\n");
                return 0;
            }

            //----------------------------------------------------------------
            //	BODY
            //----------------------------------------------------------------

            //If: negative
            if (num < 0)
            {
                //Write sign '-'
                str[ 0 ] = '-';
                //Correct sign
                positive = -num;

            }
            //If: zero or positive
            else
            {
                //Write sign '+'
                str[ 0 ] = '+';
                positive = num;
            }

            //If: number is small enough. Note that -255 doesn't fit 8bit but can still be converted by U8 num_to_str because the numeric part does fit 8bit
            if (positive < UINT8_MAX)
            {
                //launch the U8 num_to_str to the corrected num, but feed the vector shifted by 1 to make room for the sign. save the return value
                ret = num_to_str( (uint8_t)positive, str_len -Config::STRING_SIGN_SIZE, &str[Config::STRING_SIGN_SIZE] );
            }
            else
            {
                //Full sized num_to_str
                ret = num_to_str( (uint16_t)positive, str_len -Config::STRING_SIGN_SIZE, &str[Config::STRING_SIGN_SIZE] );
            }
            //If: no digits were written
            if (ret == 0)
            {
                //Add a terminator for safety
                str[0] = '\0';
                DRETURN_ARG("ERR: no digits were written\n");
                return 0;   //No point in adding a sign to an empty string: fail
            }

            //----------------------------------------------------------------
            //	RETURN
            //----------------------------------------------------------------
            DRETURN_ARG("digits written: %d | output >%s<\n", ret +Config::STRING_SIGN_SIZE, str);
            return (ret +Config::STRING_SIGN_SIZE);
        } //End public static method: num_to_str | int16_t | uint8_t | char * |

        /***************************************************************************/
        //!	@brief public static method
        //!	num_to_str | uint32_t | uint8_t | char * |
        /***************************************************************************/
        //! @param num		| uint32_t | number to be converted
        //!	@param str_len	| uint8_t | length of the provided string. Avoids overflow. Include terminator. "XXX\0" str_len = 4
        //! @param str		| const char * | return string provied by the caller
        //! @return uint8_t	| number of digits written in the string E.g. writing "255" returns 3. writing "0" returns 1 | 0 means no digits were written: fail
        //! @details
        //! convert a U32 into a string
        /***************************************************************************/

        static uint8_t num_to_str( uint32_t num, uint8_t str_len, char *str )
        {
            DENTER_ARG("U32 num: %ld\n", (uint64_t)num);
            //----------------------------------------------------------------
            //	CHECKS
            //----------------------------------------------------------------

            //If: string is invalid
            if ((SAFETY_CHECKS == true) && ((str == nullptr) || (str_len == 0)))
            {
                //Bad parameters
                return 0;
            }
            //If: number is small enough for U8
            if (num <= UINT8_MAX)
            {
                //Call the faster U8 method
                uint8_t ret = num_to_str( (uint8_t)num, str_len, str );
                DRETURN_ARG("digits written: %d\n", ret);
                return ret;
            }
            //If: number is small enough for U16
            else if (num <= UINT16_MAX)
            {
                //Call the faster 16 method
                uint8_t ret = num_to_str( (uint16_t)num, str_len, str );
                DRETURN_ARG("digits written: %d\n", ret);
                return ret;
            }

            //----------------------------------------------------------------
            //	VARS
            //----------------------------------------------------------------

            //decimal base
            const uint32_t base[Config::DIGIT32] =
            {
                1000000000,
                100000000,
                10000000,
                1000000,
                100000,
                10000,
                1000,
                100,
                10,
                1
            };
            //Fast counter
            uint8_t t, tmp;
            //index to the string
            uint8_t index = 0;
            //flag used to blank non meaningful zeros
            bool flag = true;

            //----------------------------------------------------------------
            //	BODY
            //----------------------------------------------------------------

            //For all bases
            for (t = 0;t < Config::DIGIT32; t++)
            {
                //If the base is bigger or equal than the number (division is meaningful)
                if (base[t] <= num)
                {
                    //Divide number by base, get the digit. Bounded to 0 to 9.
                    tmp = num / base[t];
                    //If: index would overflow. Need an extra slot for the final teminator
                    if ( (SAFETY_CHECKS == true) && (index >= (str_len -Config::STRING_TERMINATOR_SIZE)) )
                    {
                        //Put a terminator in first position for safety
                        str[0] = Config::TERMINATOR;
                        //Overflow occurred. Return 0 digits written
                        DRETURN_ARG("ERR: Not enough space: %d\n", str_len);
                        return 0;
                    }
                    //Write the digit
                    str[ index ] = '0' +tmp;
                    //Update the number
                    num = num - base[t] * tmp;
                    //I have found a meaningful digit
                    flag = false;
                    //Jump to the next digit
                    index++;
                }
                //If: The base is smaller then the number, and I have yet to find a non zero digit, and I'm not to the last digit
                else if ( (flag == true) && (t != (Config::DIGIT32 -1)) )
                {
                    //do nothing
                }
                //If: I have a meaningful zero
                else
                {
                    //If: index would overflow. Need an extra slot for the final teminator
                    if ( (SAFETY_CHECKS == true) && (index >= (str_len -Config::STRING_TERMINATOR_SIZE)) )
                    {
                        //Put a terminator in first position for safety
                        str[0] = Config::TERMINATOR;
                        //Overflow occurred. Return 0 digits written
                        DRETURN_ARG("ERR: Not enough space: %d\n", str_len);
                        return 0;
                    }
                    //It's a zero
                    str[ index ] = '0';
                    //Jump to the next digit
                    index++;
                }
            }	//End for: all bases

            //----------------------------------------------------------------
            //	RETURN
            //----------------------------------------------------------------

            //Append the terminator
            str[ index ] = Config::TERMINATOR;
            DRETURN_ARG("digits written: %d | output >%s<\n", index, str);
            return index;
        }   //End public static method: num_to_str | uint32_t | const char * |

        /***************************************************************************/
        //!	@brief public static method
        //!	num_to_str | int32_t | uint8_t | const char * |
        /***************************************************************************/
        //! @param num		| int32_t | number to be converted
        //!	@param str_len	| uint8_t | length of the provided string. Avoids overflow. Include terminator. "XXX\0" str_len = 4
        //! @param str		| const char * | return string provied by the caller
        //! @return uint8_t	| number of digits written in the string
        //! @details
        //!	convert a S32 into a string. Does mostly sign conversion. Make use of the U8 and U16 methods for the actual conversion.
        /***************************************************************************/

        static uint8_t num_to_str( int32_t num, uint8_t str_len, char *str )
        {
            DENTER_ARG("S32 num: %d\n", num);
            //----------------------------------------------------------------
            //	VARS
            //----------------------------------------------------------------

            //number of character written
            uint8_t ret;
            uint32_t positive;

            //----------------------------------------------------------------
            //	INIT
            //----------------------------------------------------------------

            //If: string is invalid
            if ((SAFETY_CHECKS == true) && ((str == nullptr) || (str_len == 0)))
            {
                //Bad parameters
                DRETURN_ARG("ERR: bad parameters\n");
                return 0;
            }

            //----------------------------------------------------------------
            //	BODY
            //----------------------------------------------------------------

            //If: negative
            if (num < 0)
            {
                //Write sign '-'
                str[ 0 ] = '-';
                //Correct sign
                positive = -num;

            }
            //If: zero or positive
            else
            {
                //Write sign '+'
                str[ 0 ] = '+';
                positive = num;
            }

            //If: number is small enough. Note that -255 doesn't fit 8bit but can still be converted by U8 num_to_str because the numeric part does fit 8bit
            if (positive < UINT8_MAX)
            {
                //launch the U8 num_to_str to the corrected num, but feed the vector shifted by 1 to make room for the sign. save the return value
                ret = num_to_str( (uint8_t)positive, str_len -Config::STRING_SIGN_SIZE, &str[Config::STRING_SIGN_SIZE] );
            }
            //If small enough
            else if (positive < UINT16_MAX)
            {
                //Faster 16b method
                ret = num_to_str( (uint16_t)positive, str_len -Config::STRING_SIGN_SIZE, &str[Config::STRING_SIGN_SIZE] );
            }
            //If: full size
            else
            {
                //Full sized num_to_str
                ret = num_to_str( (uint32_t)positive, str_len -Config::STRING_SIGN_SIZE, &str[Config::STRING_SIGN_SIZE] );
            }
            //If: no digits were written
            if (ret == 0)
            {
                //Add a terminator for safety
                str[0] = '\0';
                DRETURN_ARG("ERR: no digits were written\n");
                return 0;   //No point in adding a sign to an empty string: fail
            }

            //----------------------------------------------------------------
            //	RETURN
            //----------------------------------------------------------------
            DRETURN_ARG("digits written: %d | output >%s<\n", ret +Config::STRING_SIGN_SIZE, str);
            return (ret +Config::STRING_SIGN_SIZE);
        } //End public static method: num_to_str | int32_t | uint8_t | char * |

        /***************************************************************************/
        //!	@brief public static method
        //!	num_to_eng | uint32_t | uint8_t | char * |
        /***************************************************************************/
        //! @param num		| int32_t | number to be converted
        //! @param exp		| int8_t | base 10^x eponent of the number. E.G. the user might have provided a number in millivolts, exponent -3
        //!	@param str_len	| uint8_t | length of the provided string. Avoids overflow. Include terminator. Must be at least 7
        //! @param str		| const char * | return string provied by the caller
        //! @return uint8_t	| number of digits written in the string, not including terminator
        //! @details
        //!	convert a S32 into an enginnering number format. Four significant digits with SI suffix
        //!	always uses 6 characters
        //!	X.XXX{SI}
        //!	XX.XX{SI}
        //!	XXX.X{SI}
        //!	SI Suffixes
        //!	Exponents
        //!	index	|	-6	-5	-4	-3	-2	-1	0	+1	+2	+3	+4	+5	+6
        //!	exp		|	-18	-15	-12	-9	-6	-3	0	+3	+6	+9	+12	+15	+18
        //!	suffix	|	a	f	p	n	u	m		K	M	G	T	P	E
        /***************************************************************************/

        static uint8_t num_to_eng( uint32_t num, int8_t num_exp, uint8_t str_len, char *str )
        {
            DENTER_ARG("U32 num: %zu | base exponent 10^%d\n", num, num_exp);
            //----------------------------------------------------------------
            //	CHECK
            //----------------------------------------------------------------
            //If: bad string
            if ((Config::SAFETY_CHECKS == true) && ((str == nullptr) || (str_len < Config::STRING_SIZE_UENG)) )
            {
                DRETURN_ARG("bad input string: %d\n", str_len);
                return 0;
            }
            //If: 0
            if (num == 0)
            {
                //Handle it as special case ssince it would require a change in the algorithm
                str[0] = '0';
                str[1] = '.';
                str[2] = '0';
                str[3] = '0';
                str[4] = '0';
                str[5] = ' ';
                str[6] = Config::TERMINATOR;
                return Config::STRING_SIZE_UENG;
            }

            //----------------------------------------------------------------
            //	PRECISION DECODER
            //----------------------------------------------------------------
            //	Get to an integer number with at most four significant digits

            //How many position was the number shifted
            int8_t shift = 0;

            bool f_continue = true;
            //Number has too many digits
            while (f_continue == true)
            {
                //If: more than five digits
                if (num >= 100000)
                {
                    //Divide by 10 and take into account digits shifted away
                    num /= 10;
                    shift++;

                }
                //If: more than four digits
                else if (num >= 10000)
                {
                    //Compute division
                    int16_t num_tmp = num / 10;
                    //I'm shifting away a number bigger than 5
                    if (num %10 >=5)
                    {
                        //Round up
                        num_tmp++;
                    }
                    //Save
                    num = num_tmp;
                    shift++;

                }
                //if: fewer than four digits
                else if (num < 1000)
                {
                    num *= 10;
                    shift--;
                }
                else
                {
                    f_continue = false;
                }
            }
            //DPRINT("num: %d | shift: %d\n", num, shift);
            //Take into account the base exponent of the number provided by the user
            shift = shift +num_exp;

            //Algorithm to decode point and si suffix from the shift level. Number is always four digits
            int8_t point;
            uint8_t si_suffix_index;
            if (shift < 0)
            {
                point = (3- ((-shift-1) %3));
                si_suffix_index = ((shift+1)/3) +6;
            }
            else
            {
                point = 1+ ((shift) %3);
                si_suffix_index = (shift+3)/3 +6;
            }
            //exponents suffix string
            const char *suffix = "afpnum KMGTPE";
            //Construct SI suffix
            str[5] = suffix[ si_suffix_index ];
            //DPRINT("shift: %d, point: %d | si exp: %d\n", shift, point, si_suffix_index);

            //----------------------------------------------------------------
            //	NUMERIC STRING
            //----------------------------------------------------------------

            //decimal base
            const uint16_t base[Config::DIGIT_ENG] =
            {
                1000,
                100,
                10,
                1
            };
            //index to the output string
            uint8_t index = 0;
            uint8_t tmp;
            //For all bases
            for (uint8_t t = 0;t < Config::DIGIT_ENG; t++)
            {
                //If the base is bigger or equal than the number (division is meaningful)
                if (base[t] <= num)
                {
                    //Divide number by base, get the digit. Bounded to 0 to 9.
                    tmp = num / base[t];
                    //Write the digit
                    str[ index ] = '0' +tmp;
                    //Update the number
                    num = num - base[t] * tmp;
                    //Jump to the next digit
                    index++;
                    //If index is occupied by the point
                    if (index == point)
                    {
                        //Place the point
                        str[index] = '.';
                        //Jump to the next digit
                        index++;
                    }
                }
                //If: I have a meaningful zero
                else
                {
                    //It's a zero
                    str[ index ] = '0';
                    //Jump to the next digit
                    index++;
                    //If index is occupied by the point
                    if (index == point)
                    {
                        //Place the point
                        str[index] = '.';
                        //Jump to the next digit
                        index++;
                    }
                }
            }	//End for: all bases
            index = 6;
            //Terminator
            str[index] = Config::TERMINATOR;

            //----------------------------------------------------------------
            //	RETURN
            //----------------------------------------------------------------
            DRETURN_ARG("digits written: %d | output >%s<\n", index, str);
            return (index);
        } //End public static method: num_to_str | int32_t | uint8_t | char * |

        /***************************************************************************/
        //!	@brief public static method
        //!	num_to_eng | int32_t | uint8_t | const char * |
        /***************************************************************************/
        //! @param num		| int32_t | number to be converted
        //! @param exp		| int8_t | base 10^x eponent of the number. E.G. the user might have provided a number in millivolts, exponent -3
        //!	@param str_len	| uint8_t | length of the provided string. Avoids overflow. Include terminator. Must be at least 7
        //! @param str		| const char * | return string provied by the caller
        //! @return uint8_t	| number of digits written in the string, not including terminator
        //! @details
        //!	wrapper
        /***************************************************************************/

        static uint8_t num_to_eng( int32_t num, int8_t num_exp, uint8_t str_len, char *str )
        {
            DENTER_ARG("S32 num: %ld | base exponent 10^%d\n", (int32_t)num, num_exp);
            //----------------------------------------------------------------
            //	VARS
            //----------------------------------------------------------------

            //number of character written
            uint8_t ret;
            uint32_t positive;

            //----------------------------------------------------------------
            //	INIT
            //----------------------------------------------------------------

            //If: string is invalid
            if ((SAFETY_CHECKS == true) && ((str == nullptr) || (str_len == 0)))
            {
                //Bad parameters
                DRETURN_ARG("ERR: bad parameters\n");
                return 0;
            }

            //----------------------------------------------------------------
            //	BODY
            //----------------------------------------------------------------

            //If: negative
            if (num < 0)
            {
                //Write sign '-'
                str[ 0 ] = '-';
                //Correct sign
                positive = -num;

            }
            //If: zero or positive
            else
            {
                //Write sign '+'
                str[ 0 ] = '+';
                positive = num;
            }
            //Full sized num_to_str
            ret = num_to_eng( (uint32_t)positive, num_exp, str_len -Config::STRING_SIGN_SIZE, &str[Config::STRING_SIGN_SIZE] );
            //If: no digits were written
            if (ret == 0)
            {
                //Add a terminator for safety
                str[0] = '\0';
                DRETURN_ARG("ERR: no digits were written\n");
                return 0;   //No point in adding a sign to an empty string: fail
            }

            //----------------------------------------------------------------
            //	RETURN
            //----------------------------------------------------------------
            DRETURN_ARG("digits written: %d | output >%s<\n", ret +Config::STRING_SIGN_SIZE, str);
            return (ret +Config::STRING_SIGN_SIZE);
        } //End public static method: num_to_eng | int32_t | uint8_t | char * |

         /***************************************************************************/
        //!	@brief public static method
        //!	num_to_eng | uint32_t | uint8_t | const char * |
        /***************************************************************************/
        //! @param num		| int32_t | number to be converted
        //! @param exp		| int8_t | base 10^x eponent of the number. E.G. the user might have provided a number in millivolts, exponent -3
        //!	@param str_len	| uint8_t | length of the provided string. Avoids overflow. Include terminator. Must be at least 7
        //! @param str		| const char * | return string provied by the caller
        //! @return uint8_t	| number of digits written in the string, not including terminator
        //! @details
        //!	wrapper
        /***************************************************************************/

        static inline uint8_t num_to_eng( uint32_t num, uint8_t str_len, char *str )
        {
            DENTER_ARG("S32 num: %d\n", num);

            //----------------------------------------------------------------
            //	BODY
            //----------------------------------------------------------------

            //Wrapper around the conversion function that takes an optional 10^x exponent of the number
            uint8_t ret = num_to_eng( num, 0, str_len, str );

            //----------------------------------------------------------------
            //	RETURN
            //----------------------------------------------------------------
            DRETURN_ARG("digits written: %d | output >%s<\n", ret, str);
            return (ret);
        } //End public static method: num_to_eng | int32_t | uint8_t | char * |

        /***************************************************************************/
        //!	@brief public static method
        //!	num_to_eng | int32_t | uint8_t | const char * |
        /***************************************************************************/
        //! @param num		| int32_t | number to be converted
        //! @param exp		| int8_t | base 10^x eponent of the number. E.G. the user might have provided a number in millivolts, exponent -3
        //!	@param str_len	| uint8_t | length of the provided string. Avoids overflow. Include terminator. Must be at least 7
        //! @param str		| const char * | return string provied by the caller
        //! @return uint8_t	| number of digits written in the string, not including terminator
        //! @details
        //!	wrapper
        /***************************************************************************/

        static inline uint8_t num_to_eng( int32_t num, uint8_t str_len, char *str )
        {
            DENTER_ARG("S32 num: %d\n", num);

            //----------------------------------------------------------------
            //	BODY
            //----------------------------------------------------------------

            //Wrapper around the conversion function that takes an optional 10^x exponent of the number
            uint8_t ret = num_to_eng( num, 0, str_len, str );

            //----------------------------------------------------------------
            //	RETURN
            //----------------------------------------------------------------
            DRETURN_ARG("digits written: %d | output >%s<\n", ret, str);
            return (ret);
        } //End public static method: num_to_eng | int32_t | uint8_t | char * |
    //Visible only inside the class
    private:
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

};	//End Class: String

/**********************************************************************************
**	NAMESPACE
**********************************************************************************/

} //End Namespace: User

#else
    #warning "Multiple inclusion of hader file EMBEDDED_STRING_"
#endif
