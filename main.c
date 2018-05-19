#include "src/view/view.h"

int main(int argc, char **argv) {
  setWindow();

  rubikview mainView = generateView();

  while (1) {
    mainView.update(&mainView);
  }

  closeWindow();
  return 0;
}
