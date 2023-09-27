// -----------------------------------------------------------------------------------
// GOTO ASSIST GEOMETRIC ALIGN FOR ALT/AZM AND EQ MOUNTS (HIGH SPEED)
//
// by Howard Dutton
//
// Copyright (C) 2012 to 2021 Howard Dutton
//

#include "Align.hs.h"

#if defined(MOUNT_PRESENT) && defined(HIGH_SPEED_ALIGN)

#include "../../../lib/tasks/OnTask.h"

#include "../coordinates/Transform.h"
#include "../../Telescope.h"

#if ALIGN_MAX_NUM_STARS > 1

uint8_t modelNumberStars = 0;
void autoModelWrapper() { transform.align.autoModel(modelNumberStars); }

void GeoAlign::init(int8_t mountType, float latitude) {
  modelClear();

  this->mountType = mountType;
  if (mountType == ALTAZM) {
    cosLat = cosf(Deg90);
    sinLat = sinf(Deg90);
  } else {
    cosLat = cosf(latitude);
    sinLat = sinf(latitude);
  }
}


void GeoAlign::modelRead() {
  // get misc settings from NV
  if (AlignModelSize < sizeof(AlignModel)) { nv.initError = true; DL("ERR: GeoAlign::readModel(), AlignModelSize error"); }
  nv.readBytes(NV_ALIGN_MODEL_BASE, &model, AlignModelSize);
  if (model.ax1Cor < -Deg360 || model.ax1Cor > Deg360) { model.ax1Cor = 0; DLF("ERR: GeoAlign::readModel(), bad NV ax1Cor"); }
  if (model.ax2Cor < -Deg360 || model.ax2Cor > Deg360) { model.ax2Cor = 0; DLF("ERR: GeoAlign::readModel(), bad NV ax2Cor"); }
  if (model.dfCor  <    -256 || model.dfCor  >    256) { model.dfCor  = 0; DLF("ERR: GeoAlign::readModel(), bad NV dfCor");  }
  if (model.tfCor  <    -128 || model.tfCor  >    128) { model.tfCor  = 0; DLF("ERR: GeoAlign::readModel(), bad NV tfCor");  }
  if (model.doCor  <   -8192 || model.doCor  >   8192) { model.doCor  = 0; DLF("ERR: GeoAlign::readModel(), bad NV doCor");  }
  if (model.pdCor  <    -256 || model.pdCor  >    256) { model.pdCor  = 0; DLF("ERR: GeoAlign::readModel(), bad NV pdCor");  }
  if (model.altCor <  -16384 || model.altCor >  16384) { model.altCor = 0; DLF("ERR: GeoAlign::readModel(), bad NV altCor"); }
  if (model.azmCor <  -16384 || model.azmCor >  16384) { model.azmCor = 0; DLF("ERR: GeoAlign::readModel(), bad NV azmCor"); }
}

void GeoAlign::modelWrite() {
  if (AlignModelSize < sizeof(AlignModel)) { nv.initError = true; DL("ERR: GeoAlign::writeModel(), AlignModelSize error"); }
  nv.updateBytes(NV_ALIGN_MODEL_BASE, &model, AlignModelSize);
}

void GeoAlign::modelClear() {
  modelNumberStars = 0;
  model.ax1Cor = 0;  // align internal index for Axis1
  model.ax2Cor = 0;  // align internal index for Axis2
  model.altCor = 0;  // polar error relative to NCP/SCP/Zenith, - is below & + above
  model.azmCor = 0;  // - is right & + is left
  model.doCor  = 0;  // altitude axis/optics orthogonal correction
  model.pdCor  = 0;  // altitude axis/Azimuth orthogonal correction
  model.dfCor  = 0;  // altitude axis axis flex
  model.tfCor  = 0;  // tube flex
  modelIsReady = false;
}

bool GeoAlign::modelReady() {
  return modelIsReady;
}

