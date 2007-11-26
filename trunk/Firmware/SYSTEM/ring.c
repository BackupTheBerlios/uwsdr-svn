//****************************************************************************
//*
//* RING.h
//*
//* (c) 2007, Tobias Weber, tw@suprafluid.com 
//****************************************************************************


//************************* INCLUDES *****************************************

#include "ring.h"
#include "types.h"
#include <string.h>

//************************* DEFINES ******************************************

//************************* MODULE VARIABLES *********************************

u8* m_pBuffer;
//int m_elements;
int m_elementSize; // grösse eines frames
int m_firstFull; 
int m_lastFull;
int m_firstFree;
int m_lastFree;
int m_numOfElements; // Anzahl der frames im buffer
int m_level;

//************************* FUNCTIONS ******************************************


//****************************************************************************
// RING_init
//
// Parameters: u8* pBuffer, int elements, int elementSize
// Return type: void
// Init for the ring buffer
//
//****************************************************************************
void RING_init(u8* pBuffer, int elements, int elementSize, int numOfElements)
{
  //*** DEFINITON ***
  //*** INITIALIZATION ***
  //*** PARAMETER CHECK ***
  //*** PROGRAM CODE ***
  
  m_pBuffer = pBuffer;
//  m_elements = elements;
  m_elementSize = elementSize;
  m_numOfElements = numOfElements;
  m_firstFull = 0;
  m_lastFull = 0;
  m_firstFree = 0;
  m_lastFree = 0;
  m_level = 0;
}


//****************************************************************************
// RING_reset
//
// Parameters: void
// Return type: void
// resets the memory ring
//
//****************************************************************************
void RING_reset(void)
{
  //*** DEFINITON ***
  //*** INITIALIZATION ***
  //*** PARAMETER CHECK ***
  //*** PROGRAM CODE ***
  m_firstFull = 0;
  m_lastFull = 0;
  m_firstFree = 0;
  m_lastFree = 0;
  memset(m_pBuffer, 0, m_numOfElements * m_elementSize);
}



//****************************************************************************
// RING_consume
//
// Parameters: void
// Return type: u8*
// Gives back the next full buffer
//
//****************************************************************************
u8* RING_consume(void)
{
  //*** DEFINITON ***
  u8* retval;
  //*** INITIALIZATION ***
  //*** PARAMETER CHECK ***
  //*** PROGRAM CODE ***
  
  if(m_level == 0)
    return NULL;
  
  m_level--;
    
  retval = m_pBuffer + m_firstFull * m_elementSize;

  m_firstFull++;
  if(m_firstFull == m_numOfElements) {
    m_firstFull = 0;
  }
  
  return retval;
  
}


//****************************************************************************
// RING_produce
//
// Parameters: void
// Return type: u8*
// returns a pointer where the producer can put its data
//
//****************************************************************************
u8* RING_produce(void)
{
  //*** DEFINITON ***
  u8* retval;
  //*** INITIALIZATION ***
  //*** PARAMETER CHECK ***
  //*** PROGRAM CODE ***

  retval = m_pBuffer + (m_firstFree * m_elementSize);

  if(m_level < m_numOfElements-1)
    m_level++;
  
  m_firstFree++;
  if(m_firstFree == m_numOfElements) {
    m_firstFree = 0;
  }

  return retval;
  
}

