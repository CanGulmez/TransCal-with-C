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

// Libraries
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>
#include <string.h>

// General constants
#define Vbe 0.7

// Result of DC analysis
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
} DCAnalysis;

// Results of AC analysis
struct ACResults {
   double re; // re factor
   double Zi;  // input impedance
   double Zo; // output impedance
   double Av; // voltage gain
   char* phase; // phase relationship
} ACAnalysis;

// Results of Two Port System
struct TwoPortResults {
   double Avl; // load-voltage gain
   double Avs; // source-voltage gain
   double Ail; // load-current gain
} TwoPortAnalysis;

// Results of Cascaded System
struct CascadedResults {
   char *stage_names[64]; // stage names
   double stage_volts[64]; // stage voltages
   double Avt; // total voltage gain
   double Avs; // source voltage gain
   double Ait; // total current gain
} CascadedAnalysis;

/* --------------------------------------------------------------- */
/* ---------------------- Helper Definations --------------------- */
/* --------------------------------------------------------------- */

/* Get the Rth of both 'R1' and 'R2' resistors. */
double Rth(double R1, double R2) {
   // Rth is necesarry for voltage divider config.
   return 1 / (1 / R1 + 1 / R2); 
}

/* Get the Eth of both 'R1' and 'R2' resistors. */
double Eth(double Vcc, double R1, double R2) {
   // Eth is necesarry for voltage divider config.
   return Vcc * (R2 / (R1 + R2));
}

/* Save the DC results of any kind of transistor config. */
void save_dc_results(double Ib, double Ic, double Ie, double Icsat,
                     double Vce, double Vc, double Ve, double Vb, 
                     double Vbc) {
   // Save the DC results into the 'DCAnalysis' struct.
   DCAnalysis.Ib = Ib; // base current
   DCAnalysis.Ic = Ic; // collector current
   DCAnalysis.Ie = Ie; // emitter current
   DCAnalysis.Icsat = Icsat; // collector saturation (max) current
   DCAnalysis.Vce = Vce; // collector-emitter voltage
   DCAnalysis.Vc = Vc; // collector voltage
   DCAnalysis.Ve = Ve; // emitter voltage
   DCAnalysis.Vb = Vb; // base voltage
   DCAnalysis.Vbc = Vbc; // base-collector voltage
}

/* Save the AC results of any kind of transistor config. */
void save_ac_results(double re, double Zi, double Zo, double Av,
                     char* phase) {
   // Save the AC results into the 'ACAnalysis' struct.
   ACAnalysis.re = re; // re factor
   ACAnalysis.Zi = Zi; // input impedance
   ACAnalysis.Zo = Zo; // output impedance
   ACAnalysis.Av = Av; // voltage gain
   ACAnalysis.phase = phase; // phase relationships
}

/* Save the two port system results. */
void save_two_port_results(double Avl, double Avs, double Ail) {
   // Save the results of the two port systems.
   TwoPortAnalysis.Avl = Avl; // load-voltage gain
   TwoPortAnalysis.Avs = Avs; // source-voltage gain
   TwoPortAnalysis.Ail = Ail; // load-current gain
}

/* Display the DC results of any transistor. */
void display_dc_results(void) {
   // Display the DC analysis results.
   printf("Ib: %e A\n", DCAnalysis.Ib);
   printf("Ic: %e A\n", DCAnalysis.Ic);
   printf("Ie: %e A\n", DCAnalysis.Ie);
   printf("Ic(sat): %e A\n", DCAnalysis.Icsat);
   printf("Vce: %f V\n", DCAnalysis.Vce);
   printf("Vc: %f V\n", DCAnalysis.Vc);
   printf("Ve: %f V\n", DCAnalysis.Ve);
   printf("Vb: %f V\n", DCAnalysis.Vb);
   printf("Vbc: %f V\n", DCAnalysis.Vbc);
   printf("Vbe: %f V\n", Vbe);
}

/* Display the AC results of any transistor. */
void display_ac_results(void) {
   // Display the DC analysis results.
   printf("re: %f ohm\n", ACAnalysis.re);
   printf("Zi: %f ohm\n", ACAnalysis.Zi);
   printf("Zo: %f ohm\n", ACAnalysis.Zo);
   printf("Av: %f\n", ACAnalysis.Av);
   printf("phase: %s\n", ACAnalysis.phase);
}