CommandError GeoAlign::addStar(int thisStar, int numberStars, Coordinate *actual, Coordinate *mount) {
  // just return if we are processing a model or the star count is out of range, this should never happen
  if (autoModelTask != 0 || thisStar < 1 || thisStar > ALIGN_MAX_NUM_STARS || numberStars < 1 || numberStars > ALIGN_MAX_NUM_STARS) return CE_ALIGN_FAIL;

  int i = thisStar - 1;

  this->mount[i].h = mount->h;
  this->mount[i].d = mount->d;
  this->actual[i].h = actual->h;
  this->actual[i].d = actual->d;

  if (mountType == ALTAZM) {
    transform.equToHor(mount);
    this->mount[i].ax1 = mount->z;
    this->mount[i].ax2 = mount->a;

    transform.equToHor(actual);
    this->actual[i].ax1 = actual->z;
    this->actual[i].ax2 = actual->a;
  } else {
    this->mount[i].ax1 = mount->h;
    this->mount[i].ax2 = mount->d;

    this->actual[i].ax1 = actual->h;
    this->actual[i].ax2 = actual->d;
  }

  if (mount->pierSide == PIER_SIDE_WEST) {
    this->actual[i].side = -1;
    this->mount[i].side = -1;
  } else {
    this->actual[i].side = 1;
    this->mount[i].side = 1;
  }

  // two or more stars and finished
  if (thisStar >= 2 && thisStar == numberStars) {
    createModel(numberStars);
  }

  return CE_NONE;
}

void GeoAlign::createModel(int numberStars) {
  if (autoModelTask != 0) return;

  // start a task to solve for the model
  modelNumberStars = numberStars;
  autoModelTask = tasks.add(1, 0, false, 6, autoModelWrapper, "Align");
}

// returns the correction to be added to the requested RA,Dec to yield the actual RA,Dec that we will arrive at
void GeoAlign::correct(AlignCoordinate &mount, float sf, float _deo, float _pd, float _pz, float _pe, float _df, float _ff, float _tf, float *a1r, float *a2r) {
  float DO1,DOh;
  float PD,PDh;
  float PZ,PA;
  float DF,DFd,TF,FF,FFd,TFh,TFd;

  // ------------------------------------------------------------
  // A. Misalignment due to tube/optics not being perp. to Dec axis
  // negative numbers are further (S) from the NCP, swing to the
  // equator and the effect on declination is 0. At the SCP it
  // becomes a (N) offset.  Unchanged with meridian flips.
  DO1 = _deo*sf;
  // works on HA.  meridian flips effect this in HA
  DOh = DO1*(1.0F/mount.cosA2)*mount.side;

  // ------------------------------------------------------------
  // B. Misalignment, Declination axis relative to Polar axis
  // expressed as a correction to where the Polar axis is pointing
  // negative numbers are further (S) from the NCP, swing to the
  // equator and the effect on declination is 0.
  // At the SCP it is, again, a (S) offset
  PD  = _pd*sf;
  // works on HA.
  PDh = -PD*mount.tanA2*mount.side;

  // ------------------------------------------------------------
  // Misalignment, relative to NCP
  // negative numbers are east of the pole
  // C. polar left-right misalignment
  PZ  = _pz*sf;
  // D. negative numbers are below the pole
  // polar below-above misalignment
  PA  = _pe*sf;

  // ------------------------------------------------------------
  // Axis flex
  DF  = _df*sf;
  DFd = -DF*(cosLat*mount.cosA1+sinLat*mount.tanA2);

  // ------------------------------------------------------------
  // Fork flex
  FF  = _ff*sf;
  FFd = FF*mount.cosA1;

  // ------------------------------------------------------------
  // Optical axis sag
  TF  = _tf*sf;

  TFh = TF*(cosLat*mount.sinA1*(1.0/mount.cosA2));
  TFd = TF*(cosLat*mount.cosA1*mount.sinA2-sinLat*mount.cosA2);

  // ------------------------------------------------------------
  *a1r  = (-PZ*mount.cosA1*mount.tanA2 + PA*mount.sinA1*mount.tanA2 + DOh + PDh + TFh);
  *a2r  = (+PZ*mount.sinA1             + PA*mount.cosA1             + DFd + FFd + TFd);
}

