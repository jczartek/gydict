/* gy-prefs-view.c
 *
 * Copyright 2018 Jakub Czartek <kuba@linux.pl>
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

#include <glib/gi18n.h>
#include <libpeas/peas.h>
#include "gy-prefs-view.h"
#include "gy-prefs-view-addin.h"

struct _GyPrefsView
{
  DzlPreferencesView  parent_instance;
  GHashTable         *size_groups;
  PeasExtensionSet   *extens;
};

G_DEFINE_TYPE (GyPrefsView, gy_prefs_view, DZL_TYPE_PREFERENCES_VIEW)


static void
gy_prefs_view_register_appearance_prefs (DzlPreferences *prefs)
{
  dzl_preferences_add_page (prefs, "appearance", _("Appearance"), 0);
  dzl_preferences_add_list_group (prefs, "appearance", "basic", _("Themes"), GTK_SELECTION_NONE, 0);
  dzl_preferences_add_switch (prefs, "appearance", "basic",
                              "org.gtk.gydict", "night-mode", NULL, NULL,
                              _("Dark Mode"), _("Whether Gydict should use a dark theme"), _("dark theme"), 0);
  dzl_preferences_add_switch (prefs, "appearance", "basic",
                              "org.gtk.gydict", "show-grid-lines", NULL, NULL,
                              _("Grid Pattern"), _("Display a grid pattern underneath text"), NULL, 10);

  dzl_preferences_add_list_group (prefs, "appearance", "font", _("Font"), GTK_SELECTION_NONE, 10);
  dzl_preferences_add_font_button (prefs, "appearance", "font", "org.gtk.gydict", "font-name", _("Text View"), C_("Keywords", "Text view font"), 0);
}

static gint
sort_plugin_info (gconstpointer a,
                  gconstpointer b)
{
  PeasPluginInfo *plugin_info_a = (PeasPluginInfo *)a;
  PeasPluginInfo *plugin_info_b = (PeasPluginInfo *)b;
  const gchar *name_a = peas_plugin_info_get_name (plugin_info_a);
  const gchar *name_b = peas_plugin_info_get_name (plugin_info_b);

  if (name_a == NULL || name_b == NULL)
    return g_strcmp0 (name_a, name_b);

  return g_utf8_collate (name_a, name_b);
}

static void
gy_prefs_view_register_builtin_prefs (DzlPreferences *prefs)
{

  gy_prefs_view_register_appearance_prefs (prefs);

  {
    PeasEngine *engine = peas_engine_get_default ();
    const GList *list = peas_engine_get_plugin_list (engine);
    GList *copy;
    guint i = 0;

    dzl_preferences_add_page (prefs, "plugins", _("Extensions"), 700);
    dzl_preferences_add_list_group (prefs, "plugins", "plugins", _("Extensions"), GTK_SELECTION_NONE, 100);

    copy = g_list_sort (g_list_copy ((GList *)list), sort_plugin_info);

    for (const GList *iter = copy; iter; iter = iter->next, i++)
      {
        PeasPluginInfo *plugin_info = iter->data;
        g_autofree gchar *path = NULL;
        g_autofree gchar *keywords = NULL;
        const gchar *desc;
        const gchar *name;

        if (peas_plugin_info_is_hidden (plugin_info))
          continue;

        name = peas_plugin_info_get_name (plugin_info);
        desc = peas_plugin_info_get_description (plugin_info);
        keywords = g_strdup_printf ("%s %s", name, desc);
        path = g_strdup_printf ("/org/gtk/gydict/plugins/%s/",
                                peas_plugin_info_get_module_name (plugin_info));

        dzl_preferences_add_switch (prefs, "plugins", "plugins", "org.gtk.gydict.plugin", "enabled", path, NULL, name, desc, keywords, i);
      }

    g_list_free (copy);
  }
}

static void
gy_prefs_view_addin_added (PeasExtensionSet *set,
                           PeasPluginInfo   *plugin_info,
                           PeasExtension    *exten,
                           gpointer          user_data)
{
  GyPrefsView *self = GY_PREFS_VIEW (user_data);
  GyPrefsViewAddin *addin = GY_PREFS_VIEW_ADDIN (exten);

  gy_prefs_view_addin_load (addin, self);
}


static void
gy_prefs_view_addin_removed (PeasExtensionSet *set,
                             PeasPluginInfo   *plugin_info,
                             PeasExtension    *exten,
                             gpointer          user_data)
{
  GyPrefsView *self = GY_PREFS_VIEW (user_data);
  GyPrefsViewAddin *addin = GY_PREFS_VIEW_ADDIN (exten);

  gy_prefs_view_addin_unload (addin, self);
}


static void
gy_prefs_view_finalize (GObject *obj)
{
  GyPrefsView *self = GY_PREFS_VIEW (obj);

  g_clear_pointer (&self->size_groups, g_hash_table_unref);

  G_OBJECT_CLASS (gy_prefs_view_parent_class)->finalize (obj);
}

static void
gy_prefs_view_constructed (GObject *obj)
{
  GyPrefsView *self = GY_PREFS_VIEW (obj);
  G_OBJECT_CLASS (gy_prefs_view_parent_class)->constructed (obj);

  gy_prefs_view_register_builtin_prefs (DZL_PREFERENCES(obj));

  PeasEngine *engine = peas_engine_get_default ();
  self->extens = peas_extension_set_new (engine, GY_TYPE_PREFS_VIEW_ADDIN, NULL);

  g_signal_connect (self->extens, "extension-added",
                    G_CALLBACK (gy_prefs_view_addin_added), self);

  g_signal_connect (self->extens, "extension-removed",
                    G_CALLBACK (gy_prefs_view_addin_removed), self);

  peas_extension_set_foreach (self->extens, gy_prefs_view_addin_added, self);
}

static void
gy_prefs_view_class_init (GyPrefsViewClass *klass)
{
  GObjectClass *oclass = G_OBJECT_CLASS (klass);

  oclass->constructed = gy_prefs_view_constructed;
  oclass->finalize    = gy_prefs_view_finalize;
}

static void
gy_prefs_view_init (GyPrefsView *self)
{
}

/**
 * gy_prefs_view_get_size_group:
 * @self: a #GyPrefsView widget
 * @name_group:
 *
 * Returns: (transfer none): a #GtkSizeGroup
 */
GtkSizeGroup *
gy_prefs_view_get_size_group (GyPrefsView *self,
                              const gchar *name_group)
{
  GtkSizeGroup *ret = NULL;

  g_return_val_if_fail (GY_IS_PREFS_VIEW (self), NULL);

  if (self->size_groups == NULL)
    self->size_groups = g_hash_table_new_full (g_str_hash, g_str_equal,
                                               g_free, g_object_unref);

  if (!(ret = g_hash_table_lookup (self->size_groups, name_group)))
    {
      ret = gtk_size_group_new (GTK_SIZE_GROUP_HORIZONTAL);
      g_hash_table_insert (self->size_groups, g_strdup(name_group), ret);
    }

  return ret;
}

