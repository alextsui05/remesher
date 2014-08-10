#include "gtkcomboboxcheck.h"

GtkComboBoxCheck::GtkComboBoxCheck (void)
  : store(Gtk::ListStore::create(cols))
{
  this->set_model(this->store);
  this->set_row_separator_func(sigc::mem_fun
       (*this, &GtkComboBoxCheck::row_separator_func));
  this->pack_start(this->cols.active, false);
  this->pack_start(this->cols.name, true);

  this->signal_changed().connect(sigc::mem_fun
      (*this, &GtkComboBoxCheck::on_user_selected));
}

/* ---------------------------------------------------------------- */

void
GtkComboBoxCheck::set_description (std::string const& desc)
{
  this->store->clear();

  Gtk::ListStore::iterator iter = this->store->append();
  (*iter)[this->cols.index] = -1;
  (*iter)[this->cols.active] = false;
  (*iter)[this->cols.name] = desc;

  iter = this->store->append();
  (*iter)[this->cols.index] = -2;
  (*iter)[this->cols.active] = false;
  (*iter)[this->cols.name] = "";
}

/* ---------------------------------------------------------------- */

void
GtkComboBoxCheck::append_item (std::string const& name, int index, bool active)
{
  Gtk::ListStore::iterator iter = this->store->append();
  (*iter)[this->cols.index] = index;
  (*iter)[this->cols.active] = active;
  (*iter)[this->cols.name] = name;
}

/* ---------------------------------------------------------------- */

bool
GtkComboBoxCheck::row_separator_func (Glib::RefPtr<Gtk::TreeModel>
    const& /* model */, Gtk::TreeModel::iterator const& iter)
{
  return (*iter)[this->cols.name] == "";
}

/* ---------------------------------------------------------------- */

void
GtkComboBoxCheck::on_user_selected (void)
{
  Gtk::TreeModel::iterator iter = this->get_active();
  bool state = (*iter)[this->cols.active];
  int index = (*iter)[this->cols.index];

  if (index == -1)
  {
    Gtk::TreeModel::Children childs = this->store->children();
    bool sth_active = false;
    if (!childs.empty())
    {
      Gtk::TreeModel::iterator i = childs.begin();
      for (i = ++i; i != childs.end(); i++)
        if ((*i)[this->cols.active] == true)
          sth_active = true;
    }
    (*childs.begin())[this->cols.active] = sth_active;
  }

  if (index >= 0)
  {
    (*iter)[this->cols.active] = !state;
    this->set_active(0);
  }
}

/* ---------------------------------------------------------------- */

int
GtkComboBoxCheck::get_bitmask (void) const
{
  int result = 0;

  Gtk::TreeModel::Children childs = this->store->children();
  Gtk::TreeModel::iterator i = childs.begin();
  for (; i != childs.end(); i++)
  {
    int index = (*i)[this->cols.index];
    bool active = (*i)[this->cols.active];
    if (index < 0 || !active)
      continue;
    result |= (1 << index);
  }

  return result;
}