void GeoAlign::doSearch(float sf, int p1, int p2, int p3, int p4, int p5, int p6, int p7, int p8, int p9) {
  long l,

  _deo_m,_deo_p,
  _pd_m,_pd_p,
  _pz_m,_pz_p,
  _pe_m,_pe_p,
  _df_m,_df_p,
  _tf_m,_tf_p,
  _ff_m,_ff_p,
  _oh_m,_oh_p,
  _od_m,_od_p,
  
  _deo,_pd,_pz,_pe, _df,_tf,_ff, _ode,_ohe;

  float sf1 = arcsecToRad(sf);

  // search
  // set Parameter Space
  _deo_m= -p1 + round(best_deo/sf); _deo_p= p1 + round(best_deo/sf);
  _pd_m = -p2 + round(best_pd/sf);  _pd_p = p2 + round(best_pd/sf);
  _pz_m = -p3 + round(best_pz/sf);  _pz_p = p3 + round(best_pz/sf);
  _pe_m = -p4 + round(best_pe/sf);  _pe_p = p4 + round(best_pe/sf);
  _tf_m = -p5 + round(best_tf/sf);  _tf_p = p5 + round(best_tf/sf);
  _ff_m = -p6 + round(best_ff/sf);  _ff_p = p6 + round(best_ff/sf);
  _df_m = -p7 + round(best_df/sf);  _df_p = p7 + round(best_df/sf);
  _od_m = -p8 + round(best_ode/sf); _od_p = p8 + round(best_ode/sf);
  _oh_m = -p9 + round(best_ohe/sf); _oh_p = p9 + round(best_ohe/sf);

  float ma2, ma1;
  for (_ohe = _oh_m; _ohe <= _oh_p; _ohe++)
  for (_ode = _od_m; _ode <= _od_p; _ode++) {
    ode = _ode*sf1;
    odw = -ode;
    ohe = _ohe*sf1;
    ohw = ohe;

    for (l = 0; l < num; l++) {
      ma1 = mount[l].ax1;
      ma2 = mount[l].ax2;
      
      if (mount[l].side == -1) // west of the mount
      {
        ma1 = ma1 + ohw;
        ma2 = ma2 + odw;
      } else
      if (mount[l].side == 1) // east of the mount, default (fork mounts)
      {
        ma1 = ma1 + ohe;
        ma2 = ma2 + ode;
      }

      mount[l].ma1 = ma1;
      mount[l].ma2 = ma2;
      mount[l].sinA1 = sinf(ma1);
      mount[l].cosA1 = cosf(ma1);
      mount[l].sinA2 = sinf(ma2);
      mount[l].cosA2 = cosf(ma2);
      mount[l].tanA2 = mount[l].sinA2/mount[l].cosA2;
    }

    for (_deo = _deo_m; _deo <= _deo_p; _deo++)
    for (_pd = _pd_m; _pd <= _pd_p; _pd++)
    for (_pz = _pz_m; _pz <= _pz_p; _pz++)
    for (_pe = _pe_m; _pe <= _pe_p; _pe++)
    for (_df = _df_m; _df <= _df_p; _df++)
    for (_ff = _ff_m; _ff <= _ff_p; _ff++)
    for (_tf = _tf_m; _tf <= _tf_p; _tf++) {

      // check the combinations for all samples
      for (l = 0; l < num; l++) {
        float ma1r, ma2r;
        correct(mount[l], sf1, _deo, _pd, _pz, _pe, _df, _ff, _tf, &ma1r, &ma2r);

        delta[l].ax1 = actual[l].ax1 - (mount[l].ma1 - ma1r);
        if (delta[l].ax1 >  Deg180) delta[l].ax1 = delta[l].ax1 - Deg360; else
        if (delta[l].ax1 < -Deg180) delta[l].ax1 = delta[l].ax1 + Deg360;
        delta[l].ax2 = actual[l].ax2 - (mount[l].ma2 - ma2r);
        delta[l].side = mount[l].side;
      }

      // calculate the standard deviations
      float a, b;

      sum1 = 0.0;
      for (l = 0; l < num; l++) sum1 = sum1 + sq(delta[l].ax1*cosf(actual[l].ax2));
      a = sum1/(num - 1); // was sqrt(sum1/(num - 1))

      sum1 = 0.0;
      for (l = 0; l < num; l++) sum1 = sum1 + sq(delta[l].ax2);
      b = sum1/(num - 1); // was sqrt(sum1/(num - 1))

      max_dist = sqrtf(a + b); // was sq(a) + sq(b)

      // remember the best fit
      if (max_dist < best_dist) {
        best_dist = max_dist;
        best_deo  = _deo*sf;
        best_pd   = _pd*sf;
        best_pz   = _pz*sf;
        best_pe   = _pe*sf;

        best_tf   = _tf*sf;
        best_df   = _df*sf;
        best_ff   = _ff*sf;
        
        if (p8 != 0) best_odw = radToArcsec(odw); else best_odw = best_pe/2.0;
        if (p8 != 0) best_ode = radToArcsec(ode); else best_ode = -best_pe/2.0;
        if (p9 != 0) best_ohw = radToArcsec(ohw);
        if (p9 != 0) best_ohe = radToArcsec(ohe);
      }
      
      Y;
    }
  }
}