/* Display the two port system results. */
void display_two_port_results(void) {
   // Display the results of two port systems.
   printf("Avl: %f\n", TwoPortAnalysis.Avl);
   printf("Avs: %f\n", TwoPortAnalysis.Avs);
   printf("Ail: %f\n", TwoPortAnalysis.Ail);
}

/* --------------------------------------------------------------- */
/* ------------------------ Main Definations --------------------- */
/* --------------------------------------------------------------- */

/* DC analysis of fixed-bias transistor configuration. 

'Vcc', 'Rb', 'Rc' parameters stand for collector voltage, base
resistance and collector resistance. 'beta' patameter is the ratio 
of transistor's collector current to its base current. All resistor
values must be in form of 'ohm'. For example:

double Vcc=12, Rb=240000, Rc=2200, beta=50;
dc_fixed_bias(Vcc, Rb, Rc, beta);
display_dc_results();

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
void dc_fixed_bias(double Vcc, double Rb, double Rc, double beta){
   // Check if parameters of transistor are consistent.
   assert (Rb > 0 && Rc > 0 && beta > 0);
   
   // Calculate the all analyzes of transistor.
   double Ib = (Vcc - Vbe) / Rb; // base current
   double Ie = (beta + 1) * Ib; // emitter current
   double Ic = beta * Ib; // collector current
   double Icsat = Vcc / Rc; // collector saturation current
   double Vce = Vcc - (Ic * Rc); // collector-emitter voltage
   double Vc = Vce; // collector voltage
   double Ve = 0; // emitter voltage
   double Vb = Vbe; // base voltage
   double Vbc = Vb - Vc; // base-collector voltage

   // Save and display the dc results of transistor.
   save_dc_results(Ib, Ic, Ie, Icsat, Vce, Vc, Ve, Vb, Vbc);
}

/* AC analysis of fixed-bias transistor configuration. 

'Vcc', 'Rb', 'Rc' parameters stand for collector voltage, base
resistance and collector resistance. 'beta' patameter is the ratio 
of transistor's collector current to its base current. 'ro' is the 
output impedance of transistor. All resistor values must be in 
form of 'ohm'. For example:

double Vcc=12, Rb=470000, Rc=3000, beta=100, ro=50000;
ac_fixed_bias(Vcc, Rb, Rc, beta, ro);
display_ac_results();

re: 10.707088 ohm
Zi: 1068.275146 ohm
Zo: 2830.188721 ohm
Av: -264.328491
phase: Out of phase
*/
void ac_fixed_bias(double Vcc, double Rb, double Rc, double beta, 
                   double ro) {
   // Check if parameters of transistor are consistent.
   assert(Rb > 0 && Rc > 0 && beta > 0 && ro > 0);

   // Calculate the all analyzes of transistor.
   double Ib = (Vcc - Vbe) / Rb; // base current
   double Ie = (beta + 1) * Ib; // emitter current
   double re = 0.026 / Ie; // re factor
   double Zi = 1 / (1/Rb + 1/(beta * re)); // input impedance
   double Zo = 1 / (1/Rc + 1/ro); // output impedance
   double Av = -1 * (1 / (1/Rc + 1/ro)) / re; // voltage gain

   // Save and display the ac results of transistor.
   save_ac_results(re, Zi, Zo, Av, "Out of phase");
}

