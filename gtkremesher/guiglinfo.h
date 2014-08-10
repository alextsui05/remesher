#ifndef GUI_GL_INFO_HEADER
#define GUI_GL_INFO_HEADER

#include <gtkmm/label.h>
#include <gtkmm/treeview.h>
#include <gtkmm/liststore.h>

#include "winbase.h"

class GuiGLInfo : public WinBase
{
  private:
    class TreeViewCols : public Gtk::TreeModelColumnRecord
    {
      public:
        Gtk::TreeModelColumn<Glib::ustring> name;

	TreeViewCols (void)
	{ this->add(this->name); }
    } ext_store_cols;

  private:
    Gtk::Label vendor;
    Gtk::Label renderer;
    Gtk::Label version;
    Glib::RefPtr<Gtk::ListStore> ext_store;
    Gtk::TreeView extensions;

  public:
    GuiGLInfo (void);
    void populate (void);
};

#endif /* GUI_GL_INFO_HEADER */
