/* gy-dict-manager.c
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

#include <libpeas/peas.h>
#include "gy-dict-manager.h"
#include "gy-dict-manager-addin.h"
#include "gy-dict.h"

struct _GyDictManager
{
  GObject     __parent__;
  GHashTable  *dicts;
  PeasExtensionSet *extens;
};

G_DEFINE_TYPE (GyDictManager, gy_dict_manager, G_TYPE_OBJECT)


static void
gy_dict_manager_addin_added (PeasExtensionSet *set,
                             PeasPluginInfo   *plugin_info,
                             PeasExtension    *exten,
                             gpointer          user_data)
{
  GyDictManagerAddin *self = GY_DICT_MANAGER_ADDIN (exten);
  GyDictManager *manager = GY_DICT_MANAGER (user_data);

  gy_dict_manager_addin_load (self, manager);
}


static void
gy_dict_manager_addin_removed (PeasExtensionSet *set,
                               PeasPluginInfo   *plugin_info,
                               PeasExtension    *exten,
                               gpointer          user_data)
{
  GyDictManagerAddin *self = GY_DICT_MANAGER_ADDIN (exten);
  GyDictManager *manager = GY_DICT_MANAGER (user_data);

  gy_dict_manager_addin_unload (self, manager);
}

static void
gy_dict_manager_finalize (GObject *object)
{
  GyDictManager *self = (GyDictManager *)object;

  g_hash_table_destroy (self->dicts);

  G_OBJECT_CLASS (gy_dict_manager_parent_class)->finalize (object);
}

static void
gy_dict_manager_constructed (GObject *obj)
{
  GyDictManager *self = GY_DICT_MANAGER (obj);
  PeasEngine *engine;

  G_OBJECT_CLASS (gy_dict_manager_parent_class)->constructed (obj);

  engine = peas_engine_get_default ();

  self->extens = peas_extension_set_new (engine, GY_TYPE_DICT_MANAGER_ADDIN, NULL);

  g_signal_connect (self->extens, "extension-added",
                    G_CALLBACK (gy_dict_manager_addin_added), self);

  g_signal_connect (self->extens, "extension-removed",
                    G_CALLBACK (gy_dict_manager_addin_removed), self);

  peas_extension_set_foreach (self->extens, gy_dict_manager_addin_added, self);
}

static void
gy_dict_manager_dispose (GObject *obj)
{
  GyDictManager *self = GY_DICT_MANAGER (obj);

  if (self->extens != NULL)
    g_clear_object (&self->extens);

  G_OBJECT_CLASS (gy_dict_manager_parent_class)->dispose (obj);
}

static void
gy_dict_manager_class_init (GyDictManagerClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);

  object_class->constructed = gy_dict_manager_constructed;
  object_class->dispose = gy_dict_manager_dispose;
  object_class->finalize = gy_dict_manager_finalize;
}

static void
gy_dict_manager_init (GyDictManager *self)
{
  self->dicts = g_hash_table_new_full (g_str_hash, g_str_equal,
                                       NULL, g_object_unref);
}

GyDictManager *
gy_dict_manager_new (void)
{
  return g_object_new (GY_TYPE_DICT_MANAGER, NULL);
}

GyDict *
gy_dict_manager_set_dict (GyDictManager *self,
                          const gchar   *name_dict)
{
  GyDict *dict;
  GError *err = NULL;
  GHashTableIter iter;
  gpointer obj;

  g_return_val_if_fail (name_dict != NULL, NULL);
  g_return_val_if_fail (GY_IS_DICT_MANAGER (self), NULL);

  g_hash_table_iter_init (&iter, self->dicts);
  while (g_hash_table_iter_next (&iter, NULL, &obj))
    {
      g_object_set (obj, "is-used", FALSE, NULL);
    }

  dict = g_hash_table_lookup (self->dicts, name_dict);
  if (dict) goto end;

  dict = GY_DICT (gy_dict_new (name_dict));
  gy_dict_map (dict, &err);
  if (err != NULL)
    {
      g_critical ("Unable to create a new dictionary: %s", err->message);
      g_error_free (err);
      g_object_unref (dict);
      return NULL;
    }

  g_hash_table_insert (self->dicts, (gpointer) name_dict, (gpointer) dict);

end:
  g_object_set (dict, "is-used", TRUE, NULL);
  return dict;
}

GyDict*
gy_dict_manager_get_used_dict (GyDictManager *self)
{
  gpointer dict = NULL;
  GHashTableIter iter;

  g_return_val_if_fail (GY_IS_DICT_MANAGER (self), NULL);

  g_hash_table_iter_init (&iter, self->dicts);
  while (g_hash_table_iter_next (&iter, NULL, &dict))
    {
      if (gy_dict_is_used (dict))
        {
          return dict;
        }
    }

  return dict;
}
