/* The DC and AC Analysis of BJT Transistor Configurations

BJT transistor have a large usage area in electronics and related
discipline. So, I've written this source file which contain mostly
used BJT transistor calculations. 

Before using this source file, please, read these important notes. 
I hope, it will be usefull...

IMPORTANT NOTES:
----------------

1. If you see any result as -1.0, probably, the algorithms 
cannot calculate that result.
2. Don't give any voltage parameter as negative to algorithms.
If voltage source connect the inverse, the algorithm handle it.
3. All transistor configuratiions are set as 'npn' type. 
4. In ac analysis, algorithms use 're transistor' model.

EXISTING CONFIGURATIONS:
------------------------

+ dc_fixed_bias()
+ ac_fixed_bias()
+ dc_emitter_bias()
+ ac_emitter_bias()
+ dc_voltage_divider()
+ ac_voltage_divider()
+ dc_collector_feedback()
+ ac_collector_feedback()
+ ac_collector_dc_feedback()
+ dc_emitter_follower()
+ ac_emitter_follower()
+ dc_common_base()
+ ac_common_base()
+ dc_miscellaneous_bias()
+ two_port_system()
+ cascaded_system()
*/

// Libraries:
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

// General constants:
#define Vbe 0.7
#define MAX_TRANS 32

// User-defined string type:
typedef char * string;

// Result of DC analysis:
struct DCResults {
   double Ib; // base current
   double Ic; // collector current
   double Ie; // emitter current
   double Icsat; // collector saturation (max) current
   double Vce; // collector-emitter voltage
   double Vc; // collector voltage
   double Ve; // emitter voltage
   double Vb; // base current
   double Vbc; // base-collector voltage
};

// Results of AC analysis:
struct ACResults {
   double re; // re factor
   double Zi;  // input impedance
   double Zo; // output impedance
   double Av; // voltage gain
   string phase; // phase relationship
};

// Results of Two Port System:
struct TwoPortResults {
   double Avl; // load-voltage gain
   double Avs; // source-voltage gain
   double Ail; // load-current gain
};

// Results of Cascaded System:
struct CascadedResults {
   char names[MAX_TRANS][MAX_TRANS]; // stage names
   double voltages[MAX_TRANS]; // stage voltages
   double Avt; // total voltage gain
   double Avs; // source voltage gain
   double Ait; // total current gain
};

// User-defined analysis types:
typedef struct DCResults DCAnalysis;
typedef struct ACResults ACAnalysis;
typedef struct TwoPortResults TwoPortAnalysis;
typedef struct CascadedResults CascadedAnalysis;

/* --------------------------------------------------------------- */
/* ---------------------- Helper Definations --------------------- */
/* --------------------------------------------------------------- */

/* Get the Rth of both 'R1' and 'R2' resistors. */
double _Rth_(double R1, double R2) {
   // Rth is necesarry for voltage divider config.
   return 1 / (1 / R1 + 1 / R2); 
}

/* Get the Eth of both 'R1' and 'R2' resistors. */
double _Eth_(double Vcc, double R1, double R2) {
   // Eth is necesarry for voltage divider config.
   return Vcc * (R2 / (R1 + R2));
}

/* --------------------------------------------------------------- */
/* ------------------------- Display Results --------------------- */
/* --------------------------------------------------------------- */

/* Display the DC results of any transistor. */
void display_dc_results(DCAnalysis analysis) {
   // Display the DC analysis results.
   printf("Ib: %e A\n", analysis.Ib);
   printf("Ic: %e A\n", analysis.Ic);
   printf("Ie: %e A\n", analysis.Ie);
   printf("Ic(sat): %e A\n", analysis.Icsat);
   printf("Vce: %f V\n", analysis.Vce);
   printf("Vc: %f V\n", analysis.Vc);
   printf("Ve: %f V\n", analysis.Ve);
   printf("Vb: %f V\n", analysis.Vb);
   printf("Vbc: %f V\n", analysis.Vbc);
   printf("Vbe: %f V\n", Vbe);
}

