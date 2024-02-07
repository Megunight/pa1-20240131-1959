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

/*
@pre rowstart points to a row with at least 3 physical nodes
takes the node at the start of a row and returns the brightest node (sum of RGB) in the row
*/
ImgNode* minBrightNode(ImgNode* rowstart);

/*
takes the node at the start of a row and returns the node with the least colour difference, summed by comparing difference between left and right
colour difference being determined by distanceTo function in RGBAPixel.h
*/
ImgNode* minColourDifference(ImgNode* rowstart);