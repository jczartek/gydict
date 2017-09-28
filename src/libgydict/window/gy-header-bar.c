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
  GtkSearchEntry *search_entry;
};

G_DEFINE_TYPE (GyHeaderBar, gy_header_bar, GTK_TYPE_HEADER_BAR)

static void
gy_header_bar_class_init (GyHeaderBarClass *klass)
{
  gtk_widget_class_set_template_from_resource (GTK_WIDGET_CLASS (klass), "/org/gtk/gydict/gy-header-bar.ui");
  gtk_widget_class_bind_template_child (GTK_WIDGET_CLASS (klass), GyHeaderBar, dicts_button);
  gtk_widget_class_bind_template_child (GTK_WIDGET_CLASS (klass), GyHeaderBar, menu_button);
  gtk_widget_class_bind_template_child (GTK_WIDGET_CLASS (klass), GyHeaderBar, search_entry);
}

static void
gy_header_bar_init (GyHeaderBar *self)
{
  GtkWidget *popover;
  GMenu     *model;

  gtk_widget_init_template (GTK_WIDGET (self));

  model = gy_app_get_menu_by_id (GY_APP_DEFAULT, "dict-menu");
  popover = gtk_popover_new_from_model (NULL, G_MENU_MODEL (model));
  gtk_widget_set_size_request (popover, 250, -1);
  gtk_menu_button_set_popover (self->dicts_button, popover);

  model = gy_app_get_menu_by_id (GY_APP_DEFAULT, "gear-menu");
  popover = gtk_popover_new_from_model (NULL, G_MENU_MODEL (model));
  gtk_widget_set_size_request (popover, 225, -1);
  gtk_menu_button_set_popover (self->menu_button, popover);
}

void
gy_header_bar_connect_entry_with_tree_view (GyHeaderBar *self,
                                            GtkTreeView *tree_view)
{
  g_return_if_fail (GY_IS_HEADER_BAR (self));
  g_return_if_fail (GTK_IS_TREE_VIEW (tree_view));

  gtk_tree_view_set_search_entry (tree_view, GTK_ENTRY (self->search_entry));
}

void
gy_header_bar_set_text_in_entry (GyHeaderBar *self,
                                 const gchar *text)
{
  g_return_if_fail (GY_IS_HEADER_BAR (self));

  gtk_entry_set_text (GTK_ENTRY (self->search_entry), text);
}

GtkEntry *
gy_header_bar_get_entry (GyHeaderBar *self)
{
  g_return_val_if_fail (GY_IS_HEADER_BAR (self), NULL);

  return GTK_ENTRY (self->search_entry);
}

void
gy_header_bar_grab_focus_for_entry (GyHeaderBar *self)
{
  g_return_if_fail (GY_IS_HEADER_BAR (self));

  gtk_widget_grab_focus (GTK_WIDGET (self->search_entry));
}