/* Display the AC results of any transistor. */
void display_ac_results(ACAnalysis analysis) {
   // Display the DC analysis results.
   printf("re: %f ohm\n", analysis.re);
   printf("Zi: %f ohm\n", analysis.Zi);
   printf("Zo: %f ohm\n", analysis.Zo);
   printf("Av: %f\n", analysis.Av);
   printf("phase: %s\n", analysis.phase);
} 

/* Display the two port system results. */
void display_two_port_results(TwoPortAnalysis analysis) {
   // Display the results of two port systems.
   printf("Avl: %f\n", analysis.Avl);
   printf("Avs: %f\n", analysis.Avs);
   printf("Ail: %f\n", analysis.Ail);
}

/* --------------------------------------------------------------- */
/* ------------------------ Main Definations --------------------- */
/* --------------------------------------------------------------- */

/* DC analysis of fixed-bias transistor configuration. 

double Vcc=12, Rb=240000, Rc=2200, beta=50;
DCAnaysis analysis = dc_fixed_bias(Vcc, Rb, Rc, beta);
display_dc_results(analysis);

Ib: 4.708333e-05 A 
Ic: 2.354167e-03 A
Ie: 2.401250e-03 A
Ic(sat): 5.454545e-03 A
Vce: 6.820833 V
Vc: 6.820833 V
Ve: 0.000000 V
Vb: 0.700000 V
Vbc: -6.120833 V
Vbe: 0.700000 V
*/
DCAnalysis dc_fixed_bias(double Vcc, double Rb, double Rc, 
                         double beta) {
   // Check if parameters of transistor are consistent.
   assert (Rb > 0 && Rc > 0 && beta > 0);
   // Create DC analysis object.
   DCAnalysis analysis;
   // Calculate the all analyzes of transistor.
   analysis.Ib = (Vcc - Vbe) / Rb; 
   analysis.Ie = (beta + 1) * analysis.Ib; 
   analysis.Ic = beta * analysis.Ib; 
   analysis.Icsat = Vcc / Rc; 
   analysis.Vce = Vcc - (analysis.Ic * Rc); 
   analysis.Vc = analysis.Vce; 
   analysis.Ve = 0; 
   analysis.Vb = Vbe; 
   analysis.Vbc = analysis.Vb - analysis.Vc;

   return analysis;
}

/* AC analysis of fixed-bias transistor configuration. 

double Vcc=12, Rb=470000, Rc=3000, beta=100, ro=50000;
ACAnalysis analysis = ac_fixed_bias(Vcc, Rb, Rc, beta, ro);
display_ac_results(analysis);

re: 10.707088 ohm
Zi: 1068.275199 ohm
Zo: 2830.188679 ohm
Av: -264.328506
phase: Out of phase
*/
ACAnalysis ac_fixed_bias(double Vcc, double Rb, double Rc, 
                         double beta, double ro) {
   // Check if parameters of transistor are consistent.
   assert(Rb > 0 && Rc > 0 && beta > 0 && ro > 0);
   // Create AC analysis object.
   ACAnalysis analysis;
   // Calculate the all analyzes of transistor.
   double Ib = (Vcc - Vbe) / Rb; 
   double Ie = (beta + 1) * Ib; 
   analysis.re = 0.026 / Ie; 
   analysis.Zi = _Rth_(Rb, (beta * analysis.re)); 
   analysis.Zo = _Rth_(Rc, ro); 
   analysis.Av = -1 * _Rth_(Rc, ro) / analysis.re;
   analysis.phase = "Out of phase";

   return analysis;
}

