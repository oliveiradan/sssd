/*
    Authors:
        Pavel Březina <pbrezina@redhat.com>

    Copyright (C) 2016 Red Hat

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <talloc.h>
#include <tevent.h>

#include "sbus/sssd_dbus.h"
#include "providers/data_provider/dp_private.h"
#include "providers/data_provider/dp_iface.h"
#include "providers/backend.h"
#include "util/util.h"

errno_t dp_host_handler(struct sbus_request *sbus_req,
                        void *dp_cli,
                        uint32_t dp_flags,
                        const char *name,
                        const char *alias)
{
    struct dp_hostid_data *data;
    const char *key;

    if (name == NULL) {
        return EINVAL;
    }

    data = talloc_zero(sbus_req, struct dp_hostid_data);
    if (data == NULL) {
        return ENOMEM;
    }

    data->name = name;
    data->alias = SBUS_SET_STRING(alias);

    key = talloc_asprintf(data, "%s:%s", name,
                          (data->alias == NULL ? "(null)" : data->alias));
    if (key == NULL) {
        talloc_free(data);
        return ENOMEM;
    }

    dp_req_with_reply(dp_cli, NULL, "HostID", key, sbus_req, DPT_HOSTID,
                      DPM_HOSTID_HANDLER, dp_flags, data,
                      dp_req_reply_std, struct dp_reply_std);

    return EOK;
}
