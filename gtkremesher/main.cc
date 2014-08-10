#include <iostream>
#include <gtkmm/main.h>

#include "arguments.h"
#include "imagestore.h"
#include "maingui.h"

int
main (int argc, char** argv)
{
  Gtk::Main kit(argc, argv);
  Gtk::GL::init(argc, argv);
  Arguments::init(argc, argv);
  ImageStore::init();

  {
    MainGui gui;
    kit.run(gui);
  }

  ImageStore::unload();

  return 0;
}