/* DC analysis of emitter-bias transistor configuration. 

double Vcc=20, Rb=430000, Rc=2000, Re=1000, beta=50;
DCAnalysis analysis = dc_emitter_bias(Vcc, Rb, Rc, Re, beta);
display_dc_results(analysis);

Ib: 4.012474e-05 A
Ic: 2.006237e-03 A
Ie: 2.046362e-03 A
Ic(sat): 6.666667e-03 A
Vce: 13.981289 V
Vc: 16.027651 V
Ve: 2.046362 V
Vb: 2.746362 V
Vbc: -13.281289 V
Vbe: 0.700000 V
*/
DCAnalysis dc_emitter_bias(double Vcc, double Rb, double Rc, 
                           double Re, double beta) {
   // Check if parameters of transistor are consistent.
   assert (Rb > 0 && Rc > 0 && Re > 0 && beta > 0);
   // Create DC analysis object.
   DCAnalysis analysis;
   // Calculate the all analyzes of transistor.  
   analysis.Ib = (Vcc - Vbe) / (Rb + (beta + 1) * Re); 
   analysis.Ie = (beta + 1) * analysis.Ib;
   analysis.Ic = beta * analysis.Ib; 
   analysis.Icsat = Vcc / (Rc + Re); 
   analysis.Vce = Vcc - analysis.Ic * (Rc + Re); 
   analysis.Ve = analysis.Ie * Re; 
   analysis.Vc = analysis.Vce + analysis.Ve; 
   analysis.Vb = Vbe + analysis.Ve; 
   analysis.Vbc = analysis.Vb - analysis.Vc; 

   return analysis;
}

/* AC analysis of emitter-bias transistor configuration. 

double Vcc=20, Rb=470000, Rc=2200, Re=560, beta=120, ro=40000;
ACAnalysis analysis = ac_emitter_bias(Vcc, Rb, Rc, Re, beta, ro);
display_ac_results(analysis);

re: 5.987137 ohm
Zi: 56433.065511 ohm
Zo: 2197.743643 ohm
Av: -3.850258
phase: Out of phase
*/
ACAnalysis ac_emitter_bias(double Vcc, double Rb, double Rc, 
                           double Re, double beta, double ro) {
   // Check if parameters of transistor are consistent.
   assert (Rb > 0 && Rc > 0 && Re > 0 && beta > 0 && ro > 0);
   // Create AC analysis object.
   ACAnalysis analysis;
   // Calculate the all analyzes of transistor.
   double Ib = (Vcc - Vbe) / (Rb + (beta + 1) * Re);
   double Ie = (beta + 1) * Ib; 
   analysis.re = 0.026 / Ie; 
   double Zb1 = (beta + 1) + (Rc/ro);
   double Zb2 = 1 + (Rc + Re) / ro;
   double Zb = beta * analysis.re + (Zb1 / Zb2) * Re;
   analysis.Zi = _Rth_(Rb, Zb);
   double Zo1 = beta * (ro + analysis.re);
   double Zo2 = 1 + (beta * analysis.re) / Re;
   double Zo3 = ro + Zo1 / Zo2;
   analysis.Zo = _Rth_(Rc, Zo3);
   double Av1 = (-1 * (beta * Rc) / Zb) * 
                (1 + (analysis.re / ro)) + (Rc/ro);
   double Av2 = 1 + (Rc / ro);
   analysis.Av = Av1 / Av2; 
   analysis.phase = "Out of phase";

   return analysis;
}

/* DC analysis of voltage-divider transisfor configuration. 

double Vcc=22, Rb1=39000, Rb2=3900, Rc=10000, Re=1500, beta=100;
DCAnalysis analysis = dc_voltage_divider(Vcc, Rb1, Rb2, 
                                         Rc, Re, beta);
display_dc_results(analysis);

Ib: 8.384638e-06 A
Ic: 8.384638e-04 A
Ie: 8.468484e-04 A
Ic(sat): 1.913043e-03 A
Vce: 12.357666 V
Vc: 13.627939 V
Ve: 1.270273 V
Vb: 1.970273 V
Vbc: -11.657666 V
Vbe: 0.700000 V
*/
DCAnalysis dc_voltage_divider(double Vcc, double Rb1, double Rb2, 
                              double Rc, double Re, double beta) {
   // Check if parameters of transistor are consistent.
   assert (Rb1 > 0 && Rb2 > 0 && Rc > 0 && Re > 0 && beta > 0);
   // Create DC analysis object.
   DCAnalysis analysis;
   // Calculate the all analyzes of transistor.
   double rth = _Rth_(Rb1, Rb2); 
   double eth = _Eth_(Vcc, Rb1, Rb2); 
   analysis.Ib = (eth - Vbe) / (rth +(beta + 1)*Re); 
   analysis.Ie = (beta + 1) * analysis.Ib; 
   analysis.Ic = beta * analysis.Ib; 
   analysis.Icsat = Vcc / (Rc + Re); 
   analysis.Vce = Vcc - analysis.Ic * (Rc + Re); 
   analysis.Ve = analysis.Ie * Re;
   analysis.Vc = analysis.Vce + analysis.Ve;
   analysis.Vb = Vbe + analysis.Ve; 
   analysis.Vbc = analysis.Vb - analysis.Vc; 

   return analysis;
}

