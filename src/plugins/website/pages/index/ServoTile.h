// -----------------------------------------------------------------------------------
// Controller Status tile
#pragma once

#include "../htmlHeaders.h"
#include "../htmlMessages.h"
#include "../htmlScripts.h"

#if DISPLAY_SERVO_MONITOR == ON

extern void servoTile(String &data);
extern void servoTileAjax(String &data);
extern void servoTileGet();

#if DISPLAY_SERVO_ORIGIN_CONTROLS == ON
  const char html_servoZeroEncoders[] PROGMEM =
    "<br />\n"
    "<button onpointerdown=\"if (confirm('" L_ARE_YOU_SURE "?')) s('sv','zro')\" type='button'>" L_ENC_ORIGIN_TO_ONS "</button>"
    "<br />\n";
#endif

const char html_servoSelect[] PROGMEM = "<button id='svax%d' onpointerdown=\"s('svax','%d')\" type='button' class='bb'>%c</button>";

const char html_servoGraph[] PROGMEM = "<canvas id='servoCanvas' width='420' height='300' style='border:1px solid #000000;'></canvas> \n";

const char html_servoScript1[] PROGMEM =
  "<script>\n"

  "var svu=setInterval(updateServo,500);"
  "var svr=false;" // has run?
  "var svt=0;"     // time (x axis, 0.5s ticks)
  "var svd=[];"    // array of servo deltas
  "var svhs=[4,10,20,50,100,500,1000,5000,10000,50000,100000,500000,1000000,500000,10000000,50000000,100000000,500000000];"
  "var svs=10;"    // scale (y axis)
  "var svw=210;"   // half width
  "var svh=300;"   // height
  "var svx=0;"
  "var svy=0;\n"

  "function updateServo() {\n"
    "var canvas=document.getElementById('servoCanvas');"
    "var ctx=canvas.getContext('2d');\n"

    "var deltaText = document.getElementById('svoD').innerText;"
    "if (deltaText=='?') { inactiveServo(ctx); svr=false; return; }"
    "var delta=Number(deltaText);"

    "if (!svr) { for (i=0;i<svw;i++) svd[i]=0; svr=true; svt=0; svx=0; svy=0; clearServo(ctx); }\n"

    "svt++;"
    "var x=svt;"

    "if (x>=svw-1) { x=svw-1; for (i=1;i<=x;i++) { svd[i-1]=svd[i]; } clearServo(ctx); }\n"
    "svd[x]=delta;\n";

  const char html_servoScript2[] PROGMEM =
    "var max=0;"
    "for (i=0;i<=x;i++) { if (Math.abs(svd[i])>max) max=Math.abs(svd[i]); }"

    "var osvs=svs;"
    "for (i=0;i<=18;i++) { if (max<svhs[i]*0.9) { svs=svhs[i]; break; } }"
    "if (osvs!=svs) { clearServo(ctx); }\n"

    "y=svh/2-delta*((svh/2)/svs);"
    "ctx.beginPath(); ctx.moveTo(svx*2,svy); ctx.lineTo(x*2,y); ctx.strokeStyle='" COLOR_SERVO_PEN_4 "'; ctx.stroke();"
    "svx=x; svy=y;"
  "}\n"

  "function inactiveServo(ctx) {\n"
    "ctx.fillStyle='" COLOR_SERVO_BACKGROUND_1 "';"
    "ctx.fillRect(0,0,svw*2,svh);"

    "ctx.fillStyle='" COLOR_SERVO_BACKGROUND_2 "';"
    "ctx.font = 'bold 36px Arial';"
    "ctx.fillText('Inactive',svw-70,svh/2-9);"
  "}"

  "function clearServo(ctx) {\n"
    "ctx.fillStyle='" COLOR_SERVO_BACKGROUND_1 "';"
    "ctx.fillRect(0,0,svw*2,svh);"

    "ctx.strokeStyle='" COLOR_SERVO_PEN_3 "';"
    "ctx.beginPath(); ctx.moveTo(0, svh/2); ctx.lineTo(svw*2, svh/2); ctx.stroke();";

  const char html_servoScript3[] PROGMEM =

    "ctx.strokeStyle='" COLOR_SERVO_PEN_2 "';"
    "ctx.beginPath();"
    "ctx.moveTo(0,svh*0.25); ctx.lineTo(svw*2,svh*0.25);"
    "ctx.moveTo(0,svh*0.75); ctx.lineTo(svw*2,svh*0.75);"
    "ctx.stroke();"

    "ctx.strokeStyle='" COLOR_SERVO_PEN_1 "';"
    "ctx.beginPath();"
    "for (i=60; i<svw*2; i+=60) { ctx.moveTo(i,0); ctx.lineTo(i,svh); }"
    "ctx.stroke();"

    "ctx.fillStyle='" COLOR_SERVO_BACKGROUND_3 "';"
    "ctx.font = 'bold 12px Arial';"
    "ctx.textBaseline = 'top';"
    "ctx.fillText(svs.toString(),2,2);"
    "ctx.fillText(svs.toString()/2,2,svh*0.25+2);"
    "svs=-svs; ctx.fillText(svs.toString()/2,2,svh*0.75+2); svs=-svs;"
    "ctx.fillText('0',2,svh/2+2);"

    "ctx.strokeStyle='" COLOR_SERVO_PEN_4 "';"
    "ctx.beginPath(); svx=0; svy=svh/2+svd[0]/svs;"
    "for (i=0;i<=svt;i++) { ctx.moveTo(svx*2,svy); svx=i; svy=svh/2-svd[i]*((svh/2)/svs); ctx.lineTo(svx*2,svy); }"
    "ctx.stroke();"

  "}\n"

  "</script>\n";
#endif
