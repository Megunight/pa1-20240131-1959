/**
 *  @file        imglist-private.h
 *  @description Student-defined functions of ImgList class, for CPSC 221 PA1
 *
 *  THIS FILE WILL BE SUBMITTED.
 *  YOU MAY ADD YOUR OWN PRIVATE MEMBER FUNCTION DECLARATIONS HERE.
 *  IF YOU DO NOT HAVE FUNCTIONS TO ADD, LEAVE THIS BLANK
 */

// takes the northwest node and the height of the end matrix and creates all westmost row nodes recursively while providing south pointer
void initAllSouth(ImgNode* northwest, unsigned height);