/* AC analysis of voltage-divider transistor configuration. 

double Vcc=16, Rb1=90000, Rb2=10000, Rc=2200;
double Re=680, beta=210, ro=50000; 
string bypass = "bypassed";
ACAnalysis analysis = ac_voltage_divider(Vcc, Rb1, Rb2, Rc, 
                                         Re, beta, ro, bypass);
display_ac_results(analysis);

re: 20.876672 ohm
Zi: 2948.043327 ohm
Zo: 2107.279693 ohm
Av: -100.939446
phase: Out of phase

double Vcc=16, Rb1=90000, Rb2=10000, Rc=2200;
double Re=680, beta=210, ro=50000; 
string bypass = "unbypassed";
ACAnalysis analysis = ac_voltage_divider(Vcc, Rb1, Rb2, Rc, 
                                         Re, beta, ro, bypass);
display_ac_results(analysis);

re: 20.876672 ohm
Zi: 8456.660156 ohm
Zo: 2196.691162 ohm
Av: -3.118332
phase: Out of phase
*/
ACAnalysis ac_voltage_divider(double Vcc, double Rb1, double Rb2, 
   double Rc, double Re, double beta, double ro, string bypass) {
   // Check if parameters of transistor are consistent.
   assert (Rb1 > 0 && Rb2 > 0 && Rc > 0 && Re > 0 && beta > 0);
   assert (bypass == "bypassed" || bypass == "unbypassed");

   // Indicate the all variables.
   double Zb1, Zb2, Zb, Zi, Zo1, Zo2, Zo3, Zo, Av1, Av2, Av;
   // Calculate the all analyzes of transistor.   
   double rth = _Rth_(Rb1, Rb2); 
   double eth = _Eth_(Vcc, Rb1, Rb2); 
   double Ib = (eth - Vbe) / (rth +(beta + 1) * Re); 
   double Ie = (beta + 1) * Ib;
   // Create AC analysis object.
   ACAnalysis analysis; 

   analysis.re = 0.026 / Ie; 
   // According to 'bypass' parameter, there are two options.
   if (bypass = "bypassed") {
      analysis.Zi = _Rth_(rth, (beta * analysis.re));
      analysis.Zo = _Rth_(Rc, ro);
      analysis.Av = -1 * _Rth_(Rc, ro) / analysis.re; 
   } 
   if (bypass == "unbypassed") {
      Zb1 = (beta + 1) + (Rc/ro);
      Zb2 = 1 + (Rc + Re) / ro;
      Zb = beta * analysis.re + (Zb1 / Zb2) * Re;
      analysis.Zi = _Rth_(rth, Zb);
      Zo1 = beta * (ro + analysis.re);
      Zo2 = 1 + (beta * analysis.re) / Re;
      Zo3 = ro + Zo1 / Zo2;
      analysis.Zo = _Rth_(Rc, Zo3);
      Av1 = (-1 * (beta * Rc) / Zb) * 
            (1 + (analysis.re / ro)) + (Rc / ro);
      Av2 = 1 + (Rc / ro);
      analysis.Av = Av1 / Av2; 
   }
   analysis.phase = "Out of phase";
   
   return analysis;
}

