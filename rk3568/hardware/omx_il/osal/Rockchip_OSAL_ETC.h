/*
 *
 * Copyright 2013 Rockchip Electronics Co., LTD.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/*
 * @file        Rockchip_OSAL_ETC.h
 * @brief
 * @author      csy(csy@rock-chips.com)
 * @version     1.0.0
 * @history
 *   2013.11.26 : Create
 */

#ifndef Rockchip_OSAL_ETC
#define Rockchip_OSAL_ETC

#include "OMX_Types.h"

#ifdef __cplusplus
extern "C" {
#endif

OMX_S32 Rockchip_OSAL_Strcpy(OMX_PTR dest, OMX_PTR src);
OMX_S32 Rockchip_OSAL_Strncmp(OMX_PTR str1, OMX_PTR str2, size_t num);
OMX_S32 Rockchip_OSAL_Strcmp(OMX_PTR str1, OMX_PTR str2);
OMX_S32 Rockchip_OSAL_Strcat(OMX_PTR dest, OMX_PTR src);
size_t Rockchip_OSAL_Strlen(const char *str);
ssize_t getline(char **ppLine, size_t *pLen, FILE *pStream);

/* perf */
typedef enum _PERF_ID_TYPE {
    PERF_ID_CSC = 0,
    PERF_ID_DEC,
    PERF_ID_ENC,
    PERF_ID_USER,
    PERF_ID_MAX,
} PERF_ID_TYPE;

void Rockchip_OSAL_PerfInit(PERF_ID_TYPE id);
void Rockchip_OSAL_PerfStart(PERF_ID_TYPE id);
void Rockchip_OSAL_PerfStop(PERF_ID_TYPE id);
OMX_U32 Rockchip_OSAL_PerfFrame(PERF_ID_TYPE id);
OMX_U32 Rockchip_OSAL_PerfTotal(PERF_ID_TYPE id);
OMX_U32 Rockchip_OSAL_PerfFrameCount(PERF_ID_TYPE id);
int Rockchip_OSAL_PerfOver30ms(PERF_ID_TYPE id);
void Rockchip_OSAL_PerfPrint(OMX_STRING prefix, PERF_ID_TYPE id);

#ifdef __cplusplus
}
#endif

#endif
