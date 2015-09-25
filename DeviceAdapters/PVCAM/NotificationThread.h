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

#ifndef _NOTIFICATIONTHREAD_H_
#define _NOTIFICATIONTHREAD_H_

#include "../../MMDevice/DeviceThreads.h"

#include "NotificationEntry.h"

#include <boost/thread/condition_variable.hpp>
#include <deque>

class Universal;

/**
* The purpose of the NotificationThread is to reduce the risk of lost frames
* due to temporary high CPU loads or delays in MMCore that blocks the PVCAM
* callback thread for excessive amount of time - this in turn leads to lost
* frames because the callback for the frame cannot be called.
*
* The NotificationThread cannot cure slow CPU, slow MMCore or super fast
* cameras used with generally slow application. In such cases a dropped frames
* are inevitable.
*
* Operation
*
* When PVCAM finishes reading out a new frame and sends a callback to the
* adapter we push the frame pointer and  corresponding frame metadata to the 
* NotificationQueue. The frame data is not copied, however the metadata is
* copied.
* The PVCAM callback thread is immediately released, allowing PVCAM to wait for
* the next frame, reducing the risk of losing a callback due to blocking the
* PVCAM callback thread too long.
* The NotificationThread is notified about new frame available, pops the
* pointer from the queue and starts pushing it to MMCore.
* During that period the PVCAM callback for next frame may arrive and next
* frame pointer is pushed to the NotificationThread.
*
* Ideally the actual size of the NotificationQueue is 1 frame. However during
* temporary CPU load hiccups the NotificationQueue may grow a little.
*
* If the camera produces frames faster than MM can consume them, there is no
* cure for this. In such situations we always drop the oldest frame from the
* queue. The MaxSize of the NotificationQueue should always be a couple of
* frames less than the circular buffer size - this is to avoid a situation
* where PVCAM starts overwritting a frame in the circular buffer that is
* currently being copied by the MMCore to its own "Sequence Buffer".
*
* For example, if we have CircularBuffer size of 16 frames and max Queue size
* of 10 frames there is a 4 frames gap. This 4 frames gap should give us 
* enough time to copy the oldest frame to MMCore (without a risk of being
* overwriten by PVCAM) if the NotificationQueue is getting full.
*
* This entire class would not have to be used if PVCAM had its own callback
* queue and deliver the callbacks more reliably.
*/
class NotificationThread : public MMDeviceThreadBase
{
public:

    NotificationThread(Universal* pCamera);
    ~NotificationThread(void);

    /**
    * Resets the thread. This should be called before every sequence acquisition.
    */
    void Reset();

    /**
    * Sets the queue capacity. The capacity should be kept slightly lower than
    * is the circular buffer frame count.
    * @param capacity Maximum number of notification the queue can hold. If the
    * queue is full the old notifications are thrown away.
    */
    void SetQueueCapacity(int capacity);
    /**
    * Returns the maximum capacity of the queue.
    * @return Maximum number of notifications.
    */
    int  Capacity() const;

    /**
    * Pushes a new notification to the queue. If the queue is full the
    * the oldes notification is thrown away.
    * @param entry Notification to be pushed to the queue.
    */
    bool PushNotification( const NotificationEntry& entry );
    bool PushNotification( const NotificationEntry&& entry );


public:

    // From MMDeviceThreadBase
    int svc();

private:

    void requestStop();
    bool waitNextNotification( NotificationEntry& e );

    Universal*                universal_;

    boost::condition_variable frameReadyCondition_;
    boost::mutex              threadMutex_;

    MMThreadLock              queueLock_;

    std::deque<NotificationEntry> deque_;
    int                       maxSize_;

    bool                      overflowed_;
    bool                      requestStop_;
};

#endif // _NOTIFICATIONTHREAD_H_
