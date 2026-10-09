"""Pulse timing regression using functions extracted from current firmware.

Hardware, time, task scheduler and unrelated subsystems are mocked. This is
not a device measurement. Both guide axes, tracking calculation/application,
guide expiry and coordinate transforms are compiled from the current source.
Run with Python 3 and g++; ASan and UBSan are enabled.
"""
from pathlib import Path
import subprocess
import re
import tempfile

root = Path(__file__).resolve().parents[2] / 'src/telescope/mount'
sources = {name: (root / name).read_text() for name in
           ['Mount.cpp', 'guide/Guide.cpp', 'coordinates/Transform.cpp']}

def extract(file, signature):
    # Resolve conditional compilation first: update() has alternative opening
    # braces inside #if/#else, which a raw brace counter cannot interpret.
    raw = re.sub(r'^\s*#include.*$', '', sources[file], flags=re.MULTILINE)
    src = subprocess.check_output(['g++','-E','-P','-x','c++','-'],
        input=prefix+'\n#define MOUNT_PRESENT\n'+raw,text=True)
    start = src.index(signature)
    brace = src.index('{', start)
    depth = 1
    end = brace + 1
    while depth:
        if src[end] == '{': depth += 1
        elif src[end] == '}': depth -= 1
        end += 1
    return src[start:end]

