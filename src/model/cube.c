#include "cube.h"
#include "../controller/patternComparator.h"
#include "../controller/utils.h"

move inverseMove(move aMove) {
    move inverseMoves[62] = {
        Fi,Bi,Ri,Li,Ui,Di,
        fi,bi,ri,li,ui,di,
        xi,yi,zi,
        F,B,R,L,U,D,
        f,b,r,l,u,d,
        x,y,z,
        Fi2,Bi2,Ri2,Li2,Ui2,Di2,
        fi2,bi2,ri2,li2,ui2,di2,
        xi2,yi2,zi2,
        F2,B2,R2,L2,U2,D2,
        f2,b2,r2,l2,u2,d2,
        x2,y2,z2,
        RETURN, RESTART
    };

    return inverseMoves[aMove];
}

char * mapMoveToCode(move aMove){
    if ((int) aMove == -1)
        return "NONE";
    char * codes[62] = {
     "F", "B", "R", "L", "U", "D",
     "f", "b", "r", "l", "u", "d",
     "x", "y", "z",
     "Fi", "Bi", "Ri", "Li", "Ui", "Di",
     "fi", "bi", "ri", "li", "ui", "di",
     "xi", "yi", "zi",
     "F2", "B2", "R2", "L2", "U2", "D2",
     "f2", "b2", "r2", "l2", "u2", "d2",
     "x2", "y2", "z2",
     "Fi2", "Bi2", "Ri2", "Li2", "Ui2", "Di2",
     "fi2", "bi2", "ri2", "li2", "ui2", "di2",
     "xi2", "yi2", "zi2",
     "RETURN", "RESTART"
    };

    return codes[aMove];
}

move mapCodeToMove(char * moveCode){
    // 15 base moves for 60 rotations implemented
    //  (double and counter-clockwise)
    char codes[15] = {
        'F', 'B', 'R', 'L', 'U', 'D',
        'f', 'b', 'r', 'l', 'u', 'd',
        'x', 'y', 'z'
    };

    if (moveCode == NULL || moveCode[0] == '\0') {
        return -1;
    } // Invalid move code, return -1

    char moveChar = moveCode[0]; // retrieving first letter

/**
 * Returns the inverse of the given move
 *
 * If a cube is modified this way :
 * ```C
 *  move aMove = F;
 *  cube aCube = initCube();
 *  aCube->rotate(aMove);
 *  aCube->rotate(inverseMove(aMove));
 *  ```
 *  then the cube shouldn't be modified
 *
 *  @param aMove the move to be inversed
 *  @returns a move being the inverse of aMove
 */
move inverseMove(move aMove);

    // Mapping letter to base code
    move code = F-1;
    // increment code while it doesn't match
    while(++code < Fi && codes[code] != moveChar);

    // Exception handling of unknown move
    if (code == Fi) {
        return -1;
    }

    if (moveCode[1] == 'i' || moveCode[1] == '\'') {
        // Is rotation counter-clockwise ?
        code += 15;     // If it is, offset code of 15
    }

    if (moveCode[1] == '2' || (moveCode[1] == 'i' && moveCode[2] == '2')) {
        code += 30;   // If a double move is wanted, offset code of 30
    }

    return (move) code;
}

cube * initCube() {
    // Initialization of cube
    cube * self = (cube *) ec_malloc(sizeof(cube)); // Memory allocation
    self->rotate = &rotate;
    self->copy = &copyCube;
    self->equals = &cubeIsEqual;
    self->print = &printCube;                    // Public method attachment

    // Cube colorization
    unsigned char color[6] = {'g','b','r','o','w','y'};
    for (int faceIndex = F ; faceIndex <= D ; faceIndex++){
        unsigned char ** newFace = (unsigned char **) \
            ec_malloc(sizeof(unsigned char *)*3);    // Face memory allocation
        for (int index = 0; index < 3 ; index++) {
            newFace[index] = (unsigned char *) \
                             ec_malloc(sizeof(unsigned char)*3);
        } // Rows memory allocation
        self->cube[faceIndex] = newFace;

        // Face color filling
        for (int index = 0 ; index < 3 ; index++){
            for (int jindex = 0 ; jindex < 3 ; jindex++){
                self->cube[faceIndex][index][jindex] = color[faceIndex];
            }
        }
    }
    return self;
}