/* DC analysis of collector-feedback transistor configuration. 

double Vcc=10, Rf=250000, Rc=4700, Re=1200, beta=90;
DCAnalysis analysis = dc_collector_feedback(Vcc, Rf, Rc, Re, beta);
display_dc_results(analysis);

Ib: 1.190781e-05 A
Ic: 1.071703e-03 A
Ie: 1.083611e-03 A
Ic(sat): 1.694915e-03 A
Vce: 3.676953 V
Vc: 4.977286 V
Ve: 1.300333 V
Vb: 2.000333 V
Vbc: -2.976953 V
Vbe: 0.700000 V
*/
DCAnalysis dc_collector_feedback(double Vcc, double Rf, double Rc, 
                                 double Re, double beta) {  
   // Check if parameters of transistor are consistent.
   assert (Rf > 0 && Rc > 0 && Re > 0 && beta > 0);
   // Create DC analysis object.
   DCAnalysis analysis;
   // Calculate the all analyzes of transistor.
   analysis.Ib = (Vcc - Vbe) / (Rf + beta * (Rc+Re)); 
   analysis.Ie = (beta + 1) * analysis.Ib; 
   analysis.Ic = beta * analysis.Ib; 
   analysis.Icsat = Vcc / (Rc + Re); 
   analysis.Vce = Vcc - analysis.Ic * (Rc + Re); 
   analysis.Ve = analysis.Ie * Re;
   analysis.Vc = analysis.Vce + analysis.Ve; 
   analysis.Vb = Vbe + analysis.Ve;
   analysis.Vbc = analysis.Vb - analysis.Vc; 

   return analysis;
}

/* AC analysis of collector-feedback transistor configuration. 

double Vcc=9, Rf=180000, Rc=2700, beta=200, ro=1e+6;
ACAnalysis analysis = ac_collector_feedback(Vcc, Rf, Rc, beta, ro);
display_ac_results(analysis);

re: 11.221003 ohm
Zi: 565.582214 ohm
Zo: 2660.097900 ohm
Av: -237.064163
phase: Out of phase
*/
ACAnalysis ac_collector_feedback(double Vcc, double Rf, double Rc, 
                                 double beta, double ro) {
   // Check if parameters of transistor are consistent.
   assert (Rf > 0 && Rc > 0 && beta > 0 && ro > 0);
   // Create AC analysis object.
   ACAnalysis analysis; 
   // Calculate the all analyzes of transistor.
   double Ib = (Vcc - Vbe) / (Rf + beta * Rc); 
   double Ie = (beta + 1) * Ib; 
   analysis.re = 0.026 / Ie; 
   double Zi1 = 1 + (_Rth_(Rc, ro) / Rf);
   double Zi2 = (1 / (beta * analysis.re)) + (1 / Rf);
   double Zi3 = _Rth_(Rc, ro) / (beta * analysis.re * Rf);
   double Zi4 = _Rth_(Rc, ro) / (Rf * analysis.re);
   analysis.Zi = Zi1 / (Zi2 + Zi3 + Zi4); 
   analysis.Zo = 1 / (1/ro + 1/Rc + 1/Rf); 
   double Av1 = Rf / (_Rth_(Rc, ro) + Rf);
   double Av2 = _Rth_(Rc, ro) / analysis.re;
   analysis.Av = -1 * Av1 * Av2; 
   analysis.phase = "Out of phase";

   return analysis;
}

