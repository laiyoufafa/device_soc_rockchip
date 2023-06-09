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
 * @file        Rockchip_OSAL_ETC.c
 * @brief
 * @author      csy(csy@rock-chips.com)
 * @version     1.0.0
 * @history
 *   2013.11.26 : Create
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <securec.h>
#include <sys/time.h>

#include "Rockchip_OSAL_Memory.h"
#include "Rockchip_OSAL_Log.h"
#include "Rockchip_OSAL_ETC.h"

static struct timeval perfStart[PERF_ID_MAX + 1], perfStop[PERF_ID_MAX + 1];
static unsigned long perfTime[PERF_ID_MAX + 1], totalPerfTime[PERF_ID_MAX + 1];
static unsigned int perfFrameCount[PERF_ID_MAX + 1], perfOver30ms[PERF_ID_MAX + 1];

#ifndef HAVE_GETLINE
ssize_t getline(char **ppLine, size_t *pLen, FILE *pStream)
{
    char *pCurrentPointer = NULL;
    size_t const chunk = 512;

    size_t defaultBufferSize = chunk + 1;
    size_t retSize = 0;

    if (*ppLine == NULL) {
        *ppLine = (char *)malloc(defaultBufferSize);
        if (*ppLine == NULL) {
            retSize = -1;
            goto EXIT;
        }
        *pLen = defaultBufferSize;
    } else {
        if (*pLen < defaultBufferSize) {
            *ppLine = (char *)realloc(*ppLine, defaultBufferSize);
            if (*ppLine == NULL) {
                retSize = -1;
                goto EXIT;
            }
            *pLen = defaultBufferSize;
        }
    }

    while (1) {
        size_t i;
        size_t j = 0;
        size_t readByte = 0;

        pCurrentPointer = *ppLine + readByte;

        i = fread(pCurrentPointer, 1, chunk, pStream);
        if (i < chunk && ferror(pStream)) {
            retSize = -1;
            goto EXIT;
        }
        while (j < i) {
            ++j;
            if (*pCurrentPointer++ == (char)'\n') {
                *pCurrentPointer = '\0';
                if (j != i) {
                    if (fseek(pStream, j - i, SEEK_CUR)) {
                        retSize = -1;
                        goto EXIT;
                    }
                    if (feof(pStream))
                        clearerr(pStream);
                }
                readByte += j;
                retSize = readByte;
                goto EXIT;
            }
        }

        readByte += j;
        if (feof(pStream)) {
            if (readByte) {
                retSize = readByte;
                goto EXIT;
            }
            if (!i) {
                retSize = -1;
                goto EXIT;
            }
        }

        i = ((readByte + (chunk * 2)) / chunk) * chunk; // 2:byte alignment
        if (i != *pLen) {
            *ppLine = (char *)realloc(*ppLine, i);
            if (*ppLine == NULL) {
                retSize = -1;
                goto EXIT;
            }
            *pLen = i;
        }
    }

EXIT:
    return retSize;
}
#endif /* HAVE_GETLINE */

OMX_S32 Rockchip_OSAL_Strcpy(OMX_PTR dest, OMX_PTR src)
{
    return strcpy_s(dest, strlen(src)+1, src);
}

OMX_S32 Rockchip_OSAL_Strncpy(OMX_PTR dest, OMX_PTR src, size_t num)
{
    return strncpy_s(dest, num, src, strlen(src)+1);
}

OMX_S32 Rockchip_OSAL_Strcmp(OMX_PTR str1, OMX_PTR str2)
{
    return strcmp(str1, str2);
}

OMX_S32 Rockchip_OSAL_Strncmp(OMX_PTR str1, OMX_PTR str2, size_t num)
{
    return strncmp(str1, str2, num);
}

OMX_S32 Rockchip_OSAL_Strcat(OMX_PTR dest, OMX_PTR src)
{
    return strcat_s(dest, strlen(src)+1, src);
}

OMX_S32 Rockchip_OSAL_Strncat(OMX_PTR dest, OMX_PTR src, size_t num)
{
    return strncat_s(dest, num, src, strlen(src)+1);
}

size_t Rockchip_OSAL_Strlen(const char *str)
{
    return strlen(str);
}

static OMX_U32 MeasureTime(struct timeval *start, struct timeval *stop)
{
    unsigned long sec, usec, time;

    sec = stop->tv_sec - start->tv_sec;
    if (stop->tv_usec >= start->tv_usec) {
        usec = stop->tv_usec - start->tv_usec;
    } else {
        usec = stop->tv_usec + 1000000 - start->tv_usec; // 1000000:time shift
        sec--;
    }

    time = sec * 1000000 + (usec); // 1000000:time shift

    return time;
}

void Rockchip_OSAL_PerfInit(PERF_ID_TYPE id)
{
    memset_s(&perfStart[id], sizeof(perfStart[id]), 0, sizeof(perfStart[id]));
    memset_s(&perfStop[id], sizeof(perfStart[id]), 0, sizeof(perfStop[id]));
    perfTime[id] = 0;
    totalPerfTime[id] = 0;
    perfFrameCount[id] = 0;
    perfOver30ms[id] = 0;
}

void Rockchip_OSAL_PerfStart(PERF_ID_TYPE id)
{
    gettimeofday(&perfStart[id], NULL);
}

void Rockchip_OSAL_PerfStop(PERF_ID_TYPE id)
{
    gettimeofday(&perfStop[id], NULL);

    perfTime[id] = MeasureTime(&perfStart[id], &perfStop[id]);
    totalPerfTime[id] += perfTime[id];
    perfFrameCount[id]++;

    if (perfTime[id] > 30000) { // 30000:time
        perfOver30ms[id]++;
    }
}

OMX_U32 Rockchip_OSAL_PerfFrame(PERF_ID_TYPE id)
{
    return perfTime[id];
}

OMX_U32 Rockchip_OSAL_PerfTotal(PERF_ID_TYPE id)
{
    return totalPerfTime[id];
}

OMX_U32 Rockchip_OSAL_PerfFrameCount(PERF_ID_TYPE id)
{
    return perfFrameCount[id];
}

int Rockchip_OSAL_PerfOver30ms(PERF_ID_TYPE id)
{
    return perfOver30ms[id];
}

void Rockchip_OSAL_PerfPrint(OMX_STRING prefix, PERF_ID_TYPE id)
{
    OMX_U32 perfTotal;
    int frameCount;

    frameCount = Rockchip_OSAL_PerfFrameCount(id);
    perfTotal = Rockchip_OSAL_PerfTotal(id);

    omx_info("%s Frame Count: %d", prefix, frameCount);
    omx_info("%s Avg Time: %.2f ms, Over 30ms: %d",
        prefix, (float)perfTotal / (float)(frameCount * 1000), Rockchip_OSAL_PerfOver30ms(id)); // 1000:time shift
}