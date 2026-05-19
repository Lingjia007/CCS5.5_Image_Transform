/****************************************************************************/
/*  C6748.cmd                                                               */
/*  Copyright (c) 2010 Texas Instruments Incorporated                       */
/*  Author: Rafael de Souza                                                 */
/*                                                                          */
/*    Description: This file is a sample linker command file that can be    */
/*                 used for linking programs built with the C compiler and  */
/*                 running the resulting .out file on a C6748               */
/*                 device.  Use it as a guideline.  You will want to        */
/*                 change the memory layout to match your specific C6xxx    */
/*                 target system.  You may want to change the allocation    */
/*                 scheme according to the size of your program.            */
/*                                                                          */
/****************************************************************************/

//-l "..//..//chap_6_1_lib//Debug//chap_6_1_lib.lib"
//-l "../../chap_6_1_lib/Debug/chap_6_1_lib.lib"
-stack 4096
-heap 0x800000

MEMORY
{
    SHDSPL2RAM   o = 0x11800000  l = 0x00040000   /* 256kB L2 Shared Internal RAM */
    SHDSPL1PRAM  o = 0x11E00000  l = 0x00008000   /* 32kB L1 Shared Internal Program RAM */
    SHDSPL1DRAM  o = 0x11F00000  l = 0x00008000   /* 32kB L1 Shared Internal Data RAM */
    EMIFACS2     o = 0x60000000  l = 0x02000000   /* 32MB Async Data (CS2) */
    SHRAM        o = 0x80000000  l = 0x00020000   /* 128kB Shared RAM */
    DDR2         o = 0xC0000000  l = 0x20000000   /* 512MB DDR2 Data */
}                                              
                                               
SECTIONS                                       
{                                              
    .text          >  DDR2
    .stack         >  DDR2
    .bss           >  DDR2
    .cio           >  DDR2
    .const         >  DDR2
    .data          >  DDR2
    .switch        >  DDR2
    .sysmem        >  DDR2
    .far           >  DDR2
    .args          >  DDR2
    .ppinfo        >  DDR2
    .ppdata        >  DDR2
  
    /* COFF sections */
    .pinit         >  DDR2
    .cinit         >  DDR2
  
    /* EABI sections */
/*
    .binit         >  SHRAM
    .init_array    >  SHRAM
    .neardata      >  SHRAM
    .fardata       >  SHRAM
    .rodata        >  SHRAM
    .c6xabi.exidx  >  SHRAM
    .c6xabi.extab  >  SHRAM
*/
}
