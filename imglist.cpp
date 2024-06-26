/**
 *  @file        imglist.cpp
 *  @description Contains partial implementation of ImgList class
 *               for CPSC 221 PA1
 *               Function bodies to be completed by student
 * 
 *  THIS FILE WILL BE SUBMITTED
 */

#include "imglist.h"

#include <math.h> // provides fmax, fmin, and fabs functions

/*********************
* CONSTRUCTORS, ETC. *
*********************/

/**
 * Default constructor. Makes an empty list
 */
ImgList::ImgList() {
    // set appropriate values for all member attributes here
    northwest = nullptr;
    southeast = nullptr;
}

/**
 * Creates a list from image data
 * @pre img has dimensions of at least 1x1
 */
ImgList::ImgList(PNG& img) {
    // build the linked node structure and set the member attributes appropriately
	unsigned width = img.width();
    unsigned height = img.height();

    // initialization of the first row
    northwest = new ImgNode();
    ImgNode* currFirst = northwest;
    for (unsigned int i = 0; i < width; i++) {
        if (i == (width - 1)) // when pointer gets to end of row
            currFirst->colour = *img.getPixel(i, 0);
        else {
            currFirst->colour = *img.getPixel(i, 0);
            currFirst->east = new ImgNode();
            currFirst->east->west = currFirst; // setting up doubly-link east-west direction
            currFirst = currFirst->east; // advance pointer eastwards
        }
    }

    currFirst = northwest; // resets prev row pointer to first
    initAllSouth(northwest, height);
    ImgNode* currRow = northwest->south;
    for (unsigned int y = 1; y < height; y++) {
        for (unsigned int x = 0; x < width; x++) {
            if (x == (width - 1)) { // end of row
                currRow->colour = *img.getPixel(x, y);
                currFirst->south = currRow;
                currRow->north = currFirst;
                if (y == (height - 1)) { // last column of last row
                    southeast = currRow;
                } else { // think of like typewriter *kaching*!
                    currRow = northwest->south;
                    currFirst = northwest;
                    for (unsigned int i = 0; i < y; i++) {
                        currRow = currRow->south;
                        currFirst = currFirst->south;
                    }
                }
            } else {
                currRow->colour = *img.getPixel(x, y);
                currFirst->south = currRow; // south linkage for first row
                currRow->north = currFirst; // north
                currFirst = currFirst->east;
                currRow->east = new ImgNode(); // east
                currRow->east->west = currRow; // west-east link
                currRow = currRow->east;
            }
        }
    }
}

/**
 * Initializes all nodes in each row in the first column with only southerly linkage
 * @pre northwest is initialized and given
 */
void ImgList::initAllSouth(ImgNode* northwest, unsigned height) {
    if (height == 1) // base case; includes the fact northwest node is also in height
        return;
    ImgNode* newNode = new ImgNode();
    northwest->south = newNode;
    initAllSouth(newNode, height-1);
}

/************
* ACCESSORS *
************/

/**
 * Returns the horizontal dimension of this list (counted in nodes)
 * Note that every row will contain the same number of nodes, whether or not
 *   the list has been carved.
 * We expect your solution to take linear time in the number of nodes in the
 *   x dimension.
 */
unsigned int ImgList::GetDimensionX() const {
    if (northwest == nullptr) return 0;

    unsigned int count = 0; 
    ImgNode* current = northwest; 

    while (current != nullptr) {
        count++;
        current = current->east; 
    }

    return count;
}

/**
 * Returns the vertical dimension of the list (counted in nodes)
 * It is useful to know/assume that the grid will never have nodes removed
 *   from the first or last columns. The returned value will thus correspond
 *   to the height of the PNG image from which this list was constructed.
 * We expect your solution to take linear time in the number of nodes in the
 *   y dimension.
 */
unsigned int ImgList::GetDimensionY() const {
    if (southeast == nullptr) return 0;

    unsigned int count = 0; 
    ImgNode* current = southeast; 

    while (current != nullptr) {
        count++;
        current = current->north; 
    }

    return count;
}

/**
 * Returns the horizontal dimension of the list (counted in original pixels, pre-carving)
 * The returned value will thus correspond to the width of the PNG image from
 *   which this list was constructed.
 * We expect your solution to take linear time in the number of nodes in the
 *   x dimension.
 */
