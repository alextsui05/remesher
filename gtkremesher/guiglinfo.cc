#include <string>

#include <GL/gl.h>

#include <gtkmm/box.h>
#include <gtkmm/table.h>
#include <gtkmm/frame.h>
#include <gtkmm/separator.h>
#include <gtkmm/button.h>
#include <gtkmm/stock.h>
#include <gtkmm/scrolledwindow.h>

#include "libremesh/helpers.h"
#include "gtkdefines.h"
#include "guiglinfo.h"

GuiGLInfo::GuiGLInfo (void)
  : ext_store(Gtk::ListStore::create(ext_store_cols)),
    extensions(ext_store)
{
  this->extensions.append_column("Extension", this->ext_store_cols.name);
  this->extensions.set_search_column(this->ext_store_cols.name);
  this->ext_store->set_sort_column(this->ext_store_cols.name,
      Gtk::SORT_ASCENDING);

  this->vendor.property_xalign() = 0.0;
  this->renderer.property_xalign() = 0.0;
  this->version.property_xalign() = 0.0;

  this->vendor.set_selectable(true);
  this->renderer.set_selectable(true);
  this->version.set_selectable(true);

  Gtk::Label* label_vendor = MK_LABEL("GL_VENDOR:");
  Gtk::Label* label_renderer = MK_LABEL("GL_RENDERER:");
  Gtk::Label* label_version = MK_LABEL("GL_VERSION:");
  label_vendor->property_xalign() = 0.0;
  label_renderer->property_xalign() = 0.0;
  label_version->property_xalign() = 0.0;

  Gtk::Table* info_table = Gtk::manage(new Gtk::Table(3, 2, false));
  info_table->set_col_spacings(5);
  info_table->set_row_spacings(5);
  info_table->attach(*label_vendor,
      0, 1, 0, 1, Gtk::SHRINK | Gtk::FILL, Gtk::SHRINK);
  info_table->attach(*label_renderer,
      0, 1, 1, 2, Gtk::SHRINK | Gtk::FILL, Gtk::SHRINK);
  info_table->attach(*label_version,
      0, 1, 2, 3, Gtk::SHRINK | Gtk::FILL, Gtk::SHRINK);
  info_table->attach(this->vendor,
      1, 2, 0, 1, Gtk::SHRINK | Gtk::FILL, Gtk::SHRINK);
  info_table->attach(this->renderer,
      1, 2, 1, 2, Gtk::SHRINK | Gtk::FILL, Gtk::SHRINK);
  info_table->attach(this->version,
      1, 2, 2, 3, Gtk::SHRINK | Gtk::FILL, Gtk::SHRINK);

  Gtk::ScrolledWindow* scwin = Gtk::manage(new Gtk::ScrolledWindow);
  scwin->set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_ALWAYS);
  scwin->add(this->extensions);

  Gtk::VBox* content_box = MK_VBOX;
  content_box->set_border_width(5);
  content_box->pack_start(*info_table, false, false, 0);
  content_box->pack_start(*scwin, true, true, 0);

  Gtk::Frame* content_frame = MK_FRAME0;
  content_frame->set_shadow_type(Gtk::SHADOW_OUT);
  content_frame->add(*content_box);

  Gtk::Button* close_but = Gtk::manage(new Gtk::Button(Gtk::Stock::CLOSE));

  Gtk::HBox* button_box = MK_HBOX;
  button_box->pack_start(*Gtk::manage(new Gtk::HSeparator), true, true, 0);
  button_box->pack_start(*close_but, false, false, 0);

  Gtk::VBox* main_vbox = MK_VBOX;
  main_vbox->set_border_width(5);
  main_vbox->pack_start(*content_frame, true, true, 0);
  main_vbox->pack_start(*button_box, false, false, 0);

  close_but->signal_clicked().connect(sigc::mem_fun(*this, &WinBase::close));

  this->populate();

  this->set_default_size(450, 450);
  this->add(*main_vbox);
  this->set_title("GL Information");
  this->show_all();
}

/* ---------------------------------------------------------------- */

void
GuiGLInfo::populate (void)
{
  this->vendor.set_text((char const*)glGetString(GL_VENDOR));
  this->renderer.set_text((char const*)glGetString(GL_RENDERER));
  this->version.set_text((char const*)glGetString(GL_VERSION));

  std::vector<std::string> exts = Remesher::Helpers::split_string
      ((char const*)glGetString(GL_EXTENSIONS), ' ');

  for (unsigned int i = 0; i < exts.size(); ++i)
  {
    Gtk::ListStore::iterator iter = ext_store->append();
    (*iter)[ext_store_cols.name] = exts[i];
  }
}
