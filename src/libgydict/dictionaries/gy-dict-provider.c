/* gy-dict-provider.c
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

#include "libpeas/peas.h"
#include "gy-dict-provider.h"
#include "gy-dict-provider-addin.h"

struct _GyDictProvider
{
  GObject parent_instance;
  PeasExtensionSet *extens;
};

G_DEFINE_TYPE (GyDictProvider, gy_dict_provider, G_TYPE_OBJECT)

enum {
  PROP_0,
  N_PROPS
};

static GParamSpec *properties [N_PROPS];

GyDictProvider *
gy_dict_provider_new (void)
{
  return g_object_new (GY_TYPE_DICT_PROVIDER, NULL);
}

static void
gy_dict_provider_finalize (GObject *object)
{
  GyDictProvider *self = (GyDictProvider *)object;

  G_OBJECT_CLASS (gy_dict_provider_parent_class)->finalize (object);
}

static void
gy_dict_provider_get_property (GObject    *object,
                               guint       prop_id,
                               GValue     *value,
                               GParamSpec *pspec)
{
  GyDictProvider *self = GY_DICT_PROVIDER (object);

  switch (prop_id)
    {
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}

static void
gy_dict_provider_set_property (GObject      *object,
                               guint         prop_id,
                               const GValue *value,
                               GParamSpec   *pspec)
{
  GyDictProvider *self = GY_DICT_PROVIDER (object);

  switch (prop_id)
    {
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}

static void
gy_dict_provider_addin_added (PeasExtensionSet *set,
                             PeasPluginInfo   *plugin_info,
                             PeasExtension    *exten,
                             gpointer          user_data)
{
  GyDictProviderAddin *self = GY_DICT_PROVIDER_ADDIN (exten);
  GyDictProvider *provider = GY_DICT_PROVIDER (user_data);

  gy_dict_provider_addin_load (self, provider);
}


static void
gy_dict_provider_addin_removed (PeasExtensionSet *set,
                               PeasPluginInfo   *plugin_info,
                               PeasExtension    *exten,
                               gpointer          user_data)
{
  GyDictProviderAddin *self = GY_DICT_PROVIDER_ADDIN (exten);
  GyDictProvider *provider = GY_DICT_PROVIDER (user_data);

  gy_dict_provider_addin_unload (self, provider);
}

static void
gy_dict_provider_constructed (GObject *obj)
{
  GyDictProvider *self = GY_DICT_PROVIDER (obj);
  PeasEngine *engine;

  G_OBJECT_CLASS (gy_dict_provider_parent_class)->constructed (obj);

  engine = peas_engine_get_default ();

  self->extens = peas_extension_set_new (engine, GY_TYPE_DICT_PROVIDER_ADDIN, NULL);

  g_signal_connect (self->extens, "extension-added",
                    G_CALLBACK (gy_dict_provider_addin_added), self);

  g_signal_connect (self->extens, "extension-removed",
                    G_CALLBACK (gy_dict_provider_addin_removed), self);

  peas_extension_set_foreach (self->extens, gy_dict_provider_addin_added, self);
}

static void
gy_dict_provider_dispose (GObject *obj)
{
  GyDictProvider *self = GY_DICT_PROVIDER (obj);

  if (self->extens != NULL)
    g_clear_object (&self->extens);

  G_OBJECT_CLASS (gy_dict_provider_parent_class)->dispose (obj);
}

static void
gy_dict_provider_class_init (GyDictProviderClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);

  object_class->constructed = gy_dict_provider_constructed;
  object_class->finalize = gy_dict_provider_finalize;
  object_class->dispose = gy_dict_provider_dispose;
  object_class->get_property = gy_dict_provider_get_property;
  object_class->set_property = gy_dict_provider_set_property;
}

static void
gy_dict_provider_init (GyDictProvider *self)
{
}
