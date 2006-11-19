//****************************************************************************
// Projekt:	
//
// File:	flashupdate
//
// Author:	Tobias Weber
//
// Version: 1.00A
//
// Beschreibung:
//
// Copyright (c) SEBA KMT 2006 Baunach, Germany
//****************************************************************************

// *************************** HEADER INCLUDE ********************************
#include  "flashupdate.h"

//******************************* DEFINES ************************************

union {
  u8 byte[_FU_FLASH_SIZE];
  u32 word[_FU_FLASH_SIZE/4];
} FU_RAM_flash_mem;

extern u8  key[_CRYPT_KEY_SIZE];

//******************************* FUNCTIONS **********************************

//**************************************************************************
// NAME: FU_RAM_func
//
// DESC: this function is copied and executed in RAM
//
// PARAM: u32* pData
//
// RETVAL: void
//         TRUE if successful, FALSE if failed.
//**************************************************************************
static __ramfunc void FU_RAM_func(u32* pData)
{
  //***** DECLARATION *****
  int i, j;
  unsigned int status;
  u32* pul;
  u32* pDataOrig;

  //***** INITIALZATION *****
  status = 0;
  pDataOrig = pData;

  //***** RANGE CHECK *****
  if(pData == NULL) return;

  //***** PROGRAM *****

  //unlock all pages
  for(i = 0; i < AT91C_IFLASH_NB_OF_PAGES; i += AT91C_IFLASH_NB_OF_LOCK_BITS) {
    status = AT91F_Flash_Unlock (i);
  }

  //***** erase flash *****
  AT91PS_MC ptMC = AT91C_BASE_MC; //* set the Flash controller base address
  //* Write the Erase All command
  ptMC->MC_FCR = AT91C_MC_CORRECT_KEY | AT91C_MC_FCMD_ERASE_ALL ;
  //* Wait the end of command
  AT91F_Flash_Ready();

  //******* check if properly erased *******
  if(!AT91F_Flash_Check_Erase((u32*)AT91C_IFLASH, AT91C_IFLASH_SIZE)) {
    LED_RED_ON;     //error
    LED_GREEN_ON;
    for(;;) {
    }
  }

  //************ program flash ***************
  pul = (u32*)AT91C_IFLASH;
  //go through the pages
  for(j = 0; j < _FU_FLASH_SIZE/AT91C_IFLASH_PAGE_SIZE; j++ ) {
    //flash one page
    for(i = 0; i < AT91C_IFLASH_PAGE_SIZE/4; i++) {
      *pul++ = *pData++;
    }
    //* Write the write page command
    AT91C_BASE_MC->MC_FCR = AT91C_MC_CORRECT_KEY | AT91C_MC_FCMD_START_PROG | (AT91C_MC_PAGEN & (j <<8)) ;
    do { //* Wait for end of command
     status = AT91C_BASE_MC->MC_FSR;
    }
    while ((status & AT91C_MC_FRDY) != AT91C_MC_FRDY );
    //* Check the result
    if ( (status & ( AT91C_MC_PROGE | AT91C_MC_LOCKE )) != 0 ) {
      LED_RED_ON;     //error
      LED_GREEN_ON;
      for(;;) {}
    }
  }

  pul = (u32*)AT91C_IFLASH;

  //********* compare content of flash **********
  for (i = 0; i < _FU_FLASH_SIZE/4; i++) {
    if(pul[i] != pDataOrig[i]) {
      // the flash is different
      LED_RED_ON;     //error
      LED_GREEN_ON;
      for(;;) {}
    }
  }

  //******** ALL OK. endless loop waiting for unplug ********
  LED_RED_OFF;
  for(;;) {
    for(i = 0; i < _LED_PWM_VAL; i++) {
      delay_us(i * _LED_PWM_VAL);
      LED_GREEN_ON;
      delay_us((_LED_PWM_VAL - i) * _LED_PWM_VAL);
      LED_GREEN_OFF;
    }
    for(i = 0; i < _LED_PWM_VAL; i++) {
      delay_us(i * _LED_PWM_VAL);
      LED_GREEN_OFF;
      delay_us((_LED_PWM_VAL - i) * _LED_PWM_VAL);
      LED_GREEN_ON;
    }
  }
}