/* DC analysis of emitter-bias transistor configuration. 

'Vcc', 'Rb', 'Rc', 'Re' parameters stand for collector voltage, 
base resistance, collector resistance and emitter resistance. 
'beta' parameter is the ratio of the transistor's collector 
current to its base current. All resistor values must be in form 
of 'ohm'. For example:

double Vcc=20, Rb=430000, Rc=2000, Re=1000, beta=50;
dc_emitter_bias(Vcc, Rb, Rc, Re, beta);
display_dc_results();

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
void dc_emitter_bias(double Vcc, double Rb, double Rc, double Re, 
                     double beta) {
   // Check if parameters of transistor are consistent.
   assert (Rb > 0 && Rc > 0 && Re > 0 && beta > 0);

   // Calculate the all analyzes of transistor.  
   double Ib = (Vcc - Vbe) / (Rb + (beta + 1) * Re); // base current
   double Ie = (beta + 1) * Ib; // emitter current
   double Ic = beta * Ib; // collector current
   double Icsat = Vcc / (Rc + Re); // collector saturation current
   double Vce = Vcc - Ic * (Rc + Re); // collector-emitter voltage
   double Ve = Ie * Re; // emitter voltage
   double Vc = Vce + Ve; // collector voltage
   double Vb = Vbe + Ve; // base voltage
   double Vbc = Vb - Vc; // base-collector voltage

   // Save and display the dc results of transistor.
   save_dc_results(Ib, Ic, Ie, Icsat, Vce, Vc, Ve, Vb, Vbc);
}

/* AC analysis of emitter-bias transistor configuration. 

'Vcc', 'Rb', 'Rc', 'Re' parameters stand for collector voltage, 
base resistance, collector resistance and emitter resistance. 'beta'
parameter is the ratio of the transistor's collector current to its 
base current. 'ro' parameter is the output impedance of the 
transistor. All resistor values must be in form of 'ohm'. For 
example:

double Vcc=20, Rb=470000, Rc=2200, Re=560, beta=120, ro=40000;
ac_emitter_bias(Vcc, Rb, Rc, Re, beta, ro);
display_ac_results();

re: 5.987136 ohm
Zi: 56433.066406 ohm
Zo: 2197.743652 ohm
Av: -3.850258
phase: Out of phase
*/
void ac_emitter_bias(double Vcc, double Rb, double Rc, double Re, 
                     double beta, double ro) {
   // Check if parameters of transistor are consistent.
   assert (Rb > 0 && Rc > 0 && Re > 0 && beta > 0 && ro > 0);

   // Calculate the all analyzes of transistor.
   double Ib = (Vcc - Vbe) / (Rb + (beta + 1) * Re);// base current
   double Ie = (beta + 1) * Ib; // emitter current
   double re = 0.026 / Ie; // re factor
   double Zb1 = (beta + 1) + (Rc/ro);
   double Zb2 = 1 + (Rc + Re) / ro;
   double Zb = beta * re + (Zb1 / Zb2) * Re;
   double Zi = 1 / (1/Rb + 1/Zb); // input impedance
   double Zo1 = beta * (ro + re);
   double Zo2 = 1 + (beta * re) / Re;
   double Zo3 = ro + Zo1 / Zo2;
   double Zo = 1 / (1/Rc + 1/Zo3); // output impedance
   double Av1 = (-1 * (beta * Rc) / Zb) * (1 + (re/ro)) + (Rc/ro);
   double Av2 = 1 + (Rc / ro);
   double Av = Av1 / Av2; // voltage gain

   // Save and display the ac results of transistor.
   save_ac_results(re, Zi, Zo, Av, "Out of phase");
}

