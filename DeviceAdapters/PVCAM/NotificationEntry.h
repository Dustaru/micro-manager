/*
 * Copyright (c) 2007, Regents of the University of California
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its contributors
 * may be used to endorse or promote products derived from this software without
 * specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef _NOTIFICATIONENTRY_H_
#define _NOTIFICATIONENTRY_H_

#include "PvFrameInfo.h"

/**
* A class that contains a ponter to frame data and corresponding
* frame metadata. This class is used by the NotificationThread.
*/
class NotificationEntry
{
public:

    NotificationEntry();
    NotificationEntry(const void* pData, const PvFrameInfo& metadata);

    /**
    * Returns the frame metadata
    * @return Frame metadata
    */
    const PvFrameInfo& FrameMetadata() const;
    /**
    * Return the pointer to the frame data.
    * @return address of the frame data
    */
    const void* FrameData() const;

private:

    // Copy of the frame metadata
    PvFrameInfo frameMetaData_;

    // Pointer to the frame in circular buffer
    const void* pFrameData_;
};

#endif // _NOTIFICATIONENTRY_H_