/* AC analysis of collector-dc-feedback transistor configuration. 

double Vcc=12, Rf1=120000, Rf2=68000, Rc=3000, beta=140, ro=30000;
ACAnalysis analysis = ac_collector_dc_feedback(Vcc, Rf1, Rf2, 
                                               Rc, beta, ro);
display_ac_results(analysis);

re: 9.921547 ohm
Zi: 1373.122437 ohm
Zo: 2622.107910 ohm
Av: -264.284210
phase: Out of phase
*/
ACAnalysis ac_collector_dc_feedback(double Vcc, double Rf1, 
               double Rf2, double Rc, double beta, double ro) {
   // Check if the parameters of transistor are consistent.
   assert (Rf1 > 0 && Rf2 > 0 && Rc > 0 && beta > 0 && ro > 0);
   // Create AC analysis object.
   ACAnalysis analysis; 
   // Calculate the all analyzes of transistor.
   double Ib = (Vcc - Vbe) / (Rf1+Rf2 + (beta * Rc));
   double Ie = (beta + 1) * Ib; 
   analysis.re = 0.026 / Ie; 
   analysis.Zi = _Rth_(Rf1, (beta * analysis.re));
   analysis.Zo = 1 / (1 / Rc + 1 / Rf2 + 1 / ro); 
   analysis.Av = -1 * analysis.Zo / analysis.re; 
   analysis.phase = "Out of phase";

   return analysis;
}

/* DC analysis of emitter-follower transisfor configuration. 

double Vee=20, Rb=240000, Re=2000, beta=90;
DCAnalysis analysis = dc_emitter_follower(Vee, Rb, Re, beta);
display_dc_results(analysis);

Ib: 4.573460e-05 A
Ic: 4.116114e-03 A
Ie: 4.161848e-03 A
Ic(sat): -1.000000e+00 A (not calculated)
Vce: 11.676303 V
Vc: 40.000000 V
Ve: 28.323696 V
Vb: 29.023697 V
Vbc: -10.976303 V
Vbe: 0.700000 V
*/
DCAnalysis dc_emitter_follower(double Vee, double Rb, double Re, 
                               double beta) {
   // Check if the parameters of transistor are consistent.
   assert (Rb > 0 && Re > 0 && beta > 0);
   // Create DC analysis object.
   DCAnalysis analysis; 
   // Calculate the all analyzes of transistor.
   analysis.Ib = (Vee - Vbe) / (Rb + (beta + 1) * Re);
   analysis.Ie = (beta + 1) * analysis.Ib;
   analysis.Ic = beta * analysis.Ib; 
   analysis.Icsat = -1.0;
   analysis.Vce = Vee - (analysis.Ie * Re); 
   analysis.Ve = (analysis.Ie * Re) + Vee; 
   analysis.Vc = analysis.Vce + analysis.Ve; 
   analysis.Vb = Vbe + analysis.Ve; 
   analysis.Vbc = analysis.Vb - analysis.Vc; 

   return analysis;
}

/* AC analysis of emitter-follower transistor configuration. 

double Vcc=12, Rb=220000, Re=3300, beta=100, ro=1e+6;
ACAnalysis analysis = ac_emitter_follower(Vcc, Rb, Re, beta, ro);
display_ac_results(analysis);

re: 12.604749 ohm
Zi: 132550.803154 ohm
Zo: 12.432776 ohm
Av: 0.996220
phase: In phase
*/
ACAnalysis ac_emitter_follower(double Vcc, double Rb, double Re, 
                               double beta, double ro) {
   // Check if the parameters of transistor are consistent.
   assert (Rb > 0 && Re > 0 && beta > 0 && ro > 0);
   // Create AC analysis object.
   ACAnalysis analysis; 
   // Calculate the all analyzes of transistor.
   double Ib = (Vcc - Vbe) / (Rb + (beta + 1) * Re);
   double Ie = (beta + 1) * Ib; 
   analysis.re = 0.026 / Ie; 
   double Zb1 = (beta + 1) * Re;
   double Zb2 = 1 + (Re / ro);
   double Zb = (beta * analysis.re) + (Zb1 / Zb2);
   analysis.Zi = 1 / (1 /Rb + 1 /Zb); 
   double Zo1 = (beta * analysis.re) / (beta + 1);
   analysis.Zo = 1 / (1 /ro + 1 /Re + 1 /Zo1); 
   double Av1 = (beta + 1) * Re / Zb;
   analysis.Av = Av1 / (1 + (Re/ro)); 
   analysis.phase = "In phase";

   return analysis;
}