/////////////////////////////// PRIVATE FUNCTIONS ////////////////////////////

/**
 * Factored function to rotate the current face matrix.
 *
 * Only factored function in rotation. Current face may be rotated the same way
 * for any given face. But all edges translations are specific.
 *
 * @param self Pointer to the cube subject of rotation
 * @param current Index of face to be rotated
 * @returns Pointer to self
 */
cube * rotateCurrentFace(cube * self, int current){
    char swap;
    swap = self->cube[current][0][2];
    self->cube[current][0][2] = self->cube[current][0][0];
    self->cube[current][0][0] = swap;

    swap = self->cube[current][1][2];
    self->cube[current][1][2] = self->cube[current][0][1];
    self->cube[current][0][1] = swap;

    swap = self->cube[current][2][2];
    self->cube[current][2][2] = self->cube[current][0][0];
    self->cube[current][0][0] = swap;

    swap = self->cube[current][0][1];
    self->cube[current][0][1] = self->cube[current][1][0];
    self->cube[current][1][0] = swap;

    swap = self->cube[current][2][1];
    self->cube[current][2][1] = self->cube[current][1][0];
    self->cube[current][1][0] = swap;

    swap = self->cube[current][2][0];
    self->cube[current][2][0] = self->cube[current][0][0];
    self->cube[current][0][0] = swap;

    return self;
}

/**
 * Complementary function to rotateCurrentFace.
 * Useful for full cube rotations
 * @see rotateCurrentFace()
 */
cube * rotateCurrentFaceCCLW(cube * self, int current) {
    char swap[3];
    swap[0] = self->cube[current][0][0];
    swap[1] = self->cube[current][0][1];
    swap[2] = self->cube[current][0][2];

    self->cube[current][0][1] = self->cube[current][1][2];
    self->cube[current][0][2] = self->cube[current][2][2];

    self->cube[current][2][2] = self->cube[current][2][0];
    self->cube[current][1][2] = self->cube[current][2][1];

    self->cube[current][2][1] = self->cube[current][1][0];

    self->cube[current][2][0] = swap[0];
    self->cube[current][1][0] = swap[1];
    self->cube[current][0][0] = swap[2];
    return self;
}

    /////////// Unary rotation functions /////////////////////////////////

/**
 * Functions for specific rotations.
 * Each function uses rotateCurrentFace for the main rotation, then each
 * implements its own edges translations
 *
 * If not specified, each rotation is clockwise, if the face was in front of
 * the eyes of the person performing the rotation.
 *
 * Each rotation may be indiced by `i` meaning that the move will be
 * counter-clockwise rather than clockwise.
 *
 * Rotations are :
 * * F : Front face
 * * B : Back face
 * * R : Right face
 * * L : Left face
 * * U : Up face
 * * D : Down face
 * * f : The two front layers
 * * b : the two back layers
 * * r : the two right layers
 * * u : the two up layers
 * * d : the two down layers
 * * x : full cube rotation in the same angle as R
 * * y : full cube rotation in the same angle as U
 * * z : full cube rotation in the same angle as F
 *
 * @param self Reference to the cube to be rotated
 * @returns Reference to the rotated cube
 */
cube * rotateF(cube * self){
    char swap[3];
    self = rotateCurrentFace(self, F);

    swap[0] = self->cube[U][2][0];
    swap[1] = self->cube[U][2][1];
    swap[2] = self->cube[U][2][2];


    self->cube[U][2][0] = self->cube[L][2][2];
    self->cube[U][2][1] = self->cube[L][1][2];
    self->cube[U][2][2] = self->cube[L][0][2];


    self->cube[L][2][2] = self->cube[D][0][2];
    self->cube[L][1][2] = self->cube[D][0][1];
    self->cube[L][0][2] = self->cube[D][0][0];

    self->cube[D][0][2] = self->cube[R][0][0];
    self->cube[D][0][1] = self->cube[R][1][0];
    self->cube[D][0][0] = self->cube[R][2][0];

    self->cube[R][0][0] = swap[0];
    self->cube[R][1][0] = swap[1];
    self->cube[R][2][0] = swap[2];
    return self;
}

