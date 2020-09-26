/* gy-window.c
 *
 * Copyright (C) 2014 Jakub Czartek <kuba@linux.pl>
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

#include "config.h"
#include <glib/gi18n-lib.h>
#include <gtk/gtk.h>
#include <pango/pango.h>
#include <libpeas/peas.h>

#include "gy-window-addin.h"
#include "helpers/gy-utility-func.h"

#include "gy-window-private.h"

#define MOUSE_UP_BUTTON   8
#define MOUSE_DOWN_BUTTON 9

G_DEFINE_TYPE (GyWindow, gy_window, DZL_TYPE_APPLICATION_WINDOW);

static void
gy_window_show_lexical_unit (GtkTreeSelection *selection,
                             gpointer          data)
{
  GtkTreeIter   iter;
  GtkTreeModel *model;
  GyWindow     *self = GY_WINDOW (data);

  if (gtk_tree_selection_get_selected (selection, &model, &iter))
    {
      g_autoptr (GtkTreePath) path = NULL;
      gint *row;

      path = gtk_tree_model_get_path (model, &iter);
      row = gtk_tree_path_get_indices (path);
      GyService *service = gy_service_provider_get_service_by_id (self->service_provider,
                                                                  self->service_id);
      if (row)
        {
          if (GY_IS_DICT_SERVICE (service))
            {
              GError *error = NULL;
              g_autofree gchar *lexical_unit =
                gy_dict_service_get_lexical_unit(GY_DICT_SERVICE (service), *row, &error);

              if (error != NULL)
                {
                  g_critical ("Error: %s", error->message);
                  g_error_free(error);
                  return;
                }
              GyDictFormatter *formatter = gy_dict_service_get_formatter (GY_DICT_SERVICE (service));
              gy_text_buffer_insert_and_format (self->buffer, lexical_unit, formatter);
              g_object_unref (formatter);
            }
          else
            g_critical("The dictionary services: %s is not available.", self->service_id );
        }
    }
}

static gboolean
gy_window_button_press_event (GtkWidget      *w,
                              GdkEventButton *e,
                              gpointer        d)
{
  if (e->type == GDK_DOUBLE_BUTTON_PRESS && (e->button == MOUSE_UP_BUTTON || e->button == MOUSE_DOWN_BUTTON))
    g_signal_emit_by_name (GY_WINDOW (w)->deflist, "move-selection", e->button == MOUSE_UP_BUTTON ? GTK_DIR_UP : GTK_DIR_DOWN);

  return FALSE;
}

static void
gy_window_notify_top_visible (GObject    *obj,
                              GParamSpec *pspec,
                              gpointer    data)
{
  GyWindow *self = GY_WINDOW (data);
  gboolean  is_visible;

  g_object_get (obj, "top-visible", &is_visible, NULL);

    gy_search_bar_set_search_mode_enabled (self->search_bar, is_visible);

  if (!is_visible)
    gy_window_grab_focus (self);
}

static void
gy_window_dispose (GObject *obj)
{
  GyWindow *self = GY_WINDOW (obj);

  if (self->extens != NULL)
    g_clear_object (&self->extens);

  G_OBJECT_CLASS (gy_window_parent_class)->dispose (obj);
}

static void
gy_window_constructed(GObject *obj)
{

  G_OBJECT_CLASS (gy_window_parent_class)->constructed (obj);

  GyWindow *self = GY_WINDOW (obj);

  _gy_window_plugins_init_extens (self);

  gtk_stack_set_visible_child_name ((GtkStack*)self->main_view_stack, "greeting-view-page");
}

static void
gy_window_init (GyWindow *self)
{

  gtk_widget_init_template (GTK_WIDGET (self));

  _gy_window_actions_init (self);

  _gy_window_settings_register (GTK_WINDOW (self));

  self->clipboard = gtk_clipboard_get (GDK_SELECTION_PRIMARY);

  gtk_tree_view_set_search_entry (GTK_TREE_VIEW (self->deflist),
                                  GTK_ENTRY (gtk_header_bar_get_custom_title (GTK_HEADER_BAR (self->header_bar))));

  g_object_set_data (G_OBJECT (self->buffer), "textview", self->textview);

  self->selection = gtk_tree_view_get_selection (GTK_TREE_VIEW (self->deflist));
  self->menu_manager = dzl_application_get_menu_manager (DZL_APPLICATION (g_application_get_default ()));

  g_signal_connect (self->selection, "changed",
                   G_CALLBACK (gy_window_show_lexical_unit), self);

  g_signal_connect (self, "button-press-event",
                    G_CALLBACK (gy_window_button_press_event), NULL);
  g_signal_connect (self->dockbin, "notify::top-visible",
                    G_CALLBACK (gy_window_notify_top_visible), self);
}

static void
gy_window_class_init (GyWindowClass *klass)
{
  GObjectClass   *object_class = G_OBJECT_CLASS (klass);
  GtkWidgetClass *widget_class = GTK_WIDGET_CLASS (klass);

  object_class->dispose = gy_window_dispose;
  object_class->constructed = gy_window_constructed;

  gtk_widget_class_set_template_from_resource (widget_class, "/org/gtk/gydict/gy-window.ui");
  gtk_widget_class_bind_template_child (widget_class, GyWindow, dockbin);
  gtk_widget_class_bind_template_child (widget_class, GyWindow, deflist);
  gtk_widget_class_bind_template_child (widget_class, GyWindow, textview);
  gtk_widget_class_bind_template_child (widget_class, GyWindow, buffer);
  gtk_widget_class_bind_template_child (widget_class, GyWindow, header_bar);
  gtk_widget_class_bind_template_child (widget_class, GyWindow, search_bar);
  gtk_widget_class_bind_template_child (widget_class, GyWindow, search_entry);
  gtk_widget_class_bind_template_child (widget_class, GyWindow, main_view_stack);
}

GtkWidget *
gy_window_new (GyApp *application)
{
  g_return_val_if_fail (GY_IS_APP (application), NULL);
  GyWindow *self;

  self = g_object_new (GY_TYPE_WINDOW, "application", application, NULL);

  self->service_provider = gy_app_get_service_provider (application);

  g_assert (self->service_provider != NULL);

  return GTK_WIDGET (self);
}

/**
 * gy_window_get_text_view:
 * @self: the main window
 *
 * Returns: (transfer none): The GyTextView
 */