prefix = r'''
#include <cmath>
#include <iostream>
#include <cassert>
#include <cstdint>
#include <functional>
using std::isnan; using std::isinf;
#define ON 1
#define OFF 0
#define NV_INIT_ERROR_REVOKES_AUTHORITY 0
#define MOUNT_COORDS_MEMORY 0
#define GOTO_FEATURE 0
#define AXIS1_PEC 0
#define DEBUG 0
#define AXIS2_SLEW_RATE_PERCENT 100
std::function<void()> onYield;
void yieldTask(){if(onYield){auto callback=onYield;onYield=nullptr;callback();}}
#define Y yieldTask()
#define VF(...)
#define V(...)
#define VL(...)
#define VLF(...)
#define DLF(...)
constexpr double Deg90=M_PI/2, Deg180=M_PI, Deg360=M_PI*2;
constexpr double TenthArcSec=M_PI/(180*36000);
constexpr int ALTAZM=1, ALTALT=2, EQ=3;
constexpr int TS_NONE=0, TS_SIDEREAL=1, GS_NONE=0, CR_MOUNT_ALL=0;
constexpr int RC_NONE=0, RC_MODEL=1, RC_MODEL_DUAL=2, RC_REFRACTION=3, RC_REFRACTION_DUAL=4;
constexpr int SF_STOPPED=0, SF_SLEWING=1, PIER_SIDE_WEST=1;
using PierSide = int;
enum GuideAction {GA_NONE,GA_BREAK,GA_FORWARD,GA_REVERSE,GA_HOME};
enum GuideState {GU_NONE,GU_PULSE_GUIDE,GU_GUIDE,GU_SPIRAL_GUIDE,GU_HOME_GUIDE,GU_HOME_GUIDE_ABORT};
enum GuideRateSelect {GR_CUSTOM,GR_1X,GR_MAX};
enum CommandError {CE_NONE,CE_SLEW_ERR_OUTSIDE_LIMITS};
unsigned long clockMs=0; unsigned long millis(){return clockMs;}
double siderealToRad(double x){return x*M_PI/(180*240);}
float siderealToRadF(float x){return siderealToRad(x);}
float degToRadF(float x){return x*M_PI/180;}
double radToHrs(double x){return x*12/M_PI;}
constexpr float SIDEREAL_RATIO_F=1;
struct Coordinate {double h=-0.03,d=45*M_PI/180,a=0,aa2=0,z=0,aa1=0;int pierSide=0;};
struct Site {struct {struct {double sine=sin(37*M_PI/180),cosine=cos(37*M_PI/180);} latitude;} locationEx;float getSiderealRatio(){return 1;}} site;
struct Transform {
 int mountType=ALTAZM;
 bool isEquatorial(){return mountType==EQ;}
 void equToHor(Coordinate*);
 void horToEqu(Coordinate*);
 void horToAa(Coordinate*);
 void aaToHor(Coordinate*);
 void equToAa(Coordinate* c){equToHor(c);horToAa(c);}
 void aaToEqu(Coordinate* c){aaToHor(c);horToEqu(c);}
 void mountToTopocentric(Coordinate*){}
 void topocentricToObservedPlace(Coordinate*){}
 void observedPlaceToMount(Coordinate*){}
} transform;
struct Axis {
 float frequency=0;
 void setSynchronizedFrequency(float x){frequency=x;}
 void setPowerDownOverrideTime(unsigned long){}
 void setFrequencySlew(float){}
 void autoSlewAbort(){} void autoSlewStop(){} bool isEnabled(){return true;}
 bool isSlewing(){return false;}
} axis1,axis2;
struct {bool isEnabled(){return true;}} limits;
struct {bool trusted(){return true;} void setTrusted(bool){}} startupAuthority;
struct {int state=GS_NONE;void abort(){}} goTo;
struct {void flashRate(int){} void soundAlert(){}} mountStatus;
struct {void guideDone(bool){}} home;
struct Mount {
 int trackingState=TS_SIDEREAL;bool xBusy=false;
 float trackingRate=1,trackingRateAxis1=0,trackingRateAxis2=0;
 float trackingRateOffsetRA=0,trackingRateOffsetDec=0;
 struct {int rc=RC_NONE;} settings;
 Coordinate current;
 bool motorFault(){return false;} void enable(bool){}
 void updatePosition(int){transform.equToHor(&current);if(transform.mountType==ALTALT)transform.horToAa(&current);}
 bool isSlewing(){return false;}
 uint32_t trackingRateRevision=0;
 void update(bool guideChanged=false);void poll();
 void updateTrackingRates(bool immediate=false);
} mount;
struct Guide {
 GuideState state=GU_NONE;GuideAction guideActionAxis1=GA_NONE,guideActionAxis2=GA_NONE;
 float rateAxis1=0,rateAxis2=0,selectedRate=1;int pierSide=0;
 unsigned long guideFinishTimeAxis1=0,guideFinishTimeAxis2=0;
 bool activeAxis1(){return guideActionAxis1!=GA_NONE;}
 bool activeAxis2(){return guideActionAxis2!=GA_NONE;}
 void abort(){} CommandError validate(int,GuideAction){return CE_NONE;}
 float rateSelectToRate(GuideRateSelect r,int){return r==GR_MAX?100:selectedRate;}
 void finalizeTimeLimit(unsigned long&){} void backlashEnableControl(bool){}
 void axis1AutoSlew(GuideAction){} void axis2AutoSlew(GuideAction){}
 void spiralPoll(){} void poll();void stop(){stopAxis1();stopAxis2();}
 CommandError startAxis1(GuideAction,GuideRateSelect,unsigned long,bool);
 void stopAxis1(GuideAction=GA_BREAK,bool=false);
 CommandError startAxis2(GuideAction,GuideRateSelect,unsigned long,bool);
 void stopAxis2(GuideAction=GA_BREAK,bool=false);
} guide;
'''

functions = '\n'.join(extract(file, sig) for file, sig in [
    ('coordinates/Transform.cpp', 'void Transform::equToHor('),
    ('coordinates/Transform.cpp', 'void Transform::horToEqu('),
    ('coordinates/Transform.cpp', 'void Transform::horToAa('),
    ('coordinates/Transform.cpp', 'void Transform::aaToHor('),
    ('Mount.cpp', 'void Mount::update('),
    ('Mount.cpp', 'void Mount::poll()'),
    ('Mount.cpp', 'void Mount::updateTrackingRates('),
    ('guide/Guide.cpp', 'CommandError Guide::startAxis1('),
    ('guide/Guide.cpp', 'void Guide::stopAxis1('),
    ('guide/Guide.cpp', 'CommandError Guide::startAxis2('),
    ('guide/Guide.cpp', 'void Guide::stopAxis2('),
    ('guide/Guide.cpp', 'void Guide::poll()'),
])