void GeoAlign::autoModel(int n) {
  modelIsReady = false;

  VLF("MSG: Align, calculate pointing model start");

  // how many stars?
  num = n;

  best_dist = 3600.0F*180.0F;
  best_deo  = 0.0F;
  best_pd   = 0.0F;
  best_pz   = 0.0F;
  best_pe   = 0.0F;
  best_tf   = 0.0F;
  best_ff   = 0.0F;
  best_df   = 0.0F;
  best_ode  = 0.0F;
  best_ohe  = 0.0F;

  // figure out the average Axis1 offset as a starting point
  ohe = 0;
  float diff;
  for (l = 0; l < num; l++) {
    diff = actual[l].ax1 - mount[l].ax1;
    if (diff >  Deg180) diff = diff - Deg360;
    if (diff < -Deg180) diff = diff + Deg360;
    ohe = ohe + diff;
  }
  ohe = ohe/num;
  best_ohe = round(radToArcsec(ohe));
  best_ohw = best_ohe;

  // fork flex or dec axis flex, as appropriate
  if (mountType == ALTAZM) { Ff = 0; Df = 0; } else if (mountType == FORK) { Ff = 1; Df = 0; } else { Ff = 0; Df = 1; }

  // only search for cone error if > 2 stars
  int Do = 0;
  if (num > 2) Do = 1;

  // search, this can handle about 9 degrees of polar misalignment, and 4 degrees of cone error
  //              DoPdPzPeTfFf Df OdOh
  doSearch(16384,0 ,0,1,1,0, 0, 0,1,1);
  doSearch( 8192,Do,0,1,1,0, 0, 0,1,1);
  doSearch( 4096,Do,0,1,1,0, 0, 0,1,1);
  doSearch( 2048,Do,0,1,1,0, 0, 0,1,1);
  doSearch( 1024,Do,0,1,1,0, 0, 0,1,1);
  doSearch(  512,Do,0,1,1,0, 0, 0,1,1);
  #ifdef HAL_SLOW_PROCESSOR
    doSearch(256,Do,0,1,1,0, 0, 0,1,1);
    doSearch(128,Do,0,1,1,0, 0, 0,1,1);
    doSearch( 64,Do,0,1,1,0, 0, 0,1,1);
  #else
    if (num > 4) {
      doSearch(256,Do,1,1,1,0,Ff,Df,1,1);
      doSearch(128,Do,1,1,1,1,Ff,Df,1,1);
      doSearch( 64,Do,1,1,1,1,Ff,Df,1,1);
      #ifdef HAL_FAST_PROCESSOR
        doSearch( 32,Do,1,1,1,1,Ff,Df,1,1);
        doSearch( 16,Do,1,1,1,1,Ff,Df,1,1);
        doSearch(  8,Do,1,1,1,1,Ff,Df,1,1);
        #ifdef HAL_VFAST_PROCESSOR
          doSearch(  4,Do,1,1,1,1,Ff,Df,1,1);
        #endif
      #endif
    } else {
      doSearch(256,Do,0,1,1,0, 0, 0,1,1);
      doSearch(128,Do,0,1,1,0, 0, 0,1,1);
      doSearch( 64,Do,0,1,1,0, 0, 0,1,1);
      doSearch( 32,Do,0,1,1,0, 0, 0,1,1);
      #ifdef HAL_FAST_PROCESSOR
        doSearch( 16,Do,0,1,1,0, 0, 0,1,1);
        doSearch(  8,Do,0,1,1,0, 0, 0,1,1);
        #ifdef HAL_VFAST_PROCESSOR
          doSearch(  4,Do,0,1,1,0, 0, 0,1,1);
        #endif
      #endif
    }
  #endif

  // geometric corrections
  model.doCor = arcsecToRad(best_deo);
  model.pdCor = arcsecToRad(best_pd);
  model.azmCor = arcsecToRad(best_pz);
  model.altCor = arcsecToRad(best_pe);

  model.tfCor = arcsecToRad(best_tf);
  if (mountType == FORK || mountType == ALTAZM) model.dfCor = arcsecToRad(best_ff); else model.dfCor = arcsecToRad(best_df);

  model.ax1Cor = arcsecToRad(best_ohw);
  model.ax2Cor = arcsecToRad(best_odw);

  // update status and exit
  modelIsReady = true;

  VLF("MSG: Align, calculate pointing model done");
  tasks.setDurationComplete(autoModelTask);
  autoModelTask = 0;
}