/* DC analysis of common-base transisfor configuration. 

double Vcc=10, Vee=4, Rc=2400, Re=1200, beta=60;
DCAnalysis analysis = dc_common_base(Vcc, Vee, Rc, Re, beta);
display_dc_results(analysis);

Ib: 4.508197e-05 A
Ic: 2.704918e-03 A
Ie: 2.750000e-03 A
Ic(sat): -1.000000e+00 A (not calculated)
Vce: 4.100000 V
Vc: -1.000000 V (not calculated)
Ve: -1.000000 V (not calculated)
Vb: -1.000000 V (not calculated)
Vbc: -3.508196 V
Vbe: 0.700000 V
*/
DCAnalysis dc_common_base(double Vcc, double Vee, double Rc, 
                          double Re, double beta) {
   // Check if the parameters of transistor are consistent.
   assert (Rc > 0 && Re > 0 && beta > 0);
   // Create DC analysis object.
   DCAnalysis analysis; 
   // Calculate the all analyzes of transistor.
   analysis.Ie = (Vee - Vbe) / Re; 
   analysis.Ib = analysis.Ie / (beta + 1); 
   analysis.Ic = analysis.Ib * beta; 
   analysis.Icsat = -1.0; 
   analysis.Vce = Vee + Vcc - analysis.Ie * (Rc + Re);
   analysis.Vc = -1.0;
   analysis.Ve = -1.0;
   analysis.Vb = -1.0;
   double Vcb = Vcc - analysis.Ic * Rc; 
   analysis.Vbc = -1 * Vcb; 

   return analysis;
}

/* AC analysis of common-base transistor configuration. 

double Vcc=8, Vee=2, Rc=5000, Re=1000, alpha=0.98;
ACAnalysis analysis = ac_common_base(Vcc, Vee, Rc, Re, alpha);
display_ac_results(analysis);

re: 20.000000 ohm
Zi: 19.607843 ohm
Zo: 5000.000000 ohm
Av: 245.000000
phase: In phase
*/
ACAnalysis ac_common_base(double Vcc, double Vee, double Rc, 
                          double Re, double alpha) {
   // Check if the parameters of transistor are consistent.
   assert (Rc > 0 && Re > 0 && alpha > 0);
   // Create AC analysis object.
   ACAnalysis analysis; 
   // Calculate the all analyzes of transistor.
   double Ie = (Vee - Vbe) / Re; 
   analysis.re = 0.026 / Ie; 
   analysis.Zi = _Rth_(Re, analysis.re);
   analysis.Zo = Rc; 
   analysis.Av = alpha * Rc / analysis.re;
   analysis.phase = "In phase"; 

   return analysis;
}

/* DC analysis of miscellaneous-bias transisfor configuration. 

double Vcc=20, Rb=680000, Rc=4700, beta=120;
DCAnalysis analysis = dc_miscellaneous_bias(Vcc, Rb, Rc, beta);
display_dc_results(analysis);

Ib: 1.551447e-05 A
Ic: 1.861736e-03 A
Ie: 1.877251e-03 A
Ic(sat): -1.000000e+00 A (not calculated)
Vce: 11.176921 V
Vc: 11.176921 V
Ve: 0.000000 V
Vb: 0.700000 V
Vbc: -10.476921 V
Vbe: 0.700000 V
*/
DCAnalysis dc_miscellaneous_bias(double Vcc, double Rb, double Rc, 
                                 double beta) {
   // Check if the parameters of transistor are consistent.
   assert (Rb > 0 && Rc > 0 && beta > 0);
   // Create DC analysis object.
   DCAnalysis analysis; 
   // Calculate the all analyzes of transistor.
   analysis.Ib = (Vcc - Vbe) / (Rb + beta * Rc); 
   analysis.Ic = beta * analysis.Ib; 
   analysis.Ie = (beta + 1) * analysis.Ib;
   analysis.Icsat = -1.0; 
   analysis.Vce = Vcc - (analysis.Ie * Rc); 
   analysis.Ve = 0; 
   analysis.Vc = analysis.Vce + analysis.Ve;
   analysis.Vb = Vbe + analysis.Ve;
   analysis.Vbc = analysis.Vb - analysis.Vc; 

   return analysis;
}

