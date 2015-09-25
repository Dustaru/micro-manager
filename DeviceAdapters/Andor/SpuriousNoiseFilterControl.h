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

#ifndef _SPURIOUSNOISEFILTERCONTROL_H_
#define _SPURIOUSNOISEFILTERCONTROL_H_

#include "../../MMDevice/Property.h"

class AndorCamera;

class SpuriousNoiseFilterControl
{
public:
  SpuriousNoiseFilterControl(AndorCamera * cam);

  int OnSpuriousNoiseFilter(MM::PropertyBase* pProp, MM::ActionType eAct);
  int OnSpuriousNoiseFilterThreshold(MM::PropertyBase* pProp, MM::ActionType eAct);
  int OnSpuriousNoiseFilterDescription(MM::PropertyBase* pProp, MM::ActionType eAct);

private:
   typedef MM::Action<SpuriousNoiseFilterControl> CPropertyAction;
   enum MODE { NONE=0, MEDIAN, LEVEL, IQRANGE };
   MODE currentMode_;

   void CreateProperties();

   AndorCamera * camera_;

   typedef std::map<MODE,const char*> MODEMAP;
   MODEMAP modes_;
   MODEMAP modeDescriptions_;
   void InitialiseMaps();
   MODE GetMode(const char * mode);

   double spuriousNoiseFilterThreshold_;
};

#endif