unsigned int ImgList::GetDimensionFullX() const {
    if (northwest == nullptr) return 0;

    unsigned int originalWidth = 0;
    ImgNode* current = northwest;

    // Iterate over the top row
    while (current != nullptr) {
        originalWidth++; // Count the current node
        originalWidth += current->skipright; 
        current = current->east;
    }
    return originalWidth;
}

/**
 * Returns a pointer to the node which best satisfies the specified selection criteria.
 * The first and last nodes in the row cannot be returned.
 * @pre rowstart points to a row with at least 3 physical nodes
 * @pre selectionmode is an integer in the range [0,1]
 * @param rowstart - pointer to the first node in a row
 * @param selectionmode - criterion used for choosing the node to return
 *          0: minimum "brightness" across row, not including extreme left or right nodes
 *          1: node with minimum total of "colour difference" with its left neighbour and with its right neighbour.
 *        In the (likely) case of multiple candidates that best match the criterion,
 *        the left-most node satisfying the criterion (excluding the row's starting node)
 *        will be returned.
 * A note about "brightness" and "colour difference":
 * For PA1, "brightness" will be the sum over the RGB colour channels, multiplied by alpha.
 * "colour difference" between two pixels can be determined
 * using the "distanceTo" function found in RGBAPixel.h.
 */
ImgNode* ImgList::SelectNode(ImgNode* rowstart, int selectionmode) {
    if (selectionmode == 0)
        return minBrightNode(rowstart);
    else
        return minColourDifference(rowstart);
}

ImgNode* ImgList::minBrightNode(ImgNode* rowstart) {
    double minBrightness = 766; // because max brightness is 765
    ImgNode* brightNode = rowstart;

    while (rowstart != NULL) {
        RGBAPixel pixel = rowstart->colour;
        double brightness = (pixel.r + pixel.g + pixel.b) * pixel.a;

        if (brightness < minBrightness) {
            minBrightness = brightness;
            brightNode = rowstart;
        }
        rowstart = rowstart->east;
    }
    return brightNode;
}

ImgNode* ImgList::minColourDifference(ImgNode* rowstart) {
    double leastColourDiff = 7; // because distanceTo outputs in range[0, 3] so 6 is max colour diff
    ImgNode* leastDiffNode = rowstart;

    while (rowstart != NULL) {
        RGBAPixel pixel = rowstart->colour;
        RGBAPixel pixelL = rowstart->west->colour;
        RGBAPixel pixelR = rowstart->east->colour;
        double colourDiff = pixel.distanceTo(pixelL) + pixel.distanceTo(pixelR); 

        if (colourDiff < leastColourDiff) {
            leastColourDiff = colourDiff;
            leastDiffNode = rowstart;
        }
        rowstart = rowstart->east;
    }
    return leastDiffNode;
}

/**
 * Renders this list's pixel data to a PNG, with or without filling gaps caused by carving.
 * @pre fillmode is an integer in the range of [0,2]
 * @param fillgaps - whether or not to fill gaps caused by carving
 *          false: render one pixel per node, ignores fillmode
 *          true: render the full width of the original image,
 *                filling in missing nodes using fillmode
 * @param fillmode - specifies how to fill gaps
 *          0: solid, uses the same colour as the node at the left of the gap
 *          1: solid, using the averaged values (all channels) of the nodes at the left and right of the gap
 *          2: linear gradient between the colour (all channels) of the nodes at the left and right of the gap
 *             e.g. a gap of width 1 will be coloured with 1/2 of the difference between the left and right nodes
 *             a gap of width 2 will be coloured with 1/3 and 2/3 of the difference
 *             a gap of width 3 will be coloured with 1/4, 2/4, 3/4 of the difference, etc.
 *             Like fillmode 1, use the smaller difference interval for hue,
 *             and the smaller-valued average for diametric hues
 */
