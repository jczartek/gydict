/* gy-header-bar.c
 *
 * Copyright (C) 2016 Jakub Czartek <kuba@linux.pl>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#include "gy-header-bar.h"
#include "app/gy-app.h"

struct _GyHeaderBar
{
  GtkHeaderBar    __parent__;
  GtkMenuButton  *menu_button;
  GtkMenuButton  *dicts_button;
};

G_DEFINE_TYPE (GyHeaderBar, gy_header_bar, GTK_TYPE_HEADER_BAR)

enum {
  PROP_0,
  N_PROPS
};

static GParamSpec *properties [N_PROPS];

GyHeaderBar *
gy_header_bar_new (void)
{
  return g_object_new (GY_TYPE_HEADER_BAR, NULL);
}

static void
gy_header_bar_finalize (GObject *object)
{
  GyHeaderBar *self = (GyHeaderBar *)object;

  G_OBJECT_CLASS (gy_header_bar_parent_class)->finalize (object);
}

static void
gy_header_bar_get_property (GObject    *object,
                            guint       prop_id,
                            GValue     *value,
                            GParamSpec *pspec)
{
  GyHeaderBar *self = GY_HEADER_BAR (object);

  switch (prop_id)
    {
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}

static void
gy_header_bar_set_property (GObject      *object,
                            guint         prop_id,
                            const GValue *value,
                            GParamSpec   *pspec)
{
  GyHeaderBar *self = GY_HEADER_BAR (object);

  switch (prop_id)
    {
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}

static void
gy_header_bar_class_init (GyHeaderBarClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);

  object_class->finalize = gy_header_bar_finalize;
  object_class->get_property = gy_header_bar_get_property;
  object_class->set_property = gy_header_bar_set_property;

  gtk_widget_class_set_template_from_resource (GTK_WIDGET_CLASS (klass), "/org/gtk/gydict/gy-header-bar.ui");
  gtk_widget_class_bind_template_child (GTK_WIDGET_CLASS (klass), GyHeaderBar, dicts_button);
  gtk_widget_class_bind_template_child (GTK_WIDGET_CLASS (klass), GyHeaderBar, menu_button);
}

static void
gy_header_bar_init (GyHeaderBar *self)
{
  GtkWidget *popover;
  GMenu     *model;

  gtk_widget_init_template (GTK_WIDGET (self));

  model = gy_app_get_menu_by_id (GY_APP_DEFAULT, "dict-menu");
  popover = gtk_popover_new_from_model (NULL, G_MENU_MODEL (model));
  gtk_menu_button_set_popover (self->dicts_button, popover);

  model = gy_app_get_menu_by_id (GY_APP_DEFAULT, "gear-menu");
  popover = gtk_popover_new_from_model (NULL, G_MENU_MODEL (model));
  gtk_menu_button_set_popover (self->menu_button, popover);
}
