#include "solver.h"

move * fakeSolve(move * initSequence, mvstack history) {
    mvstack temp = initQueue();
    move * solvesequence;
    move currmove;
    int currSize;
    int moveNb;
    int index;

    // Allocating space for at least the endmark
    currSize = 1;
    solvesequence = (move *) ec_malloc(sizeof(move)*currSize);

    // Emptying stack in temp stack and saving inverse moves to new array
    // Stop when currmove = -1
    currmove = -1;
    index = 0;
    moveNb = 0;
    while((int) (currmove = pop(history)) != -1 ) {
        moveNb += 1; // While we found new moves, increment count

        if (currSize <= moveNb + 1) {
            currSize *= 2;
            solvesequence =
                (move * ) ec_realloc(solvesequence, sizeof(move) * currSize);
        } // Resizing array if needed

        push(temp, currmove); // Saving popped move in temp stack
        solvesequence[index++] = inverseMove(currmove);
    }

    // MHGA : Making history great again
    while(!isEmpty(temp)) {
        currmove = pop(temp);
        push(history, currmove);
    }

    freeQueue(temp);
    temp = initQueue(); // Just to be on the safe side

    // LIFOing initSequence into new temp stack
    int jindex = -1;
    while((int) (currmove = initSequence[++jindex]) != -1) {
        push(temp, currmove);
    }
    // Empty temp stack and saving inverse moves to array
    while(!isEmpty(temp)) {
        moveNb += 1;
        currmove = pop(temp);
        if (currSize <= moveNb +1) {
            currSize *= 2;
            solvesequence =
                (move *) ec_realloc(solvesequence, sizeof(move) * currSize);
        }
        solvesequence[index] = inverseMove(currmove);
        index += 1;
    }
    solvesequence[index] = -1;
    freeQueue(temp);
    return solvesequence;
}