void GeoAlign::observedPlaceToMount(Coordinate *coord) {
  if (!modelIsReady) return;

  float p = 1.0F;
  if (coord->pierSide == PIER_SIDE_WEST) p = -1.0F;
  
  float ax1, ax2;
  if (mountType == ALTAZM) {
    ax1 = coord->z;
    ax2 = coord->a;
  } else {
    ax1 = coord->h;
    ax2 = coord->d;
  }
  
  if (ax2 >  Deg90) ax2 =  Deg90;
  if (ax2 < -Deg90) ax2 = -Deg90;

  // initial rough guess at instrument coordinate
  float a1 = ax1;
  float a2 = ax2;

  // breaks-down near the poles (limited to > 1' from pole)
  if (fabs(ax2) < degToRadF(89.98333333F)) {
    for (int pass = 0; pass < 3; pass++) {
      float sinAx2 = sinf(a2);
      float cosAx2 = cosf(a2);
      float sinAx1 = sinf(a1);
      float cosAx1 = cosf(a1);

      // ------------------------------------------------------------
      // misalignment due to tube/optics not being perp. to Alt axis
      // negative numbers are further (down) from the Zenith, swing to the
      // horizon and the effect on Alt is 0. At the Nadir it
      // becomes an (up) offset.  Unchanged with meridian flips.
      // expressed as a correction to the Zenith axis misalignment
      float DOh = model.doCor*(1.0F/cosAx2)*p;
  
      // ------------------------------------------------------------
      // misalignment due to Alt axis being perp. to Azm axis
      float PDh = -model.pdCor*(sinAx2/cosAx2)*p;
  
      // Fork or Axis flex
      float DFd;
      if (mountType == FORK || mountType == ALTAZM) DFd = model.dfCor*cosAx1; else DFd = -model.dfCor*(cosLat*cosAx1 + sinLat*(sinAx2/cosAx2));
  
      // Tube flex
      float TFh = model.tfCor*(cosLat*sinAx1*(1.0F/cosAx2));
      float TFd = model.tfCor*(cosLat*cosAx1*sinAx2 - sinLat*cosAx2);
  
      // polar misalignment
      float ax1c = -model.azmCor*cosAx1*(sinAx2/cosAx2) + model.altCor*sinAx1*(sinAx2/cosAx2);
      float ax2c = +model.azmCor*sinAx1                 + model.altCor*cosAx1;

      // improved guess at instrument coordinate
      a1 = ax1 + (ax1c + PDh + DOh + TFh);
      a2 = ax2 + (ax2c + DFd + TFd);
    }
  }

  // finally, apply the index offsets
  a1 = a1 - model.ax1Cor;
  a2 = a2 - model.ax2Cor*-p;

  if (mountType == ALTAZM) {
    coord->z = a1;
    coord->a = a2;
  } else {
    coord->h = a1;
    coord->d = a2;
  }
}