main = r'''
void run(int type,int start,int duration,int axis,float rate){
 transform.mountType=type;mount=Mount();guide=Guide();clockMs=0;
 guide.selectedRate=rate;
 mount.poll();float base1=axis1.frequency,base2=axis2.frequency;
 double area=0;int first=-1,last=-1;
 for(clockMs=0;clockMs<4000;clockMs++){
   if(clockMs==(unsigned long)start){
     if(axis==1)guide.startAxis1(GA_FORWARD,GR_1X,duration,true);
     else guide.startAxis2(GA_FORWARD,GR_1X,duration,true);
   }
   // Guide monitor is 5ms, higher priority than the 1s tracking task.
   if(clockMs%5==0)guide.poll();
   if(clockMs%1000==0)mount.poll();
   double extra=std::hypot(axis1.frequency-base1,axis2.frequency-base2);
   if(extra>1e-9){if(first<0)first=clockMs;last=clockMs+1;area+=extra/1000;}
 }
 assert(first==start);
 assert(last==((start+duration+4)/5)*5);
}
void interleaving(int type){
 transform.mountType=type;mount=Mount();guide=Guide();clockMs=0;mount.poll();
 float base1=axis1.frequency,base2=axis2.frequency;
 guide.startAxis1(GA_FORWARD,GR_1X,200,true);
 guide.startAxis2(GA_FORWARD,GR_1X,500,true);
 clockMs=200;guide.poll();
 assert(guide.rateAxis1==0 && guide.rateAxis2==1);
 float one=axis1.frequency,two=axis2.frequency;
 // A recalculation with the remaining axis must preserve its correction.
 mount.poll();assert(fabs(axis1.frequency-one)<1e-8 && fabs(axis2.frequency-two)<1e-8);
 // Expiry preempts the periodic calculation after its coordinate snapshot.
 // The immediate refresh sees a newer position; the interrupted calculation
 // must not overwrite it with the older snapshot when it resumes.
 onYield=[&](){
   mount.current.h+=0.01;clockMs=500;guide.poll();
   base1=axis1.frequency;base2=axis2.frequency;
 };mount.poll();
 assert(guide.state==GU_NONE);
 assert(fabs(axis1.frequency-base1)<1e-8 && fabs(axis2.frequency-base2)<1e-8);
 // Start during a periodic calculation, then reverse and explicitly abort.
 onYield=[](){guide.startAxis2(GA_FORWARD,GR_1X,200,true);};mount.poll();
 one=axis1.frequency;two=axis2.frequency;
 assert(std::hypot(one-base1,two-base2)>1e-8);
 guide.startAxis2(GA_REVERSE,GR_1X,200,true);
 assert(std::hypot(axis1.frequency-one,axis2.frequency-two)>1e-8);
 guide.stopAxis2(GA_BREAK,true);
 assert(fabs(axis1.frequency-base1)<1e-8 && fabs(axis2.frequency-base2)<1e-8);
}
int main(){
 int count=0;
 for(int type:{ALTAZM,ALTALT,EQ})
  for(int axis:{1,2})
   for(float rate:{1.0f,0.001f})
    for(int start:{0,100,900,995})
     for(int duration:{1,20,100,200,1500}){
      run(type,start,duration,axis,rate);++count;
     }
 for(int type:{ALTAZM,ALTALT})interleaving(type);
 std::cout<<count<<" timing cases and two-axis/preemption/reversal/abort checks passed\n";
}
'''
with tempfile.TemporaryDirectory(prefix='onstep-pulse-test-') as tmp:
    cpp = Path(tmp)/'test.cpp'
    binary = Path(tmp)/'test'
    cpp.write_text(prefix+functions+main)
    subprocess.run(['g++','-std=c++17','-O0','-g',
                    '-fsanitize=address,undefined','-fno-sanitize-recover=all',
                    str(cpp),'-o',str(binary)],check=True)
    subprocess.run([str(binary)],check=True)