cube * rotateB(cube * self){
    char swap[3];
    self = rotateCurrentFace(self, B);

    swap[0] = self->cube[U][0][2];
    swap[1] = self->cube[U][0][1];
    swap[2] = self->cube[U][0][0];

    self->cube[U][0][2] = self->cube[R][2][2];
    self->cube[U][0][1] = self->cube[R][1][2];
    self->cube[U][0][0] = self->cube[R][0][2];

    self->cube[R][2][2] = self->cube[D][2][0];
    self->cube[R][1][2] = self->cube[D][2][1];
    self->cube[R][0][2] = self->cube[D][2][2];

    self->cube[D][2][0] = self->cube[L][0][0];
    self->cube[D][2][1] = self->cube[L][1][0];
    self->cube[D][2][2] = self->cube[L][2][0];

    self->cube[L][0][0] = swap[0];
    self->cube[L][1][0] = swap[1];
    self->cube[L][2][0] = swap[2];
    return self;
}

cube * rotateR(cube * self){
    char swap[3];
    self = rotateCurrentFace(self, R);

    swap[0] = self->cube[U][2][2];
    swap[1] = self->cube[U][1][2];
    swap[2] = self->cube[U][0][2];

    self->cube[U][2][2] = self->cube[F][2][2];
    self->cube[U][1][2] = self->cube[F][1][2];
    self->cube[U][0][2] = self->cube[F][0][2];

    self->cube[F][2][2] = self->cube[D][2][2];
    self->cube[F][1][2] = self->cube[D][1][2];
    self->cube[F][0][2] = self->cube[D][0][2];

    self->cube[D][2][2] = self->cube[B][0][0];
    self->cube[D][1][2] = self->cube[B][1][0];
    self->cube[D][0][2] = self->cube[B][2][0];

    self->cube[B][0][0] = swap[0];
    self->cube[B][1][0] = swap[1];
    self->cube[B][2][0] = swap[2];
    return self;
}

cube * rotateL(cube * self){
    char swap[3];
    self = rotateCurrentFace(self, L);

    swap[0] = self->cube[U][0][0];
    swap[1] = self->cube[U][1][0];
    swap[2] = self->cube[U][2][0];

    self->cube[U][0][0] = self->cube[B][2][2];
    self->cube[U][1][0] = self->cube[B][1][2];
    self->cube[U][2][0] = self->cube[B][0][2];

    self->cube[B][2][2] = self->cube[D][0][0];
    self->cube[B][1][2] = self->cube[D][1][0];
    self->cube[B][0][2] = self->cube[D][2][0];

    self->cube[D][0][0] = self->cube[F][0][0];
    self->cube[D][1][0] = self->cube[F][1][0];
    self->cube[D][2][0] = self->cube[F][2][0];

    self->cube[F][0][0] = swap[0];
    self->cube[F][1][0] = swap[1];
    self->cube[F][2][0] = swap[2];
    return self;
}

cube * rotateU(cube * self){
    char swap[3];
    self = rotateCurrentFace(self, U);

    swap[0] = self->cube[B][0][2];
    swap[1] = self->cube[B][0][1];
    swap[2] = self->cube[B][0][0];

    self->cube[B][0][2] = self->cube[L][0][2];
    self->cube[B][0][1] = self->cube[L][0][1];
    self->cube[B][0][0] = self->cube[L][0][0];

    self->cube[L][0][2] = self->cube[F][0][2];
    self->cube[L][0][1] = self->cube[F][0][1];
    self->cube[L][0][0] = self->cube[F][0][0];

    self->cube[F][0][2] = self->cube[R][0][2];
    self->cube[F][0][1] = self->cube[R][0][1];
    self->cube[F][0][0] = self->cube[R][0][0];

    self->cube[R][0][2] = swap[0];
    self->cube[R][0][1] = swap[1];
    self->cube[R][0][0] = swap[2];
    return self;
}