/* DC analysis of voltage-divider transisfor configuration. 

'Vcc', 'Rb1', 'Rb2', 'Rc', 'Re' parameters stand for collector
voltage, upper base resistance, lower base resistance, collector
resistance and emitter resistance. 'beta' parameter is the ratio of
transistor's collector current to its base current. All resistor
values must be in form of 'ohm'. For example:

double Vcc=22, Rb1=39000, Rb2=3900, Rc=10000, Re=1500, beta=100;
dc_voltage_divider(Vcc, Rb1, Rb2, Rc, Re, beta);
display_dc_results();

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
void dc_voltage_divider(double Vcc, double Rb1, double Rb2, 
                        double Rc, double Re, double beta) {
   // Check if parameters of transistor are consistent.
   assert (Rb1 > 0 && Rb2 > 0 && Rc > 0 && Re > 0 && beta > 0);

   // Calculate the all analyzes of transistor.
   double rth = Rth(Rb1, Rb2); // thevenin resistor
   double eth = Eth(Vcc, Rb1, Rb2); // thevenin voltage
   double Ib = (eth - Vbe) / (rth +(beta + 1)*Re); // base current
   double Ie = (beta + 1) * Ib; // emitter current
   double Ic = beta * Ib; // collector current
   double Icsat = Vcc / (Rc + Re); // collector saturation current
   double Vce = Vcc - Ic * (Rc + Re); // collector-emitter current
   double Ve = Ie * Re; // emitter voltage
   double Vc = Vce + Ve; // collector voltage
   double Vb = Vbe + Ve; // base voltage
   double Vbc = Vb - Vc; // base-collector voltage

   // Save and display the dc results of transistor.
   save_dc_results(Ib, Ic, Ie, Icsat, Vce, Vc, Ve, Vb, Vbc);
}

/* AC analysis of voltage-divider transistor configuration. 

'Vcc', 'Rb1', 'Rb2', 'Rc', 'Re' parameters stand for collector
voltage, upper base resistance, lower base resistance, collector
resistance and emitter resistance. 'beta' parameter is the ratio 
of transistor's collector current to its base current. 'ro' 
parameter is the output impedance of the transistor. 'bypass' 
parameter must be  'bypassed' or 'unbypassed'. That define the 
bypass status of the emitter terminal. All resistor values must 
be in form of 'ohm'. For example:

double Vcc=16, Rb1=90000, Rb2=10000, Rc=2200;
double Re=680, beta=210, ro=50000; 
char* bypass = "bypassed";
ac_voltage_divider(Vcc, Rb1, Rb2, Rc, Re, beta, ro, bypass);
display_ac_results();

re: 20.876673 ohm
Zi: 2948.043213 ohm
Zo: 2107.279785 ohm
Av: -100.939445
phase: Out of phase

double Vcc=16, Rb1=90000, Rb2=10000, Rc=2200;
double Re=680, beta=210, ro=50000; 
char* bypass = "unbypassed";
ac_voltage_divider(Vcc, Rb1, Rb2, Rc, Re, beta, ro, bypass);
display_ac_results();

re: 20.876673 ohm
Zi: 8456.660156 ohm
Zo: 2196.691162 ohm
Av: -3.118332
phase: Out of phase
*/
void ac_voltage_divider(double Vcc, double Rb1, double Rb2, 
                        double Rc, double Re, double beta, 
                        double ro, char* bypass) {
   // Check if parameters of transistor are consistent.
   assert (Rb1 > 0 && Rb2 > 0 && Rc > 0 && Re > 0 && beta > 0);
   assert (!strcmp(bypass, "bypassed") || 
           !strcmp(bypass, "unbypassed"));

   // Indicate the all variables.
   double Zb1, Zb2, Zb, Zi, Zo1, Zo2, Zo3, Zo, Av1, Av2, Av;
   // Calculate the all analyzes of transistor.   
   double rth = Rth(Rb1, Rb2); // thevenin resistor 
   double eth = Eth(Vcc, Rb1, Rb2); // thevenin voltage
   double Ib = (eth - Vbe) / (rth +(beta + 1) * Re); // base current
   double Ie = (beta + 1) * Ib; // emitter current
   double re = 0.026 / Ie; // re factor

   // According to 'bypass' parameter, there are two options.
   if (strcmp(bypass, "bypassed") == 0) {
      Zi = 1 / (1 / rth + 1 / (beta * re)); // input impedance
      Zo = 1 / (1 / Rc + 1 / ro); // output impedance
      Av = -1 * Rth(Rc, ro) / re; // voltage gain
   } 
   if (strcmp(bypass, "unbypassed") == 0) {
      Zb1 = (beta + 1) + (Rc/ro);
      Zb2 = 1 + (Rc + Re) / ro;
      Zb = beta * re + (Zb1 / Zb2) * Re;
      Zi = 1 / (1/rth + 1/Zb); // input impedance
      Zo1 = beta * (ro + re);
      Zo2 = 1 + (beta * re) / Re;
      Zo3 = ro + Zo1 / Zo2;
      Zo = 1 / (1/Rc + 1/Zo3); // output impedance
      Av1 = (-1 * (beta * Rc) / Zb) * (1 + (re/ro)) + (Rc/ro);
      Av2 = 1 + (Rc / ro);
      Av = Av1 / Av2; // voltage gain
   }
   // Save and display the ac results of transistor.
   save_ac_results(re, Zi, Zo, Av, "Out of phase");
}