PNG ImgList::Render(bool fillgaps, int fillmode) const {
    PNG outpng; // This will be returned later. Might be a good idea to resize it at some point.

    // Determine dimensions based on whether gaps should be filled
    unsigned int width = fillgaps ? GetDimensionFullX() : GetDimensionX();
    unsigned int height = GetDimensionY();
    outpng.resize(width, height); // Resize the output image accordingly

    ImgNode* rowNode = northwest;
    for (unsigned int y = 0; y < height; y++) {
        ImgNode* currNode = rowNode;
        for (unsigned int x = 0; currNode != nullptr && x < width; x++) {
            RGBAPixel* pixel = outpng.getPixel(x, y);
            if (fillgaps && currNode->east != nullptr && currNode->skipright > 0) {
                for (unsigned int gap = 0; gap <= currNode->skipright && x < width; gap++) {
                    if (fillmode == 0 || gap == 0) {
                        *pixel = currNode->colour;
                    } else if (fillmode == 1) { 
                        pixel->r = (currNode->colour.r + currNode->east->colour.r) / 2;
                        pixel->g = (currNode->colour.g + currNode->east->colour.g) / 2;
                        pixel->b = (currNode->colour.b + currNode->east->colour.b) / 2;
                        pixel->a = (currNode->colour.a + currNode->east->colour.a) / 2;
                    } else if (fillmode == 2) { // Mode 2
                        // Gradient color calculation
                        double fraction = double(gap) / (currNode->skipright + 1);
                        pixel->r = currNode->colour.r + fraction * (currNode->east->colour.r - currNode->colour.r);
                        pixel->g = currNode->colour.g + fraction * (currNode->east->colour.g - currNode->colour.g);
                        pixel->b = currNode->colour.b + fraction * (currNode->east->colour.b - currNode->colour.b);
                        pixel->a = currNode->colour.a + fraction * (currNode->east->colour.a - currNode->colour.a);
                    }

                    if (gap < currNode->skipright) { // Move to next pixel if within a gap
                        pixel = outpng.getPixel(++x, y);
                    }
                }
                currNode = currNode->east; // Move past the gap
            } else { // No gap or not filling gaps
                *pixel = currNode->colour;
                currNode = currNode->east;
            }
        }
        rowNode = rowNode->south; // Move to the next row
    }

    return outpng;
}

/************
* MODIFIERS *
************/

/**
 * Removes exactly one node from each row in this list, according to specified criteria.
 * The first and last nodes in any row cannot be carved.
 * @pre this list has at least 3 nodes in each row
 * @pre selectionmode is an integer in the range [0,1]
 * @param selectionmode - see the documentation for the SelectNode function.
 * @param this list has had one node removed from each row. Neighbours of the created
 *       gaps are linked appropriately, and their skip values are updated to reflect
 *       the size of the gap.
 */
void ImgList::Carve(int selectionmode) {

if (northwest == nullptr) return; // Check for an empty list

    // add your implementation here
    if (northwest == nullptr) return; // Empty list check

    ImgNode* currentRowStart = northwest;
    while (currentRowStart != nullptr) {
        ImgNode* nodeToRemove = SelectNode(currentRowStart, selectionmode);
        if (nodeToRemove != nullptr && nodeToRemove->east != nullptr && nodeToRemove->west != nullptr) {
            // Update the east pointer of the western neighbor
            nodeToRemove->west->east = nodeToRemove->east;
            // Update the west pointer of the eastern neighbor
            nodeToRemove->east->west = nodeToRemove->west;
            // Update skip values if needed
            nodeToRemove->west->skipright += 1 + nodeToRemove->skipright;
            nodeToRemove->east->skipleft += 1 + nodeToRemove->skipleft;

            // Finally, delete the node
            delete nodeToRemove;
        }
        // Move to the first node of the next row
        currentRowStart = currentRowStart->south;
    }
    unsigned int height = GetDimensionY();
    std::vector<ImgNode*> rows;
    ImgNode* yTraverse = northwest;
    for (unsigned int y = 0; y < height; y++) {
        rows.push_back(yTraverse);
        yTraverse = yTraverse->south;
    }

    for (unsigned int y = 0; y < height; y++) {
        ImgNode* selected = rows[y];
        selected = SelectNode(selected, selectionmode);

        // west and east nodes
        if (selected->west != NULL) {
            selected->west->east = selected->east;
            selected->west->skipright += selected->skipright + 1;

        }
        if (selected->east != NULL) {
            selected->east->west = selected->west;
            selected->east->skipleft += selected->skipleft + 1;
        }

        // north and south nodes
        if (selected->north != NULL) {
            selected->north->south = selected->south;
            selected->north->skipdown += selected->skipdown + 1;
        }
        if (selected->south != NULL) {
            selected->south->north = selected->north;
            selected->south->skipup += selected->skipup + 1;
        }

        delete selected;
    }
}