cube * rotateD(cube * self){
    char swap[3];
    self = rotateCurrentFace(self, D);

    swap[0] = self->cube[B][2][0];
    swap[1] = self->cube[B][2][1];
    swap[2] = self->cube[B][2][2];

    self->cube[B][2][0] = self->cube[R][2][0];
    self->cube[B][2][1] = self->cube[R][2][1];
    self->cube[B][2][2] = self->cube[R][2][2];

    self->cube[R][2][0] = self->cube[F][2][0];
    self->cube[R][2][1] = self->cube[F][2][1];
    self->cube[R][2][2] = self->cube[F][2][2];

    self->cube[F][2][0] = self->cube[L][2][0];
    self->cube[F][2][1] = self->cube[L][2][1];
    self->cube[F][2][2] = self->cube[L][2][2];

    self->cube[L][2][0] = swap[0];
    self->cube[L][2][1] = swap[1];
    self->cube[L][2][2] = swap[2];
    return self;
}

cube * rotateFi(cube * self){
    for(int i=0; i < 3; i++)
    {
        rotateF(self);
    }

    return self;
}

cube * rotateBi(cube * self){
    for(int i=0; i < 3; i++)
    {
        rotateB(self);
    }

    return self;
}

cube * rotateRi(cube * self){
    for(int i=0; i < 3; i++)
    {
        rotateR(self);
    }

    return self;

}

cube * rotateLi(cube * self){
    for(int i=0; i < 3; i++)
    {
        rotateL(self);
    }

    return self;
}

cube * rotateUi(cube * self){
    for(int i=0; i < 3; i++)
    {
        rotateU(self);
    }

    return self;
}

cube * rotateDi(cube * self){
    for(int i=0; i < 3; i++)
    {
        rotateD(self);
    }

    return self;
}

cube * rotatex(cube * self) {
    self = rotateCurrentFace(
            rotateCurrentFaceCCLW(self, L),
            R);
    unsigned char ** faceSwap = self->cube[F];
    self->cube[F] = self->cube[D];
    self->cube[D] = self->cube[B];
    self->cube[B] = self->cube[U];
    self->cube[U] = faceSwap;

    // Functional programming, for the lolz
    self = rotateCurrentFace(
            rotateCurrentFace(
                rotateCurrentFace(
                    rotateCurrentFace(
                        self,
                        D
                        ),
                    D
                    ),
                B
                ),
            B
            ); // The face swap changed the orientation of certain faces
               // which need to be straighten
    return self;
}

cube * rotatey(cube * self) {
    self = rotateCurrentFace(
            rotateCurrentFaceCCLW(self, D),
            U);
    unsigned char ** faceSwap = self->cube[F];
    self->cube[F] = self->cube[R];
    self->cube[R] = self->cube[B];
    self->cube[B] = self->cube[L];
    self->cube[L] = faceSwap;

    return self;
}

cube * rotatez(cube * self) {
    self = rotateCurrentFace(
            rotateCurrentFaceCCLW(self, B),
            F);
    unsigned char ** faceSwap = self->cube[U];
    self->cube[U] = self->cube[L];
    self->cube[L] = self->cube[D];
    self->cube[D] = self->cube[R];
    self->cube[R] = faceSwap;

    self = rotateCurrentFace(
            rotateCurrentFace(
                rotateCurrentFace(
                    rotateCurrentFace(
                        self,
                        D
                        ),
                    R
                    ),
                L
                ),
            U
            ); // The face swap changed the orientation of certain faces
               // which need to be straighten
    return self;
}

cube * rotatexi(cube * self) {
    for(int index = 0 ; index < 3 ; index++) {
        rotatex(self);
    }

    return self;
}

cube * rotateyi(cube * self) {
    for(int index = 0 ; index < 3 ; index++) {
        rotatey(self);
    }

    return self;
}

cube * rotatezi(cube * self) {
    for(int index = 0 ; index < 3 ; index++) {
        rotatez(self);
    }

    return self;
}

cube * rotatef(cube * self) {
    return rotatez(
            rotateB(self)
            );
}

cube * rotateb(cube * self) {
    return rotatezi(
            rotateF(self)
            );
}

cube * rotater(cube * self) {
    return rotatex(
            rotateL(self)
            );
}

cube * rotatel(cube * self) {
    return rotatexi(
            rotateR(self)
            );
}

cube * rotateu(cube * self) {
    return rotatey(
            rotateD(self)
            );
}