/* DC analysis of collector-feedback transistor configuration. 

'Vcc', 'Rf', 'Rc', 'Re' parameters stand for collector voltage, 
base  resistance, collector resistance and emitter resistance. 
'beta' parameter is the ratio of transistor's collector current 
to its base current. All resistor values must be in form of 'ohm'.
For example:

double Vcc=10, Rf=250000, Rc=4700, Re=1200, beta=90;
dc_collector_feedback(Vcc, Rf, Rc, Re, beta);
display_dc_results();

Ib: 1.190781e-05 A
Ic: 1.071703e-03 A
Ie: 1.083611e-03 A
Ic(sat): 1.694915e-03 A
Vce: 3.676953 V
Vc: 4.977285 V
Ve: 1.300333 V
Vb: 2.000333 V
Vbc: -2.976953 V
Vbe: 0.700000 V
*/
void dc_collector_feedback(double Vcc, double Rf, double Rc, 
                           double Re, double beta) {  
   // Check if parameters of transistor are consistent.
   assert (Rf > 0 && Rc > 0 && Re > 0 && beta > 0);

   // Calculate the all analyzes of transistor.
   double Ib = (Vcc - Vbe) / (Rf + beta * (Rc+Re)); // base current
   double Ie = (beta + 1) * Ib; // emitter current
   double Ic = beta * Ib; // collector current
   double Icsat = Vcc / (Rc + Re); // collector saturation current
   double Vce = Vcc - Ic * (Rc + Re); // collector-emitter voltage
   double Ve = Ie * Re; // emitter voltage
   double Vc = Vce + Ve; // collector voltage
   double Vb = Vbe + Ve; // base voltage
   double Vbc = Vb - Vc; // base-collector voltage

   // Save and display the dc results of transistor.
   save_dc_results(Ib, Ic, Ie, Icsat, Vce, Vc, Ve, Vb, Vbc);
}

/* AC analysis of collector-feedback transistor configuration. 

'Vcc', 'Rf', 'Rc' parameters stand for collector voltage, base 
resistance and collector resistance. 'beta' parameter is the ratio 
of the transistor's collector current to its base current. 'ro' 
parameter is the output impedance of transistor. All resistor values
must be in form of 'ohm'. For example:

double Vcc=9, Rf=180000, Rc=2700, beta=200, ro=1e+6;
ac_collector_feedback(Vcc, Rf, Rc, beta, ro);
display_ac_results();

re: 11.221004 ohm
Zi: 565.582214 ohm
Zo: 2660.097900 ohm
Av: -237.064163
phase: Out of phase
*/
void ac_collector_feedback(double Vcc, double Rf, double Rc, 
                           double beta, double ro) {
   // Check if parameters of transistor are consistent.
   assert (Rf > 0 && Rc > 0 && beta > 0 && ro > 0);

   // Calculate the all analyzes of transistor.
   double Ib = (Vcc - Vbe) / (Rf + beta * Rc); // base current
   double Ie = (beta + 1) * Ib; // emitter current
   double re = 0.026 / Ie; // re factor
   double Zi1 = 1 + (Rth(Rc, ro) / Rf);
   double Zi2 = (1 / (beta * re)) + (1 / Rf);
   double Zi3 = Rth(Rc, ro) / (beta * re * Rf);
   double Zi4 = Rth(Rc, ro) / (Rf * re);
   double Zi = Zi1 / (Zi2 + Zi3 + Zi4); // input impedance
   double Zo = 1 / (1/ro + 1/Rc + 1/Rf); // output impedance
   double Av1 = Rf / (Rth(Rc, ro) + Rf);
   double Av2 = Rth(Rc, ro) / re;
   double Av = -1 * Av1 * Av2; // voltage gain

   // Save and display the ac results of transistor.
   save_ac_results(re, Zi, Zo, Av, "Out of phase");
}