// note that a node on the boundary will never be selected for removal
/**
 * Removes "rounds" number of nodes (up to a maximum of node width - 2) from each row,
 * based on specific selection criteria.
 * Note that this should remove one node from every row, repeated "rounds" times,
 * and NOT remove "rounds" nodes from one row before processing the next row.
 * @pre selectionmode is an integer in the range [0,1]
 * @param rounds - number of nodes to remove from each row
 *        If rounds exceeds node width - 2, then remove only node width - 2 nodes from each row.
 *        i.e. Ensure that the final list has at least two nodes in each row.
 * @post this list has had "rounds" nodes removed from each row. Neighbours of the created
 *       gaps are linked appropriately, and their skip values are updated to reflect
 *       the size of the gap.
 */
void ImgList::Carve(unsigned int rounds, int selectionmode) {
    if (rounds > (GetDimensionX() - 2))
        rounds = GetDimensionX() - 2;
    
    for (unsigned int i = 0; i < rounds; i++)
        Carve(selectionmode);
}


/*
 * Helper function deallocates all heap memory associated with this list,
 * puts this list into an "empty" state. Don't forget to set your member attributes!
 * @post this list has no currently allocated nor leaking heap memory,
 *       member attributes have values consistent with an empty list.
 */
void ImgList::Clear() {
    // Start at the top-left corner of the grid.
    ImgNode* currentRow = northwest;
    while (currentRow != nullptr) {
        // Traverse the current row and delete all nodes in it.
        ImgNode* currentNode = currentRow;
        ImgNode* tempSouth = currentRow->south;
        while (currentNode != nullptr) {
            ImgNode* tempEast = currentNode->east; // Save the east node before deleting the current node.
            delete currentNode; // Deallocate the current node.
            currentNode = tempEast; // Move to the next node in the row.
        }
        // Move to the next row.
        currentRow = tempSouth;
    }
    // Reset member attributes to reflect an empty list.
    northwest = nullptr;
    southeast = nullptr;	
}

/**
 * Helper function copies the contents of otherlist and sets this list's attributes appropriately
 * @pre this list is empty
 * @param otherlist - list whose contents will be copied
 * @post this list has contents copied from by physically separate from otherlist
 */
void ImgList::Copy(const ImgList& otherlist) {
    if (otherlist.northwest == nullptr) {
        // otherlist is empty, so this list remains empty as well.
        northwest = nullptr;
        southeast = nullptr;
        return;
    }

    // Create the first node (top-left corner of the grid).
    northwest = new ImgNode(*otherlist.northwest);
    ImgNode* currentRowOther = otherlist.northwest;
    ImgNode* currentRowThis = northwest;
    ImgNode* lastNodeThis = nullptr;

    // Iterate over each row in otherlist.
    while (currentRowOther != nullptr) {
        ImgNode* currentNodeOther = currentRowOther;
        ImgNode* currentNodeThis = currentRowThis;

        // Iterate over each node in the current row.
        while (currentNodeOther != nullptr) {
            // Copy the current node from otherlist.
            if (currentNodeThis != currentRowThis) { // Avoid duplicating the first node of the row
                currentNodeThis = new ImgNode(*currentNodeOther);
                lastNodeThis->east = currentNodeThis;
                currentNodeThis->west = lastNodeThis;
            }

            // Link north-south if not in the first row.
            if (currentNodeThis->north != nullptr) {
                currentNodeThis->north->south = currentNodeThis;
                currentNodeThis->south = currentNodeOther->south ? new ImgNode() : nullptr;
            }

            // Prepare for next iteration in the row.
            lastNodeThis = currentNodeThis;
            currentNodeOther = currentNodeOther->east;

            // Move to the next node.
            if (currentNodeOther != nullptr) {
                currentNodeThis->east = new ImgNode();
                currentNodeThis->east->west = currentNodeThis;
                currentNodeThis = currentNodeThis->east;
            }
        }

        // Prepare for next iteration of row.
        if (currentRowOther->south != nullptr) {
            currentRowThis->south = new ImgNode();
            currentRowThis->south->north = currentRowThis;
            currentRowThis = currentRowThis->south;
        }

        // Move down to the next row.
        currentRowOther = currentRowOther->south;
    }

    // Find the southeast node (bottom-right corner of the grid).
    southeast = currentRowThis;
    while (southeast->east != nullptr) {
        southeast = southeast->east;
    }
	
}

/*************************************************************************************************
* IF YOU DEFINED YOUR OWN PRIVATE FUNCTIONS IN imglist-private.h, YOU MAY ADD YOUR IMPLEMENTATIONS BELOW *
*************************************************************************************************/