cube * rotated(cube * self) {
    return rotateyi(
            rotateU(self)
            );
}

cube * rotatefi(cube * self){
    return rotateBi(
            rotatezi(self)
            );
}

cube * rotatebi(cube * self){
    return rotateFi(
            rotatez(self)
            );
}

cube * rotateri(cube * self){
    return rotateLi(
            rotatexi(self)
            );
}

cube * rotateli(cube * self){
    return rotateRi(
            rotatex(self)
            );
}

cube * rotateui(cube * self){
    return rotateDi(
            rotateyi(self)
            );
}

cube * rotatedi(cube * self){
    return rotateUi(
            rotatey(self)
            );
}

///////////////////////////////////////////////////////////////////////////////

////////////////////  PUBLIC API OF CUBE DATA STRUCT /////////////////////////
cube * rotate(cube * self, move moveCode) {
    _Bool doubleMove = false;

    // Retrieving code of move
    if (moveCode >= 30) {
        doubleMove = true;
        moveCode -= 30;
    } // If move is a double move, switch boolean and remove offset

    cube * (* chosenMoveFn)(cube *);

    // Scary-ass switch statement : chooses correct private function
    switch(moveCode){
        case(F):
            chosenMoveFn = &rotateF;
            break;
        case(B):
            chosenMoveFn = &rotateB;
            break;
        case(R):
            chosenMoveFn = &rotateR;
            break;
        case(L):
            chosenMoveFn = &rotateL;
            break;
        case(U):
            chosenMoveFn = &rotateU;
            break;
        case(D):
            chosenMoveFn = &rotateD;
            break;
        case(Fi):
            chosenMoveFn = &rotateFi;
            break;
        case(Bi):
            chosenMoveFn = &rotateBi;
            break;
        case(Ri):
            chosenMoveFn = &rotateRi;
            break;
        case(Li):
            chosenMoveFn = &rotateLi;
            break;
        case(Ui):
            chosenMoveFn = &rotateUi;
            break;
        case(Di):
            chosenMoveFn = &rotateDi;
            break;
        case(x):
            chosenMoveFn = &rotatex;
            break;
        case(y):
            chosenMoveFn = &rotatey;
            break;
        case(z):
            chosenMoveFn = &rotatez;
            break;
        case(xi):
            chosenMoveFn = &rotatexi;
            break;
        case(yi):
            chosenMoveFn = &rotateyi;
            break;
        case(zi):
            chosenMoveFn = &rotatezi;
            break;
        case(f):
            chosenMoveFn = &rotatef;
            break;
        case(b):
            chosenMoveFn = &rotateb;
            break;
        case(r):
            chosenMoveFn = &rotater;
            break;
        case(l):
            chosenMoveFn = &rotatel;
            break;
        case(u):
            chosenMoveFn = &rotateu;
            break;
        case(d):
            chosenMoveFn = &rotated;
            break;
        case(fi):
            chosenMoveFn = &rotatefi;
            break;
        case(bi):
            chosenMoveFn = &rotatebi;
            break;
        case(ri):
            chosenMoveFn = &rotateri;
            break;
        case(li):
            chosenMoveFn = &rotateli;
            break;
        case(ui):
            chosenMoveFn = &rotateui;
            break;
        case(di):
            chosenMoveFn = &rotatedi;
            break;
        default:
            // If you end up here, something is very very wrong
            exitFatal(" in rotate(), no such operation");
            break;
    }

    if (doubleMove) {
        chosenMoveFn(self); // One more if the move is double
    }

    return chosenMoveFn(self);
}

cube * copyCube(cube * self) {
    cube * newCube = initCube();

    for (int faceIndex = F ; faceIndex <= D ; faceIndex++) {
        for (int index = 0; index < 3 ; index++) {
            for (int jindex = 0; jindex < 3 ; jindex++) {
                newCube->cube[faceIndex][index][jindex] =
                    self->cube[faceIndex][index][jindex];
            }
        }
    }

    return newCube;
}