/* AC analysis of collector-dc-feedback transistor configuration. 

'Vcc', 'Rf1', 'Rf2', 'Rc' parameters stand for collector voltage, 
first base resistance, second base resistance and collecor 
resistance. 'beta' parameter is the ratio of the transistor's 
collector current to its base current. 'ro' parameter is the output
impedance of the transistor. All resistors must be in form of 'ohm'.
For example:

double Vcc=12, Rf1=120000, Rf2=68000, Rc=3000, beta=140, ro=30000;
ac_collector_dc_feedback(Vcc, Rf1, Rf2, Rc, beta, ro);
display_ac_results();

re: 9.921547 ohm
Zi: 1373.122437 ohm
Zo: 2622.107910 ohm
Av: -264.284210
phase: Out of phase
*/
void ac_collector_dc_feedback(double Vcc, double Rf1, double Rf2, 
                              double Rc, double beta, double ro) {
   // Check if the parameters of transistor are consistent.
   assert (Rf1 > 0 && Rf2 > 0 && Rc > 0 && beta > 0 && ro > 0);

   // Calculate the all analyzes of transistor.
   double Ib = (Vcc - Vbe) / (Rf1+Rf2 + (beta * Rc)); // base current
   double Ie = (beta + 1) * Ib; // emitter current
   double re = 0.026 / Ie; // re factor
   double Zi = 1 / (1/Rf1 + 1/(beta * re)); // input impedance
   double Zo = 1 / (1/Rc + 1/Rf2 + 1/ro); // output impedance
   double Av = -1 * Zo / re; // voltage gain

   // Save and display the ac results of transistor.
   save_ac_results(re, Zi, Zo, Av, "Out of phase");
}

/* DC analysis of emitter-follower transisfor configuration. 

'Vee', 'Rb, 'Re' parameters stand for emitter voltage, base 
resistance and emitter resistance. 'beta' parameter is the ratio 
of the transistor's collector current to its base current. All 
resistor values must be in form of 'ohm'. For example:

double Vee=20, Rb=240000, Re=2000, beta=90;
dc_emitter_follower(Vee, Rb, Re, beta);
display_dc_results();

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
void dc_emitter_follower(double Vee, double Rb, double Re, 
                         double beta) {
   // Check if the parameters of transistor are consistent.
   assert (Rb > 0 && Re > 0 && beta > 0);

   // Calculate the all analyzes of transistor.
   double Ib = (Vee - Vbe) / (Rb + (beta + 1) * Re);// base current
   double Ie = (beta + 1) * Ib; // emitter current
   double Ic = beta * Ib; // collector current
   double Vce = Vee - (Ie * Re); // collector-emitter voltage
   double Ve = (Ie * Re) + Vee; // emitter voltage
   double Vc = Vce + Ve; // collector voltage
   double Vb = Vbe + Ve; // base voltage
   double Vbc = Vb - Vc; // base-collector voltage

   // Save and display the dc results of transistor.
   save_dc_results(Ib, Ic, Ie, -1.0, Vce, Vc, Ve, Vb, Vbc);
}

/* AC analysis of emitter-follower transistor configuration. 

'Vcc', 'Rb', 'Re' parameters stand for collector voltage, base
resistance and emitter resistance in order. 'beta' parameter is 
the ratio of the transistor's current to its base current. 'ro' 
parameter is the output resistor of the transistor. All resistor 
values must  be in form of 'ohm'. For example:

double Vcc=12, Rb=220000, Re=3300, beta=100, ro=1e+6;
ac_emitter_follower(Vcc, Rb, Re, beta, ro);
display_ac_results();

re: 12.604749 ohm
Zi: 132550.803154 ohm
Zo: 12.432776 ohm
Av: 0.996220
phase: In phase
*/
void ac_emitter_follower(double Vcc, double Rb, double Re, 
                         double beta, double ro) {
   // Check if the parameters of transistor are consistent.
   assert (Rb > 0 && Re > 0 && beta > 0 && ro > 0);

   // Calculate the all analyzes of transistor.
   double Ib = (Vcc - Vbe) / (Rb + (beta + 1) * Re);// base current
   double Ie = (beta + 1) * Ib; // emitter current
   double re = 0.026 / Ie; // re factor
   double Zb1 = (beta + 1) * Re;
   double Zb2 = 1 + (Re / ro);
   double Zb = (beta * re) + (Zb1 / Zb2);
   double Zi = 1 / (1 /Rb + 1 /Zb); // input impedance
   double Zo1 = (beta * re) / (beta + 1);
   double Zo = 1 / (1 /ro + 1 /Re + 1 /Zo1); // output impedance
   double Av1 = (beta + 1) * Re / Zb;
   double Av = Av1 / (1 + (Re/ro)); // voltage gain

   // Save and display the ac results of transistor.
   save_ac_results(re, Zi, Zo, Av, "In phase");
}


