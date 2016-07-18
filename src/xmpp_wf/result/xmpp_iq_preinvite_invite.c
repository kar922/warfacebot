/**
 * WarfaceBot, a blind XMPP client for Warface (FPS)
 * Copyright (C) 2015, 2016 Levak Borok <levak92@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify it
 * under the terms of the GNU Affero General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or (at your
 * option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include <wb_tools.h>
#include <wb_xmpp.h>
#include <wb_xmpp_wf.h>
#include <wb_session.h>

#include <stdlib.h>

static void xmpp_iq_preinvite_invite_cb(const char *msg_id,
                                        const char *msg,
                                        void *args)
{
    /* Accept any preinvite
       <iq from='xxxx@warface/GameClient' id='uid000000e9' type='get'>
        <query xmlns='urn:cryonline:k01'>
         <preinvite_invite from='xxxxxxxx' uid='xxxxxxx'
                 ms_resource='pve_11' channel_type='pve'/>
        </query>
       </iq>
     */

    char *data = wf_get_query_content(msg);

    if (!data)
        return;

    char *jid = get_info(msg, "from='", "'", NULL);

    char *resource = get_info(data, "ms_resource='", "'", "Resource");
    char *uid = get_info(data, "uid='", "'", "UUID");

    if (jid && resource && uid)
    {
        xmpp_send_iq_result(
            JID(jid),
            msg_id,
            "<query xmlns='urn:cryonline:k01'>"
            " <preinvite_invite uid='%s'/>"
            "</query>",
            uid);

        xmpp_send_iq_get(
            JID(jid),
            NULL, NULL,
            "<query xmlns='urn:cryonline:k01'>"
            " <preinvite_response uid='%s' accepted='1'"
            "         pid='%s' from='%s'/>"
            "</query>",
            uid,
            session.profile.id,
            session.profile.nickname);
    }

    free(uid);
    free(resource);
    free(jid);
    free(data);
}

void xmpp_iq_preinvite_invite_r(void)
{
    qh_register("preinvite_invite", 1, xmpp_iq_preinvite_invite_cb, NULL);
}
