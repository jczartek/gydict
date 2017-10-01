/* gy-history-box.c
 *
 * Copyright (C) 2017 Jakub Czartek <kuba@linux.pl>
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

#include "gy-store-entry.h"

struct _GyStoreEntry
{
  GtkListBox parent_instance;
};

G_DEFINE_TYPE (GyStoreEntry, gy_store_entry, GTK_TYPE_LIST_BOX)

static gint
gy_store_entry_sort_func (GtkListBoxRow *row1,
                          GtkListBoxRow *row2,
                          gpointer data)
{
  gpointer a = g_object_get_data (G_OBJECT (row1), "idx");
  gpointer b = g_object_get_data (G_OBJECT (row2), "idx");

  return GPOINTER_TO_UINT (a) - GPOINTER_TO_UINT (b);
}

static void
gy_store_entry_class_init (GyStoreEntryClass *klass)
{
}

static void
gy_store_entry_init (GyStoreEntry *self)
{
  gtk_list_box_set_sort_func (GTK_LIST_BOX (self),
                              gy_store_entry_sort_func,
                              NULL, NULL);
}

static void
gy_store_entry_destroy_row (GtkButton *button,
                            gpointer   data)
{
  GtkWidget  *row      = NULL;
  GtkListBox *list_box = GTK_LIST_BOX (data);

  row = gtk_widget_get_ancestor (GTK_WIDGET (button), GTK_TYPE_LIST_BOX_ROW);

  if (row != NULL) gtk_container_remove (GTK_CONTAINER (list_box), row);
}

void
gy_store_entry_add_row (GyStoreEntry *self,
                        const gchar  *entry,
                        guint         idx)
{
  GtkWidget *vbox   = NULL;
  GtkWidget *label  = NULL;
  GtkWidget *row    = NULL;
  GtkWidget *button = NULL;

  label = gtk_label_new (entry);
  gtk_label_set_ellipsize (GTK_LABEL (label), PANGO_ELLIPSIZE_END);

  button = gtk_button_new_from_icon_name ("window-close-symbolic",
                                          GTK_ICON_SIZE_BUTTON);
  g_signal_connect (button, "clicked",
                    G_CALLBACK (gy_store_entry_destroy_row), self);
  gtk_style_context_add_class (gtk_widget_get_style_context (button), "flat");

  vbox = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 2);
  gtk_box_pack_start (GTK_BOX (vbox), label, FALSE, FALSE, 0);
  gtk_box_pack_end (GTK_BOX (vbox), button, FALSE, FALSE, 0);

  row = gtk_list_box_row_new ();
  g_object_set_data (G_OBJECT (row), "idx", GUINT_TO_POINTER (idx));
  gtk_container_add (GTK_CONTAINER (row), vbox);

  gtk_widget_show_all (row);
  gtk_list_box_insert (GTK_LIST_BOX(self), row, -1);
}

static void
gy_store_entry_real_remove_all (GtkWidget *row,
                                gpointer   data)
{

  if (GTK_IS_LIST_BOX_ROW (row))
    {
     gtk_widget_destroy (row);
    }
}


void
gy_store_entry_remove_all (GyStoreEntry *self)
{
  g_return_if_fail (GY_IS_STORE_ENTRY (self));

  gtk_container_foreach (GTK_CONTAINER (self),
                         gy_store_entry_real_remove_all, NULL);

}