/* DC analysis of common-base transisfor configuration. 

''Vcc', 'Vee', 'Rc' and 'Re' parameters stand for collector voltage, 
emitter voltage, collector resistance and emitter resistance. All 
resistor values must be in form of 'ohm'. 'beta' parameter  is the 
ratio of the transistor's collector current to its base current.
For example:

double Vcc=10, Vee=4, Rc=2400, Re=1200, beta=60;
dc_common_base(Vcc, Vee, Rc, Re, beta);
display_dc_results();

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
void dc_common_base(double Vcc, double Vee, double Rc, double Re,
                    double beta) {
   // Check if the parameters of transistor are consistent.
   assert (Rc > 0 && Re > 0 && beta > 0);

   // Calculate the all analyzes of transistor.
   double Ie = (Vee - Vbe) / Re; // emitter current
   double Ib = Ie / (beta + 1); // base current
   double Ic = Ib * beta; // collector current
   double Vce = Vee + Vcc - Ie * (Rc + Re); // collector-emitter v.
   double Vcb = Vcc - Ic * Rc; // collector-base voltage
   double Vbc = -1 * Vcb; // base-collector voltage

   // Save and display the results of transistor.
   save_dc_results(Ib, Ic, Ie, -1.0, Vce, -1.0, -1.0, -1.0, Vbc);
}

/* AC analysis of common-base transistor configuration. 

'Vcc', 'Vee', 'Rc' and 'Re' parameters stand for collector voltage, 
emitter voltage, collector resistance and emitter resistance. All 
resistor values must be in form of 'ohm'. 'alpha' parameter is the 
current gain of common-base configuration. For example:

double Vcc=8, Vee=2, Rc=5000, Re=1000, alpha=0.98;
ac_common_base(Vcc, Vee, Rc, Re, alpha);
display_ac_results();

re: 20.000000 ohm
Zi: 19.607843 ohm
Zo: 5000.000000 ohm
Av: 245.000000
phase: In phase
*/
void ac_common_base(double Vcc, double Vee, double Rc, double Re, 
                    double alpha) {
   // Check if the parameters of transistor are consistent.
   assert (Rc > 0 && Re > 0 && alpha > 0);

   // Calculate the all analyzes of transistor.
   double Ie = (Vee - Vbe) / Re; // emitter current
   double re = 0.026 / Ie; // re factor
   double Zi = 1 / (1/Re + 1/re); // input impedance
   double Zo = Rc; // output impedance
   double Av = alpha * Rc / re; // voltage gain

   // Save and display the results of transistor.
   save_ac_results(re, Zi, Zo, Av, "In phase");
}

/* DC analysis of miscellaneous-bias transisfor configuration. 

'Vcc', 'Rb', 'Rc' parameters stnd for collector voltage, base
resistance and collector resistance in order. All resistors must 
be in  form of 'ohm'. 'beta' parameter is the ratio of collector 
current to its base current. For example:

double Vcc=20, Rb=680000, Rc=4700, beta=120;
dc_miscellaneous_bias(Vcc, Rb, Rc, beta);
display_dc_results();

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
void dc_miscellaneous_bias(double Vcc, double Rb, double Rc, 
                           double beta) {
   // Check if the parameters of transistor are consistent.
   assert (Rb > 0 && Rc > 0 && beta > 0);

   // Calculate the all analyzes of transistor.
   double Ib = (Vcc - Vbe) / (Rb + beta * Rc); // base current
   double Ic = beta * Ib; // collector current
   double Ie = (beta + 1) * Ib; // emitter current
   double Vce = Vcc - (Ie * Rc); // collector-emitter voltage
   double Ve = 0; // emitter voltage
   double Vc = Vce + Ve; // collector voltage
   double Vb = Vbe + Ve; // base voltage
   double Vbc = Vb - Vc; // base-collector voltage

   // Save and display the results of transistor.
   save_dc_results(Ib, Ic, Ie, -1.0, Vce, Vc, Ve, Vb, Vbc);
}

/* Two Port System Analysis 

'Avnl', 'Zi' and 'Zo' parameters stand for non-load voltage gain, 
the input impedance and output impedance of your two port system 
in order. 'Rs' and 'Rl' parameters stand for source and load 
resistances. All resistors must be in form of 'ohm'. For example:

double Avnl=-480, Zi=4000, Zo=2000, Rs=200, Rl=5600;
two_port_system(Avnl, Zi, Zo, Rs, Rl);
display_two_port_results();

Avl: -353.684204    // load-voltage gain
Avs: -336.842102    // source-voltage gain
Ail: 252.631577     // load-current gain
*/
void two_port_system(double Avnl, double Zi, double Zo, double Rs, 
                     double Rl) {
   // Check if the parameters of two port system are consistent.
   assert (Zi > 0 && Zo > 0 && Rs > 0 && Rl > 0);

   // Calculate the all analyzes of two port system.
   double Avl = Rl / (Rl + Zo) * Avnl; // load-voltage gain
   double Avs = Zi / (Zi + Rs) * Avl; // source-voltage gain
   double Ail = -1 * Avl * Zi / Rl; // load-current gain

   // Save and display the results of two port system.
   save_two_port_results(Avl, Avs, Ail);
}

