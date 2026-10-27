#include "../Source/Dsp.h"
#include <cstdio>
#include <cmath>
#include <vector>
static int fails=0;
#define CHECK(c) do{ if(!(c)){ std::printf("FAIL: %s (line %d)\n",#c,__LINE__); ++fails; } }while(0)
int main(){
    const int N=48000; const double sr=48000.0;
    auto run=[&](float driveDb,float outDb,float amp){
        tanhdrive::Processor p; p.prepare(sr); p.reset(driveDb,outDb);
        std::vector<float> L(N),R(N);
        for(int i=0;i<N;++i){ L[i]=R[i]=amp*std::sin(2*M_PI*220.0*i/sr); }
        float* ch[2]={L.data(),R.data()}; p.process(ch,2,N);
        float peak=0; for(float v:L) peak=std::max(peak,std::fabs(v));
        return std::make_pair(peak,L);
    };
    // 1) tiny signal, 0 dB drive, 0 dB out -> ~identity (tanh(x)~x)
    { auto [pk,_]=run(0,0,0.01f); CHECK(std::fabs(pk-0.01f)<1e-5f); }
    // 2) hard drive: +36 dB on 0.5 amplitude -> peak ~1.0 (tanh saturates at 1)
    { auto [pk,_]=run(36,0,0.5f); CHECK(pk>0.999f && pk<=1.0f); }
    // 3) output -6.02 dB scales the saturated peak to ~0.5
    { auto [pk,_]=run(36,-6.0206f,0.5f); CHECK(std::fabs(pk-0.5f)<1e-3f); }
    // 4) +12 dB output on a saturated signal -> peak ~3.98 (x3.98)
    { auto [pk,_]=run(36,12.0f,0.5f); CHECK(std::fabs(pk-3.981f)<0.01f); }
    // 5) dB conversion
    CHECK(std::fabs(tanhdrive::dbToGain(0)-1.0f)<1e-6f);
    CHECK(std::fabs(tanhdrive::dbToGain(-6.0206f)-0.5f)<1e-4f);
    CHECK(std::fabs(tanhdrive::dbToGain(20)-10.0f)<1e-4f);
    // 6) NaN / inf / huge input never produce NaN
    { tanhdrive::Processor p; p.prepare(sr); p.reset(12,0);
      float a[4]={NAN,INFINITY,-INFINITY,1e30f}; float* ch[1]={a}; p.process(ch,1,4);
      for(float v:a) CHECK(v==v && std::fabs(v)<=1.0f); }
    // 7) smoothing: a parameter jump does not jump the gain instantly
    { tanhdrive::Processor p; p.prepare(sr); p.reset(0,0); p.setOutputDb(-24);
      float a[2]={0.5f,0.5f}; float* ch[1]={a}; p.process(ch,1,2);
      CHECK(a[0]>0.4f); /* still near the old gain after 1 sample */ }
    // 8) symmetric (odd) waveshaper: no DC added to a symmetric input
    { auto [pk,v]=run(24,0,0.7f); double s=0; for(float x:v) s+=x; CHECK(std::fabs(s/N)<1e-3); }
    std::printf(fails? "%d FAILED\n":"all dsp tests passed\n",fails);
    return fails?1:0;
}
