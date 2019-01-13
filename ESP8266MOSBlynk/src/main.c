/*
 * Copyright (c) 2014-2018 Cesanta Software Limited
 * All rights reserved
 *
 * Licensed under the Apache License, Version 2.0 (the ""License"");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an ""AS IS"" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "mgos.h"
#include "mgos_rpc.h"
#include "mgos_rpc_channel_uart.h"
#include "blynk.h"

static uint16_t update_counter = 0;

void updater_cb(void *arg __attribute__((unused)))
{
  if (blynk_conn != NULL)
  {
    update_counter++;
    uint8_t data[7] = {'v', 'w', '\0', '0', '\0', 0, 0};
    data[5] = (update_counter / 10 % 10) + 48;
    data[6] = (update_counter % 10) + 48;
    blynk_send(blynk_conn, BLYNK_HARDWARE, 0, data, sizeof(data));
  }
}

void tester_cb(void *arg __attribute__((unused)))
{
  mgos_rpc_call("TEENSY", "test", "test", NULL, NULL);
}

enum mgos_app_init_result mgos_app_init(void)
{
  blynk_connect(NULL);

  const struct mgos_config_rpc *sccfg = mgos_sys_config_get_rpc();
  struct mg_rpc_channel *uch = mg_rpc_channel_uart(&sccfg->uart, NULL);
  mg_rpc_add_channel(mgos_rpc_get_global(), mg_mk_str("TEENSY"), uch);
  uch->ch_connect(uch);

  mgos_set_timer(5000, MGOS_TIMER_REPEAT, updater_cb, NULL);
  mgos_set_timer(2500, MGOS_TIMER_REPEAT, tester_cb, NULL);
  return MGOS_APP_INIT_SUCCESS;
}
