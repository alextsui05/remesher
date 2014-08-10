#include <iostream>
#include <gtkmm/table.h>

#include "gtkdefines.h"
#include "gtkmeshrendering.h"

GtkMeshRendering::GtkMeshRendering (void)
{
  this->draw_model_cb.set_label("Render this model");

  this->shading_combo.append_text("No shading");
  this->shading_combo.append_text("Flat shading");
  this->shading_combo.append_text("Vertex shading");

  this->debug_combo.append_text("No debugging");
  this->debug_combo.append_text("Draw vertex IDs");
  this->debug_combo.append_text("Draw face IDs");

  this->deco_combo.set_description("Mesh decorations...");
  this->deco_combo.append_item("Points for vertices", 0, true);
  this->deco_combo.append_item("Draw triangle lines", 1, true);
  this->deco_combo.append_item("Draw voronoi regions", 2, false);
  this->deco_combo.append_item("Draw vertex normals", 3, false);
  this->deco_combo.append_item("Draw face normals", 4, false);
  this->deco_combo.append_item("Draw face ordering", 5, false);
  this->deco_combo.append_item("Draw vertex regularity", 6, false);
  this->deco_combo.set_active(0);

  Gtk::Table* color_table = MK_TABLE(2, 2);
  color_table->attach(*MK_LABEL("Decoration"), 0, 1, 0, 1);
  color_table->attach(*MK_LABEL("Shading"), 1, 2, 0, 1);
  color_table->attach(this->deco_color, 0, 1, 1, 2);
  color_table->attach(this->shade_color, 1, 2, 1, 2);

  this->pack_start(this->draw_model_cb, false, false, 0);
  this->pack_start(this->shading_combo, false, false, 0);
  this->pack_start(this->deco_combo, false, false, 0);
  this->pack_start(this->debug_combo, false, false, 0);
  this->pack_start(*color_table, false, false, 0);

  this->draw_model_cb.signal_clicked().connect(sigc::mem_fun
      (*this, &GtkMeshRendering::on_draw_model_toggled));
  this->shading_combo.signal_changed().connect(sigc::mem_fun
      (*this, &GtkMeshRendering::on_shading_changed));
  this->deco_combo.signal_changed().connect(sigc::mem_fun
      (*this, &GtkMeshRendering::on_decorations_changed));
  this->debug_combo.signal_changed().connect(sigc::mem_fun
      (*this, &GtkMeshRendering::on_debug_changed));
  this->shade_color.signal_color_set().connect(sigc::mem_fun
      (*this, &GtkMeshRendering::on_shade_color_set));
  this->deco_color.signal_color_set().connect(sigc::mem_fun
      (*this, &GtkMeshRendering::on_deco_color_set));
  this->feature_color.signal_color_set().connect(sigc::mem_fun
      (*this, &GtkMeshRendering::on_feature_color_set));
}

/* ---------------------------------------------------------------- */

void
GtkMeshRendering::initialize (void)
{
  this->draw_model_cb.set_active(this->glmesh->get_visible());
  this->shading_combo.set_active(this->glmesh->get_mesh_shading());
  this->debug_combo.set_active(0);
}

/* ---------------------------------------------------------------- */

Gdk::Color
GtkMeshRendering::get_color (float r, float g, float b)
{
  Gdk::Color color;
  color.set_rgb((unsigned short)(r * 65535.0f),
      (unsigned short)(g * 65535.0f), (unsigned short)(b * 65535.0f));
  return color;
}

/* ---------------------------------------------------------------- */

void
GtkMeshRendering::set_shade_color (float r, float g, float b, float a)
{
  this->shade_color.set_use_alpha(true);
  this->shade_color.set_alpha((unsigned short)(a * 65535.0f));
  this->shade_color.set_color(this->get_color(r, g, b));
  this->on_shade_color_set();
}

/* ---------------------------------------------------------------- */

void
GtkMeshRendering::set_deco_color (float r, float g, float b, float a)
{
  this->deco_color.set_use_alpha(true);
  this->deco_color.set_alpha((unsigned short)(a * 65535.0f));
  this->deco_color.set_color(this->get_color(r, g, b));
  this->on_deco_color_set();
}

/* ---------------------------------------------------------------- */

void
GtkMeshRendering::set_feature_color (float r, float g, float b, float a)
{
  this->feature_color.set_use_alpha(true);
  this->feature_color.set_alpha((unsigned short)(a * 65535.0f));
  this->feature_color.set_color(this->get_color(r, g, b));
  this->on_feature_color_set();
}

/* ---------------------------------------------------------------- */

void
GtkMeshRendering::on_draw_model_toggled (void)
{
  bool visible = this->draw_model_cb.get_active();
  this->glmesh->set_visible(visible);
  this->gldebug->set_visible(visible);
  this->glfeatures->set_visible(visible);
  this->sig_redraw.emit();
}

/* ---------------------------------------------------------------- */

void
GtkMeshRendering::on_shading_changed (void)
{
  this->glmesh->set_mesh_shading((GLMeshShading)
      this->shading_combo.get_active_row_number());
  this->sig_redraw.emit();
}

/* ---------------------------------------------------------------- */

void
GtkMeshRendering::on_decorations_changed (void)
{
  int deco_mask = this->deco_combo.get_bitmask();
  this->glmesh->set_mesh_decoration(deco_mask);
  this->sig_redraw.emit();
}

/* ---------------------------------------------------------------- */

void
GtkMeshRendering::on_debug_changed (void)
{
  this->gldebug->set_render_type((GLDebugRenderType)
      this->debug_combo.get_active_row_number());
  this->sig_redraw.emit();
}

/* ---------------------------------------------------------------- */

void
GtkMeshRendering::on_shade_color_set (void)
{
  Gdk::Color color = this->shade_color.get_color();
  this->glmesh->set_shade_color
      ((float)color.get_red() / 65535.0f,
      (float)color.get_green() / 65535.0f,
      (float)color.get_blue() / 65535.0f,
      (float)this->shade_color.get_alpha() / 65535.0f);
  this->sig_redraw.emit();
}

/* ---------------------------------------------------------------- */

void
GtkMeshRendering::on_deco_color_set (void)
{
  Gdk::Color color = this->deco_color.get_color();
  this->glmesh->set_decoration_color
      ((float)color.get_red() / 65535.0f,
      (float)color.get_green() / 65535.0f,
      (float)color.get_blue() / 65535.0f,
      (float)this->deco_color.get_alpha() / 65535.0f);
  this->sig_redraw.emit();
}

/* ---------------------------------------------------------------- */

void
GtkMeshRendering::on_feature_color_set (void)
{
  Gdk::Color color = this->feature_color.get_color();
  this->glfeatures->set_line_color
      ((float)color.get_red() / 65535.0f,
      (float)color.get_green() / 65535.0f,
      (float)color.get_blue() / 65535.0f,
      (float)this->feature_color.get_alpha() / 65535.0f);
  this->sig_redraw.emit();
}