/* Cascaded System Analysis 

'num' stand for number of transistor in your cascaded system. 
'Avnls', 'Zis', 'Zos' stand for non-load voltage gains, input 
impedances and output impedendeces in order. These parameters must 
be given as list to algorithm. 'Rs' and 'Rl' mean source and load 
resistors. All resistors must be in form of 'ohm'. For example:

double Avnls[3] = {1, 250, 100}; // not-load voltage gains
double Zis[3] = {500, 26, 100}; // input impedances
double Zos[3] = {1000, 5100, 100}; // output impedances
double Rs = 10000, Rl = 820; // source and load resistances
cascaded_system(3, Avnls, Zis, Zos, Rs, Rl); 

Av1: 0.025341
Av2: 4.807692
Av3: 89.130435
Avt: 10.858971
Avs: 0.517094
Ait: -6.621324
*/
void cascaded_system(size_t num, double Avnls[num], double Zis[num], 
                     double Zos[num], double Rs, double Rl) {
   // Check if the parameters of cascaded system are consistent.
   for (int i=0; i<num; i++) 
      assert (Zis[i] > 0 && Zos[i] > 0 && Rs > 0 && Rl > 0);

   // Sum the voltage gains of each stage.
   double total_voltage = 1;
   // The voltage gain of each stage.
   double Av;
   // Calculate the all analyzes of cascaded system.
   for (int i = 0; i < num - 1; i++) {
      // Calculate the voltage gain in each stage and then
      // append it to total voltage gain.
      Av = Zis[i+1] / (Zis[i+1] + Zos[i]) * Avnls[i];
      total_voltage *= Av;
      // Right know, create the stage voltage gain name.
      char iname[64]; char vname[64] = "Av";
      sprintf(iname, "%d", (i + 1)); strcat(vname, iname);
      // Append the stage voltage names onto related struct.
      CascadedAnalysis.stage_names[i] = vname;
      // strcpy(CascadedAnalysis.stage_names[i], vname);
      CascadedAnalysis.stage_volts[i] = Av;

      printf("%s: %f\n", CascadedAnalysis.stage_names[i], 
                         CascadedAnalysis.stage_volts[i]);
   }
   // When calculating the analysis of last stage, process
   // changes a bit. So let's make a special analysis. 
   Av = Rl / (Rl + Zos[num-1]) * Avnls[num-1];
   total_voltage *= Av;
   // Right know, create the last stage voltage name.
   char iname[64]; char vname[64] = "Av";
   sprintf(iname, "%ld", num); strcat(vname, iname);
   // Append the rest calculations of cascaded system.
   CascadedAnalysis.stage_names[num-1] = vname;
   CascadedAnalysis.stage_volts[num-1] = Av;
   CascadedAnalysis.Avt = total_voltage;
   CascadedAnalysis.Avs = Zis[0] / (Zis[0]+Rs) * total_voltage;
   CascadedAnalysis.Ait = -1 * total_voltage * Zis[0] / Rl;

   printf("%s: %f\n", CascadedAnalysis.stage_names[num-1],
                      CascadedAnalysis.stage_volts[num-1]);
   printf("Avt: %f\n", CascadedAnalysis.Avt);
   printf("Avs: %f\n", CascadedAnalysis.Avs);
   printf("Ait: %f\n", CascadedAnalysis.Ait);
}