GtkWidget *
gy_window_get_text_view (GyWindow *self)
{
  g_return_val_if_fail (GY_IS_WINDOW (self), NULL);

  return GTK_WIDGET (self->textview);
}

void
gy_window_grab_focus (GyWindow *self)
{
  g_return_if_fail (GY_IS_WINDOW (self));

  gtk_window_set_focus (GTK_WINDOW (self), GTK_WIDGET (self->search_entry));
}

void
gy_window_clear_search_entry (GyWindow *self)
{
  g_return_if_fail (GY_IS_WINDOW (self));

  gtk_entry_set_text (GTK_ENTRY (self->search_entry), "");
}

/**
 * gy_window_get_dockbin:
 * @self: the main window
 *
 * Returns: (transfer none): the DzlDockBin
 */
DzlDockBin *
gy_window_get_dockbin (GyWindow *self)
{
  g_return_val_if_fail (GY_IS_WINDOW (self), NULL);

  return self->dockbin;
}

guint
gy_window_add_menu (GyWindow    *self,
                    const gchar *menu_id,
                    GMenuModel  *menu)
{
  g_return_val_if_fail (GY_IS_WINDOW (self), G_MAXUINT);

  return dzl_menu_manager_merge (self->menu_manager, menu_id, menu);
}

guint
gy_window_add_menu_by_resource (GyWindow     *self,
                                const gchar  *resource,
                                GError      **err)
{
  g_return_val_if_fail (GY_IS_WINDOW (self), G_MAXUINT);

  return dzl_menu_manager_add_resource (self->menu_manager, resource, err);
}

void
gy_window_remove_menu (GyWindow *self,
                       guint      id)
{
  g_return_if_fail (GY_IS_WINDOW (self));

  dzl_menu_manager_remove (self->menu_manager, id);
}