_Bool cubeIsEqual(cube * self, cube * otherCube){
    // Copying data to avoid modification
    cube * aCube = copyCube(self);
    cube * bCube = copyCube(otherCube);
    redressCube(aCube);
    redressCube(bCube);

    for(int face = F; face <= D ; face++){
        for(int index = 0; index < 3 ; index++){
            for(int jindex = 0; jindex < 3 ; jindex++){
                if (aCube->cube[face][index][jindex] !=
                        bCube->cube[face][index][jindex]){
                    return false; // If any face of a cubelet does not match
                                  // cubes are not equal
                }
            }
        }
    }

    // Destroying copies
    destroyCube(aCube);
    destroyCube(bCube);

    return true;
}

void printCube(cube * self){
    //debug("Entering printCube()");
    if (self == NULL)
        fprintf(stderr, "self Null pointer\n");
    if (self->cube == NULL)
        fprintf(stderr, "self->cube Null pointer\n");

    for (int i = 0 ; i < 6 ; i++) {
        if(self->cube[i] == NULL)
            fprintf(stderr, "Face is NULL\n");
        for(int j = 0 ; j < 3 ; j++) {
            if (self->cube[i][j] == NULL)
                fprintf(stderr, "Line is NULL\n");
        }
    }



    // Hello, I'm a fprintf statement with issues
    fprintf(stderr, "\n        |%c|%c|%c|\n        |%c|%c|%c|\n        |%c|%c|%c|\n\
 |%c|%c|%c||%c|%c|%c||%c|%c|%c||%c|%c|%c|\n\
 |%c|%c|%c||%c|%c|%c||%c|%c|%c||%c|%c|%c|\n\
 |%c|%c|%c||%c|%c|%c||%c|%c|%c||%c|%c|%c|\n\
        |%c|%c|%c|\n        |%c|%c|%c|\n        |%c|%c|%c|\n\n",
            self->cube[U][0][0],self->cube[U][0][1], self->cube[U][0][2],
            self->cube[U][1][0],self->cube[U][1][1], self->cube[U][1][2],
            self->cube[U][2][0],self->cube[U][2][1], self->cube[U][2][2],
            self->cube[L][0][0],self->cube[L][0][1], self->cube[L][0][2],
            self->cube[F][0][0],self->cube[F][0][1], self->cube[F][0][2],
            self->cube[R][0][0],self->cube[R][0][1], self->cube[R][0][2],
            self->cube[B][0][0],self->cube[B][0][1], self->cube[B][0][2],
            self->cube[L][1][0],self->cube[L][1][1], self->cube[L][1][2],
            self->cube[F][1][0],self->cube[F][1][1], self->cube[F][1][2],
            self->cube[R][1][0],self->cube[R][1][1], self->cube[R][1][2],
            self->cube[B][1][0],self->cube[B][1][1], self->cube[B][1][2],
            self->cube[L][2][0],self->cube[L][2][1], self->cube[L][2][2],
            self->cube[F][2][0],self->cube[F][2][1], self->cube[F][2][2],
            self->cube[R][2][0],self->cube[R][2][1], self->cube[R][2][2],
            self->cube[B][2][0],self->cube[B][2][1], self->cube[B][2][2],
            self->cube[D][0][0],self->cube[D][0][1], self->cube[D][0][2],
            self->cube[D][1][0],self->cube[D][1][1], self->cube[D][1][2],
            self->cube[D][2][0],self->cube[D][2][1], self->cube[D][2][2]);
            fflush(stderr);
            fflush(stdout);
}
//////////////////////////////////////////////////////////////////////////////

// Public method
// Should be static in object ?
void destroyCube(cube * self) {
    for (int faceIndex = F ; faceIndex <= D ; faceIndex++) {
        unsigned char ** oldFace = self->cube[faceIndex];
        for (int index = 0; index < 3 ; index++) {
            unsigned char * oldRow = oldFace[index];
            free(oldRow);
        }
        free(oldFace);
    }
    free(self);
    return;
}

cube* voidCube(cube *self){
    for (int faceIndex = F ; faceIndex < D+1 ; faceIndex++){
        for (int index = 0 ; index < 3 ; index++){
            for (int jindex = 0 ; jindex < 3 ; jindex++){
                    if(jindex == 1 && index == 1){
                    }
                    else{
        self->cube[faceIndex][index][jindex] = ' ';
                    }
            }
        }
    }
      return self;
}
