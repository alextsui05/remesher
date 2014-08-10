/*
 * This file is part of GtkEveMon.
 *
 * GtkEveMon is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * You should have received a copy of the GNU General Public License
 * along with GtkEveMon. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef GTK_DEFINES_HEADER
#define GTK_DEFINES_HEADER

#define MK_VBOX Gtk::manage(new Gtk::VBox(false, 5))
#define MK_HBOX Gtk::manage(new Gtk::HBox(false, 5))
#define MK_VBOX0 Gtk::manage(new Gtk::VBox(false, 0))
#define MK_HBOX0 Gtk::manage(new Gtk::HBox(false, 0))
#define MK_FRAME(str) Gtk::manage(new Gtk::Frame(str))
#define MK_FRAME0 Gtk::manage(new Gtk::Frame)
#define MK_LABEL(str) Gtk::manage(new Gtk::Label(str))
#define MK_LABEL_LEFT(str) Gtk::manage(new Gtk::Label(str, 0.0f))
#define MK_LABEL0 Gtk::manage(new Gtk::Label)
#define MK_HSEP Gtk::manage(new Gtk::HSeparator)
#define MK_VSEP Gtk::manage(new Gtk::VSeparator)
#define MK_BUT(ctor) Gtk::manage(new Gtk::Button(ctor))
#define MK_BUT0 Gtk::manage(new Gtk::Button)
#define MK_SCWIN Gtk::manage(new Gtk::ScrolledWindow)
#define MK_HPANED Gtk::manage(new Gtk::HPaned)
#define MK_VPANED Gtk::manage(new Gtk::VPaned)
#define MK_NOTEBOOK Gtk::manage(new Gtk::Notebook)
#define MK_ENTRY Gtk::manage(new Gtk::Entry);
#define MK_IMG(id,size) Gtk::manage(new Gtk::Image(id, size))
#define MK_IMG_PB(pixbuf) Gtk::manage(new Gtk::Image(pixbuf))
#define MK_TABLE(rows,cols) Gtk::manage(new Gtk::Table(rows, cols, false))
#define MK_CB(label) Gtk::manage(new Gtk::CheckButton(label))

#endif /* GTK_DEFINES_HEADER */
