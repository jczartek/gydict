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
#include "gy-lex-search-box.h"
#include "app/gy-app.h"

struct _GyHeaderBar
{
  GtkHeaderBar    __parent__;
  GtkMenuButton  *menu_button;
  GtkMenuButton  *dicts_button;
};

G_DEFINE_TYPE (GyHeaderBar, gy_header_bar, GTK_TYPE_HEADER_BAR)

GyHeaderBar *
gy_header_bar_new (void)
{
  return g_object_new (GY_TYPE_HEADER_BAR, NULL);
}

static void
gy_header_bar_class_init (GyHeaderBarClass *klass)
{
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
  GyLexSearchBox *search_box;
  GtkEntry       *entry;

  g_return_if_fail (GY_IS_HEADER_BAR (self));
  g_return_if_fail (GTK_IS_TREE_VIEW (tree_view));

  search_box = GY_LEX_SEARCH_BOX (gtk_header_bar_get_custom_title (GTK_HEADER_BAR (self)));

  g_assert (GY_IS_LEX_SEARCH_BOX (search_box));

  entry = GTK_ENTRY (_gy_lex_search_box_get_search_entry (search_box));
  gtk_tree_view_set_search_entry (tree_view, entry);
}

void gy_header_bar_set_text_in_entry (GyHeaderBar *self,
                                      const gchar *text)
{
  GyLexSearchBox *search_box;
  GtkEntry       *entry;

  g_return_if_fail (GY_IS_HEADER_BAR (self));

  search_box = GY_LEX_SEARCH_BOX (gtk_header_bar_get_custom_title (GTK_HEADER_BAR (self)));

  g_assert (GY_IS_LEX_SEARCH_BOX (search_box));

  entry = GTK_ENTRY (_gy_lex_search_box_get_search_entry (search_box));
  gtk_entry_set_text (entry, text);
}
