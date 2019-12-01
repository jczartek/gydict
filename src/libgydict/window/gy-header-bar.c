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

#include <dazzle.h>
#include "gy-header-bar.h"

struct _GyHeaderBar
{
  GtkHeaderBar    __parent__;
  GtkMenuButton  *menu_button;
  GtkMenuButton  *dicts_button;

  GtkBox *primary;
  GtkBox *secondary;

  gchar *title;
};

enum
{
  PROP_TITLE = 1,
  N_PROPERTIES
};

G_DEFINE_TYPE (GyHeaderBar, gy_header_bar, GTK_TYPE_HEADER_BAR)

static void
gy_header_bar_set_property (GObject      *object,
                            guint         property_id,
                            const GValue *value,
                            GParamSpec   *pspec)
{
  GyHeaderBar *self = (GyHeaderBar *) object;

  switch (property_id)
    {
    case PROP_TITLE:
        {
          if (self->title != NULL)
            g_free (self->title);

          self->title = g_value_dup_string (value);
          break;
        }
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
      break;
    }
}

static void
gy_header_bar_get_property (GObject    *object,
                            guint       property_id,
                            GValue     *value,
                            GParamSpec *pspec)
{
    GyHeaderBar *self = (GyHeaderBar *) object;

  switch (property_id)
    {
    case PROP_TITLE:
        {
          if (self->title != NULL)
            g_value_set_string (value, self->title);
          break;
        }
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
      break;
    }
}

static void
gy_header_bar_class_init (GyHeaderBarClass *klass)
{

  GObjectClass *object_class = G_OBJECT_CLASS (klass);

  object_class->set_property = gy_header_bar_set_property;
  object_class->get_property = gy_header_bar_get_property;

  gtk_widget_class_set_template_from_resource (GTK_WIDGET_CLASS (klass), "/org/gtk/gydict/gy-header-bar.ui");
  gtk_widget_class_bind_template_child (GTK_WIDGET_CLASS (klass), GyHeaderBar, dicts_button);
  gtk_widget_class_bind_template_child (GTK_WIDGET_CLASS (klass), GyHeaderBar, menu_button);
  gtk_widget_class_bind_template_child (GTK_WIDGET_CLASS (klass), GyHeaderBar, primary);
  gtk_widget_class_bind_template_child (GTK_WIDGET_CLASS (klass), GyHeaderBar, secondary);

  g_object_class_override_property (object_class, PROP_TITLE, "title");
}

static void
gy_header_bar_init (GyHeaderBar *self)
{
  GtkWidget *popover;
  GMenu     *model;

  gtk_widget_init_template (GTK_WIDGET (self));

  model = dzl_application_get_menu_by_id (DZL_APPLICATION (g_application_get_default ()), "dict-menu");
  popover = gtk_popover_new_from_model (NULL, G_MENU_MODEL (model));
  gtk_widget_set_size_request (popover, 250, -1);
  gtk_menu_button_set_popover (self->dicts_button, popover);

  model = dzl_application_get_menu_by_id (DZL_APPLICATION (g_application_get_default ()), "gear-menu");
  popover = gtk_popover_new_from_model (NULL, G_MENU_MODEL (model));
  gtk_widget_set_size_request (popover, 225, -1);
  gtk_menu_button_set_popover (self->menu_button, popover);
}

/**
 * gy_header_bar_add_primary:
 * @self: a #GyHeaderBar
 *
 * Adds a widget to the primary button section of the workspace header.
 * This is the left, for LTR languages.
 *
 * Since: 0.6
 */
void
gy_header_bar_add_primary (GyHeaderBar *self,
                           GtkWidget   *widget)
{
  g_return_if_fail (GY_IS_HEADER_BAR (self));
  g_return_if_fail (GTK_IS_WIDGET (widget));

  gtk_container_add (GTK_CONTAINER (self->primary), widget);
}

void
gy_header_bar_add_center_left (GyHeaderBar *self,
                               GtkWidget   *child)
{

  g_return_if_fail (GY_IS_HEADER_BAR (self));
  g_return_if_fail (GTK_IS_WIDGET (child));

  gtk_container_add_with_properties (GTK_CONTAINER (self->primary), child,
                                     "pack-type", GTK_PACK_END,
                                     NULL);
}

/**
 * gy_header_bar_add_secondary:
 * @self: a #GyHeaderBar
 *
 * Adds a widget to the secondary button section of the workspace header.
 * This is the right, for LTR languages.
 *
 * Since: 0.6
 */
void
gy_header_bar_add_secondary (GyHeaderBar *self,
                             GtkWidget   *widget)
{

  g_return_if_fail (GY_IS_HEADER_BAR (self));
  g_return_if_fail (GTK_IS_WIDGET (widget));

  gtk_container_add (GTK_CONTAINER (self->secondary), widget);
}