void GeoAlign::mountToObservedPlace(Coordinate *coord) {
  if (!modelIsReady) return;

  float p = 1.0F;
  if (coord->pierSide == PIER_SIDE_WEST) p = -1.0F;

  float ax1, ax2;
  if (mountType == ALTAZM) {
    ax1 = coord->z;
    ax2 = coord->a;
  } else {
    ax1 = coord->h;
    ax2 = coord->d;
  }
  
  ax1 = ax1 + model.ax1Cor;
  ax2 = ax2 + model.ax2Cor*-p;
  
  if (ax2 >  Deg90) ax2 =  Deg90;
  if (ax2 < -Deg90) ax2 = -Deg90;

  // breaks-down near the Zenith (limited to > 1' from Zenith)
  if (fabs(ax2) < degToRadF(89.98333333F)) {
    float sinAx2 = sinf(ax2);
    float cosAx2 = cosf(ax2);
    float sinAx1 = sinf(ax1);
    float cosAx1 = cosf(ax1);

    // ------------------------------------------------------------
    // misalignment due to tube/optics not being perp. to Alt axis
    // negative numbers are further (S) from the Zenith, swing to the
    // horizon and the effect on Alt is 0. At the Nadir it
    // becomes a (N) offset.  Unchanged with meridian flips.
    // expressed as a correction to the Azm axis misalignment
    float DOh = model.doCor*(1.0F/cosAx2)*p;

    // as the above offset becomes zero near the horizon, the affect
    // works on Azm instead.  meridian flips affect this in Azm
    float PDh = -model.pdCor*(sinAx2/cosAx2)*p;

    // Fork or Axis flex
    float DFd;
    if (mountType == FORK || mountType == ALTAZM) DFd = model.dfCor*cosAx1; else DFd = -model.dfCor*(cosLat*cosAx1 + sinLat*(sinAx2/cosAx2));

    // Tube flex
    float TFh = model.tfCor*(cosLat*sinAx1*(1.0F/cosAx2));
    float TFd = model.tfCor*(cosLat*cosAx1*sinAx2 - sinLat*cosAx2);
   
    // ------------------------------------------------------------
    // polar misalignment
    float a1 = -model.azmCor*cosAx1*(sinAx2/cosAx2) + model.altCor*sinAx1*(sinAx2/cosAx2);
    float a2 = +model.azmCor*sinAx1                 + model.altCor*cosAx1;

    ax1 = ax1 - (a1 + PDh + DOh + TFh);
    ax2 = ax2 - (a2 + DFd + TFd);
  }

  if (ax2 >  Deg90) ax2 =  Deg90;
  if (ax2 < -Deg90) ax2 = -Deg90;

  if (mountType == ALTAZM) {
    while (ax1 >  Deg360) ax1 -= Deg360;
    while (ax1 < -Deg360) ax1 += Deg360;
    coord->z = ax1;
    coord->a = ax2;
  } else {
    while (ax1 >  Deg180) ax1 -= Deg360;
    while (ax1 < -Deg180) ax1 += Deg360;
    coord->h = ax1;
    coord->d = ax2;
  }
}

#endif

#endif
