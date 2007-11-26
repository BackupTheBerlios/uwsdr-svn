//****************************************************************************
//*
//* TWI.c
//*
//* (c) 2007, Tobias Weber, tw@suprafluid.com 
//****************************************************************************


//************************* INCLUDES *****************************************

#include "TWI.h"
#include "ioat91sam7x256.h"
#include "lib_AT91SAM7XC256.h"

//************************* DEFINES ******************************************


//Sendedaten
const char *Dat2Send;
u16 AktPosTx, Len2Send;
bool SendInProcess;
//Empfangsdaten
char *Dat2Receive;
u16 AktPosRx, Len2Receive;
u8 LenMaxRx, *LenAktRx;
bool ReceiveInProcess;
bool ReadVarDat;
//Call-back functions
void (*AktWRClbFunc)(void);
void (*AktRDClbFunc)(void);

u16 intZ;


void TWI_init(void)
{
  
  
  //* Reset peripheral
  AT91C_BASE_TWI->TWI_CR = AT91C_TWI_SWRST;
  
  // Configure TWI PIOs
  AT91F_TWI_CfgPIO ();
  // Configure PMC by enabling TWI clock
  AT91F_TWI_CfgPMC ();
  // Configure TWI in master mode
  AT91F_TWI_Configure (AT91C_BASE_TWI);
  // Set TWI Clock Waveform Generator Register
  AT91C_BASE_TWI->TWI_CWGR	=  0x0000EDED;//0x0000EDED;
  
  //* Disable interrupts
  AT91C_BASE_TWI->TWI_IDR = (unsigned int) -1;
  
  //TWI Interrupt
  // Set protected mode and clear general mask. 
  //*AT91C_AIC_DCR = AT91C_AIC_DCR_PROT; //Enables protected mode 
  AT91C_BASE_AIC -> AIC_ICCR = AT91C_ID_TWI;
  AT91C_BASE_TWI->TWI_IER	=  AT91C_TWI_TXRDY | AT91C_TWI_RXRDY;// | AT91C_TWI_TXCOMP | AT91C_TWI_NACK;
  AT91F_AIC_ConfigureIt ( AT91C_BASE_AIC, AT91C_ID_TWI, TWI_INTERRUPT_LEVEL, AT91C_AIC_SRCTYPE_INT_POSITIVE_EDGE, TWI_c_irq_handler);
  AT91F_AIC_DisableIt(AT91C_BASE_AIC, AT91C_ID_TWI);
  

  
  return;  
}

void TWI_WR_BYTE(void)
{
    
  //Start
  if(AktPosTx == 0)
  {
    
//    AT91C_BASE_TWI->TWI_CR = 0;
    //Startbedingung senden
    if(AktPosTx == Len2Send-1)
      //Start und Stopp senden
      AT91C_BASE_TWI->TWI_CR = AT91C_TWI_START | AT91C_TWI_MSEN | AT91C_TWI_STOP;
    else
      //Start senden
      AT91C_BASE_TWI->TWI_CR = AT91C_TWI_START | AT91C_TWI_MSEN;

    //Interrupt freigeben
    AT91C_BASE_TWI->TWI_IDR	=  0xFFFFFFFF;
    AT91C_BASE_TWI->TWI_IER	=  AT91C_TWI_TXRDY;
    AT91F_AIC_EnableIt (AT91C_BASE_AIC, AT91C_ID_TWI);
  } 
  
  
  //Stopp senden
  if(AktPosTx >= (Len2Send-1))
  {
    AT91F_AIC_DisableIt(AT91C_BASE_AIC, AT91C_ID_TWI);
    AT91C_BASE_TWI->TWI_CR = AT91C_TWI_STOP | AT91C_TWI_MSEN;
    //Sendevorgang beendet
    SendInProcess = false;
  }
  
  if(AktPosTx < Len2Send)
  {
  //Übertragung starten
  //PrintfDBGU("TxByte: %d; Zaehler: %d;          \n\r",*(Dat2Send+AktPosTx),AktPosTx); 
  AT91C_BASE_TWI->TWI_THR = *(Dat2Send+AktPosTx);
  //Zähler+1
  AktPosTx++;

  
  //Clb aufrufen
  if((SendInProcess == false) && (AktWRClbFunc != NULL))
  {
    AktWRClbFunc();
  }
  
  }
  
  return;  
}