//**************************************************************************
// NAME: flash_update
//
// DESC: Updates program flash. If it failed, it returns, else it continues
// update.
//
// PARAM: u8* pData
//
// RETVAL: void
//**************************************************************************
void FU_perform_update(u8* pData)
{
  //***** DECLARATION *****
  int i, j;
  //u32* pu32;
  u32 crc32;
  u16 currlen;
  u16 totallen;
  u16 rxlen;
  u8 retval[4];
  u8 chksum[4];
  u8 c;

  //***** INITIALZATION *****
  currlen = 0;
  rxlen = 0;
  crc32 = 0;

  for(i = 0; i < _FU_FLASH_SIZE/4; i++) {
      FU_RAM_flash_mem.word[i] = 0;
  }

  //***** RANGE CHECK *****
  if(pData == NULL) return;

  //***** PROGRAM *****

  // return value is orig - 1
  // this indicates that flash mode has been entered
  retval[0] = pData[0] - 1;
  pCDC.Write(&pCDC, (char *)&retval, 1);
  totallen = (u16)pData[1];

  //*** wait for incoming data ***

  // first total length (16Bit, 64k max)
  while(currlen < 2) {
    currlen += pCDC.Read(&pCDC, (char*)&FU_RAM_flash_mem.byte[currlen], _FU_BULK_SIZE);
  }
  totallen = FU_RAM_flash_mem.byte[0] | FU_RAM_flash_mem.byte[1] << 8;

  //get the whole binary file
  currlen = 0;
  for(i = 0; i < totallen/_FU_BULK_SIZE; i++) {
    LED_GREEN_OFF;
    while(currlen < _FU_BULK_SIZE) {
      currlen += pCDC.Read(&pCDC, (char*)&FU_RAM_flash_mem.byte[currlen+(_FU_BULK_SIZE*i)], _FU_BULK_SIZE);
    }
    LED_GREEN_ON;
    rxlen += currlen;
    retval[0] = rxlen & 0xFF;
    retval[1] = (rxlen >> 8)& 0xFF;
    pCDC.Write(&pCDC, (char *)&retval, 2);
    currlen = 0;
  }
  LED_GREEN_OFF;

  //calc checksum
  for(i = 0; i < totallen; i++) {
    crc32 += FU_RAM_flash_mem.byte[i];
  }
  //send back checksum
  retval[0] = crc32 & 0xFF;
  retval[1] = (crc32 >> 8)& 0xFF;
  retval[2] = (crc32 >> 16)& 0xFF;
  retval[3] = (crc32 >> 24)& 0xFF;
  pCDC.Write(&pCDC, (char *)&retval, 4);

  //get checksum from Updater
  currlen = 0;
  while(currlen < 4) {
    currlen += pCDC.Read(&pCDC, (char*)&chksum[currlen], 4);
  }

  //test received checksum
  if(retval[0] != chksum[0] | retval[1] != chksum[1] |
     retval[2] != chksum[2] | retval[3] != chksum[3]) {
    LED_RED_ON;
    LED_GREEN_ON;
    return;
  }
  // Decrypt firmware
  for(j = 0; j < totallen/_CRYPT_KEY_SIZE; j++) {
    for(i = _CRYPT_KEY_SIZE-1; i >= 0; i--) {
      c = FU_RAM_flash_mem.byte[i + (j*_CRYPT_KEY_SIZE)] ^ key[i];
      FU_RAM_flash_mem.byte[i + (j*_CRYPT_KEY_SIZE)]
        = FU_RAM_flash_mem.byte[key[i] + (j*_CRYPT_KEY_SIZE)];
      FU_RAM_flash_mem.byte[key[i] + (j*_CRYPT_KEY_SIZE)] = c;
    }
  }
  // check if decryption was valid
  if(FU_RAM_flash_mem.word[1] != _FU_CHECKCODE) {
    LED_RED_ON;
    LED_GREEN_ON;
    return;
  }

  //*** flash start LED signal for a while *****
  for(i = 0; i < 5; i++) {
    LED_RED_OFF;
    LED_GREEN_ON;
    delay_us(200000);
    LED_RED_ON;
    LED_GREEN_OFF;
    delay_us(200000);
  }

  retval[0] = 'O';
  retval[1] = 'K';
  pCDC.Write(&pCDC, (char *)&retval, 2);

  __disable_interrupt();

  //set flash clock speed
  AT91F_Flash_Init();

  // now flash process is being activated. Point of no return!
  FU_RAM_func(FU_RAM_flash_mem.word);
}
