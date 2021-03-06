/**
 * @file animations.c
 */


#include "animations.h"


animationStack * generateAnimationStack(Mix_Chunk * sound) {
  animationStack * returnedStack;
  returnedStack = (animationStack *)malloc(sizeof(animationStack));
  returnedStack->animations = NULL;
  returnedStack->next = NULL;
  returnedStack->sound = sound;

  returnedStack->hasStarted = false;
  returnedStack->isFinished = false;

  returnedStack->start = &start;
  returnedStack->update = &updateAnimationStack;

  return returnedStack;
}


void addAnimationStack(animationStack ** animStack, animationStack * toAdd) {
  animationStack * currentAnimationStack = *animStack;
  if (currentAnimationStack == NULL) {
    *animStack = toAdd;
  } else {
    while (currentAnimationStack->next != NULL) {
      currentAnimationStack = currentAnimationStack->next;
    }
    currentAnimationStack->next = toAdd;
  }
}


void removeAnimationStack(animationStack ** animStack, animationStack * toRemove) {
  animationStack * currentAnimation = *animStack;
  if (currentAnimation == toRemove) {
    if (currentAnimation->next != NULL) {
      *animStack = currentAnimation->next;
    }
    else {
      *animStack = NULL;
    }
    Mix_HaltChannel(0);
    free(toRemove);
  }
  while (currentAnimation != NULL) {
    if (currentAnimation->next == toRemove) {
      currentAnimation->next = toRemove->next;
      updateAnimationList(&toRemove->animations);
      Mix_HaltChannel(0);
      free(toRemove);
      break;
    }
    currentAnimation = currentAnimation->next;
  }
}


int animationStackCount(animationStack * animStack) {
  int count = 0;
  if (animStack != NULL) {
    animationStack * currentAnimationStack = animStack;
    while (currentAnimationStack != NULL) {
      count++;
      currentAnimationStack = currentAnimationStack->next;
    }
  }
  return count;
}


void updateAnimationStack(animationStack * self, rubikcube * rubikCube) {
  animation * animationsPtr = self->animations;
  self->isFinished = true;
  while (animationsPtr != NULL) {
    animationsPtr->update(animationsPtr, rubikCube);
    self->isFinished = animationsPtr->isActive ? false : self->isFinished;
    animationsPtr = animationsPtr->next;
  }
}


animation * generateAnimation(enum FaceType animatedFace, int sliceIndex, float increment, bool ccw, void (* onFinished)(rubikcube * cube, int sliceIndex, bool ccw)) {
  animation * returnedAnimation;
  returnedAnimation = (animation *)malloc(sizeof(animation));

  returnedAnimation->currentStep = 0;
  returnedAnimation->targetStep = roundf((PI / 2.0) / increment);
  returnedAnimation->animatedFace = animatedFace;
  returnedAnimation->isActive = false;
  returnedAnimation->sliceIndex = sliceIndex;
  returnedAnimation->rotationAngle = increment;
  returnedAnimation->next = NULL;
  returnedAnimation->ccw = ccw;

  returnedAnimation->onFinished = onFinished;

  switch(animatedFace) {
    case TOP:
    case DOWN:
      returnedAnimation->update = &animateZ;
      break;
    case RIGHT:
    case LEFT:
    case MIDDLE:
      returnedAnimation->update = &animateX;
      break;
    case FRONT:
    case BACK:
      returnedAnimation->update = &animateY;
      break;
  }

  return returnedAnimation;
}


void addAnimation(animation ** animations, animation * toAdd) {
  animation * currentAnimation = *animations;
  if (currentAnimation == NULL) {
    *animations = toAdd;
  } else {
    while (currentAnimation->next != NULL) {
      currentAnimation = currentAnimation->next;
    }
    currentAnimation->next = toAdd;
  }
  toAdd->isActive = true;
}


void removeAnimation(animation ** animations, animation * toRemove) {
  animation * currentAnimation = *animations;
  if (currentAnimation == toRemove) {
    *animations = NULL;
  }
  while (currentAnimation != NULL) {
    if (currentAnimation->next == toRemove) {
      currentAnimation->next = toRemove->next;
      free(toRemove);
      break;
    }
    currentAnimation = currentAnimation->next;
  }
}


void updateAnimationList(animation ** animations) {
  animation * currentAnimation = *animations;
  while (currentAnimation != NULL) {
    if (!currentAnimation->isActive) {
      removeAnimation(animations, currentAnimation);
    }
    currentAnimation = currentAnimation->next;
  }
}


void animateX(animation * self, rubikcube * rubikCube) {
  if (self->currentStep == self->targetStep) {
    self->isActive = false;
    self->onFinished(rubikCube, self->sliceIndex, self->ccw);
  } else {
    for (int xIndex = 0; xIndex < 3; xIndex++) {
      for (int yIndex = 0; yIndex < 3; yIndex++) {
        for (int faceIndex = 0; faceIndex < 6; faceIndex++) {
          rotateFaceX(&rubikCube->cubes[self->sliceIndex][xIndex][yIndex]->faces[faceIndex], self->rotationAngle, self->ccw);
        }
      }
    }
    self->currentStep++;
  }
}


void animateY(animation * self, rubikcube * rubikCube) {
  if (self->currentStep == self->targetStep) {
    self->isActive = false;
    self->onFinished(rubikCube, self->sliceIndex, self->ccw);
  } else {
    for (int xIndex = 0; xIndex < 3; xIndex++) {
      for (int yIndex = 0; yIndex < 3; yIndex++) {
        for (int faceIndex = 0; faceIndex < 6; faceIndex++) {
          rotateFaceY(&rubikCube->cubes[xIndex][self->sliceIndex][yIndex]->faces[faceIndex], self->rotationAngle, self->ccw);
        }
      }
    }
    self->currentStep++;
  }
}


void animateZ(animation * self, rubikcube * rubikCube) {
  if (self->currentStep == self->targetStep) {
    self->isActive = false;
    self->onFinished(rubikCube, self->sliceIndex, self->ccw);
  } else {
    for (int xIndex = 0; xIndex < 3; xIndex++) {
      for (int yIndex = 0; yIndex < 3; yIndex++) {
        for (int faceIndex = 0; faceIndex < 6; faceIndex++) {
          rotateFaceZ(&rubikCube->cubes[xIndex][yIndex][self->sliceIndex]->faces[faceIndex], self->rotationAngle, self->ccw);
        }
      }
    }
    self->currentStep++;
  }
}


void start(animationStack * self, Mix_Chunk * sound) {
  Mix_PlayChannel(0, sound, 0);
  self->hasStarted = true;
}