void TWI_RD_BYTE(void)
{
  
  //Start?
  if(AktPosRx == 0)
  {
    //Nur ein Byte lesen?
    if(Len2Receive == 1)
    {
      //Start und Stopp senden
      AT91C_BASE_TWI->TWI_CR = AT91C_TWI_START | AT91C_TWI_STOP | AT91C_TWI_MSEN;    
    }
    else
    {
      //Start senden 
      AT91C_BASE_TWI->TWI_CR = AT91C_TWI_START | AT91C_TWI_MSEN;
    }
  //Interrupt freigeben
  AT91F_AIC_ConfigureIt ( AT91C_BASE_AIC, AT91C_ID_TWI, TWI_INTERRUPT_LEVEL, AT91C_AIC_SRCTYPE_INT_HIGH_LEVEL, TWI_c_irq_handler);
  AT91C_BASE_TWI->TWI_IDR	=  0xFFFFFFFF;
  AT91C_BASE_TWI->TWI_IER	=  AT91C_TWI_RXRDY;
  AT91F_AIC_EnableIt (AT91C_BASE_AIC, AT91C_ID_TWI);
  }
    
  //Stopp senden
  if((AktPosRx == Len2Receive-1) && (Len2Receive > 1))
      AT91C_BASE_TWI->TWI_CR = AT91C_TWI_STOP | AT91C_TWI_MSEN;
  
    
  return; 
}

bool TWI_WR_DAT(const char *Dat, u16 Len, u16 TWI_ADR, void (*TxClbFunc)(void))
{
  
  
  //Falls ein Sendevorgang läuft -> Abbruch
  if(ReceiveInProcess == true || SendInProcess == true)
     return false;
  //Sendevorgang läuft
  SendInProcess = true;
 
  //Überwachung
  intZ = 0;
  
  
  TWI_init();
  
  
  //Adresse der Sendedaten sowie Länge speichern
  Dat2Send = Dat;
  Len2Send = Len;
  //Clb-Adresse speichern
  AktWRClbFunc = TxClbFunc;
  //Positionszähler zurücksetzen
  AktPosTx = 0;
    
  
  
  //Bytes schreiben
  AT91C_BASE_TWI->TWI_MMR = 0;
  //Adresse setzen
  AT91C_BASE_TWI->TWI_MMR = TWI_ADR << 16;
  //Übertragung starten  
  TWI_WR_BYTE();
 
  
  return true; 
}


__arm void TWI_c_irq_handler(void)
{
  
  unsigned int RxByte;
  //Überwachung
  intZ++;
  if(intZ > 1000)
            {
              ReceiveInProcess = false;
              SendInProcess = false;
              AT91F_AIC_DisableIt(AT91C_BASE_AIC, AT91C_ID_TWI);
            }

  AT91S_TWI *TWI_pt = AT91C_BASE_TWI;
	unsigned int status;
	//TWI Status ermitteln
	status = TWI_pt->TWI_SR;
        
	//Byte gesendet
        if ( status & AT91C_TWI_TXCOMP)
        {
	
                
	}
        
        //Byte empfangen, RHR Ready
        if ( status & AT91C_TWI_RXRDY)
        {
          RxByte = AT91C_BASE_TWI->TWI_RHR;
	  if((AktPosRx < Len2Receive) && (ReceiveInProcess == true))
          {
            //Bei variabler Länge speichern
            if((ReadVarDat == true) && (AktPosRx == 1) && (RxByte < LenMaxRx))
            {            
              Len2Receive = RxByte+3;  
              *LenAktRx = RxByte+3;
            }
            
            //if(AktPosRx < LenMaxRx)
            
              //Byte in den Puffer kopieren
              *(Dat2Receive+AktPosRx++) = RxByte;
            
            
            //Sendevorgang beendet
            if(AktPosRx == Len2Receive)
            {
            AT91F_AIC_DisableIt(AT91C_BASE_AIC, AT91C_ID_TWI);
            ReceiveInProcess = false;
            //Clb Aufrufen
            if(AktRDClbFunc != 0)
              AktRDClbFunc();
            }
            else
            TWI_RD_BYTE();	
          }      
	}
        
        //Bereit zum Senden, THR Ready
        if ( status & AT91C_TWI_TXRDY )
        {
          if((AktPosTx < Len2Send) && (SendInProcess == true))
          {
            
            TWI_WR_BYTE();
          }  
                     
	}
        
        //Overrun Error
        if ( status & AT91C_TWI_OVRE)
        {
		
                
	}
        
        //Underrun Error
        if ( status & AT91C_TWI_UNRE)
        {
		
                
	}
        
        //Not Acknowledged
        if ( status & AT91C_TWI_NACK)
        {
	ReceiveInProcess = false;	
        SendInProcess = false;        
	}
  return;
}