/* Two Port System Analysis 

double Avnl=-480, Zi=4000, Zo=2000, Rs=200, Rl=5600;
TwoPortAnalysis analysis = two_port_system(Avnl, Zi, Zo, Rs, Rl);
display_two_port_results(analysis);

Avl: -353.684204    // load-voltage gain
Avs: -336.842102    // source-voltage gain
Ail: 252.631577     // load-current gain
*/
TwoPortAnalysis two_port_system(double Avnl, double Zi, double Zo, 
                                double Rs, double Rl) {
   // Check if the parameters of two port system are consistent.
   assert (Zi > 0 && Zo > 0 && Rs > 0 && Rl > 0);
   // Create two port system object.
   TwoPortAnalysis analysis; 
   // Calculate the all analyzes of two port system.
   analysis.Avl = Rl / (Rl + Zo) * Avnl; 
   analysis.Avs = Zi / (Zi + Rs) * analysis.Avl; 
   analysis.Ail = -1 * analysis.Avl * Zi / Rl; 

   return analysis;
}

/* Cascaded System Analysis 

double Avnls[3] = {1, 250, 100}; // not-load voltage gains
double Zis[3] = {500, 26, 100}; // input impedances
double Zos[3] = {1000, 5100, 100}; // output impedances
double Rs = 10000, Rl = 820; // source and load resistances
CascadedAnalysis analysis = cascaded_system(3, Avnls, Zis, 
                                            Zos, Rs, Rl); 

(You can display the each result manually.)
Av1: 0.025341
Av2: 4.807692
Av3: 89.130435
Avt: 10.858971
Avs: 0.517094
Ait: -6.621324
*/
CascadedAnalysis cascaded_system(size_t num, double Avnls[num], 
      double Zis[num], double Zos[num], double Rs, double Rl){
   // Check if the parameters of cascaded system are consistent.
   for (int i=0; i<num; i++)
      assert (Zis[i] > 0 && Zos[i] > 0 && Rs > 0 && Rl > 0);
   // Create cascaded system object.
   CascadedAnalysis analysis; 
   // Declare the voltage variables.
   double tvoltage = 1, Av;
   // Declare the stage names.
   char iname[MAX_TRANS], vname[MAX_TRANS] = "Av";

   // Calculate the all analyzes of cascaded system.
   for (int i=0; i<num; i++) {

      if (i != num - 1) {
         // Calculate the stage voltages and total voltage.
         Av = Zis[i+1] / (Zis[i+1] + Zos[i]) * Avnls[i];
         tvoltage *= Av;
         // Right know, create the stage names.
         sprintf(iname, "%d", (i + 1)); strcat(vname, iname);
         // Append the stage voltage names and gain to struct.
         strcpy(analysis.names[i], vname);
         analysis.voltages[i] = Av;
      }
      // Calculate the last stage of cascaded system.
      else {
         // Calculate the last stage voltage.
         Av = Rl / (Rl + Zos[num - 1]) * Avnls[num - 1];
         tvoltage *= Av;
         // Right know, create the last stage names.
         sprintf(iname, "%d", (i + 1)); strcat(vname, iname);
         // Append the last stage voltage names and gain to it.
         strcpy(analysis.names[num - 1], vname);
         analysis.voltages[num - 1] = Av;
      }
      // Calculate the other parameters to struct.
      analysis.Avt = tvoltage;
      analysis.Avs = Zis[0] / (Zis[0] + Rs) * tvoltage;
      analysis.Ait = -1 * tvoltage * Zis[0] / Rl;

      // Clear the 'iname' and 'vname' variables.
      strcpy(iname, ""); strcpy(vname, "Av");
   }
   return analysis;
}
