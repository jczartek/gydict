/* gy-history-box.c
 *
 * Copyright 2019 Jakub Czartek <kuba@linux.pl>
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
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "gy-history-box.h"

struct _GyHistoryBox
{
  GtkListBox parent_instance;

  gchar *filter_key;
};

G_DEFINE_TYPE (GyHistoryBox, gy_history_box, GTK_TYPE_LIST_BOX)

enum {
  PROP_0,
  PROP_FILTER_KEY,
  N_PROPS
};

static GParamSpec *properties [N_PROPS];

static gint
gy_history_box_sort (GtkListBoxRow *row1,
                     GtkListBoxRow *row2,
                     gpointer       data)
{
  gpointer a = g_object_get_data (G_OBJECT (row1), "idx");
  gpointer b = g_object_get_data (G_OBJECT (row2), "idx");

  return GPOINTER_TO_UINT (a) - GPOINTER_TO_UINT (b);
}

static gboolean
gy_history_box_filter (GtkListBoxRow *row,
                       gpointer    data)
{
  gchar *row_filter_key = NULL;
  g_autofree gchar *filter_key = NULL;
  GyHistoryBox *self = GY_HISTORY_BOX (data);

  row_filter_key = g_object_get_data (G_OBJECT (row), "filter-key");
  g_object_get (self, "filter-key", &filter_key, NULL);


  return g_strcmp0(row_filter_key, filter_key) == 0;
}

static void
gy_history_box_finalize (GObject *object)
{
  GyHistoryBox *self = (GyHistoryBox *)object;

  g_clear_pointer (&self->filter_key, g_free);

  G_OBJECT_CLASS (gy_history_box_parent_class)->finalize (object);
}

static void
gy_history_box_get_property (GObject    *object,
                             guint       prop_id,
                             GValue     *value,
                             GParamSpec *pspec)
{
  GyHistoryBox *self = GY_HISTORY_BOX (object);

  switch (prop_id)
    {
    case PROP_FILTER_KEY:
      g_value_set_string(value, self->filter_key);
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}

static void
gy_history_box_set_property (GObject      *object,
                             guint         prop_id,
                             const GValue *value,
                             GParamSpec   *pspec)
{
  GyHistoryBox *self = GY_HISTORY_BOX (object);

  switch (prop_id)
    {
    case PROP_FILTER_KEY:
      if (self->filter_key != NULL) g_clear_pointer (&self->filter_key, g_free);
      self->filter_key = g_value_dup_string (value);
      gtk_list_box_invalidate_sort (GTK_LIST_BOX (self));
      gtk_list_box_invalidate_filter (GTK_LIST_BOX (self));
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}

static void
gy_history_box_class_init (GyHistoryBoxClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);

  object_class->finalize = gy_history_box_finalize;
  object_class->get_property = gy_history_box_get_property;
  object_class->set_property = gy_history_box_set_property;

  properties[PROP_FILTER_KEY] =
    g_param_spec_string ("filter-key",
                         "filter-key",
                         "filter-key",
                         NULL, G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS);
  g_object_class_install_properties (object_class, N_PROPS, properties);
}

static void
gy_history_box_init (GyHistoryBox *self)
{
  gtk_list_box_set_sort_func (GTK_LIST_BOX (self),
                              gy_history_box_sort,
                              NULL, NULL);
  gtk_list_box_set_filter_func (GTK_LIST_BOX (self),
                                gy_history_box_filter,
                                self, NULL);
}

static void
gy_history_box_remove_row (GtkButton *btn,
                           gpointer   data)
{
  GtkWidget *row = NULL;
  GtkListBox *list_box = GTK_LIST_BOX (data);

  row = gtk_widget_get_ancestor (GTK_WIDGET (btn), GTK_TYPE_LIST_BOX_ROW);
  if (GTK_IS_LIST_BOX_ROW (row))
    {
      g_signal_handlers_disconnect_by_func (btn, gy_history_box_remove_row, data);
      gtk_container_remove (GTK_CONTAINER (list_box), row);
    }
}

void
gy_history_box_add (GyHistoryBox *self,
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
                    G_CALLBACK (gy_history_box_remove_row), self);
  gtk_style_context_add_class (gtk_widget_get_style_context (button), "flat");

  vbox = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 2);
  gtk_box_pack_start (GTK_BOX (vbox), label, FALSE, FALSE, 0);
  gtk_box_pack_end (GTK_BOX (vbox), button, FALSE, FALSE, 0);

  row = gtk_list_box_row_new ();
  g_object_set_data (G_OBJECT (row), "idx", GUINT_TO_POINTER (idx));
  g_object_set_data_full (G_OBJECT (row), "filter-key", g_strdup (self->filter_key), g_free);
  gtk_container_add (GTK_CONTAINER (row), vbox);

  gtk_widget_show_all (row);
  gtk_list_box_insert (GTK_LIST_BOX(self), row, -1);
}
