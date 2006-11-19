//*----------------------------------------------------------------------------
//*         ATMEL Microcontroller Software Support  -  ROUSSET  -
//*----------------------------------------------------------------------------
//* The software is delivered "AS IS" without warranty or condition of any
//* kind, either express, implied or statutory. This includes without
//* limitation any warranty or condition with respect to merchantability or
//* fitness for any particular purpose, or against the infringements of
//* intellectual property rights of others.
//*----------------------------------------------------------------------------
//* File Name           : Flash.c
//* Object              : Flash routine
//* Creation            : JPP   30/Jun/2004
//* Modif               : JPM   16/Nov/2004 Flash write status
//*----------------------------------------------------------------------------

// ************************** GOBAL CONFIG INCLUDE ***************************
#include "config.h"
#include "flash.h"

//*----------------------------------------------------------------------------
//* \fn    AT91F_Flash_Init
//* \brief Flash init
//*----------------------------------------------------------------------------
void AT91F_Flash_Init (void)
{
    //* Set number of Flash Waite sate
    //  SAM7S64 features Single Cycle Access at Up to 30 MHz
    //  if MCK = 47923200, 50 Cycles for 1 µseconde ( field MC_FMR->FMCN)
        AT91C_BASE_MC->MC_FMR = ((AT91C_MC_FMCN)&(72 <<16)) | AT91C_MC_FWS_1FWS ;
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_Flash_Ready
//* \brief Wait the flash ready
//*----------------------------------------------------------------------------
__ramfunc int AT91F_Flash_Ready (void)
{
    u32 status;
    status = 0;
    
    //* Wait the end of command
        while ((status & AT91C_MC_FRDY) != AT91C_MC_FRDY )
        {
          status = AT91C_BASE_MC->MC_FSR;
        }
        return status;
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_Flash_Lock_Status
//* \brief Get the Lock bits field status
//*----------------------------------------------------------------------------
__ramfunc int AT91F_Flash_Lock_Status(void)
{
  return (AT91C_BASE_MC->MC_FSR & AT91C_MC_FSR_LOCK);
}
//*----------------------------------------------------------------------------
//* \fn    AT91F_Flash_Lock
//* \brief Write the lock bit and set at 0 FSR Bit = 1
//* \input page number (0-1023)
//* \output Region
//*----------------------------------------------------------------------------
__ramfunc int AT91F_Flash_Lock (u32 Flash_Lock_Page)
{
    //* set the Flash controller base address
        AT91PS_MC ptMC = AT91C_BASE_MC;

	 //* write the flash
    //* Write the Set Lock Bit command
        ptMC->MC_FCR = AT91C_MC_CORRECT_KEY | AT91C_MC_FCMD_LOCK | (AT91C_MC_PAGEN & (Flash_Lock_Page << 8) ) ;

    //* Wait the end of command
         AT91F_Flash_Ready();

  return (AT91F_Flash_Lock_Status());
}
//*----------------------------------------------------------------------------
//* \fn    AT91F_Flash_Unlock
//* \brief Clear the lock bit and set at 1 FSR bit=0
//* \input page number (0-1023) 
//* \output Region
//*----------------------------------------------------------------------------
__ramfunc int AT91F_Flash_Unlock(u32 Flash_Lock_Page)
{
    //* Write the Clear Lock Bit command
        AT91C_BASE_MC->MC_FCR = AT91C_MC_CORRECT_KEY | AT91C_MC_FCMD_UNLOCK | (AT91C_MC_PAGEN & (Flash_Lock_Page << 8) ) ;

    //* Wait the end of command
        AT91F_Flash_Ready();

  return (AT91F_Flash_Lock_Status());
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_Flash_Check_Erase
//* \brief Check the memory at 0xFF in 32 bits access
//*----------------------------------------------------------------------------
__ramfunc int AT91F_Flash_Check_Erase (u32 * start, u32 size)
{
	u32 i;
    //* Check if flash is erased
	for (i=0; i < (size/4) ; i++ )
	{
	    if ( start[i] != ERASE_VALUE ) return  false;
	}
	return true ;
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_Flash_Erase_All
//* \brief Send command erase all flash
//*----------------------------------------------------------------------------
__ramfunc int AT91F_Flash_Erase_All(void)
{
    //* set the Flash controller base address
        AT91PS_MC ptMC = AT91C_BASE_MC;
    //* Write the Erase All command
        ptMC->MC_FCR = AT91C_MC_CORRECT_KEY | AT91C_MC_FCMD_ERASE_ALL ;
    //* Wait the end of command
        AT91F_Flash_Ready();
    //* Check the result
        return ( (ptMC->MC_FSR & ( AT91C_MC_PROGE | AT91C_MC_LOCKE ))==0) ;
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_Flash_Write
//* \brief Write in one Flash page located in AT91C_IFLASH,  size in 32 bits
//* \input Flash_Address: start at 0x0010 0000 size: in byte
//*----------------------------------------------------------------------------
__ramfunc int AT91F_Flash_Write( u32 Flash_Address ,int size ,u32 * buff)
{
    //* set the Flash controller base address
    AT91PS_MC ptMC = AT91C_BASE_MC;
    u32 i, page, status;
    u32 * Flash;
    //* init flash pointer
        Flash = (u32 *) Flash_Address;
    //* Get the Flash page number
        page = ((Flash_Address - (u32)AT91C_IFLASH ) /AT91C_IFLASH_PAGE_SIZE);
   //* copy the new value
	for (i=0; (i < AT91C_IFLASH_PAGE_SIZE) & (size > 0) ;i++, Flash++,buff++,size-=4 ){
	//* copy the flash to the write buffer ensuring code generation
	    *Flash=*buff;
	}
    //* Write the write page command
        ptMC->MC_FCR = AT91C_MC_CORRECT_KEY | AT91C_MC_FCMD_START_PROG | (AT91C_MC_PAGEN & (page <<8)) ;
    //* Wait the end of command
        status = AT91F_Flash_Ready();
    //* Check the result
    if ( (status & ( AT91C_MC_PROGE | AT91C_MC_LOCKE ))!=0) return false;
  return true;
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_Flash_Write
//* \brief Write in one Flash page located in AT91C_IFLASH,  size in byte
//* \input Start address (base=AT91C_IFLASH) size (in byte ) and buff address
//*----------------------------------------------------------------------------
__ramfunc int AT91F_Flash_Write_all( u32 Flash_Address ,int size ,u32 * buff)
{

    int   nextsize, status;
    u32  dest;
    u32 * src;

    dest = Flash_Address;
    src = buff;
    status = true;

    while( (status == true) & (size > 0) )
	{
        //* Check the size
        if (size <= AT91C_IFLASH_PAGE_SIZE) nextsize = size;
        else nextsize = AT91C_IFLASH_PAGE_SIZE;

        //* Unlock current sector base address - current address by sector size
        AT91F_Flash_Unlock((dest - (u32)AT91C_IFLASH ) /AT91C_IFLASH_PAGE_SIZE);

        //* Write page and get status
        status = AT91F_Flash_Write( dest, nextsize, src );
        // * get next page param
        size -= nextsize;
        src += AT91C_IFLASH_PAGE_SIZE;
        //dest +=  AT91C_IFLASH_PAGE_SIZE;
	}
    return status;
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_NVM_Status
//* \brief Get the NVM field status
//*----------------------------------------------------------------------------
__ramfunc int AT91F_NVM_Status(void)
{
  return (AT91C_BASE_MC->MC_FSR & AT91C_MC_FSR_MVM);
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_NVM_Set
//* \brief Write the Non Volatile Memory Bits and set at 0 FSR Bit = 1
//*----------------------------------------------------------------------------
__ramfunc int AT91F_NVM_Set (unsigned char NVM_Number)
{
    //* set the Flash controller base address
        AT91PS_MC ptMC = AT91C_BASE_MC;

	 //* write the flash
    //* Write the Set NVM Bit command
        ptMC->MC_FCR = AT91C_MC_CORRECT_KEY | AT91C_MC_FCMD_SET_GP_NVM | (AT91C_MC_PAGEN & (NVM_Number << 8) ) ;

    //* Wait the end of command
        AT91F_Flash_Ready();
  return (AT91F_NVM_Status());
}
//*----------------------------------------------------------------------------
//* \fn    AT91F_NVM_Clear
//* \brief Clear the Non Volatile Memory Bits and set at 1 FSR bit=0
//*----------------------------------------------------------------------------
__ramfunc int AT91F_NVM_Clear(unsigned char NVM_Number)
{
    //* set the Flash controller base address
        AT91PS_MC ptMC = AT91C_BASE_MC;

	 //* write the flash
    //* Write the Clear NVM Bit command
        ptMC->MC_FCR = AT91C_MC_CORRECT_KEY | AT91C_MC_FCMD_CLR_GP_NVM | (AT91C_MC_PAGEN & (NVM_Number << 8) ) ;

    //* Wait the end of command
       AT91F_Flash_Ready();

  return (AT91F_NVM_Status());
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_SET_Security_Status
//* \brief Get Flash Security Bit Status 
//*----------------------------------------------------------------------------
int AT91F_GET_Security_Status (void)
{
  return (AT91C_BASE_MC->MC_FSR & AT91C_MC_SECURITY);
}

//*----------------------------------------------------------------------------
//* \fn AT91F_SET_Security
//* \brief Set Flash Security Bit
//*----------------------------------------------------------------------------
int AT91F_SET_Security (void)
{
	 //* write the flash
    //* Write the Set Security Bit command
        AT91C_BASE_MC->MC_FCR = ( AT91C_MC_CORRECT_KEY | AT91C_MC_FCMD_SET_SECURITY ) ;

    //* Wait the end of command
       AT91F_Flash_Ready();

  return (AT91F_GET_Security_Status());
}

